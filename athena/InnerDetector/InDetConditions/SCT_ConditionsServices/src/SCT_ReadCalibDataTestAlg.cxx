/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataTestAlg.cxx Implementation file for SCT_ReadCalibDataTestAlg class
    @author: Per Johansson, based on Jorgen Dalmau TestReadSCT_CalibData example
    adapted first to test a tool and the a service
*/

// Include SCT_ReadCalibDataTestAlg and Svc
#include "SCT_ReadCalibDataTestAlg.h"
#include "SCT_ConditionsServices/ISCT_ReadCalibDataSvc.h"

// Include Event Info 
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Include Athena stuff
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "InDetIdentifier/SCT_ID.h"

// Include Gaudi stuff
#include "GaudiKernel/StatusCode.h"

// Include STL stuff
#include <vector>
#include <string>
using namespace std;

//----------------------------------------------------------------------
SCT_ReadCalibDataTestAlg::SCT_ReadCalibDataTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_sc(0),
  m_id_sct(0),
  m_currentEvent(0),
  m_moduleId(0),
  m_waferId(0),
  m_stripId(0),
  m_ReadCalibDataSvc("SCT_ReadCalibDataSvc",name),
  m_cabling("SCT_CablingSvc",name),
  m_doTestmyConditionsSummary(false),
  m_doTestmyDefectIsGood(false),
  m_doTestmyDefectType(false),
  m_doTestmyDefectsSummary(false),
  m_doTestmyDefectList(false),
  m_moduleOfflinePosition()
{
  declareProperty("SCT_ReadCalibDataSvc", m_ReadCalibDataSvc);
  declareProperty("DoTestmyConditionsSummary",   m_doTestmyConditionsSummary   = false, "Test return bool conditions summary?");
  declareProperty("DoTestmyDefectIsGood",        m_doTestmyDefectIsGood        = false, "Test return defect type summary?");
  declareProperty("DoTestmyDefectType",          m_doTestmyDefectType          = false, "Test return defect type summary?");
  declareProperty("DoTestmyDefectsSummary",      m_doTestmyDefectsSummary      = false, "Test return module defects summary?");
  declareProperty("DoTestmyDefectList",          m_doTestmyDefectList          = false, "Test return defectList?");
  declareProperty("ModuleOfflinePosition",       m_moduleOfflinePosition              , "Offline pos. as: B-EC,layer-disk,phi,eta");
}

