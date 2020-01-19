#
#
# ##########################################################################################
# Electron/photon/muon augmentation
# Use the derivation framework tools directly
# ##########################################################################################

# Cache the container names (used throughout)
jetContainer = primRPVLLDESDM.DV_containerFlags.jetCollectionName
METContainer= primRPVLLDESDM.DV_containerFlags.METCollectionName
photonContainer = primRPVLLDESDM.DV_containerFlags.photonCollectionName
muonContainer = primRPVLLDESDM.DV_containerFlags.muonCollectionName
electronContainer = primRPVLLDESDM.DV_containerFlags.electronCollectionName


# ##########################################################################################
# Selection algorithms setup
# ##########################################################################################

def DVSelectionString(flags, container):
    """Construct object selection string based on the flags provided (from DVFlags)
    """

    cutList = []
    try:  
        cutList.append('%s.pt > %s'%(container,flags.cutEtMin))
        pass
    except:
        pass
    try:
        cutList.append('abs(%s.eta) < %s'%(container,flags.cutEtaMax))
        pass
    except:
        pass
    try:   
        cutList.append("DV%s%s"%(container,flags.cutIsEM))
        pass
    except:
        pass
    cutString = 'count(' + ' && '.join(cutList) + ') > %i'%(flags.nPassed-1)
    return cutString

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

def DVTriggerSelectionString(flags):
    cutString=""
    if flags.triggers.__len__() >=1:
        cutString+=flags.triggers[0]
        if flags.triggers.__len__() >1:
            for trigger in flags.triggers[1:]:
                cutString+=" || "+trigger
                pass
            pass
        pass
    return cutString
    pass

############################################################
## Tools for combining filters
###########################################################
 ## needs package PhysicsAnalysis/DerivationFramework/DerivationFrameworkTools-00-00-20 or later..
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR


############################################################
## Trackless jet filters
###########################################################



from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DVTracklessJetFilterTool

DVSingleTracklessJetFilterTool = DerivationFramework__DVTracklessJetFilterTool( name = "DVSingleTracklessJetFilterTool",
                                                                                JetContainerKey = jetContainer,
                                                                                JetPtCut= primRPVLLDESDM.DV_SingleTracklessJetFilterFlags.cutEtMin,
                                                                                JetEtaCut= primRPVLLDESDM.DV_SingleTracklessJetFilterFlags.cutEtaMax,
                                                                                sumPtTrkCut = primRPVLLDESDM.DV_SingleTracklessJetFilterFlags.cutSumPtTrkMax,
                                                                                nJetsRequired = 1)

ToolSvc+=DVSingleTracklessJetFilterTool

DVDoubleTracklessJetFilterTool = DerivationFramework__DVTracklessJetFilterTool( name = "DVDoubleTracklessJetFilterTool",
                                                                                JetContainerKey = jetContainer,
                                                                                JetPtCut= primRPVLLDESDM.DV_DoubleTracklessJetFilterFlags.cutEtMin,
                                                                                JetEtaCut= primRPVLLDESDM.DV_DoubleTracklessJetFilterFlags.cutEtaMax,
                                                                                sumPtTrkCut = primRPVLLDESDM.DV_DoubleTracklessJetFilterFlags.cutSumPtTrkMax,
                                                                                nJetsRequired = 2)

ToolSvc+=DVDoubleTracklessJetFilterTool

###### add a prescale tool - needs DerivationFrameworkTools-00-00-22 or later
###### NOTE! Subtlety here - if we use the same instance of the PrescaleTool in more than one place, it's internal counter will increment for each instance in
###### each event, which is not the desired behaviour.. (e.g. if we had a prescale of 3, and the same instance of the tool being used in 3 different filters, 
###### the third one would always pass).  INSTEAD - use a different instance of PrescaleTool each time.


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool
DVPrescaleToolForMultijet = DerivationFramework__PrescaleTool(name = "DVPrescaleToolForMultijet",
                                                   Prescale = primRPVLLDESDM.DV_PrescalerFlags.prescale
                                                   )
ToolSvc+=DVPrescaleToolForMultijet

