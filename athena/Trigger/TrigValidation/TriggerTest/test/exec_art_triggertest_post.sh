#!/bin/bash

echo  $(date "+%FT%H:%M %Z")"     Execute TriggerTest post processing for test ${NAME}"

### DEFAULTS

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${TEST} ]; then
  export TEST="TriggerTest"
fi

if [ -z ${REF_FOLDER} ]; then
  export REF_FOLDER="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/${TEST}/ref/${AtlasBuildBranch}/${NAME}"
fi

###

echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${JOB_LOG} | tee checklog.log

echo "art-result: ${PIPESTATUS[0]} CheckLog"

# Run perfmon
timeout 1m perfmon.py -f 0.90 ntuple.pmon.gz
timeout 1m convert -density 300 -trim ntuple.perfmon.pdf -quality 100 -resize 50% ntuple.perfmon.png

echo $(date "+%FT%H:%M %Z")"     Running chainDump"
timeout 1m chainDump.py -S --rootFile=expert-monitoring.root

export JOB_LOG_TAIL=${JOB_LOG%%.*}.tail.${JOB_LOG#*.}
tail -10000  ${JOB_LOG} > ${JOB_LOG_TAIL}

if [ -f ${REF_FOLDER}/athena.regtest ]; then
  echo $(date "+%FT%H:%M %Z")"     Running regtest"
  grep REGTEST athena.log > athena.regtest
  timeout 1m regtest.pl --inputfile athena.regtest --reffile ${REF_FOLDER}/athena.regtest | tee regtest.log
  echo "art-result: ${PIPESTATUS[0]} RegTest"
else
  echo $(date "+%FT%H:%M %Z")"     No reference athena.regtest found in ${REF_FOLDER}"
fi

if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
  echo $(date "+%FT%H:%M %Z")"     Running rootcomp"
  timeout 10m rootcomp.py ${REF_FOLDER}/expert-monitoring.root | tee rootcompout.log
  echo "art-result: ${PIPESTATUS[0]} RootComp"
  echo $(date "+%FT%H:%M %Z")"     Running checkcounts"
  timeout 10m trigtest_checkcounts.sh 0 expert-monitoring.root ${REF_FOLDER}/expert-monitoring.root HLT | tee checkcountout.log
  echo "art-result: ${PIPESTATUS[0]} CheckCounts"
else
  echo $(date "+%FT%H:%M %Z")"     No reference expert-monitoring.root found in ${REF_FOLDER}"
fi

if [ -f trig_cost.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CostMon"
  timeout 2h RunTrigCostD3PD -f trig_cost.root --outputTagFromAthena --costMode > costMon.log 2>&1
else 
  echo $(date "+%FT%H:%M %Z")"     file trig_cost.root does not exist thus RunTrigCostD3PD will not be run"
fi


echo "trigedm SKIP_CHAIN_DUMP" $[SKIP_CHAIN_DUMP]
if [ $[SKIP_CHAIN_DUMP] != 1 ]; then 
   echo $(date "+%FT%H:%M %Z")"     Running check for zero L1, HLT or TE counts"
   export COUNT_EXIT=0
   if [[ `sed 's|.*\(.* \)|\1|' L1AV.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
      echo "L1 counts   ERROR  : all entires are ZERO please consult L1AV.txt"
     (( COUNT_EXIT = COUNT_EXIT || 1 ))
   fi
   if [[ `sed 's|.*\(.* \)|\1|' HLTChain.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
      echo "HLTChain counts   ERROR  : all entires are ZERO please consult HLTChain.txt"
      (( COUNT_EXIT = COUNT_EXIT || 1 ))
   fi
   if [[ `sed 's|.*\(.* \)|\1|' HLTTE.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
      echo "HLTTE counts   ERROR  : all entires are ZERO please consult HLTTE.txt"
      (( COUNT_EXIT = COUNT_EXIT || 1 ))
   fi
   echo "art-result: ${COUNT_EXIT} ZeroCounts"
else
  echo $(date "+%FT%H:%M %Z")"    Do not run ZERO counts for this test, SKIP_CHAIN_DUMP=1"
fi

if [ -f ESD.pool.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on ESD"
  timeout 10m checkFile.py ESD.pool.root | tee ESD.pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileESD"
else 
  echo $(date "+%FT%H:%M %Z")"     No ESD.pool.root to check"
fi

if [ -f AOD.pool.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on AOD"
  timeout 10m checkFile.py AOD.pool.root | tee AOD.pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileAOD"
  echo $(date "+%FT%H:%M %Z")"     Running CheckxAOD AOD"
  timeout 10m checkxAOD.py AOD.pool.root | tee AOD.pool.root.checkxAODFile
  echo "art-result: ${PIPESTATUS[0]} CheckXAOD"
else 
  echo $(date "+%FT%H:%M %Z")"     No AOD.pool.root to check"
fi

echo  $(date "+%FT%H:%M %Z")"     Files in directory:"
ls -lh

echo  $(date "+%FT%H:%M %Z")"     Finished TriggerTest post processing for test ${NAME}"
