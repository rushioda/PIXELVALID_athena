/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArFillDSPConfig.h"

#include "LArCOOLConditions/LArDSPConfig.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
LArFillDSPConfig::LArFillDSPConfig( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_onlineID(0),
  m_cablingSvc("LArCablingService")
{
  declareProperty("Foldername",m_folderName="/LAR/Configuraton/DSPConfiguration");
  declareProperty("Dump",m_dump=true);
  declareProperty("isLowMu",m_lowmu=false);
}

// Destructor
///////////////
LArFillDSPConfig::~LArFillDSPConfig() {}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode LArFillDSPConfig::initialize()
{
  CHECK(m_cablingSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode LArFillDSPConfig::stop() {  


  StatusCode sc=detStore()->retrieve(m_onlineID);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to get LArOnlineID" );
    return sc;
  }

  const unsigned nFebs=m_onlineID->febHashMax();
  

  LArDSPConfigWrite larDSPConfig;
  
  for (unsigned iFeb=0;iFeb<nFebs;++iFeb) {
    const HWIdentifier febId=m_onlineID->feb_Id(iFeb);
    //bool useMGRampIntercept=(m_onlineID->isEMBchannel(febId) || m_onlineID->isEMECchannel(febId));
    bool useMGRampIntercept=(m_onlineID->isEMBchannel(febId) || m_onlineID->isEMECOW(febId) );
    if(m_lowmu) {
       if(m_onlineID->isFCALchannel(febId)) useMGRampIntercept=true;
    } 
    uint8_t peakSample=2;
    if (m_onlineID->isHECchannel(febId)) peakSample=1; //HEC: peak-sample=1
    larDSPConfig.set(iFeb,peakSample,useMGRampIntercept);
  }

  
  std::unique_ptr<AthenaAttributeList> pAttrList(larDSPConfig.attributeList());
  ATH_CHECK(detStore()->record(std::move(pAttrList),m_folderName));
  ATH_MSG_INFO( "Successfully recorded AthenaAttributeList containing DSP configuration for " << nFebs << " Febs" );

  if (m_dump) {
    //Crosscheck:
    const AthenaAttributeList*  attrList_read=nullptr;
    ATH_CHECK(detStore()->retrieve(attrList_read,m_folderName));
  
    LArDSPConfig config(attrList_read);
  
    for (auto fit= m_onlineID->feb_begin();fit!=m_onlineID->feb_end();++fit) {
      unsigned ps=config.peakSample(*fit);
      bool ic=config.useMGRampIntercept(*fit);
      std::cout << "FEB " << m_onlineID->channel_name(*fit) << ": peakSample=" << ps << " useMGIntercept=" << ic << std::endl;
    }
  }
  return StatusCode::SUCCESS; 
}




