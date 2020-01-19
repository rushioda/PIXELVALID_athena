// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    VertexMatcher.h        
//
//                   
//  
//
//   $Id: VertexMatcher.h, v0.0   Mon 10 Aug 2015 17:47:51 CEST sutt $


#ifndef  VERTEXMATCHER_H
#define  VERTEXMATCHER_H

#include <iostream>

#include "TrigInDetAnalysisUtils/BestMatcher.h"

#include "TrigInDetAnalysis/TIDAVertex.h"


class VertexMatcher : public BestMatcher<TIDA::Vertex> {

public:


  VertexMatcher( const std::string& s, double d=20) : BestMatcher<TIDA::Vertex>( s, d*d ) { }  

  ~VertexMatcher() { }
  
  virtual double distance( const TIDA::Vertex* v0, const TIDA::Vertex* v1 ) const {
    double d = (v0->z()-v1->z());
    return d*d;

    /// ha ha ha, divide by the "expected uncertainty" you must be MAD
    //    if ( v0->dz() && v1->dz() ) return d*d/( v0->dz()*v0->dz() + v1->dz()*v1->dz() );
    //    else                        return d*d;
 };
  
};

inline std::ostream& operator<<( std::ostream& s, const VertexMatcher& /* _v */ ) { 
  return s;
}


#endif  // VERTEXMATCHER_H 



