/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/SCT3_RawData.h"
#include "InDetEventAthenaPool/SCT3_RawData_p2.h"
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "MsgUtil.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetRawData/SCT_RDO_Collection.h"
#include "SCT1_RawDataCnv_p1.h"
#include "SCT3_RawDataCnv_p2.h"
#include "SCT_RawDataContainerCnv_p2.h"

//#define SCT_DEBUG

void SCT_RawDataContainerCnv_p2::transToPers(const SCT_RDO_Container* transCont, SCT_RawDataContainer_p2* persCont, MsgStream &log) 
{

  /// The transient model has a container holding collections and the
  /// collections hold channels.
  ///
  /// The persistent model flattens this so that the persistent
  /// container has two vectors:
  ///   1) all collections, and
  ///   2) all RDO
  ///
  /// The persistent collections, then only maintain indexes into the
  /// container's vector of all channels. 
  ///
  /// So here we loop over all collection and add their channels
  /// to the container's vector, saving the indexes in the
  /// collection. 
  
  typedef SCT_RDO_Container TRANS;
  
  SCT1_RawDataCnv_p1  chan1Cnv;
  SCT3_RawDataCnv_p2  chan3Cnv;
  TRANS::const_iterator it_Coll     = transCont->begin();
  TRANS::const_iterator it_CollEnd  = transCont->end();
  unsigned int collIndex;
  unsigned int chanBegin = 0;
  unsigned int chanEnd = 0;
  int numColl = transCont->numberOfCollections();
  persCont->m_collections.resize(numColl);
#ifdef SCT_DEBUG
  MSG_DEBUG(log,"SCT_RawDataContainerCnv_p2  Preparing " << persCont->m_collections.size() << "Collections");
#endif

  /** we're going to store all the strip errors from each RDO 
   * in vectors in the collection */
  (persCont->m_numErrorsInRDO).clear();
  (persCont->m_allErrorsInContainer).clear();

  for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
    /** Add in new collection */
    const SCT_RDO_Collection& collection = (**it_Coll);
    chanBegin  = chanEnd;
    chanEnd   += collection.size();
    InDetRawDataCollection_p1& pcollection = persCont->m_collections[collIndex];
    pcollection.m_id    = collection.identify().get_compact();
    pcollection.m_hashId = (unsigned int) collection.identifyHash();
    pcollection.m_begin = chanBegin;
    pcollection.m_end   = chanEnd;
#ifdef SCT_DEBUG
    MSG_DEBUG(log,"Writing collection number " << collIndex << " with hash" << pcollection.m_hashId);
#endif
    /** Add in channels */
    if(m_type == 1) { 
#ifdef SCT_DEBUG
      MSG_DEBUG(log,"SCT_RawDataContainerCnv_p2 m_type is 1");
#endif
      persCont->m_rawdata.resize(chanEnd);
      for (unsigned int i = 0; i < collection.size(); ++i) {
	InDetRawData_p1* pchan = &(persCont->m_rawdata[i + chanBegin]);
	const SCT1_RawData* chan = dynamic_cast<const SCT1_RawData*>(collection[i]);
	chan1Cnv.transToPers(chan, pchan, log);
      }            
    } else if (m_type == 3) {
#ifdef SCT_DEBUG
      MSG_DEBUG(log,"SCT_RawDataContainerCnv_p2 m_type is 3");
#endif
      persCont->m_sct3data.resize(chanEnd);
      for (unsigned int i = 0; i < collection.size(); ++i) {
	SCT3_RawData_p2* pchan = &(persCont->m_sct3data[i + chanBegin]);
	const SCT3_RawData* chan = dynamic_cast<const SCT3_RawData*>(collection[i]);
	chan3Cnv.transToPers(chan, pchan, log);
	const std::vector<int>& errHit = chan->getErrorCondensedHit();
	persCont->m_numErrorsInRDO.push_back(errHit.size() );
        persCont->m_allErrorsInContainer.insert (persCont->m_allErrorsInContainer.end(),
                                                 errHit.begin(),
                                                 errHit.end());
      }            
    }
  }
