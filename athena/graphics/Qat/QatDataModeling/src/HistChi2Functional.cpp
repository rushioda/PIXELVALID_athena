//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#include "QatDataAnalysis/Hist1D.h"
#include "QatDataModeling/HistChi2Functional.h"
#include "CLHEP/GenericFunctions/DefiniteIntegral.hh"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include <algorithm>
#include <sstream>
#include <stdexcept> 
HistChi2Functional::HistChi2Functional(const Hist1D * histogram,
				       double minValue,
				       double maxValue,
				       bool integrate):
  _histogram(histogram),
  _minValue(minValue),
  _maxValue(maxValue),
  _integrate(integrate),
  _nBins(_histogram->nBins()),
  _min(_histogram->min()),
  _max(_histogram->max()),
  _width(_histogram->binWidth())
{
  _delta=(_max-_min)/_nBins;
}

HistChi2Functional::~HistChi2Functional() {
}

HistChi2Functional::HistChi2Functional (const HistChi2Functional & right):
  Genfun::AbsFunctional(),
  _histogram(right._histogram),
  _minValue(right._minValue),
  _maxValue(right._maxValue),
  _integrate(right._integrate),
  _nBins(right._nBins),
  _min(right._min),
  _max(right._max),
  _delta(right._delta),
  _width(right._width)
{
}


double HistChi2Functional::operator[] (const Genfun::AbsFunction & function) const{
  double c2=0;
  for (int i = 0; i<_nBins;i++) {
    double a = _min +i*_delta;
    double b = a+_delta;
    double y = _histogram->bin(i);
    double e = _histogram->binError(i);
    if (e!=0) {
      if (b>_minValue && a < _maxValue) {
	double y0;
	if (_integrate) {
	  Genfun::DefiniteIntegral integral(a,b);
	  y0=integral[function];
	}
	else {
	  y0 = function ((a+b)/2.0) * _width;
	  if (!finite(y0)) {
	    std::ostringstream message;
	    message << "HistChi2Functional, Function is infinite at " << (a+b)/2.0 << std::endl;
	    throw std::runtime_error(message.str());
	  }
	}
	double inc = (y-y0)*(y-y0)/e/e;
	if (!finite(inc)) {
	  std::ostringstream message;
	  message << "HistChi2Functional, Chi^2 contribution is infinite at " << (a+b)/2.0 << std::endl;
	  throw std::runtime_error(message.str());
	}
	c2 += inc; 
	if (!finite(c2)) {
	  std::ostringstream message;
	  std::cout << "HistChi2Functional, Chi2 is infinite at " << (a+b)/2.0 << std::endl;
	  throw std::runtime_error(message.str());
	}
      }
    } 
  }
  return c2;
}

