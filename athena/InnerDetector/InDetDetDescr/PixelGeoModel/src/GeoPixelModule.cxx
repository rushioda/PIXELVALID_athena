/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelModule.h"
#include "PixelGeoModel/GeoPixelHybrid.h"
#include "PixelGeoModel/GeoPixelChip.h"
#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"

using std::max;

GeoPixelModule::GeoPixelModule(GeoPixelSiCrystal& theSensor) :
  m_theSensor(theSensor)
{
  //
  // Define the log volume in the constructor, so I do it only once.
  //
  // The module orientation is
  // x normal to the detector
  // y in the phi direction
  // z in the eta (z) direction

  m_isModule3D=m_theSensor.GetModule3DFlag();
  m_moduleSvcThickness = 0.;
  m_moduleSvcWidth = 0.;
  nbModuleSvc = gmt_mgr->PixelModuleServiceNumber();
  int svcType = (m_isModule3D) ? 1 : 0;

  //
  // The Dimensions are in separate routines
  //
  double length = this->Length();
  double thickness = this->Thickness();
  double width = this->Width();
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");

  //  const GeoShape * moduleShape = 0;
  std::string logName = gmt_mgr->isBarrel() ? "ModuleBrl" : "ModuleEC";

  if (ThicknessP() == ThicknessN()) {
    const GeoBox* moduleBox = new GeoBox(thickness/2.,width/2.,length/2.);
    const GeoShape * moduleShape = moduleBox;
    theModule = new GeoLogVol(logName,moduleShape,air);
  } 
  else {

    // Shift so the center of the box is the center of the sensor.
    double shift = 0.5 * (ThicknessP() - ThicknessN_noSvc());
    //    const GeoShape & shiftedBox = (*moduleBox) << HepGeom::TranslateX3D(shift);
    //    moduleShape = &shiftedBox;
    
    thickness = ThicknessP()+ThicknessN_noSvc();
    const GeoBox* moduleBox = new GeoBox(thickness/2.,width/2.,length/2.);
    const GeoShape & shiftedBox = (*moduleBox) << HepGeom::TranslateX3D(shift);
    const GeoShape * moduleShape = &shiftedBox;

    if(m_moduleSvcThickness<0.001) {
      theModule = new GeoLogVol(logName,moduleShape,air);
    }
    else {
      const GeoShape * gblShape = 0;
      gblShape = addShape(gblShape, moduleShape, HepGeom::Transform3D() );

      double svcWidth = width*.6;
      m_moduleSvcWidth = svcWidth;
      const GeoBox* moduleSvcBox1 = new GeoBox(m_moduleSvcThickness*.5,svcWidth*.25,length*.5);
      double yShift = width*.5-svcWidth*.75;
      double xShift = thickness*.5+m_moduleSvcThickness*.5; 
      gblShape = addShape(gblShape, moduleSvcBox1, (HepGeom::TranslateX3D(-xShift)*HepGeom::TranslateY3D(-yShift)) );

      const GeoBox* moduleSvcBox2 = new GeoBox(m_moduleSvcThickness*.25,svcWidth*.25,length*.5);
      yShift = width*.5-svcWidth*.25;
      xShift = thickness*.5+m_moduleSvcThickness*.25; 
      gblShape = addShape(gblShape, moduleSvcBox2, (HepGeom::TranslateX3D(-xShift)*HepGeom::TranslateY3D(-yShift)) );

      for(int iSvc=0; iSvc<nbModuleSvc; iSvc++)
	{
	  int type = gmt_mgr->PixelModuleServiceModuleType(iSvc);
	  std::string name = gmt_mgr->PixelModuleServiceName(iSvc);
	  double offsetX = gmt_mgr->PixelModuleServiceOffsetX(iSvc);

	  if(type==svcType&&(name=="WingFlex"||offsetX<0)) {
	    double width_svc = gmt_mgr->PixelModuleServiceWidth(iSvc);
	    double thick_svc = gmt_mgr->PixelModuleServiceThick(iSvc);
	    double offsetY = gmt_mgr->PixelModuleServiceOffsetY(iSvc);
	    double xPos = -0.5*(gmt_mgr->PixelBoardThickness(m_isModule3D)-gmt_mgr->PixelHybridThickness(m_isModule3D)) - offsetX - thick_svc*.5;
	    const GeoBox* moduleSvcBox3 = new GeoBox(thick_svc*.5+.01,width_svc*.5+.01,length*.5+.01);
	    gblShape = addShape(gblShape, moduleSvcBox3, (HepGeom::TranslateX3D(xPos)*HepGeom::TranslateY3D(offsetY)) );
	  }
	}

      theModule = new GeoLogVol(logName,gblShape,air);	
    }
  }
  
}


