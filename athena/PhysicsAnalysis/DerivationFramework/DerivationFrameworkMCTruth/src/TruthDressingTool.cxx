/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthDressingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Kevin Finelli (kevin.finelli@cern.ch)
// Create dressed (i.e. including FSR photons) 4-vectors of truth objects

#include "DerivationFrameworkMCTruth/TruthDressingTool.h"
#include "xAODTruth/TruthEventContainer.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

// Constructor
DerivationFramework::TruthDressingTool::TruthDressingTool(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p)
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty ("particlesKey",
            m_particlesKey = "TruthParticles",
            "Name of TruthParticles key for photon list input");
    declareProperty ("dressParticlesKey",
            m_dressParticlesKey = "TruthParticles",
            "Name of TruthParticles key for input particles to be dressed.  If taus are selected, everything in this input key will be used!");
    declareProperty ("usePhotonsFromHadrons",
            m_usePhotonsFromHadrons = false,
            "Add photons coming from hadron decays while dressing particles?");
    declareProperty ("dressingConeSize", m_coneSize = 0.1,
            "Size of dR cone in which to include FSR photons in dressing");
    declareProperty ("particleIDsToDress", m_listOfPIDs = std::vector<int>{11,13},
            "List of the pdgID's of particles to be dressed (usually 11,13).  Special treatment for taus (15)");
    declareProperty ("useAntiKt", m_useAntiKt = false,
        "use anti-k_T in addition to fixed-cone dressing");

}

