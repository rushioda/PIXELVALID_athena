# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

ExtraContentElectrons=[
    "Electrons.etcone20",
    "Electrons.etcone30",
    "Electrons.etcone40",
    "Electrons.ptcone20",
    "Electrons.ptcone30",
    "Electrons.ptcone40",
    "Electrons.Loose",
    "Electrons.Medium",
    "Electrons.Tight",
    "Electrons.DFCommonElectronsLHLoose",
    "Electrons.DFCommonElectronsLHMedium",
    "Electrons.DFCommonElectronsLHTight",
    "Electrons.DFCommonElectronsML",
    "Electrons.author",
    "Electrons.OQ"
    ]

ExtraElectronsTruth=[
    "Electrons.truthOrigin",
    "Electrons.truthType",
    "Electrons.truthParticleLink"]

ExtraContentMuons=[
#    "Muons.DFCommonGoodMuon",
#    "Muons.DFCommonMuonsLoose",
#    "Muons.DFCommonMuonsMedium",
#    "Muons.DFCommonMuonsTight",
    "Muons.DFCommonMuonsPreselection",
    "Muons.ptcone20",
    "Muons.ptcone30",
    "Muons.ptcone40",
    "Muons.etcone20",
    "Muons.etcone30",
    "Muons.etcone40"]

ExtraMuonsTruth=[
    ]

ExtraContentPhotons=[
]

ExtraContentPrimaryVertices=["PrimaryVertices.x.y.sumPt2"]

ExtraPhotonsTruth=[
]

ExtraContentGSFConversionVertices=[
	"GSFConversionVertices.x",
	"GSFConversionVertices.y",
	"GSFConversionVertices.z",
	"GSFConversionVertices.px",
	"GSFConversionVertices.py",
	"GSFConversionVertices.pz",
	"GSFConversionVertices.pt1",
	"GSFConversionVertices.pt2",
	"GSFConversionVertices.etaAtCalo",
	"GSFConversionVertices.phiAtCalo",
	"GSFConversionVertices.trackParticleLinks"
	]

ExtraContentHLTPhotons=[
	"HLT_xAOD__PhotonContainer_egamma_Photons.e.pt.m.author.Rhad.Rhad1.e277.Reta.Rphi.weta2.f1.fracs1.wtots1.weta1.DeltaE.Eratio.caloClusterLinks",
	"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex.calE.calEta.calPhi.calM.e_sampl.eta_sampl.etaCalo.phiCalo.ETACALOFRAME.PHICALOFRAME"
]

#cells = ("Cells5x5","Cells3x5","Cells3x7","Cells7x11")
#layers_gains =  (	"_Lr0", "_Lr1", "_Lr2", "_Lr3",
#					"_Lr0_LwG", "_Lr1_LwG", "_Lr2_LwG", "_Lr3_LwG",
#					"_Lr0_LwG", "_Lr1_MdG", "_Lr2_MdG", "_Lr3_MdG",
#					"_Lr0_LwG", "_Lr1_HiG", "_Lr2_HiG", "_Lr3_HiG" )
#
#for cell in cells:
#	ExtraContentPhotons.append("Photons."+cell)
#	for layer in layers_gains:
#		ExtraContentPhotons.append("Photons."+cell+layer)
#
#for cell in cells:
#	ExtraContentElectrons.append("Electrons."+cell)
#	for layer in layers_gains:
#		ExtraContentElectrons.append("Electrons."+cell+layer)
from DerivationFrameworkCalo.DerivationFrameworkCaloFactories import GainDecorator, getGainDecorations
GainDecoratorTool = GainDecorator()
ExtraContentPhotons.extend( getGainDecorations(GainDecoratorTool) )
ExtraContentElectrons.extend( getGainDecorations(GainDecoratorTool) )

ExtraContentAll=ExtraContentElectrons+ExtraContentMuons+ExtraContentPhotons+ExtraContentGSFConversionVertices+ExtraContentHLTPhotons+ExtraContentPrimaryVertices
ExtraContentAllTruth=ExtraElectronsTruth+ExtraMuonsTruth+ExtraPhotonsTruth

ExtraContainersTruth=["TruthEvents", 
                      "TruthParticles",
                      "TruthVertices",
                      "AntiKt4TruthJets",
		      "egammaTruthParticles",
		      "MuonTruthParticles"
                      #,"BTagging_AntiKt4TruthWZ"
                      #,"AntiKt4TruthWZJets"
                      ]

ExtraContainersPhotons=["Photons",
                        "GSFTrackParticles",
                        "egammaClusters",
                        "ForwardElectrons",
                        "ForwardElectronClusters",
                        ]

# for trigger studies
ExtraContainersTrigger=[
	# to access the HLT egamma xAOD collections
	"HLT_xAOD__ElectronContainer_egamma_Electrons",
	"HLT_xAOD__PhotonContainer_egamma_Photons",
	"HLT_xAOD__TrigElectronContainer_L2ElectronFex",
	"HLT_xAOD__TrigPhotonContainer_L2PhotonFex",
	# to access information about EF clusters and tracks
	"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFID",
	# for L1 studies
	"LVL1EmTauRoIs",
	"HLT_TrigPassBitsCollection_passbits",
	"HLT_TrigPassFlagsCollection_passflags",
	"HLT_TrigRoiDescriptorCollection_initialRoI"
	]

ExtraContainersTriggerDataOnly=[
	"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma",
	"HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_FTF",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_L2ID",
	"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_IDTrig"
	]
