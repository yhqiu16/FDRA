package tram.dsa

import chisel3._
import chisel3.util._
import scala.collection.mutable
import scala.collection.mutable.{ListBuffer, ArrayBuffer}
import tram.common.MacroVar._
import tram.ir._


object GIB{
  def Mod(x: Int, y: Int): Int = java.lang.Math.floorMod(x, y)
  // connection rule: Map[(src, dst), (src_idx, N) => dst_idx]
  val rule: Map[(Int, Int), Seq[Int] => Int] = Map(
    (WEST, EAST)   -> ((x: Seq[Int]) => Mod(x(0), x(1))),    // i%N
    (EAST, WEST)   -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (NORTH, SOUTH) -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (SOUTH, NORTH) -> ((x: Seq[Int]) => Mod(x(0), x(1))), 
    (WEST, NORTH)  -> ((x: Seq[Int]) => Mod(-x(0), x(1))),   //(N-i)%N
    (NORTH, WEST)  -> ((x: Seq[Int]) => Mod(-x(0), x(1))),
    (EAST, SOUTH)  -> ((x: Seq[Int]) => Mod(-x(0)-2, x(1))), // (2*N-2-i)%N
    (SOUTH, EAST)  -> ((x: Seq[Int]) => Mod(-x(0)-2, x(1))),
    (NORTH, EAST)  -> ((x: Seq[Int]) => Mod(x(0)+1, x(1))),  // (i+1)%N
    (EAST, NORTH)  -> ((x: Seq[Int]) => Mod(x(0)-1, x(1))),  // (N+i-1)%N
    (SOUTH, WEST)  -> ((x: Seq[Int]) => Mod(x(0)+1, x(1))),
    (WEST, SOUTH)  -> ((x: Seq[Int]) => Mod(x(0)-1, x(1))),
    (NORTHWEST, EAST)  -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (NORTHWEST, NORTH) -> ((x: Seq[Int]) => Mod(-x(0), x(1))),
    (NORTHWEST, SOUTH) -> ((x: Seq[Int]) => Mod(x(0)-1, x(1))),
    (SOUTHWEST, NORTH) -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (SOUTHWEST, WEST)  -> ((x: Seq[Int]) => Mod(x(0)+1, x(1))),
    (SOUTHWEST, EAST)  -> ((x: Seq[Int]) => Mod(-x(0)-2, x(1))),
    (SOUTHEAST, WEST)  -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (SOUTHEAST, SOUTH) -> ((x: Seq[Int]) => Mod(-x(0)-2, x(1))),
    (SOUTHEAST, NORTH) -> ((x: Seq[Int]) => Mod(x(0)-1, x(1))),
    (NORTHEAST, SOUTH) -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (NORTHEAST, EAST)  -> ((x: Seq[Int]) => Mod(x(0)+1, x(1))),
    (NORTHEAST, WEST)  -> ((x: Seq[Int]) => Mod(-x(0), x(1))),
    (NORTHWEST, SOUTHEAST) -> ((x: Seq[Int]) => Mod(x(0), x(1))),    // i%N
    (SOUTHEAST, NORTHWEST) -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (NORTHEAST, SOUTHWEST) -> ((x: Seq[Int]) => Mod(x(0), x(1))),
    (SOUTHWEST, NORTHEAST) -> ((x: Seq[Int]) => Mod(x(0), x(1))), 
    (NORTHWEST, NORTHEAST) -> ((x: Seq[Int]) => Mod(-x(0), x(1))),   //(N-i)%N
    (NORTHEAST, NORTHWEST) -> ((x: Seq[Int]) => Mod(-x(0), x(1))),
    (SOUTHEAST, SOUTHWEST) -> ((x: Seq[Int]) => Mod(-x(0)-2, x(1))), // (2*N-2-i)%N
    (SOUTHWEST, SOUTHEAST) -> ((x: Seq[Int]) => Mod(-x(0)-2, x(1))),
    (NORTHEAST, SOUTHEAST) -> ((x: Seq[Int]) => Mod(x(0)+1, x(1))),  // (i+1)%N
    (SOUTHEAST, NORTHEAST) -> ((x: Seq[Int]) => Mod(x(0)-1, x(1))),  // (N+i-1)%N
    (SOUTHWEST, NORTHWEST) -> ((x: Seq[Int]) => Mod(x(0)+1, x(1))),
    (NORTHWEST, SOUTHWEST) -> ((x: Seq[Int]) => Mod(x(0)-1, x(1)))
  )
  /** Track to track connection, the same as wilton SB connection
   * 
   * @param numTrack  track number
   * @param trackDirections  which directions have valid track IO ports
   */ 
  def getTrack2TrackConnect(numTrack: Int, trackDirections : ListBuffer[Int]): ListBuffer[Seq[Int]] = {
    val w = numTrack
    var connect = ListBuffer[Seq[Int]]() // (srcSide, srcTrack, dstSide, dstTrack)
    for(i <- 0 until w){
      connect += Seq(WEST, i, EAST, rule((WEST,EAST))(Seq(i, w)))       
      connect += Seq(NORTH, i, SOUTH, rule((NORTH,SOUTH))(Seq(i, w)))        
      connect += Seq(WEST, i, SOUTH, rule((WEST,SOUTH))(Seq(i, w)))       
      connect += Seq(SOUTH, i, EAST, rule((SOUTH,EAST))(Seq(i, w)))       
      connect += Seq(EAST, i, NORTH, rule((EAST,NORTH))(Seq(i, w)))       
      connect += Seq(NORTH, i, WEST, rule((NORTH,WEST))(Seq(i, w))) 
      connect += Seq(EAST, rule((WEST,EAST))(Seq(i, w)), WEST, i) 
      connect += Seq(SOUTH, rule((NORTH,SOUTH))(Seq(i, w)), NORTH, i)
      connect += Seq(SOUTH, rule((WEST,SOUTH))(Seq(i, w)), WEST, i) 
      connect += Seq(EAST, rule((SOUTH,EAST))(Seq(i, w)), SOUTH, i)  
      connect += Seq(NORTH, rule((EAST,NORTH))(Seq(i, w)), EAST, i)
      connect += Seq(WEST, rule((NORTH,WEST))(Seq(i, w)), NORTH, i) 
    }
//    connect
    connect.distinct.filter(ins => {
      (trackDirections.contains(ins(0)) && trackDirections.contains(ins(2)))
    } )
  }


