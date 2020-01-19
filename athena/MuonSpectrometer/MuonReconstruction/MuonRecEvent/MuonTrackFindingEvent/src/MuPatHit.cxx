/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackFindingEvent/MuPatHit.h"
 
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventPrimitives/ResidualPull.h"

#include <iostream> 

namespace Muon {
  
 
  // Static members
  unsigned int MuPatHit::s_maxNumberOfInstantiations = 0;
  unsigned int MuPatHit::s_numberOfInstantiations = 0;
  unsigned int MuPatHit::s_numberOfCopies = 0;

  // Static functions
  void MuPatHit::resetMaxNumberOfInstantiations() {
    s_maxNumberOfInstantiations = 0;
  }

  void MuPatHit::resetNumberOfCopies() {
    s_numberOfCopies = 0;
  }

  // member functions
  MuPatHit::MuPatHit( const Trk::TrackParameters* pars, const Trk::MeasurementBase* presMeas, const Trk::MeasurementBase* broadMeas, 
		    const Info& info ) :
    m_pars(pars),m_precisionMeas(presMeas),m_broadMeas(broadMeas),m_info(info)
  {
//     std::cout << " new MuPatHit  " << this;
//     if( pars ) std::cout << " theta " << pars->momentum().theta() << " phi " << pars->momentum().phi();
//     std::cout << std::endl;
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
#endif
  }

  MuPatHit::MuPatHit( const MuPatHit& hit )
  {
//     std::cout << " ctor MuPatHit  " << this << std::endl;
    copy(hit);
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
    ++s_numberOfCopies;
#endif
  }

  MuPatHit& MuPatHit::operator=( const MuPatHit& hit ) {
    if( &hit != this ){
      cleanUp();
      copy(hit);
#ifdef MCTB_OBJECT_COUNTERS
      ++s_numberOfCopies;
#endif
    }
    return *this;
  }

  MuPatHit::~MuPatHit() {
//     std::cout << " delete MuPatHit  " << this << std::endl;
    cleanUp();
#ifdef MCTB_OBJECT_COUNTERS
    removeInstance();
#endif
  }
  
  void MuPatHit::copy( const MuPatHit& hit ){
    m_pars = hit.m_pars;
    m_precisionMeas = hit.m_precisionMeas;
    m_broadMeas = hit.m_broadMeas->clone();
    m_info = hit.m_info;
  }

  void MuPatHit::cleanUp(){
    delete m_broadMeas;
  }

  void MuPatHit::updateParameters( const Trk::TrackParameters* pars ) {
//     if( pars ) std::cout << " update pars " << this << " theta " << pars->momentum().theta() << " phi " << pars->momentum().phi() << std::endl;
    m_pars = pars;
  }


}
