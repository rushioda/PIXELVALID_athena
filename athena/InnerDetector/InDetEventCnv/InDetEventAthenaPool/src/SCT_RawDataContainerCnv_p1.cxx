/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/SCT3_RawData.h"
#include "InDetEventAthenaPool/SCT3_RawData_p1.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "MsgUtil.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "SCT1_RawDataCnv_p1.h"
#include "SCT3_RawDataCnv_p1.h"
#include "SCT_RawDataContainerCnv_p1.h"

void SCT_RawDataContainerCnv_p1::transToPers(const SCT_RDO_Container* transCont, SCT_RawDataContainer_p1* persCont, MsgStream& log) 
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

    typedef SCT_RDO_Container TRANS;

    SCT1_RawDataCnv_p1  chan1Cnv;
    SCT3_RawDataCnv_p1  chan3Cnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    int numColl = transCont->numberOfCollections();
    //if(numColl == transCont->fullSize() ) { // let's count how many collections we have:
    // numColl = 0;
    // for ( ; it_Coll != it_CollEnd; it_Coll++)
    //    numColl++;
    // it_Coll     = transCont->begin(); // reset the iterator, we used it!
    // }
    persCont->m_collections.resize(numColl);
    //    MSG_DEBUG(log," Preparing " << persCont->m_collections.size() << "Collections");
  
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
        const SCT_RDO_Collection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        InDetRawDataCollection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
	//        MSG_VERBOSE(log,"Writing collection number " << collIndex << " with hash" << pcollection.m_hashId);
        // Add in channels
        if(m_type == 1) { 
           persCont->m_rawdata.resize(chanEnd);
           for (unsigned int i = 0; i < collection.size(); ++i) {
               InDetRawData_p1* pchan = &(persCont->m_rawdata[i + chanBegin]);
               const SCT1_RawData* chan = dynamic_cast<const SCT1_RawData*>(collection[i]);
               chan1Cnv.transToPers(chan, pchan, log);
           }            
        } else if (m_type == 3) {
           persCont->m_sct3data.resize(chanEnd);
           for (unsigned int i = 0; i < collection.size(); ++i) {
               SCT3_RawData_p1* pchan = &(persCont->m_sct3data[i + chanBegin]);
               const SCT3_RawData* chan = dynamic_cast<const SCT3_RawData*>(collection[i]);
               chan3Cnv.transToPers(chan, pchan, log);
           }            
        }
    }
    //  MSG_DEBUG(log," ***  Writing SCT_RDO_Container (SCT1/3_RawData concrete type)");
}

void  SCT_RawDataContainerCnv_p1::persToTrans(const SCT_RawDataContainer_p1* persCont, SCT_RDO_Container* transCont, MsgStream &log) 
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


    SCT_RDO_Collection* coll = 0;

    SCT1_RawDataCnv_p1  chan1Cnv;
    SCT3_RawDataCnv_p1  chan3Cnv;
    // check for the type of the contained objects:
    // 
    if(persCont->m_rawdata.size() !=0 && persCont->m_sct3data.size() != 0) 
      log << MSG::FATAL << "The collection has mixed SCT1 and SCT3 elements, this is not allowed " << endreq;
    if(persCont->m_rawdata.size() != 0 ) m_type = 1;
    if(persCont->m_sct3data.size() != 0 ) m_type = 3;

    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - in NOT owner of SCT_RDO_RawData (SG::VIEW_ELEMENTS)
	// IDet collection don't have the Ownership policy c'tor
        const InDetRawDataCollection_p1& pcoll = persCont->m_collections[icoll];        
        Identifier collID(pcoll.m_id);
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new SCT_RDO_Collection(collIDHash );
        coll->setIdentifier(collID);
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
	//        MSG_VERBOSE(log,"Reading collection with id = " << pcoll.m_id << " id hash= " << pcoll.m_hashId << " and num el= " << nchans);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
          if(m_type == 1) { 
            const InDetRawData_p1* pchan = &(persCont->m_rawdata[ichan + pcoll.m_begin]);
            SCT1_RawData* chan = new SCT1_RawData();
            chan1Cnv.persToTrans(pchan, chan, log);
            (*coll)[ichan] = chan;
          } else if (m_type == 3) {  
            const SCT3_RawData_p1* pchan = &(persCont->m_sct3data[ichan + pcoll.m_begin]);
            SCT3_RawData* chan = new SCT3_RawData();
            chan3Cnv.persToTrans(pchan, chan, log);
            (*coll)[ichan] = chan;
          }
        }
        
        // register the rdo collection in IDC with hash - faster addCollection
           StatusCode sc = transCont->addCollection(coll, collIDHash);
           if (sc.isFailure()) {
               throw std::runtime_error("Failed to add collection to ID Container");
           }
	   MSG_VERBOSE(log,"AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = "
		       << (int) collIDHash << " / " << collID.get_compact() << ", added to Identifiable container.");
    }

//    MSG_DEBUG(log," ***  Reading SCT_RDO_Container (SCT1/3_RawData concrete type)");
}

//================================================================
SCT_RDO_Container* SCT_RawDataContainerCnv_p1::createTransient(const SCT_RawDataContainer_p1* persObj, MsgStream& log) {
    std::unique_ptr<SCT_RDO_Container> trans(new SCT_RDO_Container(m_sctId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}

