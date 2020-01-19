
include.block("TrigT2CaloCommon/CaloCellMasking.py")

from AthenaCommon.AppMgr import ToolSvc #if not already imported
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool, LArBadChannelMasker

ToolSvc+=LArBadChanTool("MyBadChanTool")
ToolSvc.MyBadChanTool.ReadFromASCII=False #Not necessary if you have already produced a database file
ToolSvc.MyBadChanTool.ComplementaryCoolFolder="/LAR/BadChannels/BadChannelsOnl"
#ToolSvc.MyBadChanTool.OutputLevel=VERBOSE
#ToolSvc.MyBadChanTool.OutputLevel=DEBUG

#The masker tool name must be "LArBadChannelMasker", the same as retrieved in LArCellCont.cxx
ToolSvc+=LArBadChannelMasker("LArBadChannelMasker")
ToolSvc.LArBadChannelMasker.ProblemsToMask=["unstable", "short", "highNoiseHG", "highNoiseMG","highNoiseLG", "problematicForUnknownReason"] #See LArBadChanBitPacking.cxx for the list of strings that can be used.
ToolSvc.LArBadChannelMasker.DoMasking = True #Masking can easily be turned on and off here.
#ToolSvc.LArBadChannelMasker.OutputLevel=DEBUG
ToolSvc.LArBadChannelMasker.TheLArBadChanTool = ToolSvc.MyBadChanTool #Tell the Masker to use the LArBadChanTool that was just configured.

#IOVDbSvc=theApp.service('IOVDbSvc')
#if ( svcMgr.IOVDbSvc.GlobalTag!="COMCOND-002-00" and svcMgr.IOVDbSvc.GlobalTag!="COMCOND-003-00" and svcMgr.IOVDbSvc.GlobalTag!="COMCOND-004-00" and svcMgr.IOVDbSvc.GlobalTag!="COMCOND-005-00" ):
#else:
if not conddb.folderRequested('/LAR/BadChannels/BadChannels') :
   conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannels")
conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannelsOnl")

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tIC=TileInfoConfigurator()
tIC.setupCOOL()
