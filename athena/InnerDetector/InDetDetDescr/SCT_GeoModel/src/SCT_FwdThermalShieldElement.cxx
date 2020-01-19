/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdThermalShieldElement.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

SCT_FwdThermalShieldElement::SCT_FwdThermalShieldElement(const std::string & name, int iElement)
  : SCT_SharedComponentFactory(name), m_iElement(iElement)
{
  getParameters();
  m_physVolume = build();
}


void 
SCT_FwdThermalShieldElement::getParameters()
{ 
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
  
  m_materialName = parameters->fwdThermalShieldMaterial(m_iElement);
  m_innerRadius = parameters->fwdThermalShieldInnerRadius(m_iElement);
  m_outerRadius = parameters->fwdThermalShieldOuterRadius(m_iElement);
  m_zMin        = parameters->fwdThermalShieldZMin(m_iElement);
  m_zMax        = parameters->fwdThermalShieldZMax(m_iElement);

  m_length = std::abs(m_zMax - m_zMin);

}

GeoVPhysVol * 
SCT_FwdThermalShieldElement::build() 
{
  SCT_MaterialManager materials;

  // Make the support disk. A simple tube.
  const GeoTube * elementShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  m_material = materials.getMaterialForVolume(m_materialName, elementShape->volume());
  if(!m_material) {m_material = materials.getMaterial(m_materialName);}
  const GeoLogVol * elementLog = new GeoLogVol(getName(), elementShape, m_material);

  GeoPhysVol * element = new GeoPhysVol(elementLog);

  return element;
}
