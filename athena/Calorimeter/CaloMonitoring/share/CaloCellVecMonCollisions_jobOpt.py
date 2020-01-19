# CaloCell Monitoring for Collisions,
# Evan Wulf, 12/10/09 - earwulf@nevis.columbia.edu
#
#                           D Urbaniec       15/02/09
# LArCellVecMon_jobOpt.py   Francesco Spano  07/15/07
# LArCellVecMon_jobOpt.py   R. Kehoe         07/03/06

from CaloMonitoring.CaloMonitoringConf import LArCellMonTool

from RecExConfig.RecFlags import rec

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags  import globalflags

from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
include ("AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py")

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool=CaloNoiseToolDefault()

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArChanMasker=LArBadChannelMasker("LArChanMasker")
theLArChanMasker.DoMasking=True
theLArChanMasker.ProblemsToMask=[
     "deadReadout","deadPhys","almostDead","short",
     "sporadicBurstNoise",
     "unstableNoiseLG","unstableNoiseMG","unstableNoiseHG",
     "highNoiseHG","highNoiseMG","highNoiseLG"
]
ToolSvc+=theLArChanMasker

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
ToolSvc+=theLArBadChannelTool



# All 2D plot occupancy are activate only for express and cosmiccalo
if (rec.triggerStream()=='CosmicCalo' or rec.triggerStream()=='express' or rec.triggerStream()=='Main' or rec.triggerStream()=='ZeroBias') or (DQMonFlags.monManEnvironment() == 'online'):
   do2DOcc = True
else:
   do2DOcc = False

LArCellMon = LArCellMonTool(

    #DoSaveTempHists=True,
    #OutputLevel=DEBUG,

    name = "LArCellMon",
    CaloCellContainer = "AllCalo",

    BadLBTool = GetLArBadLBFilterTool(),

    ReadyFilterTool = monAtlasReadyFilterTool,

    useElectronicNoiseOnly = False,
    #useTwoGaus = True, Tile-only

    CaloNoiseTool=theCaloNoiseTool,

    useTrigger          =  DQMonFlags.useTrigger(),
    rndmTriggerNames    = "L1_RD0, L1_RD0_FILLED, L1_RD0_EMPTY, L1_RD1, L1_RD1_NOISE, L1_RD1_HIST, L1_RD1_BGRP4, L1_RD1_BGRP5",
    caloTriggerNames    = "L1_EM[0-9]+, L1_HA[0-9]+, L1_J[0-9]+.*, L1_JB[0-9]+, L1_JF[0-9]+, L1_TE[0-9]+, L1_JE[0-9]+, L1_XE[0-9]+, L1_2EM[0-9]+, L1_2FJ[0-9]+, L1_2J[0-9]+,L1_3J[0-9]+.*,L1_4J[0-9]+.*,L1_5J[0-9]+,L1_6J[0-9]+,L1_FJ[0-9]+.*",
    minBiasTriggerNames = "L1_RD0_FILLED, L1_MBTS_1, L1_MBTS_2, L1_MBTS_1_1",
    metTriggerNames     = "EF_xe[0-9]+.*",
    miscTriggerNames    = "",
    LArBadChannelMask=theLArChanMasker,
    LArBadChannelTool=theLArBadChannelTool,
    MaskBadChannels    = False,
    MaskNoCondChannels = False,
    #doInverseMasking   = False,
 #   doMaskingOnline    = False,
    #problemsToMaskOffline =["deadReadout","deadPhys","almostDead","short","sporadicBurstNoise","unstableNoiseLG",
    #                        "unstableNoiseMG","unstableNoiseHG","highNoiseHG","highNoiseMG","highNoiseLG"],

    # Database Record Plots (filled in first event)
    doDatabaseNoiseVsEtaPhi     = True,
    doKnownBadChannelsVsEtaPhi  = True,
    doUnnormalized1DEnergy      = True,
    useLogarithmicEnergyBinning = True,

    # Use WeightedEff rather than Weighted Average Merging, where appropriate:
    #  (if merging / errors are all correct, this should have no effect, hence this is a good validation tool)
    useWeightedEffMerging = True,

    Threshold_EM_S0S1= 5000,
    Threshold_HECFCALEMS2S3= 15000,
    NsporadicPlotLimit = 300,
    Sporadic_protection = 4000,

    #DoEtaLumi = False,
    #DoPhiLumi = False, 
    
# EtaPhi Cell Plots:
    
#   These histograms will be filled* for every event in which the energy threshold (either absolute or DB noise
#   dependent) and trigger selection is passed. Plots can be switched on for each threshold type. If a default
#   energy threshold (!=-99999999.) is specified for a given type, plots will be produced for all layers of the
#   LAr calorimeter (EMB, EMEC, HEC, FCAL), otherwise, if thresholds are given for only certain layers, only
#   those layers will have histograms produced for this threshold type.

#   Triggers correspond to those specified above under Trigger Awareness, the logic is that an event will be used
#   to fill plots of a given threshold type iff it passes any of the triggers in the TriggersToInclude trigger
#   chain group without passing any of the triggers in the TrigersToExclude chain group.

# Global Settings:

    ThresholdType          = [ "noEth", "noEth_rndm", "medEth", "5Sigma", "hiEth", "hiEth_noVeto", "met"  ],
    ThresholdDirection     = [ "none" , "none"      , "both"  , "both"  , "over" , "over"        , "over" ],
    TriggersToInclude      = [ "all"  , "all"       , "all"   , "all"   , "all"  , "all"         , "met"  ],
    TriggersToExclude      = [ "none" , "none"      , "none"  , "none"  , "none" , "none"        , "none" ],

    DoPercentageOccupancy  = [ False  , False       , True    , False   , False  , False         , False  ],
    DoEtaPhiOccupancy      = [ False  , False       , False   , do2DOcc,do2DOcc, do2DOcc      , False  ],
    DoEtaOccupancy         = [ False  , False       , False   , False   , False  , False         , False  ],
    DoPhiOccupancy         = [ False  , False       , True    , False   , False  , False         , False  ],

    DoEtaPhiAverageEnergy  = [ False  , do2DOcc    , False   , do2DOcc  , False   , False        , False  ],
    DoEtaPhiTotalEnergy    = [ False  , False       , False   , False   , False   , False        , False  ],
    #DoEtaPhiEnergyRMS      = , False  , False       , False   , False   , False   , False        , False  ],
    #DoEtaPhiRMSvsDBnoise   = , False  , False       , False   , False   , False   , False        , False  ], 

    DoEtaPhiAverageQuality = [ False  , False       , False   , False   , False   , False        , False  ],
    DoEtaPhiFractionOverQth= [ False  , False       , False   , False   , do2DOcc , do2DOcc      , False  ],
    QualityFactorThreshold = [ 4000. ]*7,

    DoEtaPhiAverageTime    = [ False  , False       , False   , True    , False   , False        , False  ],
    DoEtaPhiFractionPastTth= [ False  , False       , False   , True    , False   , False        , False  ],
    TimeThreshold          = [ 4.      ]*7,

    DoBeamBackgroundRemoval= [ True   , True        , True    , True    , True   , False         , True   ],

    # Defaults: (Can be over ridden by layer specific values) ; plots will be made for all layers with DefaultThreshold != -9999999
    ThresholdTitleTemplates= ["no Threshold",
                             "no Threshold",
                             "|E_{cell}| > %0.f#sigma_{noise}^{database}",
                             "E_{cell} beyond %0.f#sigma_{noise}^{database}",
                             "E_{cell} > %0.f MeV",
                             "E_{cell} > %0.f MeV",
                             "E_{cell} > %0.f MeV, MET trigger"],

    DefaultThresholds     = [-4000000.,-4000000.    , 3.      , 5.      , 500.   , 500.          , 1000.  ],
    ThresholdinSigNoise   = [ False   , False       , True    , True    , False  , False         , False  ],

    ThresholdColumnType   = ["hiEth", "hiEth_noVeto"],

    # EMB
    EMBP_Thresh           = [ 800.  , 800.          ],
    EMB1_Thresh           = [ 600.  , 600.          ],
    EMB2_Thresh           = [ 600.  , 600.          ],
    EMB3_Thresh           = [ 600.  , 600.          ],
    # EMEC
    EMECP_Thresh          = [ 1200. , 1200.         ],
    EMEC1_Thresh          = [ 800.  , 800.          ],
    EMEC2_Thresh          = [ 800.  , 800.          ],
    EMEC3_Thresh          = [ 800.  , 800.          ],
    # HEC
    HEC0_Thresh           = [ 4000. , 4000.         ],
    HEC1_Thresh           = [ 4000. , 4000.         ],
    HEC2_Thresh           = [ 4000. , 4000.         ],
    HEC3_Thresh           = [ 4000. , 4000.         ],
    # FCAL
    FCAL1_Thresh          = [ 6000. , 6000.         ],
    FCAL2_Thresh          = [ 6000. , 6000.         ],
    FCAL3_Thresh          = [ 6000. , 6000.         ],
)

