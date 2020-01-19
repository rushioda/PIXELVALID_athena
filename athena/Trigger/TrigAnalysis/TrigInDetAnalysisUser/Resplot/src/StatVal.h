// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**************************************************************************
 **
 **   File:         StatVal.h  
 **
 **   Description:  simple class containing the value and stat error
 **
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Mon Jun 21 18:35:22 BST 2004
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __STATVAL_H
#define __STATVAL_H

#include <iostream>
#include <iomanip>
#include <cmath>


/// simple struct to hold a value and it's associated uncertainty.

class StatVal {

public:

  double value;
  double error;

  StatVal()                    : value(0), error(0) { }  
  StatVal(double v) : value(v) { error=( v>=0 ? sqrt(v) : sqrt(-v) ); }
  StatVal(double v, double e)  : value(v), error(e) { }


  // operator for StatVal*=double 
  StatVal  operator*=(const double& d)      { value*=d; error*=d; return (*this); }
  // operator for StatVal*double (for double*StatVal see later)
  StatVal  operator*(const double& d) const { return StatVal(d*value, d*error); }
  // StatVal/double 
  StatVal  operator/(const double& d) const { return StatVal(value/d, error/d); }

  // can't be bothered with the rest...

};


// operator for double*StatVal
inline StatVal operator*(double d, const StatVal& sv) { return sv*d; }

// streamer;
inline std::ostream& operator<<(std::ostream& s, const StatVal& sv) { 
  return s << sv.value << "+-" << sv.error; 
}




#endif  /* __STATVAL_H */

