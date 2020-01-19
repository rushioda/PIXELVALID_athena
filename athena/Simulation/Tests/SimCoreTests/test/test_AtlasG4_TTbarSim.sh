#!/bin/sh
#
# art-description: Run simulation outside ISF, reading ttbar events, writing HITS, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '4' \
--skipEvents '0' \
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
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 4 ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"