  /** PE output pins to tracks connection, 
   *  connect an opin in one side to the tracks on the opposite and right sides
   * 
   * @param numTrack    track number
   * @param fc          connection flexibility of the output pins, 2n
   * @param numPinList  number of the PE Output Pins on four sides
   * @param trackDirections  which directions have valid track IO ports
   */
  def getOPin2TrackConnect(numTrack: Int, fc: Int, numPinList: List[Int], trackDirections : ListBuffer[Int]): ListBuffer[Seq[Int]] = {
    // numPinList
    val numNWpin = numPinList(0)  // NORTHWEST side 
    val numNEpin = numPinList(1)  // NORTHEAST side
    val numSEpin = numPinList(2)  // SOUTHEAST side 
    val numSWpin = numPinList(3)  // SOUTHWEST side

    val w = numTrack
    val fcStep : Float = 2*w.toFloat/fc
    val connect = ListBuffer[Seq[Int]]() // (srcSide, srcTrack, dstSide, dstTrack)
    var pinStep : Float = {
      if(numNWpin > 0) fcStep/numNWpin else 0
    }
    for(i <- 0 until numNWpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(NORTHWEST, i, EAST, rule((NORTHWEST,EAST))(Seq(index, w))) 
        connect += Seq(NORTHWEST, i, SOUTH, rule((NORTHWEST,SOUTH))(Seq(index, w))) 
      }      
    }
    if(numNEpin > 0){
      pinStep = fcStep/numNEpin
    }
    for(i <- 0 until numNEpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(NORTHEAST, i, SOUTH, rule((NORTHEAST,SOUTH))(Seq(index, w))) 
        connect += Seq(NORTHEAST, i, WEST, rule((NORTHEAST,WEST))(Seq(index, w))) 
      }      
    }
    if(numSEpin > 0){
      pinStep = fcStep/numSEpin
    }
    for(i <- 0 until numSEpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(SOUTHEAST, i, WEST, rule((SOUTHEAST,WEST))(Seq(index, w))) 
        connect += Seq(SOUTHEAST, i, NORTH, rule((SOUTHEAST,NORTH))(Seq(index, w))) 
      }      
    }
    if(numSWpin > 0){
      pinStep = fcStep/numSWpin
    }
    for(i <- 0 until numSWpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(SOUTHWEST, i, NORTH, rule((SOUTHWEST,NORTH))(Seq(index, w))) 
        connect += Seq(SOUTHWEST, i, EAST, rule((SOUTHWEST,EAST))(Seq(index, w))) 
      }      
    }
