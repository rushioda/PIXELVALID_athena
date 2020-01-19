/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataSvc.cxx Implementation file for SCT_ReadCalibDataSvc.
@author Per Johansson, 17/05/08, based on SCT_ReadCalibDataTool.
*/

// Include SCT_ReadCalibDataSvc.h
#include "SCT_ReadCalibDataSvc.h"

// Include Event Info 
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "CoralBase/TimeStamp.h"


// Include Athena stuff
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"

// Include Gaudi stuff
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/StatusCode.h"

// Include STL stuff
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <limits>
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"

// Utility functions
namespace {
	template <typename C> 
	bool fillFromString(const std::string& source, C & userContainer){
	  typedef typename C::value_type V_t;
	  V_t errVal(0);
	  boost::char_separator<char> sep(" ");
	  typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
	  Tokenizer tok(source,sep);
	  bool noNan(true);
	  for(Tokenizer::iterator i(tok.begin());i != tok.end();++i){
		try{
		  userContainer.push_back(boost::lexical_cast<V_t>(*i));
		} catch(boost::bad_lexical_cast) {
		  userContainer.push_back(errVal);
		  noNan=false;
		}
	  }
	  return noNan;
	}
	float 
	coerceToFloatRange(const double value){
	 const float maxfloat(std::numeric_limits<float>::max());
	 if (value>maxfloat) return maxfloat;
	 else return float(value);
	} 
}

using namespace std;

