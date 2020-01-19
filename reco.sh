#! /bin/bash

cd /afs/cern.ch/work/r/rushioda/private/Mask/singleMuon_NoMask 

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
#source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh

lsetup git python
asetup 21.0.82,Athena,here

cd /afs/cern.ch/work/r/rushioda/private/Mask/singleMuon_NoMask/build/

cmake -DATLAS_PACKAGE_FILTER_FILE=/afs/cern.ch/work/r/rushioda/private/Mask/singleMuon_NoMask/package_filters.txt /afs/cern.ch/work/r/rushioda/private/Mask/singleMuon_NoMask/athena/Projects/WorkDir
make -j8

cd /afs/cern.ch/work/r/rushioda/private/Mask/singleMuon_NoMask/
source /afs/cern.ch/work/r/rushioda/private/Mask/singleMuon_NoMask/build/x86_64-slc6-gcc62-opt/setup.sh

Condition="NoMask"

for i in {1..1}; do
  SkipEve=$(( ( $i-1 )*5000 ))
  outputESDFileName=Muon5000_${Condition}.ESD.pool.root.$i
  outputAODFileName=Muon5000_${Condition}.xAOD.pool.root.$i
  Reco_tf.py \
    --maxEvents '5000' \
    --skipEvents $SkipEve \
    --digiSteeringConf 'StandardSignalOnlyTruth' \
    --conditionsTag 'default:OFLCOND-MC16-SDR-23' \
    --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
    --autoConfiguration 'everything' \
    --steering 'doRAWtoALL' \
    --valid 'True' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' \
               'RAWtoALL:from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import PixelPrepDataToxAOD;xAOD_PixelPrepDataToxAOD=PixelPrepDataToxAOD(name="xAOD_PixelPrepDataToxAOD");xAOD_PixelPrepDataToxAOD.WriteSiHits=True;' \
               'HITtoRDO:outStream=topSequence.StreamRDO;outStream.ItemList+=["SiHitCollection#PixelHits"];outStream.ItemList+=["SiHitCollection#SCT_Hits"];' \
               'RAWtoALL:outStream=topSequence.StreamESD;outStream.ItemList+=["SiHitCollection#PixelHits"];outStream.ItemList+=["SiHitCollection#SCT_Hits"];' \
               'all:from IOVDbSvc.CondDB import conddb;conddb.blockFolder("/PIXEL/PixMapOverlay");  conddb.addFolderWithTag("","/PIXEL/PixMapOverlay"+"<dbConnection>sqlite://X;schema=NoMask.db;dbname=OFLP200</dbConnection>","PixMapOverlay-SIM-MC16-000-02",force=True, forceMC=True, forceData=False); conddb.OutputLevel=VERBOSE;' \
    --preExec 'all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);rec.doTrigger.set_Value_and_Lock(False);' \
              'all:from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.doSlimming.set_Value_and_Lock(False)' \
              'all:from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.AODEDMSet.set_Value_and_Lock("AODFULL");' \
              'HITtoRDO:userRunLumiOverride={"run":300000, "startmu":45.0, "endmu":88.0, "stepmu":1.0, "startlb":1, "timestamp":1500000000};ScaleTaskLength=0.4;' \
              'r2a:from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doSlimming.set_Value_and_Lock(False);' \
   --inputHITSFile 'Muon100000.HITS.v1.pool.root' \
   --outputESDFile $outputESDFileName \
   --outputDAOD_PIXELVALIDFile $outputAODFileName
done

