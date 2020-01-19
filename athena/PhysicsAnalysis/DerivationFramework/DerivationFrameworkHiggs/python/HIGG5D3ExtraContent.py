# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

ExtraContent=[
    "Muons.clusterLink.EnergyLoss.energyLossType",
    "AntiKt4LCTopoJets.TrackWidthPt500.GhostTrackCount.Jvt.JvtJvfcorr.JvtRpt",
    "AntiKt4LCTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m",
    "AntiKt4EMTopoJets.TrackWidthPt500.GhostTrackCount.Jvt.JvtJvfcorr.JvtRpt",
    "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m",
    "AntiKt4EMTopoJets.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.DFCommonJets_Calib_m",
    "Photons.f3core"]

ExtraContentTruth=[
    "AntiKt4EMTopoJets.ConeTruthLabelID",
    "AntiKt4LCTopoJets.ConeTruthLabelID"]

ExtraContainers=[
    #"BTagging_AntiKt10LCTopo",
    "BTagging_AntiKt3Track",
    "BTagging_AntiKt2Track",
    #"xTrigDecision", # for xAOD::TrigDecision_v1
    #"xTrigDecisionAux", # for xAOD::TrigDecisionAuxInfo_v1
    #"TrigDecision", # for TrigDec::TrigDecision
    #"HLT_TrigEFBjetContainer_EFBjetFex", # for TrigEFBjetContainer
    #"HLT_TrigL2BjetContainer_L2BjetFex", # for TrigL2BjetContainer
    #"HLT_JetCollection_TrigJetRec",
    "MuonSegments"]


ExtraContainersTruth=[
    "TruthEvents",
    "TruthParticles",
    "TruthVertices",
    "MuonTruthParticles"]
