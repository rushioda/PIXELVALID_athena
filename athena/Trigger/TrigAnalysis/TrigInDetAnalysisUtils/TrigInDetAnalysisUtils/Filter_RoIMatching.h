/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/** @file Filter_RoIMatching.h */

#ifndef TrigInDetAnalysisUtils_Filter_RoIMatching_H
#define TrigInDetAnalysisUtils_Filter_RoIMatching_H


#include <iostream>
#include <string>
#include <cmath>
#include <map>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"
#include "TrigInDetAnalysis/TrackFilter.h"


class Filter_RoIMatching : public TIDA::TrackFilter {

 public:
  
  Filter_RoIMatching() {}
  
  bool select(const TIDA::Track* t) { 
    return true;
  }
  
  bool select(const TIDA::Track* t, const TrigInDetAnalysis::TIDARoiDescriptor* roi) { 
    double deltaEta = t->eta() - roi->eta();
    double deltaPhi = t->phi() - roi->phi(); if(deltaPhi<-M_PI)deltaPhi+=2*M_PI; if(deltaPhi>M_PI)deltaPhi-=2*M_PI;
    if(std::fabs(deltaEta)>roi->etaHalfWidth() || std::fabs(deltaPhi)>roi->phiHalfWidth()) return false;
    return true;
  }
  
};


#endif  // TrigInDetAnalysisUtils_Filter_RoIMatching_H