DVCombinedTracklessJetFilterToolForMultijet = DerivationFramework__FilterCombinationOR( name = "DVCombinedTracklessJetFilterToolForMultijet",
                                                                             FilterList = [DVSingleTracklessJetFilterTool,
                                                                                           DVDoubleTracklessJetFilterTool,
                                                                                           DVPrescaleToolForMultijet],
##                                                                             OutputLevel=DEBUG
                                                                             )
ToolSvc+=DVCombinedTracklessJetFilterToolForMultijet

DVPrescaleToolForMET = DerivationFramework__PrescaleTool(name = "DVPrescaleToolForMET",
                                                   Prescale = primRPVLLDESDM.DV_PrescalerFlags.prescale
                                                   )
ToolSvc+=DVPrescaleToolForMET

DVCombinedTracklessJetFilterToolForMET = DerivationFramework__FilterCombinationOR( name = "DVCombinedTracklessJetFilterToolForMET",
                                                                             FilterList = [DVSingleTracklessJetFilterTool,
                                                                                           DVDoubleTracklessJetFilterTool,
                                                                                           DVPrescaleToolForMET],
##                                                                             OutputLevel=DEBUG
                                                                             )
ToolSvc+=DVCombinedTracklessJetFilterToolForMET

DVPrescaleToolForPhoton = DerivationFramework__PrescaleTool(name = "DVPrescaleToolForPhoton",
                                                   Prescale = primRPVLLDESDM.DV_PrescalerFlags.prescale
                                                   )
ToolSvc+=DVPrescaleToolForPhoton

DVCombinedTracklessJetFilterToolForPhoton = DerivationFramework__FilterCombinationOR( name = "DVCombinedTracklessJetFilterToolForPhoton",
                                                                             FilterList = [DVSingleTracklessJetFilterTool,
                                                                                           DVDoubleTracklessJetFilterTool,
                                                                                           DVPrescaleToolForPhoton],
##                                                                             OutputLevel=DEBUG
                                                                             )
ToolSvc+=DVCombinedTracklessJetFilterToolForPhoton

#======================================================================================================================================
#######################################################################################################################################
## should have all the pieces now - combine them into actual filters for the DerivationKernel #########################################
#######################################################################################################################################
#======================================================================================================================================


#############################################################################################
##  DV+muon filter
#############################################################################################



from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Barrel only MS only trigger 
DVMuonBarrelTriggerFilter = skimtool( name = "DVMuonBarrelTriggerFilter",
                                expression = DVTriggerSelectionString(primRPVLLDESDM.DV_MuonBarrelFilterFlags)
                                )
ToolSvc+=DVMuonBarrelTriggerFilter

DVMuonBarrelFilterTool = skimtool( name = "DVMuonBarrelFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_MuonBarrelFilterFlags, muonContainer)
				)
ToolSvc+=DVMuonBarrelFilterTool

DV_MuonBarrelFilter = DerivationFramework__FilterCombinationAND( name = "DV_MuonBarrelFilter",
                                                                FilterList=[DVMuonBarrelFilterTool,DVMuonBarrelTriggerFilter],
                                                                #OutputLevel=DEBUG
                                                                )
ToolSvc+=DV_MuonBarrelFilter
# Full MS only trigger
DVMuonFullMSTriggerFilter = skimtool( name = "DVMuonFullMSTriggerFilter",
                                expression = DVTriggerSelectionString(primRPVLLDESDM.DV_MuonFullMSFilterFlags)
                                )
ToolSvc+=DVMuonFullMSTriggerFilter

DVMuonFullMSFilterTool = skimtool( name = "DVMuonFullMSFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_MuonFullMSFilterFlags, muonContainer)
 				)
ToolSvc+=DVMuonFullMSFilterTool

DV_MuonFullMSFilter = DerivationFramework__FilterCombinationAND( name = "DV_MuonFullMSFilter",
                                                                FilterList=[DVMuonFullMSFilterTool,DVMuonFullMSTriggerFilter],
                                                                #OutputLevel=DEBUG
                                                                )
ToolSvc+=DV_MuonFullMSFilter

# Or of barrel only and full MS
DV_MuonFinalFilter = DerivationFramework__FilterCombinationOR( name = "DV_MuonFinalFilter",
								FilterList=[DV_MuonBarrelFilter,DV_MuonFullMSFilter],
								#OutputLevel=DEBUG
								)
