/*
package chipyard

import freechips.rocketchip.config.{Config}

class TramSoCConfig extends Config(
  new tram.WithTram ++

  // Set your custom L2 configs
  new chipyard.config.WithL2TLBs(512) ++

  new freechips.rocketchip.subsystem.WithInclusiveCache(
    nBanks = 1,
    nWays = 8,
    capacityKB = 512,
    outerLatencyCycles = 40
  ) ++

  // Set the number of CPUs you want to create
  new freechips.rocketchip.subsystem.WithNBigCores(1) ++

  new chipyard.config.AbstractConfig
)
*/