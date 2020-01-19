/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDTHERMALSHIELDELEMENT_H
#define SCT_GEOMODEL_SCT_FWDTHERMALSHIELDELEMENT_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;

class SCT_FwdThermalShieldElement : public SCT_SharedComponentFactory
{

public:
  SCT_FwdThermalShieldElement(const std::string & name, int iElement);

public:
  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double zMax() const {return m_zMax;}
  double zMin() const {return m_zMin;}

  double length() const {return m_length;} 
  double radialThickness() const {return m_outerRadius - m_innerRadius;}
  double zPosition() const {return 0.5*(m_zMin+m_zMax);}

 
private:

  void getParameters();
  virtual GeoVPhysVol * build();

  std::string         m_materialName;
  const GeoMaterial * m_material;

  int m_iElement;
  double m_innerRadius;
  double m_outerRadius;
  double m_zMin;
  double m_zMax;

  double m_length;
};

#endif // SCT_GEOMODEL_SCT_FWDTHERMALSHIELDELEMENT_H

