###############################################################
#
# Job options file to run simple algorithm computing noise per cell
#
#==============================================================
from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')

DetFlags.detdescr.all_setOn()
DetFlags.Muon_setOff()

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "PartPropSvc/PartPropSvc.py" )
#
# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
#
# Pool input
#
PoolESDInput= [
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_900GeV/physics_CosmicCalo/00176051/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81._lb0002._SFO-ALL._0001.1",
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_900GeV/physics_CosmicCalo/00176051/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81._lb0003._SFO-ALL._0001.1",
"rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data11_900GeV/physics_CosmicCalo/00176051/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81/data11_900GeV.00176051.physics_CosmicCalo.recon.ESD.x81._lb0004._SFO-ALL._0001.1",
]
svcMgr.EventSelector.InputCollections = PoolESDInput
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = PoolESDInput

# the Tile, LAr and Calo detector description package
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-16-00-00'


from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" ) 
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
theADC2MeVTool = LArADC2MeVToolDefault()
ToolSvc += theADC2MeVTool


from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theNoiseTool = CaloNoiseToolDefault()
ToolSvc += theNoiseTool

# Trigger decision tool

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']
# set up trigger config service 
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt 


from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloCellNoiseAlg
myNoise = CaloCellNoiseAlg("CaloCellNoiseAlg")
myNoise.doMC = False
myNoise.readNtuple = False
myNoise.doFit = True
myNoise.ADC2MeVTool = theADC2MeVTool
# uncomment to activate trigger decision-  example below is using L1_RD1_EMPTY, checked against 16.6.1 release
myNoise.TrigDecisionTool = tdt
myNoise.TriggerChain = "L1_RD1_EMPTY"
myNoise.noiseTool = theNoiseTool
myNoise.doLumiFit = False
myNoise.noiseTool = theNoiseTool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += myNoise


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
myNoise.OutputLevel        = INFO

if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='cellnoise.root' OPT='RECREATE'"];

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 20000
theApp.EvtSel = "EventSelector"

