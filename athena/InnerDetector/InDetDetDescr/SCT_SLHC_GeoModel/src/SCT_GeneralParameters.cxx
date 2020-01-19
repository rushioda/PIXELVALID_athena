/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "InDetGeoModelUtils/TopLevelPlacements.h"

#include <iostream>

namespace InDetDDSLHC {

const double SCT_SAFETY = 0.01 * CLHEP::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.

SCT_GeneralParameters::SCT_GeneralParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps)
{
  m_placements = new TopLevelPlacements(sctdb->topLevelTable());
  SctEnvelope = sctdb->sctEnvelope();
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
  // Check text file for DOBARREL or DOENDCAPS
  // This is a text file only parameter. "SctCommon" table does not exits in database.
  // For database use TopLevel table
  if (partName == "Barrel") {
    if (db()->testField("SctCommon","DOBARREL")) {
      return db()->getInt("SctCommon","DOBARREL");
    }
  } else if (partName == "EndcapA" || partName == "EndcapC") {
    if (db()->testField("SctCommon","DOENDCAPS")) {
      return db()->getInt("SctCommon","DOENDCAPS");
    }
    // Support old format text file
    if (db()->testField("","DOENDCAPS")) {
      return db()->getInt("","DOENDCAPS");
    }
  }
  return m_placements->present(partName);
}


double SCT_GeneralParameters::envelopeRMin() const 
{
  if (db()->getTableSize(SctEnvelope)) {    
    double rmin = envelopeRMin(0);  
    for (unsigned int i = 1; i < db()->getTableSize(SctEnvelope); i++) {
      rmin = std::min(rmin, envelopeRMin(i));
    } 
    return rmin;
  } else {      
    msg(MSG::ERROR) << "Unexpected error. SctEnvelope has zero size" << endreq;
    return 0;
  }
}

double SCT_GeneralParameters::envelopeRMax() const 
{
  if (db()->getTableSize(SctEnvelope)) {
    double  rmax = envelopeRMax(0);  
    for (unsigned int i = 1; i < db()->getTableSize(SctEnvelope); i++) {
      rmax = std::max(rmax, envelopeRMax(i));
    } 
    return rmax;
  } else {      
    msg(MSG::ERROR) << "Unexpected error. SctEnvelope has zero size" << endreq;
    return 0;
  }
}

double SCT_GeneralParameters::envelopeLength() const
{
  if (db()->getTableSize(SctEnvelope)) {
    // The table should contain only +ve z values.
    return 2*envelopeZ(db()->getTableSize(SctEnvelope) - 1);
  } else {
    msg(MSG::ERROR) << "Unexpected error. SctEnvelope has zero size" << endreq;
    return 0;
  }
}

bool SCT_GeneralParameters::simpleEnvelope() const
{
  // Return true if the envelope can be built as a simple tube.
  // otherwise it will be built as a PCON.
  // True if size is 1.
  return (db()->getTableSize(SctEnvelope) == 1);
}

bool SCT_GeneralParameters::booleanEnvelope() const
{
  // Return true if the envelope is not provided. Should be built as a boolean in that case.
  // True if size is 0.
  return (db()->getTableSize(SctEnvelope) == 0);
}


unsigned int SCT_GeneralParameters::envelopeNumPlanes() const
{
  return db()->getTableSize(SctEnvelope);
}

double SCT_GeneralParameters::envelopeZ(int i) const 
{
  double zmin =  db()->getDouble(SctEnvelope,"Z",i) * CLHEP::mm;
  if (zmin < 0) msg(MSG::ERROR) << "SctEnvelope table should only contain +ve z values" << endreq;
  return std::abs(zmin);
}

double SCT_GeneralParameters::envelopeRMin(int i) const 
{
  return db()->getDouble(SctEnvelope,"RMIN",i) * CLHEP::mm;
}

double SCT_GeneralParameters::envelopeRMax(int i) const
{
  return db()->getDouble(SctEnvelope,"RMAX",i) * CLHEP::mm;
}


//
// General
//
double 
SCT_GeneralParameters::safety() const
{
  return SCT_SAFETY;
}

}