//    connect
    val trackAllDirs = List(EAST, WEST, SOUTH, NORTH)
    connect.distinct.filter(ins =>
      ! ((trackAllDirs.contains(ins(0)) && !trackDirections.contains(ins(0))) ||
        (trackAllDirs.contains(ins(2)) && !trackDirections.contains(ins(2))))
    )
  }



  /** tracks to PE input pins connection, 
   *  connect an ipin in one side to the tracks on the opposite and right sides
   * 
   * @param numTrack    track number
   * @param fc          connection flexibility of the input pins, 2n
   * @param numPinList  number of the PE Input Pins on four sides
   * @param trackDirections  which directions have valid track IO ports
   */
  def getTrack2IPinConnect(numTrack: Int, fc: Int, numPinList: List[Int], trackDirections : ListBuffer[Int]): ListBuffer[Seq[Int]] = {
    // numPinList
    val numNWpin = numPinList(0)  // NORTHWEST side
    val numNEpin = numPinList(1)  // NORTHEAST side
    val numSEpin = numPinList(2)  // SOUTHEAST side
    val numSWpin = numPinList(3)  // SOUTHWEST side

    val w = numTrack
    val fcStep: Float = 2*w.toFloat/fc
    val connect = ListBuffer[Seq[Int]]() // (srcSide, srcTrack, dstSide, dstTrack)
    var pinStep: Float = {
      if(numNWpin > 0) fcStep/numNWpin else 0
    }
    for(i <- 0 until numNWpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(EAST, rule((NORTHWEST,EAST))(Seq(index, w)), NORTHWEST, i) 
        connect += Seq(SOUTH, rule((NORTHWEST,SOUTH))(Seq(index, w)), NORTHWEST, i) 
      }      
    }
    if(numNEpin > 0){
      pinStep = fcStep/numNEpin
    }
    for(i <- 0 until numNEpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(SOUTH, rule((NORTHEAST,SOUTH))(Seq(index, w)), NORTHEAST, i) 
        connect += Seq(WEST, rule((NORTHEAST,WEST))(Seq(index, w)), NORTHEAST, i) 
      }      
    }
    if(numSEpin > 0){
      pinStep = fcStep/numSEpin
    }
    for(i <- 0 until numSEpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(WEST, rule((SOUTHEAST,WEST))(Seq(index, w)), SOUTHEAST, i) 
        connect += Seq(NORTH, rule((SOUTHEAST,NORTH))(Seq(index, w)), SOUTHEAST, i) 
      }      
    }
    if(numSWpin > 0){
      pinStep = fcStep/numSWpin
    }
    for(i <- 0 until numSWpin){
      for(j <- 0 until fc/2) { // connect the opposite and right sides
        val index = (i*pinStep + j*fcStep).toInt
        connect += Seq(NORTH, rule((SOUTHWEST,NORTH))(Seq(index, w)), SOUTHWEST, i) 
        connect += Seq(EAST, rule((SOUTHWEST,EAST))(Seq(index, w)), SOUTHWEST, i) 
      }      
    }