ToolSvc+=DV_MuonFinalFilter
topSequence += kernel("RPVLL_DVMuonFilterKernel",
                      SkimmingTools = [DV_MuonFinalFilter])
RPVLLfilterNames.extend(["RPVLL_DVMuonFilterKernel"])

########################################
## photon filter for DV+electron
#######################################

DVPhotonTriggerFilter = skimtool( name = "DVPhotonTriggerFilter",
                                expression = DVTriggerSelectionString(primRPVLLDESDM.DV_PhotonFilterFlags)
                                )
ToolSvc += DVPhotonTriggerFilter

# Photon IsEM

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__RpvEgammaIDTool
DVPhotonSelection = DerivationFramework__RpvEgammaIDTool( name               = "DVPhotonSelection",
                                                             SelectionVariables = ["Loose", "Tight"],
                                                             CollectionName     =photonContainer ,
                                                             SGPrefix           = "DV" + photonContainer,
                                                           )
ToolSvc += DVPhotonSelection
topSequence += DerivationFramework__DerivationKernel( "DVPhotonAugmentationKernel",
                                                      AugmentationTools = [DVPhotonSelection]
                                                      )



DVPhotonFilterTool = skimtool( name = "DVPhotonFilterTool",
                               expression = DVSelectionString(primRPVLLDESDM.DV_PhotonFilterFlags, photonContainer),
                               )
ToolSvc += DVPhotonFilterTool
DV_PhotonFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV_PhotonFinalFilter",
                                                                  FilterList=[DVPhotonFilterTool,DVPhotonTriggerFilter],
##                                                                  OutputLevel=DEBUG
                                                                  )

ToolSvc+=DV_PhotonFinalFilter

DVPhotonPrescaleTool = DerivationFramework__PrescaleTool(name = "DVPhotonPrescaleTool",
                                                         Prescale = primRPVLLDESDM.DV_PhotonFilterFlags.prescale
                                                         )
ToolSvc+=DVPhotonPrescaleTool

DV_PrescaledPhotonFinalFilter = DerivationFramework__FilterCombinationAND(name="DV_PrescaledPhotonFinalFilter",
                                                                     FilterList=[DV_PhotonFinalFilter,DVPhotonPrescaleTool])
ToolSvc += DV_PrescaledPhotonFinalFilter

topSequence += kernel( "RPVLL_DV_PhotonFilterKernel",
                       SkimmingTools = [DV_PrescaledPhotonFinalFilter]
                       )
RPVLLfilterNames.extend(["RPVLL_DV_PhotonFilterKernel"])

##########################################################################################################################################
### Backup - photon+trackless jet - same combination as for multijets or MET - one hard trackless jets OR 2 soft OR prescaled passthrough.
###########################################################################################################################################

DV_PhotonPlusTracklessJetFilter = DerivationFramework__FilterCombinationAND(name="DV_PhotonPlusTracklessJetFilter",
                                                                            FilterList=[DV_PhotonFinalFilter,DVCombinedTracklessJetFilterToolForPhoton]
                                                                            )
ToolSvc+= DV_PhotonPlusTracklessJetFilter

topSequence += kernel( "RPVLL_DV_PhotonPlusTLJetFilterKernel",
                       SkimmingTools = [DV_PhotonPlusTracklessJetFilter]
                       )
RPVLLfilterNames.extend(["RPVLL_DV_PhotonPlusTLJetFilterKernel"])


###########################################################################################
# DV+jets filter
###########################################################################################

DVMultiJetTriggerFilter = skimtool( name = "DVMultiJetTriggerFilter",
                                  expression = DVTriggerSelectionString(primRPVLLDESDM.DV_MultiJetTriggerFlags)
                                  )
ToolSvc += DVMultiJetTriggerFilter

DV2JetFilterTool = skimtool( name = "DV2JetFilterTool",
                                 expression = DVSelectionString(primRPVLLDESDM.DV_2JetFilterFlags, jetContainer),
                                        )
ToolSvc += DV2JetFilterTool

