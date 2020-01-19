/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/MuonSimData.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollection_p1.h"
#include "Identifier/Identifier.h"

void MuonSimDataCollectionCnv_p1::persToTrans(const Muon::MuonSimDataCollection_p1* persCol, MuonSimDataCollection* transCol, MsgStream &log) {
   log << MSG::DEBUG << "MuonSimDataCollectionCnv_p1::persToTrans" << endreq;

   std::vector<std::pair<Identifier32::value_type, Muon::MuonSimData_p1 > >::const_iterator it_Coll = persCol->m_muonsimdata.begin();
   std::vector<std::pair<Identifier32::value_type, Muon::MuonSimData_p1 > >::const_iterator it_CollEnd = persCol->m_muonsimdata.end();
   log << MSG::DEBUG << " Preparing " << persCol->m_muonsimdata.size() << " Collections" << endreq;

   for(int collIdx=0 ; it_Coll != it_CollEnd; it_Coll++, collIdx++)  {
      const Muon::MuonSimData_p1& perssimData = persCol->m_muonsimdata[collIdx].second;
      MuonSimData transsimData;
      m_muonsimdataCnv.persToTrans(&perssimData,&transsimData,log);
      transCol->insert( transCol->end(), std::make_pair( Identifier( it_Coll->first ), transsimData ) );
   }

   log << MSG::DEBUG << " Reading MuonSimDataCollection" << endreq;

}

void MuonSimDataCollectionCnv_p1::transToPers(const MuonSimDataCollection* transCol, Muon::MuonSimDataCollection_p1* persCol, MsgStream &log) {

   log << MSG::DEBUG << "MuonSimDataCollectionCnv_p1::transToPers" << endreq;
   MuonSimDataCollection::const_iterator it_Coll = transCol->begin();
   MuonSimDataCollection::const_iterator it_CollEnd = transCol->end();

   persCol->m_muonsimdata.resize(transCol->size());
   log << MSG::DEBUG << " Preparing " << persCol->m_muonsimdata.size() << " Collections" << endreq;

   for(int collIdx=0; it_Coll != it_CollEnd; it_Coll++, collIdx++){
     (persCol->m_muonsimdata[collIdx]).first = (*it_Coll).first.get_identifier32().get_compact();
      const MuonSimData &transsimData = (*it_Coll).second;
      Muon::MuonSimData_p1 &perssimData = persCol->m_muonsimdata[collIdx].second;
      m_muonsimdataCnv.transToPers(&transsimData, &perssimData, log);
   }

   log << MSG::DEBUG << " Writing MuonSimDataCollection" << endreq;

}
