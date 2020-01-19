#********************************************************************
# HIGG5D3.py (for 0-lepton)
# reductionConf flag HIGG5D3 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
# from DerivationFrameworkJetEtMiss.JetMomentFix import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *


# running on data or MC
from AthenaCommon.GlobalFlags import globalflags
#print "Yoshikazu Nagai test: globalflags.DataSource()"
#print globalflags.DataSource()  # this should be "data" or "geant4"
is_MC = (globalflags.DataSource()=='geant4')
print "is_MC = ",is_MC


#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HIGG5D3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG5D3Stream )
HIGG5D3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG5D3Stream.AcceptAlgs(["HIGG5D3Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================
thinningTools=[]

# Establish the thinning helper (which will set up the services behind the scenes) 
from DerivationFrameworkCore.ThinningHelper import ThinningHelper 
HIGG5D3ThinningHelper = ThinningHelper("HIGG5D3ThinningHelper") 
#trigger navigation content
HIGG5D3ThinningHelper.TriggerChains = 'HLT_g.*|HLT_mu.*|HLT_j.*|HLT_b.*' 
HIGG5D3ThinningHelper.AppendToStream(HIGG5D3Stream) 


# MC truth thinning (not for data)
truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))" # W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))" # Leptons
truth_cond_Quark  = "((abs(TruthParticles.pdgId) ==  6) || (abs(TruthParticles.pdgId) ==  5))" # Top quark and Bottom quark
truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 1*GeV))"       # Photon
truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+')'
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
HIGG5D3MCThinningTool = DerivationFramework__GenericTruthThinning(
    name                    = "HIGG5D3MCThinningTool", 
    ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
    ParticleSelectionString = truth_expression,
    PreserveDescendants     = False,
    PreserveGeneratorDescendants = True,
    PreserveAncestors       = True)
if (is_MC) :
    ToolSvc += HIGG5D3MCThinningTool
    thinningTools.append(HIGG5D3MCThinningTool)


# MET/Jet tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
HIGG5D3JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "HIGG5D3JetTPThinningTool",
                                                                ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += HIGG5D3JetTPThinningTool
thinningTools.append(HIGG5D3JetTPThinningTool)

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
HIGG5D3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "HIGG5D3MuonTPThinningTool",
                                                                            ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                            MuonKey                 = "Muons",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D3MuonTPThinningTool
thinningTools.append(HIGG5D3MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D3ElectronTPThinningTool",
                                                                                        ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                                        SGKey                   = "Electrons",
                                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                        BestMatchOnly           = True)
