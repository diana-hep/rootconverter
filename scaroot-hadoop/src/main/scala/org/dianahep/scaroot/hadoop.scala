package org.dianahep.scaroot

import org.apache.hadoop.fs.FileStatus
import org.apache.hadoop.fs.FileSystem
import org.apache.hadoop.fs.Path
import org.apache.hadoop.io.compress.CompressionCodecFactory
import org.apache.hadoop.io.LongWritable
import org.apache.hadoop.io.Text
import org.apache.hadoop.io.Writable
import org.apache.hadoop.mapreduce.InputSplit
import org.apache.hadoop.mapreduce.JobContext
import org.apache.hadoop.mapreduce.JobStatus
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat
import org.apache.hadoop.mapreduce.lib.input.FileSplit
import org.apache.hadoop.mapreduce.Mapper
import org.apache.hadoop.mapreduce.RecordReader
import org.apache.hadoop.mapreduce.TaskAttemptContext

import org.dianahep.scaroot.reader._

package hadoop {
  // class RootFileSplit extends InputSplit {
  //   def getLength(): Long = ...
  //   def getLocations(): Array[String] = ...
  // }

  // class RootTreeInputFormat[TYPE <: Writable] {
  //   override def createRecordReader(split: InputSplit, context: TaskAttemptContext): RecordReader[LongWritable, TYPE] =
  //     new RootTreeRecordReader[TYPE]

  //   override def isSplitable(context: JobContext, file: Path): Boolean = false

  //   override def getSplits(job: JobContext): java.util.List[InputSplit] = {





  //   }
  // }

  // class RootTreeRecordReader[TYPE <: Writable] {
  //   private var iterator: RootTreeIterator[TYPE] = null
  //   private var key: LongWritable = null
  //   private var value: TYPE

  //   override def initialize(split: InputSplit, context: TaskAttemptContext) = split match {
  //     case fileSplit: FileSplit =>
  //       val job = context.getConfiguration
  //       val path = fileSplit.getPath
  //       val fileSystem = FileSystem.get(job)
  //       val localFileSystem = FileSystem.getLocal(job)

  //       val 
  //       if (path.getFileSystem(job) == localFileSystem)




  //   }


  // }


}