//----------------------------------------------------------------------
SCT_ReadCalibDataTestAlg::~SCT_ReadCalibDataTestAlg()
{ }

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataTestAlg::initialize()
{
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in initialize()" << endmsg;
  
  // Get SCT ID helper
  m_sc = detStore()->retrieve(m_id_sct, "SCT_ID");
  if (m_sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to get SCT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found SCT detector manager" << endmsg;
  }
  
  // Process jobOption properties
  m_sc = processProperties();
  if (m_sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to process jobOpt properties" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Processed jobOpt properties" << endmsg;
  }

  // Get the SCT_ReadCaliDataSvc
  m_sc = m_ReadCalibDataSvc.retrieve();
  if (m_sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot locate CalibData service" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CalibData Service located " << endmsg;
  }

  // Retrieve SCT Cabling service
  m_sc = m_cabling.retrieve();
  if (m_sc.isFailure()) {  
    msg(MSG:: ERROR)<< "Failed to retrieve SCT cabling service" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataTestAlg::initialize()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataTestAlg::processProperties()
{
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in processProperties()" << endmsg;
  
  // Get module position from jobOpt property
  std::vector<int>::const_iterator itLoop = m_moduleOfflinePosition.value().begin();
  int offlineBarrelEC  = (*itLoop); ++itLoop;
  int offlineLayerDisk = (*itLoop); ++itLoop;
  int offlineEta       = (*itLoop); ++itLoop;
  int offlinePhi       = (*itLoop); ++itLoop;
  int offlineSide      = (*itLoop); ++itLoop;
  int offlineStrip     = (*itLoop); ++itLoop;
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Module positions from jobOpt property:" << endmsg;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "B-EC/layer-disk/eta/phi: "
      << offlineBarrelEC  << "/"
      << offlineLayerDisk << "/"
      << offlineEta       << "/"
      << offlinePhi       << "/"
      << offlineSide      << "/"
      << offlineStrip     << endmsg;
  
  // Create offline Identifier for this module position, wafer,chip and strip
  m_moduleId = m_id_sct->module_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta);
  m_waferId = m_id_sct->wafer_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta, offlineSide);
  m_stripId = m_id_sct->strip_id(offlineBarrelEC, offlineLayerDisk, offlinePhi, offlineEta, offlineSide, offlineStrip);

  // Debug output
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "id-getString : " << m_moduleId.getString() << endmsg;   // hex format
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "id-getCompact: " << m_moduleId.get_compact() << endmsg; // dec format
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "id-getCompact2: " << m_stripId.get_compact() << endmsg; // dec format
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Module Id: " << m_id_sct->print_to_string(m_moduleId) << endmsg;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Strip Id: " << m_id_sct->print_to_string(m_stripId) << endmsg;
  
  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataTestAlg::processProperties()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataTestAlg::execute()
{
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in execute()" << endmsg;
  
  // Get the current event
  m_sc = evtStore()->retrieve(m_currentEvent);
  if ( m_sc.isFailure() ) {
    msg(MSG::ERROR) << "Could not get event info" << endmsg;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Current Run.Event,Time: "
      << "[" << m_currentEvent->event_ID()->run_number()
      << "." << m_currentEvent->event_ID()->event_number()
      << "," << m_currentEvent->event_ID()->time_stamp()
      << "]" << endmsg;
  
  //Make sure data was filled
  bool CalibDataFilled = m_ReadCalibDataSvc->filled();  
  if( CalibDataFilled ){
    
    //Test ConditionsSummary
    if (m_doTestmyConditionsSummary) {
      // Test summmary, ask status of strip in module
      Identifier IdM = m_moduleId;
      Identifier IdS = m_stripId;
      bool Sok = m_ReadCalibDataSvc->isGood(IdS, InDetConditions::SCT_STRIP);
      msg(MSG::INFO) << "Strip " << IdS << " on module " << IdM << " is " << (Sok?"good":"bad") << endmsg;
    }
  }

  // Loop over all strips and check if good or not using isGood, and print the bad ones
  if (m_doTestmyDefectIsGood){
    int ngood=0;
    int nbad=0;
    //Loop over all wafers using hashIds from the cabling service
    std::vector<boost::uint32_t> listOfRODs;
    m_cabling->getAllRods(listOfRODs);
    //for ( unsigned int it(0); it<100;it++){ // For CPU time gain study
    std::vector<boost::uint32_t>::iterator rodIter = listOfRODs.begin();
    std::vector<boost::uint32_t>::iterator rodEnd = listOfRODs.end();
    for (; rodIter != rodEnd; ++rodIter) {
      std::vector<IdentifierHash> listOfHashes;
      m_cabling->getHashesForRod(listOfHashes,*rodIter);
      std::vector<IdentifierHash>::iterator hashIt = listOfHashes.begin();
      std::vector<IdentifierHash>::iterator hashEnd = listOfHashes.end();
      for (; hashIt != hashEnd; ++hashIt) {
        Identifier waferId = m_id_sct->wafer_id(*hashIt); 
        //loop over all strips and check if good or not
        for ( unsigned int stripIndex=0; stripIndex!=768; ++stripIndex){
          Identifier IdS = m_id_sct->strip_id(waferId,stripIndex);
          const unsigned int stripId = m_id_sct->strip(IdS);
          const int side = m_id_sct->side(IdS);
          const bool stripOk = m_ReadCalibDataSvc->isGood(IdS, InDetConditions::SCT_STRIP);
          if ( stripOk) ++ngood;
           else ++nbad; 
          if ( !stripOk ){ // Print info on all bad strips
            std::cout << "ModuleId/side/strip: " << m_id_sct->module_id(waferId) << "/" << side << "/" << stripId<< " is bad" << std::endl;
          }
        }
      }
      //}
    }
    std::cout << "Number of good strips: " << ngood << " number of bad strips: " << nbad << std::endl;
  }
               
  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataTestAlg::execute()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataTestAlg::finalize()
{
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in finalize()" << endmsg;
  
  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataTestAlg::finalize()

//----------------------------------------------------------------------

