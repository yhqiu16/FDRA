#!/bin/bash

help () {
  echo "Setup all the directories and symlinks that you need to use the scripts"
  echo 'in `scripts/`.'
  echo
  echo "Usage: $0 [-h|--help]"
  echo
  exit
}

if [ $# -gt 0 ]; then
   help
fi

if [ ! -d sims/generated-src ]; then
    mkdir sims/generated-src/
fi

if [ ! -d sims/waveforms ]; then
    mkdir sims/waveforms/
fi

if [ ! -f ../chipyard/src/main/scala/config/TramSoCConfigs.scala ]; then
    sed '1d; $d' $PWD/cgra-mg/src/main/scala/soc/TramSoCConfigs.scala > ../chipyard/src/main/scala/config/TramSoCConfigs.scala
fi

if [ ! -f sims/generated-src/verilator ] && [ ! -d sims/generated-src/verilator ]; then
    ln -s $PWD/../../sims/verilator/generated-src/ sims/generated-src/verilator 2>/dev/null
fi

if [ ! -f sims/generated-src/vcs ] && [ ! -d sims/generated-src/vcs ]; then
    ln -s $PWD/../../sims/vcs/generated-src/ sims/generated-src/vcs 2>/dev/null
fi

