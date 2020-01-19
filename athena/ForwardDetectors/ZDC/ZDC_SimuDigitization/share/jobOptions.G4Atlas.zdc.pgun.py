
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.bpipe_setOn()
DetFlags.ZDC_setOn()
DetFlags.Truth_setOn()

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-SDR-BS7T-05-14"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = "atlasG4.hits.pool.root"
athenaCommonFlags.EvtMax = 3

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout='ATLAS-GEO-18-01-00'
simFlags.EventFilter.set_Off()
simFlags.MagneticField.set_Off()
simFlags.ForwardDetectors.set_On()
simFlags.ForwardDetectors=2

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()

import AthenaCommon.AtlasUnixGeneratorJob
spgorders = ['pdgcode: constant 2112',
             'vertX:   flat -1.0 1.0',
             'vertY:   flat -1.0 1.0',
             'vertZ:   constant -139500.0',
             't:       constant 0.0',
             'eta:     constant -100.0',
             'phi:     flat  0 6.28318',
             'e:       constant 1360000'
             ]

from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
topSeq += ParticleGenerator()
topSeq.ParticleGenerator.orders = sorted(spgorders)

include("G4AtlasApps/G4Atlas.flat.configuration.py")

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

