package org.dianahep.scaroot

import scala.collection.immutable.SortedSet
import scala.collection.mutable
import scala.collection.JavaConversions._

import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.fs.FileSystem
import org.apache.hadoop.fs.Path
import org.apache.hadoop.io.Writable
import org.apache.hadoop.io.WritableComparable
import org.apache.hadoop.mapreduce.InputSplit
import org.apache.hadoop.mapreduce.JobContext
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat
import org.apache.hadoop.mapreduce.lib.input.FileSplit
import org.apache.hadoop.mapreduce.RecordReader
import org.apache.hadoop.mapreduce.TaskAttemptContext

import org.dianahep.scaroot.reader._

package hadoop {
  case class KeyWritable(var fileIndex: Int, var treeEntry: Long) extends Serializable with WritableComparable[KeyWritable] {
    def this() { this(-1, -1L) }
    def fileLocation(configuration: Configuration) = configuration.getStrings("scaroot.reader.fileLocations")(fileIndex)
    def write(out: java.io.DataOutput) {
      out.writeInt(fileIndex)
      out.writeLong(treeEntry)
    }
    def readFields(in: java.io.DataInput) {
      fileIndex = in.readInt()
      treeEntry = in.readLong()
    }
    def compareTo(that: KeyWritable) =
      if (this.fileIndex < that.fileIndex)
        -1
      else if (this.fileIndex > that.fileIndex)
        1
      else
        this.treeEntry compare that.treeEntry
  }

  case class XRootDFilePiece(url: String, fileIndex: Int, size: Long, num: Int, of: Int) {
    def fractionalSize = Math.round(size.toDouble / of)   // approximate! some round up, some round down
  }

  case class XRootDFileSplit(pieces: List[XRootDFilePiece]) extends InputSplit {
    def getLength(): Long = pieces.map(_.fractionalSize).sum
    def getLocations(): Array[String] = Array[String]()
  }

  // Tells Hadoop how to split up files.
  class RootTreeInputFormat[TYPE <: Writable] extends FileInputFormat[KeyWritable, TYPE] {
    System.err.println("RootTreeInputFormat constructor")

    override def createRecordReader(split: InputSplit, context: TaskAttemptContext): RecordReader[KeyWritable, TYPE] = {
      System.err.println("RootTreeInputFormat createRecordReader")
      new RootTreeRecordReader[TYPE]
    }

    override def getSplits(jobContext: JobContext): java.util.List[InputSplit] = {
      System.err.println("RootTreeInputFormat getSplits")

      val configuration = jobContext.getConfiguration
      val inputPath = configuration.get("mapreduce.input.fileinputformat.inputdir", configuration.get("mapred.input.dir", ""))
      val maxSize = configuration.getLong("mapreduce.input.fileinputformat.split.maxsize", java.lang.Long.MAX_VALUE)  // By default, only one split.

      System.err.println(s"inputPath $inputPath maxSize $maxSize (${maxSize == java.lang.Long.MAX_VALUE})")

      // We handle the case of XRootD, which has no locality (all files are remote, just distribute work evenly).
      if (inputPath startsWith "root://") {
        val files: SortedSet[XRootD.File] = XRootD(inputPath)
        if (files.isEmpty)
          throw new IllegalArgumentException(s"""cannot find any XRootD files that match "$inputPath"""")

        configuration.setStrings("scaroot.reader.fileLocations", files.map(_.url).toList: _*)
        val fileToIndex = files.map(_.url).toList.zipWithIndex.toMap
        
        // Working list of splits.
        val splits = mutable.ListBuffer[mutable.ListBuffer[XRootDFilePiece]]()

        // Add pieces to the splits to try to balance the load: biggest files first and try to fill in the cracks before making new splits.
        def add(newPiece: XRootDFilePiece) {
          splits.find(pieces => pieces.map(_.fractionalSize).sum + newPiece.fractionalSize < maxSize) match {
            case Some(pieces) => pieces += newPiece
            case None => splits += mutable.ListBuffer[XRootDFilePiece](newPiece)
          }
        }

        // For each of the files, break it into (evenly sized) pieces if you have to and distribute it according to the above algorithm.
        files foreach {file =>
          if (file.size > maxSize) {
            val numPieces = Math.ceil(file.size.toDouble / maxSize).toInt
            val pieces = 0 until numPieces map {i => XRootDFilePiece(file.url, fileToIndex(file.url), file.size, i, numPieces)}
            pieces foreach {piece => add(piece)}
          }
          else
            add(XRootDFilePiece(file.url, fileToIndex(file.url), file.size, 0, 1))
        }

        splits.map(x => new XRootDFileSplit(x.toList)).toList
      }
      // They handle all other cases: HDFS and local filesystem.
      else {
        System.err.println("else clause 1")
        configuration.setStrings("scaroot.reader.fileLocations", listStatus(jobContext).map(_.getPath.toString): _*)
        System.err.println("else clause 2")
        val out = super.getSplits(jobContext)
        System.err.println("else clause 3")
        out
      }
    }

    // In the default algorithm (especially important for HDFS), files are not splitable.
    override def isSplitable(jobContext: JobContext, file: Path): Boolean = false
  }

  // Tells Hadoop how to interpret files as records.
  class RootTreeRecordReader[TYPE <: Writable] extends RecordReader[KeyWritable, TYPE] {
    System.err.println("RootTreeRecordReader constructor")

    private var fileToIndex: Map[String, Int] = null
    private var treeLocation: String = null
    private var libs: List[String] = Nil
    private var myclasses: Map[String, My[_]] = null
    private var microBatchSize: Int = -1

    private var pieces: List[XRootDFilePiece] = Nil
    private var iterator: RootTreeReloadableIterator[TYPE] = null
    private var fileIndex: Int = -1
    private var progressCurrentSize: Long = -1L
    private var progressNumer: Float = 0.0F
    private var progressDenom: Float = 0.0F
    private var key: KeyWritable = null
    private var value: TYPE = null.asInstanceOf[TYPE]

    override def initialize(split: InputSplit, context: TaskAttemptContext) = {
      System.err.println("RootTreeRecordReader initialize")

      val configuration = context.getConfiguration

      treeLocation = configuration.get("scaroot.reader.treeLocation")
      libs = Option(configuration.getStrings("scaroot.reader.libs")).toList.flatten
      val classNames = Option(configuration.getStrings("scaroot.reader.classNames")).toList.flatten
      myclasses = classNames.map(name => (name, configuration.getClass("scaroot.reader.myclasses." + name, null, classOf[My[_]]).getConstructor().newInstance())).toMap
      microBatchSize = configuration.getInt("scaroot.reader.microBatchSize", 10)

      System.err.println(s"treeLocation $treeLocation libs $libs myclasses $myclasses microBatchSize $microBatchSize")
      System.err.println(s"""fileLocations ${configuration.getStrings("scaroot.reader.fileLocations")}""")

      split match {
        // HDFS and local filesystem cases.
        case fileSplit: FileSplit =>
          val path = fileSplit.getPath
          val fileSystem = FileSystem.get(configuration)
          val localFileSystem = FileSystem.getLocal(configuration)

          val localFile =
            if (path.getFileSystem(configuration) == localFileSystem) {
              System.err.println(s"on local filesystem $path")
              localFileSystem.pathToFile(path)
            }
            else {
              System.err.println(s"on HDFS $path")
              // When the ROOT file is in HDFS, there's no better way to run over it than to copy it locally first.
              // HTTP access (not enabled for all HDFS clusters) is not seekable, and the ROOT file format depends
              // heavily on seeking (it's more of an object database than a file format).
              val name = fileSplit.getPath.getName
              val cwd = localFileSystem.getWorkingDirectory
              fileSystem.copyToLocalFile(false, path, cwd)

              System.err.println(s"copied: $name $cwd")

              localFileSystem.pathToFile(new Path(cwd, name))
            }

          val fileIndex = Option(configuration.getStrings("scaroot.reader.fileLocations")).toSeq.flatten.indexOf(path)

          this.pieces = List(XRootDFilePiece("file://" + localFile.getAbsolutePath, fileIndex, fileSplit.getLength, 0, 1))

        case XRootDFileSplit(pieces) =>
          // XRootD is a remote file protocol that works well with ROOT, so we can let it do its thing.
          this.pieces = pieces
      }

      System.err.println(s"""mapred.map.child.env ${configuration.get("mapred.map.child.env")}""")
      System.err.println(s"""LD_LIBRARY_PATH ${System.getenv()("LD_LIBRARY_PATH")}""")

      progressDenom = this.pieces.map(_.fractionalSize).sum

      val first :: rest = this.pieces
      this.pieces = rest
      iterator = RootTreeReloadableIterator[TYPE](first.url, first.num, first.of, treeLocation, libs, myclasses, microBatchSize)
      fileIndex = first.fileIndex
      progressCurrentSize = first.fractionalSize
    }

    override def nextKeyValue(): Boolean = (iterator.hasNext, pieces) match {
      case (false, Nil) =>
        false

      case (false, next :: rest) =>
        this.pieces = rest
        iterator.reload(next.url, next.num, next.of)
        fileIndex = next.fileIndex
        progressCurrentSize = next.fractionalSize

        if (iterator.hasNext) {
          key = KeyWritable(fileIndex, iterator.index)
          value = iterator.next()
          true
        }
        else
          false

      case (true, _) =>
        key = KeyWritable(fileIndex, iterator.index)
        value = iterator.next()
        true
    }

    override def getCurrentKey() = key
    override def getCurrentValue() = value
    override def getProgress() = (progressCurrentSize * iterator.progress.toFloat) / progressDenom
    override def close() { }
  }
}
