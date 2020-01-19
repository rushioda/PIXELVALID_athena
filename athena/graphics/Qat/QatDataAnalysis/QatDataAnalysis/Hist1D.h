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


#ifndef _HIST1D_H_
#define _HIST1D_H_
#include <string>
#include <vector>
#include <cmath>
class Hist1D {

 public:



  // Constructor:
  Hist1D(const std::string & name, size_t nBins, double min, double max);

  // Copy Constructor:
  Hist1D(const Hist1D & source); 

  // Destructor
  ~Hist1D();

  // Accumulate Data:
  void accumulate(double x, double weight=1);

  // Properties of the container:
  inline std::string & name(); 
  inline const std::string & name()  const;
  inline size_t nBins() const;
  inline double min() const;
  inline double max() const;
  inline double binWidth() const;

  // Properties of the bins::  
  inline double binUpperEdge(unsigned int i) const;
  inline double binLowerEdge(unsigned int i) const;
  inline double binCenter(unsigned int i) const;

  // Stored data
  inline double bin(unsigned int i) const;
  inline double binError(unsigned int i) const;
  inline size_t overflow() const;
  inline size_t underflow() const;
  inline size_t entries() const;

  // Statistical properties of the data:
  inline double variance() const;
  inline double mean() const;
  inline double minContents() const;
  inline double maxContents() const;
  inline double sum() const;

  // Operations:
  Hist1D & operator  = (const Hist1D & source);
  Hist1D & operator += (const Hist1D & source);
  Hist1D & operator -= (const Hist1D & source);
  Hist1D & operator *= (double scale);
  
  // Clear:
  void clear ();

  // The internals are described like this:
  class Clockwork; 
  
  // Get the internals:
  const Clockwork *clockwork() const;
  
  // Remake this from the internals:
  Hist1D(const Clockwork * c);

 private:

  Clockwork           *c;
                                              
};

#include "QatDataAnalysis/Hist1D.icc"

#endif

