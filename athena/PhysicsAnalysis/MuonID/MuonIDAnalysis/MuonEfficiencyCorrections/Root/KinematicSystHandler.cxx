/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include <MuonEfficiencyCorrections/KinematicSystHandler.h>
namespace CP {

    float IKinematicSystHandler::Eta(const xAOD::Muon &mu) const {
        return mu.eta();
    }
    float IKinematicSystHandler::Pt(const xAOD::Muon &mu) const {
        return mu.pt();
    }
    float IKinematicSystHandler::PtGeV(const xAOD::Muon &mu) const {
        return mu.pt() / 1.e3;
    }
    float IKinematicSystHandler::AbsEta(const xAOD::Muon &mu) const {
        return fabs(mu.eta());
    }
    IKinematicSystHandler::KinVariable IKinematicSystHandler::GetMuonVariableToUse(const std::string &name) {
        if (name == "pt") return &IKinematicSystHandler::Pt;
        if (name == "ptGeV") return &IKinematicSystHandler::PtGeV;
        if (name == "eta") return &IKinematicSystHandler::Eta;
        if (name == "AbsEta") return &IKinematicSystHandler::AbsEta;
        return NULL;
    }

    PtDependentSystHandler::PtDependentSystHandler(HistHandler* HistHandler) :
                    m_Handler(HistHandler),
                    m_SystWeight(0) {

    }
    CorrectionCode PtDependentSystHandler::GetKineDependent(const xAOD::Muon &mu, float& Eff) const {
        if (mu.pt() <= 20.e3) return CorrectionCode::Ok;

        int binsys = -1;
        CorrectionCode cc = m_Handler->FindBin(mu, binsys);
        if (cc != CorrectionCode::Ok) {
            return cc;
        }
        Eff *= (1. + m_SystWeight * m_Handler->GetBinContent(binsys) * mu.pt() / 1.0e6);
        return CorrectionCode::Ok;
    }
    void PtDependentSystHandler::SetSystematicWeight(float SystWeight) {
        m_SystWeight = SystWeight;
    }
    bool PtDependentSystHandler::initialize() {
        return m_Handler != nullptr;
    }
    PtDependentSystHandler::~PtDependentSystHandler() {
        if (m_Handler) delete m_Handler;
    }

    std::string BadMuonVetoSystHandler::GetNextProperty(std::string &sstr) {
        //Elimnate the beginning underscores
        if (sstr.find("_") == 0) {
            sstr.substr(1, sstr.size());
        }
        size_t pos = sstr.find("_");
        std::string Property = pos != std::string::npos ? sstr.substr(0, pos) : sstr;
        if (pos != std::string::npos) {
            sstr = sstr.substr(Property.size() + 1, sstr.size());
        }
        return Property;
    }

    BadMuonVetoSystHandler::BadMuonVetoSystHandler(TDirectory* InDir) :
                    m_SystPolynomials(),
                    m_FirstVar(NULL),
                    m_SecondVar(NULL),
                    m_SystWeight(0.) {
        if (!InDir) {
            Error("BadMuonSysVetoHandler()", "No TDirectory is given");
            return;
        }
        TIter next(InDir->GetListOfKeys());
        while (TObject* obj = next()) {
            std::string ObjName = obj->GetName();
            float lowRange(-1.e20), highRange(1.e20);
            if (ObjName.find("TF1") == std::string::npos) {
                continue;
            }
            TF1* TF = NULL;
            InDir->GetObject(ObjName.c_str(), TF);
            if (!TF) {
                continue;
            }
            //Elimnate the TF1_
            GetNextProperty(ObjName);
            m_FirstVar = GetMuonVariableToUse(GetNextProperty(ObjName));
            m_SecondVar = GetMuonVariableToUse(GetNextProperty(ObjName));
            std::string LowRange_str = GetNextProperty(ObjName);
            std::string HighRange_str = GetNextProperty(ObjName);
            if (!LowRange_str.empty()) {
                lowRange = atof(LowRange_str.c_str()) / 10.;
            }
            if (!HighRange_str.empty()) {
                highRange = atof(HighRange_str.c_str()) / 10.;
            }
            m_SystPolynomials.insert(std::pair<Ranges, TF1*>(Ranges(lowRange, highRange), TF));
        }

    }
    CorrectionCode BadMuonVetoSystHandler::GetKineDependent(const xAOD::Muon &mu, float& Eff) const {
        TF1* Poly = NULL;
        if (m_SystWeight == 0) {
            return CorrectionCode::Ok;
        }
        // we know that Eff=(1+relative sys error), since SF==1
        float RelHighPtSys = 0.;
        if (mu.pt() >= 100.e3) {
            CorrectionCode cc = FindAppropiatePolynomial(mu, Poly);
            if (cc != CorrectionCode::Ok) {
                return cc;
            }
            RelHighPtSys = fabs(1 - Poly->Eval((this->*m_FirstVar)(mu)));
//            std::cout<<"SystWeight: "<<m_SystWeight<<"  Blub: "<<1 - Poly->Eval((this->*m_FirstVar)(mu))
//                    <<"Muon: " <<(this->*m_FirstVar)(mu)<<std::endl;

        } else {
            //Apply flat 0.5% systematic
            RelHighPtSys = 0.005;
        }

        Eff = 1 + m_SystWeight * sqrt((Eff - 1) * (Eff - 1) + RelHighPtSys * RelHighPtSys);

        return CorrectionCode::Ok;
    }
    void BadMuonVetoSystHandler::SetSystematicWeight(float SystWeight) {
        m_SystWeight = SystWeight;
    }

    bool BadMuonVetoSystHandler::initialize() {
        if (m_SystPolynomials.empty()) {
            Error("BadMuonVetoSystHandler::initialize()", "No polynomials");
            return false;
        }
        if (!m_FirstVar || !m_SecondVar) {
            return false;
        }
        return true;
    }
    BadMuonVetoSystHandler::~BadMuonVetoSystHandler() {
        for (auto& Syst : m_SystPolynomials) {
            if (Syst.second) delete Syst.second;
        }
    }
    CP::CorrectionCode BadMuonVetoSystHandler::FindAppropiatePolynomial(const xAOD::Muon& mu, TF1* &Poly) const {
        if (!m_SecondVar) {
            Error("BadMuonVetoSystHandler()", "Something went wrong with the initialization");
            return CorrectionCode::Error;
        }
        for (const auto& BinnedPoly : m_SystPolynomials) {
            if (BinnedPoly.first.first <= (this->*m_SecondVar)(mu) && (this->*m_SecondVar)(mu) < BinnedPoly.first.second) {
                Poly = BinnedPoly.second;
                return CorrectionCode::Ok;
            }
        }
        return CP::CorrectionCode::OutOfValidityRange;
    }

} /* namespace CP */
