///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainer_p2.h 
// Header file for class egDetailContainer_p2
// Date:   April 2009
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGDETAIL_P2_H 
#define EGAMMAEVENTTPCNV_EGDETAIL_P2_H 

// STL includes
#include <vector>
#include <string>

// forward declarations
class egDetailCnv_p2;

class egDetail_p2 
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  //  friend class egDetailCnv_p2;
  friend class egDetailContainerCnv_p2;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  egDetail_p2();

  /** Destructor: 
   */
  ~egDetail_p2();

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

  /// egDetail Parameters and name of class
  std::string m_className;
  std::vector<unsigned int> m_egDetailEnumParams;
  std::vector<float> m_egDetailFloatParams;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egDetail_p2::egDetail_p2()
{}

#endif //> EGAMMAEVENTTPCNV_EGDETAIL_P2_H