// Destructor
DerivationFramework::TruthDressingTool::~TruthDressingTool() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthDressingTool::initialize()
{
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthDressingTool::finalize()
{
    ATH_MSG_VERBOSE("finalize() TruthDressingTool");
    return StatusCode::SUCCESS;
}

// Function to do dressing, implements interface in IAugmentationTool
StatusCode DerivationFramework::TruthDressingTool::addBranches() const
{
    // Retrieve the truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles(nullptr);
    if (evtStore()->retrieve(importedTruthParticles, m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticleContainer with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    const xAOD::TruthParticleContainer* importedDressTruthParticles(nullptr);
    if (evtStore()->retrieve(importedDressTruthParticles, m_dressParticlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticleContainer with name " << m_dressParticlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    SG::AuxElement::Decorator< float > decorator_e("e_dressed");
    SG::AuxElement::Decorator< float > decorator_pt("pt_dressed");
    SG::AuxElement::Decorator< float > decorator_eta("eta_dressed");
    SG::AuxElement::Decorator< float > decorator_phi("phi_dressed");

    // for truth taus, use 'vis' in the decoration name to avoid prompt/visible tau momentum ambiguity
    // use (pt,eta,phi,m) for taus, for consistency with other TauAnalysisTools decorations
    SG::AuxElement::Decorator< float > decorator_pt_vis("pt_vis_dressed");
    SG::AuxElement::Decorator< float > decorator_eta_vis("eta_vis_dressed");
    SG::AuxElement::Decorator< float > decorator_phi_vis("phi_vis_dressed");
    SG::AuxElement::Decorator< float > decorator_m_vis("m_vis_dressed");

    SG::AuxElement::Decorator< int > decorator_nphoton("nPhotons_dressed");

    //anti-kt dressing
    SG::AuxElement::Decorator< float > decorator_e_akt("e_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_pt_akt("pt_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_eta_akt("eta_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_phi_akt("phi_dressed_akt");

    // anti-kt for truth taus
    SG::AuxElement::Decorator< float > decorator_pt_vis_akt("pt_vis_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_eta_vis_akt("eta_vis_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_phi_vis_akt("phi_vis_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_m_vis_akt("m_vis_dressed_akt");

    //get struct of helper functions
    DerivationFramework::DecayGraphHelper decayHelper;

    std::vector<const xAOD::TruthParticle*> listOfParticlesToDress;
    std::vector<xAOD::TruthParticle::FourMom_t> listOfDressedParticles;
    std::vector<int> dressedParticlesNPhot;

    // when dressing only truth taus, it is assumed that the truth tau container has
    // been built beforehand and is used as input
    if(m_listOfPIDs.size()==1 && abs(m_listOfPIDs[0])==15) {
      for (auto pItr = importedDressTruthParticles->begin(); pItr != importedDressTruthParticles->end(); ++pItr) {
        listOfParticlesToDress.push_back(*pItr);
      }
    } else {
      decayHelper.constructListOfFinalParticles(importedDressTruthParticles, listOfParticlesToDress, m_listOfPIDs);
    }

    //initialize list of dressed particles
    for (const auto& part : listOfParticlesToDress) {
      listOfDressedParticles.push_back(part->p4());
      dressedParticlesNPhot.push_back(0);
    }

    //fill the photon list
    std::vector<const xAOD::TruthParticle*>  photonsFSRList;
    std::vector<int> photonPID{22};
    decayHelper.constructListOfFinalParticles(importedTruthParticles, photonsFSRList, photonPID, m_usePhotonsFromHadrons);

    //loop over photons, uniquely associate each to nearest bare particle
    for (const auto& phot : photonsFSRList ) {
      double dRmin = m_coneSize;
      int idx = -1;

      for (size_t i = 0; i < listOfParticlesToDress.size(); ++i) {
        xAOD::TruthParticle::FourMom_t bare_part;
        if(abs(listOfParticlesToDress[i]->pdgId())==15) {

          if( !listOfParticlesToDress[i]->isAvailable<double>("pt_vis") ||
              !listOfParticlesToDress[i]->isAvailable<double>("eta_vis") ||
              !listOfParticlesToDress[i]->isAvailable<double>("phi_vis") ||
              !listOfParticlesToDress[i]->isAvailable<double>("m_vis")) {
            ATH_MSG_ERROR("Visible momentum not available for truth taus, cannot perform dressing!");
            return StatusCode::FAILURE;
          }

          bare_part.SetPtEtaPhiM(listOfParticlesToDress[i]->auxdata<double>("pt_vis"),
                                 listOfParticlesToDress[i]->auxdata<double>("eta_vis"),
                                 listOfParticlesToDress[i]->auxdata<double>("phi_vis"),
                                 listOfParticlesToDress[i]->auxdata<double>("m_vis"));
        }
        else {
          bare_part = listOfParticlesToDress[i]->p4();
        }

        double dR = bare_part.DeltaR(phot->p4());
        if (dR < dRmin) {
          dRmin = dR;
          idx = i;
        }
      }

      if(idx > -1) {
        listOfDressedParticles[idx] += phot->p4();
        dressedParticlesNPhot[idx]++;
      }
    }

    //loop over particles and add decorators
    //for (const auto& part : listOfDressedParticles) {
    for (size_t i = 0; i < listOfParticlesToDress.size(); ++i) {
        const xAOD::TruthParticle* part = listOfParticlesToDress[i];
        xAOD::TruthParticle::FourMom_t& dressedVec = listOfDressedParticles[i];

      if(abs(part->pdgId())==15) {
        decorator_pt_vis(*part)      = dressedVec.Pt();
        decorator_eta_vis(*part)     = dressedVec.Eta();
        decorator_phi_vis(*part)     = dressedVec.Phi();
        decorator_m_vis(*part)       = dressedVec.M();
      }
      else {
        decorator_e(*part)       = dressedVec.E();
        decorator_pt(*part)      = dressedVec.Pt();
        decorator_eta(*part)     = dressedVec.Eta();
        decorator_phi(*part)     = dressedVec.Phi();
      }
      decorator_nphoton(*part) = dressedParticlesNPhot[i];
    }

    //build the anti-kt jet list
    if (m_useAntiKt) {
      std::vector<fastjet::PseudoJet> sorted_jets;
      std::vector<fastjet::PseudoJet> fj_particles;
      for (const auto& part : listOfParticlesToDress) {

        if(abs(part->pdgId())==15) {
          if(!part->isAvailable<double>("pt_vis") || !part->isAvailable<double>("eta_vis")
              || !part->isAvailable<double>("phi_vis") || !part->isAvailable<double>("m_vis")) {
            ATH_MSG_ERROR("Visible momentum not available for truth taus, cannot perform dressing!");
            return StatusCode::FAILURE;
          }

          TLorentzVector tauvis;
          tauvis.SetPtEtaPhiM(part->auxdata<double>("pt_vis"), part->auxdata<double>("eta_vis"), part->auxdata<double>("phi_vis"), part->auxdata<double>("m_vis"));
          fj_particles.push_back(fastjet::PseudoJet(tauvis.Px(), tauvis.Py(), tauvis.Pz(), tauvis.E()));
        }
        else {
          fj_particles.push_back(fastjet::PseudoJet(part->px(), part->py(), part->pz(), part->e()));
        }

        fj_particles.back().set_user_index(part->barcode());
      }
      for (const auto& part : photonsFSRList) {
        fj_particles.push_back(fastjet::PseudoJet(part->px(), part->py(), part->pz(), part->e()));
        fj_particles.back().set_user_index(part->barcode());
      }

      //run the clustering
      fastjet::JetAlgorithm alg=fastjet::antikt_algorithm;
      const fastjet::JetDefinition jet_def(alg, m_coneSize);
      fastjet::ClusterSequence cseq(fj_particles, jet_def);
      sorted_jets = sorted_by_pt(cseq.inclusive_jets(0));
      //associate clustered jets back to bare particles
      for (const auto& part : listOfParticlesToDress) {
        //loop over fastjet pseudojets and associate one with this particle by barcode
        bool found=false;
        auto pjItr=sorted_jets.begin();
        while(!found && pjItr!=sorted_jets.end()) {
          std::vector<fastjet::PseudoJet> constituents = pjItr->constituents();
          for(const auto& constit : constituents) {
            if (part->barcode()==constit.user_index()) {

              // shall we count the number of photons among the pseudojet constituents 
              // to decorate leptons with the number of dressing photons found by the anti-kt algorithm?

              if(abs(part->pdgId())==15) {
                decorator_pt_vis_akt(*part)      = pjItr->pt();
                decorator_eta_vis_akt(*part)     = pjItr->pseudorapidity();
                decorator_phi_vis_akt(*part)     = pjItr->phi_std(); //returns phi in [-pi,pi]
                decorator_m_vis_akt(*part)       = pjItr->m();
              }
              else {
                decorator_e_akt(*part)       = pjItr->e();
                decorator_pt_akt(*part)      = pjItr->pt();
                decorator_eta_akt(*part)     = pjItr->pseudorapidity();
                decorator_phi_akt(*part)     = pjItr->phi_std(); //returns phi in [-pi,pi]
              }
              found=true;
              break;
            }

          }
          ++pjItr;
        }
        if (!found) {
          if(abs(part->pdgId())==15) {
            decorator_pt_vis_akt(*part)      = 0.;
            decorator_eta_vis_akt(*part)     = 0.;
            decorator_phi_vis_akt(*part)     = 0.;
            decorator_m_vis_akt(*part)       = 0.;
          }
          else {
            decorator_e_akt(*part)       = 0;
            decorator_pt_akt(*part)      = 0;
            decorator_eta_akt(*part)     = 0;
            decorator_phi_akt(*part)     = 0;
          }
          ATH_MSG_WARNING("Bare particle not found in constituents ");
        }
      }
    }

    return StatusCode::SUCCESS;
}