GeoVPhysVol* GeoPixelModule::Build( ) {
  GeoFullPhysVol* modulePhys = new GeoFullPhysVol(theModule);
  //
  // Place the Si Crystal
  //
  GeoVPhysVol *theSi = m_theSensor.Build();
  _id = m_theSensor.getID();
  std::string sensorName = gmt_mgr->isBarrel() ? "SensorBrl" : "SensorEC";
  GeoNameTag *tag = new GeoNameTag(sensorName);         
  modulePhys->add(tag);
  // We give the barrel sensors an id of 100 and endcap sensors an id of 200 so that they can
  // can be distinguished in the G4 sensitive detector. For SLHC endcap we set it to 300.
  int idTag = 100; // barrel (same for slhc/non slhc)
  if (gmt_mgr->isEndcap()) {
    idTag = (gmt_mgr->slhc()) ? 300 : 200; // endcap (300: slhc, 200: non slhc)
  }
  modulePhys->add(new GeoIdentifierTag(idTag) );
  //Sensor is centered so we don't need the transform.
  //GeoTransform *xformsi = new GeoTransform(HepGeom::Transform3D());
  //modulePhys->add(xformsi);
  modulePhys->add(theSi );
  //
  // Place the Hybrid
  //
  if (gmt_mgr->PixelHybridThickness(m_isModule3D)>0.00001*CLHEP::mm){
    GeoPixelHybrid ph(m_isModule3D);
    double hybxpos = -0.5*(gmt_mgr->PixelBoardThickness(m_isModule3D)+gmt_mgr->PixelHybridThickness(m_isModule3D));
    GeoTransform* xform = new GeoTransform(HepGeom::TranslateX3D(hybxpos));
    modulePhys->add(xform);
    modulePhys->add(ph.Build() );
  }

  //
  // Place the Chip
  //
  GeoPixelChip pc(m_isModule3D);
  double chipxpos = 0.5*(gmt_mgr->PixelBoardThickness(m_isModule3D)+gmt_mgr->PixelChipThickness(m_isModule3D))+gmt_mgr->PixelChipGap(m_isModule3D);
  double chipypos =gmt_mgr->PixelChipOffset(m_isModule3D);
  GeoTransform* xform = new GeoTransform(HepGeom::TranslateX3D(chipxpos)*HepGeom::TranslateY3D(chipypos));
  modulePhys->add(xform);
  modulePhys->add(pc.Build() );

  //
  // Add the module services
  //
  if(nbModuleSvc==0) return modulePhys;
  
  int svcType = (m_isModule3D) ? 1 : 0;
  for(int iSvc=0; iSvc<nbModuleSvc; iSvc++)
    {
      int type = gmt_mgr->PixelModuleServiceModuleType(iSvc);
      
      if(type==svcType){
	double length = gmt_mgr->PixelModuleServiceLength(iSvc);
	double width = gmt_mgr->PixelModuleServiceWidth(iSvc);
	double thick = gmt_mgr->PixelModuleServiceThick(iSvc);
	double offsetX = gmt_mgr->PixelModuleServiceOffsetX(iSvc); 
	double offsetY = gmt_mgr->PixelModuleServiceOffsetY(iSvc);
	double offsetZ = gmt_mgr->PixelModuleServiceOffsetZ(iSvc);
	std::string name = gmt_mgr->PixelModuleServiceName(iSvc);
	std::string material = gmt_mgr->PixelModuleServiceMaterial(iSvc);

	const GeoBox* svcBox = new GeoBox(thick*.5-0.01,width*.5,length*.5);      
	const GeoMaterial* svcMat = mat_mgr->getMaterialForVolume(material,svcBox->volume());
	GeoLogVol* svcLogVol = new GeoLogVol(name, svcBox, svcMat);
	GeoPhysVol* svcPhys = new GeoPhysVol(svcLogVol);

	double xPos = -0.5*(gmt_mgr->PixelBoardThickness(m_isModule3D)-gmt_mgr->PixelHybridThickness(m_isModule3D)) - offsetX - thick*.5;
	double yPos = offsetY;
	double zPos = offsetZ;
	GeoTransform* xform = new GeoTransform(HepGeom::Translate3D(xPos,yPos,zPos));	
	modulePhys->add(xform);
	modulePhys->add(svcPhys);
      }
    }


  return modulePhys;
}

