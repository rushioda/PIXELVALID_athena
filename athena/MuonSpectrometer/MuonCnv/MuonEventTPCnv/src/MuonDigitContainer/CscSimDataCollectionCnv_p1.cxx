/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimData/CscSimData.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollectionCnv_p1.h"
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCollection_p1.h"
#include "Identifier/Identifier.h"

void CscSimDataCollectionCnv_p1::persToTrans(const Muon::CscSimDataCollection_p1* persCol, CscSimDataCollection* transCol, MsgStream &log) {
   log << MSG::DEBUG << "CscSimDataCollectionCnv_p1::persToTrans" << endreq;

   std::vector<std::pair<Identifier32::value_type, Muon::CscSimData_p1 > >::const_iterator it_Coll = persCol->m_cscsimdata.begin();
   std::vector<std::pair<Identifier32::value_type, Muon::CscSimData_p1 > >::const_iterator it_CollEnd = persCol->m_cscsimdata.end();
   log << MSG::DEBUG << " Preparing " << persCol->m_cscsimdata.size() << " Collections" << endreq;

   for(int collIdx=0 ; it_Coll != it_CollEnd; it_Coll++, collIdx++)  {
      const Muon::CscSimData_p1& perssimData = persCol->m_cscsimdata[collIdx].second;
      CscSimData transsimData;
      m_cscsimdataCnv.persToTrans(&perssimData,&transsimData,log);
      transCol->insert( transCol->end(), std::make_pair( Identifier( it_Coll->first ), transsimData ) );
   }

   log << MSG::DEBUG << " Reading CscSimDataCollection" << endreq;

}

void CscSimDataCollectionCnv_p1::transToPers(const CscSimDataCollection* transCol, Muon::CscSimDataCollection_p1* persCol, MsgStream &log) {

   log << MSG::DEBUG << "CscSimDataCollectionCnv_p1::transToPers" << endreq;
   CscSimDataCollection::const_iterator it_Coll = transCol->begin();
   CscSimDataCollection::const_iterator it_CollEnd = transCol->end();

   persCol->m_cscsimdata.resize(transCol->size());
   log << MSG::DEBUG << " Preparing " << persCol->m_cscsimdata.size() << " Collections" << endreq;

   for(int collIdx=0; it_Coll != it_CollEnd; it_Coll++, collIdx++){
      (persCol->m_cscsimdata[collIdx]).first = (*it_Coll).first.get_identifier32().get_compact();
      const CscSimData &transsimData = (*it_Coll).second;
      Muon::CscSimData_p1 &perssimData = persCol->m_cscsimdata[collIdx].second;
      m_cscsimdataCnv.transToPers(&transsimData, &perssimData, log);
   }

   log << MSG::DEBUG << " Writing CscSimDataCollection" << endreq;

}
