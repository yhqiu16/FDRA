#!/bin/bash

help () {
  echo "Build a cycle-accurate Verilator simulator for SoC programs,"
  echo 'matching `TramSoCConfig` in `chipyard/src/main/scala/config/TramSoCConfigs.scala.'
  echo
  echo "Usage: $0 [-h|--help] [--debug]"
  echo
  echo "Options:"
  echo " debug   Builds a Verilator simulator which generates waveforms. Without"
  echo "         this option, the simulator will not generate any waveforms."
  exit
}

show_help=0
DEBUG=""

while [ $# -gt 0 ] ; do
  case $1 in
    -h | --help) show_help=1 ;;
    --debug) DEBUG="debug"
  esac

  shift
done

if [ $show_help -eq 1 ]; then
 help
fi

cd ../../sims/verilator/
make ${DEBUG} CONFIG=TramSoCConfig VERILATOR_THREADS=6

