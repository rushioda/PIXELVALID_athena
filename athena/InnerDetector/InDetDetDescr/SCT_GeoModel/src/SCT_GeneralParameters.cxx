/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "SCT_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "InDetGeoModelUtils/TopLevelPlacements.h"

#include <iostream>


const double SCT_SAFETY = 0.01 * CLHEP::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.


SCT_GeneralParameters::SCT_GeneralParameters()
{
  m_rdb = SCT_DataBase::instance();
  m_placements = new TopLevelPlacements(m_rdb->topLevelTable());
}


SCT_GeneralParameters::~SCT_GeneralParameters()
{
  delete m_placements;
}


const HepGeom::Transform3D & 
SCT_GeneralParameters::partTransform(const std::string & partName) const 
{
  return m_placements->transform(partName);
}


bool 
SCT_GeneralParameters::partPresent(const std::string & partName) const
{
  return m_placements->present(partName);
}



//
// General
//
double 
SCT_GeneralParameters::safety() const
{
  return SCT_SAFETY;
}

// Default Conditions. Values should be come form conditions data base. These values provide
// default vlaues if nothing from the conditions database is provided.


double 
SCT_GeneralParameters::temperature() const
{
  if (m_rdb->conditionsTable()->size() == 0) {
    return 266.15 * CLHEP::kelvin; // -7 C
  }
  return (m_rdb->conditions()->getDouble("TEMPERATURE") + 273.15) * CLHEP::kelvin;
}


double 
SCT_GeneralParameters::biasVoltage() const
{
  if (m_rdb->conditionsTable()->size() == 0) {
    return 100 * CLHEP::volt;
  }
  return m_rdb->conditions()->getDouble("BIASVOLT") * CLHEP::volt;
}

double 
SCT_GeneralParameters::depletionVoltage() const
{
  if (m_rdb->conditionsTable()->size() == 0) {
    return 20 * CLHEP::volt;
  }
  return m_rdb->conditions()->getDouble("DEPLETIONVOLT") * CLHEP::volt;
}



  
