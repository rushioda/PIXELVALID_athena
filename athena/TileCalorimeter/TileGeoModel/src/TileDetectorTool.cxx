/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileGeoModel/TileDetectorTool.h"
#include "TileGeoModel/TileDetectorFactory.h"
#include "TileGeoModel/TileAtlasFactory.h"
#include "TileGeoModel/TileTBFactory.h"
#include "TileDetDescr/TileDetDescrManager.h" 
#include "TileDetDescr/TileDddbManager.h"
#include "TileConditions/TileCablingService.h"

#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "CLIDSvc/tools/ClassID_traits.h"
#include "SGTools/DataProxy.h"

TileDetectorTool::TileDetectorTool(const std::string& type, 
				   const std::string& name, 
				   const IInterface* parent):
  GeoModelTool(type, name, parent),
  m_testBeam(false),
  m_addPlates(true),
  m_Ushape(-1),
  m_not_locked(true),
  m_useNewFactory(true),
  m_geometryConfig("FULL"),
  m_manager(0)
{
  declareProperty("TestBeam", m_testBeam);
  //  declareProperty("AddPlatesToCellVolume", m_addPlates); to make user aware that the value is taken from DB rather from JO
  declareProperty("UseNewFactory", m_useNewFactory);
  declareProperty("GeometryConfig",m_geometryConfig);
  declareProperty("Ushape",m_Ushape);
}

TileDetectorTool::~TileDetectorTool()
{
}