//    connect
    val trackAllDirs = List(EAST, WEST, SOUTH, NORTH)
    connect.distinct.filter(ins =>
      ! ((trackAllDirs.contains(ins(0)) && !trackDirections.contains(ins(0))) ||
        (trackAllDirs.contains(ins(2)) && !trackDirections.contains(ins(2))))
    )
  }



  /** PE output pins to PE input pins connection, 
   *  connect an opin in one side to the ipins on the other one or two sides
   * 
   * @param fc      connection flexibility of the output pins, 1n or 2n
   * @param diag    if support diagonal connections
   * @param numIOPinList  number of the PE I/O Pins on four sides
   */
  def getOPin2IPinConnect(fc: Int, diag: Boolean, numIOPinList: List[Int]): ListBuffer[Seq[Int]] = {
    // numIOPinList
    val nNWi = numIOPinList(0)  // number of the PE input pins on the NORTHWEST side of the GIB  
    val nNWo = numIOPinList(1)  // number of the PE output pins on the NORTHWEST side of the GIB
    val nNEi = numIOPinList(2)  // number of the PE input pins on the NORTHEAST side of the GIB
    val nNEo = numIOPinList(3)  // number of the PE output pins on the NORTHEAST side of the GIB
    val nSEi = numIOPinList(4)  // number of the PE input pins on the SOUTHEAST side of the GIB
    val nSEo = numIOPinList(5)  // number of the PE output pins on the SOUTHEAST side of the GIB
    val nSWi = numIOPinList(6)  // number of the PE input pins on the SOUTHWEST side of the GIB
    val nSWo = numIOPinList(7)  // number of the PE output pins on the SOUTHWEST side of the GIB
    val fcPerSide = { if(diag) fc/2 else fc }
    val connect = ListBuffer[Seq[Int]]() // (srcSide, srcTrack, dstSide, dstTrack)
    val fcNWstep = nNWi/fcPerSide.toFloat
    val fcNEstep = nNEi/fcPerSide.toFloat
    val fcSEstep = nSEi/fcPerSide.toFloat
    val fcSWstep = nSWi/fcPerSide.toFloat
    for(i <- 0 until nNWo){
      for(j <- 0 until fcPerSide) { // connect to the other sides
        if(nSEi > 0 && diag){
          val iSE = (i*fcSEstep/nNWo + j*fcSEstep).toInt
          connect += Seq(NORTHWEST, i, SOUTHEAST, rule((NORTHWEST,SOUTHEAST))(Seq(iSE, nSEi))) 
        }
//        if(nSWi > 0){
//          val iSW = (i*fcSWstep/nNWo + j*fcSWstep).toInt
//          connect += Seq(NORTHWEST, i, SOUTHWEST, rule((NORTHWEST,SOUTHWEST))(Seq(iSW, nSWi)))
//        }
        if(nNEi > 0){
          val iNE = (i*fcNEstep/nNWo + j*fcNEstep).toInt
          connect += Seq(NORTHWEST, i, NORTHEAST, rule((NORTHWEST,NORTHEAST))(Seq(iNE, nNEi)))
        }
      }      
    }
    for(i <- 0 until nNEo){
      for(j <- 0 until fcPerSide) { // connect to the other sides
        if(nSEi > 0){
          val iSE = (i*fcSEstep/nNEo + j*fcSEstep).toInt
          connect += Seq(NORTHEAST, i, SOUTHEAST, rule((NORTHEAST,SOUTHEAST))(Seq(iSE, nSEi))) 
        }
        if(nSWi > 0  && diag){
          val iSW = (i*fcSWstep/nNEo + j*fcSWstep).toInt
          connect += Seq(NORTHEAST, i, SOUTHWEST, rule((NORTHEAST,SOUTHWEST))(Seq(iSW, nSWi)))
        }
//        if(nNWi > 0){
//          val iNW = (i*fcNWstep/nNEo + j*fcNWstep).toInt
//          connect += Seq(NORTHEAST, i, NORTHWEST, rule((NORTHEAST,NORTHWEST))(Seq(iNW, nNWi)))
//        }
      }      
    }
    for(i <- 0 until nSEo){
      for(j <- 0 until fcPerSide) { // connect to the other sides
        if(nSWi > 0){
          val iSW = (i*fcSWstep/nSEo + j*fcSWstep).toInt
          connect += Seq(SOUTHEAST, i, SOUTHWEST, rule((SOUTHEAST,SOUTHWEST))(Seq(iSW, nSWi)))
        }
        if(nNWi > 0  && diag){
          val iNW = (i*fcNWstep/nSEo + j*fcNWstep).toInt
          connect += Seq(SOUTHEAST, i, NORTHWEST, rule((SOUTHEAST,NORTHWEST))(Seq(iNW, nNWi)))
        }
//        if(nNEi > 0){
//          val iNE = (i*fcNEstep/nSEo + j*fcNEstep).toInt
//          connect += Seq(SOUTHEAST, i, NORTHEAST, rule((SOUTHEAST,NORTHEAST))(Seq(iNE, nNEi)))
//        }
      }      
    }
    for(i <- 0 until nSWo){
      for(j <- 0 until fcPerSide) { // connect to the other sides
//        if(nSEi > 0){
//          val iSE = (i*fcSEstep/nSWo + j*fcSEstep).toInt
//          connect += Seq(SOUTHWEST, i, SOUTHEAST, rule((SOUTHWEST,SOUTHEAST))(Seq(iSE, nSEi)))
//        }
        if(nNWi > 0){
          val iNW = (i*fcNWstep/nSWo + j*fcNWstep).toInt
          connect += Seq(SOUTHWEST, i, NORTHWEST, rule((SOUTHWEST,NORTHWEST))(Seq(iNW, nNWi)))
        }
        if(nNEi > 0  && diag){
          val iNE = (i*fcNEstep/nSWo + j*fcNEstep).toInt
          connect += Seq(SOUTHWEST, i, NORTHEAST, rule((SOUTHWEST,NORTHEAST))(Seq(iNE, nNEi))) 
        }
      }      
    }
    connect.distinct
  }

  /** GIB internal connection,
   *  oPin2IPin, track2IPin, oPin2Track, track2Track
   *
   * @param numTrack      track number
   * @param diagPinConect if support diagonal connections between OPins and IPins
   * @param numIOPinList  number of the PE I/O Pins on four sides
   * @param fcList        connect flexibility
   * @param trackDirections  which directions have valid track IO ports
   */
  def getGIBConnect(numTrack: Int, diagPinConect: Boolean, numIOPinList: List[Int], fcList: List[Int], trackDirections : ListBuffer[Int]): ListBuffer[(Seq[Int], ListBuffer[Seq[Int]])] = {
    var dstList = ListBuffer[Seq[Int]]()
    var srcsList = ListBuffer[ListBuffer[Seq[Int]]]() // srcs connected to each dst
    val oPin2IPinConnect = getOPin2IPinConnect(fcList(2), diagPinConect, numIOPinList)
    oPin2IPinConnect.map{ dWire =>
      val dst = Seq(TYPE_IPIN, dWire(2), dWire(3)) // type, direction, index
      if(!dstList.contains(dst)){
        dstList += dst
      }
      val index = dstList.indexOf(dst)
      if(srcsList.size <= index){
        srcsList += ListBuffer(Seq(TYPE_OPIN, dWire(0), dWire(1)))
      } else{
        srcsList(index) += Seq(TYPE_OPIN, dWire(0), dWire(1))
      }      
    }
    if(numTrack > 0) {
      val track2IPinConnect = getTrack2IPinConnect(numTrack, fcList(0), List(numIOPinList(0), numIOPinList(2), numIOPinList(4), numIOPinList(6)), trackDirections)
      track2IPinConnect.map { dWire =>
        val dst = Seq(TYPE_IPIN, dWire(2), dWire(3)) // type, direction, index
        if (!dstList.contains(dst)) {
          dstList += dst
        }
        val index = dstList.indexOf(dst)
        if (srcsList.size <= index) {
          srcsList += ListBuffer(Seq(TYPE_ITRACK, dWire(0), dWire(1)))
        } else {
          srcsList(index) += Seq(TYPE_ITRACK, dWire(0), dWire(1))
        }
      }
      val oPin2TrackConnect = getOPin2TrackConnect(numTrack, fcList(1), List(numIOPinList(1), numIOPinList(3), numIOPinList(5), numIOPinList(7)), trackDirections)
      oPin2TrackConnect.map { dWire =>
        val dst = Seq(TYPE_OTRACK, dWire(2), dWire(3)) // type, direction, index
        if (!dstList.contains(dst)) {
          dstList += dst
        }
        val index = dstList.indexOf(dst)
        if (srcsList.size <= index) {
          srcsList += ListBuffer(Seq(TYPE_OPIN, dWire(0), dWire(1)))
        } else {
          srcsList(index) += Seq(TYPE_OPIN, dWire(0), dWire(1))
        }
      }
      val track2TrackConnect = getTrack2TrackConnect(numTrack, trackDirections)
      track2TrackConnect.map { dWire =>
        val dst = Seq(TYPE_OTRACK, dWire(2), dWire(3)) // type, direction, index
        if (!dstList.contains(dst)) {
          dstList += dst
        }
        val index = dstList.indexOf(dst)
        if (srcsList.size <= index) {
          srcsList += ListBuffer(Seq(TYPE_ITRACK, dWire(0), dWire(1)))
        } else {
          srcsList(index) += Seq(TYPE_ITRACK, dWire(0), dWire(1))
        }
      }
    }

    // remove duplicates and sort by side and index
    srcsList = srcsList.map{ x => x.distinct.sortBy(dst => (dst(0), dst(1), dst(2))) }
    dstList.zip(srcsList).sortBy{case (dst, srcs) => (dst(0), dst(1), dst(2)) } // return ListBuffer(dst, srcs)
  }

}