//----------------------------------------------------------------------
SCT_ReadCalibDataSvc::SCT_ReadCalibDataSvc (const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_storeGateSvc("StoreGateSvc", name),
  m_detStoreSvc("DetectorStore", name),
  m_IOVDbSvc("IOVDbSvc", name),
  m_cabling("SCT_CablingSvc",name),
  m_SCTdetMgr(0),
  m_attrListColl(0),
  m_id_sct(0),
  m_dataFilled(false),
  m_NPGDefects(0),
  m_NODefects(0),
  m_printCalibDefectMaps(false),
  m_recoOnly(true),
  m_ignoreDefects(0),
  m_ignoreDefectParameters(0)
{
  declareProperty("PrintCalibDefectMaps",  m_printCalibDefectMaps  = false, "Print data read from the Calib Defect map?");
  declareProperty("AttrListCollFolders",   m_atrcollist, "List of calibration data folder?"); 
  declareProperty("RecoOnly",              m_recoOnly, "Use new improved isGood method, all other methods defunct"); 
  std::vector<std::string> names(2);
  names[0] = std::string("/SCT/DAQ/Calibration/NPtGainDefects");
  names[1] = std::string("/SCT/DAQ/Calibration/NoiseOccupancyDefects");
  m_atrcollist.setValue(names);
  //
  WaferIsGoodInfo_t OneWaferIsGoodInfo;
  OneWaferIsGoodInfo.assign(0);
  m_isGoodAllWafersInfo.assign(OneWaferIsGoodInfo);
  //
  //  m_ignoreDefects.push_back("BADFIT");
  m_ignoreDefects.push_back("NOISE_SLOPE");
  m_ignoreDefects.push_back("OFFSET_SLOPE");
  m_ignoreDefects.push_back("GAIN_SLOPE");
  m_ignoreDefects.push_back("BAD_OPE");
  //  m_ignoreDefectParameters.push_back(-1000.);
  m_ignoreDefectParameters.push_back(-1000.);
  m_ignoreDefectParameters.push_back(-1000.);
  m_ignoreDefectParameters.push_back(-1000.);
  m_ignoreDefectParameters.push_back(-1000.);
  //
  declareProperty("IgnoreDefects", m_ignoreDefects, "Defects to ignore");
  declareProperty("IgnoreDefectsParameters", m_ignoreDefectParameters, "Limit on defect to ignore parameters");
  //
}

//----------------------------------------------------------------------
SCT_ReadCalibDataSvc::~SCT_ReadCalibDataSvc(){ 
 
  }

//----------------------------------------------------------------------

StatusCode SCT_ReadCalibDataSvc::initialize(){
  StatusCode sc;
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in initialize()" << endmsg;
  // Get SCT detector manager
  sc = m_detStoreSvc->retrieve(m_SCTdetMgr, "SCT");
  if (sc.isFailure()) {
    msg(MSG:: FATAL) << "Failed to get SCT detector manager" << endmsg;
    return StatusCode::FAILURE;
  }else {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Found SCT detector manager" << endmsg;
  }

  // Get SCT helper
  sc = m_detStoreSvc->retrieve(m_id_sct, "SCT_ID");
  if (sc.isFailure()) {
    msg(MSG:: FATAL) << "Failed to get SCT helper" << endmsg;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Found SCT helper" << endmsg;
  }

  // Retrieve IOVDb service
  sc = m_IOVDbSvc.retrieve();
  if (sc.isFailure()) {  
    msg(MSG:: ERROR)<< "Failed to retrieve IOVDbSvc " << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve SCT Cabling service
  sc = m_cabling.retrieve();
  if (sc.isFailure()) {  
    msg(MSG:: ERROR)<< "Failed to retrieve SCT cabling service" << endmsg;
    return StatusCode::FAILURE;
  }

  //Register callbacks for CalibData folders using a vector of keys defined in jobOpt
  std::vector<std::string>::const_iterator itr(m_atrcollist.value().begin());
  std::vector<std::string>::const_iterator end(m_atrcollist.value().end()); 
  for (;itr!=end;++itr) {
    m_key = *itr;
    if ( m_key == "/SCT/DAQ/Calibration/NPtGainDefects" ){
      if ( StatusCode::SUCCESS==m_detStoreSvc->regFcn(&SCT_ReadCalibDataSvc::fillData,this,m_CalibDefects_NP,m_key) ){
        msg(MSG:: INFO) << "Registered callback for key: " << m_key << endmsg;
      } else {
        msg(MSG:: ERROR) << "Cannot register callbacks function for key " << m_key << endmsg;
      }
    }
    else if ( m_key == "/SCT/DAQ/Calibration/NoiseOccupancyDefects" ){
      if ( StatusCode::SUCCESS==m_detStoreSvc->regFcn(&SCT_ReadCalibDataSvc::fillData,this,m_CalibDefects_NO,m_key) ){
        msg(MSG:: INFO) << "Registered callback for key: " << m_key << endmsg;
      } else {
        msg(MSG:: ERROR) << "Cannot register callbacks function for key " << m_key << endmsg;
      }
    } else {
      msg(MSG:: ERROR) << "Cannot register callbacks function for key " << m_key << endmsg;
    }
  }
  
  // Create Calib Defect and Data maps 
  m_NPGDefects = new SCT_CalibDefectData;
  m_NODefects = new SCT_CalibDefectData;
   
  // Initialize arrays and all strips to True
  for (int w(0); w!=NUMBER_OF_WAFERS;++w){
    for (int s(0);s!=STRIPS_PER_WAFER;++s){
      m_isGoodAllWafersInfo[w][s]=true;
    }
  }
  
  //Check ignoreDefects vectors are the same size
  if ( m_ignoreDefects.size() != m_ignoreDefectParameters.size() ){
    msg(MSG:: ERROR) << "IgnoreDefect != IgnoreDefectsParameters, check job options!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataSvc::initialize()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataSvc::finalize(){
  //delete new'ed objects (bug http://savannah.cern.ch/bugs/?47484 )
  delete m_NPGDefects;
  delete m_NODefects;
  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataSvc::finalize()

//----------------------------------------------------------------------
// Query the interfaces.
StatusCode SCT_ReadCalibDataSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if ( ISCT_ReadCalibDataSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface =  dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
//Can only report good/bad at strip level
bool SCT_ReadCalibDataSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h==InDetConditions::SCT_STRIP);
}


//----------------------------------------------------------------------
// Returns ok if fillData worked properly
bool SCT_ReadCalibDataSvc::filled() const {
  return m_dataFilled;
} //SCT_ReadCalibDataSvc::filled()


//----------------------------------------------------------------------
// Fill the data structures from a callback
StatusCode SCT_ReadCalibDataSvc::fillData(int& /*i*/ , std::list<std::string>& l){
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "fillData has been triggered by: ";
  std::list<std::string>::const_iterator itr;
  for (itr=l.begin(); itr!=l.end(); ++itr) {
    msg() << *itr << " ";
  }
  msg() << endmsg;

  StatusCode sc0= SCT_ReadCalibDataSvc::fillCalibDefectData(l);
  // No longer need the conditions folder as stored locally
  for (itr=l.begin(); itr!=l.end(); ++itr) {
    m_IOVDbSvc->dropObject(*itr,true); 
  }

  if ( sc0==StatusCode::SUCCESS ){
    m_dataFilled = true;
    msg(MSG:: INFO) << "Calib Defect array/maps filled ok" << endmsg;
    // Print the defect maps
    if ( m_printCalibDefectMaps and !m_recoOnly ){
      std::string NPGCalibDefectMap;
      std::string NOCalibDefectMap;
      NPGCalibDefectMap = m_NPGDefects -> str();
      NOCalibDefectMap = m_NODefects -> str();
      msg(MSG::DEBUG) << "\n" << NPGCalibDefectMap << endmsg;
      msg(MSG::DEBUG) << NOCalibDefectMap << endmsg;
    }
    return StatusCode::SUCCESS;
  } else {
    m_dataFilled = false;
    msg(MSG:: ERROR) << "fillData failed" << endmsg;
    return StatusCode::FAILURE;
  }
} //SCT_ReadCalibDataSvc::fillData()

//----------------------------------------------------------------------
// Callback for Calib defect data
StatusCode SCT_ReadCalibDataSvc::fillCalibDefectData(std::list<std::string>& keys){
  // Defect type map, contains Fit, NPtGain and No defects for now
  static std::map<int, std::string> s_defectMapIntToString;    
  // Fit Defects
  s_defectMapIntToString[0]  = "UNKNOWN";       //<! Defect type not in this map, add!  
  s_defectMapIntToString[1]  = "DEAD";          //<! Output always < 1%
  s_defectMapIntToString[2]  = "STUCKON";       //<! Output always > 98%
  s_defectMapIntToString[3]  = "UNDER";         //<! Occupancy never reaches max, always less than 95%
  s_defectMapIntToString[4]  = "OVER";          //<! Occcupancy greater than 100%
  s_defectMapIntToString[5]  = "BADFIT";        //<! The fit was not good for some reason - parameter is a chi2 cut
  // NPt Gain Defects
  s_defectMapIntToString[32] = "VLO_GAIN";      //<! Gain < 0.3 * chip average
  s_defectMapIntToString[9]  = "LO_GAIN";       //<! Gain < 0.75 * chip average
  s_defectMapIntToString[10] = "HI_GAIN";       //<! Gain > 1.25 * chip average
  s_defectMapIntToString[11] = "LO_OFFSET";     //<! Offset < -100
  s_defectMapIntToString[12] = "HI_OFFSET";     //<! Offset > 200
  s_defectMapIntToString[13] = "UNBONDED";      //<! Noise <= 750
  s_defectMapIntToString[14] = "PARTBONDED";    //<! Noise <= 1100
  s_defectMapIntToString[15] = "NOISY";         //<! Noise > 1.15* av chip noise
  s_defectMapIntToString[33] = "V_NOISY";       //<! Noise > 1.25* av chip noise
  s_defectMapIntToString[34] = "NOISE_SLOPE";   //<! Slope in noise across module, slope/chan > 1.
  s_defectMapIntToString[35] = "OFFSET_SLOPE";  //<! Slope in offset across module, slope/chan > 0.07
  s_defectMapIntToString[36] = "GAIN_SLOPE";    //<! Slope in gain across module, slope/chan > 0.04
  // Noise Occupancy Defects
  s_defectMapIntToString[19] = "NO_HI";         //<! High noise occupancy, 0.0005
  s_defectMapIntToString[37] = "BAD_OPE";       //<! Bad occupancy per event variance/binomial variance > 2.0)
  s_defectMapIntToString[38] = "DOUBTR_HI";     //<! High double trigger noise occupancy, > 5

  // Get the current event and print info
  const EventInfo* event;
  if (m_storeGateSvc->retrieve(event)==StatusCode::SUCCESS) {
    coral::TimeStamp::ValueType nsTime=event->event_ID()->time_stamp()*1000000000LL;
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In run/event [" << event->event_ID()->run_number() <<
      "," << event->event_ID()->event_number() << "] timestamp " << nsTime << 
      endmsg;
    // print the timestamp in UTC and local
    // coral::TimeStamp utctime(nsTime);
    // coral::TimeStamp localtime(utctime.time(), true); // get ptime and convert to local
    // if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Timestamp UTC: " << utctime.toString() << 
    //  " local: " << localtime.toString() << endmsg;
  } else {
    msg(MSG:: ERROR) << "Could not get pointer to event" << endmsg;
  }

  if (!m_recoOnly){
    // Check if maps empty, and if not clear them
    if( !m_NPGDefects -> empty() ) m_NPGDefects -> clear();
    if( !m_NODefects -> empty() ) m_NODefects -> clear();
  }
  
  // Create pointer to CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects theseDefects;
  
  // Retrieve CondAttrListCollection
  const CondAttrListCollection* atrlistcol;
  std::list<std::string>::const_iterator itr_key;
  for (itr_key=keys.begin(); itr_key!=keys.end(); ++itr_key) {
    std::string folder = *itr_key;
    if (m_detStoreSvc->retrieve(atrlistcol,*itr_key)==StatusCode::SUCCESS) {
      // loop over collection
      CondAttrListCollection::const_iterator itLoop=atrlistcol->begin();
      CondAttrListCollection::const_iterator itLoop_end=atrlistcol->end();
      for (; itLoop!=itLoop_end; ++itLoop) {
        CondAttrListCollection::ChanNum chanNum = (*itLoop).first;
        coral::AttributeList anAttrList = (*itLoop).second;
  
        // Convert chanNum=offlineID into identifier
        Identifier moduleId(chanNum);
        IdentifierHash hashId0 = m_id_sct->wafer_hash(moduleId);
        IdentifierHash hashId1;
        m_id_sct->get_other_side(hashId0, hashId1);

        // Check for PhiSwap readout
        bool phiSwap0Present=false;
        bool phiSwap1Present=false;
        InDetDD::SiDetectorElement * p_element = (m_SCTdetMgr->getDetectorElement(hashId0));
        if (p_element->swapPhiReadoutDirection()) { phiSwap0Present = true; }
        p_element = (m_SCTdetMgr->getDetectorElement(hashId1));
        if (p_element->swapPhiReadoutDirection()) { phiSwap1Present = true; }
       
  // Clear theseDefects
        theseDefects.begDefects.clear();
        theseDefects.endDefects.clear();
        theseDefects.typeOfDefect.clear();
        theseDefects.parValue.clear();
  
  // Get all defect parameters from COOL attrib list
        std::string Gaindefectb = ((anAttrList)["defectBeginChannel"]).data<std::string>();
        std::string Gaindefecte = ((anAttrList)["defectEndChannel"]).data<std::string>();
        std::string DefectType = ((anAttrList)["defectType"]).data<std::string>();
        std::string parValue = ((anAttrList)["defectParameter"]).data<std::string>();
  
  // Convert the defect strings to vectors
        vector<unsigned int> Gaindefectbvec; fillFromString(Gaindefectb, Gaindefectbvec);
        vector<unsigned int> Gaindefectevec; fillFromString(Gaindefecte, Gaindefectevec);
        vector<unsigned int> DefectTypevec;  fillFromString(DefectType, DefectTypevec);
        vector<double> parValuevec; fillFromString(parValue, parValuevec);

  // Fill the Calib defect objects    
        unsigned Gainvec_size = Gaindefectbvec.size();
        for ( unsigned int i = 0; i<Gainvec_size; ++i ){
          theseDefects.begDefects.push_back(Gaindefectbvec[i]);
          theseDefects.endDefects.push_back(Gaindefectevec[i]);
          theseDefects.typeOfDefect.push_back(s_defectMapIntToString[DefectTypevec[i]]);
          theseDefects.parValue.push_back(coerceToFloatRange(parValuevec[i]));
        }
        // Fill the isGoodWaferArray
        if ( !theseDefects.begDefects.empty() ){
          for ( unsigned int i = 0; i < theseDefects.begDefects.size(); ++i ){ // loop over all defects
          // Check for defects and their limits not to take into account in isGood 
            bool ignoreDefect = false; 
            unsigned int ig=0;
            //std::string defectType = theseDefects.typeOfDefect[i];
            while ( ig<m_ignoreDefects.size() ){ //loop until found defect or end of ignoredefects
              if ( m_ignoreDefects[ig] == theseDefects.typeOfDefect[i] ){
                if ( m_ignoreDefectParameters[ig] < -999 ) //no check on parameter value, defect ignored
                  ignoreDefect = true;
                else if ( theseDefects.typeOfDefect[i] == "NO_HI" and m_ignoreDefectParameters[ig] > theseDefects.parValue[i] ) ignoreDefect = true; //noise below threshold, > 0.0005 (in DB, so default values printed here)
                else if ( theseDefects.typeOfDefect[i] == "NOISY" and m_ignoreDefectParameters[ig] > theseDefects.parValue[i] ) ignoreDefect = true; //noise below threshold, > 1.15* av chip average
                else if ( theseDefects.typeOfDefect[i] == "V_NOISY" and m_ignoreDefectParameters[ig] > theseDefects.parValue[i] ) ignoreDefect = true; //noise below threshold, > 1.25* av chip average
                else if ( theseDefects.typeOfDefect[i] == "VLO_GAIN" and m_ignoreDefectParameters[ig] < theseDefects.parValue[i] ) ignoreDefect = true; // gain to low, < 0.3 * chip average
                else if ( theseDefects.typeOfDefect[i] == "LO_GAIN" and m_ignoreDefectParameters[ig] < theseDefects.parValue[i] ) ignoreDefect = true; // gain to low < 0.75 * chip average
                else if ( theseDefects.typeOfDefect[i] == "HI_GAIN" and m_ignoreDefectParameters[ig] > theseDefects.parValue[i] ) ignoreDefect = true; // gain to high > 1.25 * chip average
                else if ( theseDefects.typeOfDefect[i] == "LO_OFFSET" and m_ignoreDefectParameters[ig] > theseDefects.parValue[i] ) ignoreDefect = true; // offset to low < -100
                else if ( theseDefects.typeOfDefect[i] == "HI_OFFSET" and m_ignoreDefectParameters[ig] < theseDefects.parValue[i] ) ignoreDefect = true; // offset to high > 200
              }
              ig++;
            }
            if (!ignoreDefect){
            //set the isGoodBool value for all strips for this defect
              for ( unsigned int strip = theseDefects.begDefects[i]; strip <= theseDefects.endDefects[i]; ++strip ){ 
              // Check for phiSwap and which wafer side before filling isGood vector
                if ( strip < 768 ){ //side 0 0->767
                  const unsigned int waferId0=hashId0;
                  WaferIsGoodInfo_t & thisWaferIsGoodData0 = m_isGoodAllWafersInfo[waferId0];
                  const unsigned int side0StripNumber = phiSwap0Present?(767-strip):strip;
                  thisWaferIsGoodData0[side0StripNumber] = false;
                } else {               // side 1 768->1535 => 0->767
                  const unsigned int waferId1=hashId1;
                  WaferIsGoodInfo_t & thisWaferIsGoodData1 = m_isGoodAllWafersInfo[waferId1];
                  const unsigned int side1StripNumber = phiSwap1Present?(1535-strip):(strip-768);
                  thisWaferIsGoodData1[side1StripNumber] = false;
                }
              }
            }
          }
        }

        if ( !m_recoOnly ){
        // Check what folder we are in
          size_t folderfoundNP = folder.find("NPtGain");
          size_t folderfoundNO = folder.find("NoiseOccupancy");

          // Fill the CalibDefectData maps with the Calib defect objects
          if ( folderfoundNP!=string::npos ){ 
            if ( theseDefects.begDefects.empty() ){
              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No NPtGain defects for module " << moduleId << endmsg;  
              continue;
            }
            if (!(m_NPGDefects -> addModule( moduleId, theseDefects ))) {
              msg(MSG:: ERROR) << "Unable to add module " << moduleId << " to NPtGain defect map" << endmsg;
              return StatusCode::FAILURE;
            } else {
              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Defects for module " << moduleId << " added to NPG defect map" << endmsg;
            }
          }else if ( folderfoundNO!=string::npos ){  
            if ( theseDefects.begDefects.empty() ){
              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No NoiseOccupancy defects for module " << moduleId << endmsg; 
              continue;
            }
            if (!(m_NODefects -> addModule( moduleId, theseDefects ))) {
              msg(MSG:: ERROR) << "Unable to add module " << moduleId << " to NoiseOccupancy defect map" << endmsg;
              return StatusCode::FAILURE;
            } else {
              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Defects for module " << moduleId << " added to NoiseOccupancy defect map" << endmsg;
            } 
          }
        }
      }
    } else {
      msg(MSG:: ERROR) << "Could not retrieve CondAttrListCollection " << *itr_key << endmsg;
    }  
  }

  if (!m_recoOnly){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There are " << m_NPGDefects->size() << " elements in the NPtGain module defect map" << endmsg;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There are " << m_NODefects->size() << " elements in the NoiseOccupancy module defect map" << endmsg;
  }
 
  return StatusCode::SUCCESS;
} //SCT_ReadCalibDataSvc::fillCalibDefectData()

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool 
SCT_ReadCalibDataSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h){
  // Status of the compId
  bool status(true);
  // Extract the moduleId from the comp identifier
  Identifier moduleId = m_id_sct->module_id(elementId);
  switch (h){
    case InDetConditions::SCT_MODULE:
    {
      // Not applicable for Calibration data
      msg(MSG:: WARNING) << "summary(): Module good/bad is not applicable for Calibration data" << endmsg;
      break;
    }

    case InDetConditions::SCT_SIDE:
    {
      // Not applicable for Calibration data
      msg(MSG:: WARNING) << "summary(): Wafer good/bad is not applicable for Calibration data" << endmsg;
      break;
    }

    case InDetConditions::SCT_CHIP:
    {
      // Not applicable for Calibration data
      msg(MSG:: WARNING) << "summary(): Chip good/bad is not applicable for Calibration data" << endmsg;
      break;
    }
    case InDetConditions::SCT_STRIP:
    {
      // Get hashId
      IdentifierHash hashIdx = m_id_sct->wafer_hash(moduleId);
      int side = m_id_sct->side(elementId);
      if ( side==1 ) m_id_sct->get_other_side(hashIdx, hashIdx);
      unsigned int waferIdx=hashIdx;
      //unsigned int waferIdx=hashIdx+side; //uhm
      // Get strip on wafer to check
      int strip = m_id_sct->strip(elementId);
      // Retrieve isGood Wafer data
      WaferIsGoodInfo_t & wantedIsGoodWafer = m_isGoodAllWafersInfo[waferIdx];
      // Set value
      status = wantedIsGoodWafer[strip];
      break;
    }
    default:
    {
      status = true ;
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Unknown component has been asked for, should be Module/Wafer/Chip or Strip; returning 'good' and continuing" << endmsg ; 
    }    
  }//end of switch structure
  
  // Print status  
  return status ;
} //SCT_ReadCalibDataSvc::summary()


//----------------------------------------------------------------------
// Returns a defect summary of a defect strip, scan, type and value
SCT_ReadCalibDataSvc::calibDefectType SCT_ReadCalibDataSvc::defectType(const Identifier & stripId, InDetConditions::Hierarchy h){
  // Print where you are
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in defectType()" << endmsg;

  // Extract the moduleId from the comp identifier
  Identifier moduleId = m_id_sct->module_id(stripId);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Summary wanted for component: " << stripId << " on module: " << moduleId << endmsg;

  // Create the CalibDataDefect objects
  SCT_CalibDefectData::CalibModuleDefects wantedNPGDefects;
  SCT_CalibDefectData::CalibModuleDefects wantedNODefects;

  // Create the calibDefectSummary
  calibDefectType theseSummaryDefects;
  // Retrieve defect data from map
  wantedNPGDefects = m_NPGDefects->findModule(moduleId);
  wantedNODefects = m_NODefects->findModule(moduleId);

  switch (h){
    case InDetConditions::SCT_MODULE:
    {
      // Not applicable for Calibration data
      msg(MSG:: WARNING) << "summary(): Module defect summary is not applicable for Calibration data" << endmsg;
      break;
    }

    case InDetConditions::SCT_SIDE:
    {
      // Not applicable for Calibration data
      msg(MSG:: WARNING) << "summary(): Wafer defect summary is not applicable for Calibration data" << endmsg;
      break;
    }

    case InDetConditions::SCT_CHIP:
    {
      // Not applicable for Calibration data
      msg(MSG:: WARNING) << "summary(): Chip defect summary is not applicable for Calibration data" << endmsg;
      break;
    }
    case InDetConditions::SCT_STRIP:
    {
      // Get the strip/channel number to check
      int side = m_id_sct->side(stripId);
      int strip = m_id_sct->strip(stripId);
      unsigned int stripNum;
      const InDetDD::SiDetectorElement * p_element = (m_SCTdetMgr->getDetectorElement(stripId));
      if (p_element->swapPhiReadoutDirection()) {
        if ( side == 0 ){
          stripNum = 767 - strip;
        } else {
          stripNum = 1535 - strip;
        }      
      } else {
        stripNum = side * 768 + strip;
      }
      
      // Find the bad strip and fill calibDefectSummary
      if ( wantedNPGDefects.begDefects.empty() ){
        msg(MSG::VERBOSE) << "No NPtGain defects in this module" << endmsg;
      }
      else{
        for ( unsigned int i = 0; i < wantedNPGDefects.begDefects.size(); ++i ){
          if ( stripNum >= wantedNPGDefects.begDefects[i] && stripNum <= wantedNPGDefects.endDefects[i] ){
            theseSummaryDefects.scan.push_back("NPtGain");
            theseSummaryDefects.defect.push_back(wantedNPGDefects.typeOfDefect[i]);
            theseSummaryDefects.value.push_back(wantedNPGDefects.parValue[i]);
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "NPtGain defect summary for strip " << stripNum << " filled" << endmsg;
          }
        }
      }

      if ( wantedNODefects.begDefects.empty() ){
        msg(MSG::VERBOSE) << "No NoiseOccupancy defects in this module" << endmsg;
      }else{
        for ( unsigned int i = 0; i != wantedNODefects.begDefects.size(); ++i ){
          if ( stripNum >= wantedNODefects.begDefects[i] && stripNum <= wantedNODefects.endDefects[i] ){
            theseSummaryDefects.scan.push_back("NoiseOccupancy");
            theseSummaryDefects.defect.push_back(wantedNODefects.typeOfDefect[i]);
            theseSummaryDefects.value.push_back(wantedNODefects.parValue[i]);
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "NoiseOccupancy defect summary for strip " << stripNum << "  filled" << endmsg;
          }
        }       
      } 
      if ( theseSummaryDefects.scan.empty() ){
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "defectSummary(): can't retrieve the defects for this strip: " <<  stripNum << " since strip good" << endmsg;
      }     
      break;    
    }
    default:
    {
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Unknown component requested, should be one of Module/Side/Chip or Strip" << endmsg ; 
      return theseSummaryDefects;
    }

  }//end of switch structure

  return theseSummaryDefects;
} //SCT_ReadCalibDataSvc::defectType()

//----------------------------------------------------------------------
// Returns a summary of all defects on a module for a given scan
SCT_CalibDefectData::CalibModuleDefects SCT_ReadCalibDataSvc::defectsSummary(const Identifier & moduleId, const std::string& scan){
  // Create pointer to the CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects wantedDefects;
  // Retrieve the correct defect map
  if ( scan == "NPtGain" ){
    wantedDefects = m_NPGDefects->findModule(moduleId);
  } else if ( scan == "NoiseOccupancy" ){
    wantedDefects = m_NODefects->findModule(moduleId);
  } else{
    msg(MSG:: ERROR) << "defectsSummary(): Module defects for scan" << scan << " does not exist (only NPtGain or NoiseOccupancy)." << endmsg; 
  }

  return wantedDefects;
} //SCT_ReadCalibDataSvc::defectsSummary()

//---------------------------------------------------------------------- 
//----------------------------------------------------------------------
// Returns a list of all strips with a certain defects
std::list<Identifier> SCT_ReadCalibDataSvc::defectList(const std::string& defect){
  std::list<Identifier> defectList;
  // Create pointer to the CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects wantedDefects;
  
  //Check which scan the defect belongs
  bool NPDefect = false;
  bool NODefect = false;
  if ( defect == "NO_HI" || defect == "BAD_OPE" || defect == "DOUBTR_HI" ){
    NODefect = true;
  }else{
    NPDefect = true;
  }
  
  //Loop over all wafers using hashIds from the cabling service
  std::vector<boost::uint32_t> listOfRODs;
  m_cabling->getAllRods(listOfRODs);
  std::vector<boost::uint32_t>::iterator rodIter = listOfRODs.begin();
  std::vector<boost::uint32_t>::iterator rodEnd = listOfRODs.end();
  for (; rodIter != rodEnd; ++rodIter) {
    std::vector<IdentifierHash> listOfHashes;
    m_cabling->getHashesForRod(listOfHashes,*rodIter);
    std::vector<IdentifierHash>::iterator hashIt = listOfHashes.begin();
    std::vector<IdentifierHash>::iterator hashEnd = listOfHashes.end();
    for (; hashIt != hashEnd; ++hashIt) {
      Identifier waferId = m_id_sct->wafer_id(*hashIt); 
      int side = m_id_sct->side(waferId);
      //Only use the hashid for side 0, since data saved per module basis
      if ( side!=0 ) continue;
      Identifier moduleId = m_id_sct->module_id(waferId); 
      if ( NPDefect ){
        wantedDefects = m_NPGDefects->findModule(moduleId);
      }  else if ( NODefect ){
        wantedDefects = m_NODefects->findModule(moduleId);
      }
      if ( !wantedDefects.begDefects.empty() ){
        for ( unsigned int i = 0; i < wantedDefects.begDefects.size(); ++i ){
          if ( wantedDefects.typeOfDefect[i] == defect ){
        // Create identifier for all strips inside begin to end
            int strip_beg = wantedDefects.begDefects[i];
            int strip_end = wantedDefects.endDefects[i];
            // In DB: strip from 0-1535, need to convert to side and 0-767 and take into account phiSwaps
            for ( int strip=strip_beg; strip<strip_end+1; strip++ ){
              int nside = (strip<768) ? 0 : 1; 
              int strip_cor;
              const InDetDD::SiDetectorElement * p_element;
              if (nside == 1){ // if side 1 need waferId of side 1 to get phiswap and correct stripId
                IdentifierHash hashSide1;
                m_id_sct->get_other_side(*hashIt, hashSide1);
                waferId = m_id_sct->wafer_id(hashSide1);
                p_element = (m_SCTdetMgr->getDetectorElement(hashSide1));  
              }else{
                p_element = (m_SCTdetMgr->getDetectorElement(*hashIt));  
              }
              if (p_element->swapPhiReadoutDirection()) {
                if ( nside == 0 ){
                  strip_cor = 767 - strip;
                }else{
                  strip_cor = 1535 - strip;
                }      
              }else{
                strip_cor = strip - nside * 768;
              }
              Identifier stripId = m_id_sct->strip_id(waferId,strip_cor);
              defectList.push_back(stripId);
            }
          }
        }
      }
    }
  }
  return defectList;
} //SCT_ReadCalibDataSvc::defects()
//---------------------------------------------------------------------- 
