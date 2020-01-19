import commands

###########################################################################
#
# <pavol@mail.cern.ch>
#
# jobOptions to reconstruction one or more ramp run from data per calibration line
#
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")
include("RecExCommission/GetInputFiles.py")

#######################################################
#       Run properties
#######################################################

if not 'SubDet' in dir():
   SubDet = "EndCap"

if not 'RunNumberList' in dir():
   RunNumberList = [ '0018660' ]
   
if not 'AllWavesPerCh' in dir():
        AllWavesPerCh      =  True

if not 'MaxCalLinePerCh' in dir():
        MaxCalLinePerCh = 4

if not 'FilePrefix' in dir():
   if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Ramp"
   else :
      FilePrefix = "data*"
     
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2008/"+RunNumberList[0]
   
if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"     

if not 'FullFileName' in dir():
   if (int(RunNumberList[0]))<99800 :
      Trigger = "*"+Partition
   else :
      Trigger = "calibration_LArElec-Ramp"+".*"+Partition   
   
   FullFileName = []
   for RunNumber in RunNumberList :
       FullFileName+=GetInputFilesFromTokens(InputDir,int(RunNumber),FilePrefix,Trigger)

if not 'GainList' in dir():
   GainList = [ "HIGH", "MEDIUM", "LOW" ]
   
if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"
        
if not 'ChannelSelection' in dir():
   ## to read all
   ChannelSelection = " "
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>" 
   #ChannelSelection = "<channelSelection>0,0:1000</channelSelection>"
   
if not 'runAccumulator' in dir(): 
   runAccumulator = False # averaged mode

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

#######################################################
#                Monitoring properties
#######################################################

if not 'doMonitoring' in dir():
   doMonitoring = False

if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = False     

if not 'online' in dir():
   online = False

#######################################################
#                Ramp properties                      #
#######################################################

if not 'isHEC' in dir():
   isHEC= True

if not 'HECKey' in dir():
   HECKey="LArHEC_PAmap"

if not 'PeakOF' in dir():
   PeakOF = True   # OF reconstruction
   
if not 'CorrectBias' in dir():
   CorrectBias = False   # Set whether to correct for bias - if True, will look for peds from COOL
	
if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = False # Xtalk correction for strips

if not "ADCSaturation" in dir():
   ADCsaturation = 4095 # Set to 0 if you want to keep saturating pulses
   
if not 'KeyOutput' in dir():  
   KeyOutput = "LArRamp" # Key of LArRampComplete object in DetStore

if AllWavesPerCh and (not 'KeyOutputSplitted' in dir() ):
      KeyOutputSplitted = []
      for i in range(0, MaxCalLinePerCh):
         KeyOutputSplitted.append(KeyOutput+str(i+1)) 

if not 'SaveRawRamp' in dir():
   SaveRawRamp = True
  
if not 'SaveAverages' in dir():
   SaveAverages = False # Add averages to ROOT file

if not 'CorrectBadChannels' in dir():
   CorrectBadChannels=False
   
#######################################################
#       Ramp output name
#######################################################

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = int(RunNumberList[0])
   
if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd   

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "COOLOFL_LAR/CONDBR2"

## HEC map
if not 'ReadHECMapFromCOOL' in dir():
   ReadHECMapFromCOOL = True
	
if not 'InputHECMapPoolFileDir' in dir():
   InputHECMapPoolFileDir  = commands.getoutput("pwd")
   
if not 'InputHECMapPoolFileName' in dir():
   InputHECMapPoolFileName = "LArHECMap.pool.root"   
   
## Pedestal
if not 'ReadPedFromCOOL' in dir():
   ReadPedFromCOOL = True
	
#if not 'PedLArCalibFolderTag' in dir():
#   PedLArCalibFolderTag = LArCalib_Flags.tagSuffix      

if not 'PedRunNumber' in dir():
   PedRunNumber = "1005020_1005021_1005022"

if not 'InputPedPoolFileDir' in dir():
   InputPedPoolFileDir  = commands.getoutput("pwd")
   
if not 'InputPedPoolFileName' in dir():
   InputPedPoolFileName = "LArPedestal_" + str(PedRunNumber) +"_"+Partition.replace("*","")+ ".pool.root"

## OFC Cali
if not 'ReadOFCFromCOOL' in dir():
   ReadOFCFromCOOL = True
	
#if not 'OFCLArCalibFolderTag' in dir():
#   OFCLArCalibFolderTag = LArCalib_Flags.tagSuffix

