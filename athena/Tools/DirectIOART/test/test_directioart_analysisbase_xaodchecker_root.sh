#!/bin/bash

# art-description: DirectIOART AnalysisBase xAODChecker protocol=ROOT
# art-type: grid
# art-include: 21.2/AnalysisBase

set -e

xAODChecker root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/data18_13TeV/e6/c2/DAOD_MUON0.14316152._000001.pool.root.1

echo "art-result: $? DirectIOART_AnalysisBase_xAODChecker_protocol_ROOT"

PFC_FILE=PoolFileCatalog.xml
touch $PFC_FILE
echo "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" >> $PFC_FILE
echo "<!-- Edited By POOL -->" >> $PFC_FILE
echo "<!DOCTYPE POOLFILECATALOG SYSTEM \"InMemory\">" >> $PFC_FILE
echo "<POOLFILECATALOG>" >> $PFC_FILE
echo "" >> $PFC_FILE
echo "</POOLFILECATALOG>" >> $PFC_FILE