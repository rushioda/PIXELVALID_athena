///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Electron_p2.h 
// Header file for class Electron_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRON_P2_H 
#define EGAMMAEVENTTPCNV_ELECTRON_P2_H 

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p2.h"

// forward declarations
class ElectronCnv_p2;

class Electron_p2 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class ElectronCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  Electron_p2();

  /** Destructor: 
   */
  ~Electron_p2();

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

  /// the egamma part 
  egamma_p2 m_egamma;
  
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline Electron_p2::Electron_p2() :
  m_egamma()
{}

#endif //> EGAMMAEVENTTPCNV_ELECTRON_P2_H