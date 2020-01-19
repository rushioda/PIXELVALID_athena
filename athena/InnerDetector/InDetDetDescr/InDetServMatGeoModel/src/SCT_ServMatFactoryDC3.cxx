/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/SCT_ServMatFactoryDC3.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoCons.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoElement.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"


#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#define TRTELEMENTSINEL 9
#define SCTELEMENTSINEL 8

#include <sstream>

SCT_ServMatFactoryDC3::SCT_ServMatFactoryDC3(const InDetDD::AthenaComps * athenaComps, 
					     InDetMaterialManager * matManager)
  : InDetDD::SubDetectorFactoryBase(athenaComps, matManager)
{  
}


SCT_ServMatFactoryDC3::~SCT_ServMatFactoryDC3()
{

}



//## Other Operations (implementation)
void SCT_ServMatFactoryDC3::create(GeoPhysVol *mother)
{

  msg(MSG::DEBUG) << "Building SCT Service Material" << endreq;


  double epsilon = 0.002;

  DecodeVersionKey atlasVersionKey(geoModelSvc(), "ATLAS");
  DecodeVersionKey indetVersionKey(geoModelSvc(), "InnerDetector");
  DecodeVersionKey sctVersionKey(geoModelSvc(), "SCT");
  DecodeVersionKey trtVersionKey(geoModelSvc(), "TRT");

  IRDBRecordset_ptr atls = rdbAccessSvc()->getRecordsetPtr("AtlasMother", atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr inel = rdbAccessSvc()->getRecordsetPtr("INEL",  indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr tsci = rdbAccessSvc()->getRecordsetPtr("TSCI", trtVersionKey.tag(), trtVersionKey.node());
  // Comment out until SCT updates available.
  //const IRDBRecordset* sctFwdServices = rdbAccessSvc()->getRecordset("SctFwdServices", sctVersionKey.tag(), sctVersionKey.node());

//VVK  10/09/2005 Construct a gap for rails
  double outROfIDet =       (*atls)[0]->getDouble("IDETOR")*CLHEP::cm;
  double endZOfIDet =       (*atls)[0]->getDouble("IDETZMX")*CLHEP::cm;
  double minRofGap  =       1050.0*CLHEP::mm;
  double phiWid=(70.*CLHEP::mm)/outROfIDet;    double safetyGap=1.*CLHEP::mm;
  const GeoShape* railGap1=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap , 
					-phiWid/2.,phiWid);
  const GeoShape* railGap2=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap ,
					-phiWid/2.+M_PI,phiWid);

  
  // Build SCT services in Endcap.
  // (Code taken from TRT_GeoModel)
  
  // Hardwire min sct services for now. The database structures should be moved out of TRT anyway.
  double rminSCTServ = 620*CLHEP::mm;

  //double innerRadiusOfSCTSupport = (*tsci)[0]->getDouble("RMIN")*CLHEP::cm + 2*epsilon;
  double innerRadiusOfSCTSupport =  rminSCTServ + 2*epsilon;
  double outerRadiusOfSCTSupport = (*tsci)[0]->getDouble("RMAX")*CLHEP::cm;
  double lengthOfSCTSupport = ((*tsci)[0]->getDouble("ZMAX")-(*tsci)[0]->getDouble("ZMIN"))*CLHEP::cm - epsilon;
  double positionOfSCTSupport= 0.5 * ((*tsci)[0]->getDouble("ZMAX")+(*tsci)[0]->getDouble("ZMIN"))*CLHEP::cm;

  //double innerRadiusOfSCTCables = (*tsci)[1]->getDouble("RMIN")*CLHEP::cm + 2*epsilon;
  double innerRadiusOfSCTCables = rminSCTServ + 2*epsilon;
  double outerRadiusOfSCTCables = (*tsci)[1]->getDouble("RMAX")*CLHEP::cm;
  double lengthOfSCTCables = ((*tsci)[1]->getDouble("ZMAX")-(*tsci)[1]->getDouble("ZMIN"))*CLHEP::cm - epsilon;
  double positionOfSCTCables = 0.5 * ((*tsci)[1]->getDouble("ZMAX")+(*tsci)[1]->getDouble("ZMIN"))*CLHEP::cm;
 
  //double innerRadiusOfSCTCooling = (*tsci)[2]->getDouble("RMIN")*CLHEP::cm + 2*epsilon;
  double innerRadiusOfSCTCooling = rminSCTServ + 2*epsilon;
  double outerRadiusOfSCTCooling = (*tsci)[2]->getDouble("RMAX")*CLHEP::cm;
  double lengthOfSCTCooling = ((*tsci)[2]->getDouble("ZMAX")-(*tsci)[2]->getDouble("ZMIN"))*CLHEP::cm - epsilon;
  double positionOfSCTCooling = 0.5 * ((*tsci)[2]->getDouble("ZMAX")+(*tsci)[2]->getDouble("ZMIN"))*CLHEP::cm;


  // For new LMT we get name from SCT table SctFwdServices.

  
  // Comment out until SCT updates available.
  /*
  std::string sctCablesMaterialName = (*sctFwdServices)[0]->getString("POWERTAPEMATERIAL");

  const GeoMaterial * sctCablesMaterial;
  if (sctCablesMaterialName == "sct::CuKapton"){
    // We define it here for now as a quick fix.
    
    // Thickness of CuK 896 tapes smeared in phi = 0.08575cm
    double tapeCrossSection = (*sctFwdServices)[0]->getDouble("POWERTAPECROSSSECT")*CLHEP::mm2;
    double rave = 2*innerRadiusOfSCTCables*outerRadiusOfSCTCables/(innerRadiusOfSCTCables+outerRadiusOfSCTCables);
    double thickness = 988*tapeCrossSection/(2*CLHEP::pi*rave);
    // We need to scale the density to fit in with space given.
    //std::cout << "LMT thickness (mm) = " << thickness/CLHEP::mm << std::endl;
    double densityfactor = thickness/lengthOfSCTCables;
    const GeoElement  *copper  = m_materialManager->getElement("Copper");
    const GeoMaterial *kapton  = m_materialManager->getMaterial("std::Kapton");
    GeoMaterial * matCuKapton   = new GeoMaterial("CuKaptoninTRT",densityfactor * 2.94*CLHEP::gram/CLHEP::cm3);
    matCuKapton->add(const_cast<GeoElement*>(copper),  0.6142);
    matCuKapton->add(const_cast<GeoMaterial*>(kapton), 0.3858);
    matCuKapton->lock();
    sctCablesMaterial = matCuKapton;
  } else {
    sctCablesMaterial = m_materialManager->getMaterial(sctCablesMaterialName);
  }
  */
  std::string sctCablesMaterialName = "trt::SCTCables";
  const GeoMaterial * sctCablesMaterial =  materialManager()->getMaterial(sctCablesMaterialName);


  // SCT Support: 
  GeoTube    *sSCTSupport = new GeoTube( innerRadiusOfSCTSupport, outerRadiusOfSCTSupport, 0.5*lengthOfSCTSupport); 
  GeoLogVol  *lSCTSupport = new GeoLogVol("SCTSupport", sSCTSupport, sctCablesMaterial);
  GeoPhysVol *pSCTSupport = new GeoPhysVol(lSCTSupport);
  GeoTransform *xSCTSupportPlus  = new GeoTransform(HepGeom::TranslateZ3D(+positionOfSCTSupport));
  GeoTransform *xSCTSupportMinus = new GeoTransform(HepGeom::TranslateZ3D(-positionOfSCTSupport));
  mother->add(xSCTSupportPlus);
  mother->add(pSCTSupport);
  mother->add(xSCTSupportMinus);
  mother->add(pSCTSupport);
   
  // SCT Cables:
  GeoTube    *sSCTCables = new GeoTube(innerRadiusOfSCTCables, outerRadiusOfSCTCables, 0.5*lengthOfSCTCables); 
  GeoLogVol  *lSCTCables = new GeoLogVol("SCTCables", sSCTCables, materialManager()->getMaterial("trt::SCTCables"));
  GeoPhysVol *pSCTCables = new GeoPhysVol(lSCTCables);
  GeoTransform *xSCTCablesPlus  = new GeoTransform(HepGeom::TranslateZ3D(+positionOfSCTCables ));
  GeoTransform *xSCTCablesMinus = new GeoTransform(HepGeom::TranslateZ3D(-positionOfSCTCables ));

  mother->add(xSCTCablesPlus);
  mother->add(pSCTCables);
  mother->add(xSCTCablesMinus);
  mother->add(pSCTCables);

   
  // SCT Cooling:
  GeoTube    *sSCTCooling = new GeoTube( innerRadiusOfSCTCooling, outerRadiusOfSCTCooling, 0.5*lengthOfSCTCooling); 
  GeoLogVol  *lSCTCooling = new GeoLogVol("SCTCooling", sSCTCooling, materialManager()->getMaterial("trt::SCTCooling"));
  GeoPhysVol *pSCTCooling = new GeoPhysVol(lSCTCooling);
  GeoTransform *xSCTCoolingPlus  = new GeoTransform(HepGeom::TranslateZ3D(+positionOfSCTCooling ));
  GeoTransform *xSCTCoolingMinus = new GeoTransform(HepGeom::TranslateZ3D(-positionOfSCTCooling ));
  mother->add(xSCTCoolingPlus);
  mother->add(pSCTCooling);
  mother->add(xSCTCoolingMinus);
  mother->add(pSCTCooling);


  // Build the INEL part of the services:
  //
//VK  for (int jj =0; jj < SCTELEMENTSINEL; jj++) {
//VK    int ii = jj + TRTELEMENTSINEL;

  int NRECORDS = (int) (*inel).size();
  int irecold=0;
  for (int ii =0; ii < NRECORDS; ii++) {
    if ((*inel)[ii]->getFloat("IBLOC") < 11.0) continue; //VK skip TRT services 
    // Build the Phys Vol
    std::ostringstream o;
    o << irecold++;
    std::string logName = "SctInel"+o.str();  
    double halflength = ((*inel)[ii]->getFloat("ZMAX")-(*inel)[ii]->getFloat("ZMIN"))/2.*CLHEP::cm;
    int volType = (int) (*inel)[ii]->getFloat("VOLTYP");

    const GeoShape* serviceTubeTmp1 = createShape(volType,
					      (*inel)[ii]->getFloat("RMIN1")*CLHEP::cm,
					      (*inel)[ii]->getFloat("RMAX1")*CLHEP::cm,
					      halflength,
					      (*inel)[ii]->getFloat("RMIN2")*CLHEP::cm,
					      (*inel)[ii]->getFloat("RMAX2")*CLHEP::cm);

    const GeoShape* serviceTube = serviceTubeTmp1;
    if( (*inel)[ii]->getFloat("RMAX1")*CLHEP::cm  > minRofGap   ||
        (*inel)[ii]->getFloat("RMAX2")*CLHEP::cm  > minRofGap     )  {
//
//VVK Subtract RailGap out of services
        const GeoShape* serviceTubeTmp2 = (GeoShape*) & (*serviceTubeTmp1).subtract(*railGap1);
                        serviceTube     = (GeoShape*) & (*serviceTubeTmp2).subtract(*railGap2);
    }    

    // create the material...
    // In AGE the radiation length is specified and from that the density is
    // calculated assuming the material is C. I do the same here for now but
    // will eventually define all the materials.
    double fractionRL = (*inel)[ii]->getFloat("MATER");
    const GeoMaterial* cylMat;
    if (fractionRL > 0.999) { 
      // In this case a fixed material is used called Services which is the same
      // as trt::PatchOut
      cylMat = materialManager()->getMaterial("trt::PatchOut");
    } else {
      std::ostringstream nameStr;
      nameStr << "SctInelServices" << ii;
      cylMat = createMaterial(nameStr.str(),
			      volType,
			      fractionRL,
			      (*inel)[ii]->getFloat("RMIN1")*CLHEP::cm,
			      (*inel)[ii]->getFloat("RMAX1")*CLHEP::cm,
			      halflength,
			      (*inel)[ii]->getFloat("RMIN2")*CLHEP::cm,
			      (*inel)[ii]->getFloat("RMAX2")*CLHEP::cm); 
    }

    std::string trtMaterialName = (*inel)[ii]->getString("RADL");
    if( trtMaterialName != "std::Carbon"){
      cylMat = materialManager()->getMaterial(trtMaterialName);
    }

    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    GeoVPhysVol* ServPhys = new GeoPhysVol(ServLog);
    double zpos = ((*inel)[ii]->getFloat("ZMAX")+(*inel)[ii]->getFloat("ZMIN"))/2.*CLHEP::cm+epsilon;
    // place two
    CLHEP::Hep3Vector servpos1(0.,0.,zpos);
    CLHEP::Hep3Vector servpos2(0.,0.,-zpos);
    GeoTransform *xform1 = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),servpos1));
    GeoTransform *xform2 = new GeoTransform(HepGeom::Transform3D( CLHEP::HepRotation(),servpos2));
    mother->add(xform1);
    mother->add(ServPhys);
    mother->add(xform2);
    mother->add(ServPhys);
  }

  
}


