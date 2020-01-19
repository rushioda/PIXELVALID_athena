/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/OraclePixGeoManager.h"
#include "PixelGeoModel/PixelLegacyManager.h"
#include "PixelGeoModel/PixelMaterialMap.h"
#include "PixelGeoModel/PixelStaveTypes.h"
#include "InDetGeoModelUtils/PairIndexMap.h"
#include "InDetGeoModelUtils/TopLevelPlacements.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

// to permit access to StoreGate
#include "StoreGate/StoreGateSvc.h"

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GeoModelKernel/GeoMaterial.h"

//
// Get the pixelDD Manager from SG.
//
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

//
// Distorted material manager
//
#include "InDetGeoModelUtils/DistortedMaterialManager.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

using InDetDD::PixelDetectorManager; 

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

OraclePixGeoManager::OraclePixGeoManager(const PixelGeoModelAthenaComps * athenaComps)
  : PixelGeometryManager(athenaComps), 
    m_eta(0),
    m_phi(0),
    currentLD(0),
    BarrelEndcap(0),
    _side(0),
    m_diskFrontBack(0),
    m_servicesOnLadder(true),  
    m_services(true),  
    m_initialLayout(false), 
    m_dc1Geometry(false),
    m_alignable(true),
    m_slhc(false),
    m_ibl(false),
    m_PlanarModuleNumber(0),
    m_3DModuleNumber(0),
    m_dbm(false),
    m_legacyManager(0),
    m_gangedIndexMap(0),
    m_frameElementMap(0),
    m_diskRingIndexMap(0),
    m_zPositionMap(0),
    m_dbVersion(0),
    m_defaultLengthUnit(CLHEP::mm)
{
  m_commonItems = 0;
  m_pDDmgr = 0;

  init();
}

void
OraclePixGeoManager::init()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using ORACLE PIXEL GEOMETRY MANAGER" << endmsg;

  IRDBAccessSvc *rdbSvc = athenaComps()->rdbAccessSvc();
  IGeoModelSvc *geoModel = athenaComps()->geoModelSvc();;

  // Get version tag and node for Pixel.
  DecodeVersionKey versionKey(geoModel, "Pixel");
  std::string detectorKey  = versionKey.tag();
  std::string detectorNode = versionKey.node();

  // Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(geoModel, "InnerDetector");

  m_versionTag = rdbSvc->getChildTag("Pixel", versionKey.tag(), versionKey.node(), false);

/////////////////////////////////////////////////////////
//
// Gets the structures from the det store
//
/////////////////////////////////////////////////////////

  if(msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::INFO) << "Key = " << detectorKey << " Node = " << detectorNode << endmsg;
  }

  //atls = rdbSvc->getRecordset("AtlasMother",geoModel->atlasVersion(), "ATLAS");
  PixelSwitches      = rdbSvc->getRecordsetPtr("PixelSwitches",          detectorKey, detectorNode);
  PixelBarrelGeneral = rdbSvc->getRecordsetPtr("PixelBarrelGeneral",     detectorKey, detectorNode);
  PixelBarrelService = rdbSvc->getRecordsetPtr("PixelBarrelService",     detectorKey, detectorNode);
  PixelCommon        = rdbSvc->getRecordsetPtr("PixelCommon",            detectorKey, detectorNode);
  PixelEnvelope      = rdbSvc->getRecordsetPtr("PixelEnvelope",          detectorKey, detectorNode);
  PixelDisk          = rdbSvc->getRecordsetPtr("PixelDisk",              detectorKey, detectorNode);
  PixelDiskRing      = rdbSvc->getRecordsetPtr("PixelDiskRing",          detectorKey, detectorNode);
  PixelRing          = rdbSvc->getRecordsetPtr("PixelRing",              detectorKey, detectorNode);
  PixelEndcapGeneral = rdbSvc->getRecordsetPtr("PixelEndcapGeneral",     detectorKey, detectorNode);
  PixelEndcapService = rdbSvc->getRecordsetPtr("PixelEndcapService",     detectorKey, detectorNode);
  PixelEnvelopeService = rdbSvc->getRecordsetPtr("PixelEnvelopeService",     detectorKey, detectorNode);
  PixelLayer         = rdbSvc->getRecordsetPtr("PixelLayer",             detectorKey, detectorNode);
  PixelModule        = rdbSvc->getRecordsetPtr("PixelModule",            detectorKey, detectorNode);
  PixelModuleSvc     = rdbSvc->getRecordsetPtr("PixelModuleSvc",            detectorKey, detectorNode);
  PixelStave         = rdbSvc->getRecordsetPtr("PixelStave",             detectorKey, detectorNode);
  PixelStaveZ        = rdbSvc->getRecordsetPtr("PixelStaveZ",            detectorKey, detectorNode);
  PixelTopLevel      = rdbSvc->getRecordsetPtr("PixelTopLevel",          detectorKey, detectorNode);
  PixelReadout       = rdbSvc->getRecordsetPtr("PixelReadout",           detectorKey, detectorNode);
  PixelGangedPixels  = rdbSvc->getRecordsetPtr("GangedPixels",           detectorKey, detectorNode);
  PixelBarrelCable   = rdbSvc->getRecordsetPtr("PixelBarrelCable",       detectorKey, detectorNode);
  PixelTMT           = rdbSvc->getRecordsetPtr("PixelTMT",               detectorKey, detectorNode);
  PixelOmega         = rdbSvc->getRecordsetPtr("PixelOmega",             detectorKey, detectorNode);
  PixelOmegaGlue     = rdbSvc->getRecordsetPtr("PixelOmegaGlue",         detectorKey, detectorNode);
  PixelAlTube        = rdbSvc->getRecordsetPtr("PixelAlTube",            detectorKey, detectorNode);
  PixelFluid         = rdbSvc->getRecordsetPtr("PixelFluid",             detectorKey, detectorNode);
  PixelConnector     = rdbSvc->getRecordsetPtr("PixelConnector",         detectorKey, detectorNode);
  PixelPigtail       = rdbSvc->getRecordsetPtr("PixelPigtail",           detectorKey, detectorNode);
  PixelSimpleService = rdbSvc->getRecordsetPtr("PixelSimpleService",     detectorKey, detectorNode);
  PixelFrame         = rdbSvc->getRecordsetPtr("PixelFrame",             detectorKey, detectorNode);
  PixelFrameSect     = rdbSvc->getRecordsetPtr("PixelFrameSect",         detectorKey, detectorNode);
  PixelIBLStave      = rdbSvc->getRecordsetPtr("PixelIBLStave"  ,        detectorKey, detectorNode);
  PixelIBLSupport    = rdbSvc->getRecordsetPtr("PixelIBLSupport",        detectorKey, detectorNode);
  PixelIBLFlex    = rdbSvc->getRecordsetPtr("PixelIBLFlex",        detectorKey, detectorNode);
  PixelIBLFlexMaterial    = rdbSvc->getRecordsetPtr("PixelIBLFlexMaterial",        detectorKey, detectorNode);
  PixelIBLGlueGrease    = rdbSvc->getRecordsetPtr("PixelIBLGlueGrease",        detectorKey, detectorNode);
  PixelConicalStave     = rdbSvc->getRecordsetPtr("PixelConicalStave",             detectorKey, detectorNode);

  // Weights table 
  m_weightTable = rdbSvc->getRecordsetPtr("PixelWeights", detectorKey, detectorNode);

  // Extra Scaling table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("PixelMatScaling", indetVersionKey.tag(), indetVersionKey.node());

  // MaterialMap
  m_materialTable = rdbSvc->getRecordsetPtr("PixelMaterialMap", detectorKey, detectorNode);

  // Pixel stave types
  m_staveTypeTable = rdbSvc->getRecordsetPtr("PixelStaveType", detectorKey, detectorNode);

  // DBM
  DBMTelescope  = rdbSvc->getRecordsetPtr("DBMTelescope",      detectorKey, detectorNode);
  DBMBracket    = rdbSvc->getRecordsetPtr("DBMBracket",        detectorKey, detectorNode);
  DBMCage    = rdbSvc->getRecordsetPtr("DBMCage",        detectorKey, detectorNode);
  DBMModule     = rdbSvc->getRecordsetPtr("DBMModule",         detectorKey, detectorNode);
  m_dbmWeightTable  = rdbSvc->getRecordsetPtr("DBMMaterials",      detectorKey, detectorNode);

  m_dbVersion = determineDbVersion();

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Database version number: " << dbVersion() << endmsg;

  if (dbVersion() < 4) {
    m_legacyManager = new PixelLegacyManager(rdbSvc,  detectorKey, detectorNode);
  }

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "... Record Sets retrieved." << endmsg;

  m_distortedMatManager = new InDetDD::DistortedMaterialManager;
 
  // Set default lenth unit to CLHEP::mm for newer version and CLHEP::cm for older versions
  m_defaultLengthUnit =  (dbVersion() < 3) ? CLHEP::cm : CLHEP::mm;

  // Get the top level placements
  m_placements = new TopLevelPlacements(PixelTopLevel);

  // If all individual pieces are not present, then actually all are present.
  m_allPartsPresent = (!m_placements->present("Barrel") && !m_placements->present("EndcapA") &&  !m_placements->present("EndcapC"));

  // cache the number of inner frames
  if (dbVersion() < 3) {
    m_barrelInFrames =  (*PixelBarrelGeneral)[0]->getInt("NFRAMEIN");
    m_endcapInFrames =  (*PixelEndcapGeneral)[0]->getInt("NFRAMEIN");
  } else {
    m_barrelInFrames = 0;
    m_endcapInFrames = 0;
  }

  //
  // Get the InDet material manager. This is a wrapper around the geomodel one with some extra functionality to deal
  // with weights table if it exists
 
  m_pMatMgr = new InDetMaterialManager("PixelMaterialManager", athenaComps());
  m_pMatMgr->addWeightTable(m_weightTable, "pix");
  m_pMatMgr->addScalingTable(m_scalingTable);

  // add the DBM weight table
  m_pMatMgr->addWeightTable(m_dbmWeightTable, "pix");

  // Create material map
  m_materialMap = new PixelMaterialMap(db(), m_materialTable);
  if (m_materialTable->size() == 0) addDefaultMaterials();

  // Create stave type map
  m_pixelStaveTypes = new PixelStaveTypes(db(), m_staveTypeTable);
 
  
  //
  // Print the version number for the barrel and endcap geometry
  //  
  //cout << "Instantiating Pixel Detector" << endl;
  //cout << "Barrel Version " << this->PixelBarrelMajorVersion() << "." << this->PixelBarrelMinorVersion() << endl;
  //cout << "Endcap Version " << this->PixelEndcapMajorVersion() << "." << this->PixelEndcapMinorVersion() << endl;
}

InDetMaterialManager* OraclePixGeoManager::getMaterialManager()
{
  return m_pMatMgr;
}

PixelLegacyManager * OraclePixGeoManager::legacyManager() const
{
  return m_legacyManager;
}


OraclePixGeoManager::~OraclePixGeoManager()
{
  delete m_placements;
  delete m_distortedMatManager;
  delete m_materialMap;
  delete m_pixelStaveTypes;
  delete m_legacyManager;
  delete m_pMatMgr;
  delete m_gangedIndexMap;
  delete m_diskRingIndexMap;
  delete m_zPositionMap;
  delete m_frameElementMap;
}


InDetDD::SiCommonItems * 
OraclePixGeoManager::commonItems() const
{
  return m_commonItems;
}


void 
OraclePixGeoManager::setCommonItems(InDetDD::SiCommonItems * commonItems)
{
  m_commonItems = commonItems;
}


const PixelID * 
OraclePixGeoManager::getIdHelper() 
{
  return athenaComps()->getIdHelper();
}


const HepGeom::Transform3D & 
OraclePixGeoManager::partTransform(const std::string & partName) const 
{
  return m_placements->transform(partName);
}


bool 
OraclePixGeoManager::partPresent(const std::string & partName) const
{
  // First check if overridden from text file.
  if (partName == "Barrel") {
    if (db()->testField("PixelCommon","DOBARREL")) {
      return db()->getInt("PixelCommon","DOBARREL");
    }
  } else if (partName == "EndcapA" || partName == "EndcapC") {
    if (db()->testField("PixelCommon","DOENDCAPS")) {
      return db()->getInt("PixelCommon","DOENDCAPS");
    }
  }
  // otherwise check database.
  return (m_allPartsPresent || m_placements->present(partName));
}

/////////////////////////////////////////////////////////
//
// Setting of Layer/Disk and Barrel/EndCap
//
/////////////////////////////////////////////////////////
void OraclePixGeoManager::SetCurrentLD(int i)
{
  if(isBarrel() ) {
    if(i <= PixelBarrelNLayer()) {
      currentLD=i;
    } else {
      msg(MSG::ERROR) << "Layer set out of bounds: " << i << ", Setting it to 0" << endmsg;
      currentLD = 0;
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Current layer set to " << currentLD << endmsg;
  }
  if(isEndcap() ) {
     if (i<= PixelEndcapNDisk() ) {
       currentLD=i;
     } else {
       msg(MSG::ERROR) << "Disk set out of bounds: " << i << ", Setting it to 0" << endmsg;
       currentLD = 0;
     }
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Current disk set to " << currentLD << endmsg;
  } 
 if(isDBM() ) {
     if (i<= 2 ) {
       currentLD=i;
     } else {
       msg(MSG::ERROR) << "DBM: Disk set out of bounds: " << i << ", Setting it to 0" << endmsg;
       currentLD = 0;
     }
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<" Current disk set to " << currentLD << endmsg;
  }
}

void OraclePixGeoManager::SetBarrel() {
  //msg(MSG::DEBUG) << "Setting Barrel" << endmsg;
  BarrelEndcap = 0;
}
void OraclePixGeoManager::SetEndcap() {
  BarrelEndcap = 1;
  //msg(MSG::DEBUG) << "Setting Endcap" << endmsg;
}
void OraclePixGeoManager::SetPartsDBM() {
  BarrelEndcap = 2;
  //msg(MSG::DEBUG) << "Setting DBM" << endmsg;
}
/////////////////////////////////////////////////////////
//
// Check if the current layer/disk has to be retrieved
// By default in MySQL all the three layers have to be retrieved
// If m_initialLayout is true we ignore NOVA for layer/disk 1.
// For initial layout layer=1 and disk=1 (2nd layer and disk) is missing.
//
/////////////////////////////////////////////////////////
bool OraclePixGeoManager::isLDPresent() {
  if(isBarrel()) {
    if (m_initialLayout && currentLD == 1) return false;
    std::ostringstream A;
    A << "_" << currentLD;
    // More than 3 layers not yet supported in database so
    // if not present in text file assume using this layer
    if (slhc() && !db()->testFieldTxt(PixelBarrelGeneral,"USELAYER"+A.str())) return true;
    return db()->getInt(PixelBarrelGeneral,"USELAYER"+A.str());
  }
  if(isEndcap() ) {
    if (m_initialLayout && currentLD == 1) return false;
    std::ostringstream A;
    A << "_" << currentLD;
    // More than 3 disks not yet supported in database so
    // if not present in text file assume using this disks
    if (slhc() && !db()->testFieldTxt(PixelEndcapGeneral,"USEDISK"+A.str())) return true;
    return db()->getInt(PixelEndcapGeneral,"USEDISK"+A.str());
  }
  return false;
}


bool OraclePixGeoManager::isBarrel() {
  if(BarrelEndcap == 0) {
    return true;
  } else {
    return false;
  }
}
bool OraclePixGeoManager::isEndcap() {
  if(BarrelEndcap == 1) {
    return true;
  } else {
    return false;
  }
  return false;
}
bool OraclePixGeoManager::isDBM() {
  if(BarrelEndcap == 2) {
    return true;
  } else {
    return false;
  }
}

bool OraclePixGeoManager::DoServices() {
  return m_services;
}
bool OraclePixGeoManager::DoServicesOnLadder() {
  return m_servicesOnLadder;
}

bool OraclePixGeoManager::InitialLayout() const {
  return m_initialLayout;
}

void OraclePixGeoManager::SetDC1Geometry(bool flag) {
  m_dc1Geometry = flag; 
  if (m_legacyManager) m_legacyManager->SetDC1Geometry(flag);
}

bool OraclePixGeoManager::DC1Geometry() const {
  return m_dc1Geometry;
}

bool OraclePixGeoManager::Alignable() const {
  return m_alignable;
}


PixelDetectorManager* OraclePixGeoManager::GetPixelDDManager() {
  if(m_pDDmgr == NULL) {
    //
    // retrieve the pointer to the DD manager
    //
    StatusCode sc = athenaComps()->detStore()->retrieve(m_pDDmgr);  
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Cannot retrieve PixelDetectorManager" << endmsg;
    } 
  }
  return m_pDDmgr;
}  