ToolSvc += HIGG5D3ElectronTPThinningTool
thinningTools.append(HIGG5D3ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
HIGG5D3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(       name                    = "HIGG5D3PhotonTPThinningTool",
                                                                                      ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                                      SGKey                   = "Photons",
                                                                                      InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                      BestMatchOnly           = True)
ToolSvc += HIGG5D3PhotonTPThinningTool
thinningTools.append(HIGG5D3PhotonTPThinningTool)

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
HIGG5D3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                  = "HIGG5D3TauTPThinningTool",
                                                                          ThinningService         = HIGG5D3ThinningHelper.ThinningSvc(),
                                                                          TauKey                  = "TauJets",
                                                                          ConeSize                = 0.6,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += HIGG5D3TauTPThinningTool
thinningTools.append(HIGG5D3TauTPThinningTool)

#====================================================================
# Skimming Tool
#====================================================================
Run2DataTriggers=['HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23',
                  'HLT_2j35_bmv2c2070_split_2j35_L13J25.0ETA23',
                  'HLT_2j35_btight_2j35_L13J25.0ETA23',
                  'HLT_2j35_btight_2j35_L14J15.0ETA25',
                  'HLT_2j35_btight_split_2j35_L13J25.0ETA23',
                  'HLT_2j35_btight_split_2j35_L14J15.0ETA25',
                  'HLT_2j45_bmedium_2j45_L13J25.0ETA23',
                  'HLT_2j45_bmedium_2j45_L14J15.0ETA25',
                  'HLT_2j45_bmedium_split_2j45_L13J25.0ETA23',
                  'HLT_2j45_bmedium_split_2j45_L14J15.0ETA25',
                  'HLT_2j45_bmv2c2077_2j45_L13J25.0ETA23',
                  'HLT_2j45_bmv2c2077_split_2j45_L13J25.0ETA23',
                  'HLT_2j45_btight_2j45',
                  'HLT_2j45_btight_2j45_L13J25.ETA23',
                  'HLT_2j45_btight_split_2j45',
                  'HLT_2j45_btight_split_2j45_L13J25.ETA23',
                  'HLT_2j55_bloose_L14J20.0ETA49',
                  'HLT_2j55_bmedium_2j55',
                  'HLT_2j55_bmedium_2j55_L13J25.ETA23',
                  'HLT_2j55_bmedium_L14J20.0ETA49',
                  'HLT_2j55_bmedium_split_2j55',
                  'HLT_2j55_bmedium_split_2j55_L13J25.ETA23',
                  'HLT_2j55_bperf_L14J20.0ETA49',
                  'HLT_2j65_btight_j65',
                  'HLT_2j65_btight_split_j65',
                  'HLT_2j70_bmedium_j70',
                  'HLT_2j70_bmedium_split_j70',
                  'HLT_2j70_btight_j70',
                  'HLT_2j70_btight_split_j70',
                  'HLT_2j75_bmedium_j75',
                  'HLT_2j75_bmedium_split_j75',
                  'HLT_j0_perf_bperf_L1MU10',
                  'HLT_mu4_4j40_dr05_L1MU4_3J15',
                  'HLT_mu4_4j40_dr05_L1MU4_3J20',
                  'HLT_mu4_j40_dr05_3j40_L14J20',
                  'HLT_mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_mu6_2j40_0eta490_invm600_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_mu6_2j40_0eta490_invm800_L1MU6_J30.0ETA49_2J20.0ETA49',
                  'HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490',
                  'HLT_g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700',
                  'HLT_g25_loose_2j40_0eta490_3j25_0eta490_invm700',
                  'HLT_g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700',
                  'HLT_g20_loose',
                  'HLT_g25_loose', # updated with TriggerMenu-00-12-40 (July 2015)
                  'HLT_g25_medium_L1EM22VHI_4j35_0eta490_invm700',
                  'HLT_g25_medium_L1EM22VHI_j35_0eta490_bmv2c2077_3j25_0eta490_invm700',
                  'HLT_g25_medium_L1EM22VHI_2j35_0eta490_bmv2c2077_2j35_0eta490',
                  'HLT_g25_loose_L1EM20VH_4j35_0eta490'] # added on Oct 2015


Run2MCTriggers=["L1_3J20_4J20.0ETA49_MJJ-400",
                "L1_3J25.ETA23",
                "L1_4J20",
                "L1_3J15_BTAG-MU4J15",
                "L1_MJJ-700",
                "L1_MJJ-400",
                "L1_EM15VH",
                "L1_EM13VH"] # based on TriggerMenu-00-09-41-12 for MC15 production


# Run2MCTriggers=["HLT_2j55_bloose_L13J20_4J20.0ETA49_MJJ-400",
#                 "HLT_2j55_bmedium_2j55_L13J25.ETA23",
#                 "HLT_2j45_btight_2j45_L13J25.ETA23",
#                 "HLT_2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-400",
#                 "HLT_mu4_3j35_dr05_j35_bloose_L13J15_BTAG-MU4J15",
#                 "HLT_mu4_3j35_dr05_j35_bloose_L14J20",
#                 "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700",
#                 "HLT_g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700",
#                 "HLT_g20_loose_2j40_0eta490_3j25_0eta490"]

beamEnergy = jobproperties.Beam.energy()
from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__SkimmingToolHIGG5VBF
if (beamEnergy < 4.1e+06): # 8 TeV - Run1
    if (is_MC): 
        HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                        JetContainerKey         = "AntiKt4EMTopoJets",
                                                                        # jet multiplicity requirement 2b + 2j
                                                                        ReqNAllJets             = True,
                                                                        NumberOfAllJets         = 4,
                                                                        AllJetPtCut             = 40.*Units.GeV,
                                                                        AllJetEtaCut            = 4.9,
                                                                        # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                        ReqNCentralJets         = True,
                                                                        NumberOfCentralJets     = 2,
                                                                        CentralJetPtCut         = 40.*Units.GeV,
                                                                        CentralJetEtaCut        = 2.6,
                                                                        # trigger requirement
                                                                        ReqTrigger              = False,
                                                                        Triggers                = [],
                                                                        # Mjj requirement
                                                                        ReqVBFMjj               = True,
                                                                        MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                        DoDebug                 = False,
                                                                        #photon requirement (p. rose)
                                                                        PhotonContainerKey      = "Photons",
                                                                        ReqPhoton               = True,
                                                                        PhotonPtCut             = 15.*Units.GeV,
                                                                        CentralPhotonEtaCut     = 2.6)

    else :
        HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                        JetContainerKey         = "AntiKt4EMTopoJets",
                                                                        # jet multiplicity requirement 2b + 2j
                                                                        ReqNAllJets             = True,
                                                                        NumberOfAllJets         = 4,
                                                                        AllJetPtCut             = 40.*Units.GeV,
                                                                        AllJetEtaCut            = 4.9,
                                                                        # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                        ReqNCentralJets         = True,
                                                                        NumberOfCentralJets     = 2,
                                                                        CentralJetPtCut         = 40.*Units.GeV,
                                                                        CentralJetEtaCut        = 2.6,
                                                                        # trigger requirement
                                                                        ReqTrigger              = True,
                                                                        #Triggers                = ["EF_2b35_loose_4j35_a4tchad","EF_b35_medium_j35_a4tchad_vbf_3L1J15_FJ15","EF_b35_medium_j35_a4tchad_vbf_2L1FJ15", "EF_g20_loose"],
                                                                        Triggers                = ["HLT_2b35_loose_4j35_a4tchad","HLT_b35_medium_j35_a4tchad_vbf_3L1J15_FJ15","HLT_b35_medium_j35_a4tchad_vbf_2L1FJ15", "HLT_g20_loose"],
                                                                        # Mjj requirement
                                                                        ReqVBFMjj               = True,
                                                                        MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                        DoDebug                 = False,
                                                                        
                                                                        #photon requirement (p. rose)
                                                                        PhotonContainerKey      = "Photons",
                                                                        ReqPhoton               = True,
                                                                        PhotonPtCut             = 20.*Units.GeV,
                                                                        CentralPhotonEtaCut     = 2.6)

