/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TgcPREPDATACONTAINERCNV_P3_H
#define TgcPREPDATACONTAINERCNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/TgcPrepDataContainer.h"

#include "MuonEventTPCnv/MuonPrepRawData/MuonPRD_Container_p2.h"


#include <iostream>

class StoreGateSvc;
class TgcIdHelper;
namespace MuonGM {class MuonDetectorManager;}

namespace Muon{
/** Class to handle the conversion of the transient TgcPrepDataContainer into its persistent representation (defined in MuonPRD_Container_p3).
This replaces TgcPrepDataContainerCnv_tlp1, which is a AthenaPoolTopLevelTPConverter, as I have now simplified/flattened the TgcPrepData.*/
class TgcPrepDataContainerCnv_p3 : public T_AthenaPoolTPCnvBase<Muon::TgcPrepDataContainer, Muon::TgcPrepDataContainer_p3>
{
public:
  TgcPrepDataContainerCnv_p3() : m_TgcId(0), m_storeGate(0), m_muonDetMgr(0), m_isInitialized(0)  {};

    virtual void	persToTrans(const Muon::TgcPrepDataContainer_p3* persCont,
        Muon::TgcPrepDataContainer* transCont,
        MsgStream &log) ;
    virtual void	transToPers(const Muon::TgcPrepDataContainer* transCont,
        Muon::TgcPrepDataContainer_p3* persCont,
        MsgStream &log) ;

    virtual Muon::TgcPrepDataContainer* createTransient(const Muon::TgcPrepDataContainer_p3* persObj, MsgStream& log);


private:
    const TgcIdHelper *m_TgcId;
    StoreGateSvc *m_storeGate;
    const MuonGM::MuonDetectorManager* m_muonDetMgr;
    bool m_isInitialized;
    StatusCode initialize(MsgStream &log);
};
}


#endif


