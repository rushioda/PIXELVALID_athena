/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_SLHC_DetectorTool.h"
#include "SCT_SLHC_GeoModel/SCT_DetectorFactory.h" 
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h" 
#include "SCT_SLHC_GeoModel/SCT_Options.h" 
#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h" 
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetCondServices/ISiLorentzAngleSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using InDetDD::SCT_DetectorManager; 
using InDetDD::SiDetectorManager; 


SCT_SLHC_DetectorTool::SCT_SLHC_DetectorTool( const std::string& type,
				    const std::string& name, 
				    const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_detectorName("SCT"),
    m_alignable(false),
    m_forceBuild(false),
    m_manager(0), 
    m_athenaComps(0),
    m_serviceBuilderTool(""),
    m_geoModelSvc("GeoModelSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("InDetGeometryDBSvc",name),
    m_lorentzAngleSvc("SCTLorentzAngleSvc", name)
{
    // Get parameter values from jobOptions file
    declareProperty("ForceBuild", m_forceBuild);
    declareProperty("DetectorName", m_detectorName);
    declareProperty("Alignable", m_alignable);
    declareProperty("ServiceBuilderTool", m_serviceBuilderTool);
    declareProperty("RDBAccessSvc", m_rdbAccessSvc);
    declareProperty("GeometryDBSvc", m_geometryDBSvc);
    declareProperty("GeoModelSvc", m_geoModelSvc);
    declareProperty("LorentzAngleSvc", m_lorentzAngleSvc);
}

SCT_SLHC_DetectorTool::~SCT_SLHC_DetectorTool()
{
  delete m_athenaComps;
}

// Create the Geometry via the factory corresponding to this tool
StatusCode SCT_SLHC_DetectorTool::create( StoreGateSvc* detStore ){ 

  StatusCode result = StatusCode::SUCCESS;
  // Get the detector configuration.
  // Get the detector configuration.
  StatusCode sc = m_geoModelSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;
    return (StatusCode::FAILURE); 
  }  

  DecodeVersionKey versionKey(&*m_geoModelSvc, "SCT");
 
  // Issue error if AUTO.
  if (versionKey.tag() == "AUTO"){
    msg(MSG::ERROR) << "AUTO Atlas version. Please select a version." << endreq;
  }

  msg(MSG::INFO) << "Building SCT SLHC with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node() << endreq;

  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endreq;
    return (StatusCode::FAILURE); 
  }  

  // Print the SCT version tag:
  std::string sctVersionTag;
  sctVersionTag = m_rdbAccessSvc->getChildTag("SCT", versionKey.tag(), versionKey.node(), false);
  msg(MSG::INFO) << "SCT Version: " << sctVersionTag <<  "  Package Version: " << PACKAGE_VERSION << endreq;

  // Check if version is empty. If so, then the SCT cannot be built. 
  //This may or may not be intentional. We just issue an INFO message. 
  if (sctVersionTag.empty()){ 
     msg(MSG::INFO) <<  "No SCT Version. SCT_SLHC will not be built." << endreq;
  }else{
    std::string versionName;
    std::string descrName="noDescr";

    if(versionKey.custom()){
      msg(MSG::WARNING) << "SCT_SLHC_DetectorTool:  Detector Information coming "
	  <<"from a custom configuration!!" << endreq;    
    }else{
      msg(MSG::DEBUG) <<"SCT_SLHC_DetectorTool:  Detector Information coming from the"
		      <<" database and job options IGNORED." << endreq;
      msg(MSG::DEBUG) << "Keys for SCT Switches are "  << versionKey.tag()  
		      << "  " << versionKey.node() << endreq;
      IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("SctSwitches", versionKey.tag(), versionKey.node());
      const IRDBRecord    *switches   = (*switchSet)[0];
        
      m_detectorName       = switches->getString("DETECTORNAME");
      
      if (!switches->isFieldNull("VERSIONNAME")) {
	versionName = switches->getString("VERSIONNAME"); 
      } 
      if (!switches->isFieldNull("DESCRIPTION")) {
	descrName = switches->getString("DESCRIPTION");
      }
  
    }
  
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "VersioName = " << versionName  << endreq;
   

    // Only build if versionName is "SLHC" or ForceBuild is true

    if (!m_forceBuild && versionName != "SLHC") {

       msg(MSG::INFO) << "Not SLHC version. SCT_SLHC will not be built." << endreq;

    } else { // SLHC version requested.
      
      msg(MSG::DEBUG) << "Creating the SCT (SLHC version)" << endreq;

      // Create the SCT_DetectorFactory
      m_manager = 0;
      // Locate the top level experiment node  
      GeoModelExperiment * theExpt; 
      if (StatusCode::SUCCESS != detStore->retrieve(theExpt, "ATLAS")){ 
	msg(MSG::ERROR) 
	  << "Could not find GeoModelExperiment ATLAS" 
	  << endreq; 
	return (StatusCode::FAILURE); 
      } 

      // Retrieve the Geometry DB Interface
      sc = m_geometryDBSvc.retrieve();
      if (sc.isFailure()) {
	msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endreq;
	return (StatusCode::FAILURE); 
      }  

      // Pass athena services to factory, etc
      m_athenaComps = new InDetDDSLHC::SCT_GeoModelAthenaComps;
      m_athenaComps->setDetStore(detStore);
      m_athenaComps->setGeoModelSvc(&*m_geoModelSvc);
      m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);
      m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
      m_athenaComps->setLorentzAngleSvc(m_lorentzAngleSvc);

      const SCT_ID* idHelper;
      if (detStore->retrieve(idHelper, "SCT_ID").isFailure()) {
	msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
	return StatusCode::FAILURE;
      } else {
	m_athenaComps->setIdHelper(idHelper);
      }
    
      //
      // LorentzAngleService
      //
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) 
	<< "Lorentz angle service passed to InDetReadoutGeometry with name: " << m_lorentzAngleSvc.name() << endreq;
    

      // Service builder tool
      if (!m_serviceBuilderTool.empty()) {
	sc = m_serviceBuilderTool.retrieve(); 
	if (!sc.isFailure()) {
	  msg(MSG::INFO) << "Service builder tool retrieved: " << m_serviceBuilderTool << endreq;
	  m_athenaComps->setServiceBuilderTool(&*m_serviceBuilderTool);
	} else {
	  msg(MSG::ERROR) << "Could not retrieve " <<  m_serviceBuilderTool << ",  some services will not be built." << endreq;
	}
      } else {
	// This will become an error once the tool is ready.
	//msg(MSG::ERROR) << "Service builder tool not specified. Some services will not be built" << endreq;
	msg(MSG::INFO) << "Service builder tool not specified." << endreq; 
      } 


      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      GeoPhysVol *world=&*theExpt->getPhysVol();
      InDetDDSLHC::SCT_Options options;
      options.setAlignable(m_alignable);
      InDetDDSLHC::SCT_DetectorFactory theSCT(m_athenaComps, options);
      if(descrName.compare("TrackingGeometry")!=0)
	theSCT.create(world);
      else
	msg(MSG::INFO) << "SCT_SLHC - TrackingGeometry tag - no geometry built" << endreq; 
      m_manager = theSCT.getDetectorManager();
    
      if (!m_manager) {
	msg(MSG::ERROR) << "SCT_DetectorManager not created" << endreq;
	return( StatusCode::FAILURE );
      }
    
      // Get the manager from the factory and store it in the detector store.
      msg(MSG::DEBUG) << "Registering SCT_DetectorManager. " << endreq;
      result = detStore->record(m_manager, m_manager->getName());
      if (result.isFailure() ) {
	msg(MSG::ERROR) << "Could not register SCT_DetectorManager" << endreq;
	return( StatusCode::FAILURE );
      }
      theExpt->addManager(m_manager);
      // Create a symLink to the SiDetectorManager base class
      const SiDetectorManager * siDetManager = m_manager;
      result = detStore->symLink(m_manager, siDetManager);
      if(result.isFailure()){
	msg(MSG::ERROR) << "Could not make link between SCT_DetectorManager and "
			<<"SiDetectorManager" << endreq;
	return( StatusCode::FAILURE );
      } 
   

      //
      // LorentzAngleService
      // We retrieve it to make sure it is initialized during geometry initialization.
      //
      if (!m_lorentzAngleSvc.empty()) {
	sc = m_lorentzAngleSvc.retrieve();
	if (!sc.isFailure()) {
	  msg(MSG::INFO) << "Lorentz angle service retrieved: " << m_lorentzAngleSvc << endreq;
	} else {
	  msg(MSG::INFO) << "Could not retrieve Lorentz angle service:" <<  m_lorentzAngleSvc << endreq;
	}
      } else {
	msg(MSG::INFO) << "Lorentz angle service not requested." << endreq;
      } 
    }  
  }
  return result;
}

StatusCode 
SCT_SLHC_DetectorTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* _proxy = detStore->proxy(ClassID_traits<InDetDD::SCT_DetectorManager>::ID(),m_manager->getName());
  if(_proxy) {
    _proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