const InDetDD::DistortedMaterialManager *
OraclePixGeoManager::distortedMatManager() const{
  return m_distortedMatManager;
}  


/////////////////////////////////////////////////////////
//
// Calculate Thickness. This is used for the materials
// which thickness is given in % of r.l.
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::CalculateThickness(double tck,string mat) {
  const GeoMaterial* material =  m_pMatMgr->getMaterial(mat);
  double rl = material->getRadLength();
  material->ref();
  material->unref();
  return -1.*rl*tck/100.;
}

int OraclePixGeoManager::moduleType()
{
  int type = 0;
  if (slhc() || ibl()) {
    if (isBarrel()) {
      type = db()->getInt(PixelLayer,"MODULETYPE",currentLD);
    }
    if (isEndcap() && slhc()) {
      // Not in DB yet.
      int ringType = getDiskRingType(currentLD,m_eta);
      if (ringType>=0) {
	type = db()->getInt(PixelRing,"MODULETYPE",ringType);
      }
    }
  } else {
    if(isBarrel()) type = currentLD;
    if(isEndcap()) type = currentLD+PixelBarrelNLayer();
  }
  return type;
}

int OraclePixGeoManager::moduleType3D()
{
  int type = -1;
  if (!isBarrel()||currentLD>0) return type;

  if (ibl()) {
    try {
      type = db()->getInt(PixelLayer,"MODULETYPE3D",currentLD);
      return type;
    }
    catch(...)
      { 
	return moduleType()+1;
      }
  }
  
  return type;
}



