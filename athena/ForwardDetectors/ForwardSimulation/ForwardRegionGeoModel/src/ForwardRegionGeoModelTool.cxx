/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardRegionGeoModel/ForwardRegionGeoModelTool.h"
#include "ForwardRegionGeoModel/ForwardRegionGeoModelFactory.h"
#include "ForwardRegionGeoModel/ForwardRegionGeoModelManager.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

/**
 ** Constructor(s)
 **/
ForwardRegionGeoModelTool::ForwardRegionGeoModelTool( const std::string& type, const std::string& name, const IInterface* parent )
: GeoModelTool( type, name, parent )
{
    m_Config.clear();
    declareProperty("TCL4JawDistB1I",m_Config.TCL4JawDistB1I=57*CLHEP::mm);
    declareProperty("TCL4JawDistB2I",m_Config.TCL4JawDistB2I=57*CLHEP::mm);
    declareProperty("TCL5JawDistB1I",m_Config.TCL5JawDistB1I=57*CLHEP::mm);
    declareProperty("TCL5JawDistB2I",m_Config.TCL5JawDistB2I=57*CLHEP::mm);
    declareProperty("TCL6JawDistB1I",m_Config.TCL6JawDistB1I=57*CLHEP::mm);
    declareProperty("TCL6JawDistB2I",m_Config.TCL6JawDistB2I=57*CLHEP::mm);
    declareProperty("TCL4JawDistB1O",m_Config.TCL4JawDistB1O=57*CLHEP::mm);
    declareProperty("TCL4JawDistB2O",m_Config.TCL4JawDistB2O=57*CLHEP::mm);
    declareProperty("TCL5JawDistB1O",m_Config.TCL5JawDistB1O=57*CLHEP::mm);
    declareProperty("TCL5JawDistB2O",m_Config.TCL5JawDistB2O=57*CLHEP::mm);
    declareProperty("TCL6JawDistB1O",m_Config.TCL6JawDistB1O=57*CLHEP::mm);
    declareProperty("TCL6JawDistB2O",m_Config.TCL6JawDistB2O=57*CLHEP::mm);
    declareProperty("vp1Compatibility", m_Config.vp1Compatibility=false);
    declareProperty("buildTCL4",m_Config.buildTCL4=false);
    declareProperty("buildTCL6",m_Config.buildTCL6=false);
    declareProperty("ALFAInNewPosition",m_Config.ALFAInNewPosition=false);
    declareProperty("newPosB7L1",m_Config.newPosB7L1=245656.77*CLHEP::mm);
    declareProperty("newPosB7R1",m_Config.newPosB7R1=-245656.11*CLHEP::mm);
    declareProperty("posAFPL1",m_Config.posAFPL1=204500*CLHEP::mm);
    declareProperty("posAFPR1",m_Config.posAFPR1=-204500*CLHEP::mm);
    declareProperty("posAFPL2",m_Config.posAFPL2=212675*CLHEP::mm);
    declareProperty("posAFPR2",m_Config.posAFPR2=-212675*CLHEP::mm);
}

/**
 ** Destructor
 **/
ForwardRegionGeoModelTool::~ForwardRegionGeoModelTool()
{
        // This will need to be modified once we register the  DetectorNode in
	// the Transient Detector Store
	if ( 0 != m_detector ) {
		delete m_detector;
		m_detector = 0;
	}
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode
ForwardRegionGeoModelTool::create( StoreGateSvc* detStore )
{ 
  MsgStream log(msgSvc(), name()); 
  // 
  // Locate the top level experiment node 
  // 
  DataHandle<GeoModelExperiment> theExpt;
  StatusCode sc = detStore->retrieve( theExpt, "ATLAS" );
  if (StatusCode::SUCCESS != sc) {
    log << MSG::ERROR 
	<< "Could not find GeoModelExperiment ATLAS" 
	<< endreq; 
    return (StatusCode::FAILURE); 
  } 

  ForwardRegionGeoModelFactory theFactory(detStore, &m_Config);
  if ( 0 == m_detector ) {
    // Create the DetectorNode instance
    try {   
      //
      // This strange way of casting is to avoid an
      // utterly brain damaged compiler warning.
      //
      GeoPhysVol *world=&*theExpt->getPhysVol();
      theFactory.create(world);
    } catch (std::bad_alloc) {
      log << MSG::FATAL << "Could not create new DetectorNode!" << endreq;
      return StatusCode::FAILURE; 
    }
    // Register the DetectorNode instance with the Transient Detector Store
    theExpt->addManager(theFactory.getDetectorManager());
    sc = detStore->record(theFactory.getDetectorManager(),theFactory.getDetectorManager()->getName());
    if (StatusCode::SUCCESS != sc) {
      log << MSG::ERROR
          << "Could not register DetectorNode"
          << endreq;
      return (StatusCode::FAILURE);
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
