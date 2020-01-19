/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/TRT_ServMatFactoryDC3.h"

// GeoModel includes
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoTubs.h"  
#include "GeoModelKernel/GeoPcon.h"  
#include "GeoModelKernel/GeoCons.h"  
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTransform.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"


#define NUMBEROFPANEL 2
#define TRTELEMENTSINEL 9  // VK - now number of record is determined automatically

#include <sstream>
#include <iostream>

TRT_ServMatFactoryDC3::TRT_ServMatFactoryDC3(const InDetDD::AthenaComps * athenaComps, 
					     InDetMaterialManager * matManager)
  : InDetDD::SubDetectorFactoryBase(athenaComps, matManager)
{  
}


TRT_ServMatFactoryDC3::~TRT_ServMatFactoryDC3()
{

}



//## Other Operations (implementation)
void TRT_ServMatFactoryDC3::create(GeoPhysVol *mother)
{

  msg(MSG::DEBUG) << "Building TRT Service Material" << endreq;

  double epsilon = 0.002;
  

  DecodeVersionKey atlasVersionKey(geoModelSvc(), "ATLAS");
  DecodeVersionKey indetVersionKey(geoModelSvc(), "InnerDetector");

  IRDBRecordset_ptr atls = rdbAccessSvc()->getRecordsetPtr("AtlasMother",  atlasVersionKey.tag(), atlasVersionKey.node());
  IRDBRecordset_ptr ipan = rdbAccessSvc()->getRecordsetPtr("IPAN", indetVersionKey.tag(), indetVersionKey.node());
  IRDBRecordset_ptr inel = rdbAccessSvc()->getRecordsetPtr("INEL", indetVersionKey.tag(), indetVersionKey.node());


//VVK  10/09/2005 Construct a gap for rails
    double outROfIDet =       (*atls)[0]->getDouble("IDETOR")*CLHEP::cm;
    double endZOfIDet =       (*atls)[0]->getDouble("IDETZMX")*CLHEP::cm;
    double minRofGap  =       1050.0;
    double phiWid=70./outROfIDet;    double safetyGap=1.;
    const GeoShape* railGap1=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap , 
                                          -phiWid/2.,phiWid);
    const GeoShape* railGap2=new GeoTubs( minRofGap, outROfIDet+safetyGap ,endZOfIDet+safetyGap ,
                                          -phiWid/2.+M_PI,phiWid);


  // Build the patch panels:
  
  for (int ii=0; ii<NUMBEROFPANEL; ii++) {
    const GeoMaterial* cylMat = materialManager()->getMaterial("trt::PatchOut");
    double rmin = (*ipan)[ii]->getFloat("RMIN")*CLHEP::cm;
    double rmax = (*ipan)[ii]->getFloat("RMAX")*CLHEP::cm;
    double zmin = (*ipan)[ii]->getFloat("ZMIN")*CLHEP::cm;
    double zmax = (*ipan)[ii]->getFloat("ZMAX")*CLHEP::cm;

    double halflength = (zmax-zmin)/2.-2*epsilon;
    double zpos = zmin + halflength+2*epsilon;
    // Build the Phys Vol
    std::ostringstream o;
    o << ii;
    std::string logName = "TrtPanel"+o.str();
    const GeoShape* serviceTube = 0;
    if ( fabs(rmax) < minRofGap ){    
      serviceTube = new GeoTube(fabs(rmin),fabs(rmax),halflength);
    }else{
//
//VVK Subtract RailGap out of services
      const GeoShape* serviceTubeTmp1 = new GeoTube(fabs(rmin),fabs(rmax),halflength);
      const GeoShape* serviceTubeTmp2 = (GeoShape*) & (*serviceTubeTmp1).subtract(*railGap1);
      serviceTube     = (GeoShape*) & (*serviceTubeTmp2).subtract(*railGap2);
    }

    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    
    GeoVPhysVol* ServPhys = new GeoPhysVol(ServLog);
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

  
// Build the INEL part of the services:
//
// VK - change of INEL record order in ORACLE
//
//  for (int ii =0; ii < TRTELEMENTSINEL; ii++) {

  int NRECORDS = (int) (*inel).size();
  int irecold=0;
  for (int ii =0; ii < NRECORDS; ii++) {

    if ((*inel)[ii]->getFloat("IBLOC") > 11.0) continue; //VK skip SCT services 

    // Build the Phys Vol
    std::ostringstream o;
//    o << ii;  //VK
    o << irecold++;
    std::string logName = "TrtInel"+o.str();  
    int volType = (int) (*inel)[ii]->getFloat("VOLTYP");
    double RMIN1=(*inel)[ii]->getFloat("RMIN1")*CLHEP::cm;
    double RMAX1=(*inel)[ii]->getFloat("RMAX1")*CLHEP::cm;
    double RMIN2=(*inel)[ii]->getFloat("RMIN2")*CLHEP::cm;
    double RMAX2=(*inel)[ii]->getFloat("RMAX2")*CLHEP::cm;
    double ZMAX= (*inel)[ii]->getFloat("ZMAX")*CLHEP::cm;
    double ZMIN= (*inel)[ii]->getFloat("ZMIN")*CLHEP::cm;

//VK Change of TRT barrel cables definition
//    if(ii == 3) { RMIN1 += 0; RMAX1=RMIN1+0.589; ZMIN=950.; ZMAX=3250;}
//    if(ii == 4) { RMIN1 += 2; RMAX1=RMIN1+1.285; ZMIN=950.; ZMAX=3250;}
//    std::cout << ii<<", "<<RMIN1<<", "<<RMAX1<<", "<<(*inel)[ii]->getFloat("IBLOC")<<'\n';

    double halflength = (ZMAX-ZMIN)/2.;

    const GeoShape* serviceTubeTmp1 = createShape(volType,RMIN1,RMAX1, halflength,RMIN2,RMAX2);

    const GeoShape* serviceTube = serviceTubeTmp1;
    if( RMAX1  > minRofGap   || RMAX2 > minRofGap     )  {
//VVK Subtract RailGap out of services
       const GeoShape* serviceTubeTmp2 = (GeoShape*) & (*serviceTubeTmp1).subtract(*railGap1);
                       serviceTube     = (GeoShape*) & (*serviceTubeTmp2).subtract(*railGap2);
    }

    // create the material...
    // In AGE the radiation length is specified and from that the density is
    // calculated assuming the material is C. I do the same here for now but
    // will eventually define all the materials.
    double fractionRL = (*inel)[ii]->getFloat("MATER");
    std::string trtMaterialName = (*inel)[ii]->getString("RADL");
    const GeoMaterial* cylMat = 0;

    // 
    if( trtMaterialName != "std::Carbon"){ 
      cylMat = materialManager()->getMaterial(trtMaterialName);
    } else { // old AGE style material - modified version of carbon.
      if (fractionRL > 0.999) { 
	// In this case a fixed material is used called Services which is the same
	// as trt::PatchOut
	cylMat = materialManager()->getMaterial("trt::PatchOut");
      } else {
	std::ostringstream nameStr;
	nameStr << "TrtInelServices" << ii;
	cylMat = createMaterial(nameStr.str(),volType,fractionRL,RMIN1,RMAX1,halflength,RMIN2,RMAX2); 
      }
    }


    const GeoLogVol* ServLog = new GeoLogVol(logName,serviceTube,cylMat);
    GeoVPhysVol* ServPhys = new GeoPhysVol(ServLog);
    double zpos = (ZMAX+ZMIN)/2.+epsilon;
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


  const GeoShape* TRT_ServMatFactoryDC3::createShape(int volType, 
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

const GeoMaterial* TRT_ServMatFactoryDC3::createMaterial(const std::string & name,
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

  