StatusCode TileDetectorTool::create(StoreGateSvc* detStore)
{ 
  MsgStream log(msgSvc(), name()); 
  log << MSG::INFO <<" Entering TileDetectorTool::create()" << endreq;

  // Get the detector configuration
  IGeoModelSvc *geoModel;
  service ("GeoModelSvc",geoModel);

  std::string atlasVersion = geoModel->atlasVersion();
  std::string tileVersion = geoModel->tileVersionOverride(); 
 
  std::string versionTag = tileVersion.empty()? atlasVersion : tileVersion;
  std::string versionNode = tileVersion.empty()? "ATLAS" : "TileCal";
 
  if(versionTag == "AUTO")
  {
    versionTag = "TileCal-00";
    versionNode = "TileCal";	
  }
  if (atlasVersion.compare(0,9,"ATLAS-CTB") == 0 || tileVersion.compare(0,6,"TileTB") == 0) {
    log << MSG::INFO << "CTB geometry detected: " << atlasVersion  << " " << tileVersion << endreq;
    m_testBeam = true;
  }
  
  //Locate the top level experiment node
  DataHandle<GeoModelExperiment> theExpt; 
  if (StatusCode::SUCCESS != detStore->retrieve(theExpt, "ATLAS")) 
  { 
    log << MSG::ERROR << "Could not find GeoModelExperiment ATLAS" << endreq; 
    return (StatusCode::FAILURE); 
  } 

  if ( 0 == m_detector )
  {
    IRDBAccessSvc* raccess = 0;
    service("RDBAccessSvc",raccess);

    TileDddbManager_ptr  dbManager(new TileDddbManager(raccess,versionTag,versionNode,&log));
    m_manager = new TileDetDescrManager(dbManager,&log);

    if (0==dbManager->GetNumberOfEnv() && m_useNewFactory) {
      log << MSG::WARNING <<
             "New TileAtlasFactory can not be used because TileGlobals do not exist in Database"<< endreq;
      log << MSG::WARNING <<"Use old TileDetectorFactory instead" << endreq;
      m_useNewFactory = false;
    }

    if (StatusCode::SUCCESS != initIds(detStore,m_manager,&log)) 
    {
      log << MSG::ERROR << "Cannot initialize IdDict helpers" << endreq;
      return (StatusCode::FAILURE); 
    }

    int UshapeDB = dbManager->Ushape();
    if (m_Ushape < 0) {
       m_Ushape = UshapeDB;
       log << MSG::INFO << " U-shape parameter from database is: " << m_Ushape << endreq;
    } else {
       if (m_Ushape != UshapeDB) {
           log << MSG::WARNING << " Overriding U-shape value from DB by value from jobOptions, using " 
               << m_Ushape << " instead of " << UshapeDB << endreq;
       } else {
           log << MSG::INFO << " U-shape parameter from jobOptions is: " << m_Ushape << endreq;
       }
    }
    m_not_locked = false;
    
    m_addPlates = dbManager->addPlatesToCell();
    GeoPhysVol *world=&*theExpt->getPhysVol();
    if(m_testBeam)
    {
      TileTBFactory theTileTBFactory(detStore,m_manager,m_addPlates,m_Ushape,&log);
      theTileTBFactory.create(world);
    }
    else if (m_useNewFactory)
    {
      TileAtlasFactory theTileFactory(detStore,m_manager,m_addPlates,m_Ushape,&log,m_geometryConfig=="FULL");
      theTileFactory.create(world);
    }
    else
    {
//      service("RDBAccessSvc",raccess);
//      const IRDBRecordset* switches = raccess->getRecordset("TileSwitches",versionTag,versionNode);
//      if ((*switches).size()==0) return StatusCode::SUCCESS; 
//      m_computeCellDim = (*switches)[0]->getInt("COMPUTECELLDIM");
//      m_buildGap = (*switches)[0]->getInt("BUILDGAP");
//      m_buildCrack = (*switches)[0]->getInt("BUILDCRACK");
//
      TileDetectorFactory theTileFactory(detStore,m_manager,m_addPlates,m_Ushape,&log);
      theTileFactory.create(world);
    }

    if (StatusCode::SUCCESS != createElements(m_manager,&log)) 
     {
       log << MSG::ERROR << "Cannot create CaloDetDescrElements" << endreq;
       return (StatusCode::FAILURE); 
     }

    // Register the TileDetDescrManager instance with the Transient Detector Store
    if (StatusCode::SUCCESS != detStore->record(m_manager, m_manager->getName()))
    {
      log << MSG::ERROR << "Could not record TileDetDescr manager in detector store" << endreq; 
      return StatusCode::FAILURE;
    }
    theExpt->addManager(m_manager);

    // For reco jobs: release DB manager. Cannot do it here for simulation jobs, they
    // release DB manager as part of global GeoModel release
    if(m_geometryConfig=="RECO") 
      m_manager->releaseDbManager();

    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

StatusCode TileDetectorTool::clear(StoreGateSvc* detStore)
{
  SG::DataProxy* proxy = detStore->proxy(ClassID_traits<TileDetDescrManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode TileDetectorTool::initIds(StoreGateSvc* detStore,
                                     TileDetDescrManager *manager, 
                                     MsgStream *log)
{
  const TileID* tileID;
  const CaloCell_ID* cellID;
  const TileHWID* tileHWID;

  // Get the TileID helper from the detector store
  StatusCode status = detStore->retrieve(tileID, "TileID");
  if (status.isFailure())
  {
    (*log) << MSG::ERROR <<"Could not get TileID helper !" << endreq;
    return StatusCode::FAILURE;
  } 
  manager->set_helper(tileID);


  // Get the CaloCell_ID helper from the detector store
  status = detStore->retrieve(cellID, "CaloCell_ID");
  if (status.isFailure()) 
  {
    (*log) << MSG::ERROR <<"Could not get CaloCell_ID helper!" << endreq;
    return StatusCode::FAILURE;
  } 
  manager->set_helper(cellID);


  // Get the tile HW helper from the detector store
  status = detStore->retrieve(tileHWID, "TileHWID");
  if (status.isFailure()) 
  {
    (*log) << MSG::ERROR <<"Could not get TileHWID helper!" << endreq;
    return StatusCode::FAILURE;
  } 
  manager->set_helper(tileHWID);

  // instanciate Cabling Svc to initialize pointers to helpers there
  TileCablingService * cabling = TileCablingService::getInstance(); 
  if(cabling==0)
  {
    (*log) << MSG::ERROR <<"Could not get instance of TileCablingService" << endreq;
    return StatusCode::FAILURE;
  }
  
  cabling->setTestBeam(m_testBeam);

  return StatusCode::SUCCESS;
}


StatusCode TileDetectorTool::createElements(TileDetDescrManager *manager, MsgStream *log)
{
  // ----------------- Fill in the readout part of detector manager -----------
  const TileID* tileID = manager->get_id();
  const TileHWID* tileHWID = manager->get_hwid();
  bool do_checks = tileID->do_checks();

  tileID->set_do_checks(true);
  manager->create_elements(log);
  tileID->set_do_checks(do_checks);
  tileHWID->set_do_checks(false);

  return StatusCode::SUCCESS;
}
