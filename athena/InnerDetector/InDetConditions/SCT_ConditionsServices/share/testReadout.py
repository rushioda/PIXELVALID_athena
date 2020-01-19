import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2015-03-01-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.detdescr.SCT_setOn()
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()
DetFlags.TRT_setOff()

# ---- switch parts of ID off/on as follows
#switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3

#ToolSvc = ServiceMgr.ToolSvc

conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Chip")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Module")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/MUR")

#--------------------------------------------------------------
# Setup readout test algorithm
#--------------------------------------------------------------

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadoutTestAlg
SCT_ReadoutTestAlg = SCT_ReadoutTestAlg()

# Module type and link status
#SCT_ReadoutTestAlg.ModuleId = 143704064   # Endcap (default is barrel)
#SCT_ReadoutTestAlg.LinkOStatus = False    # Link 0 disabled (default is 'good')
#SCT_ReadoutTestAlg.Link1Status = False    # Link 1 disabled (default is 'good')

# Some possible chip configs strings
masterChip       = "10110000000001"
slaveChip        = "10110000000101"
slaveChipIn1     = "10110000010101"
slaveChipOut1    = "10110000001101"
endChip          = "10110000000111"
masterAndEndChip = "10110000000011"

# Some test module configs
defaultLink            = [masterChip] + [slaveChip]*4 + [endChip]
noEndLink              = [masterChip] + [slaveChip]*5 
endAt2Link             = [masterChip] + [slaveChip] + [endChip] + [slaveChip]*3
bypass2Link            = [masterChip] + [slaveChipIn1] + [slaveChip] + [slaveChipOut1]  + [slaveChip] + [endChip]
master3Link            = [slaveChip]*3 + [masterChip] + [slaveChip] + [endChip]
noMapped4Link          = [masterChip] + [slaveChip]*3 + [slaveChipIn1] + [endChip]
nothingTalkingTo2Link  = [masterChip] + [slaveChip] + [slaveChipIn1] + [slaveChip]*2 + [endChip]
allExcept1Link0Mod     = [masterChip] + [slaveChip]*4 + [slaveChipIn1] + [slaveChipOut1] + [masterAndEndChip] + [slaveChip]*3 + [endChip]
allLink0EndcapMod      = [masterChip] + [slaveChip]*10 + [endChip]
allLink1EndcapMod      = [slaveChip]*5 + [endChip] + [masterChip] + [slaveChip]*5 
infiniteEndcapMod      = [masterChip] + [slaveChip]*11

# Choose the configuration to test
#SCT_ReadoutTestAlg.ChipConfigurationList = defaultLink*2
#SCT_ReadoutTestAlg.ChipConfigurationList = defaultLink + endAt2Link
#SCT_ReadoutTestAlg.ChipConfigurationList = bypass2Link + defaultLink
#SCT_ReadoutTestAlg.ChipConfigurationList = master3Link + defaultLink
#SCT_ReadoutTestAlg.ChipConfigurationList =  defaultLink + noMapped4Link
#SCT_ReadoutTestAlg.ChipConfigurationList =  defaultLink + nothingTalkingTo2Link
#SCT_ReadoutTestAlg.ChipConfigurationList  = noEndLink + defaultLink
#SCT_ReadoutTestAlg.ChipConfigurationList = allExcept1Link1Mod
#SCT_ReadoutTestAlg.ChipConfigurationList = allLink0EndcapMod
#SCT_ReadoutTestAlg.ChipConfigurationList = allLink1EndcapMod
#SCT_ReadoutTestAlg.ChipConfigurationList = infiniteEndcapMod

# Chris Lester's 'blind' tests
SCT_ReadoutTestAlg.ChipConfigurationList = ["10110000010001","10110000011101","10110000011101","10110000011101","10110000001111","10110000011101",
                                            "10110000010001","10110000011101","10110000011101","10110000011101","10110000001111","10110000011101"]  
#SCT_ReadoutTestAlg.ChipConfigurationList = ["10110000010001","10110000011101","10110000011101","10110000011111","10110000001101","10110000010101",
#                                            "10110000010001","10110000011101","10110000011101","10110000011111","10110000001101","10110000010101"]  
#SCT_ReadoutTestAlg.ChipConfigurationList = ["10110000010001","10110000011101","10110000011101","10110000011111","10110000001101","10110000010101",
#                                            "10110000010001","10110000011101","10110000011101","10110000011101","10110000001111","10110000011101"]  

job+= SCT_ReadoutTestAlg

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 40341 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  = 1204216576 #found valid in db browser?
theApp.EvtMax                   = 1

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
