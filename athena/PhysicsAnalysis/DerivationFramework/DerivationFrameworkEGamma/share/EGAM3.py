#********************************************************************
# EGAM3.py
# Z->eegamma reduction for low-pT electron and photon studies
# reductionConf flag EGAM3 in Reco_tf.py
# author: giovanni.marchiori@cern.ch
#********************************************************************

# notes: 

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *

RecomputeElectronSelectors = True
#RecomputeElectronSelectors = False

#====================================================================
# SKIMMING TOOLS
#====================================================================


#====================================================================
# eegamma selection for photon studies, di-electron triggers
# two opposite-sign medium el, pT>10 GeV, |eta|<2.5, mee>40 GeV
# gamma: reco, ET>10 GeV< |eta|<2.5
#====================================================================

# if skim size too large either require tight electrons (at least one) or raise electron pT threshold (at least one)
#requirement = 'DFCommonElectronsLHMedium && (DFCommonElectrons_pt > 9.5*GeV)'
if RecomputeElectronSelectors :
    requirement = '(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 9.5*GeV)'
else :
    requirement = '(Electrons.Medium || Electrons.DFCommonElectronsLHMedium) && (Electrons.pt > 9.5*GeV)'

from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGInvariantMassTool
EGAM3_EEMassTool = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool",
                                                             Object1Requirements = requirement,
                                                             Object2Requirements = requirement,
                                                             StoreGateEntryName = "EGAM3_DiElectronMass",
                                                             Mass1Hypothesis = 0.511*MeV,
                                                             Mass2Hypothesis = 0.511*MeV,
                                                             Container1Name = "Electrons",
                                                             Container2Name = "Electrons",
                                                             CheckCharge = True,
                                                             DoTransverseMass = False,
                                                             MinDeltaR = 0.0)
ToolSvc += EGAM3_EEMassTool
print EGAM3_EEMassTool


#====================================================================
# eegamma selection for low-pT electron studies with T&P
# tag e: tight, |eta|<2.5, pT>25 GeV
# probe e: reco, ET>7 GeV, no eta cut 
# gamma: tight, ET>10 GeV
#====================================================================
# asymmetric electron cuts/single e trigger, low pT cut for subleading e (for e calibration studies at low pT)
#requirement1 = 'DFCommonElectronsLHTight && (DFCommonElectrons_pt > 24.5*GeV)'
if RecomputeElectronSelectors :
    requirement1 = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
else :
    requirement1 = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
requirement2 = '(Electrons.pt > 6.5*GeV)'

EGAM3_EEMassTool2 = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool2",
                                                              Object1Requirements = requirement1,
                                                              Object2Requirements = requirement2,
                                                              StoreGateEntryName = "EGAM3_DiElectronMass2",
                                                              Mass1Hypothesis = 0.511*MeV,
                                                              Mass2Hypothesis = 0.511*MeV,
                                                              Container1Name = "Electrons",
                                                              Container2Name = "Electrons",
                                                              CheckCharge = True,
                                                              #CheckCharge = False,
                                                              DoTransverseMass = False,
                                                              MinDeltaR = 0.0)

ToolSvc += EGAM3_EEMassTool2
print EGAM3_EEMassTool2

if RecomputeElectronSelectors :
    requirement1 = '(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
else :
    requirement1 = '(Electrons.Tight || Electrons.DFCommonElectronsLHTight) && (Electrons.pt > 24.5*GeV)'
requirement2 = '(ForwardElectrons.pt > 6.5*GeV)'
#requirement2 = '(ForwardElectrons.pt > 9.5*GeV)'

EGAM3_EEMassTool3 = DerivationFramework__EGInvariantMassTool( name = "EGAM3_EEMassTool3",
                                                              Object1Requirements = requirement1,
                                                              Object2Requirements = requirement2,
                                                              StoreGateEntryName = "EGAM3_DiElectronMass3",
                                                              Mass1Hypothesis = 0.511*MeV,
                                                              Mass2Hypothesis = 0.511*MeV,
                                                              Container1Name = "Electrons",
                                                              Container2Name = "ForwardElectrons",
                                                              CheckCharge = True,
                                                              #CheckCharge = False,
                                                              DoTransverseMass = False,
                                                              MinDeltaR = 0.0)
