package tram

import chisel3._
import freechips.rocketchip.tile._
import freechips.rocketchip.config._
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.subsystem.SystemBusKey
import tram.spec.TramSpec


class WithTram extends Config((site, here, up) => {
  case BuildRoCC => up(BuildRoCC) ++ Seq(
    (p: Parameters) => {
      val tram = LazyModule(new Tram(OpcodeSet.custom0)(p))
      tram
    }
  )
  case SystemBusKey => up(SystemBusKey).copy(beatBytes = TramSpec.system_bus_beat_bits/8)
})