const GeoShape* SCT_ServMatFactoryDC3::createShape(int volType, 
						double rmin1, 
						double rmax1, 
						double halflength,
						double rmin2=0.,
						double rmax2=0.) 
  
{
  const double epsilon = 0.001*CLHEP::mm;
  enum VOLTYPE{Tube=1, Cone, ICone};
  const GeoShape* IDShape = 0;
  if(volType == Tube) {
    IDShape = new GeoTube(rmin1,rmax1,halflength);
  } 
  if(volType == Cone) {
    IDShape = new GeoCons(rmin1,rmin2-epsilon,rmax1,rmax2,halflength,0.,2*M_PI);
  }
  if(volType == ICone) {
    double zMid = halflength*(2*rmin1/rmax1-1.);
    GeoPcon* pPcon = new GeoPcon(0.,2*M_PI);
    pPcon->addPlane(-halflength, rmin1, rmax1);
    pPcon->addPlane(zMid, rmin1, rmax1);
    pPcon->addPlane(halflength, rmin1, rmin1);
    IDShape = pPcon;
  }
  return IDShape;
}

const GeoMaterial* SCT_ServMatFactoryDC3::createMaterial(const std::string & name,
						   int volType, 
						   double fractionRL,
						   double rmin1, 
						   double rmax1, 
						   double halflength,
						   double rmin2=0.,
						   double rmax2=0.) 
{
  double thickness=0.;
  enum VOLTYPE{Tube=1, Cone, ICone};
  if(volType == Tube) {
    thickness = std::min(2.*halflength, rmax1 - rmin1); 
  } 
  if(volType == Cone) {
    // There is a bug in AGE at this point, but the code is never called so it doesn't 
    // mater.
    double rmin = std::min(rmin1,rmin2);
    double rmax = std::max(rmax1,rmax2);
    thickness = std::min(2.*halflength, rmax - rmin); 
  }
  if(volType == ICone) {
    thickness = halflength*(rmin1/rmax1 + 1.);
  }
  
  // Make new material as carbon with density modified to give the right radiation length.

  const GeoMaterial* carbonMat= materialManager()->getMaterial("std::Carbon");
  double radiationLength = thickness / fractionRL;
  double density = carbonMat->getDensity() * carbonMat->getRadLength() / radiationLength;

  GeoMaterial * newMaterial = new GeoMaterial(name, density);
  const GeoElement * carbon = materialManager()->getElement("Carbon");
  newMaterial->add(const_cast<GeoElement *>(carbon),1.);
  newMaterial->lock();

  return newMaterial;
}

  
