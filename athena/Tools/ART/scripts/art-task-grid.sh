#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
#
# Example command lines for three types:
#
# art-task-grid.sh [--no-action] batch <submit_directory> <script_directory> <sequence_tag> <package> <outfile> <inform_panda> <job_type> <number_of_tests>
#
# art-task-grid.sh [--no-action] single [--inds <input_file> --n-files <number_of_files> --split <split>] <submit_directory> <script_directory> <sequence_tag> <package> <outfile> <inform_panda> <job_name>
#
# env: ART_GRID_OPTIONS
#
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
#
# options have to be in-order, and at the correct place
#
# example: [--test-name TestName --inDS user.tcuhadar.SingleMuon... --nFiles 3 --in] tmp /cvmfs/atlas-nightlies.cern.ch/sw/... Tier0ChainTests grid 316236 3  user.${USER}.atlas.${NIGHTLY_RELEASE_SHORT}.${PROJECT}.${PLATFORM}.${NIGHTLY_TAG}.${SEQUENCE_TAG}.${PACKAGE}[.${TEST_NUMBER}]
set -e

echo "art-task-grid.sh executed by $(whoami) on $(date)"

NO_ACTION=0
if [ "$1" == "--no-action" ]; then
    NO_ACTION=1
    shift
    echo "NO_ACTION=${NO_ACTION}"
fi

TYPE=$1
shift
echo "TYPE=${TYPE}"

case ${TYPE} in

    'batch')
        echo "Running 'batch'"
        SPLIT=""
        ;;
    'single')
        echo "Running 'single'"
        INDS=""
        if [ "$1" == "--inds" ]; then
            INDS="--inDS $2"
            shift
            shift
        fi
        NFILES=""
        NFILES_PER_JOB=""
        if [ "$1" == "--n-files" ]; then
            NFILES="--nFiles $2"
            NFILES_PER_JOB="--nFilesPerJob $2"
            NCORE_NFILES_PER_JOB="--nFilesPerJob $2"
            shift
            shift
        fi
        SPLIT=""
        LARGE_JOB="--long --memory 4096"
        if [ "$1" == "--split" ]; then
            SPLIT="--split $2"
            NFILES_PER_JOB=""
            LARGE_JOB=""
            shift
            shift
        fi
        IN_FILE=""
        if [ "$1" == "--in" ]; then
          IN_FILE="--in=%IN"
          shift
        fi
        NCORES=""
        if [ "$1" == "--ncore" ]; then
            NCORES="--nCore $2"
            NFILES_PER_JOB="${NCORE_NFILES_PER_JOB}"
            LARGE_JOB=""
            shift
            shift
        fi
        ATHENA_MT=""
        if [ "$1" == "--athena_mt" ]; then
            NCORES="--nCore $2"
            LARGE_JOB=""
            shift
            shift
        fi
        ;;
    *)
        echo "Unknown TYPE: ${TYPE}"
        exit 1
        ;;
esac

SUBMIT_DIRECTORY=$1
shift
echo "SUBMIT_DIRECTORY=${SUBMIT_DIRECTORY}"

SCRIPT_DIRECTORY=$1
shift
echo "SCRIPT_DIRECTORY=${SCRIPT_DIRECTORY}"

SEQUENCE_TAG=$1
shift
echo "SEQUENCE_TAG=${SEQUENCE_TAG}"

PACKAGE=$1
shift
echo "PACKAGE=${PACKAGE}"

OUTFILE=$1
shift
echo "OUTFILE=${OUTFILE}"

INFORM_PANDA=$1
shift
echo "INFORM_PANDA=${INFORM_PANDA}"

case ${TYPE} in

    'batch')
        JOB_TYPE=$1
        shift
        echo "JOB_TYPE=${JOB_TYPE}"

        NUMBER_OF_TESTS=$1
        SPLIT="--split ${NUMBER_OF_TESTS}"
        shift
        echo "NUMBER_OF_TESTS=${NUMBER_OF_TESTS}"
        echo "SPLIT=${SPLIT}"
        ;;

    'single')
        JOB_NAME=$1
        shift
        echo "JOB_NAME=${JOB_NAME}"
        ;;
esac

# general options
PATHENA_OPTIONS="--noBuild --expertOnly_skipScout --noEmail --maxAttempt 2"
OUT="%OUT.tar"

# we seem to have to copy the env variables locally
GRID_OPTIONS=$ART_GRID_OPTIONS
echo "GRID_OPTIONS=${GRID_OPTIONS}"


case ${TYPE} in

    'batch')
        # <script_directory> <sequence_tag> <package> <outfile> <job_type> <job_index>
        INTERNAL_COMMAND="grid batch"
        JOB_INDEX="%RNDM:0"
        ARGS="${JOB_TYPE} ${JOB_INDEX}"
        echo "JOB_INDEX=${JOB_INDEX}"
        echo "ARGS=${ARGS}"
        ;;
    'single')
        # <script_directory> <sequence_tag> <package> <outfile> <job_name>
        INTERNAL_COMMAND="grid single"
        PATHENA_TYPE_OPTIONS="${LARGE_JOB} ${INDS} ${NFILES} ${NFILES_PER_JOB} ${NCORES} ${ATHENA_MT}"
        ARGS="${JOB_NAME}"
        echo "PATHENA_TYPE_OPTIONS=${PATHENA_TYPE_OPTIONS}"
        echo "ARGS=${ARGS}"
        ;;
esac


# NOTE: for art-internal.py the current dir can be used as it is copied there
cd "${SUBMIT_DIRECTORY}"/"${PACKAGE}"/run
SUBCOMMAND="./art-internal.py ${INTERNAL_COMMAND} ${IN_FILE} ${SCRIPT_DIRECTORY} ${SEQUENCE_TAG} ${PACKAGE} ${OUT} ${INFORM_PANDA} ${ARGS}"
CMD="pathena ${GRID_OPTIONS} ${PATHENA_OPTIONS} ${PATHENA_TYPE_OPTIONS} --trf \"${SUBCOMMAND}\" ${SPLIT} --outDS ${OUTFILE} --extOutFile art-job.json"

#--disableAutoRetry
#--excludedSite=ANALY_TECHNION-HEP-CREAM
#--site=ANALY_NIKHEF-ELPROD_SHORT,ANALY_NIKHEF-ELPROD"

echo "Command: ${CMD}"

if [ ${NO_ACTION} -ne 1 ]; then
    echo "Submitting..."
    RESULT=$(eval "${CMD}")
    echo "${RESULT}"
fi
