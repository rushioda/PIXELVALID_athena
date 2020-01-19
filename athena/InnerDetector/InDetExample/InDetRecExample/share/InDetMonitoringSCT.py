from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

########################################################################
# Conditions access
# These lines were previously in SCT_Monitoring_ConditionsAccess.py
########################################################################

from SCT_Monitoring.SCT_MonitoringConf import SCTHitsNoiseMonTool
InDetSCTHitsTool = SCTHitsNoiseMonTool ( name                         = "InDetSCTHitsNoiseMonTool",
                                         OutputLevel                  = 4,
                                         doHitmapHistos               = False,
                                         CheckRate                    = 1000,
                                         doPositiveEndcap             = True,
                                         doNegativeEndcap             = True,
                                         doTrigger = globalflags.DataSource == "data")

from RecExConfig.RecFlags import rec
if not rec.doTrigger():
  InDetSCTHitsTool.doTrigger=False
	
InDetSCTHitsTool.tracksName = InDetKeys.SCTTracks() if  InDetFlags.doTrackSegmentsSCT() else InDetKeys.UnslimmedTracks()

ToolSvc += InDetSCTHitsTool
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTHitsTool

from SCT_Monitoring.SCT_MonitoringConf import SCTTracksMonTool
InDetSCTTracksMonTool = SCTTracksMonTool ( name             = "InDetSCTTracksMonTool",
                                           OutputLevel      = 4,
                                           CheckRate        = 1000,
                                           doPositiveEndcap = True,
                                           doNegativeEndcap = True,
                                           doUnbiasedCalc   = True,
                                           doTrigger        = globalflags.DataSource == "data")

InDetSCTTracksMonTool.tracksName = InDetKeys.SCTTracks() if  InDetFlags.doTrackSegmentsSCT() else InDetKeys.UnslimmedTracks()

if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
  InDetSCTTracksMonTool.FilterTools.append(monFilledBunchFilterTool)

ToolSvc += InDetSCTTracksMonTool
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTTracksMonTool

                                           
from SCT_Monitoring.SCT_MonitoringConf import SCTErrMonTool
InDetSCTErrMonTool = SCTErrMonTool ( name             = "InDetSCTErrMonTool",
                                     OutputLevel      = 4,
                                     histoPathBase    = "/stat",
                                     CheckRate        = 1000,
                                     doPositiveEndcap = True,
                                     doNegativeEndcap = True)

if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
  InDetSCTErrMonTool.FilterTools.append(monFilledBunchFilterTool)
  
ToolSvc += InDetSCTErrMonTool
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTErrMonTool
               


if not hasattr(ServiceMgr,"TrigConfigSvc"):
  from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc
  trigconfigsvc= SetupTrigConfigSvc()
  trigconfigsvc.SetStates('ds')
  trigconfigsvc.InitialiseSvc()
  pass

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
SCT_BunchCrossingTool = BunchCrossingTool()


from SCT_Monitoring.SCT_MonitoringConf import SCTHitEffMonTool
InDetSCTHitEffMonTool = SCTHitEffMonTool(name = "InDetSCTHitEffMonTool",
                                         DetectorMode            = 3,
                                         OutputLevel             = 4,
                                         IsCosmic                = InDetFlags.doCosmics(),
                                         UseMasks                = False,
                                         LookAtDatabase          = False,
                                         FlaggedConditionService = ServiceMgr.InDetSCT_FlaggedConditionSvc,
                                         ChronoTime              = False,
                                         HoleSearch              = InDetHoleSearchTool,
                                         IsSim                   = globalflags.DataSource != "data",
                                         ROTCreator              = SCT_ClusterOnTrackTool,
                                         BunchCrossingTool       = SCT_BunchCrossingTool)

InDetSCTHitEffMonTool.TrackName = InDetKeys.SCTTracks() if  InDetFlags.doTrackSegmentsSCT() else InDetKeys.UnslimmedTracks()

if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
  InDetSCTHitEffMonTool.FilterTools.append(monFilledBunchFilterTool)
  
ToolSvc += InDetSCTHitEffMonTool
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTHitEffMonTool

#from SCT_Monitoring.SCT_MonitoringConf import SCTTimeDependentMonTool
#SCTTimeDependentMonTool = SCTTimeDependentMonTool("SCTTimeDependentMonTool")
#ToolSvc+=SCTTimeDependentMonTool
#InDetSCTMonMan.AthenaMonTools += [SCTTimeDependentMonTool]

from SCT_Monitoring.SCT_MonitoringConf import SCTLorentzMonTool
InDetSCTLorentzMonTool = SCTLorentzMonTool ( name             = "InDetSCTLorentzMonTool",
                                             OutputLevel      = 4)

InDetSCTLorentzMonTool.tracksName = InDetKeys.SCTTracks() if  InDetFlags.doTrackSegmentsSCT() else InDetKeys.UnslimmedTracks()

if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
  InDetSCTLorentzMonTool.FilterTools.append(monFilledBunchFilterTool)
  
ToolSvc += InDetSCTLorentzMonTool
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTLorentzMonTool


from SCT_Monitoring.SCT_MonitoringConf import SCTRatioNoiseMonTool
InDetSCTRatioNoiseMonTool = SCTRatioNoiseMonTool ( name             = "InDetSCTRatioNoiseMonTool",
                                             OutputLevel      = 4,
                                                   doTrigger = globalflags.DataSource == "data")


from RecExConfig.RecFlags import rec
if not rec.doTrigger():
  InDetSCTRatioNoiseMonTool.doTrigger=False

ToolSvc += InDetSCTRatioNoiseMonTool
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTRatioNoiseMonTool
           

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

InDetSCTMonMan = AthenaMonManager("InDetSCTMonManager",
                                  FileKey             = DQMonFlags.monManFileKey(),
                                  ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                  ManualRunLBSetup    = DQMonFlags.monManManualRunLBSetup(),
                                  DataType            = DQMonFlags.monManDataType(),
                                  Environment         = DQMonFlags.monManEnvironment(),
                                  Run                 = DQMonFlags.monManRun(),
                                  LumiBlock           = DQMonFlags.monManLumiBlock(),


AthenaMonTools      = [ InDetSCTTracksMonTool, InDetSCTRatioNoiseMonTool, InDetSCTLorentzMonTool, InDetSCTHitEffMonTool, InDetSCTHitsTool, InDetSCTErrMonTool ] )

topSequence += InDetSCTMonMan
if (InDetFlags.doPrintConfigurables()):
  print InDetSCTMonMan
