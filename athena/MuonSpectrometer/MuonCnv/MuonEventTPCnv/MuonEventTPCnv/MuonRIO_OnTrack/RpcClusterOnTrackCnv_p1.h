/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPC_CLUSTER_ON_TRACK_CNV_P1_H
#define RPC_CLUSTER_ON_TRACK_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrackCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrack_p1.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"

#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModel/ElementLink.h"
#include "MuonPrepRawData/RpcPrepData.h"

class MsgStream;


class RpcClusterOnTrackCnv_p1
    : public T_AthenaPoolTPPolyCnvBase<     Trk::MeasurementBase,
                                            Muon::RpcClusterOnTrack,
                                            Muon::RpcClusterOnTrack_p1 >
{
public:
    RpcClusterOnTrackCnv_p1() : m_RIOCnv(0) {}

    void persToTrans( const Muon :: RpcClusterOnTrack_p1 *persObj,
        Muon :: RpcClusterOnTrack    *transObj,
        MsgStream                       &log );
    void transToPers( const Muon :: RpcClusterOnTrack    *transObj,
        Muon :: RpcClusterOnTrack_p1 *persObj,
        MsgStream                       &log );

protected:
    RIO_OnTrackCnv_p1*                                  m_RIOCnv;
    ElementLinkCnv_p1< ElementLinkToIDC_RPC_Container > m_elCnv;     
};

#endif // RPC_CLUSTER_ON_TRACK_CNV_P1_H
