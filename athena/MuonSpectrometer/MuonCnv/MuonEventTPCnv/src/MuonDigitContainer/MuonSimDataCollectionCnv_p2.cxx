/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollectionCnv_p2.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollection_p2.h"
#include "Identifier/Identifier.h"

void MuonSimDataCollectionCnv_p2::persToTrans(const Muon::MuonSimDataCollection_p2* persCol, MuonSimDataCollection* transCol, MsgStream &log) {
   log << MSG::DEBUG << "MuonSimDataCollectionCnv_p2::persToTrans" << endreq;

   std::vector<std::pair<Identifier32::value_type, Muon::MuonSimData_p2 > >::const_iterator it_Coll = persCol->m_muonsimdata.begin();
   std::vector<std::pair<Identifier32::value_type, Muon::MuonSimData_p2 > >::const_iterator it_CollEnd = persCol->m_muonsimdata.end();
   log << MSG::DEBUG << " Preparing " << persCol->m_muonsimdata.size() << " Collections" << endreq;

   for(int collIdx=0 ; it_Coll != it_CollEnd; it_Coll++, collIdx++)  {
      const Muon::MuonSimData_p2& perssimData = persCol->m_muonsimdata[collIdx].second;
      MuonSimData transsimData;
      m_muonsimdataCnv.persToTrans(&perssimData,&transsimData,log);
      transCol->insert( transCol->end(), std::make_pair( Identifier( it_Coll->first ), transsimData ) );
   }

   log << MSG::DEBUG << " Reading MuonSimDataCollection" << endreq;

}

void MuonSimDataCollectionCnv_p2::transToPers(const MuonSimDataCollection*, Muon::MuonSimDataCollection_p2*, MsgStream &/*log*/) {
  throw std::runtime_error("MuonSimDataCollectionCnv_p2::transToPers is not supported in this release!");
}