#ifdef SCT_DEBUG
  MSG_DEBUG(log," ***  Writing SCT_RDO_Container (SCT1/3_RawData concrete type)");
#endif
}

void  SCT_RawDataContainerCnv_p2::persToTrans(const SCT_RawDataContainer_p2* persCont, SCT_RDO_Container* transCont, MsgStream &log) 
{

    /// The transient model has a container holding collections and the
    /// collections hold channels.
    ///
    /// The persistent model flattens this so that the persistent
    /// container has two vectors:
    ///   1) all collections, and
    ///   2) all channels
    ///
    /// The persistent collections, then only maintain indexes into the
    /// container's vector of all channels. 
    ///
    /// So here we loop over all collection and extract their channels
    /// from the vector.


    SCT_RDO_Collection* coll = 0;

    SCT1_RawDataCnv_p1  chan1Cnv;
    SCT3_RawDataCnv_p2  chan3Cnv;
    /** check for the type of the contained objects: */
    
    if(persCont->m_rawdata.size() !=0 && persCont->m_sct3data.size() != 0) 
       log << MSG::FATAL << "The collection has mixed SCT1 and SCT3 elements, this is not allowed " << endreq;
    if(persCont->m_rawdata.size() != 0 ) m_type = 1;
    if(persCont->m_sct3data.size() != 0 ) m_type = 3;
#ifdef SCT_DEBUG
    MSG_DEBUG(log," Reading " << persCont->m_collections.size() << " Collections");
#endif
    int errCount = 0;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

      /** Create trans collection - in NOT owner of SCT_RDO_RawData (SG::VIEW_ELEMENTS)
       * IDet collection don't have the Ownership policy c'tor */
        const InDetRawDataCollection_p1& pcoll = persCont->m_collections[icoll];        
        Identifier collID(pcoll.m_id);
        IdentifierHash collIDHash(pcoll.m_hashId);
        coll = new SCT_RDO_Collection(IdentifierHash(collIDHash));
        coll->setIdentifier(collID);
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;

        coll->resize(nchans);
#ifdef SCT_DEBUG
        MSG_DEBUG(log,"Reading collection with id = " << pcoll.m_id << " id hash= " << pcoll.m_hashId
		  << " and num el= " << nchans << " m_type is "<<m_type);
#endif
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
          if(m_type == 1) { 
            const InDetRawData_p1* pchan = &(persCont->m_rawdata[ichan + pcoll.m_begin]);
            SCT1_RawData* chan = new SCT1_RawData();
            chan1Cnv.persToTrans(pchan, chan, log);
            (*coll)[ichan] = chan;
          } else if (m_type == 3) {  
            const SCT3_RawData_p2* pchan = &(persCont->m_sct3data[ichan + pcoll.m_begin]);
	    /** now need to fill the vector of errors for the transient RDO
	     *  using the vectors stored in the persistent collection */
            std::vector<int> errorCondensedHit;
	    int errEnd = errCount + persCont->m_numErrorsInRDO[ichan + pcoll.m_begin];
	    for ( ; errCount < errEnd ; ++errCount) 
	      errorCondensedHit.push_back(persCont->m_allErrorsInContainer[errCount]);

            (*coll)[ichan] = new SCT3_RawData (Identifier (pchan->m_rdoId),
                                               pchan->m_word,
                                               std::move(errorCondensedHit));
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
#ifdef SCT_DEBUG
    MSG_DEBUG(log," ***  Reading SCT_RDO_Container (SCT1/3_RawData concrete type)");
#endif
}

//================================================================
SCT_RDO_Container* SCT_RawDataContainerCnv_p2::createTransient(const SCT_RawDataContainer_p2* persObj, MsgStream& log) {
#ifdef SCT_DEBUG
  MSG_DEBUG(log,"creating transient SCT_RDO_Container");
#endif
  std::unique_ptr<SCT_RDO_Container> trans(new SCT_RDO_Container(m_sctId->wafer_hash_max()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}