DV3JetFilterTool = skimtool( name = "DV3JetFilterTool",
                                 expression = DVSelectionString(primRPVLLDESDM.DV_3JetFilterFlags, jetContainer),
                                        )
ToolSvc += DV3JetFilterTool


DV3JetFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV3JetFinalFilter",
                                                               FilterList=[DV3JetFilterTool,DV2JetFilterTool],
#                                                              OutputLevel=DEBUG
                                                               )
ToolSvc+=DV3JetFinalFilter

DV4JetFilterTool = skimtool( name = "DV4JetFilterTool",
                                 expression = DVSelectionString(primRPVLLDESDM.DV_4JetFilterFlags, jetContainer),
                                        )
ToolSvc += DV4JetFilterTool


DV5JetFilterTool = skimtool( name = "DV5JetFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_5JetFilterFlags, jetContainer),
                             )
ToolSvc += DV5JetFilterTool


DV6JetFilterTool = skimtool( name = "DV6JetFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_6JetFilterFlags, jetContainer),
                             )
ToolSvc += DV6JetFilterTool

DV7JetFilterTool = skimtool( name = "DV7JetFilterTool",
                             expression = DVSelectionString(primRPVLLDESDM.DV_7JetFilterFlags, jetContainer),
                             )
ToolSvc += DV7JetFilterTool




DV_multiJet_offlineJetFilter = DerivationFramework__FilterCombinationOR( name = "DV_multiJet_offlineJetFilter",
                                                                        FilterList=[DV3JetFinalFilter,DV4JetFilterTool,DV5JetFilterTool,DV6JetFilterTool,DV7JetFilterTool],
##                                                                        OutputLevel=DEBUG
                                                                        )
ToolSvc += DV_multiJet_offlineJetFilter



DV_MultiJetFinalTracklessFilter = DerivationFramework__FilterCombinationAND( name = "DV_MultiJetFinalTracklessFilter",
                                                                    FilterList=[DV_multiJet_offlineJetFilter,DVCombinedTracklessJetFilterToolForMultijet,DVMultiJetTriggerFilter],
##                                                                    OutputLevel=DEBUG
                                                                        )
ToolSvc+= DV_MultiJetFinalTracklessFilter


DV_MultiJet2JHighPtFilter = skimtool( name = "DV_MultiJet2JHighPtFilter",
                             expression = DVSelectionString(primRPVLLDESDM.DV_2JetFilterFlags_HighpTCut, jetContainer),
                             )
ToolSvc += DV_MultiJet2JHighPtFilter

DV_MultiJet3JHighPtFilter = skimtool( name = "DV_MultiJet3JHighPtFilter",
                             expression = DVSelectionString(primRPVLLDESDM.DV_3JetFilterFlags_HighpTCut, jetContainer),
                             )
ToolSvc += DV_MultiJet3JHighPtFilter


DV_Multi3_2JetHighPtFilter = DerivationFramework__FilterCombinationAND( name = "DV_Multi3_2JetHighPtFilter",
                                                               FilterList=[DV_MultiJet2JHighPtFilter,DV_MultiJet3JHighPtFilter],
##                                                                    OutputLevel=DEBUG
                                                               )
ToolSvc+=DV_Multi3_2JetHighPtFilter


DV_MultiJet4JHighPtFilter = skimtool( name = "DV_MultiJet4JHighPtFilter",
                             expression = DVSelectionString(primRPVLLDESDM.DV_4JetFilterFlags_HighpTCut, jetContainer),
                             )
ToolSvc += DV_MultiJet4JHighPtFilter


DV_MultiJet5JHighPtFilter = skimtool( name = "DV_MultiJet5JHighPtFilter",
                             expression = DVSelectionString(primRPVLLDESDM.DV_5JetFilterFlags_HighpTCut, jetContainer),
                             )
ToolSvc += DV_MultiJet5JHighPtFilter


DV_MultiJet6JHighPtFilter = skimtool( name = "DV_MultiJet6JHighPtFilter",
                             expression = DVSelectionString(primRPVLLDESDM.DV_6JetFilterFlags_HighpTCut, jetContainer),
                             )
ToolSvc += DV_MultiJet6JHighPtFilter


