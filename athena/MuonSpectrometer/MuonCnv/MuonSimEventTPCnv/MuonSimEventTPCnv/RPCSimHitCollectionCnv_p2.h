/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCSimHitCOLLECTIONCNV_P2_H
#define RPCSimHitCOLLECTIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "RPCSimHitCollection_p2.h"
// namespace Muon {
//     class RPCSimHitCollection_p1;
// }

class RPCSimHitCollectionCnv_p2 : public T_AthenaPoolTPCnvBase<RPCSimHitCollection, Muon::RPCSimHitCollection_p2>
{
 public:
  
  RPCSimHitCollectionCnv_p2()  {};
  
  virtual RPCSimHitCollection* createTransient(const Muon::RPCSimHitCollection_p2* persObj, MsgStream &log);
  virtual void  persToTrans(const Muon::RPCSimHitCollection_p2* persCont,
                RPCSimHitCollection* transCont,
                MsgStream &log) ;
  virtual void  transToPers(const RPCSimHitCollection* transCont,
                Muon::RPCSimHitCollection_p2* persCont,
                MsgStream &log) ;

};

#endif

