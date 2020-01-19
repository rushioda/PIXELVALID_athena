#!/bin/sh
#
# art-description: Run cosmics simulation outside ISF from the athena command-line, generating events on-the-fly
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid

athena G4AtlasApps/jobOptions.G4Cosmic.py

echo  "art-result: $? simulation"

# TODO This is a regression test I think. 
ArtPackage=$1
ArtJobName=$2
art.py compare grid ${ArtPackage} ${ArtJobName}

echo  "art-result: $? regression"
