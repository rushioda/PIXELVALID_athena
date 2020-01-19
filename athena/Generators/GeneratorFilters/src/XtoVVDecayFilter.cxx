/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/XtoVVDecayFilter.h"
#include "GaudiKernel/AlgFactory.h"


XtoVVDecayFilter::XtoVVDecayFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("PDGGrandParent", m_PDGGrandParent);
  declareProperty("PDGParent", m_PDGParent);
  declareProperty("StatusParent", m_StatusParent);
  declareProperty("PDGChild1", m_PDGChild1);
  declareProperty("PDGChild2", m_PDGChild2);
  
  m_nHtoVV = 0;
  m_nGoodHtoVV = 0;
}


StatusCode XtoVVDecayFilter::filterInitialize() {
  ATH_MSG_INFO("PDGGrandParent(H) = " << m_PDGGrandParent);
  ATH_MSG_INFO("PDGParent(V)      = " << m_PDGParent << " " << "StatusParent(V)      = " << m_StatusParent);
  if (m_PDGChild1.empty()) ATH_MSG_ERROR("PDGChild1[] not set ");
  if (m_PDGChild2.empty()) ATH_MSG_ERROR("PDGChild2[] not set ");
  for (size_t i = 0; i < m_PDGChild1.size(); ++i)
    ATH_MSG_INFO("PDGChild1[" << i << "] = " << m_PDGChild1[i]);
  for (size_t i = 0; i < m_PDGChild2.size(); ++i)
    ATH_MSG_INFO("PDGChild2[" << i << "] = " << m_PDGChild2[i]);

  // init
  m_nHtoVV = 0;
  m_nGoodHtoVV = 0;
  return StatusCode::SUCCESS;
}


StatusCode XtoVVDecayFilter::filterFinalize() {
  ATH_MSG_INFO("Statistics of X->VV, V->decay ");
  ATH_MSG_INFO("  ALL  X->VV  " << m_nHtoVV);
  ATH_MSG_INFO("  Good X->VV  " << m_nGoodHtoVV);
  if (m_nHtoVV != 0) ATH_MSG_INFO("  Fraction   " << double(m_nGoodHtoVV)/double(m_nHtoVV));
  return StatusCode::SUCCESS;
}


StatusCode XtoVVDecayFilter::filterEvent() {
  bool okPDGChild1 = false;
  bool okPDGChild2 = false;
  int nGoodParent = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    for (HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      if ( abs((*pitr)->pdg_id()) == m_PDGParent && (*pitr)->status() == m_StatusParent) {
        HepMC::GenVertex::particle_iterator firstMother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);
        HepMC::GenVertex::particle_iterator thisMother = firstMother;
        bool isGrandParentOK = false;
        for (; thisMother != endMother; ++thisMother) {
          ATH_MSG_DEBUG(" Parent " << (*pitr)->pdg_id() << " barcode = "   << (*pitr)->barcode() << " status = "  << (*pitr)->status());
          ATH_MSG_DEBUG(" a Parent mother "  << (*thisMother)->pdg_id()<< " barcode = " << (*thisMother)->barcode());
          if ( (*thisMother)->pdg_id() == m_PDGGrandParent ) isGrandParentOK = true;
        }
        ATH_MSG_DEBUG(" Grand Parent is OK? " << isGrandParentOK);
        if (!isGrandParentOK) continue;
        ++nGoodParent;
        HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        //HepMC::GenVertex::particle_iterator thisChild = firstChild;
        FindAncestor(firstChild, endChild, m_PDGParent, okPDGChild1, okPDGChild2);
      }
    }
  }
  ATH_MSG_DEBUG("Result " << nGoodParent << " " << okPDGChild1 << " " << okPDGChild2);

  if (nGoodParent == 2) {
    ++m_nHtoVV;
    if (okPDGChild1 && okPDGChild2)  {
      ++m_nGoodHtoVV;
      return StatusCode::SUCCESS;
    }
  }

  // If we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}


void XtoVVDecayFilter::FindAncestor(const HepMC::GenVertex::particle_iterator &firstAncestor,
                                    const HepMC::GenVertex::particle_iterator &endAncestor,
                                    int targetPDGID, bool& okPDGChild1, bool& okPDGChild2) {
  HepMC::GenVertex::particle_iterator thisAncestor = firstAncestor;
  for (; thisAncestor != endAncestor; ++thisAncestor){
    //ATH_MSG_DEBUG(" child " << (*thisAncestor)->pdg_id());
    if (abs((*thisAncestor)->pdg_id()) == targetPDGID) { //same particle as parent
      HepMC::GenVertex::particle_iterator firstChild = (*thisAncestor)->end_vertex()->particles_begin(HepMC::children);
      HepMC::GenVertex::particle_iterator endChild = (*thisAncestor)->end_vertex()->particles_end(HepMC::children);
      FindAncestor(firstChild, endChild, targetPDGID, okPDGChild1, okPDGChild2);
    } else {
      if (!okPDGChild1) {
        for (size_t i = 0; i < m_PDGChild1.size(); ++i) {
          if (abs((*thisAncestor)->pdg_id()) == m_PDGChild1[i]) {
            okPDGChild1 = true;
            break;
          }
        }
        if (okPDGChild1) break;
      }
      if (!okPDGChild2) {
        for (size_t i = 0; i < m_PDGChild2.size(); ++i) {
          if (abs((*thisAncestor)->pdg_id()) == m_PDGChild2[i]) {
            okPDGChild2 = true;
            break;
          }
        }
        if (okPDGChild2) break;
      }
    }
  }
}
