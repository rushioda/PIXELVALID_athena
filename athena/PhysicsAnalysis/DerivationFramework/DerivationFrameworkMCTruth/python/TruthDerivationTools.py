# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc

# Set up the MCTruthClassifier
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
DFCommonTruthClassifier = MCTruthClassifier(name = "DFCommonTruthClassifier",
                                    ParticleCaloExtensionTool="") 
ToolSvc += DFCommonTruthClassifier

#==============================================================================
# Schedule the tools for adding new truth collection
#==============================================================================

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau

DFCommonTruthMuonTool = DerivationFramework__TruthCollectionMaker(name                   = "DFCommonTruthMuonTool",
                                                         NewCollectionName       = "TruthMuons",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 13) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset))
ToolSvc += DFCommonTruthMuonTool
DFCommonTruthElectronTool = DerivationFramework__TruthCollectionMaker(name               = "DFCommonTruthElectronTool",
                                                         NewCollectionName       = "TruthElectrons",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 11) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset))
ToolSvc += DFCommonTruthElectronTool
DFCommonTruthPhotonTool = DerivationFramework__TruthCollectionMaker(name                 = "DFCommonTruthPhotonTool",
                                                         NewCollectionName       = "TruthPhotons",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset))
ToolSvc += DFCommonTruthPhotonTool
# this tool is needed for making TruthPhotons from sim samples, where extra cuts are needed
DFCommonTruthPhotonToolSim = DerivationFramework__TruthCollectionMaker(name              = "DFCommonTruthPhotonToolSim",
                                                         NewCollectionName       = "TruthPhotons",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && ((TruthParticles.classifierParticleOrigin != 42) || (TruthParticles.pt > 20.0*GeV)) && ( TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset)+")")
ToolSvc += DFCommonTruthPhotonToolSim


# Tau truth handled in TauCommon
#DFCommonTruthTauTool = DerivationFramework__TruthCollectionMakerTau(name                    = "DFCommonTruthTauTool",
#                                                             NewCollectionName       = "TruthTaus",
#                                                             MCTruthClassifier       = DFCommonTruthClassifier)
#ToolSvc += DFCommonTruthTauTool

neutrinoexpression = "((abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.status == 1)) && TruthParticles.barcode < "+str(DerivationFrameworkSimBarcodeOffset)
DFCommonTruthNeutrinoTool = DerivationFramework__TruthCollectionMaker(name                 = "DFCommonTruthNeutrinoTool",
                                                             NewCollectionName       = "TruthNeutrinos",
                                                             ParticleSelectionString = neutrinoexpression)
ToolSvc += DFCommonTruthNeutrinoTool

TRUTH3TopTool = DerivationFramework__TruthCollectionMaker(name                   = "TRUTH3TopTool",
                                                          NewCollectionName       = "TruthTop",
                                                          ParticleSelectionString = "(abs(TruthParticles.pdgId) == 6)",
                                                          Do_Compress = True,
                                                          )
ToolSvc += TRUTH3TopTool

TRUTH3BosonTool = DerivationFramework__TruthCollectionMaker(name                   = "TRUTH3BosonTool",
                                                            NewCollectionName       = "TruthBoson",
                                                            ParticleSelectionString = "(abs(TruthParticles.pdgId) == 23 || abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) == 25)",
                                                            Do_Compress = True,
                                                            Do_Sherpa= True)
ToolSvc += TRUTH3BosonTool

TRUTH3BSMTool = DerivationFramework__TruthCollectionMaker(name                   = "TRUTH3BSMTool",
                                                          NewCollectionName       = "TruthBSM",
                                                          ParticleSelectionString = "( (31<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<38) || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==41 || abs(TruthParticles.pdgId)==42 || abs(TruthParticles.pdgId)== 7 || abs(TruthParticles.pdgId)== 8 || (1000000<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<1000040) || (2000000<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<2000040) )",
                                                          Do_Compress = True)
ToolSvc += TRUTH3BSMTool

#==============================================================================
# Decoration tools
#==============================================================================

#add the 'decoration' tool to dress the main truth collection with the classification
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
DFCommonTruthClassificationTool = DerivationFramework__TruthClassificationDecorator(name = "DFCommonTruthClassificationTool",
                                                                             ParticlesKey = "TruthParticles",
                                                                             MCTruthClassifier = DFCommonTruthClassifier)  
ToolSvc += DFCommonTruthClassificationTool

#add the 'decoration' tools for dressing and isolation
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthDressingTool
DFCommonTruthElectronDressingTool = DerivationFramework__TruthDressingTool(name = "DFCommonTruthElectronDressingTool",
                                                                  dressParticlesKey = "TruthElectrons",
                                                                  usePhotonsFromHadrons	 	= False,
                                                                  dressingConeSize = 0.1,
                                                                  particleIDsToDress = [11]
                                                                  )
ToolSvc += DFCommonTruthElectronDressingTool
DFCommonTruthMuonDressingTool = DerivationFramework__TruthDressingTool(name = "DFCommonTruthMuonDressingTool",
                                                                  dressParticlesKey = "TruthMuons",
                                                                  usePhotonsFromHadrons	 	= False,
                                                                  dressingConeSize = 0.1,
                                                                  particleIDsToDress = [13]
                                                                  )
ToolSvc += DFCommonTruthMuonDressingTool

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
DFCommonTruthElectronIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthElectronIsolationTool1",
                                                                  isoParticlesKey = "TruthElectrons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += DFCommonTruthElectronIsolationTool1
DFCommonTruthElectronIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthElectronIsolationTool2",
                                                                  isoParticlesKey = "TruthElectrons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += DFCommonTruthElectronIsolationTool2
DFCommonTruthMuonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthMuonIsolationTool1",
                                                                  isoParticlesKey = "TruthMuons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += DFCommonTruthMuonIsolationTool1
DFCommonTruthMuonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthMuonIsolationTool2",
                                                                  isoParticlesKey = "TruthMuons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += DFCommonTruthMuonIsolationTool2
DFCommonTruthPhotonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthPhotonIsolationTool1",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += DFCommonTruthPhotonIsolationTool1
DFCommonTruthPhotonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "DFCommonTruthPhotonIsolationTool2",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += DFCommonTruthPhotonIsolationTool2

