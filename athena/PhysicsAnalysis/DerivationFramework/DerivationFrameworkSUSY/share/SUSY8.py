#====================================================================
# Derivation targeting searches for charged stable massive particles
# reductionConf flag SUSY8 in Reco_tf.py    
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY8Stream )
SUSY8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY8Stream.AcceptAlgs(["SUSY8KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY8ThinningHelper = ThinningHelper( "SUSY8ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY8 = CfgMgr.AthSequencer("SeqSUSY8")
DerivationFrameworkJob += SeqSUSY8


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY8TriggerList import *

jetMETTriggers  = SUSY8JetMETTriggers
muonTriggers    = SUSY8MuonTriggers
dimuonTriggers  = SUSY8DimuonTriggers
xeTriggers      = SUSY8METTriggers

trigNavThinningEx = '|'.join(jetMETTriggers+xeTriggers+muonTriggers+dimuonTriggers)
SUSY8ThinningHelper.TriggerChains = trigNavThinningEx

SUSY8ThinningHelper.AppendToStream( SUSY8Stream )


#====================================================================
# TRIGGER SKIMMING
#====================================================================

expression = '(' + ' || '.join(jetMETTriggers+xeTriggers+muonTriggers+dimuonTriggers) + ')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY8SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY8SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY8SkimmingTool


#====================================================================
# THINNING TOOL
#====================================================================\

thinning_expression = "(InDetTrackParticles.pt > 10.0*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (abs(DFCommonInDetTrackZ0AtPV) < 10.0)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY8MetTPThinningTool = DerivationFramework__TrackParticleThinning( name               = "SUSY8MetTPThinningTool",
                                                                ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY8MetTPThinningTool
thinningTools.append(SUSY8MetTPThinningTool)

# TrackParticles associated with jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY8JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          	= "SUSY8JetTPThinningTool",
                                                                ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                JetKey                  = "AntiKt4EMTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY8JetTPThinningTool
thinningTools.append(SUSY8JetTPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY8MuonTPThinningTool",
                                                                         ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY8MuonTPThinningTool
thinningTools.append(SUSY8MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                      = "SUSY8ElectronTPThinningTool",
                                                                                 ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Electrons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY8ElectronTPThinningTool
thinningTools.append(SUSY8ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                        = "SUSY8PhotonTPThinningTool",
                                                                                 ThinningService         = SUSY8ThinningHelper.ThinningSvc(),
                                                                                 SGKey                   = "Photons",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY8PhotonTPThinningTool
thinningTools.append(SUSY8PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY8TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            	= "SUSY8TauTPThinningTool",
                                                                        ThinningService 	= SUSY8ThinningHelper.ThinningSvc(),
                                                                        TauKey          	= "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY8TauTPThinningTool
thinningTools.append(SUSY8TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY8TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY8TruthThinningTool",
                                                       ThinningService              = SUSY8ThinningHelper.ThinningSvc(),
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
  ToolSvc += SUSY8TruthThinningTool
  thinningTools.append(SUSY8TruthThinningTool)


#====================================================================
# TrackParticleCaloCellDecoration
#====================================================================

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TrackParticleCaloCellDecorator
SUSY8_TrackParticleCaloCellDecorator = DerivationFramework__TrackParticleCaloCellDecorator( name = "SUSY8_TrackParticleCaloCellDecorator",
											    DecorationPrefix = "SUSY8",
	                                                                      	   	    ContainerName = "InDetTrackParticles")

ToolSvc += SUSY8_TrackParticleCaloCellDecorator
AugmentationTools.append(SUSY8_TrackParticleCaloCellDecorator)
print SUSY8_TrackParticleCaloCellDecorator


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY8KernelSkim",
  SkimmingTools = [SUSY8SkimmingTool]
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY8")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY8)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY8GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY8GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY8 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY8KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkSUSY.SUSY8ContentList import *

SUSY8SlimmingHelper = SlimmingHelper("SUSY8SlimmingHelper")

SUSY8SlimmingHelper.SmartCollections = SUSY8SmartContent
SUSY8SlimmingHelper.ExtraVariables = SUSY8ExtraVariables
SUSY8SlimmingHelper.AllVariables = SUSY8AllVariablesContent

SUSY8SlimmingHelper.IncludeMuonTriggerContent = True
SUSY8SlimmingHelper.IncludeJetTriggerContent = True
SUSY8SlimmingHelper.IncludeEtMissTriggerContent = True

SUSY8SlimmingHelper.AppendContentToStream(SUSY8Stream)
