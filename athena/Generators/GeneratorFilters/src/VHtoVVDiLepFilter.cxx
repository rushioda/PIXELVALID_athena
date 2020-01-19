/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/VHtoVVDiLepFilter.h"
#include "GaudiKernel/AlgFactory.h"


VHtoVVDiLepFilter::VHtoVVDiLepFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator),   m_nVHtoVV(0), m_nGoodVHtoVV(0)
{
  declareProperty("PDGGrandParent", m_PDGGrandParent);
  declareProperty("PDGParent", m_PDGParent);
  declareProperty("PDGChildren", m_PDGChildren);
  declareProperty("PDGAssoc", m_PDGAssoc);
  declareProperty("StatusCode", m_StatusCode);
}


StatusCode VHtoVVDiLepFilter::filterInitialize() {
  ATH_MSG_INFO("PDGGrandParent(H) = " << m_PDGGrandParent);
  ATH_MSG_INFO("PDGParent(V)      = " << m_PDGParent);
  ATH_MSG_INFO("PDGAssoc(V)      = " << m_PDGAssoc);
  if (m_PDGChildren.empty()) ATH_MSG_ERROR("PDGChildren[] not set ");
  for (size_t i = 0; i < m_PDGChildren.size(); ++i)
    ATH_MSG_INFO("PDGChildren[" << i << "] = " << m_PDGChildren[i]);

  // Init counters
  m_nVHtoVV = 0;
  m_nGoodVHtoVV = 0;
  return StatusCode::SUCCESS;
}


StatusCode VHtoVVDiLepFilter::filterFinalize() {
  ATH_MSG_INFO("Statistics of WH->VV, W->decay V->decay ");
  ATH_MSG_INFO("  ALL  VH->VV " << m_nVHtoVV);
  ATH_MSG_INFO("  Good VH->VV " << m_nGoodVHtoVV);
  if (m_nVHtoVV != 0)
    ATH_MSG_INFO("  Fraction   " << double(m_nGoodVHtoVV)/double(m_nVHtoVV));
  return StatusCode::SUCCESS;
}


StatusCode VHtoVVDiLepFilter::filterEvent() {
  int n_okPDGHVChildren = 0;
  int n_okPDGAssocVChild = 0;
  int nHiggsParent = 0;
  int nVParent = 0;
  for (McEventCollection::const_iterator ievt = events()->begin(); ievt != events()->end(); ++ievt) {
    // Loop over all particles in the event
    for (HepMC::GenEvent::particle_const_iterator pitr = (*ievt)->particles_begin(); pitr != (*ievt)->particles_end(); ++pitr) {

      // Loop over particles from the primary interaction that match the PDG Parent
      if ( (abs((*pitr)->pdg_id()) == m_PDGParent ||abs((*pitr)->pdg_id()) == m_PDGAssoc) && abs((*pitr)->status()) == m_StatusCode) {
        HepMC::GenVertex::particle_iterator firstMother = (*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endMother = (*pitr)->production_vertex()->particles_end(HepMC::parents);
        HepMC::GenVertex::particle_iterator thisMother = firstMother;
        // Check if originated from the Higgs or is the associated vector boson
        bool isGrandParentHiggs = false;
        bool isGrandParentV = false;
        for (; thisMother != endMother; ++thisMother) { // loop over chain of grandparents
          ATH_MSG_DEBUG(" Parent " << (*pitr)->pdg_id() << " barcode = "   << (*pitr)->barcode() << " status = "  << (*pitr)->status() );
          ATH_MSG_DEBUG(" a Parent mother "  << (*thisMother)->pdg_id()<< " barc = " << (*thisMother)->barcode() );
          if ( (*thisMother)->pdg_id() == m_PDGGrandParent ) isGrandParentHiggs = true;
          else  isGrandParentV = true;
        }
        ATH_MSG_DEBUG(" Grand Parent is Higgs? " << isGrandParentHiggs );
        ATH_MSG_DEBUG(" Grand Parent is V? " << isGrandParentV );

        if (!isGrandParentHiggs && !isGrandParentV) continue;

        if (isGrandParentHiggs) {
          ++nHiggsParent;
          if (!((*pitr)->end_vertex())) continue;
          HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
          HepMC::GenVertex::particle_iterator thisChild = firstChild;
          findAncestor(firstChild, endChild, m_PDGParent, n_okPDGHVChildren);
        } // end of higgs grandparent loop

        if (isGrandParentV) {
          ++nVParent;
          HepMC::GenVertex::particle_iterator firstChildStart = (*pitr)->end_vertex()->particles_begin(HepMC::children);
          HepMC::GenVertex::particle_iterator endChildStart = (*pitr)->end_vertex()->particles_end(HepMC::children);
          findAncestor(firstChildStart, endChildStart, m_PDGAssoc, n_okPDGAssocVChild);
        } // end of v grandparent loop

      } // end good parent loop
    }
  }

  ATH_MSG_DEBUG("Result " << nHiggsParent << " " << n_okPDGHVChildren );
  ATH_MSG_DEBUG("Result " << nVParent << " " << n_okPDGAssocVChild );

  if (nHiggsParent == 2 && nVParent >= 1) ++m_nVHtoVV;

  if (nHiggsParent == 2 && nVParent == 1 && (n_okPDGAssocVChild + n_okPDGHVChildren) > 1) {
    ++m_nGoodVHtoVV;
    return StatusCode::SUCCESS;
  }

  // If we get here we have failed
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}


void VHtoVVDiLepFilter::findAncestor(const HepMC::GenVertex::particle_iterator& firstAncestor,
                                     const HepMC::GenVertex::particle_iterator& endAncestor,
                                     int targetPDGID, int& n_okPDGChild) {
  std::vector<int> foundCodes;
  HepMC::GenVertex::particle_iterator thisAncestor = firstAncestor;
  for (; thisAncestor != endAncestor; ++thisAncestor) {
    if (abs((*thisAncestor)->pdg_id()) == targetPDGID) { // same particle as parent
      HepMC::GenVertex::particle_iterator firstChild = (*thisAncestor)->end_vertex()->particles_begin(HepMC::children);
      HepMC::GenVertex::particle_iterator endChild = (*thisAncestor)->end_vertex()->particles_end(HepMC::children);
      findAncestor(firstChild, endChild, targetPDGID, n_okPDGChild);
    } else {
      for (size_t i = 0; i < m_PDGChildren.size(); ++i) {
        int testPdgID = (*thisAncestor)->pdg_id();
        if (abs(testPdgID) == m_PDGChildren[i]) {
          const bool alreadyFound = (std::find(foundCodes.begin(), foundCodes.end(), testPdgID) != foundCodes.end());
          if (!alreadyFound) {
            n_okPDGChild++;
            foundCodes.push_back((*thisAncestor)->pdg_id()); // add to list of found particles and check to avoid double counting
          }
          else break;
        }
      }
    }
  }
}
