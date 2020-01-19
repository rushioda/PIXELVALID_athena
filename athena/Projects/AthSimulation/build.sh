#!/bin/bash
#
# Script for building the release on top of externals built using one of the
# scripts in this directory.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build.sh [-t build type] [-b build dir] [-c] [-m] [-i] [-p] [-a]"
    echo " -c: Execute CMake step"
    echo " -m: Execute make step"
    echo " -i: Execute install step"
    echo " -p: Execute CPack step"
    echo " -a: Abort on error"
    echo "If none of the c, m, i or p options are set then the script will do"
    echo "*all* steps. Otherwise only the enabled steps are run - it's your"
    echo "reponsibility to ensure that precusors are in good shape"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
EXE_CMAKE=""
EXE_MAKE=""
EXE_INSTALL=""
EXE_CPACK=""
NIGHTLY=true
while getopts ":t:b:hcmipa" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
	c)
	    EXE_CMAKE="1"
	    ;;
	m)
	    EXE_MAKE="1"
	    ;;
	i)
	    EXE_INSTALL="1"
	    ;;
	p)
	    EXE_CPACK="1"
	    ;;
	a)
	    NIGHTLY=false
	    ;;
        h)
            usage
            exit 0
            ;;
        :)
            echo "Argument -$OPTARG requires a parameter!"
            usage
            exit 1
            ;;
        ?)
            echo "Unknown argument: -$OPTARG"
            usage
            exit 1
            ;;
    esac
done

# If no step was explicitly specified, turn them all on:
if [ -z "$EXE_CMAKE" -a -z "$EXE_MAKE" -a -z "$EXE_INSTALL" -a -z "$EXE_CPACK" ]; then
    EXE_CMAKE="1"
    EXE_MAKE="1"
    EXE_INSTALL="1"
    EXE_CPACK="1"
fi

# Stop on errors from here on out:
set -e
# consider a pipe failed if ANY of the commands fails
set -o pipefail

# Source in our environment
AthSimulationSrcDir=$(dirname ${BASH_SOURCE[0]})
if [ -z "$BUILDDIR" ]; then
    BUILDDIR=${AthSimulationSrcDir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd ${BUILDDIR} && pwd)
source $AthSimulationSrcDir/build_env.sh -b $BUILDDIR >& $BUILDDIR/build_env.log
cat $BUILDDIR/build_env.log

# create the actual build directory
mkdir -p ${BUILDDIR}/build/AthSimulation
cd ${BUILDDIR}/build/AthSimulation

# CMake:
if [ -n "$EXE_CMAKE" ]; then
    # Remove the CMakeCache.txt file, to force CMake to find externals
    # from scratch in an incremental build.
    rm -f CMakeCache.txt
    # Now run the actual CMake configuration:
    time cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} \
        -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
        ${AthSimulationSrcDir} 2>&1 | tee cmake_config.log
fi

# for nightly builds we want to get as far as we can
if [ "$NIGHTLY" = true ]; then
    # At this point stop worrying about errors:
    set +e
fi

# make:
if [ -n "$EXE_MAKE" ]; then
    time make -k 2>&1 | tee cmake_build.log
fi

# Install the results:
if [ -n "$EXE_INSTALL" ]; then
    time make install/fast \
	DESTDIR=${BUILDDIR}/install/AthSimulation/${NICOS_PROJECT_VERSION} 2>&1 | tee cmake_install.log
fi

# Build an RPM for the release:
if [ -n "$EXE_CPACK" ]; then
    time cpack 2>&1 | tee cmake_cpack.log
    cp AthSimulation*.rpm ${BUILDDIR}/
fi
