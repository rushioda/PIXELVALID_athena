/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_BaseBoard.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_BarrelModuleParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

SCT_BaseBoard::SCT_BaseBoard(const std::string & name)
  : SCT_SharedComponentFactory(name)
{
  getParameters();
  m_physVolume = build();
}

void
SCT_BaseBoard::getParameters() 
{
  const SCT_GeneralParameters * generalParameters = geometryManager()->generalParameters();
  const SCT_BarrelModuleParameters * parameters = geometryManager()->barrelModuleParameters();

  SCT_MaterialManager materials;
 
  m_material  = materials.getMaterial(parameters->baseBoardMaterial());
  m_safety    = generalParameters->safety();
  m_thickness = parameters->baseBoardThickness();
  m_width     = parameters->baseBoardWidth();
  m_length    = parameters->baseBoardLength();

}

GeoVPhysVol * 
SCT_BaseBoard::build()
{
  // Make BaseBoard. Just a simple box.
  const GeoBox * simpleBaseBoardShape = new GeoBox(0.5*m_thickness,
						   0.5*m_width,
						   0.5*m_length);

  const GeoLogVol * simpleBaseBoardLog = 
    new GeoLogVol(getName(), simpleBaseBoardShape, m_material);

  GeoPhysVol * simpleBaseBoard = new GeoPhysVol(simpleBaseBoardLog);

  return simpleBaseBoard;
}
