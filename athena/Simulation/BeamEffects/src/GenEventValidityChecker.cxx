/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventValidityChecker.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenEventValidityChecker.h"

// HepMC includes
#include "HepMC/GenEvent.h"
// CLHEP includes
#include "CLHEP/Geometry/Point3D.h"

/** Constructor **/
Simulation::GenEventValidityChecker::GenEventValidityChecker( const std::string& t,
                                                       const std::string& n,
                                                       const IInterface* p )
  : base_class(t,n,p),
    m_checkTime(true)
{
    declareProperty( "CheckTime",
                     m_checkTime=true,
                     "Check the value in time dimension");
}


/** Athena algtool's Hooks */
StatusCode  Simulation::GenEventValidityChecker::initialize()
{

    ATH_MSG_VERBOSE("Initializing ...");
    ATH_MSG_VERBOSE("Initialize successful");
    return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  Simulation::GenEventValidityChecker::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    ATH_MSG_VERBOSE("Finalize successful");
    return StatusCode::SUCCESS;
}


/** checks the given GenEvent */
StatusCode Simulation::GenEventValidityChecker::manipulate(HepMC::GenEvent& ge) const
{
  bool allOK = true;

  // loop over the vertices in the GenEvent
  HepMC::GenEvent::vertex_iterator vtxIt  = ge.vertices_begin();
  HepMC::GenEvent::vertex_iterator vtxEnd = ge.vertices_end();
  for( ; vtxIt != vtxEnd; ++vtxIt) {
    // for quick access:
    const HepMC::FourVector &curPos = (*vtxIt)->position();

    // check if all position values are in range
    allOK &= std::isfinite( curPos.x() );
    allOK &= std::isfinite( curPos.y() );
    allOK &= std::isfinite( curPos.z() );
    // in case m_checkTime==false --> always return true here:
    allOK &= std::isfinite( curPos.t() ) || !m_checkTime;
  }

  if (allOK) {
    ATH_MSG_DEBUG("All vertices in the given GenEvent are valid.");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_ERROR("At least one vertex in the given GenEvent has an invalid position value (NaN or inf).");
  return StatusCode::FAILURE;
}
