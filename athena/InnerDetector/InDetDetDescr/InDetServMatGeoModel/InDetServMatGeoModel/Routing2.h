/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Routing2_H
#define Routing2_H

#include "InDetServMatGeoModel/ServicesTracker.h"
#include "InDetServMatGeoModel/ServiceVolume.h"
#include "InDetServMatGeoModel/VRoute.h"
#include "InDetServMatGeoModel/HRoute.h"

#include "AthenaKernel/MsgStreamMember.h"
#include <vector>

class Routing2 {
public:

  typedef ServicesTracker::LayerContainer    LayerContainer;

  Routing2(const Athena::MsgStreamMember&);

  void createRoutingVolumes(ServicesTracker& tracker);

  const std::vector<ServiceVolume*>& volumes() const {return m_volumes;}

private:

  bool m_routePixelBarrelOnPST;
  bool m_pixelAlongBarrelStrip;
  bool m_ISTexists;

  int c_nInnerPixelLayers;  // FIXME: should come from tracker geometry

  double c_bpEosLength;
  double c_epEosLength;
  double c_bsEosLength;
  double c_safetyGap;

  double c_EosTolerance;
  double c_halfEosThick;
  double c_EosTolerance2;
  double c_ServiceDiskThickness;
  double c_ServiceCylinderThickness; //!
  double c_LayerLengthTolerance;

  std::vector<ServiceVolume*> m_volumes;
  ServicesLayer* previousBarrelLayer;

  VRoute m_bpVRoute;
  HRoute m_bpHRoute;
  HRoute m_epHRoute;

  VRoute m_pixelV2Route;
  HRoute m_pixelH2Route;

  VRoute m_bsVRoute;
  VRoute m_istVRoute;
  HRoute m_istHRoute;

  HRoute  m_ISTouter_HRoute; // Route running on outside of IST
  HRoute  m_MSTinside_HRoute; // Route running on outside of IST

  HRoute  m_MSTO_HRoute;
  HRoute  m_MSTM_HRoute;
  HRoute  m_MSTM_HRouteInner;
  HRoute  m_MSTI_HRoute;

  double eosTolerance( DetType::Type type, DetType::Part part);
  double eosLength( DetType::Type type, DetType::Part part);
  double eosHalfThickness( DetType::Type type, DetType::Part part);

  void connect( ServiceVolume* prev, ServiceVolume* newv);
  void routeBarrelLayer(LayerContainer::const_iterator bl, LayerContainer::const_iterator blend, ServicesTracker& tracker, VRoute& route);
  void routeEndcapLayer(LayerContainer::const_iterator bl, LayerContainer::const_iterator blend, ServicesTracker& tracker, HRoute& route);

  bool isRoutedOutsideSupportTube(LayerContainer::const_iterator bl, HRoute& route);
  void routeOuterBarrelPixel(ServicesTracker& tracker);
  void routeEndcapPixel(ServicesTracker& tracker);
  void routeBarrelStrip(ServicesTracker& tracker);
  void routeInnerBarrelPixel(ServicesTracker& tracker);

  void createRoutes(ServicesTracker& tracker);
  void createRoutesInIST(ServicesTracker& tracker);
  void createOuterPixelRoutes(ServicesTracker& tracker);

  void addVolume( ServiceVolume* v) { m_volumes.push_back( v);}

  ServiceVolume* createSingleRouteVolume( Route& rt);

  void connectRoutes( Route& in, Route& out);
  void dumpRoute( const Route& route);
  std::string nextVolumeName( const Route& route) const;

  // the message stream (same for all derived classes)
  const Athena::MsgStreamMember m_msg;
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  const Athena::MsgStreamMember& msgStream() const {return m_msg;}
  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }
};

#endif
