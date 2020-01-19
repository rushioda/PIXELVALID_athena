/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Barrel.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"
#include "SCT_GeoModel/SCT_Identifier.h"

#include "SCT_GeoModel/SCT_Layer.h"
#include "SCT_GeoModel/SCT_Module.h"
#include "SCT_GeoModel/SCT_Sensor.h"
#include "SCT_GeoModel/SCT_InterLink.h"
#include "SCT_GeoModel/SCT_Spider.h"
#include "SCT_GeoModel/SCT_PixelAttachment.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>

SCT_Barrel::SCT_Barrel(const std::string & name)
  : SCT_UniqueComponentFactory(name)
{
  getParameters();
  m_logVolume = preBuild();
}


void
SCT_Barrel::getParameters()
{
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
    
  m_innerRadius = parameters->barrelInnerRadius();
  m_outerRadius = parameters->barrelOuterRadius();
  m_length =      parameters->barrelLength();
  m_numLayers =   parameters->numLayers(); 

  // Used in old geometry
  m_thermalShieldEndWallThickness = parameters->thermalShieldEndCapThickness();

  // Clearannce in z between layer and interlink.
  m_zClearance = 1*CLHEP::mm;

  // Layer internal structure and services depend on geometry version
  m_isOldGeometry = parameters->isOldGeometry();

  // Set numerology
  detectorManager()->numerology().setNumLayers(m_numLayers);

}

const GeoLogVol * 
SCT_Barrel::preBuild()
{
  // Create the barrel volume
  // Tube envelope containing the barrel.
  SCT_MaterialManager materials;
  const GeoTube * barrelEnvelopeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  GeoLogVol * barrelLog = 
    new GeoLogVol(getName(), barrelEnvelopeShape, materials.gasMaterial());
  return barrelLog;
}

GeoVPhysVol * 
SCT_Barrel::build(SCT_Identifier id) const
{

  GeoFullPhysVol * barrel = new GeoFullPhysVol(m_logVolume);
    
  // Old geometries are no longer supported - give up now if one is requested
  if(m_isOldGeometry) {
    msg(MSG::FATAL) << "Old barrel geometry versions are not supported" << endreq;
    return barrel;
  }

  // There is only one type of module. So we create it just the once and pass it to the layers.
  SCT_Module module("Module");

  // Create the interlinks
  SCT_InterLink interLink("InterLink");

  // Calculte the length of the layer cylinder. This is the barrel length less the thermal 
  // shield and interlink width.
  // This is only used for 'OldGeometry". In new geometry, layer length is set internally,
  // and is equal to support cylinder length
  double layerLength = m_length - 2*m_thermalShieldEndWallThickness - 2*interLink.length();

  // We reduce to allow some alignment clearance
  layerLength -= 2*m_zClearance;

  for (int iLayer = 0; iLayer < m_numLayers; iLayer++) {

    // Create the layers

    layerLength = 0.;
    SCT_Layer layer("Layer"+intToString(iLayer), iLayer, &module);
    barrel->add(new GeoNameTag("Layer#"+intToString(iLayer)));
    barrel->add(new GeoIdentifierTag(iLayer)); // Identifier layer= iLayer
    id.setLayerDisk(iLayer); 
    GeoAlignableTransform * transform = new GeoAlignableTransform(HepGeom::Transform3D());
    barrel->add(transform);
    GeoVPhysVol * layerPV = layer.build(id);
    barrel->add(layerPV);
    // Store alignable transform
    detectorManager()->addAlignableTransform(2, id.getWaferId(), transform, layerPV);
    layerLength = std::max(layerLength,layer.length());
  }

  // Build and place the interlinks
  double interLinkZPos = 0.;
  interLinkZPos = 0.5 * layerLength + m_zClearance + 0.5 * interLink.length();
  barrel->add(new GeoTransform(HepGeom::TranslateZ3D(+interLinkZPos)));
  barrel->add(interLink.getVolume());
  barrel->add(new GeoTransform(HepGeom::TranslateZ3D(-interLinkZPos)));
  barrel->add(interLink.getVolume());

  // Build and place the cooling spiders
  double spiderZPos = 0.;
  SCT_Spider spider("Spider");
  spiderZPos =  interLinkZPos + 0.5*interLink.length() + 0.5*spider.length();
  barrel->add(new GeoTransform(HepGeom::TranslateZ3D(+spiderZPos)));
  barrel->add(spider.getVolume());
  barrel->add(new GeoTransform(HepGeom::TranslateZ3D(-spiderZPos)));
  barrel->add(spider.getVolume());

  // Build and place the thermal shield.
  buildThermalShield(barrel);

  // Build and place the EMI shield (inner thermal shield).
  buildEMIShield(barrel);

  // Build and place SCT to Pixel attachment
  SCT_PixelAttachment pixelAttachment("AttachmentPixelToSCT");
  barrel->add(new GeoTransform(HepGeom::TranslateZ3D(+pixelAttachment.zPosition()))); // +ve z
  barrel->add(pixelAttachment.getVolume());
  barrel->add(new GeoTransform(HepGeom::TranslateZ3D(-pixelAttachment.zPosition()))); // -ve z
  barrel->add(pixelAttachment.getVolume());

  // Extra Material
  InDetDD::ExtraMaterial xMat(geometryManager()->distortedMatManager());
  xMat.add(barrel, "SCTBarrel");
  
  return barrel;
  
}