if not 'OFCRunNumber' in dir():
   OFCRunNumber = "30950"

if not 'InputOFCPoolFileDir' in dir():
   InputOFCPoolFileDir  = commands.getoutput("pwd")

if not 'InputOFCPoolFileName' in dir():
   if PeakOF :
      InputOFCPoolFileName = "LArOFCCali_" + str(PedRunNumber) +"_"+Partition.replace("*","")+ ".pool.root"
   if ( PeakOF and StripsXtalkCorr ) :
      InputOFCPoolFileName = "LArOFCCali_StripsXtalkCorr_" + str(OFCRunNumber) +"_"+Partition.replace("*","")+ ".pool.root"

## Output   
if not 'LArCalibFolderOutputTag' in dir():
   rs=FolderTagResover()
   LArCalibFolderOutputTag = rs.getFolderTagSuffix(LArCalib_Flags.LArRampFolder)      

if not 'OutputRampRootFileDir' in dir():
   OutputRampRootFileDir = commands.getoutput("pwd")
   
if not 'OutputRampPoolFileDir' in dir():
   OutputRampPoolFileDir = commands.getoutput("pwd")

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)
	
if not 'BaseFileName' in dir():
   BaseFileName = "LArRamp"
   
   for RunNumber in RunNumberList :
      BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","")
   
   if StripsXtalkCorr:
      BaseFileName += "_StripsXtalkCorr"
   if PeakOF:
      BaseFileName += "_OFC"	   

if not 'OutputRampRootFileName' in dir():
   OutputRampRootFileName = BaseFileName+".root"
   
if not 'OutputRampPoolFileName' in dir():
   OutputRampPoolFileName = BaseFileName+".pool.root"

if not 'OutputObjectSpecRamp' in dir():
   if ( AllWavesPerCh ) :
      OutputObjectSpecRamp = []
      OutputObjectSpecRamp.append("LArRampComplete#"+KeyOutputSplitted[i]+"#"+LArCalib_Flags.LArRampFolder)
   else:   
      OutputObjectSpecRamp = ["LArRampComplete#"+KeyOutput+"#"+LArCalib_Flags.LArRampFolder]
   OutputObjectSpecTagRamp = LArCalibFolderTag(LArCalib_Flags.LArRampFolder,LArCalibFolderOutputTag)
   
if ( ReadPedFromCOOL ):
   if 'InputPedSQLiteFile' in dir():
      InputDBConnectionPed = DBConnectionFile(InputPedSQLiteFile)
   else:
      InputDBConnectionPed = DBConnectionCOOL

if ( PeakOF and ReadOFCFromCOOL ):
   if 'InputOFCSQLiteFile' in dir():
      InputDBConnectionOFC = DBConnectionFile(InputOFCSQLiteFile)
   else:
      InputDBConnectionOFC = DBConnectionCOOL

if ( ReadHECMapFromCOOL ):
   if 'InputHECMapSQLiteFile' in dir():
      InputDBConnectionHECMap = DBConnectionFile(InputHECMapSQLiteFile)
   else:
      InputDBConnectionHECMap = DBConnectionCOOL

## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      #InputDBConnectionBadChannel = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;user=ATLAS_COOL_READER"
      InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"
      
#######################################################################################
#                                print summary                                        #
#######################################################################################

RampLog = logging.getLogger( "RampLog" )
RampLog.info( " ======================================================== " )
RampLog.info( " ***                 LAr Ramp summary                  *** " )
RampLog.info( " ======================================================== " )
RampLog.info( " RunNumber                          = "+str(RunNumberList) )
RampLog.info( " SubDetector                        = "+SubDet )
RampLog.info( " Partition                          = "+Partition )
RampLog.info( " Type                               = Ramp " )
for i in range(len(FullFileName)):
   RampLog.info( " FullFileName                       = "+FullFileName[i] )
if ( ReadPedFromCOOL ):
   RampLog.info( " InputDBConnectionPed               = "+InputDBConnectionPed )
else :
   RampLog.info( " InputPedPoolFileName               = "+InputPedPoolFileName )
if 'PedLArCalibFolderTag' in dir() :   
   RampLog.info( " PedLArCalibFolderTag               = "+PedLArCalibFolderTag )
if ( PeakOF and ReadOFCFromCOOL ):
   RampLog.info( " InputDBConnectionOFC               = "+InputDBConnectionOFC )
elif ( PeakOF) :
   RampLog.info( " InputOFCPoolFileName               = "+InputOFCPoolFileName )
