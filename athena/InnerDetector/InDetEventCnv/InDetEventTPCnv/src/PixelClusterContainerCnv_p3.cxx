/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p3.h"
#include "InDetEventTPCnv/PixelClusterContainer_p3.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p2.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p3.h"
#include "InDetEventTPCnv/PixelClusterContainerCnv_p3.h"
#include "DataModel/DataPool.h"

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


void PixelClusterContainerCnv_p3::transToPers(const InDet::PixelClusterContainer* transCont, InDet::PixelClusterContainer_p3* persCont, MsgStream &log) {

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all PRD
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef InDet::PixelClusterContainer TRANS;
        
    // this is the id of the latest collection read in
    // This starts from the base of the TRT identifiers
    unsigned int idLast(0);

    // 
    //PixelClusterCnv_p3  chanCnv;
    if(!m_isInitialized) {
      if (this->initialize(log) != StatusCode::SUCCESS) {
	log << MSG::FATAL << "Could not initialize PixelClusterContainerCnv_p3 " << endreq;
      }
    }
    PixelClusterCnv_p3  chanCnv(m_pixId);

    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endreq;
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        const InDet::PixelClusterCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDet::InDetPRD_Collection_p2& pcollection = persCont->m_collections[collIndex];
	unsigned int deltaId = (collection.identifyHash()-idLast);
	//        unsigned int deltaId = (collection.identify().get_compact()-idLast)/IDJUMP;
        // if(deltaId*IDJUMP != collection.identify().get_compact()-idLast ) 
        //   log << MSG::FATAL << "THere is a mistake in Identifiers of the collection" << endreq;
        // if(deltaId > 0xFFFF) {
        //   log << MSG::FATAL << "Fixme!!! This is too big, something needs to be done " << endreq;
        // }
        // pcollection.m_idDelta = (unsigned short) deltaId;
        // idLast = collection.identify().get_compact(); // then update the last identifier 
        pcollection.m_hashId = deltaId;
	idLast=collection.identifyHash();
        pcollection.m_size = collection.size();
        // Add in channels
        persCont->m_rawdata.resize(chanEnd);
	//        if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "Reading collections with " <<  collection.size() << "PRDs " << endreq;
        for (unsigned int i = 0; i < collection.size(); ++i) {
            InDet::PixelCluster_p3* pchan = &(persCont->m_rawdata[i + chanBegin]);
            const InDet::PixelCluster* chan = dynamic_cast<const InDet::PixelCluster*>(collection[i]);
            chanCnv.transToPers(chan, pchan, log);
        }
    }
    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing InDet::PixelClusterContainer" << endreq;
}

void  PixelClusterContainerCnv_p3::persToTrans(const InDet::PixelClusterContainer_p3* persCont, InDet::PixelClusterContainer* transCont, MsgStream &log) 
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


    InDet::PixelClusterCollection* coll = 0;

    //PixelClusterCnv_p3  chanCnv;
    PixelClusterCnv_p3  chanCnv(m_pixId);
    unsigned int collBegin(0);
    // this is the id of the latest collection read in
    // This starts from the base of the TRT identifiers
    unsigned int idLast(0);
    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endreq;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - in NOT owner of PixelDriftCircle (SG::VIEW_ELEMENTS)
	// IDet collection don't have the Ownership policy c'tor
        const InDet::InDetPRD_Collection_p2& pcoll = persCont->m_collections[icoll];       
        idLast += pcoll.m_hashId;
        // Identifier collID= Identifier(idLast);
        IdentifierHash collIDHash=IdentifierHash((unsigned int) idLast);
        Identifier collID = m_pixId->wafer_id(collIDHash);
        coll = new InDet::PixelClusterCollection(collIDHash);
        coll->setIdentifier(Identifier(collID));
        unsigned int nchans           = pcoll.m_size;
        coll->resize(nchans);
        InDetDD::SiDetectorElement * de = m_pixMgr->getDetectorElement(collIDHash);
        // Fill with channels:
        // This is used to read the vector of errMat
        // values and lenght of the value are specified in separate vectors
        //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Reading collection with " << nchans << "Channels " << endreq;
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const InDet::PixelCluster_p3* pchan = &(persCont->m_rawdata[ichan + collBegin]);
            // In preparation for 64-bit ids, set the initial cluster
            // id to the wafer id. Actual cluster id will be set in
            // the converter from diff.
            InDet::PixelCluster* chan = new InDet::PixelCluster
              (chanCnv.createPixelCluster (pchan, collID, de, log));
	    // DC Bugfix: Set the hash index!
            chan->setHashAndIndex(collIDHash, ichan);
            (*coll)[ichan] = chan;
        }
        collBegin += pcoll.m_size;

        // register the PRD collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
	//        if (log.level() <= MSG::DEBUG) {
	//            log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << collID << ", added to Identifiable container." << endreq;
	//        }
    }

    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading InDet::PixelClusterContainer" << endreq;
}



//================================================================
InDet::PixelClusterContainer* PixelClusterContainerCnv_p3::createTransient(const InDet::PixelClusterContainer_p3* persObj, MsgStream& log) {
    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PixelClusterContainerCnv_p3::createTransient called " << endreq;
    if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize PixelClusterContainerCnv_p3 " << endreq;
     }
    }
    std::auto_ptr<InDet::PixelClusterContainer> trans(new InDet::PixelClusterContainer(m_pixId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}


StatusCode PixelClusterContainerCnv_p3::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "PixelClusterContainerCnv_p3::initialize called " << endreq;
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
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endreq;
   //   }

   // Get the sct helper from the detector store
   sc = detStore->retrieve(m_pixId, "PixelID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
      return StatusCode::FAILURE;
   } 
   //   else {
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the PixelID helper." << endreq;
   //   }

   sc = detStore->retrieve(m_pixMgr);
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelDetectorDescription" << endreq;
      return sc;
   }

   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endreq;
   return StatusCode::SUCCESS;
}