DV_MultiJet7JHighPtFilter = skimtool( name = "DV_MultiJet7JHighPtFilter",
                             expression = DVSelectionString(primRPVLLDESDM.DV_7JetFilterFlags_HighpTCut, jetContainer),
                             )
ToolSvc += DV_MultiJet7JHighPtFilter


DV_MultiJetHighPtFilter = DerivationFramework__FilterCombinationOR( name = "DV_MultiJetHighPtFilter",
                                                               FilterList=[DV_Multi3_2JetHighPtFilter,DV_MultiJet4JHighPtFilter,DV_MultiJet5JHighPtFilter,DV_MultiJet6JHighPtFilter,DV_MultiJet7JHighPtFilter],
##                                                                    OutputLevel=DEBUG
                                                               )
ToolSvc+=DV_MultiJetHighPtFilter


DV_MultiJetFinalFilter = DerivationFramework__FilterCombinationOR( name = "DV_MultiJetFinalFilter",
                                                               FilterList=[DV_MultiJetFinalTracklessFilter,DV_MultiJetHighPtFilter],
##                                                                    OutputLevel=DEBUG
                                                               )
ToolSvc+= DV_MultiJetFinalFilter

topSequence += kernel( "RPVLL_DV_MultiJetFilterKernel",
                       SkimmingTools = [DV_MultiJetFinalFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_DV_MultiJetFilterKernel"])


############################################################
## DV+MET filter
###########################################################

DVMETTriggerFilter = skimtool( name = "DVMETTriggerFilter",
                               expression = DVTriggerSelectionString(primRPVLLDESDM.DV_METFilterFlags)
                               )

ToolSvc+=DVMETTriggerFilter

from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DVMissingETFilterTool


DVMETFilterTool = DerivationFramework__DVMissingETFilterTool(name = "DVMETFilterTool",
                                                             METContainerKey = METContainer,
                                                             METCut=primRPVLLDESDM.DV_METFilterFlags.cutMetMin)
ToolSvc+=DVMETFilterTool

DV_METFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV_METFinalFilter",
                                                               FilterList=[DVMETFilterTool,DVMETTriggerFilter],
##                                                               OutputLevel=DEBUG
                                                               )
ToolSvc += DV_METFinalFilter

topSequence += kernel( "RPVLL_DV_METFilterKernel",
                       SkimmingTools = [DV_METFinalFilter],
                       )
RPVLLfilterNames.extend(["RPVLL_DV_METFilterKernel"])

#########################################################################
### M_eff filter - use the MET trigger, but then cut on Meff, or MET/Meff
#######################################################################
# turning off DVMeffFilter 
#DVMeffTriggerFilter = skimtool( name = "DVMeffTriggerFilter",
#                               expression = DVTriggerSelectionString(primRPVLLDESDM.DV_MeffFilterFlags)
#                               )
#
#ToolSvc+=DVMeffTriggerFilter
#
#from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DVMeffFilterTool
#DVMeffFilterTool = DerivationFramework__DVMeffFilterTool(name = "DVMeffFilterTool",
#                                                         METContainerKey = METContainer,
#                                                         MeffCut=primRPVLLDESDM.DV_MeffFilterFlags.cutMeffMin,
#                                                         METCut=primRPVLLDESDM.DV_MeffFilterFlags.cutMETMin,
#                                                         METoverMeffCutMin=primRPVLLDESDM.DV_MeffFilterFlags.cutMEToverMeffMin,
#                                                         METoverMeffCutMax=primRPVLLDESDM.DV_MeffFilterFlags.cutMEToverMeffMax)
#ToolSvc += DVMeffFilterTool
#
#DV_MeffFinalFilter = DerivationFramework__FilterCombinationAND( name = "DV_MEffFinalFilter",
#                                                               FilterList=[DVMeffFilterTool,DVMeffTriggerFilter],
###                                                               OutputLevel=DEBUG
#                                                                )
#ToolSvc += DV_MeffFinalFilter
#
#topSequence += kernel( "RPVLL_DV_MeffFilterKernel",
#                       SkimmingTools = [DV_MeffFinalFilter],
#                       )
#RPVLLfilterNames.extend(["RPVLL_DV_MeffFilterKernel"])

