#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --conditionsTag all:CONDBR2-BLKPA-2017-05 --ignoreErrors 'False' --autoConfiguration='everything' --maxEvents '250' --geometryVersion all:ATLAS-R2-2015-04-00-00 --steering='doRAWtoALL' --inputBSFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00307716.physics_Main.daq.RAW._lb0220._SFO-1._0001.data' --outputDRAW_EGZFile 'myDRAW_EGZ.data' --outputDESDM_IDALIGNFile 'myDESDM_IDALIGN.pool.root' --outputDESDM_SGLELFile 'myDESDM_SGLEL.pool.root' --outputDESDM_SLTTMUFile 'myDESDM_SLTTMU.pool.root' --outputDAOD_IDTRKVALIDFile 'myDAOD_IDTRKVALID.pool.root' --outputDRAW_ZMUMUFile 'myDRAW_ZMUMU.data' --outputAODFile 'myAOD.pool.root' --outputDRAW_TAUMUHFile 'myDRAW_TAUMUH.data' --outputDESDM_EGAMMAFile 'myDESDM_EGAMMA.pool.root' --outputDESDM_MCPFile 'myDESDM_MCP.pool.root' --outputDESDM_CALJETFile 'myDESDM_CALJET.pool.root' --outputDESDM_PHOJETFile 'myDESDM_PHOJET.pool.root' --outputDESDM_TILEMUFile 'myDESDM_TILEMU.pool.root' --outputDRAW_RPVLLFile 'myDRAW_RPVLL.data' --outputDESDM_EXOTHIPFile 'myDESDM_EXOTHIP.pool.root' --outputESDFile 'myESD.pool.root' --outputHISTFile 'myHIST.root' --outputDAOD_IDTIDEFile 'myDAOD_IDTIDE.pool.root' --imf False
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 2 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
