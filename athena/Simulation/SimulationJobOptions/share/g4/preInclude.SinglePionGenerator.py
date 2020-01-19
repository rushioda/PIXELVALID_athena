print 'SINGLE PARTICLE GENERATOR'

## Run ParticleGun
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

import ParticleGun as PG
from G4AtlasApps.SimFlags import simFlags
pg = PG.ParticleGun(randomSvcName=simFlags.RandomSvc.get_Value(), randomStream="SINGLE")
pg.sampler.pid = PG.CyclicSeqSampler([-211,211])
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=50000, eta=[-4,4])
topSeq += pg

try:
    from GeneratorModules.GeneratorModulesConf import CopyEventWeight
    topSeq += CopyEventWeight(TruthCollKey="GEN_EVENT")
except:
    include("G4AtlasApps/fragment.SimCopyWeights.py")
