#!/bin/bash

#cd ../../sims/verilator/
#make verilog CONFIG=TramSoCConfig
cd ../../sims/vcs/
make ${DEBUG} CONFIG=TramSoCConfig EXTRA_FIRRTL_OPTIONS=--emission-options=disableMemRandomization,disableRegisterRandomization


