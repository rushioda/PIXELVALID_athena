#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
def useSmartSlimmingIfSupported(itemPairs, smAllVarlist, addItemList, ContainerNamesAndTypes):
    for i in itemPairs:
        cname = i.split('#')[-1]
        if cname in ContainerNamesAndTypes and itemPairs[i].split('#')[-1].rstrip('.') in ContainerNamesAndTypes:
            smAllVarlist.append(cname)
            print cname, 'added to smartlimming'
        else:
            addItemList.append(i)
            addItemList.append(itemPairs[i])

def getMUON0TriggerContainers():
    return {'xAOD::MuonRoIContainer#LVL1MuonRoIs':'xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.',
'xAOD::L2StandAloneMuonContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfo':'xAOD::L2StandAloneMuonAuxContainer#HLT_xAOD__L2StandAloneMuonContainer_MuonL2SAInfoAux.',
'xAOD::L2CombinedMuonContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfo':'xAOD::L2CombinedMuonAuxContainer#HLT_xAOD__L2CombinedMuonContainer_MuonL2CBInfoAux.',
'xAOD::L2IsoMuonContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfo':'xAOD::L2IsoMuonAuxContainer#HLT_xAOD__L2IsoMuonContainer_MuonL2ISInfoAux.',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuonEFInfo':'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuonEFInfoAux.',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_MuTagIMO_EF':'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_MuTagIMO_EFAux.',
'xAOD::MuonContainer#HLT_xAOD__MuonContainer_eMuonEFInfo':'xAOD::MuonAuxContainer#HLT_xAOD__MuonContainer_eMuonEFInfoAux.',
'xAOD::MuonRoIContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuon':'xAOD::MuonRoIAuxContainer#HLT_xAOD__MuonRoIContainer_L1TopoMuonAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticles':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticlesAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFID':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_EFIDAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTFAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFIDAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTF':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_MuonIso_FTFAux.',
'xAOD::TrackParticleContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig':'xAOD::TrackParticleAuxContainer#HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrigAux.'}


class MuonsDxAODStreamConfigurer:
    commonItems = []
    MUON0OnlyItems = ["CosmicMuonCollection#HLT_CosmicMuons", "MuonCaloEnergyContainer#MuonCaloEnergyCollection"]
    MUON1OnlyItems = ['CaloClusterCellLinkContainer#MuonClusterCollection_links', 'CaloCellContainer#AODCellContainer']
    MUON2OnlyItems = []
    MUON3OnlyItems = []

    MUON0OnlyItems += ['xAOD::TrigNavigation#*','xAOD::TrigNavigationAuxInfo#*'] ## be careful, they could go to smart slimming...

    ### samrt slimming containers
    comSmSlList = []
    smSlContainer = {'MUON0':[], 'MUON1':['AntiKt4LCTopoJets'], 'MUON2':['AntiKt4LCTopoJets'], 'MUON3':[]}

    ### all varaible containers
    commonAllVarList = ["Muons", "PrimaryVertices", "InDetTrackParticles", "MuonSegments", "MuonTruthParticles", "CombinedMuonTrackParticles", "ExtrapolatedMuonTrackParticles", "MuonSpectrometerTrackParticles"]
    MUON0OnlyAllVar = ['Staus','ExtrapolatedStauTrackParticles','CombinedStauTrackParticles','SlowMuons'] # slow muons
    MUON1OnlyAllVar = ['CaloCalTopoClusters', 'MuonClusterCollection']

#     useSmartSlimmingIfSupported(getMUON0TriggerContainers(), MUON0OnlyAllVar, MUON0OnlyItems)
    allVarContainer = {'MUON0':MUON0OnlyAllVar, 'MUON1':MUON1OnlyAllVar, 'MUON2':[], 'MUON3':[]}
    Items = {'MUON0':MUON0OnlyItems, 'MUON1':MUON1OnlyItems, 'MUON2':MUON2OnlyItems, 'MUON3':MUON3OnlyItems}

    ### keep trigger content
    UseTriggerContent = {'MUON0': False} # default is True, MUON0 added by hand

    ### MUON4
    Items['MUON4'] = []
    smSlContainer['MUON4'] = []
    allVarContainer['MUON4'] = []
    UseTriggerContent['MUON4'] = False

    checkContainers = {'MUON0':getMUON0TriggerContainers(), 'MUON1':getMUON0TriggerContainers(), 'MUON2':getMUON0TriggerContainers(), 'MUON3':getMUON0TriggerContainers()}

    ### get final lists
    for s in Items: Items[s]+=commonItems
    for s in allVarContainer: allVarContainer[s]+=commonAllVarList
    for s in smSlContainer: smSlContainer[s]+=comSmSlList

    @classmethod
    def Config(self, stream, configKey):
        ### smart sliming
        from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
        SlHelper = SlimmingHelper(configKey+"SlimmingHelper")
        SlHelper.SmartCollections = self.smSlContainer[configKey]
        useSmartSlimmingIfSupported(self.checkContainers.get(configKey, []),self.allVarContainer[configKey], self.Items[configKey], SlHelper.NamesAndTypes)
        SlHelper.AllVariables = self.allVarContainer[configKey]
#         SlHelper.StaticContent = self.Items[configKey]
#         SlHelper.ExtraVariables = self.extraVariables.get(configKey, [])
        SlHelper.IncludeMuonTriggerContent = self.UseTriggerContent.get(configKey, True)
        SlHelper.AppendContentToStream(stream)

        ### other items
        for item in self.Items[configKey]:
            stream.AddItem(item)

    def show(self):
        print 'Add Items:'
        for i in self.Items: print i,'=',self.Items[i]
        print 'Smart slimming:'
        for i in self.smSlContainer: print i,'=',self.smSlContainer[i]
        print 'Keep all varaibles:'
        for i in self.allVarContainer: print i,'=',self.allVarContainer[i]
        print 'Keep trigger content:'
        for i in self.UseTriggerContent: print i,'=',self.UseTriggerContent[i]


if __name__ == '__main__':
    a = MuonsDxAODStreamConfigurer()
    a.show()
