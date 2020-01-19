#!/bin/sh
#
# art-description: Art test job for MooPerformance package, migrated from RTT job: SingleMuons-mc12-Pt50GeV
# 
# art-type: grid
# art-include: master/Athena
# art-include: 21.0/Athena
# art-include: 21.3/Athena
#
# art-input: user.zhidong.SingleMuon_50GeV_task12089639.nopileup.HITStoRDO.r9707.20170907.v01_EXT0
# art-input-nfiles: 10
# art-output: trk*.txt

set -e
fileList="['${ArtInFile//,/', '}']"
echo "List of files = ", $fileList

athena.py -c "from AthenaCommon.AthenaCommonFlags import athenaCommonFlags; athenaCommonFlags.EvtMax=20000; athenaCommonFlags.FilesInput = $fileList" -b MooPerformance/MooPerformance_topOptions.py