void SCT_Barrel::buildThermalShield(GeoFullPhysVol * parent) const
{
 
  // The thermal shield is now in 3 parts:
  // (a) outer cylinder
  // (b) bulkheads
  // (c) end panels
  // The inner cylinder is called 'EMI shield' for backwards compatibility

  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  SCT_MaterialManager materials;

  double cylinderOuterRadius   = parameters->thermalShieldOuterRadius();
  double cylinderInnerRadius   = parameters->thermalShieldInnerRadius();
  double cylinderLength        = parameters->cylinderLength();
  double bulkheadInnerRadius   = parameters->thermalShieldBulkheadInnerRadius();
  double bulkheadOuterRadius   = parameters->thermalShieldBulkheadOuterRadius();
  double bulkheadThickness     = parameters->thermalShieldEndCapCylThickness();
  double endPanelInnerRadius   = parameters->thermalShieldEndPanelInnerRadius();
  double endPanelOuterRadius   = parameters->thermalShieldEndPanelOuterRadius();
  double endPanelThickness     = parameters->thermalShieldEndCapThickness();
  double endPanelZMax          = parameters->thermalShieldEndZMax();
  
  std::string cylinderMaterialName = parameters->thermalShieldMaterialCyl();
  std::string bulkheadMaterialName = parameters->thermalShieldMaterialOuterSect();
  std::string endPanelMaterialName = parameters->thermalShieldMaterialInnerSect();


  // The outer cylinder part of thermal shield.
  const GeoTube * cylinderShape  = new GeoTube(cylinderInnerRadius, cylinderOuterRadius, 0.5*cylinderLength);
  const GeoMaterial* cylinderMaterial = materials.getMaterialForVolume(cylinderMaterialName,cylinderShape->volume());
  const GeoLogVol  * cylinderLog = new GeoLogVol("ThShieldOuterCyl", cylinderShape, cylinderMaterial);
  GeoPhysVol * cylinder = new GeoPhysVol(cylinderLog);
  parent->add(cylinder);

  // The bulkheads
  const GeoTube * bulkheadShape  = new GeoTube(bulkheadInnerRadius, bulkheadOuterRadius, 0.5*bulkheadThickness);
  const GeoMaterial* bulkheadMaterial = materials.getMaterialForVolume(bulkheadMaterialName,bulkheadShape->volume());
  const GeoLogVol  * bulkheadLog = new GeoLogVol("ThShieldBulkhead", bulkheadShape, bulkheadMaterial);
  GeoPhysVol * bulkhead = new GeoPhysVol(bulkheadLog);
  GeoTransform * bulkheadPosPlus  =  new GeoTransform(HepGeom::TranslateZ3D(+(endPanelZMax-endPanelThickness-0.5*bulkheadThickness)));
  GeoTransform * bulkheadPosMinus =  new GeoTransform(HepGeom::TranslateZ3D(-(endPanelZMax-endPanelThickness-0.5*bulkheadThickness)));
  parent->add(bulkheadPosPlus);
  parent->add(bulkhead);
  parent->add(bulkheadPosMinus);
  parent->add(bulkhead);

  // The end panels
  const GeoTube * endPanelShape  = new GeoTube(endPanelInnerRadius, endPanelOuterRadius,  0.5*endPanelThickness);
  const GeoMaterial* endPanelMaterial = materials.getMaterialForVolume(endPanelMaterialName,endPanelShape->volume());
  const GeoLogVol  * endPanelLog = new GeoLogVol("ThShieldEndPanel", endPanelShape, endPanelMaterial);
  GeoPhysVol * endPanel = new GeoPhysVol(endPanelLog);
  GeoTransform * endPanelPosPlus  =  new GeoTransform(HepGeom::TranslateZ3D(+(endPanelZMax-0.5*endPanelThickness)));
  GeoTransform * endPanelPosMinus =  new GeoTransform(HepGeom::TranslateZ3D(-(endPanelZMax-0.5*endPanelThickness)));
  parent->add(endPanelPosPlus);
  parent->add(endPanel);
  parent->add(endPanelPosMinus);
  parent->add(endPanel);

}

