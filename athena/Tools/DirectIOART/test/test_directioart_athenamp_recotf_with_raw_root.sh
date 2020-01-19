#!/bin/bash

# art-description: DirectIOART AthenaMP Reco_tf.py inputFile:RAW protocol=ROOT
# art-type: grid
# art-output: *.pool.root
# art-include: 21.0/Athena
# art-include: master/Athena

set -e

export ATHENA_PROC_NUMBER=2
Reco_tf.py --AMI q431 --inputBSFile root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/data15_13TeV/71/42/data15_13TeV.00284285.physics_Main.daq.RAW._lb0856._SFO-1._0003.data --outputESDFile art.pool.root

echo "art-result: $? DirectIOART_AthenaMP_RecoTF_inputBS_protocol_ROOT"