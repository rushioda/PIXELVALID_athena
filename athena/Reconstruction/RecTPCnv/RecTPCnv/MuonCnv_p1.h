///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p1.h 
// Header file for class MuonCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCNV_P1_H 
#define RECTPCNV_MUONCNV_P1_H 

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/Muon_p1.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Muon; }

class MuonCnv_p1 : public T_AthenaPoolTPCnvBase<Analysis::Muon, Muon_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;

  /** Default constructor: 
   */
  MuonCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Muon
   *  from its persistent representation @c Muon_p1
   */
  virtual void persToTrans( const Muon_p1* persObj, 
                            Analysis::Muon* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c Muon_p1
   *  from its transient representation @c Analysis::Muon
   */
  virtual void transToPers( const Analysis::Muon* transObj, 
                            Muon_p1* persObj, 
                            MsgStream& msg );

  void setKey ( const std::string key ) { m_muonCaloEnergyContainerName=key; }
  void setEventStore( const StoreGateSvc_t storeGate ) { m_storeGate=storeGate; }

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  StoreGateSvc_t m_storeGate;
  std::string m_muonCaloEnergyContainerName;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline MuonCnv_p1::MuonCnv_p1()
  : m_storeGate( "StoreGateSvc", "MuonContainerCnv" )
{}

#endif //> RECTPCNV_MUONCNV_P1_H