/** Generic Interconnect Block including PE I/O pins and routing tracks
 * 
 * @param attrs     module attributes
 */ 
class GIB(attrs: mutable.Map[String, Any]) extends Module with IR {
  val width = attrs("data_width").asInstanceOf[Int]
  // cfgParams
  val cfgDataWidth = attrs("cfg_data_width").asInstanceOf[Int]
  val cfgAddrWidth = attrs("cfg_addr_width").asInstanceOf[Int]
  val cfgBlkIndex  = attrs("cfg_blk_index").asInstanceOf[Int]     // configuration index of this block, cfg_addr[width-1 : offset]
  val cfgBlkOffset = attrs("cfg_blk_offset").asInstanceOf[Int]   // configuration offset bit of blocks
  
  val numTrack = attrs("num_track").asInstanceOf[Int]
  // if support diagonal connections between OPins and IPins
  val diagPinConect = attrs("diag_iopin_connect").asInstanceOf[Boolean]
  // numIOPinList
  val numIOPinMap = attrs("num_iopin_list").asInstanceOf[mutable.Map[String, Int]]
  val nNWi = numIOPinMap("ipin_nw")  // number of the PE input pins on the NORTHWEST side of the GIB  
  val nNWo = numIOPinMap("opin_nw")  // number of the PE output pins on the NORTHWEST side of the GIB
  val nNEi = numIOPinMap("ipin_ne")  // number of the PE input pins on the NORTHEAST side of the GIB
  val nNEo = numIOPinMap("opin_ne")  // number of the PE output pins on the NORTHEAST side of the GIB
  val nSEi = numIOPinMap("ipin_se")  // number of the PE input pins on the SOUTHEAST side of the GIB
  val nSEo = numIOPinMap("opin_se")  // number of the PE output pins on the SOUTHEAST side of the GIB
  val nSWi = numIOPinMap("ipin_sw")  // number of the PE input pins on the SOUTHWEST side of the GIB
  val nSWo = numIOPinMap("opin_sw")  // number of the PE output pins on the SOUTHWEST side of the GIB
  val numIOPinList = List(nNWi, nNWo, nNEi, nNEo, nSEi, nSEo, nSWi, nSWo)
  // fcList
  val fcList = attrs("connect_flexibility").asInstanceOf[List[Int]]
//  val fcMap = attrs("connect_flexibility").asInstanceOf[mutable.Map[String, Int]]
//  val fci  = fcMap("num_itrack_per_ipin")     // ipin-itrack connection flexibility, connected track number, 2n
//  val fco  = fcMap("num_otrack_per_opin")     // opin-otrack connection flexibility, connected track number, 2n
//  val fcio = fcMap("num_ipin_per_opin")       // opin-ipin  connection flexibility, connected ipin number, 2n
//  val fcList = List(fci, fco, fcio)