void SCT_Barrel::buildEMIShield(GeoFullPhysVol * parent) const
{
  
  const SCT_BarrelParameters * parameters = geometryManager()->barrelParameters();
  SCT_MaterialManager materials;

  // Parameters of cylinder 
  double innerRadius       = parameters->emiShieldInnerRadius();
  double deltaR            = parameters->emiShieldDeltaR();
  double outerRadius       = innerRadius + deltaR;
  double length            = 2 * parameters->emiShieldZMax();
  std::string materialName = parameters->emiShieldMaterial();

  // Parameters of Electrical Shield Joint
  double jointDeltaR = 0;
  double jointRPhi = 0;
  std::string jointMaterialName;
  if(!m_isOldGeometry) {
    jointDeltaR            = parameters->emiJointDeltaR();
    jointRPhi              = parameters->emiJointRPhi();
    jointMaterialName = parameters->emiJointMaterial();
  }

  // Parameters of Pixel Attachment - needed for cut-out
  double pixelAttachmentLength  = parameters->pixelAttachmentDeltaZ();
  double pixelAttachmentZpos    = parameters->pixelAttachmentZMin() + 0.5 * pixelAttachmentLength; 

  // Build cylinder (with cut-outs)
  const GeoShape * emiShieldShape  = 0;
  const GeoMaterial * material;
  const GeoTube * emiShieldTube  = new GeoTube(innerRadius, outerRadius,  0.5*length);
  //  std::cout << "EMI tube volume = " << emiShieldTube->volume() << std::endl; 
  if (m_isOldGeometry) {
    emiShieldShape = emiShieldTube;
    material = materials.getMaterial(materialName);
  } else {
    const GeoTube* cutOut = new GeoTube(innerRadius, outerRadius, 0.5*pixelAttachmentLength);
    //    std::cout << "Cut-out volume = " << cutOut->volume() << std::endl; 
    const GeoShape* emiTemp = (GeoShape*)&(emiShieldTube->subtract(*cutOut << HepGeom::TranslateZ3D(pixelAttachmentZpos)));
    emiShieldShape = (GeoShape*)&emiTemp->subtract(*cutOut << HepGeom::TranslateZ3D(-pixelAttachmentZpos));
    double emiVolume = emiShieldTube->volume() - 2. * cutOut->volume();
    //    std::cout << "EMI final volume = " << emiVolume << std::endl; 
    material = materials.getMaterialForVolume(materialName, emiVolume);
  }
  const GeoLogVol  * emiShieldLog = new GeoLogVol("EMIShield", emiShieldShape, material);
  GeoPhysVol * emiShield = new GeoPhysVol(emiShieldLog);
  parent->add(emiShield);

  // Build electrical shield joints (with cut-outs)
  if (!m_isOldGeometry) {
    double dphi = jointRPhi / outerRadius;
    const GeoTubs* emiJointTubs = new GeoTubs(outerRadius, outerRadius+jointDeltaR, 0.5*length,
						-0.5 * dphi * CLHEP::radian, dphi * CLHEP::radian);
    //    std::cout << "EMIJoint tubs volume = " << emiJointTubs->volume() << std::endl; 
    const GeoTubs* jointCutOut = new GeoTubs(outerRadius, outerRadius+jointDeltaR, 0.5*pixelAttachmentLength,
						-0.5 * dphi * CLHEP::radian, dphi * CLHEP::radian);
    //    std::cout << "Cut-out volume = " << jointCutOut->volume() << std::endl; 
    const GeoShape* jointTemp = (GeoShape*)&(emiJointTubs->subtract(*jointCutOut << HepGeom::TranslateZ3D(pixelAttachmentZpos)));
    const GeoShape* emiJointShape = (GeoShape*)&jointTemp->subtract(*jointCutOut << HepGeom::TranslateZ3D(-pixelAttachmentZpos));
    double jointVolume = emiJointTubs->volume() - 2. * jointCutOut->volume();
    //    std::cout << "EMIJoint final volume = " << jointVolume << std::endl; 
    const GeoMaterial * jointMaterial = materials.getMaterialForVolume(jointMaterialName, jointVolume);
    const GeoLogVol  * emiJointLog = new GeoLogVol("EMIShieldJoint", emiJointShape, jointMaterial);
    GeoPhysVol * emiJoint = new GeoPhysVol(emiJointLog);
    // Place 3 copies
    for (int i=0; i<3; i++) {
      double angle = (90. + i * 120.) * CLHEP::degree;
      parent->add(new GeoTransform(HepGeom::RotateZ3D(angle)));
      parent->add(emiJoint);
    }
  }
}
