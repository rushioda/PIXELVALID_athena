#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantinoMapping.
#		It can be run using athena.py
#
__version__="$Revision: 757321 $"
#==============================================================


#--- Algorithm sequence ---------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL) ----
#from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 20000

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

import random
import time

### pass arguments with athena -c "..." ...jobOptions.py:
if 'myMomentum' not in dir() :
    myMomentum = 10000

if 'myRandomOffset' not in dir() :
    myRandomOffset = int(random.uniform(0,time.time())*0.001 )

if 'myRandomSeed1' not in dir() :
    myRandomSeed1 = int(random.uniform(0,time.time()))

if 'myRandomSeed2' not in dir() :
    myRandomSeed2 = int(random.uniform(0,time.time()))

if 'myMaxEvent' not in dir() :
    myMaxEvent = 10

if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    myGeo = 'ATLAS-R1-2012-02-01-00_VALIDATION'

print 'Random seeds and offset as calcluated by jobOptions ', myRandomSeed1, ' ', myRandomSeed2, ' offset - ', myRandomOffset


# Set everything to ATLAS
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# the global flags
globalflags.ConditionsTag = 'OFLCOND-SIM-00-00-00'
print globalflags.ConditionsTag


#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()   ### is this necessary?
athenaCommonFlags.PoolHitsOutput = 'Hits.pool.root'
athenaCommonFlags.EvtMax = 100

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags() # Going to use an ATLAS layout
simFlags.SimLayout = myGeo
simFlags.EventFilter.set_Off()

myMinEta = -6.0
myMaxEta =  6.0

myPDG    = 999   # 999 = Geantinos, 13 = Muons

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 100

import ParticleGun as PG
pg = PG.ParticleGun()
pg.sampler.pid = 999
pg.sampler.mom = PG.EEtaMPhiSampler(energy=10000, eta=[-6.,6.])
topSeq += pg

simFlags.RandomSeedOffset = myRandomOffset
simFlags.RandomSeedList.addSeed( "SINGLE", myRandomSeed1, myRandomSeed2 )

from RngComps.RngCompsConf import AtRndmGenSvc 
myAtRndmGenSvc = AtRndmGenSvc()
myAtRndmGenSvc.Seeds = ["SINGLE "+str(myRandomSeed1)+" "+str(myRandomSeed2) ] 
myAtRndmGenSvc.OutputLevel 	= VERBOSE
myAtRndmGenSvc.EventReseeding   = False
ServiceMgr += myAtRndmGenSvc

## add the material step recording action
SimFlags.OptionalUserActionList.addAction('G4UA::MaterialStepRecorderTool',['Run','Event','Step'])

############### The Material hit collection ##################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# --- check dictionary
ServiceMgr.AthenaSealSvc.CheckDictionary   = True
# --- commit interval (test)
ServiceMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10
MaterialStream              = AthenaPoolOutputStream ( 'MaterialStream' )
MaterialStream.OutputFile   =   "MaterialStepFile.root"
MaterialStream.ItemList    += [ 'Trk::MaterialStepCollection#*']

##############################################################

include("G4AtlasApps/G4Atlas.flat.configuration.py")

# Add the beam effects algorithm
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Populate alg sequence
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)

#--- End jobOptions.GeantinoMapping.py file  ------------------------------