if (beamEnergy > 6.0e+06): # 13 TeV - Run2
    if (is_MC):
        HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                        JetContainerKey         = "AntiKt4EMTopoJets",
                                                                        # jet multiplicity requirement 2b + 2j
                                                                        ReqNAllJets             = False,
                                                                        NumberOfAllJets         = 4,
                                                                        AllJetPtCut             = 40.*Units.GeV,
                                                                        AllJetEtaCut            = 4.9,
                                                                        # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                        ReqNCentralJets         = True,
                                                                        NumberOfCentralJets     = 2,
                                                                        CentralJetPtCut         = 15.*Units.GeV,  # to be adjusted according to b-jet trigger pT threshold
                                                                        CentralJetEtaCut        = 2.6,
                                                                        # trigger requirement
                                                                        ReqTrigger              = True,
                                                                        Triggers                = Run2MCTriggers,
                                                                        # Mjj requirement
                                                                        ReqVBFMjj               = False,
                                                                        MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                        DoDebug                 = False,
                                                                        #photon requirement (p. rose)
                                                                        PhotonContainerKey      = "Photons",
                                                                        ReqPhoton               = False,
                                                                        PhotonPtCut             = 20.*Units.GeV,
                                                                        CentralPhotonEtaCut     = 2.6)
    else:
        HIGG5D3SkimmingTool = DerivationFramework__SkimmingToolHIGG5VBF(name                    = "HIGG5D3SkimmingTool",
                                                                        JetContainerKey         = "AntiKt4EMTopoJets",
                                                                        # jet multiplicity requirement 2b + 2j
                                                                        ReqNAllJets             = False,
                                                                        NumberOfAllJets         = 4,
                                                                        AllJetPtCut             = 40.*Units.GeV,
                                                                        AllJetEtaCut            = 4.9,
                                                                        # jet multiplicity requirement in track fiducial volume (for b-tagged jet)
                                                                        ReqNCentralJets         = True,
                                                                        NumberOfCentralJets     = 2,
                                                                        CentralJetPtCut         = 15.*Units.GeV,  # to be adjusted according to b-jet trigger pT threshold
                                                                        CentralJetEtaCut        = 2.6,
                                                                        # trigger requirement
                                                                        ReqTrigger              = True,
                                                                        Triggers                = Run2DataTriggers,
                                                                        # Mjj requirement
                                                                        ReqVBFMjj               = False,
                                                                        MjjCut                  = 200.*Units.GeV, # used to control event rate (and according to trigger threshold in Run2)
                                                                        DoDebug                 = False,
                                                                        #photon requirement (p. rose)
                                                                        PhotonContainerKey      = "Photons",
                                                                        ReqPhoton               = False,
                                                                        PhotonPtCut             = 20.*Units.GeV,
                                                                        CentralPhotonEtaCut     = 2.6)        