#Tile monitoring:
from CaloMonitoring.CaloMonitoringConf import TileCalCellMonTool
TileCalCellMon=TileCalCellMonTool("TileCalCellMon",
                             CaloNoiseTool=theCaloNoiseTool
                            )


if DQMonFlags.monManEnvironment == 'online':
   LArCellMon.isOnline=True
   LArCellMon.useLArNoisyAlg=False
   LArCellMon.ProcessNEvents = 100
else:
   #Offline processing:
   LArCellMon.useLArNoisyAlg=True

   if (rec.triggerStream()=='CosmicCalo'):
      LArCellMon.Sporadic_switch=True
      LArCellMon.useBeamBackgroundRemoval = True
      LArCellMon.useLArCollisionFilterTool = True
      TileCalCellMon.useBeamBackgroundRemoval = True
      TileCalCellMon.useLArCollisionFilterTool = True
   else:
      LArCellMon.Sporadic_switch=False
      LArCellMon.useBeamBackgroundRemoval = False
      LArCellMon.useLArCollisionFilterTool = False
      TileCalCellMon.useBeamBackgroundRemoval = False
      TileCalCellMon.useLArCollisionFilterTool = False

   if globalflags.DataSource() == 'data': #not MC
      LArCellMon.useBadLBTool=True
      LArCellMon.useReadyFilterTool = True
      TileCalCellMon.useBadLBTool=True
      TileCalCellMon.useReadyFilterTool = True
   else:
      LArCellMon.useBadLBTool=False
      LArCellMon.useReadyFilterTool = False
      TileCalCellMon.useBadLBTool=False
      TileCalCellMon.useReadyFilterTool = False
      


ToolSvc+=LArCellMon
ToolSvc+=TileCalCellMon
CaloMon.AthenaMonTools += [LArCellMon,TileCalCellMon]


