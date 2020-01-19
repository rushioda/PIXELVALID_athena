#********************************************************************
# SUSY4.py 
# reductionConf flag SUSY4 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY4Stream )
SUSY4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY4Stream.AcceptAlgs(["SUSY4KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY4ThinningHelper = ThinningHelper( "SUSY4ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY4 = CfgMgr.AthSequencer("SeqSUSY4")
DerivationFrameworkJob += SeqSUSY4


#====================================================================
# Trigger navigation thinning
#====================================================================
# To be added.
SUSY4ThinningHelper.AppendToStream( SUSY4Stream )


#====================================================================
# THINNING TOOLS 
#====================================================================

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
SUSY4TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY4TPThinningTool",
                                                                 ThinningService         = SUSY4ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY4TPThinningTool
thinningTools.append(SUSY4TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY4MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY4MuonTPThinningTool",
                                                                         ThinningService         = SUSY4ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY4MuonTPThinningTool
thinningTools.append(SUSY4MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY4ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY4ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY4ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	    = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY4ElectronTPThinningTool
thinningTools.append(SUSY4ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY4PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY4PhotonTPThinningTool",
										 ThinningService	 = SUSY4ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY4PhotonTPThinningTool
thinningTools.append(SUSY4PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY4TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY4TauTPThinningTool",
                                                                        ThinningService = SUSY4ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY4TauTPThinningTool
thinningTools.append(SUSY4TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY4TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY4TruthThinningTool",
                                                       ThinningService              = SUSY4ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteBHadrons                = True,
                                                       WriteGeant                   = False,
                                                       GeantPhotonPtThresh          = 20000,
                                                       WriteTauHad                  = True,
                                                       PartonPtThresh               = -1.0,
                                                       WriteBSM                     = True,
                                                       WriteBosons                  = True,
                                                       WriteBosonProducts           = True,
                                                       WriteBSMProducts             = True,
                                                       WriteTopAndDecays            = True,
                                                       WriteEverything              = False,
                                                       WriteAllLeptons              = False,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY4TruthThinningTool
  thinningTools.append(SUSY4TruthThinningTool)

AugmentationTools = []
###############
#TRUTH3 content
###############

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__Truth3CollectionMaker

SUSY4TRUTH3MuonTool = DerivationFramework__Truth3CollectionMaker(name                   = "SUSY4TRUTH3MuonTool",
							    NewCollectionName       = "SUSY4TRUTHMuons",
							    ParticleSelectionString = "abs(TruthParticles.pdgId) == 13 && TruthParticles.status==1")
if globalflags.DataSource()=='geant4':
	ToolSvc += SUSY4TRUTH3MuonTool
	AugmentationTools.append(SUSY4TRUTH3MuonTool)

SUSY4TRUTH3ElectronTool = DerivationFramework__Truth3CollectionMaker(name               = "SUSY4TRUTH3ElectronTool",
                                                         NewCollectionName       = "SUSY4TRUTHElectrons",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 11 && TruthParticles.status==1")
if globalflags.DataSource()=='geant4':
	ToolSvc += SUSY4TRUTH3ElectronTool
	AugmentationTools.append(SUSY4TRUTH3ElectronTool)

SUSY4TRUTH3PhotonTool = DerivationFramework__Truth3CollectionMaker(name                 = "SUSY4TRUTH3PhotonTool",
                                                         NewCollectionName       = "SUSY4TRUTHPhotons",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 22 && TruthParticles.pt > 50.*GeV")
if globalflags.DataSource()=='geant4':
	ToolSvc += SUSY4TRUTH3PhotonTool
	AugmentationTools.append(SUSY4TRUTH3PhotonTool)

SUSY4TRUTH3TauTool = DerivationFramework__Truth3CollectionMaker(name                    = "SUSY4TRUTH3TauTool",
                                                         NewCollectionName       = "SUSY4TRUTHTaus",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 15")
if globalflags.DataSource()=='geant4':
	ToolSvc += SUSY4TRUTH3TauTool
	AugmentationTools.append(SUSY4TRUTH3TauTool)

SUSY4TRUTH3SherpaMETool = DerivationFramework__Truth3CollectionMaker(name                   = "SUSY4TRUTH3SherpaMETool",
                                                         NewCollectionName       = "SUSY4TRUTHSherpaME",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) < 100 || ((abs(TruthParticles.pdgId) >= 1000001) && (abs(TruthParticles.pdgId) <= 1000039))) && TruthParticles.status==3")
if globalflags.DataSource()=='geant4':
	ToolSvc += SUSY4TRUTH3SherpaMETool
	AugmentationTools.append(SUSY4TRUTH3SherpaMETool)

##############
##############
##############
	
#====================================================================
# SKIMMING TOOL 
#====================================================================
applyJetCalibration_xAODColl("AntiKt4EMTopo")
muonsRequirements = '(Muons.pt >= 15.*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(Electrons.pt > 15.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
#jetRequirements = '(AntiKt4EMTopoJets.JetConstitScaleMomentum_pt - AntiKt4EMTopoJets.ActiveArea4vec_pt * Kt4EMTopoEventShape.Density >= 22.*GeV && (abs(AntiKt4EMTopoJets.JetConstitScaleMomentum_eta)<2.2))'
jetRequirements = '(AntiKt4EMTopoJets.DFCommonJets_Calib_pt >= 40.*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.5)'

expression = '(((count('+electronsRequirements+') + count('+muonsRequirements+') >= 1) && (count('+jetRequirements+') >=3)) || ((count('+electronsRequirements+') + count('+muonsRequirements+') >= 2) && (count('+jetRequirements+') >=2)) || (HLT_6j45_0eta240) || (HLT_6j45_0eta240_L14J20) || (HLT_6j50_0eta240_L14J20) || (HLT_7j45) || (HLT_5j70) || (HLT_5j85) || (HLT_5j45) || (HLT_6j45) )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY4SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY4SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY4SkimmingTool

from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg

#====================================================================
# ISOLATION TOOL 
#====================================================================
#Track selection
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

TrackIsoTool500 = xAOD__TrackIsolationTool("TrackIsoTool500")
TrackIsoTool500.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool500.TrackSelectionTool.minPt= 500.
TrackIsoTool500.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool500

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
import ROOT, cppyy
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
Pt1000IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt1000IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt1000_'
                                                               )
Pt500IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt500IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool500,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt500_'
                                                               )
ToolSvc += Pt1000IsoTrackDecorator
ToolSvc += Pt500IsoTrackDecorator

AugmentationTools.append(Pt1000IsoTrackDecorator)
AugmentationTools.append(Pt500IsoTrackDecorator)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY4KernelSkim",
  SkimmingTools = [SUSY4SkimmingTool]
)
# Calibrating jets in derivation framework not yet possible (no alg)
#from JetRec.JetRecCalibrationFinder import jrcf
#JetCalTool = jrcf.find("AntiKt", 0.4, "EMTopo", "aroj", "reco", "Kt4")
#ToolSvc += JetCalTool
#DerivationFrameworkJob += JetCalTool


#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY4")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY4)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY4GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY4GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY4 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY4KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)



#====================================================================
# ADD THE FAT JET THAT WILL BE IN THE PRE-RECOMMENDATION - NOT INCLUDED ANYMORE
#====================================================================
# included in ExtendedJetCommon.py since 15.04.2015 
#from JetRec.JetRecStandard import jtm
#from JetRec.JetRecConf import JetAlgorithm
#from JetRec.JetRecFlags import jetFlags
#includePreTools=False
#if jetFlags.useTruth:
#        includePreTools=True
#        DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.2, ptfrac=0.05, includePreTools=True)
#if includePreTools==False:
#        DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05, includePreTools=True)
#else:
#        DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05)

#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY4SlimmingHelper = SlimmingHelper("SUSY4SlimmingHelper")
SUSY4SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets", "AntiKt4EMTopoJets", "MET_Reference_AntiKt4EMTopo", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices", "AntiKt4EMPFlowJets"]
SUSY4SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "InDetTrackParticles", "METAssoc_AntiKt4EMPFlow", "MET_Core_AntiKt4EMPFlow", "MET_Reference_AntiKt4EMPFlow"]
SUSY4SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                    "Electrons.Loose.Medium.Tight",
                    "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                    "Photons.author.Loose.Tight",
                    "AntiKt4EMTopoJets.JvtJvfcorr.JvtRpt.Jvt.TruthLabelID.BchCorrCell.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt1000.TrackWidthPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                    "AntiKt4EMPFlowJets.JvtJvfcorr.JvtRpt.Jvt.TruthLabelID.BchCorrCell.NumTrkPt1000.NumTrkPt500.SumPtTrkPt500.SumPtTrkPt1000.TrackWidthPt1000.TrackWidthPt500",
                    "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
                    "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                    "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                    "TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink",
                    "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                    "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelID",
                    "InDetTrackParticles.truthOrigin.truthType.truthMatchProbability.TrkIso*"]
SUSY4SlimmingHelper.IncludeMuonTriggerContent   = False
SUSY4SlimmingHelper.IncludeEGammaTriggerContent = False
SUSY4SlimmingHelper.IncludeBPhysTriggerContent  = False 
SUSY4SlimmingHelper.IncludeJetTriggerContent    = False
SUSY4SlimmingHelper.IncludeTauTriggerContent    = False
SUSY4SlimmingHelper.IncludeEtMissTriggerContent = False
SUSY4SlimmingHelper.IncludeBJetTriggerContent   = False


#addJetOutputs(SUSY4SlimmingHelper,["LargeR"])

if globalflags.DataSource()=='geant4':
  SUSY4Stream.AddItem( "xAOD::TruthParticleContainer#SUSY4TRUTHMuons*")
  SUSY4Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSY4TRUTHMuons*")
  SUSY4Stream.AddItem( "xAOD::TruthParticleContainer#SUSY4TRUTHElectrons*")
  SUSY4Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSY4TRUTHElectrons*")
  SUSY4Stream.AddItem( "xAOD::TruthParticleContainer#SUSY4TRUTHSherpaME*")
  SUSY4Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSY4TRUTHSherpaME*")
  #SUSY4Stream.AddItem( "xAOD::TruthParticleContainer#SUSY4TRUTHTaus*")
  #SUSY4Stream.AddItem( "xAOD::TruthParticleAuxContainer#SUSY4TRUTHTaus*")
  
  SUSY4SlimmingHelper.StaticContent += [
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux.",
  ]

SUSY4SlimmingHelper.AppendContentToStream(SUSY4Stream) # AppendContentToStream must always go last
