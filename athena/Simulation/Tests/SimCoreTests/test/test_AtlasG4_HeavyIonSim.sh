#!/bin/sh
#
# art-description: Run simulation outside ISF, reading lead ions peripheral simulation (low multiplicity) events, writing HITS, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.HITS.pool.root

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/pbpb_Peripheral-hijing-5500.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '1' \
--skipEvents '4' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R1-2012-03-00-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '210184' \
--physicsList 'FTFP_BERT' \
--postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:SimulationJobOptions/postInclude.HijingPars.py' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 1 ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"
