///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleBase_p2.h 
// Header file for class ParticleBase_p2
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLEBASE_P2_H 
#define PARTICLEEVENTTPCNV_PARTICLEBASE_P2_H 

// STL includes
#include <vector>

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/AthenaBarCode_p1.h"

// Forward declaration
class ParticleBaseCnv_p2;

class ParticleBase_p2
{ 

  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class ParticleBaseCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ParticleBase_p2();

  /** Destructor: 
   */
  ~ParticleBase_p2() {}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Charge of the particle
  float m_charge;

  /// switch to know if the particle has charge informations
  bool m_hasCharge;

  /// switch to know if the particle has PDG Id informations
  bool m_hasPdgId;

  /// ParticleDataGroup Id of the particle
  int m_pdgId;

  /// type of data (see @c IParticle for more infos)
  int m_dataType;

  /// link to the particle's origin
  ElementLinkInt_p3 m_origin;

  /// athena barcode
  AthenaBarCode_p1 m_athenabarcode;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> PARTICLEEVENTTPCNV_PARTICLEBASE_P2_H
