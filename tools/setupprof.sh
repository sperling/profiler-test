#!/bin/sh

# run with source ./setupprof.sh or . ./setupprof.sh

CURRENT_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
FULL_PATH=$CURRENT_PATH/../bin/Product/Linux.x64.Debug
# libExtension determines extension for dynamic library files
OSName=$(uname -s)
libExtension=
case $OSName in
    Linux)
        libExtension="so"
        ;;

    Darwin)
        libExtension="dylib"
        ;;
    *)
        echo "Unsupported OS $OSName detected, configuring as if for Linux"
        libExtension="so"
        ;;
esac
ABSOLUTE_PATH=$(cd $FULL_PATH; pwd)/profiler-test.$libExtension

export COMPlus_LogEnable=1
export COMPlus_LogToFile=1

export CORECLR_PROFILER_PATH=$ABSOLUTE_PATH
export CORECLR_ENABLE_PROFILING=1
export CORECLR_PROFILER={C4D6E538-1AF1-44D0-92C0-5525DE10B726}

