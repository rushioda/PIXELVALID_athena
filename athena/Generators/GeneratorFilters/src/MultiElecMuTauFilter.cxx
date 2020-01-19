/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/MultiElecMuTauFilter.h"
#include "CLHEP/Vector/LorentzVector.h"


MultiElecMuTauFilter::MultiElecMuTauFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
  declareProperty("MinPt",            m_minPt          = 5000. );
  declareProperty("MaxEta",           m_maxEta         = 10.0  );
  declareProperty("MinVisPtHadTau",   m_minVisPtHadTau = 10000.);
  declareProperty("NLeptons",         m_NLeptons       = 4     );
  declareProperty("IncludeHadTaus",   m_incHadTau      = true  );
}


StatusCode MultiElecMuTauFilter::filterEvent() {
  int numLeptons = 0;
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      // Electrons and muons
      if ((*pitr)->status() == 1 && (abs((*pitr)->pdg_id()) == 11 || abs((*pitr)->pdg_id()) == 13)) {
        if ((*pitr)->momentum().perp() >= m_minPt && fabs((*pitr)->momentum().pseudoRapidity()) <= m_maxEta) {
          ATH_MSG_DEBUG("Found lepton with PDG ID = " << (*pitr)->pdg_id()
                        << ", status = " <<  (*pitr)->status()
                        << ", pt = "     <<  (*pitr)->momentum().perp()
                        << ", eta = "    <<  (*pitr)->momentum().pseudoRapidity());
          numLeptons++;
        }
        continue;
      }

      // Look for Hadronic taus (leptonic ones will be saved by above) that have status!=3 and don't decay to another tau (FSR)
      if (!m_incHadTau) continue;

      HepMC::GenParticle *tau(0), *taunu(0);
      if (abs((*pitr)->pdg_id()) == 15 && (*pitr)->status() != 3) {
        tau = *pitr;

        // Loop over children and:
        // 1. Find if it is hadronic (i.e. no decay lepton).
        // 2. Veto tau -> tau (FSR)
        // 3. Store the tau neutino to calculate the visible momentum
        HepMC::GenVertex::particles_out_const_iterator citr;
        for (citr = tau->end_vertex()->particles_out_const_begin(); citr != tau->end_vertex()->particles_out_const_end(); citr++) {
          // Ignore tau -> tau (FSR)
          if ((*pitr)->pdg_id() == (*citr)->pdg_id()) {
            ATH_MSG_DEBUG("FSR tau decay.  Ignoring!");
            tau = 0;
            break;
          }

          // Ignore leptonic decays
          if (abs((*citr)->pdg_id()) == 13 || abs((*citr)->pdg_id()) == 11) {
            tau = 0;
            break;
          }

          // Find tau decay nu
          if (fabs((*citr)->pdg_id()) == 16) {
            taunu = *citr;
          }
        }

        if (tau) {
          // Good hadronic decay
          CLHEP::HepLorentzVector tauVisMom = CLHEP::HepLorentzVector(tau->momentum().px() - taunu->momentum().px(),
                                                        tau->momentum().py() - taunu->momentum().py(),
                                                        tau->momentum().pz() - taunu->momentum().pz(),
                                                        tau->momentum().e()  - taunu->momentum().e());
          if (tauVisMom.perp() >= m_minVisPtHadTau && fabs(tauVisMom.pseudoRapidity()) <= m_maxEta) {
            ATH_MSG_DEBUG("Found hadronic tau decay with PDG ID = " << tau->pdg_id()
                          << ", status = " << tau->status()
                          << ", vis pt = " << tauVisMom.perp()
                          << ", eta = " <<  tauVisMom.pseudoRapidity());
            numLeptons++;
          }
        }
      }

    }
  }

  ATH_MSG_INFO("Found " << numLeptons << "Leptons");
  setFilterPassed(numLeptons >= m_NLeptons);
  return StatusCode::SUCCESS;
}
