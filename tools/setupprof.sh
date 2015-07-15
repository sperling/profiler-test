#!/bin/sh

# run with source ./setupprof.sh or . ./setupprof.sh

CURRENT_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
FULL_PATH=$CURRENT_PATH/../bin/Product/Linux.x64.Debug
ABSOLUTE_PATH=$(cd $FULL_PATH; pwd)/profiler-test

export COMPlus_LogEnable=1
export COMPlus_LogToFile=1

export CORECLR_PROFILER_PATH=$ABSOLUTE_PATH
export CORECLR_ENABLE_PROFILING=1
export CORECLR_PROFILER={C4D6E538-1AF1-44D0-92C0-5525DE10B726}

