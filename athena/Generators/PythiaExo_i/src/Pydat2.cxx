/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// access pythia common Pydat2
#include "PythiaExo_i/Pydat2.h"

#include <cstdlib>

// set pointer to zero at start
Pydat2::PYDAT2* Pydat2::s_pydat2 =0;

// Constructor
Pydat2::Pydat2() 
{
  m_dummy = -999;
  m_realdummy = -999.;
}

// Destructor
Pydat2::~Pydat2() 
{
}

// access kchg in common
int& Pydat2::kchg(int kc, int i) {
  init(); // check COMMON is initialized
  if( kc < 1 || kc > lenPmas() ||
      i  < 1 || i  > depthKchg())
 {
  m_dummy = -999;
  return m_dummy;
  }
  return s_pydat2->kchg[i-1][kc-1];
}
// access pmas in common
double& Pydat2::pmas(int kc, int i) {
  init(); // check COMMON is initialized
  if( kc < 1 || kc > lenPmas() ||
      i  < 1 || i  > depthPmas())
 {
  m_realdummy = -999.;
  return m_realdummy;
  }
  return s_pydat2->pmas[i-1][kc-1];
}

// access parf in common
double& Pydat2::parf(int n) {
  init(); // check COMMON is initialized
  if(n < 1 || n > lenParf()) {
  m_realdummy = -999.;
  return m_realdummy;
  }
  return s_pydat2->parf[n-1];

}// access vckm in common
double& Pydat2::vckm(int i,int j) {
  init(); // check COMMON is initialized
  if(i < 1 || i > lenVckm() ||
     j < 1 || j > lenVckm()) {
  m_realdummy = -999.;
  return m_realdummy;
  }
  return s_pydat2->vckm[j-1][i-1];
}

