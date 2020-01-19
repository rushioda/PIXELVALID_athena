/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/SoftLeptonFilter.h"

SoftLeptonFilter::SoftLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  declareProperty("Ptcut",m_Ptmin = 5000.);
  declareProperty("Etacut",m_EtaRange = 3.0);
  declareProperty("LeptonType",m_LeptonType = 0);
}


StatusCode SoftLeptonFilter::filterEvent() {
  for (McEventCollection::const_iterator itr = events()->begin(); itr!=events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if ((*pitr)->status() != 1) continue;
 	  if ( ( 2 != m_LeptonType && 11 == abs((*pitr)->pdg_id()) ) || ( 1 != m_LeptonType && 13 == abs((*pitr)->pdg_id()) ) ) {
        if ( ((*pitr)->momentum().perp() >= m_Ptmin) && fabs((*pitr)->momentum().pseudoRapidity()) <= m_EtaRange) {
 		  // select e's from hadrons and from taus that come from hadrons,
 		  // and electrons from gammas that come from hadrons... (dalitz decay)
 		  HepMC::GenVertex::particle_iterator firstParent;
 		  HepMC::GenVertex * decayVtx = (*pitr)->production_vertex();
 		  firstParent  = decayVtx->particles_begin(HepMC::parents);
		  if (*firstParent==0) {
            ATH_MSG_ERROR("firstParent iterator is null for this muon; skip it");
            continue;
          }
          int parentID((*firstParent)->pdg_id());
          HepMC::GenVertex::particle_iterator theParent=firstParent;
          while(abs(parentID)==15||parentID==(*pitr)->pdg_id()) {
            HepMC::GenVertex * parentDecayVtx = (*theParent)->production_vertex();
            HepMC::GenVertex::particle_iterator firstGrandParent = parentDecayVtx->particles_begin(HepMC::parents);
            if (*firstGrandParent==0) {
              ATH_MSG_ERROR("firstGrandParent iterator is null for this muon from tau decay; skip it");
              continue;
            }
            parentID=(*firstGrandParent)->pdg_id();
            theParent=firstGrandParent;
          }
          // Exclude gauge and Higgs boson decays
          if (abs(parentID) > 37) {
            ATH_MSG_DEBUG("Found a lepton " << (*pitr)->pdg_id()
                          << " from " << parentID
                          << " PT="   << (*pitr)->momentum().perp()
                          << " eta="  << (*pitr)->momentum().pseudoRapidity());
            return StatusCode::SUCCESS;
          }
        }
      }
    }
  }
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
