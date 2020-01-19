/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_InDetDDAthenaComps_H
#define InDetGeoModelUtils_InDetDDAthenaComps_H

// Message Stream Member
#include "AthenaKernel/MsgStreamMember.h"
class  StoreGateSvc;
class  IGeoModelSvc;
class  IRDBAccessSvc;
class  IGeometryDBSvc; 

#include <string>
namespace InDetDD {

/// Class to hold various Athena components.
class AthenaComps {

public:

  AthenaComps(const std::string & msgStreamName);

  //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

  void setDetStore(StoreGateSvc *);
  void setGeoModelSvc(IGeoModelSvc *);
  void setRDBAccessSvc(IRDBAccessSvc *);
  void setGeometryDBSvc(IGeometryDBSvc *);

  StoreGateSvc * detStore() const;
  IGeoModelSvc * geoModelSvc() const;
  IRDBAccessSvc * rdbAccessSvc() const;
  IGeometryDBSvc * geomDB() const;
  
private:
  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg;
  
  StoreGateSvc * m_detStore;
  IGeoModelSvc * m_geoModelSvc;
  IRDBAccessSvc * m_rdbAccessSvc;
  IGeometryDBSvc * m_geometryDBSvc;

};

inline StoreGateSvc * AthenaComps::detStore() const
{
  return m_detStore;
}

inline IGeoModelSvc * AthenaComps::geoModelSvc() const
{
  return m_geoModelSvc;
}

inline IRDBAccessSvc * AthenaComps::rdbAccessSvc() const
{
  return m_rdbAccessSvc;
}

inline IGeometryDBSvc * AthenaComps::geomDB() const
{
  return m_geometryDBSvc;
}

} // endnamespace

#endif // InDetGeoModelUtils_InDetDDAthenaComps_H

