#
# This helper script is meant to set up a valid value for the
# TDAQ_RELEASE_BASE environment variable for the builds.
#
# If the environment variable is already set when the script runs, its
# value is not modified. If it isn't set yet, then the script tries to
# check for the existence of a number of standard directories in which
# TDAQ releases usually reside.
#

if [ -z "${TDAQ_RELEASE_BASE}" ]; then

    if [ -d /cvmfs/atlas.cern.ch/repo/sw/tdaq ]; then
        export TDAQ_RELEASE_BASE=/cvmfs/atlas.cern.ch/repo/sw/tdaq
    elif [ -d /afs/cern.ch/atlas/project/tdaq/prod ]; then
        export TDAQ_RELEASE_BASE=/afs/cern.ch/atlas/project/tdaq/prod
    else
        echo "Error: Cannot find TDAQ software installation"
        return 1
    fi
    echo "Set TDAQ_RELEASE_BASE = ${TDAQ_RELEASE_BASE}"

else
    echo "Leaving TDAQ_RELEASE_BASE = ${TDAQ_RELEASE_BASE}"
fi