double GeoPixelModule::ThicknessN_noSvc() {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessN
  // is the max of ThicknessP and thickness from the module center to
  // the outer surface of the hybrid plus some safety.
  //
  double safety = 0.01*CLHEP::mm; 
  double thickn = 0.5 * gmt_mgr->PixelBoardThickness(m_isModule3D)+ gmt_mgr->PixelHybridThickness(m_isModule3D) + safety;
  double thick = max(thickn, ThicknessP()); 
  
  if(nbModuleSvc==0) return thick;
  
  double thickSvc=0;
  int svcType = (m_isModule3D) ? 1 : 0;
  for(int iSvc=0; iSvc<nbModuleSvc; iSvc++)
    {
      int type = gmt_mgr->PixelModuleServiceModuleType(iSvc);
      int fullSize = gmt_mgr->PixelModuleServiceFullSize(iSvc);
      if(type==svcType&&fullSize==1){
	double locThick = gmt_mgr->PixelModuleServiceThick(iSvc);
	double offsetX = gmt_mgr->PixelModuleServiceOffsetX(iSvc);
	double tmp = offsetX+locThick;
	thickSvc = std::max(thickSvc,tmp);
      }
    }

  return thick+thickSvc;

}


double GeoPixelModule::ThicknessN() {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessN
  // is the max of ThicknessP and thickness from the module center to
  // the outer surface of the hybrid plus some safety.
  //


  double safety = 0.01*CLHEP::mm; 
  double thickn = 0.5 * gmt_mgr->PixelBoardThickness(m_isModule3D)+ gmt_mgr->PixelHybridThickness(m_isModule3D) + safety;
  double thick = max(thickn, ThicknessP()); 

  if(nbModuleSvc==0) return thick;
  
  double thickSvc=0;
  int svcType = (m_isModule3D) ? 1 : 0;
  for(int iSvc=0; iSvc<nbModuleSvc; iSvc++)
    {
      int type = gmt_mgr->PixelModuleServiceModuleType(iSvc);
      if(type==svcType){
	double locThick = gmt_mgr->PixelModuleServiceThick(iSvc);
	double offsetX = gmt_mgr->PixelModuleServiceOffsetX(iSvc);
	double tmp = offsetX+locThick;
	thickSvc = std::max(thickSvc,tmp);
      }
    }
  m_moduleSvcThickness = thickSvc;

  return thick+thickSvc;
}


double GeoPixelModule::ThicknessP() {
  //
  // The module envelope is no longer forced to symmetric about its
  // center to allow for room between the module and TMT. ThicknessP
  // is thickness from the module center to the outer surface of the
  // chips plus some safety.

  double safety = 0.01*CLHEP::mm;
  double thick = 0.5 * gmt_mgr->PixelBoardThickness(m_isModule3D) +
    gmt_mgr->PixelChipThickness(m_isModule3D)+gmt_mgr->PixelChipGap(m_isModule3D) + safety;

  return thick;
}

double GeoPixelModule::Thickness() {
  // This is total thickness of the module envelope
  return ThicknessP()+ThicknessN();
}

double GeoPixelModule::Width() {

  double chipypos =fabs(gmt_mgr->PixelChipOffset(m_isModule3D));
  double width = max( max(
                     gmt_mgr->PixelBoardWidth(m_isModule3D),
                     gmt_mgr->PixelHybridWidth(m_isModule3D)),
                     gmt_mgr->PixelChipWidth(m_isModule3D)+2.*chipypos);
  return width;
}
double GeoPixelModule::Length() {
  // balcony is zero

  //  std::cout<<m_isModule3D<<" "<<gmt_mgr->PixelHybridLength(m_isModule3D)<<" "<<gmt_mgr->PixelBoardLength(m_isModule3D)<<" "<<gmt_mgr->PixelChipLength(m_isModule3D)<<std::endl;

  double length = max( max( //max(
                                gmt_mgr->PixelHybridLength(m_isModule3D),
                                gmt_mgr->PixelBoardLength(m_isModule3D)),
		       //2*gmt_mgr->PixelBalcony()),
                       gmt_mgr->PixelChipLength(m_isModule3D));

  return length;
}

Identifier GeoPixelModule::getID() {
  return _id;
}


const GeoShape * GeoPixelModule::addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans)
{
  const GeoShape * shiftedShape = &(*nextShape << trans);
  if (lastShape) {
    lastShape = &(lastShape->add(*shiftedShape));
  } else {
    lastShape = shiftedShape;
  }
  return lastShape;
}
