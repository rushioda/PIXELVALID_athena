/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "CscRawDataCollectionCnv_p3.h"
#include <sstream>

void
CscRawDataCollectionCnv_p3::persToTrans(const CscRawDataCollection_p3* persColl, CscRawDataCollection* transColl, MsgStream &log) 
{
   //log << MSG::DEBUG  << " ***  Reading CscRawDataCollection" << endmsg;

   IdentifierHash idhash = transColl->identifyHash();
   transColl->setOnlineId (persColl->m_id);
   transColl->setIdentifyHash (idhash);
   transColl->setRodId (persColl->m_rodId);
   transColl->setSubDetectorId (persColl->m_subDetectorId);
   transColl->setNumDPU (persColl->m_numRPU);
   transColl->set_samplingPhase (persColl->m_samplingPhase);
   transColl->set_triggerType (persColl->m_triggerType);
   transColl->set_firstBitSummary (persColl->m_firstBitSummary);
   transColl->set_eventType (persColl->m_eventType);
   transColl->setRPUID (persColl->m_rpuID);
   transColl->set_dataType (persColl->m_dataType);
   transColl->set_spuCount (persColl->m_spuCount);
   transColl->set_scaAddress (persColl->m_scaAddress);

   // Invoke vector converter from the base template
   CscRawDataCollection_Cnv_p3_base_t::persToTrans( persColl, transColl, log );

   // debug output
   // for(CscRawDataCollection::const_iterator it=transColl->begin(); it != transColl->end(); ++it)  {
   //    std::ostringstream ostr;
   //    ostr << **it;
   //    log <<  MSG::VERBOSE << "Retrieved " << ostr.str() << endmsg;
   // }  
}



void
CscRawDataCollectionCnv_p3::transToPers(const CscRawDataCollection* transColl, CscRawDataCollection_p3* persColl, MsgStream &log) 
{
    persColl->m_id             = transColl->identify();
    persColl->m_rodId          = transColl->rodId() ;
    persColl->m_subDetectorId  = transColl->subDetectorId();
    persColl->m_numRPU         = transColl->numRPU();
    persColl->m_samplingPhase  = transColl->samplingPhase() ;
    persColl->m_triggerType    = transColl->triggerType() ;
    persColl->m_firstBitSummary= transColl->firstBitSummary();
    persColl->m_eventType      = transColl->eventType();
    persColl->m_rpuID          = transColl->rpuID();
    persColl->m_dataType       = transColl->dataType();
    persColl->m_spuCount       = transColl->spuCount();
    persColl->m_scaAddress     = transColl->scaAddress();
    
    // Invoke vector converter from the base template	 
    CscRawDataCollection_Cnv_p3_base_t::transToPers( transColl, persColl, log );
}