  // if there are registers in the track output ports
  val trackReged = attrs("track_reged").asInstanceOf[Boolean]
  // track connecting diractions
  val trackDirections =  attrs("track_directions").asInstanceOf[ListBuffer[Int]]
  val numIn = nNWo + nNEo + nSEo + nSWo + numTrack * trackDirections.size
  val numOut = nNWi + nNEi + nSEi + nSWi + numTrack * trackDirections.size
  apply("data_width", width)
  apply("cfg_blk_index", cfgBlkIndex)
  apply("num_iopin_list", numIOPinMap) // for debug
  apply("track_reged", trackReged)
  apply("num_input", numIn)
  apply("num_output", numOut)

  val io = IO(new Bundle{
    val cfg_en   = Input(Bool())
    val cfg_addr = Input(UInt(cfgAddrWidth.W))
    val cfg_data = Input(UInt(cfgDataWidth.W))
    val ipinNW = Output(Vec(nNWi, UInt(width.W)))
    val opinNW = Input (Vec(nNWo, UInt(width.W)))
    val ipinNE = Output(Vec(nNEi, UInt(width.W)))
    val opinNE = Input (Vec(nNEo, UInt(width.W)))
    val ipinSE = Output(Vec(nSEi, UInt(width.W)))
    val opinSE = Input (Vec(nSEo, UInt(width.W)))
    val ipinSW = Output(Vec(nSWi, UInt(width.W)))
    val opinSW = Input (Vec(nSWo, UInt(width.W)))
    val itrackW = Input (Vec(numTrack, UInt(width.W)))
    val otrackW = Output(Vec(numTrack, UInt(width.W)))
    val itrackN = Input (Vec(numTrack, UInt(width.W)))
    val otrackN = Output(Vec(numTrack, UInt(width.W)))
    val itrackE = Input (Vec(numTrack, UInt(width.W)))
    val otrackE = Output(Vec(numTrack, UInt(width.W)))
    val itrackS = Input (Vec(numTrack, UInt(width.W)))
    val otrackS = Output(Vec(numTrack, UInt(width.W)))
  })

  io.ipinNW.map{ _ := 0.U }
  io.ipinNE.map{ _ := 0.U }
  io.ipinSW.map{ _ := 0.U }
  io.ipinSE.map{ _ := 0.U }
  io.otrackW.map{ _ := 0.U }
  io.otrackN.map{ _ := 0.U }
  io.otrackE.map{ _ := 0.U }
  io.otrackS.map{ _ := 0.U }

  val wireMap = Map(
    (TYPE_OPIN, NORTHWEST) -> io.opinNW,
    (TYPE_OPIN, NORTHEAST) -> io.opinNE,
    (TYPE_OPIN, SOUTHEAST) -> io.opinSE,
    (TYPE_OPIN, SOUTHWEST) -> io.opinSW,
    (TYPE_IPIN, NORTHWEST) -> io.ipinNW,
    (TYPE_IPIN, NORTHEAST) -> io.ipinNE,
    (TYPE_IPIN, SOUTHEAST) -> io.ipinSE,
    (TYPE_IPIN, SOUTHWEST) -> io.ipinSW,
    (TYPE_OTRACK, WEST ) -> io.otrackW,
    (TYPE_OTRACK, NORTH) -> io.otrackN,
    (TYPE_OTRACK, EAST ) -> io.otrackE,
    (TYPE_OTRACK, SOUTH) -> io.otrackS,
    (TYPE_ITRACK, WEST ) -> io.itrackW,
    (TYPE_ITRACK, NORTH) -> io.itrackN,
    (TYPE_ITRACK, EAST ) -> io.itrackE,
    (TYPE_ITRACK, SOUTH) -> io.itrackS)
  // wire connections
  val connect = GIB.getGIBConnect(numTrack, diagPinConect, numIOPinList, fcList, trackDirections)
  val cfgsBit = connect.map{case (dst, srcs) => log2Ceil(srcs.size)}.reduce(_+_) // total configuration bits

