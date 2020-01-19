#====================================================================
# JETM2.py 
# reductionConf flag JETM2 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
#from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
#
from DerivationFrameworkJetEtMiss.METCommon import *
#
if globalflags.DataSource()=='geant4':
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    from DerivationFrameworkTau.TauTruthCommon import *

#====================================================================
# SKIMMING TOOL 
#====================================================================

from DerivationFrameworkJetEtMiss.TriggerLists import *
electronTriggers = singleElTriggers
muonTriggers = singleMuTriggers

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 25*GeV) && (Electrons.DFCommonElectronsLHMedium)) >= 1'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 25*GeV) && (Muons.DFCommonMuonsPreselection)) >= 1'])
muonSelection = '( (' + mutrigsel + ') && (' + muofflinesel + ') )'

expression = '( ' + electronSelection + ' || ' + muonSelection + ' )'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM2SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM2SkimmingTool1",
                                                                    expression = expression)
ToolSvc += JETM2SkimmingTool

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM2Stream )
JETM2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM2Stream.AcceptAlgs(["JETM2Kernel"])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM2ThinningHelper = ThinningHelper( "JETM2ThinningHelper" )
JETM2ThinningHelper.TriggerChains = orstr.join(electronTriggers+muonTriggers)
JETM2ThinningHelper.AppendToStream( JETM2Stream )

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name     = "JETM2MuonTPThinningTool",
                                                                    ThinningService         = JETM2ThinningHelper.ThinningSvc(),
                                                                    MuonKey                 = "Muons",
                                                                    InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM2MuonTPThinningTool
thinningTools.append(JETM2MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM2ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM2ElectronTPThinningTool",
                                                                               ThinningService         = JETM2ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM2ElectronTPThinningTool
thinningTools.append(JETM2ElectronTPThinningTool)

# TrackParticles associated with photons
JETM2PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "JETM2PhotonTPThinningTool",
                                                                             ThinningService         = JETM2ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM2PhotonTPThinningTool
thinningTools.append(JETM2PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "JETM2TauTPThinningTool",
                                                                        ThinningService = JETM2ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM2TauTPThinningTool
thinningTools.append(JETM2TauTPThinningTool)

# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and globalflags.DataSource()=='geant4':
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))" # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                 # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))" # Photon
  
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+'||'+truth_cond_Gluon+' || '+truth_cond_Photon+')'
  
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM2TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM2TruthThinningTool",
                                                                        ThinningService        = JETM2ThinningHelper.ThinningSvc(),
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
  
    ToolSvc += JETM2TruthThinningTool
    thinningTools.append(JETM2TruthThinningTool)
    #thinningTools.append(ToolSvc.DFCommonTruthPhotonThinning)

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm2Seq = CfgMgr.AthSequencer("JETM2Sequence")
DerivationFrameworkJob += jetm2Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm2Seq += CfgMgr.DerivationFramework__DerivationKernel("JETM2Kernel",
                                                         SkimmingTools = [JETM2SkimmingTool],
                                                         ThinningTools = thinningTools)

#=======================================
# SCHEDULE CUSTOM MET RECONSTRUCTION
#=======================================

if globalflags.DataSource()=='geant4':
    addMETTruthMap('AntiKt4EMTopo')
    addMETTruthMap('AntiKt4LCTopo')
    addMETTruthMap('AntiKt4EMPFlow')
    scheduleMETAssocAlg(jetm2Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM2SlimmingHelper = SlimmingHelper("JETM2SlimmingHelper")
JETM2SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets"]
JETM2SlimmingHelper.AllVariables = ["BTagging_AntiKt4LCTopo", "BTagging_AntiKt4EMTopo",# "CaloCalTopoClusters",
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "MuonSegments"
                                    ]
JETM2SlimmingHelper.ExtraVariables = ["Muons.energyLossType.EnergyLoss.ParamEnergyLoss.MeasEnergyLoss.EnergyLossSigma.MeasEnergyLossSigma.ParamEnergyLossSigmaPlus.ParamEnergyLossSigmaMinus",
                                      "TauJets.IsTruthMatched.truthParticleLink.truthJetLink"]
for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM2SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM2SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

# Trigger content
JETM2SlimmingHelper.IncludeMuonTriggerContent = True
JETM2SlimmingHelper.IncludeEGammaTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM2SlimmingHelper,["SmallR"])
# Add the MET containers to the stream
addMETOutputs(JETM2SlimmingHelper,["Diagnostic","Assocs","TruthAssocs","Track"])

JETM2SlimmingHelper.AppendContentToStream(JETM2Stream)
