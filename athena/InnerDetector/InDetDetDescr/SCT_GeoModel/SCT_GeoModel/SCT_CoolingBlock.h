/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_COOLINGBLOCK_H
#define SCT_GEOMODEL_SCT_COOLINGBLOCK_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;


class SCT_CoolingBlock: public SCT_SharedComponentFactory

{

public:
  SCT_CoolingBlock(const std::string & name);

public:
  const GeoMaterial * material() const {return m_material;}
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}
  
private:
  void getParameters();
  virtual GeoVPhysVol * build();
  
  const GeoMaterial * m_material;
  std::string m_materialName;
  double m_thickness;
  double m_width;
  double m_length;

};

#endif // SCT_GEOMODEL_SCT_COOLINGBLOCK_H