  /** configuration bit sequence
   * LSB ------------> MSB
   * sorted by wire type
   *   sorted by side
   *     sorted by index
   */
  if(cfgsBit > 0) {
    val cfg = Module(new ConfigMem(cfgsBit, 1, cfgDataWidth)) // configuration memory
    cfg.io.cfg_en := io.cfg_en && (cfgBlkIndex.U === io.cfg_addr(cfgAddrWidth - 1, cfgBlkOffset))
    cfg.io.cfg_addr := io.cfg_addr(cfgBlkOffset - 1, 0)
    cfg.io.cfg_data := io.cfg_data
    assert(cfg.cfgAddrWidth <= cfgBlkOffset)
    assert(cfgBlkIndex < (1 << (cfgAddrWidth-cfgBlkOffset)))

    var offset = 0
    connect.map { case (dst, srcs) =>
      if(srcs.size > 1) {
        val mux = Module(new Muxn(width, srcs.size))
        // connect src nodes to Mux inputs, and connect Mux output to dst node
        srcs.zipWithIndex.map { case (src, i) => mux.io.in(i) := wireMap((src(0), src(1)))(src(2)) }
        if (trackReged && (dst(0) == TYPE_OTRACK)) {
          wireMap((dst(0), dst(1)))(dst(2)) := RegNext(mux.io.out)
        } else {
          wireMap((dst(0), dst(1)))(dst(2)) := mux.io.out
        }
        // connect input configuration port to the Mux configuration ports
        val w = log2Ceil(srcs.size)
        mux.io.config := cfg.io.out(0)(offset + w - 1, offset)
        offset += w
      }else{
        val src = srcs(0)
        wireMap((dst(0), dst(1)))(dst(2)) := wireMap((src(0), src(1)))(src(2))
      }
    }
  }else{
    connect.map { case (dst, srcs) =>
      val src = srcs(0)
      wireMap((dst(0), dst(1)))(dst(2)) := wireMap((src(0), src(1)))(src(2))
    }
  }


