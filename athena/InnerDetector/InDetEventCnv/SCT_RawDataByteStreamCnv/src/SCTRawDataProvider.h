/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
/// Top algorithm to decode the SCT BS
/// copied from TRT equivalent
/// Gets vector of ROBFragments from the ROBDataProviderSvc
/// and uses AlgTools (SCTRawDataProviderTool and SCT_RodDecoder
/// to read the ByteStream and make RDOs.
/// Output is one RDO container (IDC) per event, which contains
/// one Collection per link (8196 in total), each of which contains
/// RDOs for hits (one RDO per strip in expanded mode, one per cluster
/// in condensed mode).
///////////////////////////////////////////////////////////////////

#ifndef SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H
#define SCT_RAWDATABYTESTREAMCNV_SCTRAWDATAPROVIDER_H

/** Base class */
#include "AthenaBaseComps/AthAlgorithm.h"

/** Gaudi */
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
/** other */
#include "InDetRawData/InDetTimeCollection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "StoreGate/WriteHandleKey.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
/** STL */
#include <string>

/** forward declarations */
class ISCTRawDataProviderTool;
class ISCT_CablingSvc;
class SCT_ID;

class SCTRawDataProvider : public AthAlgorithm
{
 public:

  //! Constructor.
  SCTRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  //! Initialize
  virtual StatusCode initialize();

  //! Execute
  virtual StatusCode execute();

  //! Finalize
  virtual StatusCode finalize()
  { return StatusCode::SUCCESS;}

  //! Destructur
  ~SCTRawDataProvider() 
    { }

 private:

  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<ISCTRawDataProviderTool> m_rawDataTool;
  ServiceHandle<ISCT_CablingSvc>     m_cabling;
  const SCT_ID*                      m_sct_id; 
  SG::WriteHandleKey<SCT_RDO_Container> m_rdoContainerKey;
  SG::WriteHandleKey<InDetTimeCollection> m_lvl1CollectionKey;
  SG::WriteHandleKey<InDetTimeCollection> m_bcidCollectionKey;

};

#endif
