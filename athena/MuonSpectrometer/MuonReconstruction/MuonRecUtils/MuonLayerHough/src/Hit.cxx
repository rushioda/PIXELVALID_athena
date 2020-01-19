/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/HitNtuple.h"
#include <iostream>


namespace MuonHough {

  HitDebugInfo::HitDebugInfo() : 
    type(UNINITIALIZED),sector(UNINITIALIZED),region(Muon::MuonStationIndex::DetectorRegionUnknown),
    layer(Muon::MuonStationIndex::LayerUnknown),sublayer(UNINITIALIZED),pdgId(UNINITIALIZED),
    barcode(UNINITIALIZED),muonIndex(UNINITIALIZED),clusterSize(UNINITIALIZED),clusterLayers(UNINITIALIZED),
    isEtaPhi(UNINITIALIZED),trigConfirm(UNINITIALIZED),binpos(UNINITIALIZED),bintheta(UNINITIALIZED),
    time(UNINITIALIZED),r(UNINITIALIZED),ph(UNINITIALIZED),phn(UNINITIALIZED),
    ph1(UNINITIALIZED),ph2(UNINITIALIZED),rot(UNINITIALIZED) {}

  HitDebugInfo::HitDebugInfo( int type_, int sector_, Muon::MuonStationIndex::DetectorRegionIndex region_,
                              Muon::MuonStationIndex::LayerIndex layer_, int sublayer_ ) : 
    type(type_),sector(sector_),region(region_),layer(layer_),sublayer(sublayer_),pdgId(UNINITIALIZED),
    barcode(UNINITIALIZED),muonIndex(UNINITIALIZED),clusterSize(UNINITIALIZED),clusterLayers(UNINITIALIZED),
    isEtaPhi(UNINITIALIZED),trigConfirm(UNINITIALIZED),binpos(UNINITIALIZED),bintheta(UNINITIALIZED),
    time(UNINITIALIZED),r(UNINITIALIZED),ph(UNINITIALIZED),phn(UNINITIALIZED),
    ph1(UNINITIALIZED),ph2(UNINITIALIZED),rot(UNINITIALIZED) {}


  Hit::Hit( int layer_, float x_, float ymin_, float ymax_, float w_, 
	    HitDebugInfo* d_, const Trk::PrepRawData* prd_, const Muon::TgcClusterObj3D* tgc_ ) : 
    layer(layer_),x(x_),ymin(ymin_),ymax(ymax_),w(w_),
    prd(prd_),tgc(tgc_),m_debug(d_) {
  }

  Hit::~Hit() {
    delete m_debug;
  }

  Hit::Hit( const Hit& h_) {
    copy(h_);
  }
  
  Hit& Hit::operator=(const Hit& h_ ){
    if( &h_ != this ){
      delete m_debug;
      m_debug = 0;
      copy(h_);
    }
    return *this;
  }

  void Hit::copy( const Hit& hit ){
    layer = hit.layer;
    x = hit.x;
    ymin = hit.ymin;
    ymax = hit.ymax;
    w = hit.w;
    if( hit.m_debug ) m_debug = new HitDebugInfo(*hit.m_debug);
    else m_debug = 0;
    prd = hit.prd;
    tgc = hit.tgc;
  }

  PhiHit::PhiHit( int layer_, float r_, float phimin_, float phimax_, float w_, 
		  HitDebugInfo* d_, const Trk::PrepRawData* prd_, const Muon::TgcClusterObj3D* tgc_ ) : 
    layer(layer_),r(r_),phimin(phimin_),phimax(phimax_),w(w_),prd(prd_),tgc(tgc_),m_debug(d_) {}

  PhiHit::~PhiHit() {
    delete m_debug;
  }

  PhiHit::PhiHit( const PhiHit& h_) {
    copy(h_);
  }
  
  PhiHit& PhiHit::operator=(const PhiHit& h_ ){
    if( &h_ != this ){
      delete m_debug;
      m_debug = 0;
      copy(h_);
    }
    return *this;
  }

  void PhiHit::copy( const PhiHit& hit ){
    layer = hit.layer;
    r = hit.r;
    phimin = hit.phimin;
    phimax = hit.phimax;
    w = hit.w;
    if( hit.m_debug ) m_debug = new HitDebugInfo(*hit.m_debug);
    else m_debug = 0;
    prd = hit.prd;
    tgc = hit.tgc;
  }
}
