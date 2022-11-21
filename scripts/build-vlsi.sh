#!/bin/bash

cd ../../vlsi/
source vlsi_env.sh
#make verilog CONFIG=TramSoCVLSIConfig EXTRA_FIRRTL_OPTIONS="--emission-options=disableMemRandomization,disableRegisterRandomization"
make verilog CONFIG=TramSoCVLSIConfig EXTRA_FIRRTL_OPTIONS="--emission-options=disableMemRandomization,disableRegisterRandomization" tech_name=nangate45 INPUT_CONFS="example-nangate45.yml"
