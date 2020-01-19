// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    TIDAVertex.h        
//
//                   
//  
//
//   $Id: TIDAVertex.h, v0.0   Thu 22 Apr 2010 15:32:23 BST sutt $


#ifndef __TRACKVERTEX_H
#define __TRACKVERTEX_H

#include <iostream>

#include "TObject.h"


namespace TIDA { 

class Vertex : public TObject {

public:

  Vertex(double  x=0, double  y=0, double  z=0,
	 double dx=0, double dy=0, double dz=0, 
	 int Ntracks=0, double c2=0, int dof=0 ) { 
    m_x[0] = x;
    m_x[1] = y;
    m_x[2] = z;

    m_dx[0] = dx;
    m_dx[1] = dy;
    m_dx[2] = dz;

    m_Ntracks=Ntracks;
    m_chi2 = c2;
    m_ndof = dof;
  } 

  Vertex(const TIDA::Vertex& v) : TObject(v) { 
    for ( int i=3 ; i-- ; ) m_x[i]  = v.m_x[i];
    for ( int i=3 ; i-- ; ) m_dx[i] = v.m_dx[i];
    m_Ntracks=v.m_Ntracks;
    m_chi2=v.m_chi2;
    m_ndof=v.m_ndof;
  }


  virtual ~Vertex();

  const double* position() const { return m_x; }
  double*       position()       { return m_x; }

  double x() const { return m_x[0]; }
  double y() const { return m_x[1]; }
  double z() const { return m_x[2]; }

  const double* error() const { return m_dx; }
  double*       error()       { return m_dx; }

  double dx() const { return m_dx[0]; }
  double dy() const { return m_dx[1]; }
  double dz() const { return m_dx[2]; }

  int Ntracks() const  { return m_Ntracks; }

  double chi2()    const  { return m_chi2; }
  int    ndof()    const  { return m_ndof; }


private:

  double m_x[3];
  double m_dx[3];
  
  int    m_Ntracks;
  double m_chi2;
  int    m_ndof;

  ClassDef(TIDA::Vertex,2)

};


}

inline std::ostream& operator<<( std::ostream& s, const TIDA::Vertex& _t ) { 
  return s <<  "[ x=" << _t.position()[0]  << " +- " << _t.error()[0]
	   <<  "\ty=" << _t.position()[1]  << " +- " << _t.error()[1]
	   <<  "\tz=" << _t.position()[2]  << " +- " << _t.error()[2]
	   << ";\tchi2="   << _t.chi2()    << "/" << _t.ndof()
	   << "\tNtracks=" << _t.Ntracks() << "\t]"; 
}


#endif  // __TRACKVERTEX_H 










