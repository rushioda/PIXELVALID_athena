###############################################################
#
# Job options file 1
#
#==============================================================
#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()


#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]

# change DB server
# LArCondCnvDbServer = 'db1.usatlas.bnl.gov'

# Other LAr related
# include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArCondCnv/LArCondCnv_G4_jobOptions.py" )

# ATLAS
# include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )



# LArCondCnv
#  theApp.TopAlg = [ "LArIOV_TestAlg" ]; # test algorithm
#/--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
AthenaSealSvc.CheckDictionary = True
AthenaSealSvc.CheckDictAtInit = True

theApp.Dlls += [ "EventSelectorAthenaPool","LArCondAthenaPoolPoolCnv" ]

ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

CondProxyProvider = Service( "CondProxyProvider" )
# CondProxyProvider.InputCollection = "oracle://devdb/hma"
CondProxyProvider.InputCollection = "LArIdMap.pool.root"

EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

include ("LArRawConditions/LArRawConditionsDict_joboptions.py")

# Exercise writing the objects to NOVA (no IOV).
theApp.OutStream     =["StreamDet"]
theApp.OutStreamType ="AthenaOutputStream"
StreamDet = Algorithm("StreamDet")

theApp.EvtMax=1
StreamDet.StoreName="DetectorStore"
StreamDet.EvtConversionSvc="AthenaPoolCnvSvc"

StreamDet.ItemList += ["LArOnOffIdMap#*"]
StreamDet.ItemList += ["LArFebRodMap#*"]
StreamDet.ForceRead = True;
StreamDet.OutputFile = "LArIdMap2.pool.root"

# StreamDet.OutputFile =  "oracle://devdb/hma"

ToolSvc=Service("ToolSvc")
ToolSvc.LArRoI_Map.Print=False