if 'OFCLArCalibFolderTag' in dir() :
   RampLog.info( " OFCLArCalibFolderTag               = "+OFCLArCalibFolderTag )
RampLog.info( " OutputRampRootFullFileName         = "+OutputRampRootFileDir+"/"+OutputRampRootFileName )
RampLog.info( " OutputRampPoolFullFileName         = "+OutputRampPoolFileDir+"/"+OutputRampPoolFileName )
#RampLog.info( " OutputObjectSpecRamp               = "+OutputObjectSpecRamp )
RampLog.info( " OutputObjectSpecTagRamp            = "+OutputObjectSpecTagRamp )
RampLog.info( " IOVBegin                           = "+str(IOVBegin) )
RampLog.info( " IOVEnd                             = "+str(IOVEnd) )
RampLog.info( " LArCalibOutputDB                   = "+OutputDB )
RampLog.info( " ======================================================== " )
#######################################################################################

include ("LArConditionsCommon/LArMinimalSetup.py")

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   RampLog.info( "No FullFileName! Please give a FullFileName list." )
   theApp.exit(-1)

else :   
   theByteStreamInputSvc.FullFileName=FullFileName
   
theByteStreamInputSvc.MaxBadEvents=0

##############################################################################################
#                                                                                            #
# FullFileName is already the list of files compute by the fonction GetInputFilesFromTokens  #
#                                                                                            #
# maybe useful one day                                                                       #
#                                                                                            #
#else                                                                                        #
#   theByteStreamInputSvc.FullFileName=OneFileName                                           #
#   for i in range(len(theByteStreamInputSvc.FullFileName)):                                 #
#      theByteStreamInputSvc.NumFile+=[10000]                                                #
##############################################################################################

###############################################################
#                                                             #
#                To read BS for one or more FT                #
#                                                             #
###############################################################

## For splitting the delay runs, there are three new jobOptions.
## All three are vectors of integers
#################################################################

from LArByteStream.LArByteStreamConf import LArRodDecoder
svcMgr.ToolSvc += LArRodDecoder()

#ToolSvc.LArRodDecoder.BEPreselection     = [0]                                                   ## : [Barrel=0,Endcap=1]
#ToolSvc.LArRodDecoder.PosNegPreselection = [1]                                                   ## : [C-side (negative eta)=0, A-side (positive eta)=1]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]               ## : first half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31]     ## : second half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,4,7,8,11,12,13,14,17,18,19,20,23,24]             ## : [EMEC Standard feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [2,9,15,21]                                           ## : [EMEC Special feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [3,10,16,22]                                          ## : [HEC feedthrough numbers]  (note: slots 1&2 are EMEC slots)
#ToolSvc.LArRodDecoder.FTNumPreselection  = [6]                                                   ## : [FCAL feedthrough number]

      
 
###########################################################################
#                                                                         #
#                          Accumulator                                    #
#                                                                         #
###########################################################################

if ( runAccumulator ) :
   # this is a OLD jobOptions which can maybe work but only for the barrel                       #
   # can be used as a skeleton if needed but                                                     #
   # need to be updated for the barrel and the patterns for EMEC, HEC and FCAL need to be added   #
   include("LArCalibProcessing/LArCalib_CalibrationPatterns.py")

else :
   theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
   theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/HIGH"  ]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/MEDIUM"]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/LOW"   ]

## This algorithm verifies that no FEBs are dropping out of the run
## If it finds corrupt events, it breaks the event loop and terminates the job rapidly
include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")       
#from LArCalibDataQuality.LArCalibDataQualityConf import LArBadEventCatcher
#theLArBadEventCatcher=LArBadEventCatcher()
#theLArBadEventCatcher.CheckAccCalibDigitCont=True
#theLArBadEventCatcher.CheckBSErrors=True
#theLArBadEventCatcher.KeyList=GainList
#theLArBadEventCatcher.StopOnError=False
#topSequence+=theLArBadEventCatcher    

##########################################################################
#                                                                        #
#                         Ramp run reconstruction                        #
#                                                                        #
##########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

include ("LArCalibProcessing/LArCalib_BadChanTool.py")

if not 'InputBadChannelSQLiteFile' in dir():
   RampLog.info( "Read Bad Channels from Oracle DB")
else :   
   RampLog.info( "Read Bad Channels from SQLite file") 

if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

if 'MissingFEBsLArCalibFolderTag' in dir() :
   MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,MissingFEBsLArCalibFolderTag)   
   conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB   

