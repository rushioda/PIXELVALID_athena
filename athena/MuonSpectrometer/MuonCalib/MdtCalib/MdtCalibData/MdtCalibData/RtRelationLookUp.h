/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIB_RTRELATIONLOOKUP_H
#define MUONCALIB_RTRELATIONLOOKUP_H

#include "MdtCalibData/IRtRelation.h"
#include <iostream>
#include <climits>

namespace MuonCalib{

  /** Equidistant look up table for rt-relations with the time as key. 
      The first parameter should be the time corresponding to the first bin.
      The second parameter should be the binsize.
      
      The r value is calculated by linear interpolation.
  */
  
  class RtRelationLookUp : public IRtRelation {
  public:
    explicit RtRelationLookUp( const ParVec &vec ) : IRtRelation(vec) {
      if( vec.size() < 4 ){
	t_min=9e9;
	bin_size=1.0; //will be always out of range
	std::cout << "RtRelationLookUp ERROR <to few parameters> " << std::endl;
      }else{
	t_min = par(0);
	bin_size = par(1);
	if (bin_size==0) {
	  std::cout << "RtRelationLookUp ERROR <bin size=0> " << std::endl;
	}
      }
    }
    
    std::string name() const {
      return "RtRelationLookUp"; 
    }

    /** returns drift radius for a given time */
    double radius( double t ) const;
    
    /** returns drift velocity for a given time */
    double driftvelocity( double t ) const;

    /** return rt range */
    inline double tLower(void) const;
    inline double tUpper(void) const;

  private:
    int getBin( double t ) const { 
    double t_minus_tmin(t-t_min);
    double rel=t_minus_tmin/bin_size;
    if (rel<static_cast<double>(INT_MIN))
      return INT_MIN;
    if (rel>static_cast<double>(INT_MAX))
      return INT_MAX;
    return static_cast<int>(rel); }
    
    // take offset due to t_min and binsize into account
    int rtBins() const { return nPar()-2; }
    double getRadius( int bin ) const { return par( bin + 2 ); }
    // returns best matching bin within rtRange
    int binInRtRange( double t) const;
      
    double t_min;
    double bin_size;
  };

  inline double RtRelationLookUp::radius( double t ) const {

    // get best matching bin in rt range
    int bin = binInRtRange( t );

    // shift bin so we are using the last two bins for extrapolation
    if( bin >= rtBins() - 1 ) bin = rtBins() - 2;

    double r1 = getRadius( bin );      // get bin value
    double r2 = getRadius( bin + 1 );  // get value of next bin
    double dr = r2-r1;

    // scale factor for interpolation
    double scale = (t-t_min)/bin_size-(double)bin;

    double r = r1 + dr*scale;

    return r >= 0 ? r : 0;
  }

  inline double RtRelationLookUp::driftvelocity( double t ) const {

    // get best matching bin in rt range
    int bin = binInRtRange( t );

    // shift bin so we are using the last two bins for extrapolation
    if( bin >= rtBins() - 1 ) bin = rtBins() - 2;

    double r1 = getRadius( bin );      // get bin value
    double r2 = getRadius( bin + 1 );  // get value of next bin
    double dr = r2-r1;

    double v = dr/bin_size;

    return v;
  }
  
  inline int RtRelationLookUp::binInRtRange( double t) const {
    // get bin 
    int bin = getBin( t );

    // if t corresponds to a negativ bin return first
    if( bin < 0 ) bin = 0;

    // if t corresponds to a bin outside the range of the lookup table return the last bin
    if( bin >= rtBins() ) bin = rtBins()-1;

    return bin;
  }

  inline double RtRelationLookUp::tLower(void) const {
    return t_min;
  }
	
  inline double RtRelationLookUp::tUpper(void) const {
    return t_min + bin_size * rtBins();
  }
  
}  //namespace MuonCalib
#endif
