// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TrackMatchDeltaR.h        
//
//                   
// 
//
//   $Id: TrackMatchDeltaR.h, v0.0   Sun 18 Jan 2009 19:40:47 GMT sutt $


#ifndef TRIGINDETANALYSISUTILS_TRACKMATCHDELTAR_H
#define TRIGINDETANALYSISUTILS_TRACKMATCHDELTAR_H

#include <iostream>
#include <string>
#include <cmath>
#include <map>


#include "TrigInDetAnalysis/TrackAssociator.h"
#include "TrigInDetAnalysis/Track.h"


class TrackMatchDeltaR : public TrackAssociator {

public:

  TrackMatchDeltaR(const std::string& name, double R) : 
    TrackAssociator(name), m_R2(R*R) 
  { } 

  ~TrackMatchDeltaR() { } 

  //  virtual void match( const std::vector<Track*>& ref, const std::vector<Track*>& test ) { 
  virtual void match(const std::vector<TIDA::Track*>& s1, 
		     const std::vector<TIDA::Track*>& s2 ) 
  {
    clear();
    
    const std::vector<TIDA::Track*>& ref  = s1; 
    const std::vector<TIDA::Track*>& test = s2;

    // loop over reference tracks
    for ( int i=ref.size() ; i-- ; ) {
      
      TIDA::Track* reftrack = ref[i];
      
      //  find the closest track
      TIDA::Track* tmptrack = NULL;
      double R2min = 0;

      // loop over test tracks
      for ( int j=test.size() ; j-- ; ) { 

	TIDA::Track* testtrack = test[j];

	double R2tmp = deltaR2( reftrack, testtrack );

	// found a close track
	if ( tmptrack==NULL ||  R2tmp<R2min ) { 
	  R2min = R2tmp;
	  tmptrack = testtrack;
	} 
      }

      // is this inside the delta R specification?
      if ( tmptrack && R2min<m_R2 ) { 
	mmatched.insert(    map_type::value_type(reftrack,tmptrack) );
	mrevmatched.insert( map_type::value_type(tmptrack,reftrack) );
	
	std::cout << "\t\tSUTT matched " << *reftrack  << "\t -> \t" << *tmptrack << "\tDr=" << R2min << std::endl; 
      }
    }
  }
   

  virtual double deltaR2( const TIDA::Track* reftrack, const TIDA::Track* testtrack ) {
    double deta = reftrack->eta()-testtrack->eta();
    double dphi = reftrack->phi()-testtrack->phi();
    if ( dphi> M_PI ) dphi-=2*M_PI;
    if ( dphi<-M_PI ) dphi+=2*M_PI;
    return deta*deta+dphi*dphi;
  } 

private:

  double m_R2;

};


inline std::ostream& operator<<(std::ostream& s, const TrackMatchDeltaR& /*tm*/) { 
  return s;
}


#endif  // TRIGINDETANALYSISUTILS_TRACKMATCHDELTAR_H
