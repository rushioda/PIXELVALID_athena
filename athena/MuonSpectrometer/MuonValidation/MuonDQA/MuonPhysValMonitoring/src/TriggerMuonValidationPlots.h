/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_TRIGGERMUONVALIDATIONPLOTS_H

#include "ResoTriggerMuonPlots.h"
#include "TriggerEfficiencyPlots.h"
#include "MuonHistUtils/L1TriggerMuonPlots.h"
#include "MuonHistUtils/HLTriggerMuonPlots.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODMuon/MuonAuxContainer.h"

class TriggerMuonValidationPlots:public PlotBase {
  public:
  TriggerMuonValidationPlots(PlotBase* pParent, std::string sDir, std::vector<unsigned int> authors, bool isData, bool doTrigMuonL1Validation, bool doTrigMuonL2Validation, bool doTrigMuonEFValidation, std::vector<std::vector<std::string>> ChainSeed,std::vector<std::string> L1MuonItems);

    float PlateauTreshold;
    virtual ~TriggerMuonValidationPlots();
    void fill(const xAOD::MuonRoI &TrigL1mu);
    void fill(const xAOD::L2StandAloneMuon &L2SAmu);
    void fill(const xAOD::L2CombinedMuon &L2CBmu);
    void fill(const xAOD::Muon& Trigmu);
    void fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
    void fill(const xAOD::L2StandAloneMuon& L2SAmu, const xAOD::Muon& Recomu);
    void fill(const xAOD::L2CombinedMuon& L2CBmu, const xAOD::Muon& Recomu);
    void fillNumEff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillDenEff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillNumL1Eff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillDenL1Eff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillFeatPlots(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillFeatPlots(const xAOD::MuonRoI& TrigL1mu, std::string selectedChain);
    void fillDenRELEff(const xAOD::Muon& Trigmu, std::string selectedChain);
    void fillNumRELEff(const xAOD::Muon& Trigmu, std::string selectedChain);

    std::vector<unsigned int> m_selectedAuthors;
    std::vector<std::string> m_chains;
    std::vector<std::string> m_seeds;
    L1TriggerMuonPlots* m_oL1TriggerMuonPlots;    
    std::vector<HLTriggerMuonPlots*> m_oEFTriggerMuonPlots;
    std::vector<HLTriggerMuonPlots*> m_oL2TriggerMuonPlots;
    std::vector<Muon::ResoTriggerMuonPlots*> m_oEFTriggerMuonBarrelResolutionPlots;
    std::vector<Muon::ResoTriggerMuonPlots*> m_oEFTriggerMuonEndcapsResolutionPlots;
    std::vector<Muon::ResoTriggerMuonPlots*> m_oEFTriggerMuonResolutionPlots;
    std::vector<Muon::ResoTriggerMuonPlots*> m_oL2TriggerMuonBarrelResolutionPlots;
    std::vector<Muon::ResoTriggerMuonPlots*> m_oL2TriggerMuonEndcapsResolutionPlots;
    std::vector<Muon::ResoTriggerMuonPlots*> m_oL2TriggerMuonResolutionPlots;
    std::vector<TriggerEfficiencyPlots*> m_oL1TriggerChainEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oL1TriggerChainBarrelEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oL1TriggerChainEndcapsEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainBarrelEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainEndcapsEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainRELEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainBarrelRELEfficiencyPlots;
    std::vector<TriggerEfficiencyPlots*> m_oEFTriggerChainEndcapsRELEfficiencyPlots;

 private:
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu);
    void fillTriggerMuonPlots(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
    void fillTriggerMuonPlots(const xAOD::L2StandAloneMuon &L2SAmu, const xAOD::Muon &Recomu);
    void fillTriggerMuonPlots(const xAOD::L2CombinedMuon &L2CBmu, const xAOD::Muon &Recomu);
    void fillTriggerMuonPlots(const xAOD::MuonRoI& TrigL1mu);
    void fillTriggerMuonPlots(const xAOD::L2StandAloneMuon& L2SAmu);
    void fillTriggerMuonPlots(const xAOD::L2CombinedMuon& L2CBmu);
    float findTrigTreshold(TString chain);

    bool m_isData;
    bool m_doTrigMuonL1Validation;
    bool m_doTrigMuonL2Validation;
    bool m_doTrigMuonEFValidation;
    std::vector<std::vector<std::string>> m_ChainSeed;
    std::vector<std::string> m_L1MuonItems;


};

#endif