/////////////////////////////////////////////////////////
//
// Si Boards Parameters:
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::PixelBoardWidth(bool isModule3D) 
{
  if(ibl()&&isModule3D){
    return db()->getDouble(PixelModule,"BOARDWIDTH",moduleType3D())*mmcm();
  }

  return db()->getDouble(PixelModule,"BOARDWIDTH",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelBoardLength(bool isModule3D) 
{
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"BOARDLENGTH",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"BOARDLENGTH",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelBoardThickness(bool isModule3D) 
{
  if (m_dc1Geometry && isBarrel() && (currentLD == 0)) {
    return 200*CLHEP::micrometer;
  }

  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"BOARDTHICK",moduleType3D())*mmcm();
  return db()->getDouble(PixelModule,"BOARDTHICK",moduleType())*mmcm();
}

double  OraclePixGeoManager::PixelBoardActiveLength(bool isModule3D) 
{
  return DesignZActiveArea(isModule3D); 
}


/////////////////////////////////////////////////////////
//
// Hybrid Parameters:
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::PixelHybridWidth(bool isModule3D) 
{
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"HYBRIDWIDTH",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"HYBRIDWIDTH",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelHybridLength(bool isModule3D) 
{
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"HYBRIDLENGTH",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"HYBRIDLENGTH",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelHybridThickness(bool isModule3D) 
{
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"HYBRIDTHICK",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"HYBRIDTHICK",moduleType())*mmcm();
}
 
/////////////////////////////////////////////////////////
//
// Chip Parameters:
//
/////////////////////////////////////////////////////////

double OraclePixGeoManager::PixelChipWidth(bool isModule3D) 
{
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"CHIPWIDTH",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"CHIPWIDTH",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelChipLength(bool isModule3D) 
{
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"CHIPLENGTH",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"CHIPLENGTH",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelChipGap(bool isModule3D) 
{
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"CHIPGAP",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"CHIPGAP",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelChipOffset(bool isModule3D) 
{
   if(!ibl()||GetLD()!=0||!isBarrel()||!(db()->testField(PixelModule,"CHIPOFFSET"))){
     return 0.;
   }

  if(isModule3D)
    return db()->getDouble(PixelModule,"CHIPOFFSET",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"CHIPOFFSET",moduleType())*mmcm();
}

double OraclePixGeoManager::PixelChipThickness(bool isModule3D)  {
  if(ibl()&&isModule3D)
    return db()->getDouble(PixelModule,"CHIPTHICK",moduleType3D())*mmcm();

  return db()->getDouble(PixelModule,"CHIPTHICK",moduleType())*mmcm();
}



/////////////////////////////////////////////////////////
//
// Module services
//
/////////////////////////////////////////////////////////

int OraclePixGeoManager::PixelModuleServiceNumber()
{
  if(!ibl()||GetLD()>0||!isBarrel()) return 0;

  if (db()->getTableSize(PixelModuleSvc)) 
    return db()->getTableSize(PixelModuleSvc);
  return 0;
}

double OraclePixGeoManager::PixelModuleServiceLength(int svc)
{
  return db()->getDouble(PixelModuleSvc,"LENGTH",svc)*mmcm();
}

double OraclePixGeoManager::PixelModuleServiceWidth(int svc)
{
  return db()->getDouble(PixelModuleSvc,"WIDTH",svc)*mmcm();
}

double OraclePixGeoManager::PixelModuleServiceThick(int svc)
{
  return db()->getDouble(PixelModuleSvc,"THICK",svc)*mmcm();
}

double OraclePixGeoManager::PixelModuleServiceOffsetX(int svc)
{
  return db()->getDouble(PixelModuleSvc,"XOFFSET",svc)*mmcm();
}

double OraclePixGeoManager::PixelModuleServiceOffsetY(int svc)
{
  return db()->getDouble(PixelModuleSvc,"YOFFSET",svc)*mmcm();
}

double OraclePixGeoManager::PixelModuleServiceOffsetZ(int svc)
{
  return db()->getDouble(PixelModuleSvc,"ZOFFSET",svc)*mmcm();
}

int OraclePixGeoManager::PixelModuleServiceFullSize(int svc)
{
  return db()->getInt(PixelModuleSvc,"FULLSIZE",svc);
}

int OraclePixGeoManager::PixelModuleServiceModuleType(int svc)
{
  return db()->getInt(PixelModuleSvc,"MODULE3D",svc)*mmcm();
}

std::string OraclePixGeoManager::PixelModuleServiceName(int svc)
{
  return db()->getString(PixelModuleSvc,"NAME",svc);
}

std::string OraclePixGeoManager::PixelModuleServiceMaterial(int svc)
{
  return db()->getString(PixelModuleSvc,"MATERIAL",svc);
}



/////////////////////////////////////////////////////////
//
// Pixel Disks carbon structure
//
/////////////////////////////////////////////////////////
double OraclePixGeoManager::PixelECCarbonRMin(string a) {

  int isup=0;
  if (a == "Inner") {
    isup = 0;
  } else if(a == "Central") {
    isup = 1;
  } else {
    isup = 2;
  }

  return PixelDiskSupportRMin(isup);
}

double OraclePixGeoManager::PixelECCarbonRMax(string a) {
  int isup=0;
  if (a == "Inner") {
    isup = 0;
  } else if(a == "Central") {
    isup = 1;
  } else {
    isup = 2;
  }
  return PixelDiskSupportRMax(isup);
}

double OraclePixGeoManager::PixelECCarbonThickness(string a) {

  int isup=0;
  if (a == "Inner") {
    isup = 0;
  } else if(a == "Central") {
    isup = 1;
  } else {
    isup = 2;
  }
  return PixelDiskSupportThickness(isup);
}


int OraclePixGeoManager::PixelECCarbonMaterialTypeNum(string a) {

  if (dbVersion() < 3) return 0;
  int isup = 0;
  if (a == "Inner") {
    isup = 0;
  } else if(a == "Central") {
    isup = 1;
  } else {
    isup = 2;
  }
  return PixelDiskSupportMaterialTypeNum(isup);
}


/////////////////////////////////////////////////////////
//
// Central Services
//
/////////////////////////////////////////////////////////
//
// This is complicated in the DB...
// If Rmin < 0 this means that the cylinder has to be placed only once
// So I return RMin with its sign, for further processing by the service
// methods.
// If RMax is <0 the thickness is given in % of r.l. which have to be 
// calculated by the method calculatethickness
//
// If Zmin and Zmax have DIFFERENT sign, then the thickness is given in 
// % of r.l....
//

int OraclePixGeoManager::PixelServiceElements(const std::string & type) {
  // FIXME
  /*
  if (dbVersion() < 3) {
    if(isBarrel() ) {
      if(a == "Inside") return (*PixelBarrelGeneral)[0]->getInt("NFRAMEIN");
      if(a == "Outside") return (*PixelBarrelGeneral)[0]->getInt("NFRAMEOUT");
    }
    if(isEndcap() ) {
      if(a == "Inside") return (*PixelEndcapGeneral)[0]->getInt("NFRAMEIN");
      if(a == "Outside") return (*PixelEndcapGeneral)[0]->getInt("NFRAMEOUT");   
    }
    return 0;
  } else {
  */
    // a is ignored. Use frame num to distinguish between inside (<1000) and ouside (>=1000). 
  if(type == "simple") return db()->getTableSize(PixelSimpleService);
  if(type == "barrel") return db()->getTableSize(PixelBarrelService);
  if(type == "endcap") return db()->getTableSize(PixelEndcapService);
  if(type == "envelope") return db()->getTableSize(PixelEnvelopeService);
  return 0;
  //}
}

// Name used when naming G4/Geo volumes 
std::string OraclePixGeoManager::PixelServiceName(const std::string & type, int index) {

  if (useLegacy() || !getPixelServiceRecordTestField("VOLNAME",type,index)) {
    return "";
  } else {
    return getPixelServiceRecordString("VOLNAME",type,index); 
  }
}


// Flag to say whether volume should be described in both positive and
// negative halves.
bool OraclePixGeoManager::PixelServiceZsymm(const std::string & type, int index) {
  if (dbVersion() < 3 || !getPixelServiceRecordTestField("ZSYMM",type,index)) {
    // If ZSYMM not defined use old logic to determine if volume is
    // duplicated in both positive and negative halves.
    double rmin = getPixelServiceRecordDouble("RIN",type,index);
    double zmin = getPixelServiceRecordDouble("ZIN",type,index);
    return (rmin>0 && zmin > 0.000001); 
  } else {
    return getPixelServiceRecordInt("ZSYMM",type,index);
  }
}


double OraclePixGeoManager::PixelServiceRMin(const std::string & type, int index) {
  return std::abs(getPixelServiceRecordDouble("RIN",type,index)) * mmcm();
}

double OraclePixGeoManager::PixelServiceRMax(const std::string & type, int index) {
  double rtmp =  getPixelServiceRecordDouble("ROUT",type,index);
  // If this is negative this is the thickness of the cyl in % of r.l.
  double rmin = PixelServiceRMin(type,index);
  double rmax = 0;
  if(rtmp > 0) {
    rmax = rtmp * mmcm();
  } else {
    string material = PixelServiceMaterial(type,index);
    rmax = rmin + CalculateThickness(rtmp, material);
  }
  return rmax;
}

double OraclePixGeoManager::PixelServiceRMin2(const std::string & type, int index) {
  if (!getPixelServiceRecordTestField("RIN2",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordDouble("RIN2",type,index) * CLHEP::mm;
  }
}

double OraclePixGeoManager::PixelServiceRMax2(const std::string & type, int index) {
  if (!getPixelServiceRecordTestField("ROUT2",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordDouble("ROUT2",type,index) * CLHEP::mm;
  }
}

double OraclePixGeoManager::PixelServiceZMin(const std::string & type, int index) {
  return getPixelServiceRecordDouble("ZIN",type,index) * mmcm();
}

double OraclePixGeoManager::PixelServiceZMax(const std::string & type, int index) {
  double zout =  getPixelServiceRecordDouble("ZOUT",type,index);
  double zmin  =  PixelServiceZMin(type,index);
  double zmax = 0;
  // If zin and zout are opposite sign then zout is the thickness of the cyl in % of r.l.
  if(zmin * zout > -0.000000001) {
    zmax = zout * mmcm();
  } else {
    string material = PixelServiceMaterial(type,index);
    double sign = (zmin > 0) ? 1: -1;
    zmax = zmin + sign*CalculateThickness(zout, material);
  }
  return zmax;
}

double OraclePixGeoManager::PixelServicePhiLoc(const std::string & type, int index) {
  if (!getPixelServiceRecordTestField("PHI",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordDouble("PHI",type,index) * CLHEP::degree; 
  }
}

double OraclePixGeoManager::PixelServiceWidth(const std::string & type, int index) {
  if (!getPixelServiceRecordTestField("WIDTH",type,index)) {
    return 0;
  } else {
    // Can be in degree or CLHEP::mm. Leave it to GeoPixelServices to interpret.    
    return getPixelServiceRecordDouble("WIDTH",type,index); 
  }
}

int OraclePixGeoManager::PixelServiceRepeat(const std::string & type, int index) {
  if (!getPixelServiceRecordTestField("REPEAT",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordInt("REPEAT",type,index); 
  }
}

std::string OraclePixGeoManager::PixelServiceShape(const std::string & type, int index) {
  if (type == "simple") return "TUBE";
  if (!getPixelServiceRecordTestField("SHAPE",type,index)) {
    return "TUBE";
  } else {
    return getPixelServiceRecordString("SHAPE",type,index); 
  }
}


int OraclePixGeoManager::PixelServiceShift(const std::string & type, int index) {
  if (!getPixelServiceRecordTestField("SHIFT",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordInt("SHIFT",type,index); 
  }
}


int OraclePixGeoManager::PixelServiceLD(const std::string & type, int index) {
  return getPixelServiceRecordInt("LAYERNUM",type,index)-1;
}

string OraclePixGeoManager::PixelServiceMaterial(const std::string & type, int index) {

  int imat = 0;
  if (type != "simple") {
    imat = getPixelServiceRecordInt("MATERIAL",type,index);
  }
  std::string materialName;
  if (!imat) {
    materialName = getPixelServiceRecordString("MATERIALNAME",type,index);
  } else {
    // Old
    if(type == "barrel") {
      string mat[11] = {
	"std::Berillia",
	"std::Carbon",
	"pix::Services",
	"pix::Titanium",
	"pix::MatPP11",
	"pix::MatPP12",
	"pix::MatPP13",
	"pix::MatPP14",
	"pix::MatPP15",
	"pix::MatPP16",
	"pix::MatPP17"};
      materialName =  mat[imat-1];
    } 
    if(type == "endcap") {
      string mat[4] = {
	"std::Berillia",
	"std::Carbon",
	"pix::ECServices",
	"pix::Disk"};
      materialName =  mat[imat-1];
    }
  }
  return materialName;
}


int OraclePixGeoManager::PixelServiceFrameNum(const std::string & type, int index) {
  // In older version frame num indicated "inside" or "outside"
  // 0-999:  Inside
  // >=1000: Outside
  // No frame number in simple table.
  if (type == "simple") return index+1;
  int framenum = getPixelServiceRecordInt("FRAMENUM",type,index);
  if (framenum <= 0) return index+1;
  if (dbVersion() < 3) {
    if(type == "barrel") {
      if (index >= m_barrelInFrames) framenum += 1000;
    }
    if(type == "endcap") {
      if (index >= m_endcapInFrames) framenum += 1000;
    }
  }
  return framenum;
  // FIXME
  /*
  if (dbVersion() < 3) return framenum;
  if (type == "Inside" && framenum < 1000) return framenum;
  if (type == "Outside" && framenum >= 1000) return framenum%1000;
  return -1; 
  */
}

// Access child/envelope service parameters
int OraclePixGeoManager::PixelServiceEnvelopeNum(const std::string & type, int index) {

  if (type != "envelope") return 0;

  try{
    int envnum = getPixelServiceRecordInt("ENVNUM",type,index);
    return envnum;
  }
  catch(...)
    {}

  return 0;
}

int OraclePixGeoManager::PixelServiceParentEnvelopeNum(const std::string & type, int index) {

  //  if (type == "envelope") return 0;

  if (type == "envelope"){
    try{
      int envnum = getPixelServiceRecordInt("ENVPARENT",type,index);
      return envnum;
    }
    catch(...)
      {}
  }
  else {

    try{
      int envnum = getPixelServiceRecordInt("ENVELOPE",type,index);
      return envnum;
    }
    catch(...)
      {
      }
  }


  return 0;
}

std::string OraclePixGeoManager::getPixelServiceRecordString(const std::string & name, const std::string & type, int index) {
  IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
  return db()->getString(recordSet, name, index);
}

int OraclePixGeoManager::getPixelServiceRecordInt(const std::string & name, const std::string & type, int index) {
  IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
  return db()->getInt(recordSet, name, index);
}


double OraclePixGeoManager::getPixelServiceRecordDouble(const std::string & name, const std::string & type, int index) {
  IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
  return db()->getDouble(recordSet, name, index);
}

bool OraclePixGeoManager::getPixelServiceRecordTestField(const std::string & name, const std::string & type, int index) {
  try {
    IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
    return db()->testField(recordSet, name, index);
  }
  catch(...){}
  return false;
}


// Returns IRDBRecordset
IRDBRecordset_ptr  OraclePixGeoManager::getPixelServiceRecordset(const std::string & type) {
  // m_barrelInFrames and m_endcapInFrames should be zero in dbVersion >= 3
  IRDBRecordset_ptr recordSet;
  if (type == "simple") {
    recordSet = PixelSimpleService;
  } else if(type == "barrel") {
    recordSet = PixelBarrelService;
    //if(type != "Inside") index += m_barrelInFrames;
  } else if(type == "endcap") {
    recordSet = PixelEndcapService;
    //if(type != "Inside") index += m_endcapInFrames;
  } else if(type == "envelope") {
    recordSet = PixelEnvelopeService;
    //if(type != "Inside") index += m_endcapInFrames;
  } else {
    msg(MSG::ERROR) << "ERROR:  getPixelServiceRecord(), neither Barrel of Endcap selected!" << endmsg;
  }
  return recordSet;
} 

double OraclePixGeoManager::PixelECCablesThickness() 
{
  double tck = db()->getDouble(PixelDisk,"CABLETHICK",currentLD);
  if( tck > 0.) {
    return tck*mmcm();
  } else {    
    std::string matName =  getMaterialName("DiskCable", currentLD);
    return CalculateThickness(tck,matName);
  }
}

int 
OraclePixGeoManager::PixelCableElements()
{
  if (dbVersion() < 3) return m_legacyManager->PixelCableElements();
  return db()->getTableSize(PixelBarrelCable);
}

int 
OraclePixGeoManager::PixelCableLayerNum(int index)
{
  if (dbVersion() < 3) return 0;
  return db()->getInt(PixelBarrelCable,"LAYER",index);
}

int 
OraclePixGeoManager::PixelCableBiStaveNum(int index)
{
  if (dbVersion() < 3) return 0;
  return db()->getInt(PixelBarrelCable,"BISTAVE",index);
}


double 
OraclePixGeoManager::PixelCableZStart(int index)
{
  if (dbVersion() < 3) return m_legacyManager->PixelCableZStart(index);
  return db()->getDouble(PixelBarrelCable,"ZSTART",index) * CLHEP::mm;
}

double 
OraclePixGeoManager::PixelCableZEnd(int index)
{
  if (dbVersion() < 3) return m_legacyManager->PixelCableZEnd(index);
  return db()->getDouble(PixelBarrelCable,"ZEND",index) * CLHEP::mm;
}

double 
OraclePixGeoManager::PixelCableWidth(int index)
{
  if (dbVersion() < 3) return m_legacyManager->PixelCableWidth(index);
  return db()->getDouble(PixelBarrelCable,"WIDTH",index) * CLHEP::mm;
}

double 
OraclePixGeoManager::PixelCableThickness(int index)
{
  if (dbVersion() < 3) return m_legacyManager->PixelCableThickness(index);
  return db()->getDouble(PixelBarrelCable,"THICK",index) * CLHEP::mm;
}

double 
OraclePixGeoManager::PixelCableStackOffset(int index)
{
  if (dbVersion() < 3) return m_legacyManager->PixelCableStackOffset(index);
  return db()->getDouble(PixelBarrelCable,"STACKPOS",index) * CLHEP::mm;
}

double 
OraclePixGeoManager::PixelCableWeight(int index)
{
  if (dbVersion() < 3) return 0;
  return db()->getDouble(PixelBarrelCable,"WEIGHT",index) * CLHEP::g;
}

std::string
OraclePixGeoManager::PixelCableLabel(int index)
{
  if (dbVersion() < 3) return m_legacyManager->PixelCableLabel(index);
  return db()->getString(PixelBarrelCable,"LABEL",index);
}


//
// Version of the Geometry
//

int OraclePixGeoManager::determineDbVersion() {
  // This determines a version depending on various changes in the database;
  int version = 0;

  if (db()->testField(PixelSwitches,"VERSIONNAME") 
      && db()->getString(PixelSwitches,"VERSIONNAME")== "SLHC"){
    version = 4; // SLHC may have TMT table removed. 
    return version;
  }

  if (!(*PixelLayer)[0]->isFieldNull("PHIOFMODULEZERO")) version = 1;
  if (PixelReadout->size() != 0) version = 2;
  if (m_weightTable->size() != 0) version = 3;
  if (PixelTMT->size() != 0) version = 4; // Removed all legacy tables

  return version;
}



std::string OraclePixGeoManager::getMaterialName(const std::string & volumeName, int layerdisk, int typenum) {
  return m_materialMap->getMaterial(layerdisk, typenum, volumeName);
}


void OraclePixGeoManager::addDefaultMaterials() {
  // This is for backward compatibilty. Newer geometies get the
  // gets them from the database.
  m_materialMap->addMaterial(0,0,"Sensor","std::Silicon");
  m_materialMap->addMaterial(0,0,"Chip","pix::Chip");
  m_materialMap->addMaterial(0,0,"Hybrid","pix::Hybrid");
  m_materialMap->addMaterial(0,0,"Omega","pix::MatOmega");
  m_materialMap->addMaterial(0,0,"AlTube","pix::MatAlTube");
  m_materialMap->addMaterial(1,0,"AlTube","pix::MatAlTube");
  m_materialMap->addMaterial(2,0,"AlTube","pix::MatAlTubeFix");
  m_materialMap->addMaterial(0,1,"Fluid","pix::MatCap1");
  m_materialMap->addMaterial(0,2,"Fluid","pix::MatCap2");
  m_materialMap->addMaterial(0,0,"TMT","pix::MatTMT");
  m_materialMap->addMaterial(0,0,"GlueOmegaStave","pix::MatGlue");
  m_materialMap->addMaterial(0,0,"Connector","pix::MatConn");
  m_materialMap->addMaterial(0,0,"PigtailCyl","pix::MatPigtail");
  m_materialMap->addMaterial(0,0,"PigtailFlat","pix::MatPigtail");
  m_materialMap->addMaterial(0,0,"Cable","pix::MatT0");
  m_materialMap->addMaterial(0,0,"DiskCable","pix::ECCables");
  m_materialMap->addMaterial(0,0,"DiskSupport","pix::Disk");
  m_materialMap->addMaterial(0,0,"Frame","std::Carbon");
  m_materialMap->addMaterial(0,0,"EndCone","std::Carbon");
}

std::string OraclePixGeoManager::getLD_Label()
{
  std::ostringstream o;
  if(isBarrel()) {
     if (currentLD == 0) {
       o << "BL";
     } else {
       o << "L" << currentLD;
     }
  } else {
    o << "D" << currentLD;
  }
  return o.str();
}
 
int OraclePixGeoManager::PixelBarrelMajorVersion()
{ 
  return static_cast<int>((*PixelBarrelGeneral)[0]->getDouble("VERSION"));
}

int OraclePixGeoManager::PixelBarrelMinorVersion()
{ 
  return static_cast<int>(((*PixelBarrelGeneral)[0]->getDouble("VERSION") - PixelBarrelMajorVersion())*10 + 0.5);
}

int OraclePixGeoManager::PixelEndcapMajorVersion()
{
  return static_cast<int>((*PixelEndcapGeneral)[0]->getDouble("VERSION"));
}

int OraclePixGeoManager::PixelEndcapMinorVersion() 
{ 
  return static_cast<int>(((*PixelEndcapGeneral)[0]->getDouble("VERSION") - PixelEndcapMajorVersion())*10 + 0.5);
}


std::string OraclePixGeoManager::versionDescription() const
{
  std::string description;
  if (db()->testField(PixelSwitches,"DESCRIPTION")) {
    description = db()->getString(PixelSwitches,"DESCRIPTION");
  }
  return description;
}

std::string OraclePixGeoManager::versionName() const
{
  std::string name;
  if (db()->testField(PixelSwitches,"VERSIONNAME")) {
    name = db()->getString(PixelSwitches,"VERSIONNAME");
  }
  return name;
}

std::string OraclePixGeoManager::versionLayout() const
{
  std::string layout;
  if (db()->testField(PixelSwitches,"LAYOUT")) {
    layout = db()->getString(PixelSwitches,"LAYOUT");
  }
  return layout;
}


double OraclePixGeoManager::PixelRMin() 
{
  if (db()->getTableSize(PixelEnvelope)) {
    double rmin = PixelEnvelopeRMin(0);  
    for (unsigned int i = 1; i < db()->getTableSize(PixelEnvelope); i++) {
      rmin = std::min(rmin, PixelEnvelopeRMin(i));
    } 
    return rmin;
  } else {      
    return db()->getDouble(PixelCommon,"RMIN")*mmcm();
  }
}

double OraclePixGeoManager::PixelRMax() 
{
  if (db()->getTableSize(PixelEnvelope)) {
    double  rmax = PixelEnvelopeRMax(0);  
    for (unsigned int i = 1; i < db()->getTableSize(PixelEnvelope); i++) {
      rmax = std::max(rmax, PixelEnvelopeRMax(i));
    } 
    return rmax;
  } else {      
    return db()->getDouble(PixelCommon,"RMAX")*mmcm();
  }
}

double OraclePixGeoManager::PixelHalfLength() 
{

  if (db()->getTableSize(PixelEnvelope)) {
    // The table should contain only +ve z values.
    return PixelEnvelopeZ(db()->getTableSize(PixelEnvelope) - 1);
  } else {
    return db()->getDouble(PixelCommon,"HALFLENGTH")*mmcm();
  }
}

bool OraclePixGeoManager::PixelSimpleEnvelope() 
{
  // Return true if the envelope can be built as a simple tube.
  // otherwise it will be built as a PCON.
  // True if size is 0 or 1.
  return (!(db()->getTableSize(PixelEnvelope) > 1));
}

unsigned int OraclePixGeoManager::PixelEnvelopeNumPlanes() 
{
  return db()->getTableSize(PixelEnvelope);
}

double OraclePixGeoManager::PixelEnvelopeZ(int i) 
{
  double zmin =  db()->getDouble(PixelEnvelope,"Z",i) * CLHEP::mm;
  if (zmin < 0) msg(MSG::ERROR) << "PixelEnvelope table should only contain +ve z values" << endmsg;
  return std::abs(zmin);
}

double OraclePixGeoManager::PixelEnvelopeRMin(int i) 
{
  return db()->getDouble(PixelEnvelope,"RMIN",i) * CLHEP::mm;
}

double OraclePixGeoManager::PixelEnvelopeRMax(int i) 
{
  return db()->getDouble(PixelEnvelope,"RMAX",i) * CLHEP::mm;
}


int OraclePixGeoManager::PixelBarrelNLayer() 
{
  return db()->getInt(PixelBarrelGeneral,"NLAYER");
}

// PixelBarrelGeneral
double OraclePixGeoManager::PixelBarrelRMin() 
{
  return db()->getDouble(PixelBarrelGeneral,"RMIN")*mmcm();
}

double OraclePixGeoManager::PixelBarrelRMax() 
{
  return db()->getDouble(PixelBarrelGeneral,"RMAX")*mmcm();
}

double OraclePixGeoManager::PixelBarrelHalfLength() 
{
  return db()->getDouble(PixelBarrelGeneral,"HALFLENGTH")*mmcm();
}

// Described in general services for later geometries.
bool OraclePixGeoManager::oldFrame()
{
  if (useLegacy()) return m_legacyManager->oldFrame();
  return false;
} 

// For new geometry a detailed frame is built.
bool OraclePixGeoManager::detailedFrame()
{
  return db()->getTableSize(PixelFrame);
}
  
int OraclePixGeoManager::PixelFrameSections()
{
  return db()->getTableSize(PixelFrame);
}

double OraclePixGeoManager::PixelFrameRMinSide(int sectionIndex)
{
  return db()->getDouble(PixelFrame, "RMINSIDE", sectionIndex) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFrameRMaxSide(int sectionIndex)
{
  return db()->getDouble(PixelFrame, "RMAXSIDE", sectionIndex) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFrameSideWidth(int sectionIndex)
{
  return db()->getDouble(PixelFrame, "SIDEWIDTH", sectionIndex) * CLHEP::mm;
} 
 
double OraclePixGeoManager::PixelFrameZMin(int sectionIndex)
{ 
  return db()->getDouble(PixelFrame, "ZMIN", sectionIndex) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFrameZMax(int sectionIndex)
{ 
  return db()->getDouble(PixelFrame, "ZMAX", sectionIndex) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFramePhiStart(int sectionIndex)
{
  return db()->getDouble(PixelFrame, "PHISTART", sectionIndex) * CLHEP::deg;
}
 
int OraclePixGeoManager::PixelFrameNumSides(int sectionIndex)
{
  return db()->getInt(PixelFrame, "NUMSIDES", sectionIndex);
}

bool OraclePixGeoManager::PixelFrameMirrorSides(int sectionIndex)
{
  return db()->getInt(PixelFrame, "MIRRORSIDES", sectionIndex);
}
									
std::string OraclePixGeoManager::PixelFrameSideMaterial(int sectionIndex)
{
  return db()->getString(PixelFrame, "SIDEMATERIAL", sectionIndex);
}

std::string OraclePixGeoManager::PixelFrameCornerMaterial(int sectionIndex)
{
  return db()->getString(PixelFrame, "CORNERMATERIAL", sectionIndex);
} 

int OraclePixGeoManager::PixelFrameSectionFromIndex(int sectionIndex)
{
  return db()->getInt(PixelFrame,"SECTION",sectionIndex);
}
  
void 
OraclePixGeoManager::makeFrameIndexMap()
{
  if (!m_frameElementMap) {
    m_frameElementMap = new std::map<int,std::vector<int> >;
    for (unsigned int i = 0; i < db()->getTableSize(PixelFrameSect); ++i) {
      int section = db()->getInt(PixelFrameSect,"SECTION",i);
      (*m_frameElementMap)[section].push_back(i);
    }
  }
}

int OraclePixGeoManager::getFrameElementIndex(int sectionIndex, int element)
{
  // make map if it is not already made.
  makeFrameIndexMap();

  int section = PixelFrameSectionFromIndex(sectionIndex);

  int newIndex = -1;
  std::map<int,std::vector<int> >::const_iterator iter = m_frameElementMap->find(section);
  if (iter ==  m_frameElementMap->end()) {
    // Should never be the case as PixelFrameNumSideElements should generally be called first
    msg(MSG::ERROR) << "Frame section " << section << " has no elements." << endmsg;
  } else {
    const std::vector<int> & vec = iter->second;
    if (static_cast<unsigned int>(element) >= vec.size()) {
      msg(MSG::ERROR) << "Element index " << element << " for section " << section << " out of range." << endmsg;
    } else {
      newIndex = vec[element];
    }
  }
  return newIndex;
}


int OraclePixGeoManager::PixelFrameNumSideElements(int sectionIndex)
{ 
  // make map if it is not already made.
  makeFrameIndexMap();

  int section = PixelFrameSectionFromIndex(sectionIndex);
  int numElements = 0;

  std::map<int,std::vector<int> >::const_iterator iter = m_frameElementMap->find(section);
  if (iter ==  m_frameElementMap->end()) {
    msg(MSG::DEBUG) << "Frame section " << section << " has no elements." << endmsg;
  } else {
    numElements = iter->second.size();
  }
  return numElements;
}
 
double OraclePixGeoManager::PixelFrameElementZMin1(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return db()->getDouble(PixelFrameSect, "ZMIN1", index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFrameElementZMin2(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return db()->getDouble(PixelFrameSect, "ZMIN2", index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFrameElementZMax1(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return db()->getDouble(PixelFrameSect, "ZMAX1", index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFrameElementZMax2(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return db()->getDouble(PixelFrameSect, "ZMAX2", index) * CLHEP::mm;
}

int OraclePixGeoManager::PixelStaveIndex(int layer)
{
  if (!slhc() && !ibl()) return 0;
  if (!db()->testField(PixelLayer,"STAVEINDEX",layer)) return 0;
  return db()->getInt(PixelLayer,"STAVEINDEX",layer);
}

int OraclePixGeoManager::PixelStaveLayout()
{
  if (!slhc() && !ibl()) return 0;
  int defaultLayout = 0;
  if (slhc()) defaultLayout = 1;
  int index = PixelStaveIndex(currentLD);

  //  if(currentLD==0)
  //    std::cout<<"OraclePixGeoManager::PixelStaveLayout() : "<<currentLD<<" "<<index<<" "<<db()->getInt(PixelStave,"LAYOUT",index)<<std::endl;


  if (!db()->testField(PixelStave,"LAYOUT",index)) return defaultLayout;
  return db()->getInt(PixelStave,"LAYOUT",index);
}

int OraclePixGeoManager::PixelStaveAxe()
{
  if (!ibl()) return 0;
  int index = PixelStaveIndex(currentLD);

  if (db()->testField(PixelStave,"STAVEAXE",index))
    return db()->getInt(PixelStave,"STAVEAXE",index);
  return 0;
}

double OraclePixGeoManager::PixelLayerRadius() 
{
  double radius = db()->getDouble(PixelLayer,"RLAYER",currentLD)*mmcm();
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "PixelLayerRadius for layer " << currentLD
      << " is " << radius
      << endmsg;
  return radius;
}

double OraclePixGeoManager::PixelLayerGlobalShift() 
{
  if (db()->testField(PixelLayer,"GBLSHIFT",currentLD))
    return db()->getDouble(PixelLayer,"GBLSHIFT",currentLD);
  return 0.;
}


bool OraclePixGeoManager::PixelLayerSupportCylPresent() 
{
  return ((slhc() || ibl())  && db()->testField(PixelLayer,"SUPPORTTHICK",currentLD) &&  PixelLayerSupportThick() > 0);
}

double OraclePixGeoManager::PixelLayerSupportRMin() 
{
  // If rmin provided and valid use it, otherwise calculate from offset from layer radius.
  double rmin = -1;
  if (db()->testField(PixelLayer,"SUPPORTRMIN",currentLD)) {
    rmin = db()->getDouble(PixelLayer,"SUPPORTRMIN",currentLD);
  }
  if (rmin <= 0) {
    rmin = PixelLayerRadius()+PixelLayerSupportROffset();
  }
  return rmin;
}

double OraclePixGeoManager::PixelLayerSupportROffset() 
{
  return db()->getDouble(PixelLayer,"SUPPORTROFFSET",currentLD);
}

double OraclePixGeoManager::PixelLayerSupportThick() 
{
  return db()->getDouble(PixelLayer,"SUPPORTTHICK",currentLD);
}

double OraclePixGeoManager::PixelLadderLength() 
{
  if (useLegacy()) return m_legacyManager->PixelLadderLength(); 
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"ENVLENGTH",index)*CLHEP::mm;
}

double OraclePixGeoManager::PixelLadderWidthClearance() 
{
  if (useLegacy()) return 0.9*CLHEP::mm; 
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"CLEARANCEY",index)*CLHEP::mm;  
}

// Only used if ladder thickness is automatically calculated it, ie ENVTHICK = 0
// SLHC/IBL only
double OraclePixGeoManager::PixelLadderThicknessClearance() 
{
  int index = PixelStaveIndex(currentLD);
  if (db()->testField(PixelStave,"CLEARANCEX",index)) {
    return db()->getDouble(PixelStave,"CLEARANCEX",index)*CLHEP::mm;  
  }
  return 0.1*CLHEP::mm;
}

double OraclePixGeoManager::PixelLadderThickness() 
{
  if (useLegacy()) return m_legacyManager->PixelLadderThickness();  // 2*1.48972 mm
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"ENVTHICK",index)*CLHEP::mm;
}

double OraclePixGeoManager::PixelLadderTilt() 
{
  return db()->getDouble(PixelLayer,"STAVETILT",currentLD)*CLHEP::deg;
}

double OraclePixGeoManager::PixelLadderServicesX() 
{
  if (useLegacy()) return m_legacyManager->PixelLadderServicesX(); // 1.48972 mm
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"SERVICEOFFSETX",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelLadderServicesY() 
{
  if (useLegacy()) return m_legacyManager->PixelLadderServicesY();  // 3mm
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"SERVICEOFFSETY",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelLadderCableOffsetX() 
{
  if (useLegacy()) return m_legacyManager->PixelLadderCableOffsetX(); // 0
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"CABLEOFFSETX",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelLadderCableOffsetY() 
{
  if (useLegacy()) return m_legacyManager->PixelLadderCableOffsetY();  // 4mm
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"CABLEOFFSETY",index) * CLHEP::mm;
}

// SLHC/IBL only
double OraclePixGeoManager::PixelLadderSupportThickness() 
{
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"SUPPORTTHICK",index) * CLHEP::mm;
}

// SLHC/IBL only
double OraclePixGeoManager::PixelLadderSupportWidth() 
{
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"SUPPORTWIDTH",index) * CLHEP::mm;
}





// SLHC/IBL only
double OraclePixGeoManager::PixelLadderBentStaveAngle() 
{
  if (!db()->testFieldTxt(PixelConicalStave,"BENTSTAVEANGLE")) return 0;
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelConicalStave,"BENTSTAVEANGLE",index);
}

// SLHC/IBL only
int OraclePixGeoManager::PixelBentStaveNModule() 
{
  if (!db()->testFieldTxt(PixelConicalStave,"BENTSTAVENMODULE")) return 0;
  int index = PixelStaveIndex(currentLD);
  return db()->getInt(PixelConicalStave,"BENTSTAVENMODULE",index);
}

double OraclePixGeoManager::PixelLadderModuleDeltaZ()
{
  int index = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"MODULEDZ",index);
}

// SLHC/IBL only
double OraclePixGeoManager::PixelLadderSupportLength() 
{
  int index = PixelStaveIndex(currentLD);
  if (db()->testField(PixelStave,"SUPPORTHLENGTH",index)) {
    double halflength = db()->getDouble(PixelStave,"SUPPORTHLENGTH",index) * CLHEP::mm;
    if (halflength > 0)  return 2 * halflength;
  } 
  double safety = 0.01*CLHEP::mm;
  return PixelLadderLength() - safety;
}

// IBL detailed stave support only

HepGeom::Point3D<double> OraclePixGeoManager::IBLStaveRotationAxis() 
{
  // set layer to 0  (in order to read read IBL data)
  int currentLD_tmp = currentLD;
  currentLD = 0;

  double boardThick = PixelBoardThickness();
  double chipThick = PixelChipThickness();
  double chipGap = PixelChipGap();

  double xCenterCoolingPipe = boardThick*.5+chipThick+chipGap+                // from sensor sensor to plate
    IBLStaveFacePlateThickness() + IBLStaveFacePlateGreaseThickness() +       // plate thickness (grease + plate)
    IBLStaveTubeMiddlePos();                                                  // from plate to colling pipe center
  double yCenterCoolingPipe = IBLStaveMechanicalStaveOffset();
  HepGeom::Point3D<double> centerCoolingPipe(xCenterCoolingPipe, yCenterCoolingPipe, 0.);

  currentLD = currentLD_tmp;  
  return centerCoolingPipe;
}


double OraclePixGeoManager::IBLStaveRadius() 
{
  // set layer to 0  (in order to read read IBL data)
  int currentLD_tmp = currentLD;
  currentLD = 0;

  //  Point that defines the center of the cooling pipe
  HepGeom::Point3D<double> centerCoolingPipe_inv = -IBLStaveRotationAxis();
  HepGeom::Point3D<double> origin(0.,0.,0.);
  double layerRadius = PixelLayerRadius();
  double ladderTilt  = PixelLadderTilt();
  
  // Transforms
  HepGeom::Transform3D staveTrf = HepGeom::RotateZ3D(ladderTilt)*HepGeom::Translate3D(centerCoolingPipe_inv);
  HepGeom::Point3D<double> sensorPos = staveTrf*origin;
  
  double yPos = sensorPos.y();
  HepGeom::Point3D<double> sensorPos_layer(sqrt(layerRadius*layerRadius-yPos*yPos),yPos,0.);
  
  double staveRadius = sensorPos_layer.x()-sensorPos.x();

  currentLD = currentLD_tmp;  
  return staveRadius;
}


double OraclePixGeoManager::IBLStaveFacePlateThickness() 
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"FACEPLATETHICK",index)) {
    double thickness = db()->getDouble(PixelIBLStave,"FACEPLATETHICK",index) * CLHEP::mm;
    if (thickness > 0)  return thickness ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveMechanicalStaveWidth()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"STAVEWIDTH",index)) {
    double value = db()->getDouble(PixelIBLStave,"STAVEWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveMechanicalStaveEndBlockLength()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"ENDBLOCKLENGTH",index)) {
    double value = db()->getDouble(PixelIBLStave,"ENDBLOCKLENGTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveMechanicalStaveEndBlockFixPoint()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"ENDBLOCKFIXINGPOS",index)) {
    double value = db()->getDouble(PixelIBLStave,"ENDBLOCKFIXINGPOS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveMechanicalStaveEndBlockOmegaOverlap()
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (db()->testField(PixelIBLStave,"ENDBLOCKOMEGAOVERLAP",index)) {
      double value = db()->getDouble(PixelIBLStave,"ENDBLOCKOMEGAOVERLAP",index) * CLHEP::mm;
      return value ;
    } 
    return 0.0;
  }
  catch(...){}
  return 0.;
}

double OraclePixGeoManager::IBLStaveLength()
{
 // try and catch (param availbale only if db tag > IBL-03-00-00)
  try
    {
      int index=0;
      if (db()->testField(PixelIBLStave,"STAVELENGTH",index)) {
	double value = db()->getDouble(PixelIBLStave,"STAVELENGTH",index) * CLHEP::mm;
	return value ;
      } 
    }
  catch(...)
    {
      // FIXME : patch for initial IBL geometry (SES)
      //           IBL stave length not eqal to other stave length 
    }  
  
  return 748.0 * CLHEP::mm;  
}

double OraclePixGeoManager:: IBLStaveMechanicalStaveOffset(bool isModule3D)
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (!isModule3D&&db()->testField(PixelIBLStave,"MODULELATERALOFFSET",index)) {
    double value = db()->getDouble(PixelIBLStave,"MODULELATERALOFFSET",index) * CLHEP::mm;
    return value ;
  } 
  if (isModule3D&&db()->testField(PixelIBLStave,"MODULELATERALOFFSET3D",index)) {
    double value = db()->getDouble(PixelIBLStave,"MODULELATERALOFFSET3D",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveMechanicalStaveModuleOffset()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"STAVETOMODULEGAP",index)) {
    double value = db()->getDouble(PixelIBLStave,"STAVETOMODULEGAP",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveTubeOuterDiameter()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"TUBEOUTERDIAM",index)) {
    double value = db()->getDouble(PixelIBLStave,"TUBEOUTERDIAM",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveTubeInnerDiameter()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"TUBEINNERDIAM",index)) {
    double value = db()->getDouble(PixelIBLStave,"TUBEINNERDIAM",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveTubeMiddlePos()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"TUBEMIDDLEPOS",index)) {
    double value = db()->getDouble(PixelIBLStave,"TUBEMIDDLEPOS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveFlexLayerThickness()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"FLEXLAYERTHICK",index)) {
    double value = db()->getDouble(PixelIBLStave,"FLEXLAYERTHICK",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveFlexBaseThickness()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"FLEXBASETHICK",index)) {
    double value = db()->getDouble(PixelIBLStave,"FLEXBASETHICK",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveFlexWidth()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"FLEXWIDTH",index)) {
    double value = db()->getDouble(PixelIBLStave,"FLEXWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLStaveFlexOffset()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"FLEXOFFSET",index)) {
    double value = db()->getDouble(PixelIBLStave,"FLEXOFFSET",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


double OraclePixGeoManager::IBLStaveOmegaThickness()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGATHICK",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGATHICK",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLStaveOmegaEndCenterX()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGAENDCENTERX",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGAENDCENTERX",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double OraclePixGeoManager::IBLStaveOmegaEndCenterY()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGAENDCENTERY",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGAENDCENTERY",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double OraclePixGeoManager::IBLStaveOmegaEndRadius()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGAENDRADIUS",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGAENDRADIUS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double OraclePixGeoManager::IBLStaveOmegaEndAngle()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGAENDANGLE",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGAENDANGLE",index) * CLHEP::deg;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLStaveOmegaMidCenterX()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGAMIDCENTERX",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGAMIDCENTERX",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLStaveOmegaMidRadius()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGAMIDRADIUS",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGAMIDRADIUS",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}
double OraclePixGeoManager::IBLStaveOmegaMidAngle()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"OMEGAOPENINGANGLE",index)) {
    double value = db()->getDouble(PixelIBLStave,"OMEGAOPENINGANGLE",index) * CLHEP::deg;
    return value ;
  } 
  return 0.0;
}

int OraclePixGeoManager::IBLStaveModuleNumber_AllPlanar()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"NMODULE",index)) {
    int value = db()->getInt(PixelIBLStave,"NMODULE",index);
    if (value > 0)  return value ;
  } 
  return 0;
}

int OraclePixGeoManager::IBLStaveModuleNumber()
{
  return m_PlanarModuleNumber+m_3DModuleNumber;

}

double OraclePixGeoManager::IBLStaveModuleGap()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"MODULEGAP",index)) {
    double value = db()->getDouble(PixelIBLStave,"MODULEGAP",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

int OraclePixGeoManager::IBLStaveModuleType() 
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLStave,"MODULETYPE",index)) {
    int value = db()->getInt(PixelIBLStave,"MODULETYPE",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0;
}

double OraclePixGeoManager::IBLStaveFacePlateGreaseThickness()
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (db()->testField(PixelIBLGlueGrease,"FACEPLATEGREASETHICK",index)) {
      double value = db()->getDouble(PixelIBLGlueGrease,"FACEPLATEGREASETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...){}
  return 0.;
}

double OraclePixGeoManager::IBLStaveFacePlateGlueThickness()
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (db()->testField(PixelIBLGlueGrease,"FACEPLATEGLUETHICK",index)) {
      double value = db()->getDouble(PixelIBLGlueGrease,"FACEPLATEGLUETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...) {}
  return 0.;
}

double OraclePixGeoManager::IBLStaveTubeGlueThickness()
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (db()->testField(PixelIBLGlueGrease,"TUBEGLUETHICK",index)) {
      double value = db()->getDouble(PixelIBLGlueGrease,"TUBEGLUETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...) {}
  return 0.;
}

double OraclePixGeoManager::IBLStaveOmegaGlueThickness()
{
  // try and catch (param availbale only if db tag > IBL-03-00-00)
  try{
    int index=0;
    if (db()->testField(PixelIBLGlueGrease,"OMEGAGLUETHICK",index)) {
      double value = db()->getDouble(PixelIBLGlueGrease,"OMEGAGLUETHICK",index) * CLHEP::mm;
      return value ;
    }
    return 0.;
  }
  catch(...){}
  return 0.;
}


double OraclePixGeoManager:: IBLSupportRingWidth()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLSupport,"STAVERINGWIDTH",index)) {
    double value = db()->getDouble(PixelIBLSupport,"STAVERINGWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLSupportRingInnerRadius()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLSupport,"STAVERINGINNERRADIUS",index)) {
    double value = db()->getDouble(PixelIBLSupport,"STAVERINGINNERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLSupportRingOuterRadius()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLSupport,"STAVERINGOUTERRADIUS",index)) {
    double value = db()->getDouble(PixelIBLSupport,"STAVERINGOUTERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}


double OraclePixGeoManager:: IBLSupportMechanicalStaveRingFixPoint()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLSupport,"STAVERINGFIXINGPOS",index)) {
    double value = db()->getDouble(PixelIBLSupport,"STAVERINGFIXINGPOS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLSupportMidRingWidth()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLSupport,"STAVEMIDRINGWIDTH",index)) {
    double value = db()->getDouble(PixelIBLSupport,"STAVEMIDRINGWIDTH",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLSupportMidRingInnerRadius()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLSupport,"STAVEMIDRINGINNERRADIUS",index)) {
    double value = db()->getDouble(PixelIBLSupport,"STAVEMIDRINGINNERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value;
  } 
  return 0.0;
}

double OraclePixGeoManager:: IBLSupportMidRingOuterRadius()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLSupport,"STAVEMIDRINGOUTERRADIUS",index)) {
    double value = db()->getDouble(PixelIBLSupport,"STAVEMIDRINGOUTERRADIUS",index) * CLHEP::mm;
    if (value > 0)  return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLFlexMiddleGap()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,"FLEXMIDGAP",index)) {
    double value = db()->getDouble(PixelIBLFlex,"FLEXMIDGAP",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

bool OraclePixGeoManager::IBLFlexAndWingDefined()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,"FLEXMIDGAP",index)) return true;
  return false;
}


double OraclePixGeoManager::IBLFlexDoglegLength()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,"FLEXDOGLEGLENGTH",index)) {
    double value = db()->getDouble(PixelIBLFlex,"FLEXDOGLEGLENGTH",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


double OraclePixGeoManager::IBLStaveFlexWingWidth()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,"FLEXWINGWIDTH",index)) {
    double value = db()->getDouble(PixelIBLFlex,"FLEXWINGWIDTH",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLStaveFlexWingThick()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,"FLEXWINGTHICK",index)) {
    double value = db()->getDouble(PixelIBLFlex,"FLEXWINGTHICK",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLFlexDoglegRatio()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,"FLEXDOGLEGRATIO",index)) {
    double value = db()->getDouble(PixelIBLFlex,"FLEXDOGLEGRATIO",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLFlexDoglegHeight(int iHeight)
{
  std::ostringstream lname;
  lname << "FLEXDOGLEGHEIGHT"<<iHeight;

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,lname.str(),index)) {
    double value = db()->getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLFlexDoglegDY()
{
  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,"FLEXDOGLEGDY",index)) {
    double value = db()->getDouble(PixelIBLFlex,"FLEXDOGLEGDY",index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLFlexPP0Z(int iPos)
{
  std::ostringstream lname;
  lname << "FLEXPP0_Z"<<iPos;

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,lname.str(),index)) {
    double value = db()->getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


double OraclePixGeoManager::IBLFlexPP0Rmin(int iPos)
{
  std::ostringstream lname;
  lname << "FLEXPP0_S"<<iPos<<"RMIN";

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,lname.str(),index)) {
    double value = db()->getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}

double OraclePixGeoManager::IBLFlexPP0Rmax(int iPos)
{
  std::ostringstream lname;
  lname << "FLEXPP0_S"<<iPos<<"RMAX";

  //  int index = PixelStaveIndex(currentLD);
  int index=0;
  if (db()->testField(PixelIBLFlex,lname.str(),index)) {
    double value = db()->getDouble(PixelIBLFlex,lname.str(),index) * CLHEP::mm;
    return value ;
  } 
  return 0.0;
}


std::string OraclePixGeoManager::IBLFlexMaterial(int iPos, const std::string& flexType)
{

  int nbMaterial=db()->getTableSize(PixelIBLFlexMaterial);
  int cmptType=0;

  for(int index=0; index<nbMaterial; index++)
    {
      std::string flexTypeIdx = db()->getString(PixelIBLFlexMaterial,"TYPE",index);
      if(flexTypeIdx.compare(flexType)==0)
	{
	  cmptType++;
	  if(iPos==cmptType){
	    std::string matTypeIdx = db()->getString(PixelIBLFlexMaterial,"MATERIALNAME",index);
	    return matTypeIdx;
	  }
	}
    }
  return std::string("noMat");
      
}


double OraclePixGeoManager:: IBLServiceGetMinRadialPosition(const std::string& srvName, const std::string& srvType, 
							    double srvZmin, double srvZmax)
{
  
  double rmin=99999.;
  
  int numServices =  PixelServiceElements(srvType);
  int nbSrv=0;
  for(int ii = 0; ii < numServices; ii++) {
    // Retrieve/calculate the parameters for the volume.
    //
    std::string name;
    if(srvType=="simple")
      name=db()->getString(PixelSimpleService,"NAME",ii);
    else
      name=PixelServiceName(srvType,ii);

    if(name.find(srvName)!=std::string::npos){
      double zmin, zmax, r;
      int symm;
      if(srvType=="simple"){
	zmin=db()->getDouble(PixelSimpleService,"ZMIN",ii)*CLHEP::mm;
	zmax=db()->getDouble(PixelSimpleService,"ZMAX",ii)*CLHEP::mm;
	symm=db()->getInt(PixelSimpleService,"ZSYMM",ii);
	r=db()->getDouble(PixelSimpleService,"RMAX",ii)*CLHEP::mm;
      }
      else {
	zmin=PixelServiceZMin(srvType, ii);
	zmax=PixelServiceZMax(srvType, ii);
	symm=PixelServiceZsymm(srvType, ii);
	r=PixelServiceRMin(srvType, ii);
      }

      bool bZintervalle = false;
      if( (srvZmin-zmin)*(srvZmin-zmax)<0 || (srvZmax-zmin)*(srvZmax-zmax)<0 ) bZintervalle=true; 
      if( symm==1 && ((-srvZmin-zmin)*(-srvZmin-zmax)<0 || (-srvZmax-zmin)*(-srvZmax-zmax)<0) ) bZintervalle=true; 
      
      if(bZintervalle){
	if(r<rmin) rmin=r;
	nbSrv++;
      }
    }
  }
  
 if(nbSrv<1)return -1;
  return rmin;

}

double OraclePixGeoManager:: IBLServiceGetMaxRadialPosition(const std::string& srvName, const std::string& srvType, 
							    double srvZmin, double srvZmax)
{
  
  double rmax=-1.;
  int numServices =  PixelServiceElements(srvType);

  int nbSrv=0;
  for(int ii = 0; ii < numServices; ii++) {
    // Retrieve/calculate the parameters for the volume.
    //
    std::string name;
    if(srvType=="simple")
      name=db()->getString(PixelSimpleService,"NAME",ii);
    else
      name=PixelServiceName(srvType,ii);

    if(name.find(srvName)!=std::string::npos){

      double zmin, zmax, r;
      int symm;
      if(srvType=="simple"){
	zmin=db()->getDouble(PixelSimpleService,"ZMIN",ii)*CLHEP::mm;
	zmax=db()->getDouble(PixelSimpleService,"ZMAX",ii)*CLHEP::mm;
	symm=db()->getInt(PixelSimpleService,"ZSYMM",ii);
	r=db()->getDouble(PixelSimpleService,"RMAX",ii)*CLHEP::mm;
      }
      else {
	zmin=PixelServiceZMin(srvType, ii);
	zmax=PixelServiceZMax(srvType, ii);
	symm=PixelServiceZsymm(srvType, ii);
	r=PixelServiceRMax(srvType, ii);
      }
      
      bool bZintervalle = false;
      if( (srvZmin-zmin)*(srvZmin-zmax)<0 || (srvZmax-zmin)*(srvZmax-zmax)<0 ) bZintervalle=true; 
      if( symm==1 && ((-srvZmin-zmin)*(-srvZmin-zmax)<0 || (-srvZmax-zmin)*(-srvZmax-zmax)<0) ) bZintervalle=true; 
      
      if(bZintervalle && r>rmax){
	rmax=r;
	nbSrv++;
      }
    }
  }
  
 if(nbSrv<1)return -1;
  return rmax;

}

int OraclePixGeoManager::PixelBiStaveType(int layer, int phi)
{
  if (m_staveTypeTable->size() == 0) return phi % 2; 
  return m_pixelStaveTypes->getBiStaveType(layer, phi) % 2;
}

int OraclePixGeoManager::NPixelSectors() 
{
  return db()->getInt(PixelLayer,"NSECTORS",currentLD);
}

double OraclePixGeoManager::PhiOfModuleZero()
{
  // For backward compatibilty first module is at 1/2 a module division
  if (!db()->testField(PixelLayer,"PHIOFMODULEZERO",currentLD)){
    if(NPixelSectors()>0) return 180.0*CLHEP::degree/NPixelSectors();
    return 0.;
  } else { 
    return db()->getDouble(PixelLayer,"PHIOFMODULEZERO",currentLD) * CLHEP::degree;
  }
}


int OraclePixGeoManager::PixelNModule() 
{
  int staveIndex = PixelStaveIndex(currentLD);
  if(ibl() && PixelStaveLayout()>3 && PixelStaveLayout()<7 && currentLD==0)
    return IBLStaveModuleNumber();
  else
    return db()->getInt(PixelStave,"NMODULE",staveIndex);

}

double OraclePixGeoManager::PixelModuleAngle() 
{
  int staveIndex = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"MODULETILT",staveIndex)*CLHEP::deg;
}

double OraclePixGeoManager::PixelModuleDrDistance() 
{
  int staveIndex = PixelStaveIndex(currentLD);
  return db()->getDouble(PixelStave,"CENTRMODULESHIFT",staveIndex)*mmcm();
}

double OraclePixGeoManager::PixelModuleZPosition(int etaModule) 
{
  // ZPOSTYPE 0. Means equi-distant modules.
  // ZPOSTYPE != 0. Means tabulated z positions.
  int staveIndex = PixelStaveIndex(currentLD);
  int zPosType = 0;
  if ((slhc() || ibl()) &&  db()->testField(PixelStave,"ZPOSTYPE",staveIndex)) {
    zPosType = db()->getInt(PixelStave,"ZPOSTYPE",staveIndex);
  }
  if (zPosType) {
    // Z positions from table
    return PixelModuleZPositionTabulated(etaModule, zPosType);
  } else {
   // Equi-distant modules
    int moduleIndex =  PixelModuleIndexFromEta(etaModule);  
    return db()->getDouble(PixelStave,"MODULEDZ",staveIndex)*mmcm() * (moduleIndex - 0.5*(PixelNModule()-1));
  }
}

double OraclePixGeoManager::PixelModuleZPositionTabulated(int etaModule, int type) 
{ 
  if (!m_zPositionMap) {
    m_zPositionMap = new InDetDD::PairIndexMap;
    for (unsigned int indexTmp = 0; indexTmp < db()->getTableSize(PixelStaveZ); ++indexTmp) {
      int eta_module = db()->getInt(PixelStaveZ,"ETAMODULE",indexTmp);
      int type_tmp       = db()->getInt(PixelStaveZ,"TYPE",indexTmp);
      m_zPositionMap->add(type_tmp,eta_module,indexTmp);
    }
  }
  int index = m_zPositionMap->find(type, etaModule);
  if (index < 0) {
    msg(MSG::ERROR) << "Z position not found for etaModule,type =  " << etaModule << ", " << type << endmsg;
    return 0;
  }
  return db()->getDouble(PixelStaveZ,"ZPOS",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelModuleShiftFlag(int etaModule) 
{
  if (centerModule(etaModule)) return 1;
  return 0.;
}

double OraclePixGeoManager::PixelModuleStaggerDistance()
{
  int staveIndex = PixelStaveIndex(currentLD);
  if (!(slhc() || ibl()) || !db()->testField(PixelStave,"STAGGERDIST",staveIndex)) return 0; 
  return db()->getDouble(PixelStave,"STAGGERDIST",staveIndex) * CLHEP::mm;
}

int OraclePixGeoManager::PixelModuleStaggerSign(int etaModule)
{
  int staveIndex = PixelStaveIndex(currentLD);
  if (!(slhc() || ibl()) || !db()->testField(PixelStave,"FIRSTSTAGGER",staveIndex)) return 0;  
  // FIRSTSTAGGER refers to whether the first module (lowest etavalue) is staggered up (+1) or down(-1)
  int firstStagger =  db()->getInt(PixelStave,"FIRSTSTAGGER",staveIndex);
  int moduleIndex = PixelModuleIndexFromEta(etaModule);
  return firstStagger * (moduleIndex%2 ? -1 : 1);
}

bool OraclePixGeoManager::allowSkipEtaZero()
{
  bool allowSkip = true;
  if (ibl() || slhc()){
    int staveIndex = PixelStaveIndex(currentLD);
    if (db()->testField(PixelStave,"NOSKIPZERO",staveIndex)) {
      if (db()->getInt(PixelStave,"NOSKIPZERO",staveIndex)) allowSkip = false;
    }
  }
  return allowSkip;
}

bool OraclePixGeoManager::centerModule(int etaModule) 
{
  // There is only a center module if there are an odd number
  // of modules. In that case it will be etaModule = 0.
  return (etaModule == 0 && PixelNModule()%2);
}

int OraclePixGeoManager::PixelModuleEtaFromIndex(int index) 
{
  int nModules = PixelNModule();
  int etaModule = index-nModules/2;
  // If even number of modules skip eta = 0.
  // For IBL or SLHC this behaviour can be disabled.
  if (allowSkipEtaZero() && (etaModule >= 0) && !(nModules%2)) etaModule++; 
  return etaModule;
}

int OraclePixGeoManager::PixelModuleIndexFromEta(int etaModule) 
{
  int nModules = PixelNModule();  
  int index = etaModule + nModules/2;
  // If even number of modules skip eta = 0.
  // For IBL or SLHC this behaviour can be disabled.
  if (allowSkipEtaZero() && (etaModule >= 0) && (nModules%2 == 0)) index--; 
  return index;
}


double OraclePixGeoManager::PixelModuleAngleSign(int etaModule) 
{
  if (centerModule(etaModule)) return 0;
  if(etaModule < 0) return 1.;
  return -1.;
}

int OraclePixGeoManager::PixelEndcapNDisk() 
{
  return db()->getInt(PixelEndcapGeneral,"NDISK");
}

// Endcap container
double  OraclePixGeoManager::PixelEndcapRMin()
{
  return db()->getDouble(PixelEndcapGeneral,"RMIN")*mmcm();
}

double  OraclePixGeoManager::PixelEndcapRMax() 
{
  return db()->getDouble(PixelEndcapGeneral,"RMAX")*mmcm();
}

double  OraclePixGeoManager::PixelEndcapZMin() 
{
  return db()->getDouble(PixelEndcapGeneral,"ZMIN")*mmcm();
}

double  OraclePixGeoManager::PixelEndcapZMax()
{
  return db()->getDouble(PixelEndcapGeneral,"ZMAX")*mmcm();
}

int OraclePixGeoManager::PixelEndcapNSupportFrames()
{
   // Obsolete - retus 0 in recent versions
 return (int) db()->getDouble(PixelEndcapGeneral,"NFRAME");
}

// Endcap Inner 
double  OraclePixGeoManager::PixelDiskZPosition() 
{
  return db()->getDouble(PixelDisk,"ZDISK",currentLD)*mmcm();
}

// See new SLHC/LHC version below
//double  OraclePixGeoManager::PixelDiskRMin()
//{
//  return db()->getDouble(PixelDisk,"RIDISK",currentLD)*mmcm();
//}

double OraclePixGeoManager::PixelECSiDz1() 
{
  return db()->getDouble(PixelDisk,"DZCOUNTER",currentLD)*mmcm();
}

double OraclePixGeoManager::PixelECSiDz2() 
{
  return PixelECSiDz1();
}

int OraclePixGeoManager::PixelECNSectors1()
{
  return db()->getInt(PixelDisk,"NMODULE",currentLD);
}

int OraclePixGeoManager::PixelECNSectors2() 
{
  return PixelECNSectors1();
}

// Endcap Cables
double OraclePixGeoManager::PixelECCablesRMin()
{
  return db()->getDouble(PixelDisk,"RMINCABLE",currentLD)*mmcm();
}

double OraclePixGeoManager::PixelECCablesRMax()
{
  return db()->getDouble(PixelDisk,"RMAXCABLE",currentLD)*mmcm();
}


double OraclePixGeoManager::PixelECCablesDistance()
{
  return db()->getDouble(PixelDisk,"ZCABLE",currentLD)*mmcm();
}

//
/// TMT
//
int OraclePixGeoManager::PixelTMTNumParts()
{
  if (useLegacy()) return m_legacyManager->PixelTMTNumParts();
  return db()->getTableSize(PixelTMT);
}

double OraclePixGeoManager::PixelTMTWidthX1(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTWidthX1(iPart);
  return db()->getDouble(PixelTMT,"WIDTHX1",iPart) * CLHEP::mm;
}

double OraclePixGeoManager::PixelTMTWidthX2(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTWidthX2(iPart);
  return db()->getDouble(PixelTMT,"WIDTHX2",iPart) * CLHEP::mm;
}

double OraclePixGeoManager::PixelTMTWidthY(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTWidthY(iPart);
  return db()->getDouble(PixelTMT,"WIDTHY",iPart) * CLHEP::mm;
}

double OraclePixGeoManager::PixelTMTBaseX1(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTBaseX1(iPart);
  return db()->getDouble(PixelTMT,"BASEX1",iPart) * CLHEP::mm;
}

double OraclePixGeoManager::PixelTMTBaseX2(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTBaseX2(iPart);
  return db()->getDouble(PixelTMT,"BASEX2",iPart) * CLHEP::mm;
}

double OraclePixGeoManager::PixelTMTPosY(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTPosY(iPart);
  return db()->getDouble(PixelTMT,"Y",iPart) * CLHEP::mm;
}

double OraclePixGeoManager::PixelTMTPosZ1(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTPosZ1(iPart);
  return db()->getDouble(PixelTMT,"Z1",iPart) * CLHEP::mm;
}

double OraclePixGeoManager::PixelTMTPosZ2(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTPosZ2(iPart);
  return db()->getDouble(PixelTMT,"Z2",iPart) * CLHEP::mm;
}

bool OraclePixGeoManager::PixelTMTPerModule(int iPart)
{
  if (useLegacy()) return m_legacyManager->PixelTMTPerModule(iPart);
  return db()->getInt(PixelTMT,"PERMODULE",iPart);
}

//
// Omega parameters
//
double OraclePixGeoManager::PixelOmegaUpperBendX()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaUpperBendX();
  return db()->getDouble(PixelOmega,"UPPERBENDX") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaUpperBendY()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaUpperBendY();
  return db()->getDouble(PixelOmega,"UPPERBENDY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaUpperBendRadius()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaUpperBendRadius();
  return db()->getDouble(PixelOmega,"UPPERBENDR") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaLowerBendX()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaLowerBendX();
  return db()->getDouble(PixelOmega,"LOWERBENDX") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaLowerBendY()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaLowerBendY();
  return db()->getDouble(PixelOmega,"LOWERBENDY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaLowerBendRadius()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaLowerBendRadius();
  return db()->getDouble(PixelOmega,"LOWERBENDR") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaWallThickness()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaWallThickness();
  return db()->getDouble(PixelOmega,"THICK") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaLength()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaLength();
  return db()->getDouble(PixelOmega,"LENGTH") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaStartY()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaStartY();
  return db()->getDouble(PixelOmega,"STARTY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaEndY()
{
  if (useLegacy()) return m_legacyManager->PixelOmegaEndY();
  return db()->getDouble(PixelOmega,"ENDY") * CLHEP::mm;
}

//
// Al Tube
//

double OraclePixGeoManager::PixelAlTubeUpperBendX()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeUpperBendX();
  return db()->getDouble(PixelAlTube,"UPPERBENDX") * CLHEP::mm;
}

double OraclePixGeoManager::PixelAlTubeUpperBendY()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeUpperBendY();
  return db()->getDouble(PixelAlTube,"UPPERBENDY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelAlTubeUpperBendRadius()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeUpperBendRadius();
  return db()->getDouble(PixelAlTube,"UPPERBENDR") * CLHEP::mm;
}

double OraclePixGeoManager::PixelAlTubeLowerBendX()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeLowerBendX();
  return db()->getDouble(PixelAlTube,"LOWERBENDX") * CLHEP::mm;
}

double OraclePixGeoManager::PixelAlTubeLowerBendY()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeLowerBendY();
  return db()->getDouble(PixelAlTube,"LOWERBENDY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelAlTubeLowerBendRadius()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeLowerBendRadius();
  return db()->getDouble(PixelAlTube,"LOWERBENDR") * CLHEP::mm;
}

double OraclePixGeoManager::PixelAlTubeWallThickness()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeWallThickness();
  return db()->getDouble(PixelAlTube,"THICK") * CLHEP::mm;
}

double OraclePixGeoManager::PixelAlTubeLength()
{
  if (useLegacy()) return m_legacyManager->PixelAlTubeLength();
  return db()->getDouble(PixelAlTube,"LENGTH") * CLHEP::mm;
}

//
// Glue
// 

int OraclePixGeoManager::PixelNumOmegaGlueElements()
{
  if (useLegacy()) return m_legacyManager->PixelNumOmegaGlueElements();
  return db()->getTableSize(PixelOmegaGlue);
}

double OraclePixGeoManager::PixelOmegaGlueStartX(int index)
{
  if (useLegacy()) return m_legacyManager->PixelOmegaGlueStartX(index);
  return db()->getDouble(PixelOmegaGlue,"STARTX",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaGlueThickness(int index)
{
  if (useLegacy()) return m_legacyManager->PixelOmegaGlueThickness(index);
  return db()->getDouble(PixelOmegaGlue,"THICK",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaGlueStartY(int index)
{
  if (useLegacy()) return m_legacyManager->PixelOmegaGlueStartY(index);
  return db()->getDouble(PixelOmegaGlue,"STARTY",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaGlueEndY(int index)
{
  if (useLegacy()) return m_legacyManager->PixelOmegaGlueEndY(index);
  return db()->getDouble(PixelOmegaGlue,"ENDY",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaGlueLength(int index)
{
  if (useLegacy()) return m_legacyManager->PixelOmegaGlueLength(index);
  return db()->getDouble(PixelOmegaGlue,"LENGTH",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelOmegaGluePosZ(int index)
{
  if (useLegacy()) return m_legacyManager->PixelOmegaGluePosZ(index);
  return db()->getDouble(PixelOmegaGlue,"Z",index) * CLHEP::mm;
}

int OraclePixGeoManager::PixelOmegaGlueTypeNum(int index)
{
  if (useLegacy()) return m_legacyManager->PixelOmegaGlueTypeNum(index);
  return db()->getInt(PixelOmegaGlue,"TYPENUM",index);
}


//
// Fluid
// 
double OraclePixGeoManager::PixelFluidZ1(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidZ1(index);
  return db()->getDouble(PixelFluid,"Z1",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFluidZ2(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidZ2(index);
  return db()->getDouble(PixelFluid,"Z2",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFluidThick1(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidThick1(index);
  return db()->getDouble(PixelFluid,"THICK1",index) * CLHEP::mm;
}


double OraclePixGeoManager::PixelFluidThick2(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidThick2(index);
  return db()->getDouble(PixelFluid,"THICK2",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFluidWidth(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidWidth(index);
  return db()->getDouble(PixelFluid,"WIDTH",index) * CLHEP::mm;
}


double OraclePixGeoManager::PixelFluidX(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidX(index);
  return db()->getDouble(PixelFluid,"X",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelFluidY(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidY(index);
  return db()->getDouble(PixelFluid,"Y",index) * CLHEP::mm;
}

int OraclePixGeoManager::PixelFluidType(int index)
{
  if (useLegacy()) return m_legacyManager->PixelFluidType(index);
  return db()->getInt(PixelFluid,"TYPE",index);
}

int OraclePixGeoManager::PixelFluidNumTypes()
{
  if (useLegacy()) return m_legacyManager->PixelFluidNumTypes();
  return db()->getTableSize(PixelFluid);
}

int OraclePixGeoManager::PixelFluidIndex(int type)
{
  for (int i = 0; i < PixelFluidNumTypes(); i++) {
    if (type == PixelFluidType(i)) return i;
  }
  msg(MSG::ERROR) << "Unrecognized fluid volume type: " << type << endmsg;
  return -1;
}
 
std::string OraclePixGeoManager::PixelFluidMat(int index) {
  int matType = 0;
  if (useLegacy()) {
    matType = m_legacyManager->PixelFluidMatType(index);
  } else {
    matType = db()->getInt(PixelFluid,"MATTYPE",index);
  }
  return getMaterialName("Fluid", 0, matType);
}

int OraclePixGeoManager::PixelFluidOrient(int layer, int phi) 
{
  if (useLegacy()) return m_legacyManager->PixelFluidOrient(layer, phi);
  return m_pixelStaveTypes->getFluidType(layer,phi);
}

//
// Pigtail
//
double OraclePixGeoManager::PixelPigtailThickness()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailThickness();
  return db()->getDouble(PixelPigtail,"THICK") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailStartY()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailStartY();
  return db()->getDouble(PixelPigtail,"STARTY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailEndY()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailEndY();
  return db()->getDouble(PixelPigtail,"ENDY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailWidthZ()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailWidthZ();
  return db()->getDouble(PixelPigtail,"WIDTHZ") * CLHEP::mm;
}

// Different width from the curved section in old geometry
double OraclePixGeoManager::PixelPigtailFlatWidthZ()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailFlatWidthZ();
  return PixelPigtailWidthZ();
}

double OraclePixGeoManager::PixelPigtailPosX()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailPosX();
  return db()->getDouble(PixelPigtail,"X") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailPosZ()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailPosZ();
  return db()->getDouble(PixelPigtail,"Z") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailBendX()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailBendX();
  return db()->getDouble(PixelPigtail,"BENDX") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailBendY()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailBendY();
  return db()->getDouble(PixelPigtail,"BENDY") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailBendRMin()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailBendRMin();
  return db()->getDouble(PixelPigtail,"BENDRMIN") * CLHEP::mm;
}

double OraclePixGeoManager::PixelPigtailBendRMax()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailBendRMax();
  return PixelPigtailBendRMin() + PixelPigtailThickness();
}

double OraclePixGeoManager::PixelPigtailBendPhiMin()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailBendPhiMin();
  return db()->getDouble(PixelPigtail,"BENDPHIMIN") * CLHEP::deg;
}

double OraclePixGeoManager::PixelPigtailBendPhiMax()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailBendPhiMax();
  return db()->getDouble(PixelPigtail,"BENDPHIMAX") * CLHEP::deg;
}

double OraclePixGeoManager::PixelPigtailEnvelopeLength()
{
  if (useLegacy()) return m_legacyManager->PixelPigtailEnvelopeLength();
  return db()->getDouble(PixelPigtail,"ENVLENGTH") * CLHEP::mm;
}

//
// Connector
//
int OraclePixGeoManager::PixelNumConnectorElements()
{
  if (useLegacy()) return m_legacyManager->PixelNumConnectorElements();
  return db()->getTableSize(PixelConnector);
}

double OraclePixGeoManager::PixelConnectorWidthX(int index)
{
  if (useLegacy()) return m_legacyManager->PixelConnectorWidthX(index);
  return db()->getDouble(PixelConnector,"WIDTHX",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelConnectorWidthY(int index)
{
  if (useLegacy()) return m_legacyManager->PixelConnectorWidthY(index);
  return db()->getDouble(PixelConnector,"WIDTHY",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelConnectorWidthZ(int index)
{
  if (useLegacy()) return m_legacyManager->PixelConnectorWidthZ(index);
  return db()->getDouble(PixelConnector,"WIDTHZ",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelConnectorPosX(int index)
{
  if (useLegacy()) return m_legacyManager->PixelConnectorPosX(index);
  return db()->getDouble(PixelConnector,"X",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelConnectorPosY(int index)
{
  if (useLegacy()) return m_legacyManager->PixelConnectorPosY(index);
  return db()->getDouble(PixelConnector,"Y",index) * CLHEP::mm;
}

double OraclePixGeoManager::PixelConnectorPosZ(int index)
{
  if (useLegacy()) return m_legacyManager->PixelConnectorPosZ(index);
  return db()->getDouble(PixelConnector,"Z",index) * CLHEP::mm;
}

//
// Design
//


int  OraclePixGeoManager::designType(bool isModule3D)
{
  
  if(isModule3D) return designType3D();

  if (dbVersion() < 2) {
    return 0; // Not used
  } else {
    if (PixelReadout->size() == 0) {
      msg(MSG::ERROR) << "ERROR in PixelReadout size. Should not occur!" << endmsg;
      return 0;
    } else if (PixelReadout->size() == 1 && !slhc() && !ibl()) {
      return 0;
    } else { // Only in IBL and SLHC
      return db()->getInt(PixelModule,"DESIGNTYPE",moduleType());
    }
  }
}


int  OraclePixGeoManager::designType3D()
{

  if (dbVersion() < 2) {
    return 0; // Not used
  } 
  else {
    if (PixelReadout->size() == 0) {
      msg(MSG::ERROR) << "ERROR in PixelReadout size. Should not occur!" << endmsg;
      return 0;
    } else if (PixelReadout->size() == 1 && !slhc() && !ibl()) {
      return 0;
    } else { // Only in IBL and SLHC
      int type = db()->getInt(PixelModule,"DESIGNTYPE",moduleType3D());
      return type;
    }
  }
}

int OraclePixGeoManager::DesignReadoutSide(bool isModule3D)
{
  if (dbVersion() < 2) {
    return -1; 
  } else {
    int type = designType((ibl()&&isModule3D));

    return db()->getInt(PixelReadout,"READOUTSIDE",type);
  }
}

int OraclePixGeoManager::DesignNumChipsPhi(bool isModule3D)
{
  if (dbVersion() < 2) {  
    return m_legacyManager->DesignNumChipsPhi();
  } else {
    int type = designType((ibl()&&isModule3D));

    return db()->getInt(PixelReadout,"NCHIPSPHI",type);
  } 
}    


int OraclePixGeoManager::DesignNumChipsEta(bool isModule3D)
 {
  if (dbVersion() < 2) {  
    return m_legacyManager->DesignNumChipsEta();
  } else {
    int type = designType((ibl()&&isModule3D));

    return db()->getInt(PixelReadout,"NCHIPSETA",type);
  }
}

int OraclePixGeoManager::DesignNumRowsPerChip(bool isModule3D)
{
  if (dbVersion() < 2) {  
    return m_legacyManager->DesignNumRowsPerChip(isInnermostPixelLayer());
  } else {
    int type = designType((ibl()&&isModule3D));

    return db()->getInt(PixelReadout,"ROWSPERCHIP",type);
  }
}

int OraclePixGeoManager::DesignNumColsPerChip(bool isModule3D) 
{
  if (dbVersion() < 2) {  
    return m_legacyManager->DesignNumColsPerChip(isInnermostPixelLayer());
  } else {
    int type = designType((ibl()&&isModule3D));

    return db()->getInt(PixelReadout,"COLSPERCHIP",type);
  }
}


int OraclePixGeoManager::DesignDiodesPhiTotal(bool isModule3D)
{
  if  (dbVersion() < 2) {
    return m_legacyManager->DesignDiodesPhiTotal(isInnermostPixelLayer());
  } else {
    return DesignNumChipsPhi(isModule3D) * (DesignNumRowsPerChip(isModule3D)+DesignNumEmptyRowsInGap(isModule3D)) - DesignNumEmptyRowsInGap(isModule3D);
  }
}

int OraclePixGeoManager::DesignDiodesEtaTotal(bool isModule3D)
{
  if  (dbVersion() < 2) {
    return m_legacyManager->DesignDiodesEtaTotal(isInnermostPixelLayer());
  } else {
    return DesignNumChipsEta(isModule3D) * DesignNumColsPerChip(isModule3D);
  }
}


int OraclePixGeoManager::DesignCellRowsPerCircuit(bool isModule3D)
{
  return DesignNumChipsPhi(isModule3D) * DesignNumRowsPerChip(isModule3D);
}

int OraclePixGeoManager::DesignCellColumnsPerCircuit(bool isModule3D)
{
  return DesignNumColsPerChip(isModule3D);
}

int OraclePixGeoManager::DesignDiodeRowsPerCircuit(bool isModule3D)
{
  return DesignDiodesPhiTotal(isModule3D);
}

int OraclePixGeoManager::DesignDiodeColumnsPerCircuit(bool isModule3D)
{
  return DesignNumColsPerChip(isModule3D);
}

int  OraclePixGeoManager::DesignNumEmptyRowsInGap(bool isModule3D)
{
  // Could determine it from m_gangedIndexMap but expect it to be filled correctly in PixelReadoutTable 
  if (dbVersion() < 2) {
    return m_legacyManager->DesignNumEmptyRowsInGap();
  } else {
    int type=designType((ibl()&&isModule3D));

    return db()->getInt(PixelReadout,"EMPTYROWS",type);
  } 
}

// Ganged Pixels
int OraclePixGeoManager::GangedType()
{
  // type 0 means no ganged pixels
  if (!slhc() && !ibl()) return 1;
  if (ibl()) {
    return db()->getInt(PixelReadout,"GANGEDTYPE",designType());
  } else {
    int type = 1;
    if (db()->testField(PixelReadout,"GANGEDTYPE",designType())) {
      type = db()->getInt(PixelReadout,"GANGEDTYPE",designType());
    }
    return type;
  }
}
      

int OraclePixGeoManager::GangedTableIndex(int index, int type)
{
  // There is only one type for standard ATLAS so we just return the index.
  if (!slhc() && !ibl()) return index; 

  if (!m_gangedIndexMap) {
    // First time we create the map
    m_gangedIndexMap = new std::map<int,std::vector<int> >;
    for (unsigned int i = 0; i < db()->getTableSize(PixelGangedPixels); i++){
      int testType = 1;
      if (db()->testField(PixelGangedPixels,"TYPE",i)) {
	testType = db()->getInt(PixelGangedPixels,"TYPE",i);
      }
      (*m_gangedIndexMap)[testType].push_back(i);
    }
  }

  int newIndex = -1;
  std::map<int,std::vector<int> >::const_iterator iter = m_gangedIndexMap->find(type);
  if (iter == m_gangedIndexMap->end()) {
    msg(MSG::ERROR) << "Ganged pixel type " << type << " not found." << endmsg;
  } else {
    const std::vector<int> & vec = iter->second;
    if (index < 0 || static_cast<unsigned int>(index) >= vec.size()) {
      msg(MSG::ERROR) << "Ganged pixel index " << index << " for type " << type << " out of range." << endmsg;
    } else {
      newIndex = vec[index];
    }
  }
  return newIndex;
}
 
int OraclePixGeoManager::NumberOfEmptyRows(bool isModule3D)
{
  return DesignNumEmptyRowsInGap(isModule3D);
}

int OraclePixGeoManager::EmptyRows(int index)
{
  if (dbVersion() < 2) {
    return m_legacyManager->EmptyRows(index);
  } else {
    int newIndex = GangedTableIndex(index, GangedType());
    if (newIndex >= 0) {
      return db()->getInt(PixelGangedPixels,"EMPTYROW",newIndex);
    } else {
      return 0;
    }
  }
}

int OraclePixGeoManager::EmptyRowConnections(int index)
{
  if (dbVersion() < 2) {
    return m_legacyManager->EmptyRowConnections(index);
  } else {
    int newIndex = GangedTableIndex(index, GangedType());
    if (newIndex >= 0) {
      return db()->getInt(PixelGangedPixels,"CONNECTROW",newIndex);
    } else {
      return 0;
    }
  }    
}


double OraclePixGeoManager::DesignRPActiveArea(bool isModule3D)
{
  if (dbVersion() < 2) { 
    return m_legacyManager->DesignRPActiveArea();
  } else {
    // All layers assumed to be the same.
    return DesignPitchRP(isModule3D) * ((DesignNumRowsPerChip(isModule3D)+DesignNumEmptyRowsInGap(isModule3D)) * DesignNumChipsPhi(isModule3D) - DesignNumEmptyRowsInGap(isModule3D));
  } 
}

double OraclePixGeoManager::DesignZActiveArea(bool isModule3D)
{
  if (dbVersion() < 2) {   
    return m_legacyManager->DesignZActiveArea();
  } else {
    // All layers assumed to be the same.
    return (DesignPitchZ(isModule3D) * (DesignNumColsPerChip(isModule3D) - 2) + 2 * DesignPitchZLong(isModule3D)) * DesignNumChipsEta(isModule3D) + 2 * (DesignPitchZLongEnd(isModule3D) - DesignPitchZLong(isModule3D));
  }
}
    
double OraclePixGeoManager::DesignPitchRP(bool isModule3D)
{
  if (dbVersion() < 2) {
    return m_legacyManager->DesignPitchRP(isInnermostPixelLayer());
  } else {
    int type = designType((ibl()&&isModule3D));
    return db()->getDouble(PixelReadout,"PITCHPHI",type) * CLHEP::mm;
 } 
}

double OraclePixGeoManager::DesignPitchZ(bool isModule3D)
{
  if (dbVersion() < 2) {
    return m_legacyManager->DesignPitchZ(isInnermostPixelLayer());
  } else {
    int type = designType((ibl()&&isModule3D));
    return db()->getDouble(PixelReadout,"PITCHETA",type) * CLHEP::mm;
  }
}

double OraclePixGeoManager::DesignPitchZLong(bool isModule3D)
{
  // Defaults to DesignPitchZ if not specified or is zero.
  if (dbVersion() < 2) {
    return m_legacyManager->DesignPitchZLong(isInnermostPixelLayer());
  } else {
    int type = designType((ibl()&&isModule3D));
    double pitch = db()->getDouble(PixelReadout,"PITCHETALONG",type) * CLHEP::mm;
    if (pitch == 0) pitch = DesignPitchZ(isModule3D);
    return pitch;
  }
}

double OraclePixGeoManager::DesignPitchZLongEnd(bool isModule3D)
{
  // Defaults to DesignPitchZLongEnd if not specified or is zero.
  if (!slhc() && !ibl()) { // This check is not really needed once the field is in the database.
    return DesignPitchZLong(isModule3D);
  } else {
    int type = designType((ibl()&&isModule3D));
    double pitch = 0;
    if (db()->testField(PixelReadout,"PITCHETAEND",type)) {
      pitch = db()->getDouble(PixelReadout,"PITCHETAEND",type) * CLHEP::mm;
    }
    if (pitch == 0) pitch = DesignPitchZLong(isModule3D);
    return pitch;
  }
}


double OraclePixGeoManager::DesignGapRP(bool isModule3D)
{
  if (dbVersion() < 2) {
    return m_legacyManager->DesignGapRP();
  } else {
    return DesignNumEmptyRowsInGap(isModule3D) * DesignPitchRP(isModule3D);
  }
}

double OraclePixGeoManager::DesignGapZ(bool isModule3D)
{
  if (dbVersion() < 2) {  
    return m_legacyManager->DesignGapZ();
  } else {
    return 2. * (DesignPitchZLong(isModule3D) - DesignPitchZ(isModule3D));
  }
}

int OraclePixGeoManager::DesignCircuitsPhi(bool /* isModule3D */)
{
  //
  // This should be (*pdch)[0]->getDouble("NRPCHIP"), but in the current
  // design we prefer to have one chip in the rphi direction
  // and define the connections for the pixels in the gap
  return 1;
}

int OraclePixGeoManager::DesignCircuitsEta(bool isModule3D)
{
  return DesignNumChipsEta(isModule3D);
}



// Endcap 
// SLHC only
int OraclePixGeoManager::getDiskRingIndex(int disk, int eta)
{ 
  if (!slhc()) return 0;
  if (!m_diskRingIndexMap) {
    m_diskRingIndexMap = new InDetDD::PairIndexMap;
    for (unsigned int indexTmp = 0; indexTmp < db()->getTableSize(PixelDiskRing); ++indexTmp) {
      int diskTmp = db()->getInt(PixelDiskRing,"DISK",indexTmp);
      int ringTmp = db()->getInt(PixelDiskRing,"RING",indexTmp);
      m_diskRingIndexMap->add(diskTmp,ringTmp,indexTmp);
    }
  }
  int index = m_diskRingIndexMap->find(disk, eta);
  if (index < 0)  msg(MSG::ERROR) << "Index not found for disk,ring =  " << disk << ", " << eta << endmsg;
  //std::cout << "Index found for disk,ring =  " << disk << ", " << eta << " : " << index << std::endl;
  return index;
}

// SLHC Only
int OraclePixGeoManager::getDiskRingType(int disk, int eta)
{
  int index = getDiskRingIndex(disk, eta);
  if (index < 0) return -1;
  return  db()->getInt(PixelDiskRing,"RINGTYPE",index);
}

int OraclePixGeoManager::PixelDiskNRings() 
{
  if (slhc()) {
    return db()->getInt(PixelDisk,"NRINGS",currentLD);
  } else {
    return 1;
  }
}


int OraclePixGeoManager::PixelDiskRingNModules() {
  if (slhc()) {
    int ringType = getDiskRingType(currentLD,m_eta);
    if (ringType>=0) return db()->getInt(PixelRing,"NMODULE",ringType);
    return 0;
  } else {
    return 2*db()->getInt(PixelDisk,"NMODULE",currentLD);
  }
}

// NB. SLHC/LHC different parameter name
double  OraclePixGeoManager::PixelDiskRMin(bool includeSupports)
{
  if (!slhc()) {
    return db()->getDouble(PixelDisk,"RIDISK",currentLD)*mmcm();
  } else {
    double result = db()->getDouble(PixelDisk,"RMIN",currentLD) * CLHEP::mm;
    if(includeSupports) {
      result = std::min( result, PixelDiskSupportRMin(0) );
    }
    int etaInner = 0; // Inner ring
    int ringType = getDiskRingType(currentLD,etaInner); 
    if (ringType >= 0 && db()->testField(PixelRing,"RMIN",ringType) && db()->getDouble(PixelRing,"RMIN",ringType)) {
      double ringRmin = db()->getDouble(PixelRing,"RMIN",ringType) * CLHEP::mm - 0.01*CLHEP::mm;  // ring envelope has a 0.01mm safety
      if (ringRmin < result) {
	msg(MSG::WARNING) << "Ring rmin is less than disk rmin for disk : " << currentLD 
			  << ". Ring rmin: " << ringRmin << ", Disk rmin: " << result <<endmsg;
	result = ringRmin - 0.1*CLHEP::mm; // NB. ring envelope has a 0.01mm saftey added, but we add a little more.
      }
    }
    return result;
  }
}

// SLHC only
double OraclePixGeoManager::PixelDiskRMax(bool includeSupports)
{
  double result = db()->getDouble(PixelDisk,"RMAX",currentLD) * CLHEP::mm;
  if(includeSupports) {
    result = std::max( result, PixelDiskSupportRMax(2) );
  }
  // save current state
  int tmpEta = m_eta;
  m_eta = PixelDiskNRings() - 1; // Outer ring
  int ringType = getDiskRingType(currentLD,m_eta); 
  if (ringType >=0 && db()->testField(PixelRing,"RMIN",ringType) && db()->getDouble(PixelRing,"RMIN",ringType)) {
    // This is not so nice as PixelRingRMax can potentially call PixelDiskRMax, however it
    // only calls PixelDiskRMax if the above condition is not satisified. So hopefully OK.
    // TODO: Code could do with some improvement to make it less fragile.
    double ringRmax  = PixelRingRMax(0.01*CLHEP::mm); // ring envelope has a 0.01mm safety
    if (ringRmax > result) {
      msg(MSG::WARNING) << "Ring rmax is greater than disk rmax for disk : " << currentLD 
			<< ". Ring rmax: " << ringRmax << ", Disk rmax: " << result <<endmsg;
      result = ringRmax + 0.1*CLHEP::mm; // NB. ring envelope has a 0.01mm saftey added, but we add a little more.
    }
  }
  // restore state
  m_eta = tmpEta;
  return result;
}

// SLHC only
double OraclePixGeoManager::PixelDiskThickness(double safety) {
  int nrings = PixelDiskNRings();
  int tmpRing = m_eta;
  m_eta = 0;
  double teven = PixelRingZpos() + PixelRingThickness()/2 + safety;
  double thick = 2*teven;
  if (nrings > 1) {
    m_eta = 1;
    double todd  = PixelRingZpos() + PixelRingThickness()/2 + safety;
    thick = 2*std::max(teven,todd);
  }  
  m_eta = tmpRing;
  return thick;
}

///

//
// endcap rings
//
// SLHC only

double OraclePixGeoManager::PixelRingRcenter() {
  
  // If ring rmin is present and non-zero use that.
  int ringType = getDiskRingType(currentLD,m_eta); 
  if (ringType >=0 && db()->testField(PixelRing,"RMIN",ringType) && db()->getDouble(PixelRing,"RMIN",ringType)) {
    return db()->getDouble(PixelRing,"RMIN",ringType)  * CLHEP::mm + PixelModuleLength()/2;
  } else { 
    // Otherwise calculate from disk rmin/rmax
    int nrings = PixelDiskNRings();
    assert( m_eta>=0 && m_eta<nrings );
    
    double rcFirst = PixelDiskRMin()+PixelModuleLength()/2;
    if(m_eta==0) return rcFirst;
    
    double rcLast = PixelDiskRMax()-PixelModuleLength()/2;
    if(m_eta==nrings-1) return rcLast;
    
    double deltar = (rcLast - rcFirst) / (nrings-1);
    return rcFirst + m_eta*deltar;
  }
}

// SLHC only
double OraclePixGeoManager::PixelRingRMin(double safety) {
  // default safety value is 0.01mm, but for exact calculations, safety=0 must be forced
  // If ring rmin is present and non-zero use that.
  int ringType = getDiskRingType(currentLD,m_eta); 
  if (ringType >= 0 && db()->testField(PixelRing,"RMIN",ringType) && db()->getDouble(PixelRing,"RMIN",ringType)) {
    return db()->getDouble(PixelRing,"RMIN",ringType)  * CLHEP::mm - std::abs(safety); 
  } else {
    // Otherwise calculated it from disk rmin
    if(m_eta==0) return PixelDiskRMin() - std::abs(safety);
    else return PixelRingRcenter() - PixelModuleLength()/2 - std::abs(safety); 
  }
}

// SLHC only
double OraclePixGeoManager::PixelRingRMax(double safety) {
  //
  // ringRmax (OB) is the longest side of the triangle OAB, formed by
  // rinRmin (OA) and the line AB joining the center of the inner edge
  // of the module (A) to any of the two outer vertices of the module
  // (B).  Point O is the center of the ring.
  //
  // Default safety value is 0.01mm, but for exact calculations, safety=0 must be forced
  // modL,modW,modD are module length,width

  //  I can't see what's wrong here...
  double modL = PixelModuleLength();
  double modW = PixelModuleWidth();
  double distAB_sqr = modL*modL + modW*modW/4.;
  double distAB = sqrt(distAB_sqr);
  double distOA = PixelRingRMin(0);
  double costheta = modL/distAB;
  double distOB_sqr = distOA*distOA + distAB_sqr + 2*distOA*distAB*costheta;
  double ringRmax = sqrt(distOB_sqr);
  return ringRmax + std::abs(safety);
}

// SLHC only
double OraclePixGeoManager::PixelRingThickness(double safety) {
  return PixelModuleThickness() + PixelRingStagger() + 2*safety;
}

// SLHC only
double OraclePixGeoManager::PixelRingZpos() {
  // Returns always a positive value, although rings go on both sides of a support disk
  return PixelRingZoffset();
}

// SLHC only
double OraclePixGeoManager::PixelRingZoffset() 
{
  int index = getDiskRingIndex(currentLD,m_eta);
  return std::abs(db()->getDouble(PixelDiskRing,"ZOFFSET",index))*CLHEP::mm;
}

// SLHC only
int OraclePixGeoManager::PixelRingSide() 
{
  int index = getDiskRingIndex(currentLD,m_eta);
  return db()->getInt(PixelDiskRing,"SIDE",index);
}

// SLHC only
double OraclePixGeoManager::PixelRingStagger() 
{
  int ringType = getDiskRingType(currentLD,m_eta);
  return db()->getDouble(PixelRing,"STAGGER",ringType)*CLHEP::mm;
}


// SLHC only
//int OraclePixGeoManager::PixelRingNmodules() {
//  return db()->getInt("PixelRing","NMODULES",ringIndex)*CLHEP::mm();
//}


int OraclePixGeoManager::PixelDiskNumSupports() {
  // Hardwire for now
  return 3;
}

double OraclePixGeoManager::PixelDiskSupportRMin(int isup) {
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMIN";
  return db()->getDouble(PixelDisk,field.str(),currentLD)*mmcm();
}

double OraclePixGeoManager::PixelDiskSupportRMax(int isup) {
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMAX";
  return db()->getDouble(PixelDisk,field.str(),currentLD)*mmcm();
}


// SLHC only
double OraclePixGeoManager::PixelDiskSupportThickness(int isup) {

  std::ostringstream prefix;
  prefix <<"SUP"<< isup+1 <<"THICK";

  bool found = false;
  double tck = 0;

  // First check text file
  // default support thickness
  if (db()->testFieldTxt(PixelDisk,"SUP_THICK")) {
    tck = db()->getDouble(PixelDisk,"SUP_THICK");
    found = true;
  } 
  // overwrites if found
  if (db()->testFieldTxt(PixelDisk,prefix.str(),currentLD)) {
    tck = db()->getDouble(PixelDisk,prefix.str(),currentLD);
    found = true;
  }

  // Now check database
  if (!found)  tck = db()->getDouble(PixelDisk,prefix.str(),currentLD);

  if(tck>0.) {
    return tck * mmcm();
  } else { // radlen
    int typeNum = PixelDiskSupportMaterialTypeNum(isup);
    std::string matName = getMaterialName("DiskSupport", currentLD, typeNum);
    return CalculateThickness(tck, matName);
  }
}

// SLHC only
int OraclePixGeoManager::PixelDiskSupportMaterialTypeNum(int isup) {
 
  if (dbVersion() < 3) return 0;

  std::ostringstream prefix;
  prefix <<"SUP"<< isup+1 <<"MAT";

  int imat = 0;
  bool found = false;
  // default material type
  if (db()->testFieldTxt(PixelDisk,"SUP_MAT")) {
    imat = db()->getInt(PixelDisk,"SUP_MAT");
    found = true;
  } 
  // overwrites if found
  if (db()->testFieldTxt(PixelDisk,prefix.str(),currentLD)) {
    imat = db()->getInt(PixelDisk,prefix.str(),currentLD);
    found = true;
  }

  if (!found) {
    imat = db()->getInt(PixelDisk,prefix.str(),currentLD);
  }
  return imat;
}

// SLHC only
double OraclePixGeoManager::PixelModuleThicknessN() {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessN
  // is the max of ThicknessP and thickness from the module center to
  // the outer surface of the hybrid plus some safety.
  //
  double safety = 0.01*CLHEP::mm;
  double thickn = 0.5 * PixelBoardThickness()
    + PixelHybridThickness() + safety;
  double thick = std::max(thickn, PixelModuleThicknessP());
  return thick;
}

// SLHC only
double OraclePixGeoManager::PixelModuleThicknessP() {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessP
  // is thickness from the module center to the outer surface of the
  // chips plus some safety.

  double safety = 0.01*CLHEP::mm;
  double thick = 0.5 * PixelBoardThickness() +
    PixelChipThickness() + PixelChipGap() + safety;

  return thick;
}


// SLHC only
double OraclePixGeoManager::PixelModuleThickness() {
  // This is total thickness of the module envelope
  // return PixelModuleThicknessP()+PixelModuleThicknessN();

  // Return thickness of a box symmetric about sensor. This may be larger 
  // than the actual module envelope as the envelop can be shifted off center
  return 2*std::max(PixelModuleThicknessP(),PixelModuleThicknessN());
}

// SLHC only
double OraclePixGeoManager::PixelModuleWidth() {
  double width = std::max( std::max( PixelBoardWidth(),
				     PixelHybridWidth()),
			   PixelChipWidth());
  return width;
}

// SLHC only
double OraclePixGeoManager::PixelModuleLength() {
  // balcony is assumed zero
  double length = std::max( std::max(PixelHybridLength(),
				     PixelBoardLength()),
			    PixelChipLength());
  return length;
}




//
//*** DBM Parameters with local database  ***//
//

// return angle of the telescope
double OraclePixGeoManager::DBMAngle() {
  return db()->getDouble(DBMTelescope,"ANGLE")*CLHEP::deg;
}

// return dimension of the DBM telescope
double OraclePixGeoManager::DBMTelescopeX() {
   return db()->getDouble(DBMTelescope,"WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMTelescopeY() {
   return db()->getDouble(DBMTelescope,"HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMTelescopeZ() {
   return db()->getDouble(DBMTelescope,"LENGTH")*CLHEP::mm;
}

// return height and length of the module cage having a 3-layers structure
double OraclePixGeoManager::DBMModuleCageY() {
  return db()->getDouble(DBMTelescope,"CAGE_HEIGHT")*CLHEP::mm;
} 
double OraclePixGeoManager::DBMModuleCageZ() {
  return db()->getDouble(DBMTelescope,"CAGE_LENGTH")*CLHEP::mm;
} 

// return layer spacing
double OraclePixGeoManager::DBMSpacingZ() {
  return db()->getDouble(DBMCage,"ZSPACING")*CLHEP::mm;
}
double OraclePixGeoManager::DBMSpacingRadial() {
  if (currentLD == 0)
    return db()->getDouble(DBMCage,"RADIAL_SPACE_0")*CLHEP::mm;
  else if (currentLD == 1)
    return db()->getDouble(DBMCage,"RADIAL_SPACE_1")*CLHEP::mm;
  else if (currentLD == 2)
    return db()->getDouble(DBMCage,"RADIAL_SPACE_2")*CLHEP::mm;
  else {
     msg(MSG::WARNING) << "DBMSpacingRadial() is not found" << endmsg;
     return 0.;
  }
}
// return dimension of bracket unit
double OraclePixGeoManager::DBMBracketX() {
  return db()->getDouble(DBMBracket,"WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBracketY() {
  return db()->getDouble(DBMBracket,"HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBracketZ() {
  return db()->getDouble(DBMBracket,"THICKNESS")*CLHEP::mm;
}
double OraclePixGeoManager::DBMTrapezBackTheta() {
  return db()->getDouble(DBMBracket,"TRAPEZBACK_THETA")*CLHEP::deg;
}
double OraclePixGeoManager::DBMTrapezBackX() {
  return db()->getDouble(DBMBracket,"TRAPEZBACK_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMTrapezBackY() {
  return db()->getDouble(DBMBracket,"TRAPEZBACK_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMTrapezBackShortZ() {
  return db()->getDouble(DBMBracket,"TRAPEZBACK_ZSHORT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktWindowX() {
  return db()->getDouble(DBMBracket,"WINDOW_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktWindowY() {
  return db()->getDouble(DBMBracket,"WINDOW_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktWindowOffset() {
  return db()->getDouble(DBMBracket,"WINDOW_OFFSET")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktWindowCenterZ() {
  return db()->getDouble(DBMBracket,"WINDOW_CENTERZ")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktTopBlockZ() {
  return db()->getDouble(DBMBracket,"TOPBLOCK_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktSideBlockX() {
  return db()->getDouble(DBMBracket,"SIDEBLOCK_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktSideBlockY() {
  return db()->getDouble(DBMBracket,"SIDEBLOCK_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktLockZ() {
  return db()->getDouble(DBMBracket,"LOCK_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktLockY() {
  return db()->getDouble(DBMBracket,"LOCK_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktFinLongZ() {
  return db()->getDouble(DBMBracket,"COOLINGFIN_ZLONG")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktFinHeight() {
  return db()->getDouble(DBMBracket,"COOLINGFIN_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktFinThick() {
  return db()->getDouble(DBMBracket,"COOLINGFIN_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMBrcktFinPos() {
  return db()->getDouble(DBMBracket,"COOLINGFIN_POS")*CLHEP::mm;
}

// return spacing between V-slide and first layer
double OraclePixGeoManager::DBMSpace() {
  return db()->getDouble(DBMCage,"SPACING1")*CLHEP::mm;
}

// return dimensions of the main plate
double OraclePixGeoManager::DBMMainPlateX() {
  return db()->getDouble(DBMCage,"MAINPLATE_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMMainPlateY() {
  return db()->getDouble(DBMCage,"MAINPLATE_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMMainPlateZ() {
  return db()->getDouble(DBMCage,"MAINPLATE_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMMPlateWindowWidth() {
  return db()->getDouble(DBMCage,"MPWINDOW_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMMPlateWindowHeight() {
  return db()->getDouble(DBMCage,"MPWINDOW_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMMPlateWindowPos() {
  return db()->getDouble(DBMCage,"MPWINDOW_POS")*CLHEP::mm;
}
// return dimensions of aluminium side plates
double OraclePixGeoManager::DBMCoolingSidePlateX() {
  return db()->getDouble(DBMCage,"SIDEPLATE_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMCoolingSidePlateY() {
  return db()->getDouble(DBMCage,"SIDEPLATE_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMCoolingSidePlateZ() {
  return db()->getDouble(DBMCage,"SIDEPLATE_LENGTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMCoolingSidePlatePos() {
  return db()->getDouble(DBMCage,"SIDEPLATE_POS")*CLHEP::mm;
}

// return dimension of sensor, chip and ceramic
double OraclePixGeoManager::DBMDiamondX() {
  return db()->getDouble(DBMModule,"DIAMOND_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMDiamondY() {
  return db()->getDouble(DBMModule,"DIAMOND_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMDiamondZ() {
  return db()->getDouble(DBMModule,"DIAMOND_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMFEI4X() {
  return db()->getDouble(DBMModule,"FEI4_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMFEI4Y() {
  return db()->getDouble(DBMModule,"FEI4_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMFEI4Z() {
  return db()->getDouble(DBMModule,"FEI4_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMCeramicX() {
  return db()->getDouble(DBMModule,"CERAMIC_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMCeramicY() {
  return db()->getDouble(DBMModule,"CERAMIC_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMCeramicZ() {
  return db()->getDouble(DBMModule,"CERAMIC_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMAirGap() {
  return db()->getDouble(DBMModule,"AIR_GAP")*CLHEP::mm;
}
double OraclePixGeoManager::DBMKaptonZ() {
  return db()->getDouble(DBMModule,"KAPTONZ")*CLHEP::mm;
}

// flex support
double OraclePixGeoManager::DBMFlexSupportX() {
  return db()->getDouble(DBMCage,"FLEXSUPP_WIDTH")*CLHEP::mm;
}
double OraclePixGeoManager::DBMFlexSupportY() {
    return db()->getDouble(DBMCage,"FLEXSUPP_HEIGHT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMFlexSupportZ() {
  return db()->getDouble(DBMCage,"FLEXSUPP_THICK")*CLHEP::mm;
}
double OraclePixGeoManager::DBMFlexSupportOffset() {
    return db()->getDouble(DBMCage, "FLEXSUPP_OFFSET")*CLHEP::mm;
}

// return radius of supporting rod
double OraclePixGeoManager::DBMRodRadius() {
  return db()->getDouble(DBMCage,"ROD_RADIUS")*CLHEP::mm;
}
// return distance between center of rods
double OraclePixGeoManager::DBMMPlateRod2RodY() {
  return db()->getDouble(DBMCage,"ROD2ROD_VERT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMMPlateRod2RodX() {
  return db()->getDouble(DBMCage,"ROD2ROD_HOR")*CLHEP::mm;
}

// radius and thickness of PP0 board
double OraclePixGeoManager::DBMPP0RIn() {
  return db()->getDouble(DBMTelescope,"PP0_RIN")*CLHEP::mm;
}
double OraclePixGeoManager::DBMPP0ROut() {
  return db()->getDouble(DBMTelescope,"PP0_ROUT")*CLHEP::mm;
}
double OraclePixGeoManager::DBMPP0Thick() {
  return db()->getDouble(DBMTelescope,"PP0_THICK")*CLHEP::mm;
}


