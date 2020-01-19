/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
// File: GeneratorFilters/MultiParticleFilter.cxx
// Description:
//
//   Allows the user to search for Np particles with multiple statuses passing pT and Eta cuts.
//   The particles are required to be listed in the vectors particlePDG[] and particleStatus[].
//   The filter will search for these particles and if Np particles are found which fulfil these requirements and the kinematic cuts, the filter returns success,
//   e.g., setting Np = 2, particlePDG = [23,24] with particleStatus = [23] and ptMinParticle = 10 in the jobOptions, 
//   will accepts events with at least two W/Z bosons with status=23 and pT > 10 GeV. 
//   
//   This filter was tested and used only for Alpgen/Jimmy/Herwig events.
//   User should check if the filter works for other generators.
//
//
// AuthorList:         
//   Orel Gueta    March 2014

// Header for this module:-

#include "GeneratorFilters/MultiParticleFilter.h"

// Other classes used by this class:-
#include <math.h>

using HepMC::GenParticle;

//--------------------------------------------------------------------------
MultiParticleFilter::MultiParticleFilter(const std::string& name, ISvcLocator* pSvcLocator): GenFilter(name,pSvcLocator)
  , m_total(0)
  , m_passed(0)
{
//--------------------------------------------------------------------------    
  
    declareProperty("particlePDG",m_particlePDG); 
    declareProperty("particleStatus",m_particleStatus); 
    declareProperty("ptMinParticle",m_ptMinParticle = 0.); 
    declareProperty("ptMaxParticle",m_ptMaxParticle = 1e9);
    declareProperty("etaRangeParticle",m_etaRangeParticle = 999.0);
    declareProperty("Np",m_Np = 1);

}

//--------------------------------------------------------------------------
 MultiParticleFilter::~MultiParticleFilter(){
//--------------------------------------------------------------------------

}

//---------------------------------------------------------------------------
StatusCode MultiParticleFilter::filterInitialize() {
//---------------------------------------------------------------------------
 
    msg(MSG:: INFO) << "MultiParticleFilter INITIALISING "  << endreq;   

    if(m_particlePDG.size()==0) 
        msg(MSG:: ERROR) << "particlePDG[] not set " << endreq; 
    if(m_particleStatus.size()==0) 
        msg( MSG:: ERROR) << "particleStatus[] not set " << endreq; 

    for(unsigned int i=0; i<m_particlePDG.size(); i++) 
        msg(MSG:: INFO) << "particlePDG["<<i<<"] = " << m_particlePDG[i] << endreq;
    for(unsigned int i=0; i<m_particleStatus.size(); i++) 
        msg( MSG:: INFO) << "particleStatus["<<i<<"] = " << m_particleStatus[i] << endreq;
    
    msg(MSG:: INFO) << "ptMinParticle = " << m_ptMinParticle << endreq;
    msg(MSG:: INFO) << "ptMaxParticle = " << m_ptMaxParticle << endreq;
    msg(MSG:: INFO) << "etaRangeParticle = " << m_etaRangeParticle << endreq;
    msg(MSG:: INFO) << "Np = " << m_Np << endreq;
    
    return StatusCode::SUCCESS;

}

//---------------------------------------------------------------------------
StatusCode MultiParticleFilter::filterFinalize() {
//---------------------------------------------------------------------------
    msg(MSG:: INFO) << "Total efficiency: " << 100.*double(m_passed)/double(m_total) << "%" << endreq;
    return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode MultiParticleFilter::filterEvent() {
//---------------------------------------------------------------------------

    // Loop over all events in McEventCollection

    McEventCollection::const_iterator itr;
    for (itr = events()->begin(); itr != events()->end(); ++itr) {

        m_total++; // Bookkeeping
        unsigned int Np(0);
                
        // Loop over all particles in the event
        const HepMC::GenEvent* genEvt = (*itr);
        for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin(); pitr!=genEvt->particles_end(); ++pitr){
              
            bool passedPDG = false;
            bool passedStatus = false;
            
            for(unsigned int i=0; i<m_particlePDG.size(); i++) 
                if(abs((*pitr)->pdg_id()) == m_particlePDG[i]) passedPDG = true;
              
            for(unsigned int i=0; i<m_particleStatus.size(); i++)
                if(abs((*pitr)->status()) == m_particleStatus[i]) passedStatus = true;
              
            if(passedPDG && passedStatus)
	      if ((*pitr)->momentum().perp() >= m_ptMinParticle && (*pitr)->momentum().perp() <= m_ptMaxParticle && fabs((*pitr)->momentum().eta()) <= m_etaRangeParticle)
		Np++; // Found a particle passing all the cuts.

            // Test if we fulfilled all the requirements and return in that case.
            if (Np >= m_Np){
                setFilterPassed(true);
                m_passed++;
                return StatusCode::SUCCESS;
            }
        }	
    }
    
    // if we got here, we failed
    setFilterPassed(false);
    return StatusCode::SUCCESS;

}