ToolSvc += HIGG5D3SkimmingTool

#print HIGG5D3SkimmingTool


#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================
higg5d3Seq = CfgMgr.AthSequencer("HIGG5D3Sequence")

# Then apply the TruthWZ fix
if globalflags.DataSource()=='geant4':
    replaceBuggyAntiKt4TruthWZJets(higg5d3Seq,'HIGG5D3')
    replaceBuggyAntiKt10TruthWZJets(higg5d3Seq,'HIGG5D3')

# Jet calibration should come after fat jets
applyJetCalibration_xAODColl(jetalg="AntiKt4EMTopo", sequence=higg5d3Seq)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

higg5d3Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D3Kernel_obj",
    SkimmingTools = [HIGG5D3SkimmingTool]
    )


#====================================================================
# Special jets
#====================================================================
if not "HIGG5D3Jets" in OutputJets:
    OutputJets["HIGG5D3Jets"] = ["AntiKt3PV0TrackJets","AntiKt2PV0TrackJets","AntiKt10LCTopoJets","CamKt12LCTopoJets"]

    if jetFlags.useTruth:
        OutputJets["HIGG5D3Jets"].append("AntiKt4TruthJets")
        OutputJets["HIGG5D3Jets"].append("AntiKt4TruthWZJets")
        # OutputJets["HIGG5D3Jets"].append("AntiKt10TruthWZJets")
        # OutputJets["HIGG5D3Jets"].append("AntiKt10TruthJets")
        #OutputJets["HIGG5D3Jets"].append("CamKt12TruthJets")
        addTrimmedJets("AntiKt", 1.0, "TruthWZ", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d3Seq,outputGroup="HIGG5D3Jets")

    addFilteredJets("CamKt", 1.2, "LCTopo", mumax=1.0, ymin=0.15, includePreTools=False, algseq=higg5d3Seq,outputGroup="HIGG5D3Jets")
    addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, includePreTools=False, algseq=higg5d3Seq,outputGroup="HIGG5D3Jets")


higg5d3Seq += CfgMgr.DerivationFramework__DerivationKernel(
    "HIGG5D3Kernel",
    ThinningTools = thinningTools
    )


applyJetCalibration_CustomColl(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20", sequence=higg5d3Seq)


DerivationFrameworkJob += higg5d3Seq


# # Thinning
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName=HIGG5D3ThinningHelper.ThinningSvc(), outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#===================o=================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
#from DerivationFrameworkHiggs.HIGG5D3ContentList import HIGG5D3Content
from DerivationFrameworkHiggs.HIGG5D3ExtraContent import *
HIGG5D3SlimmingHelper = SlimmingHelper("HIGG5D3SlimmingHelper")

HIGG5D3SlimmingHelper.SmartCollections = [ "Electrons",
                                           "Photons",
                                           "Muons",
                                           "TauJets",
                                           "MET_Reference_AntiKt4EMTopo",
                                           "MET_Reference_AntiKt4LCTopo",
                                           "AntiKt4EMTopoJets",
                                           "AntiKt4LCTopoJets",
                                           "BTagging_AntiKt4EMTopo",
                                           "BTagging_AntiKt4LCTopo",
                                           "InDetTrackParticles",
                                           "PrimaryVertices" ]

HIGG5D3SlimmingHelper.ExtraVariables = ExtraContent
HIGG5D3SlimmingHelper.AllVariables = ExtraContainers
if (is_MC) :
    HIGG5D3SlimmingHelper.ExtraVariables += ExtraContentTruth
    HIGG5D3SlimmingHelper.AllVariables += ExtraContainersTruth

# Add the jet containers to the stream
addJetOutputs(HIGG5D3SlimmingHelper,["HIGG5D3Jets"])
# Add the MET containers to the stream
addMETOutputs(HIGG5D3SlimmingHelper,["AntiKt4LCTopo","Track"])

#HIGG5D3SlimmingHelper.IncludeMuonTriggerContent = True
HIGG5D3SlimmingHelper.IncludeEGammaTriggerContent = True
#HIGG5D3SlimmingHelper.IncludeBPhysTriggerContent = True
#HIGG5D3SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
#HIGG5D3SlimmingHelper.IncludeEtMissTriggerContent = True
HIGG5D3SlimmingHelper.IncludeJetTriggerContent = True
HIGG5D3SlimmingHelper.IncludeBJetTriggerContent = True

HIGG5D3SlimmingHelper.AppendContentToStream(HIGG5D3Stream)
