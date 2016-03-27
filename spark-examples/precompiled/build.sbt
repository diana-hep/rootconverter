name := "SparkProject"
version := "0.1"
scalaVersion := "2.10.5"

resolvers += "Local Maven" at Path.userHome.asFile.toURI.toURL + ".m2/repository"

libraryDependencies += "org.apache.spark" %% "spark-core" % "1.6.1" % "provided"
libraryDependencies += "org.diana-hep" % "scaroot-reader" % "0.1" classifier "scala_2.10-root_6.06"
