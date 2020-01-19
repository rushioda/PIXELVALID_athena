/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/PixelServMatFactoryFS.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoTransform.h"


#include "InDetGeoModelUtils/InDetMaterialManager.h"
#include "InDetGeoModelUtils/GenericTubeMaker.h"
#include "InDetGeoModelUtils/TubeVolData.h"


// StoreGate includes
#include "StoreGate/StoreGateSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/Bootstrap.h"
#include <iostream>


PixelServMatFactoryFS::PixelServMatFactoryFS(StoreGateSvc *detStore,
					     ServiceHandle<IRDBAccessSvc> pRDBAccess) :
  m_detStore(detStore),
  m_rdbAccess(pRDBAccess),
  m_materialManager(0),
  m_msg("PixelServMatFactoryFS")
{ 
}


PixelServMatFactoryFS::~PixelServMatFactoryFS()
{
  delete m_materialManager;
}


//## Other Operations (implementation)
void PixelServMatFactoryFS::create(GeoPhysVol *motherP, GeoPhysVol *motherM)
{
  msg(MSG::DEBUG) << "Building Pixel Service Material" << endreq;

  
 // Get the SvcLocator 
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IGeoModelSvc *geoModel;
  StatusCode sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc.isFailure()) msg(MSG::FATAL) << "Could not locate GeoModelSvc" << endreq;

  DecodeVersionKey indetVersionKey(geoModel, "InnerDetector");
  DecodeVersionKey pixelVersionKey(geoModel, "Pixel");

  IRDBRecordset_ptr pixelGenServices = m_rdbAccess->getRecordsetPtr("PixelGenServices",  indetVersionKey.tag(), indetVersionKey.node());

  // Doesn't exist at time of writing but we include it to keep things general
  IRDBRecordset_ptr weightTable = m_rdbAccess->getRecordsetPtr("PixelWeights", pixelVersionKey.tag(), pixelVersionKey.node());

  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table if it exists
  m_materialManager = new InDetMaterialManager("PixelMaterialManager", m_detStore, weightTable, "pix");


  // Build general services:
  //

  for (unsigned int ii =0; ii < pixelGenServices->size(); ii++) {
    
    InDetDD::GenericTubeMaker tubeHelper((*pixelGenServices)[ii]);
    const GeoShape * serviceTubeTmp = tubeHelper.buildShape();

    std::string logName =  tubeHelper.name();
    if (logName.empty()) {
      std::ostringstream o; o << ii;
      logName = "ServMat"+o.str();  
    }
    logName = "Pix" + logName;
   
    const GeoShape* serviceTube = serviceTubeTmp;

    std::string materialName = tubeHelper.materialName();
    //const GeoMaterial* material = m_materialManager->getMaterialForVolume(materialName, serviceTube->volume());
    const GeoMaterial* material = m_materialManager->getMaterial(materialName);
   

    const GeoPcon* servicePcon = dynamic_cast<const GeoPcon*>(serviceTube);
    if(servicePcon) {
      // Divide PCON into two tubs and one cons and place them separately
      if(servicePcon->getNPlanes()!=4) continue;

      GeoTubs* _tubs1 = new GeoTubs(servicePcon->getRMinPlane(0),
				    servicePcon->getRMaxPlane(0),
				    (servicePcon->getZPlane(1)-servicePcon->getZPlane(0))*0.5,
				    servicePcon->getSPhi(),
				    servicePcon->getDPhi());

      GeoCons* _tubs2 = new GeoCons(servicePcon->getRMinPlane(1),servicePcon->getRMinPlane(2),
				    servicePcon->getRMaxPlane(1),servicePcon->getRMaxPlane(2),
				    (servicePcon->getZPlane(2)-servicePcon->getZPlane(1))*0.5,
				    servicePcon->getSPhi(),
				    servicePcon->getDPhi());

      GeoCons* _cons = new GeoCons(servicePcon->getRMinPlane(2),servicePcon->getRMinPlane(3),
				   servicePcon->getRMaxPlane(2),servicePcon->getRMaxPlane(3)+1E-9*CLHEP::mm,
				   (servicePcon->getZPlane(3)-servicePcon->getZPlane(2))*0.5,
				   servicePcon->getSPhi(),
				   servicePcon->getDPhi());

      const GeoLogVol* servLog1 = new GeoLogVol(logName,_tubs1,material);
      GeoVPhysVol* servPhys1 = new GeoPhysVol(servLog1);
      const GeoLogVol* servLog2 = new GeoLogVol(logName,_tubs2,material);
      GeoVPhysVol* servPhys2 = new GeoPhysVol(servLog2);
      const GeoLogVol* servLog3 = new GeoLogVol(logName,_cons,material);
      GeoVPhysVol* servPhys3 = new GeoPhysVol(servLog3);

      // Placement
      tubeHelper.placeVolTwoSide(motherP,motherM,servPhys1,-(servicePcon->getZPlane(1)+servicePcon->getZPlane(0))*0.5);
      tubeHelper.placeVolTwoSide(motherP,motherM,servPhys2,-(servicePcon->getZPlane(2)+servicePcon->getZPlane(1))*0.5);
      tubeHelper.placeVolTwoSide(motherP,motherM,servPhys3,-(servicePcon->getZPlane(3)+servicePcon->getZPlane(2))*0.5);

    } else {
      const GeoLogVol* servLog = new GeoLogVol(logName,serviceTube,material);
      GeoVPhysVol* servPhys = new GeoPhysVol(servLog);
      
      tubeHelper.placeVolTwoSide(motherP,motherM,servPhys);
    }
  }
}

