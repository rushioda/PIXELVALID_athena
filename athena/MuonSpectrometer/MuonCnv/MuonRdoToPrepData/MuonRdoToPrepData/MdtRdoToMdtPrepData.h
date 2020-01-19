/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTRDOTOMDTPREPDATA_H
#define MDTRDOTOMDTPREPDATA_H 

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

class MdtIdHelper;
class MdtDigit;
class MdtCalibrationSvc;
class MDTcablingSvc;

////////////////////////////////////////////////////////////////////////////////////////
/// Author: Davide Costanzo
/// BNL, April 04, 2005
///
/// algorithm to decode RDO into MdtPrepData
/// get the RDO container from Storegate
/// loop over the RDO
/// Decode RDO into PrepRawData
/// loop over the PrepRawData and build the PrepRawData container
/// store the PrepRawData container in StoreGate
////////////////////////////////////////////////////////////////////////////////////////

class MdtRdoToMdtPrepData : public AthAlgorithm {

 public:

  MdtRdoToMdtPrepData(const std::string& name, ISvcLocator* pSvcLocator);
    
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

    // EJWM - where is this implemented? Removing to avoid missing symbol
    //void printRpcPrepRawData(); //!< Prints information about the resultant PRDs.

    ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_tool; //!< Tool used to do actual decoding.

    bool                                    m_print_inputRdo; //!<< If true, will dump information about the input RDOs.
    bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.
};

#endif /// MDTRDOTOMDTPREPDATA_H


