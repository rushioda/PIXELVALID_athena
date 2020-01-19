/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_CLAMP_H
#define SCT_GEOMODEL_SCT_CLAMP_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;

class SCT_Clamp : public SCT_SharedComponentFactory
{

public:
  SCT_Clamp(const std::string & name, int iLayer,
                  double innerRadius);

public:
  const GeoMaterial * material() const {return m_material;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  double length() const {return m_length;} 

 
private:
  void getParameters();
  virtual GeoVPhysVol * build();

  int m_iLayer;

  const GeoMaterial * m_material;
  std::string m_materialName;
  double m_innerRadius;
  double m_outerRadius;
  double m_length;
  double m_radialWidth;
};

#endif // SCT_GEOMODEL_SCT_CLAMP_H

