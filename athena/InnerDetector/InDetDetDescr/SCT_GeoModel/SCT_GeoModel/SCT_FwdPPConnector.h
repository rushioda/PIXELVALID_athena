/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDPPCONNECTOR_H
#define SCT_GEOMODEL_SCT_FWDPPCONNECTOR_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

class GeoMaterial;

class SCT_FwdPPConnector : public SCT_SharedComponentFactory
{

public:
  SCT_FwdPPConnector(const std::string & name);

  //
  // Methods to return basic and derived parameters. 
  //
  const GeoMaterial * material() const {return m_material;}
  double deltaR() const {return m_deltaR;} 
  double rphi() const {return m_rphi;} 
  double thickness() const {return m_thickness;}


private:
  void getParameters();
  virtual GeoVPhysVol * build();

  // Basic parameters
  double m_deltaR;
  double m_rphi;
  double m_thickness;
  const GeoMaterial * m_material;
  std::string m_materialName;

};

#endif // SCT_GEOMODEL_SCT_FWDPPCONNECTOR_H

