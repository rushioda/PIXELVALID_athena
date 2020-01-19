/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTDIGITTOMDTRDO_H
#define MDTDIGITTOMDTRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

class MuonMDT_CablingSvc;
class MdtCsmContainer;
class MdtIdHelper;

/////////////////////////////////////////////////////////////////////////////

class MdtDigitToMdtRDO : public AthAlgorithm {

 public:

  MdtDigitToMdtRDO (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  StatusCode fill_MDTdata() const;
  StatusCode fillTagInfo() const;

 protected:

  ServiceHandle<ActiveStoreSvc> m_activeStore;
  ServiceHandle<MuonMDT_CablingSvc>       m_cabling;
  MdtCsmContainer *    m_csmContainer;
  const MdtIdHelper*   m_mdtIdHelper;
  bool m_BMEpresent;
};

#endif