  val iPortMap = mutable.Map[String, Int]()
  var offset = 0
  if(trackDirections.contains(WEST)) {
    (0 until numTrack).map { i =>
      val name = "itrackW" + i.toString
      iPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  if(trackDirections.contains(NORTH)) {
    (0 until numTrack).map { i =>
      val name = "itrackN" + i.toString
      iPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  if(trackDirections.contains(EAST)) {
    (0 until numTrack).map { i =>
      val name = "itrackE" + i.toString
      iPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  if(trackDirections.contains(SOUTH)) {
    (0 until numTrack).map { i =>
      val name = "itrackS" + i.toString
      iPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  (0 until nNWo).map{ i => 
    val name = "opinNW" + i.toString
    iPortMap += name -> (i+offset)
  }
  offset += nNWo
  (0 until nNEo).map{ i => 
    val name = "opinNE" + i.toString
    iPortMap += name -> (i+offset)
  }
  offset += nNEo
  (0 until nSEo).map{ i => 
    val name = "opinSE" + i.toString
    iPortMap += name -> (i+offset)
  }
  offset += nSEo
  (0 until nSWo).map{ i => 
    val name = "opinSW" + i.toString
    iPortMap += name -> (i+offset)
  }

//  apply("num_input", offset+nSWo)

  val oPortMap = mutable.Map[String, Int]()
  offset = 0
  if(trackDirections.contains(WEST)) {
    (0 until numTrack).map { i =>
      val name = "otrackW" + i.toString
      oPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  if(trackDirections.contains(NORTH)) {
    (0 until numTrack).map { i =>
      val name = "otrackN" + i.toString
      oPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  if(trackDirections.contains(EAST)) {
    (0 until numTrack).map { i =>
      val name = "otrackE" + i.toString
      oPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  if(trackDirections.contains(SOUTH)) {
    (0 until numTrack).map { i =>
      val name = "otrackS" + i.toString
      oPortMap += name -> (i + offset)
    }
    offset += numTrack
  }
  (0 until nNWi).map{ i => 
    val name = "ipinNW" + i.toString
    oPortMap += name -> (i+offset)
  }
  offset += nNWi
  (0 until nNEi).map{ i => 
    val name = "ipinNE" + i.toString
    oPortMap += name -> (i+offset)
  }
  offset += nNEi
  (0 until nSEi).map{ i => 
    val name = "ipinSE" + i.toString
    oPortMap += name -> (i+offset)
  }
  offset += nSEi
  (0 until nSWi).map{ i => 
    val name = "ipinSW" + i.toString
    oPortMap += name -> (i+offset)
  }

//  apply("num_output", offset+nSWi)

  val portNameMap = Map(
    (TYPE_OPIN, NORTHWEST) -> "opinNW",
    (TYPE_OPIN, NORTHEAST) -> "opinNE",
    (TYPE_OPIN, SOUTHEAST) -> "opinSE",
    (TYPE_OPIN, SOUTHWEST) -> "opinSW",
    (TYPE_IPIN, NORTHWEST) -> "ipinNW",
    (TYPE_IPIN, NORTHEAST) -> "ipinNE",
    (TYPE_IPIN, SOUTHEAST) -> "ipinSE",
    (TYPE_IPIN, SOUTHWEST) -> "ipinSW",
    (TYPE_OTRACK, WEST ) -> "otrackW",
    (TYPE_OTRACK, NORTH) -> "otrackN",
    (TYPE_OTRACK, EAST ) -> "otrackE",
    (TYPE_OTRACK, SOUTH) -> "otrackS",
    (TYPE_ITRACK, WEST ) -> "itrackW",
    (TYPE_ITRACK, NORTH) -> "itrackN",
    (TYPE_ITRACK, EAST ) -> "itrackE",
    (TYPE_ITRACK, SOUTH) -> "itrackS")


  // ======= sub_module attribute ========//
  // track registers are hiden
  if(cfgsBit > 0) {
    val sub_modules = (0 until 1).map { i =>
      Map(
        "id" -> 1,
        "type" -> "Muxn"
      )
    }
    apply("sub_modules", sub_modules)
  }

  // ======= sub_module instance attribute ========//
  // apply("sub_module_format", ("id", "type"))
  // 0 : this module
  // 1-n : sub-modules 
  val smi_id: mutable.Map[String, ListBuffer[Int]] = mutable.Map(
    "This" -> ListBuffer(0),
    "Muxn" -> ListBuffer[Int]()
  )

  // ======= connections & configuration attribute ========//
  // apply("connection_format", ("src_id", "src_type", "src_out_idx", "dst_id", "dst_type", "dst_in_idx"))
  // This:src_out_idx is the input index
  // This:dst_in_idx is the output index
  val connections = ListBuffer[(Int, String, Int, Int, String, Int)]()
  val configuration = mutable.Map( // id : type, high, low
    smi_id("This")(0) -> ("This", cfgsBit-1, 0)
  )
  offset = 0
  var i = 0
  connect.map{ case (dst, srcs) =>
    val dst_name = portNameMap((dst(0),dst(1))) + dst(2).toString
    if(srcs.size > 1) {
      smi_id("Muxn") += (i+1)
      // connect src nodes to Mux inputs, and connect Mux output to dst node
      srcs.zipWithIndex.map { case (src, j) =>
        val src_name = portNameMap((src(0), src(1))) + src(2).toString
        connections.append((smi_id("This")(0), "This", iPortMap(src_name), smi_id("Muxn")(i), "Muxn", j))
      }
      connections.append((smi_id("Muxn")(i), "Muxn", 0, smi_id("This")(0), "This", oPortMap(dst_name)))
      // connect input configuration port to the Mux configuration ports
      val w = log2Ceil(srcs.size)
      configuration += smi_id("Muxn")(i) -> ("Muxn", offset + w - 1, offset)
      i += 1
      offset += w
    }else{
      val src = srcs(0)
      val src_name = portNameMap((src(0), src(1))) + src(2).toString
      connections.append((smi_id("This")(0), "This", iPortMap(src_name), smi_id("This")(0), "This", oPortMap(dst_name)))
    }
  }

  // apply("connections", connections)
  apply("connections", connections.zipWithIndex.map{case (c, i) => i -> c}.toMap)
  if(cfgsBit > 0){
    apply("configuration", configuration)
  }
  val instances = smi_id.map{case (name, ids) =>
    ids.map{id => Map(
      "id" -> id,
      "type" -> name,
      "module_id" -> {if(name == "This") 0 else 1}
    )}
  }.flatten
  apply("instances", instances)

//  val outFilename = "test_run_dir/my_cgra_test.json"
//  printIR(outFilename)
}





// object VerilogGen extends App {
//   // test getGIBConnect
//   val connect = GIB.getGIBConnect(0, false, List(2,1,2,1,1,1,1,1), List(2, 4, 2))
//   println(connect.size)
//   var sum = 0
//   var i = 0
//   connect.map{ case (dst, srcs) =>
//     sum += srcs.size
//     println(i + ": " +  srcs.size)
//     i += 1
//     println(dst)
//     println(srcs)
//   }
//   println(sum)
//
////   val num_iopin_list = mutable.Map(
////     "ipin_nw" -> 2,
////     "opin_nw" -> 1,
////     "ipin_ne" -> 2,
////     "opin_ne" -> 1,
////     "ipin_se" -> 2,
////     "opin_se" -> 1,
////     "ipin_sw" -> 2,
////     "opin_sw" -> 1
////   )
////   val connect_flexibility = mutable.Map(
////     "num_itrack_per_ipin" -> 2,
////     "num_otrack_per_opin" -> 4,
////     "num_ipin_per_opin"   -> 4
////   )
////   val attrs: mutable.Map[String, Any] = mutable.Map(
////     "data_width" -> 32,
////     "cfg_data_width" -> 64,
////     "cfg_addr_width" -> 8,
////     "cfg_blk_index" -> 1,
////     "cfg_blk_offset" -> 4,
////     "num_track" -> 0,
////     "track_reged" -> false,
////     "diag_iopin_connect" -> true,
////     "num_iopin_list" -> num_iopin_list,
////     "connect_flexibility" -> connect_flexibility
////   )
////
////   (new chisel3.stage.ChiselStage).emitVerilog(new GIB(attrs),args)
// }