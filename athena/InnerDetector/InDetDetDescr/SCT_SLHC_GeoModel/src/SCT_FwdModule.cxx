/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_FwdModule.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_FwdSpine.h"
#include "SCT_SLHC_GeoModel/SCT_FwdSensor.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"

#include <cmath>
#include <sstream>

namespace InDetDDSLHC {

inline double sqr(double x) {return x*x;}


SCT_FwdModule::SCT_FwdModule(const std::string & name, int ring, int doubleSided)
  : SCT_UniqueComponentFactory(name), m_ring(ring), m_doubleSided(doubleSided),
  m_stereoAngle(0),
  m_length(0),m_innerRadius(0),
  m_middleRadius(0),m_outerRadius(0),
  m_innerWidth(0), m_outerWidth(0),
  m_deltaPhi(0),m_thickness(0),
  m_moduleShift(0),m_interSidesGap(0),
  m_sensor(0), m_spine(0){
    getParameters();
    m_logVolume = preBuild();
  }	

SCT_FwdModule::~SCT_FwdModule(){
  delete m_spine;
  delete m_sensor;
}
  

void 
SCT_FwdModule::getParameters()
{
  const SCT_ForwardModuleParameters* parameters = geometryManager()->forwardModuleParameters();
  m_stereoAngle   = parameters->fwdModuleStereoAngle(m_ring);
  m_interSidesGap = parameters->moduleInterSidesGap(m_ring);
}


const GeoLogVol * SCT_FwdModule::preBuild(){  
  //int iring = m_ring%4;//the sensors are defined in Simulation/G4Atlas/G4AtlasApps/python/atlas_idet.py
  m_spine  = new SCT_FwdSpine("SCT_FwdSpine0", m_ring);
  m_sensor = new SCT_FwdSensor("ECSensor0", m_ring);

  //prepare the module envelope volume
  m_length       = std::max(m_sensor->length(), m_spine->length()) + 0.50*CLHEP::cm;//0.01mm safety necessary (for stereo angle)
  m_middleRadius = m_sensor->middleRadius();
  m_innerRadius  = m_middleRadius - 0.5*m_length;
  m_outerRadius  = m_middleRadius + 0.5*m_length;
  m_deltaPhi    = std::max(m_sensor->deltaPhi(), m_spine->deltaPhi());
  if(m_doubleSided){
    double interSidesGap = std::max(m_spine->thickness(), m_interSidesGap);
    m_thickness = 2*m_sensor->thickness() + interSidesGap + 0.01*CLHEP::mm;//0.01mm safety necessary
    //the term 10*CLHEP::degree*3.14/180, is to accommodate the stereo rotation
    m_deltaPhi    = m_deltaPhi + 10*CLHEP::degree*3.14/180.;
    //add 1cm, to accomodate for stereo rotation (to be dealt correctly with later)
    //m_innerRadius = m_innerRadius - 0.5*CLHEP::cm;
    //m_outerRadius = m_outerRadius + 0.5*CLHEP::cm;
    m_innerWidth = std::max(m_sensor->innerWidth(), m_spine->innerWidth()) + 2*CLHEP::cm; 
    m_outerWidth = std::max(m_sensor->outerWidth(), m_spine->outerWidth()) + 2*CLHEP::cm; 
  }else{
    m_thickness   = m_sensor->thickness() + m_spine->thickness() + 0.01*CLHEP::mm;//0.01mm safety necessary
    m_innerWidth = std::max(m_sensor->innerWidth(), m_spine->innerWidth()); 
    m_outerWidth = std::max(m_sensor->outerWidth(), m_spine->outerWidth());
  }

  const GeoTrd* moduleEnvelope = new GeoTrd(0.5*m_thickness, 0.5*m_thickness,
					    0.5*m_innerWidth, 0.5*m_outerWidth,  
					    0.5*m_length);
  SCT_MaterialManager * materials = geometryManager()->materialManager();
  GeoLogVol* moduleLog =  new GeoLogVol(getName(), moduleEnvelope, materials->gasMaterial());
  return moduleLog;
}

GeoVPhysVol* SCT_FwdModule::build(SCT_Identifier id) const{

  GeoFullPhysVol* module = new GeoFullPhysVol(m_logVolume);

  //first, calculate the module components positions
  HepGeom::Transform3D innerSidePos, spinePos, outerSidePos;
  if(m_doubleSided){
    //inner side position (shift this side towards the intreaction point, ie Z negative)
    //this is the thickness direction
    CLHEP::HepRotation inner_Rot;
    //inner_Rot.rotateZ(180*CLHEP::deg);
    inner_Rot.rotateX(0.5*m_stereoAngle);
    double interSidesGap = std::max(m_spine->thickness(), m_interSidesGap);
    double Xpos = 0.5*( interSidesGap + m_sensor->thickness());
    //protection
    if(fabs(Xpos)+0.5*m_sensor->thickness() > 0.5*m_thickness){
      std::cout<<"SCT_FwdModule.cxx: sensor position should be < than module half thickness, "
	       <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_sensor->thickness()
	       <<", halfmodthick = "<<0.5*m_thickness
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
    CLHEP::Hep3Vector  inner_Xpos = CLHEP::Hep3Vector(Xpos, 0.0, 0.0);
    innerSidePos = HepGeom::Transform3D(inner_Rot, inner_Xpos);
    //spine position (no shift)
    spinePos = HepGeom::Translate3D(CLHEP::Hep3Vector( 0.0, 0.0, 0.0));
    //outer side (shift towards X positive)
    CLHEP::HepRotation outer_Rot;
    outer_Rot.rotateZ(180*CLHEP::deg);
    outer_Rot.rotateX(-0.5*m_stereoAngle);
    Xpos = -0.5*(interSidesGap + m_sensor->thickness());
    //protection
    if(fabs(Xpos)+0.5*m_sensor->thickness() > 0.5*m_thickness){
      std::cout<<"SCT_FwdModule.cxx: sensor position should be < than module half thickness, "
	       <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_sensor->thickness()
	       <<", halfmodthick = "<<0.5*m_thickness
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
    CLHEP::Hep3Vector outer_Xpos = CLHEP::Hep3Vector(Xpos, 0.0, 0.0);
    outerSidePos = HepGeom::Transform3D(outer_Rot, outer_Xpos);
  }else{
    //inner side position (shift this side towards the intreaction point, ie X negative)
    CLHEP::HepRotation inner_Rot;
    inner_Rot.rotateX(0);//don't rotate (only one side)
    double Xpos = 0.5*m_spine->thickness();
    //protection
    if(fabs(Xpos)+0.5*m_sensor->thickness() > 0.5*m_thickness){
      std::cout<<"SCT_FwdModule.cxx: sensor position should be < than module half thickness, "
	       <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_sensor->thickness()
	       <<", halfmodthick = "<<0.5*m_thickness
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
    CLHEP::Hep3Vector  inner_Xpos = CLHEP::Hep3Vector(Xpos, 0.0, 0.0);
    innerSidePos = HepGeom::Transform3D(inner_Rot, inner_Xpos);
    //spine position (shift towards Z positive)
    Xpos = -0.5*m_sensor->thickness();
    //protection!
    if(fabs(Xpos)+0.5*m_spine->thickness() > 0.5*m_thickness){
      std::cout<<"SCT_FwdModule.cxx: sensor position should be < than module half thickness, "
	       <<"sensXpos+halfsensthick = "<<fabs(Xpos)+0.5*m_spine->thickness()
	       <<", halfmodthick = "<<0.5*m_thickness
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
    spinePos = HepGeom::Translate3D(CLHEP::Hep3Vector( Xpos, 0.0, 0.0));
  }

  //add the physical inner side to the mdoule
  GeoAlignableTransform* innerTransform = new GeoAlignableTransform(innerSidePos);
  int inner_id = 1;//the closest to suport disk is side 0
  id.setSide(inner_id);
  module->add(new GeoNameTag("Sensor_Side#"+intToString(inner_id)));
  // 600 is used to determine the hierarchy used in decoding the identifier in the sensitive detector
  module->add(new GeoIdentifierTag(600+inner_id));
  module->add(innerTransform);
  GeoVPhysVol* innerSidePV = m_sensor->build(id);
  module->add(innerSidePV);
  //Store transform
  detectorManager()->addAlignableTransform(0, id.getWaferId(), innerTransform, innerSidePV);
 
  //add the spine  
  module->add(new GeoTransform(spinePos));
  module->add(m_spine->getVolume());
  
  //add the outer side
  if(m_doubleSided){
    GeoAlignableTransform* outerTransform = new GeoAlignableTransform(outerSidePos);
    int outer_id = 0;//the closest to suport disk is side 0
    id.setSide(outer_id);
    module->add(new GeoNameTag("Sensor_Side#"+intToString(outer_id)));
    // 600 is used to determine the hierarchy used in decoding the identifier in the sensitive detector
    module->add(new GeoIdentifierTag(600+outer_id));
    module->add(outerTransform);
    GeoVPhysVol* outerSidePV = m_sensor->build(id);
    module->add(outerSidePV);
    // Store transform
    detectorManager()->addAlignableTransform(0, id.getWaferId(), outerTransform, outerSidePV); 
  }
  return module;
}
}