ToolSvc += EGAM3_EEMassTool3
print EGAM3_EEMassTool3



#====================================================================
# SKIMMING TOOL
#====================================================================

if RecomputeElectronSelectors :
    photon_quality = 'Photons.DFCommonPhotonsIsEMTight'
else :
    photon_quality = 'Photons.Tight'

expression = '(count(DFCommonPhotons_et>9.5*GeV)>=1 && count(EGAM3_DiElectronMass > 40.0*GeV)>=1) || (count(DFCommonPhotons_et>9.5*GeV && '+ photon_quality + ')>=1 && (count(EGAM3_DiElectronMass2 > 40.0*GeV)>=1 || count(EGAM3_DiElectronMass3 > 40.0*GeV)>=1))'
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EGAM3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EGAM3SkimmingTool",
                                                                 expression = expression)
ToolSvc += EGAM3SkimmingTool
print "EGAM3 skimming tool:", EGAM3SkimmingTool

#====================================================================
# Gain and cluster energies per layer decoration tool
#====================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations, getClusterEnergyPerLayerDecorator, getClusterEnergyPerLayerDecorations
EGAM3_GainDecoratorTool = GainDecorator()
ToolSvc += EGAM3_GainDecoratorTool

cluster_sizes = (3,5), (5,7), (7,7), (7,11)
EGAM3_ClusterEnergyPerLayerDecorators = [getClusterEnergyPerLayerDecorator(neta, nphi)() for neta, nphi in cluster_sizes]

#================
# THINNING
#================
thinningTools=[]
# TO BE ADDED


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EGAM3Kernel",
                                                                       AugmentationTools = [EGAM3_EEMassTool,EGAM3_EEMassTool2,EGAM3_EEMassTool3,EGAM3_GainDecoratorTool] + EGAM3_ClusterEnergyPerLayerDecorators,
                                                                       SkimmingTools = [EGAM3SkimmingTool],
                                                                       ThinningTools = thinningTools
                                                                       )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EGAM3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EGAM3Stream )
EGAM3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
EGAM3Stream.AcceptAlgs(["EGAM3Kernel"])

#Special lines for thinning
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EGAM3ThinningSvc", outStreams=[evtStream] )


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkEGamma.EGAM3ExtraContent import *

EGAM3SlimmingHelper = SlimmingHelper("EGAM3SlimmingHelper")
EGAM3SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices" ]

# Add egamma trigger objects
EGAM3SlimmingHelper.IncludeEGammaTriggerContent = True

# Extra variables
EGAM3SlimmingHelper.ExtraVariables = ExtraContentAll
EGAM3SlimmingHelper.AllVariables = ExtraContainersPhotons
EGAM3SlimmingHelper.AllVariables += ExtraContainersTrigger
if globalflags.DataSource()!='geant4':
    EGAM3SlimmingHelper.AllVariables += ExtraContainersTriggerDataOnly

if globalflags.DataSource()=='geant4':
    EGAM3SlimmingHelper.ExtraVariables += ExtraContentAllTruth
    EGAM3SlimmingHelper.AllVariables += ExtraContainersTruth

for tool in EGAM3_ClusterEnergyPerLayerDecorators:
    EGAM3SlimmingHelper.ExtraVariables.extend( getClusterEnergyPerLayerDecorations( tool ) )

# This line must come after we have finished configuring EGAM3SlimmingHelper
EGAM3SlimmingHelper.AppendContentToStream(EGAM3Stream)

# Add MET_RefFinalFix
# JRC: COMMENTED TEMPORARILY
#addMETOutputs(EGAM3Stream)

# Add AODCellContainer (have to find how to keep only cells belonging to e/gamma objects)
EGAM3Stream.AddItem("CaloCellContainer#AODCellContainer")