svcMgr.PoolSvc.ReadCatalog += ["xmlcatalog_file:/afs/cern.ch/user/l/larcalib/w0/stableConds/PoolCat_stable.xml",
                               "prfile:poolcond/PoolCat_oflcond.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_diskbuffer_afs.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond09_data.000001.lar.COND_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_cond08_data.000001.lar.COND_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000005.lar_conditions.recon.pool.v0000_castor.xml",
                               "xmlcatalog_file:/afs/cern.ch/atlas/conditions/poolcond/catalogue/fragments/PoolCat_comcond.000006.lar_conditions.recon.pool.v0000_castor.xml"]

if ( doLArCalibDataQuality  ) :
   ## The reference is the Oracle DB
   conddb.addFolder("",LArCalib_Flags.LArRampFolder+"<key>LArRampRef</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)

#if (CorrectBadChannels or StripsXtalkCorr):
#   conddb.addFolder("LAR","/LAR/BadChannels/BadChannels")
#   conddb.addFolder("LAR","/LAR/BadChannels/MissingFEBs")
   
if (isHEC):
   if ( ReadHECMapFromCOOL ):
      if not 'InputHECMapSQLiteFile' in dir():
         svcMgr.PoolSvc.ReadCatalog += [ "prfile:poolcond/PoolCat_comcond_castor.xml", "prfile:poolcond/PoolCat_comcond.xml" ]
         RampLog.info( "Read HECMap from Oracle DB" )
      else :
         RampLog.info( "Read HEC map from SQLite file" )

   if ( ReadHECMapFromCOOL ):
      HECMapFolder  = "/LAR/ElecCalibOfl/HecPAMap"
      if "HECMapTagSpec" in dir():
         conddb.addFolder("",HECMapFolder+"<tag>"+HECMapTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionHECMap+"</dbConnection>")
      else :
         conddb.addFolder("",HECMapFolder+"<dbConnection>"+InputDBConnectionHECMap+"</dbConnection>")

   else:
      if 'InputHECMapPoolFileName' in dir():
         RampLog.info( "Read HECMap form POOL file" )
         PoolFileList += [ InputHECMapPoolFileDir +"/"+ InputHECMapPoolFileName ]   
      else:
         RampLog.info( "No PoolFileList found! Please list the POOL files containing HECMap or read from COOL." )
         theApp.exit(-1)    

if (CorrectBias or PeakOF or StripsXtalkCorr):
   if ( ReadPedFromCOOL or ReadOFCFromCOOL ):
      if not 'InputPedSQLiteFile' in dir():
         svcMgr.PoolSvc.ReadCatalog += [ "prfile:poolcond/PoolCat_comcond_castor.xml", "prfile:poolcond/PoolCat_comcond.xml" ]
         RampLog.info( "Read Pedestal from Oracle DB" )
      else :  
         RampLog.info( "Read Pedestal from SQLite file" )
          
      if not 'InputOFCSQLiteFile' in dir():
         svcMgr.PoolSvc.ReadCatalog += [ "prfile:poolcond/PoolCat_comcond_castor.xml", "prfile:poolcond/PoolCat_comcond.xml" ] 
         RampLog.info( "Read OFC Cali from Oracle DB" )
      else :
         RampLog.info( "Read OFC Cali from SQLite file" )
         
if ( ReadPedFromCOOL ):
   if (CorrectBias or StripsXtalkCorr or PeakOF):
      PedestalFolder  = LArCalib_Flags.LArPedestalFolder
      if 'PedLArCalibFolderTag' in dir() :
         PedestalTagSpec = LArCalibFolderTag(PedestalFolder,PedLArCalibFolderTag)
         conddb.addFolder("",PedestalFolder+"<tag>"+PedestalTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+ChannelSelection)
      else :
         conddb.addFolder("",PedestalFolder+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+ChannelSelection)

else:
   if 'InputPedPoolFileName' in dir():
      RampLog.info( "Read Pedestal from POOL file" )
      PoolFileList += [ InputPedPoolFileDir +"/"+ InputPedPoolFileName ]   
   else:
      RampLog.info( "No PoolFileList found! Please list the POOL files containing Pedestal or read from COOL." )
      theApp.exit(-1)

   
if ( ReadOFCFromCOOL ):
   if PeakOF:
      if ( not StripsXtalkCorr ):
         CaliOFCFolder  = LArCalib_Flags.LArOFCCaliFolder
      else:
         CaliOFCFolder  = LArCalib_Flags.LArOFCCaliFolderXtlk
      if 'OFCLArCalibFolderTag' in dir() :
         if AllWavesPerCh:
            for i in range(0, MaxCalLinePerCh):
               CaliOFCTagSpec = LArCalibFolderTag(CaliOFCFolder+str(i),OFCLArCalibFolderTag)
               conddb.addFolder("",CaliOFCFolder+str(i)+"<tag>"+CaliOFCTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionOFC+"</dbConnection>"+ChannelSelection)
         else:      
               CaliOFCTagSpec = LArCalibFolderTag(CaliOFCFolder,OFCLArCalibFolderTag)
               conddb.addFolder("",CaliOFCFolder+"<tag>"+CaliOFCTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionOFC+"</dbConnection>"+ChannelSelection)
      else :
         conddb.addFolder("",CaliOFCFolder+"<dbConnection>"+InputDBConnectionOFC+"</dbConnection>"+ChannelSelection)

else:
   if 'InputOFCPoolFileName' in dir():
         RampLog.info( "Read OFC Cali form POOL file" )
         PoolFileList += [ InputOFCPoolFileDir +"/"+ InputOFCPoolFileName ]   
   else:
         RampLog.info( "No PoolFileList found! Please list the POOL files containing OFC or read from COOL." )
         theApp.exit(-1)

if ( len(PoolFileList)>0 ):
      
   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

if ( StripsXtalkCorr ) :
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   from LArCalibUtils.LArCalibUtilsConf import LArStripsCrossTalkCorrector
   theLArStripsCrossTalkCorrector = LArStripsCrossTalkCorrector()
   theLArStripsCrossTalkCorrector.KeyList = GainList
   theLArStripsCrossTalkCorrector.ADCsaturation = ADCsaturation
   theLArStripsCrossTalkCorrector.NoXtalkCorr=LArBadChannelMasker("NoXtalkCorr",
                                                               DoMasking=True,
                                                               ProblemsToMask=["deadReadout","deadPhys","deadCalib","almostDead"]
                                                               )
   theLArStripsCrossTalkCorrector.DontUseForXtalkCorr=LArBadChannelMasker("DontUseForXtalkCorr",
                                                                       DoMasking=True,
                                                                       ProblemsToMask=["short","peculiarCalibrationLine"]
                                                                       )
   theLArStripsCrossTalkCorrector.AcceptableDifference=25.0 #in per-cent
   topSequence +=theLArStripsCrossTalkCorrector

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
ToolSvc+=theLArBadChannelTool

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
theLArRCBMasker.DoMasking=True
theLArRCBMasker.ProblemsToMask=[
   "deadCalib","deadReadout","deadPhys","almostDead","short" 
   ]
ToolSvc+=theLArRCBMasker

from LArCalibUtils.LArCalibUtilsConf import LArRampBuilder
#Bad-channel mask used by the LArRampBuilder and the Ramp-patcher
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
theLArRCBMasker.DoMasking=True
theLArRCBMasker.ProblemsToMask=[
   "deadCalib","deadReadout","deadPhys","almostDead","short"
      ]
ToolSvc+=theLArRCBMasker

if not PeakOF:
   from LArRecUtils.LArRecUtilsConf import LArParabolaPeakRecoTool
   theLArParabolaPeakRecoTool=LArParabolaPeakRecoTool()
   theLArParabolaPeakRecoTool.correctBias = CorrectBias
   ToolSvc+=theLArParabolaPeakRecoTool

if ( AllWavesPerCh ) :
   ContNameSplitted = []
   for i in range(0, MaxCalLinePerCh):
          ContNameSplitted.append("CalLine"+str(i))
   from LArCalibUtils.LArCalibUtilsConf import LArAccumulatedCalibDigitContSplitter
   LArAccCaDigContSplitter = []
   for j in range(0,len(GainList)):
      ThisKeyOut = []
      for i in range(0, MaxCalLinePerCh):
         ThisKeyOut.append(ContNameSplitted[i]+GainList[j])
      topSequence+=LArAccumulatedCalibDigitContSplitter("Splitter"+GainList[j]) 
      exec 'LArAccCaDigContSplitter.append( topSequence.Splitter%(fn)s )' % {'fn' : GainList[j]}
      LArAccCaDigContSplitter[j].KeyList         = [ GainList[j] ]
      LArAccCaDigContSplitter[j].KeyOutputList = ThisKeyOut
      LArAccCaDigContSplitter[j].NumberSplitted  = MaxCalLinePerCh
      LArAccCaDigContSplitter[j].UseDacAndIsPulsedIndex = True
      LArAccCaDigContSplitter[j].OutputLevel = ERROR

   theLArRampBuilder = []
   for i in range(0, MaxCalLinePerCh):
      ThisKeyList = []
      for j in range(0,len(GainList)):
         ThisKeyList.append(ContNameSplitted[i]+GainList[j])

      topSequence+=LArRampBuilder("LArRampBuilder"+str(i+1))
      exec 'theLArRampBuilder.append( topSequence.LArRampBuilder%(fn)s )' % {'fn' :i+1}
      theLArRampBuilder[i].KeyList      = ThisKeyList
      theLArRampBuilder[i].SubtractDac0 = False
      theLArRampBuilder[i].DAC0         = -1
      theLArRampBuilder[i].StoreRawRamp = SaveRawRamp
      theLArRampBuilder[i].StoreRecRamp = True
      theLArRampBuilder[i].Polynom      = 1
      theLArRampBuilder[i].isHEC        = isHEC
      theLArRampBuilder[i].HECKey       = HECKey
      if ( PeakOF) :
            theLArRampBuilder[i].RecoType     = "OF"
            from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
            exec 'theLArOFPeakRecoTool%(fn)s=LArOFPeakRecoTool("LArOFPeakRecoTool%(fg)s")' % {'fn' :i+1, 'fg' : i+1}
            exec 'svcMgr.ToolSvc+=theLArOFPeakRecoTool%(fn)s' % {'fn' :i+1}
            theLArRampBuilder[i].PeakOFTool   = "LArOFPeakRecoTool/LArOFPeakRecoTool"+str(i+1)
            exec 'svcMgr.ToolSvc.LArOFPeakRecoTool%(fn)s.KeyShape="" ' % {'fn' :i+1} # Don't use shape here
            exec 'svcMgr.ToolSvc.LArOFPeakRecoTool%(fn)s.KeyOFC="LArOFC%(fn)s" ' % {'fn' :i+1} 
#            exec 'svcMgr.ToolSvc.LArOFPeakRecoTool%(fn)s.Iterate=IterateOFC ' % {'fn' :i+1}
            exec 'svcMgr.ToolSvc.LArOFPeakRecoTool%(fn)s.OutputLevel = WARNING ' % {'fn' :i+1}
      else :
            theLArRampBuilder[i].RecoType     = "Parabola"
      
      theLArRampBuilder[i].RampRange    = 3600  # Check on the raw data ADC sample before ped subtraction
                                   # and pulse reconstruction to include point in fit
      theLArRampBuilder[i].correctBias  = CorrectBias
      theLArRampBuilder[i].ConsecutiveADCs = 0;
#      theLArRampBuilder[i].Iterate=IterateOFC
      theLArRampBuilder[i].minDAC       = 10      # minimum DAC value to use in fit
      theLArRampBuilder[i].KeyOutput    = KeyOutputSplitted[i]
      theLArRampBuilder[i].BadChannelMask = theLArRCBMasker
      theLArRampBuilder[i].DeadChannelCut = -9999
      theLArRampBuilder[i].GroupingType = "ExtendedSubDetector"
      theLArRampBuilder[i].OutputLevel = ERROR
      theLArRampBuilder[i].BadChannelMask=theLArRCBMasker

else:

   theLArRampBuilder = LArRampBuilder()
   theLArRampBuilder.KeyList      = GainList
   theLArRampBuilder.SubtractDac0 = False

   if ( PeakOF ):
     theLArRampBuilder.RecoType = "OF"
     from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
     theLArOFPeakRecoTool=LArOFPeakRecoTool()
     theLArOFPeakRecoTool.UseShape = False
     ToolSvc+=theLArOFPeakRecoTool
   else :
     theLArRampBuilder.RecoType = "Parabola"

   # As the first ramp point no longer is taken at DAC=0
   # this point should not have a special place.
   theLArRampBuilder.DAC0         = -1
   theLArRampBuilder.StoreRawRamp = SaveRawRamp
   theLArRampBuilder.StoreRecRamp = True
   theLArRampBuilder.Polynom      = 1    
   theLArRampBuilder.RampRange    = 3600 # Check on the raw data ADC sample before ped subtraction
                                      # and pulse reconstruction to include point in fit
   theLArRampBuilder.correctBias  = CorrectBias
   theLArRampBuilder.ConsecutiveADCs = 0;
   theLArRampBuilder.minDAC = 10      # minimum DAC value to use in fit
   theLArRampBuilder.KeyOutput = KeyOutput
   theLArRampBuilder.BadChannelMask = theLArRCBMasker
   theLArRampBuilder.DeadChannelCut = -9999
   theLArRampBuilder.GroupingType = GroupingType
   theLArRampBuilder.BadChannelMask=theLArRCBMasker

   if ( isHEC ) :
     theLArRampBuilder.isHEC = isHEC
     theLArRampBuilder.HECKey = HECKey
   
   topSequence+=theLArRampBuilder


######################################################################
#                                                                    #
#                          Correction                                #
#                                                                    #
######################################################################

if CorrectBadChannels:
   from LArCalibUtils.LArCalibUtilsConf import LArCalibPatchingAlg_LArRampComplete_
   theLArRampPatcher=LArCalibPatchingAlg_LArRampComplete_("LArRampPatcher")
   theLArRampPatcher.ContainerKey=KeyOutput
   theLArRampPatcher.PatchMethod="PhiAverage"
   
   theLArRampPatcher.MaskingTool=theLArRCBMasker
   topSequence+=theLArRampPatcher



######################################################################
#                                                                    #
#                          Output                                    #
#                                                                    #
######################################################################

if ( doLArCalibDataQuality  ) :
   
   from LArCalibDataQuality.LArCalibDataQualityConf import LArRampValidationAlg
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   theLArRampValBCMask=LArBadChannelMasker("RampValBCMask",
                                           DoMasking=True,
                                           ProblemsToMask=["deadReadout","deadCalib","deadPhys","almostDead",
                                                           "highNoiseHG","highNoiseMG","highNoiseLG"]
                                           )
   svcMgr.ToolSvc+=theLArRampValBCMask
   
   theRampValidationAlg=LArRampValidationAlg()
   theRampValidationAlg.BadChannelMaskingTool=theLArRampValBCMask
   theRampValidationAlg.KeyList=GainList
   theRampValidationAlg.ValidationKey="LArRamp"
   theRampValidationAlg.ReferenceKey="LArRampRef"
   theRampValidationAlg.MsgLevelForDeviations=INFO
   theRampValidationAlg.ListOfDevFEBs="rampFebs.txt"
   theRampValidationAlg.ThinnedValContainer="ThinRampsVal"
   theRampValidationAlg.ThinnedRefContainer="ThinRampsRef"
   topSequence+=theRampValidationAlg

   ## second instance of the validation tool to detect "bad" channel 
   theBadRamp=LArRampValidationAlg("theBadRamp")
   theBadRamp.BadChannelMaskingTool=theLArRampValBCMask
   theBadRamp.KeyList=GainList
   theBadRamp.ValidationKey="LArRamp"
   theBadRamp.ReferenceKey="LArRampRef"
   theBadRamp.MsgLevelForDeviations=INFO
   theBadRamp.ListOfDevFEBs="Bad_rampFebs.txt"
   topSequence+=theBadRamp


if ( doMonitoring ) :
   
   from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
   topSequence += AthenaMonManager( "LArMon" )
   LArMon = topSequence.LArMon

    ## old style
   #LArMon.FileKey = "AllMon"
   #LArMon.ManualDataTypeSetup = True
   #LArMon.Environment         = "user"
   #LArMon.ManualRunLBSetup    = True
   #LArMon.Run                 = 1
   #LArMon.LumiBlock           = 1
   #LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run

   ## tier0 style
   LArMon.FileKey = "GLOBAL"
   LArMon.ManualDataTypeSetup = True
   LArMon.Environment         = "tier0"
   LArMon.ManualRunLBSetup    = True
   LArMon.Run                 = int(RunNumberList[0])
   LArMon.LumiBlock           = 1
   LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run
   
   include("LArMonTools/LArFEBMon_jobOptions.py")
   
   RootHistOutputFileName='RootHistos_'+BaseFileName+".root"

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import THistSvc
   if os.path.exists(OutputRampRootFileDir+ "/" +RootHistOutputFileName): 
      os.remove(OutputRampRootFileDir+ "/" +RootHistOutputFileName)
   svcMgr += THistSvc()
   #svcMgr.THistSvc.Output = ["AllMon DATAFILE='"+OutputRampRootFileDir+ "/" +RootHistOutputFileName+"' OPT='New'"] 

   svcMgr.THistSvc.Output = ["GLOBAL DATAFILE='"+OutputRampRootFileDir+ "/" +RootHistOutputFileName+"' OPT='New'"]
   
if (WriteNtuple):
   
   # Ramp fit ntuple
   from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
   AllGainList = ["HIGH","MEDIUM","LOW"]
   if ( AllWavesPerCh ) :
      LArRamps2NtupleVec = []
      for i in range(0, MaxCalLinePerCh):
        topSequence+=LArRamps2Ntuple("LArRamps2Ntuple"+str(i+1))
        exec 'LArRamps2NtupleVec.append( topSequence.LArRamps2Ntuple%(fn)s)' % {'fn': i+1}
        if SaveRawRamp:
               rawkeys = []
               for g in range(0, len(AllGainList)):
                  rawkeys += [ KeyOutputSplitted[i]+AllGainList[g] ]
               LArRamps2NtupleVec[i].ContainerKey = rawkeys
        LArRamps2NtupleVec[i].RampKey =  KeyOutputSplitted[i] 
        LArRamps2NtupleVec[i].NtupleName   = "RAMPS"+str(i)
        LArRamps2NtupleVec[i].RawRamp      = SaveRawRamp
        LArRamps2NtupleVec[i].AddFEBTempInfo = False
        LArRamps2NtupleVec[i].OutputLevel  = ERROR
   else:     
      LArRamps2Ntuple=LArRamps2Ntuple("LArRamps2Ntuple")
      if SaveRawRamp:
         rawkeys = []
         for g in range(0, len(GainList)):
                  rawkeys += [ KeyOutput+AllGainList[g] ]
      LArRamps2Ntuple.ContainerKey = rawkeys #Only for raw ramp
      LArRamps2Ntuple.NtupleName = "RAMPS"
      LArRamps2Ntuple.RawRamp = SaveRawRamp
      LArRamps2Ntuple.AddFEBTempInfo = False
   
      topSequence+= LArRamps2Ntuple
      
   if ( SaveAverages ):
      # Ramp points ntuple(s)
      from LArCalibTools.LArCalibToolsConf import LArAverages2Ntuple
      index = 0
      AveragesVec=[]
      if ( AllWavesPerCh ) :
         for i in range(0, MaxCalLinePerCh):
            for j in range(0,len(AllGainList)):
               if AllGainList[j] in GainList:
                  topSequence+=LArAverages2Ntuple("Averages"+str(i+1)+AllGainList[j])
                  exec 'AveragesVec.append( topSequence.Averages%(fn)s%(fg)s )' % {'fn': i+1, 'fg':AllGainList[j]}
                  AveragesVec[index].ContainerKey = ContNameSplitted[i]+AllGainList[j]
                  index += 1
            if GainList.count("FREE"):
               topSequence+=LArAverages2Ntuple("Averages"+str(i+1))
               exec 'AveragesVec.append( topSequence.Averages%(fn)s)' % {'fn': i+1}
               AveragesVec[index].ContainerKey = ContNameSplitted[i]
               index += 1
      else:

         for j in range(0,len(AllGainList)):
            if AllGainList[j] in GainList:
               topSequence+=LArAverages2Ntuple("Averages"+AllGainList[j])
               exec 'AveragesVec.append( topSequence.Averages%(fg)s )' % {'fg':AllGainList[j]}
               AveragesVec[index].ContainerKey = AllGainList[j]
               index += 1
         if GainList.count("FREE"):
            topSequence+=LArAverages2Ntuple("Averages")
            exec 'AveragesVec.append( topSequence.Averages)'
            AveragesVec[index].ContainerKey = KeyOutput
            index += 1

      
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputRampRootFileDir+"/"+OutputRampRootFileName): 
      os.remove(OutputRampRootFileDir+"/"+OutputRampRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputRampRootFileDir+"/"+OutputRampRootFileName+"' OPT='NEW'" ]

if ( WritePoolFile ):

   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   if os.path.exists(OutputRampPoolFileDir+"/"+OutputRampPoolFileName): 
      os.remove(OutputRampPoolFileDir+"/"+OutputRampPoolFileName)  
   theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputRampPoolFileDir+"/"+OutputRampPoolFileName,
                             [OutputObjectSpecRamp],[OutputObjectSpecTagRamp],WriteIOV)
   theOutputConditionsAlg.Run1 = IOVBegin
   if IOVEnd>0:
      theOutputConditionsAlg.Run2=IOVEnd

   svcMgr.IOVDbSvc.dbConnection  = OutputDB

   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
   svcMgr.IOVRegistrationSvc.RecreateFolders = False

###########################################################################	
	
svcMgr.MessageSvc.OutputLevel  = WARNING
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = WARNING)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################
