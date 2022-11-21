package tram.ir
// Architecture Description IR

import scala.collection.mutable


// Architecture Specification, abstract basic class
trait IR{
	val attributes = mutable.Map[String, Any]()

	def getAttrs = attributes
	def getKeys = attributes.keys

	def getAttrValue(key: String): Any = {
		if(attributes.contains(key))
			attributes(key)
		else
			None
	}

	def getAttrValue(keys: Seq[String]): Seq[Any] = {
		for(key <- keys) yield
			getAttrValue(key)
	}


	def apply(key: String, value: Any): IR = {
    if(attributes.contains(key)){
      attributes(key) = value
    }else{
      attributes += key -> value
    }
    this
  }

  def apply(attrs: mutable.Map[String, Any]): IR = {
  	attrs.foreach{attr => apply(attr._1, attr._2)}
    this
  }

  def addAttr(key: String, value: Any): Unit = {
  	apply(key, value)
  }

  def delAttr(keys: String*): Unit = {
  	for(key <- keys)
  		attributes -= key
  }


  def printIR(filename: String): Unit = {
  	IRHandler.dumpIR(attributes, filename)
  }

}



//object IRTest extends App {
//  val inFilename = "src/main/resources/my_cgra_test_w.json"
//  val jsonMap = IRHandler.loadIR(inFilename)
//
//  class ADGNode extends IR
//  val node =  new ADGNode
//  node(jsonMap)
//  val outFilename = "src/main/resources/my_cgra_test_ww.json"
//  node.printIR(outFilename)
//}