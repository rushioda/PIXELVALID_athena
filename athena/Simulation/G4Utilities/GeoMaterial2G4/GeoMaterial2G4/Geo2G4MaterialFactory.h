/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMATERIAL2G4_Geo2G4MaterialFactory_h
#define GEOMATERIAL2G4_Geo2G4MaterialFactory_h

#include "G4Material.hh"
#include "GeoModelKernel/GeoMaterial.h"
#include "AthenaKernel/MsgStreamMember.h"
#include <map>
#include <string>
typedef std::map<const GeoMaterial* , G4Material*, std::less<const GeoMaterial*> > matList;
typedef std::map<std::string, const GeoMaterial*, std::less<std::string> > matNames;

class Geo2G4MaterialFactory {
public:
  Geo2G4MaterialFactory();
  G4Material* Build(const GeoMaterial*);
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
private:
  matList definedMaterials;
  matNames definedMatNames;
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};

#endif
