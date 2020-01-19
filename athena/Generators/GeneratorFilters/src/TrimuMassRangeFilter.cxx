/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/TrimuMassRangeFilter.cxx
// Description:
//   Based on MassFilter.cxx
//
//   Allows the user to search for the events where 2 particles of user defined type that
// will pass if they have p_t, eta, and invariant mass in the specified range. 
// If there are more than 1 combinations which pass the eta and pT
// selection in the event, take the pair which has the largest invariant mass and then
// apply the invariant mass selection in order to avoid duplication.(Original MassFilter 
// takes the events where at least one pair passes the invariant mass selection.)
// default is 2 muons with p_t>=10 GeV, |eta| <= 10, and Mass between 0-14TeV.
// Always allow same sign events.
//
// AuthorList:
//         
//  Jeremy Love and Akimasa Ishikawa Sep 2010 

// Header for this module:-

#include "GeneratorFilters/TrimuMassRangeFilter.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"


// Other classes used by this class:-
#include <math.h>

using HepMC::GenVertex;
using HepMC::GenParticle;

//--------------------------------------------------------------------------
TrimuMassRangeFilter::TrimuMassRangeFilter(const std::string& name, 
                         ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator) {
  //----------------------------    
  declareProperty("PtCut1",m_Ptmin1 = 3500.);  
  declareProperty("PtCut2",m_Ptmin2 = 3500.);  
  declareProperty("PtCut3",m_Ptmin3 = 3500.);  
  declareProperty("EtaCut1",m_EtaRange1 = 4.5); 
  declareProperty("EtaCut2",m_EtaRange2 = 4.5);
  declareProperty("EtaCut3",m_EtaRange3 = 4.5);
  declareProperty("InvMassMin",m_InvMassMin=500.);
  declareProperty("InvMassMax",m_InvMassMax=5000.);
  declareProperty("PartId1",m_PartId1=13);
  declareProperty("PartId2",m_PartId2=13);
  declareProperty("PartId3",m_PartId3=13);
  declareProperty("PartStatus",m_PartStatus=1);
}

//--------------------------------------------------------------------------
TrimuMassRangeFilter::~TrimuMassRangeFilter(){
  //--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode TrimuMassRangeFilter::filterInitialize() {
  //---------------------------------------------------------------------------
  MsgStream log(messageService(), name());

  log << MSG::INFO
      << "TrimuMassRangeFilter "  << endreq
      << " PtCut1     " << m_Ptmin1 << endreq
      << " PtCut2     " << m_Ptmin2 << endreq
      << " PtCut3     " << m_Ptmin3 << endreq
      << " EtaCut1    " << m_EtaRange1 << endreq
      << " EtaCut2    " << m_EtaRange2 << endreq
      << " EtaCut3    " << m_EtaRange3 << endreq
      << " InvMassMin " << m_InvMassMin << endreq
      << " InvMassMax " << m_InvMassMax << endreq
      << " PartId1    " << m_PartId1 << endreq
      << " PartId2    " << m_PartId2 << endreq
      << " PartId3    " << m_PartId3 << endreq
      << " PartStatus " << m_PartStatus << endreq;

  return StatusCode::SUCCESS;
 }

