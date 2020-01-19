#!/bin/bash

# art-description: Trigger Event Data Model test on AOD
# art-type: grid
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: master/AthenaP1
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-output: *check*
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*
# art-output: TotalEventsProcessed.txt

export NAME="mc_pp_v7_trigedm_grid"
export JOB_LOG="athena.log"
export TEST="TrigAnalysisTest"
export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/AthenaTrigAOD_TrigEDMandTDTCheck_MC_pp_v7_chain/AOD.pool.root"]'

athena.py -c "RunningRTT=TRUE;jp.AthenaCommonFlags.PoolAODInput=${DS};DetDescrVersion='ATLAS-R2-2015-03-01-00'" -b TrigAnalysisTest/testAthenaTrigAOD_TrigEDMCheck.py | tee ${JOB_LOG}
echo "art-result: ${PIPESTATUS[0]} ${JOB_LOG%%.*}"

export SKIP_CHAIN_DUMP=1
echo "trigedm SKIP_CHAIN_DUMP" $[SKIP_CHAIN_DUMP]
source exec_art_triggertest_post.sh
