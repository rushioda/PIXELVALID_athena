#!/bin/sh
#
# art-description: Tests detector response to single electrons, generated on-the-fly, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

AtlasG4_tf.py \
--preInclude 'SimulationJobOptions/preInclude.SingleElectronGenerator.py' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '1000' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'FTFP_BERT' \
--postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest.py' \
--preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and run DCube
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"

# TODO Remake plots by reading in the HITS file produced by the previous job (Tests TP conversion) - need to run DCube on output
# athena G4AtlasTests/test_AtlasG4_electrons_noevgen.py


