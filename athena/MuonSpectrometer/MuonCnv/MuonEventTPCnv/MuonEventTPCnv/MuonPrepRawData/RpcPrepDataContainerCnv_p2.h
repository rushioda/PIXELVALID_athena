/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCPREPDATACONTAINERCNV_p2_H
#define RPCPREPDATACONTAINERCNV_p2_H

//-----------------------------------------------------------------------------
//
// file:   RpcPrepDataContainerCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p1.h"

namespace MuonGM{ class MuonDetectorManager;}

class MsgStream;
class StoreGateSvc;
class RpcIdHelper;

namespace Muon{
    class RpcPrepDataContainerCnv_p2 : public T_AthenaPoolTPCnvBase< Muon::RpcPrepDataContainer, Muon::MuonPRD_Container_p1 >

    {
    public:
        typedef Muon::MuonPRD_Container_p1 PERS; 
        typedef Muon::RpcPrepDataContainer TRANS;
        RpcPrepDataContainerCnv_p2(): m_isInitialized(0) {}
        virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log); 
        virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
        virtual Muon::RpcPrepDataContainer* createTransient(const Muon::MuonPRD_Container_p1* persObj, MsgStream& log);
    private:
        StatusCode initialize(MsgStream &log);

        const RpcIdHelper *m_RpcId;
        StoreGateSvc *m_storeGate;
        const MuonGM::MuonDetectorManager* m_muonDetMgr;
        bool m_isInitialized;
    };

}
#endif 

