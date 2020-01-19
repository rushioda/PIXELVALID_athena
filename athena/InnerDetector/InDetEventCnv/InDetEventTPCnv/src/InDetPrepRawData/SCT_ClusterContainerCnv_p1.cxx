/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_ClusterContainerCnv_p1.h"

// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"

// Athena
#include "StoreGate/StoreGateSvc.h"



#include "DataModel/DataPool.h"

void InDet::SCT_ClusterContainerCnv_p1::transToPers(const InDet::SCT_ClusterContainer* transCont,  InDet::InDetPRD_Container_p1* persCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all RDO
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef InDet::SCT_ClusterContainer TRANS;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    SCT_ClusterCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endreq;
  
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
//         if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " New collection" << endreq;
        const InDet::SCT_ClusterCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDet::InDetPRD_Collection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_PRD.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            const InDet::SCT_Cluster* chan = collection[i];
            persCont->m_PRD[i + chanBegin] = toPersistent((CONV**)0, chan, log );
        }
    }
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing SCT_ClusterContainer ***" << endreq;
}

void  InDet::SCT_ClusterContainerCnv_p1::persToTrans(const InDet::InDetPRD_Container_p1* persCont, InDet::SCT_ClusterContainer* transCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all channels
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and extract their channels
    // from the vector.


    InDet::SCT_ClusterCollection* coll = 0;

    SCT_ClusterCnv_p1  chanCnv;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endreq;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - is NOT owner of SCT_Cluster (SG::VIEW_ELEMENTS)
	// IDet collection don't have the Ownership policy c'tor
        const InDet::InDetPRD_Collection_p1& pcoll = persCont->m_collections[icoll];        
        //Identifier collID(Identifier(pcoll.m_id));
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new InDet::SCT_ClusterCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
        InDetDD::SiDetectorElement * de = m_sctMgr->getDetectorElement(collIDHash);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const TPObjRef pchan = persCont->m_PRD[ichan + pcoll.m_begin];
            InDet::SCT_Cluster* chan = dynamic_cast<InDet::SCT_Cluster*>(createTransFromPStore((CONV**)0, pchan, log ) );
            chan->m_detEl = de;
            (*coll)[ichan] = chan;
        }
        
        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
//         if (log.level() <= MSG::DEBUG) {
//             log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
// collID.get_compact() << ", added to Identifiable container." << endreq;
//         }
    }

//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading SCT_ClusterContainer" << endreq;
}



//================================================================
InDet::SCT_ClusterContainer* InDet::SCT_ClusterContainerCnv_p1::createTransient(const InDet::InDetPRD_Container_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize SCT_ClusterContainerCnv_p1 " << endreq;
     } 
    }
    std::auto_ptr<InDet::SCT_ClusterContainer> trans(new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}

StatusCode InDet::SCT_ClusterContainerCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;
   
   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return StatusCode::FAILURE;
   } 
   //   else {
   //      if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endreq;
   //   }

   // Get the SCT helper from the detector store
   sc = detStore->retrieve(m_sctId, "SCT_ID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_ID helper !" << endreq;
      return StatusCode::FAILURE;
   } 
   //   else {
   //      if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the SCT_ID helper." << endreq;
   //   }

   sc = detStore->retrieve(m_sctMgr);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_DetectorDescription" << endreq;
      return sc;
   }

   //   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endreq;
   return StatusCode::SUCCESS;
}
