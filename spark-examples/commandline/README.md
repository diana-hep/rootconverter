java -jar ../../scaroot-reader/target/scaroot-reader-0.1-scala_2.10-root_6.06.jar ../../root2avro/test_Event/Event.root T --libs=../../root2avro/test_Event/Event_cxx.so --name=Tree > myclasses.scala

sbt console

import data.root._

val iterator = RootTreeIterator[Tree](List("../../root2avro/test_Event/Event.root"), "T", List("../../root2avro/test_Event/Event_cxx.so"), myclasses)

while (iterator.hasNext)
  println(iterator.next())
