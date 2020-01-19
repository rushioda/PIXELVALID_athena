#!/bin/sh
#
# art-description: Run simulation using ISF with the FullG4 simulator, reading single pion events, writing HITS including full CaloCalibrationHit information, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root
# art-output: comparison.root
# art-output: comparison.ps

Sim_tf.py \
--conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
--physicsList 'FTFP_BERT' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4'  \
--postInclude 'G4AtlasTests/postInclude.DCubeTest_CaloCalibHits.py' \
--preExec 'simFlags.ReleaseGeoModel=False;' \
--preInclude 'SimulationJobOptions/preInclude.CalHits.py,SimulationJobOptions/preInclude.ParticleID.py' \
--DataRunNumber '222525' \
--geometryVersion 'default:ATLAS-R2-2015-03-01-00' \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/pi_E50_eta0-60.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '10' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"
