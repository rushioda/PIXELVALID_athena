#!/bin/sh
#
# art-description: Run simulation outside ISF, reading Z->tau tau events, modifying the Neutron timing cut, writing HITS, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.HITS.pool.root

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/A3_Ztautau_filter-pythia6-7000.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '5' \
--skipEvents '0' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'FTFP_BERT' \
--preInclude 'SimulationJobOptions/preInclude.G4SignalCavern.py' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
--imf False

echo  "art-result: $? simulation"

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 5 ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"