//---------------------------------------------------------------------------
StatusCode TrimuMassRangeFilter::filterFinalize() {
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode TrimuMassRangeFilter::filterEvent() {
  //---------------------------------------------------------------------------

  MsgStream log(messageService(), name());
  log << MSG::INFO << " TEST MESSAGE " << endreq;

  // Loop over all events in McEventCollection

  bool   samePDGID12  = ( fabs(m_PartId1) == fabs(m_PartId2) );
  bool   samePDGID123 = ( fabs(m_PartId1) == fabs(m_PartId2) && fabs(m_PartId1) == fabs(m_PartId3) );

  McEventCollection::const_iterator itr;
  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {

    // Loop over all particles in the event
    const HepMC::GenEvent* genEvt = (*itr);
    int n=0;
    for(HepMC::GenEvent::particle_const_iterator pitr1 = genEvt->particles_begin();
	pitr1!=genEvt->particles_end(); ++pitr1 ){
      n++;
      if( ( fabs((*pitr1)->pdg_id()) != fabs(m_PartId1)  && 99999 != fabs(m_PartId1) ) || //PDG ID selection
          (*pitr1)->status() != m_PartStatus  ||    //status of the particle 
          (*pitr1)->momentum().perp() < m_Ptmin1  || // pT cut
          fabs((*pitr1)->momentum().pseudoRapidity()) > m_EtaRange1
        ) continue;//eta cut
          
      log << MSG::INFO << "   type1 " << (*pitr1)->pdg_id() << " pT1 " << (*pitr1)->momentum().perp()
          << " eta1 " << (*pitr1)->momentum().pseudoRapidity() << " phi1 " << (*pitr1)->momentum().phi()
          << " stat1 " << (*pitr1)->status() << endreq;

      HepMC::GenEvent::particle_const_iterator pitr2 = genEvt->particles_begin();
      if( samePDGID12 ){
        pitr2 = pitr1;
        pitr2++;//pirt2 = pitr1 + 1  is not allowed. operator+ is not defined....
      }

      for(; pitr2!=genEvt->particles_end(); ++pitr2 ){

        if( pitr1 == pitr2 ) continue;//if the pointers are the same. 
          
        if( ( fabs((*pitr2)->pdg_id()) != fabs(m_PartId2) && 99999 != fabs(m_PartId2) ) || //PDG ID selection
            (*pitr2)->status() != m_PartStatus  ||    //status of the particle 
            (*pitr2)->momentum().perp() < m_Ptmin2  || // pT cut
            fabs((*pitr2)->momentum().pseudoRapidity()) > m_EtaRange2
          ) continue;//eta cut
        if( samePDGID12 && !samePDGID123 && (*pitr1)->pdg_id()==(*pitr2)->pdg_id()) continue;
          
        log << MSG::INFO << "   type2 " << (*pitr2)->pdg_id() << " pT2 " << (*pitr2)->momentum().perp()
            << " eta2 " << (*pitr2)->momentum().pseudoRapidity() << " phi2 " << (*pitr2)->momentum().phi()
            << " stat2 " << (*pitr2)->status() << endreq;

        HepMC::GenEvent::particle_const_iterator pitr3 = genEvt->particles_begin();
        if( samePDGID123 ){
          pitr3 = pitr2;
          pitr3++;//pirt2 = pitr1 + 1  is not allowed. operator+ is not defined....
        }

        for(; pitr3!=genEvt->particles_end(); ++pitr3 ){

          if( pitr1 == pitr3 || pitr2 == pitr3 ) continue;//if the pointers are the same. 
  
          if( ( fabs((*pitr3)->pdg_id()) != fabs(m_PartId3) && 99999 != fabs(m_PartId3) ) || //PDG ID selection
              (*pitr3)->status() != m_PartStatus  ||    //status of the particle 
              (*pitr3)->momentum().perp() < m_Ptmin3  || // pT cut
              fabs((*pitr3)->momentum().pseudoRapidity()) > m_EtaRange3
            ) continue;//eta cut
          
          log << MSG::INFO << "   type3 " << (*pitr3)->pdg_id() << " pT3 " << (*pitr3)->momentum().perp()
              << " eta3 " << (*pitr3)->momentum().pseudoRapidity() << " phi3 " << (*pitr3)->momentum().phi()
              << " stat3 " << (*pitr3)->status() << endreq;

          if( ( fabs(m_PartId1) != 99999 && fabs(m_PartId2) != 99999 && fabs(m_PartId3) != 99999 ) &&
              ( (*pitr1)->pdg_id() + (*pitr2)->pdg_id() + (*pitr3)->pdg_id() != m_PartId1 + m_PartId2 + m_PartId3 ) ) continue;
          HepMC::FourVector vec((*pitr1)->momentum().px() + (*pitr2)->momentum().px() + (*pitr3)->momentum().px(),
                                (*pitr1)->momentum().py() + (*pitr2)->momentum().py() + (*pitr3)->momentum().py(),
                                (*pitr1)->momentum().pz() + (*pitr2)->momentum().pz() + (*pitr3)->momentum().pz(),
                                (*pitr1)->momentum().e() + (*pitr2)->momentum().e() + (*pitr3)->momentum().e());
          
          double invMass = vec.m();
          
          log << MSG::INFO << " Mass " << invMass << endreq;
          
          if( m_InvMassMin<=invMass && invMass<m_InvMassMax ){
            log << MSG::INFO << "TrimuMassRangeFilter passed : MassMin <= Mass < MassMax " << m_InvMassMin << " <= " << invMass << " < " << m_InvMassMax << endreq;;
            return StatusCode::SUCCESS;
          }
          log << MSG::INFO << "TrimuMassRangeFilter not passed : MassMin <= Mass < MassMax " << m_InvMassMin << " <= " << invMass << " < " << m_InvMassMax << endreq;;
        
        }//pitr3
      }//pitr2
    }//pitr1
  }//McEventCollection

  log << MSG::INFO << "TrimuMassRangeFilter not passed at all" << endreq;;

  //Haven't found anything...
  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
