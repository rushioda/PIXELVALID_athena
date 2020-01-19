#!/bin/sh

# art-description: OverlayChain+Reco test for data16.
# art-type: grid
# art-include: 21.0/Athena                                                    
# art-include: 21.3/Athena                                                    
# art-include: master/Athena 

# art-output: *.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

OverlayChain_tf.py \
--inputZeroBiasBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1 \
--DataRunNumber 2015 \
--inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.EVNT.e3601_e5984/EVNT.12228944._002158.pool.root.1 \
--outputRDOFile testRTT.RDO.pool.root \
--outputHITSFile testRTT.HITS.pool.root \
--triggerBit 240 \
--skipEvents 0 --maxEvents 10 --randomSeed 123456789 \
--geometryVersion ATLAS-R2-2015-03-01-00 \
--conditionsTag CONDBR2-BLKPA-2016-12 \
--digiSeedOffset1=211 --digiSeedOffset2=122 \
--samplingFractionDbTag FTFP_BERT_BIRK --fSampltag LARElecCalibMCfSampl-G496-19213- \
--preInclude 'sim:EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/magfield.py' 'overlayBS:EventOverlayJobTransforms/custom.py' \
--preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.nSamples.set_Value_and_Lock(4);from LArConditionsCommon.LArCondFlags import larCondFlags; larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase")' \
--postInclude 'sim:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py,EventOverlayJobTransforms/muAlign.py,EventOverlayJobTransforms/g4runnumber.py,EventOverlayJobTransforms/blockFoldersData_EVNTtoHITS.py' 'overlayBS:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py,EventOverlayJobTransforms/blockFoldersData_OverlayBS.py' \
--ignorePatterns "L1TopoMenuLoader.+ERROR." \
--imf False
echo "art-result: $? dataoverlay"

Reco_tf.py \
--inputRDOFile testRTT.RDO.pool.root \
--outputESDFile testRTT.ESD.pool.root \
--outputAODFile testRTT.AOD.pool.root \
--preInclude 'EventOverlayJobTransforms/custom.py,EventOverlayJobTransforms/recotrfpre.py' \
--postInclude 'r2e:EventOverlayJobTransforms/Rt_override_CONDBR2-BLKPA-2015-12.py,EventOverlayJobTransforms/muAlign_reco.py,EventOverlayJobTransforms/blockFoldersData_RAWtoESD.py' 'ESDtoAOD:EventOverlayJobTransforms/blockFoldersData_ESDtoAOD.py' \
--preExec 'from LArConditionsCommon.LArCondFlags import larCondFlags;larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples1phase");rec.doTrigger=False;' \
--ignorePatterns "L1TopoMenuLoader.+ERROR." \
--postExec 'r2e:from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool;LArCellBuilderFromLArRawChannelTool.RawChannelsName="LArRawChannels_FromDigits"' \
--imf False
echo "art-result: $? reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? regression"
