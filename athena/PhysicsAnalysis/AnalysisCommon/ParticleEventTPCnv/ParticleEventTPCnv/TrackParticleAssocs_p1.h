///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackParticleAssocs_p1.h 
// Header file for class TrackParticleAssocs_p1
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCS_p1_H 
#define PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCS_p1_H

// STL includes
#include <string>
#include <vector>
#include <utility>

// DataModelAthenaPool
#include "DataModelAthenaPool/DataLink_p2.h"
#include "DataModelAthenaPool/ElementLink_p3.h"

// Forward declaration
class TrackParticleAssocsCnv_p1;

class TrackParticleAssocs_p1
{ 
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class TrackParticleAssocsCnv_p1;

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public:
  // some typedefs as a workaround for templated classes with a long name
  typedef std::vector<ElementLinkInt_p3>	ElemLinkVect_t;
  typedef std::pair<ElementLinkInt_p3,
		    ElemLinkVect_t>		AssocElem_t;
  typedef std::vector<AssocElem_t>		Assocs_t;
  typedef DataLinkVector_p2              	TrackParticleStores_t;

   /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TrackParticleAssocs_p1();

  /** Destructor: 
   */
  ~TrackParticleAssocs_p1(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  Assocs_t                m_assocs;
  TrackParticleStores_t        m_assocStores;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
TrackParticleAssocs_p1::TrackParticleAssocs_p1() :
  m_assocs(),
  m_assocStores()
{}

inline TrackParticleAssocs_p1::~TrackParticleAssocs_p1()
{}

#endif //> PARTICLEEVENTTPCNV_INAV4MOMASSOCS_p3_H
