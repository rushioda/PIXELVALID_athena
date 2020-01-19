/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERN_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERN_H

#include "MuonHoughPatternEvent/MuonHoughHitContainer.h"
#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"
#include "DataModel/DataVector.h"

namespace MuonHough{
  /** enum to identify the different houghtransformers */
  enum MuonHoughTransformers {hough_xy,hough_rzcosmics,hough_curved_at_a_cylinder,hough_rz,hough_rz_mdt,hough_rz_rpc,hough_yz};
}

class MuonHoughPattern : public MuonHoughHitContainer
{
  /** MuonHoughPattern does not own its hits (contrary to the default) MuonHoughHitContainer! */

 public:
  /** constructor */
  MuonHoughPattern(int id_number, bool ownhits = false);

  /** destructor */
  virtual ~MuonHoughPattern();

  /** clear pattern */
  void resetTracksegment();
  
  /** returns if hit is in pattern */
  bool hitInHoughPattern(MuonHoughHit* hit)const;

  /** calculate estimated z-position of pattern */
  double calculateEZ()const;
  /** returns distance between first and last hit */
  double patternLength()const;
  
  /** prints out info about hough pattern */
  void printHoughPattern()const;
  
  /** returns id number of hough transform used to generate pattern */
  int getIdNumber() const;

  /** returns phi of pattern */
  double getEPhi()const;
  /** returns r0/d0 of pattern */
  double getERPhi()const;
  /** returns theta of pattern */
  double getETheta()const;
  /** returns z0 of pattern */
  double getERTheta()const;
  /** returns curvature of pattern */
  double getECurvature()const;
  /** returns maximum of histogram used to generate pattern */
  double getMaximumHistogram()const;

  /** set phi of pattern */
  void setEPhi(double ephi);
  /** set r0 of pattern */
  void setERPhi(double erphi);
  /** set theta of pattern */
  void setETheta(double etheta);
  /** set z0 of pattern */
  void setERTheta(double ertheta);
  /** set curvature of pattern */
  void setECurvature(double curvature);
  /** set maximum of histogram used to generate pattern */
  void setMaximumHistogram(double maximumhistogram);

  /** returns angle in precision plane in rad */ 
  double getEAngle()const;
  /** returns radius in precision plane in mm */ 
  double getER()const;
  /** set angle in precision plane in rad */ 
  void setEAngle(double eangle);
  /** set radius in precision plane in mm */ 
  void setER(double er);

  /** set which segment pattern is in, not in use */
  void setWhichSegment (bool which_segment);

  /** calulates 3d point closest to ip */
  std::vector<double> getEPos()const;
  /** calculates direction at point closest to ip */
  std::vector<double> getEDir()const;

  /** update parameters in rphi plane based on weighted fit*/
  void updateParametersRPhi(bool cosmics=false);

  typedef DataVector <MuonHoughPattern> MuonHoughPatternCollection;

 private:

  /** id number of hough transform used to generate pattern */
  int m_id_number;  
  
  /** which segment is pattern created in, not relevant if split search is off 
   * 0 lower segment, 1 uppersegment */
  bool m_whichsegment; 

  /** phi in rad */
  double m_ephi; 
  /** r0 in mm */
  double m_erphi;
  /** theta in rad */
  double m_etheta; 
  /** z0 in mm */
  double m_ertheta;
  /** curvature of pattern in rz plane in mm */
  double m_ecurvature; 
  /** maximum of histogram */
  double m_maximumhistogram;
  
  /** object for use of mathematical formulas for trackmodels */
  MuonHoughMathUtils m_muonhoughmathutils;
};

inline int MuonHoughPattern::getIdNumber() const {return m_id_number;}

inline double MuonHoughPattern::getEPhi()const{return m_ephi;}
inline double MuonHoughPattern::getERPhi()const{return m_erphi;}
inline double MuonHoughPattern::getETheta()const{return m_etheta;}
inline double MuonHoughPattern::getERTheta()const{return m_ertheta;}
inline double MuonHoughPattern::getECurvature()const{return m_ecurvature;}
inline double MuonHoughPattern::getMaximumHistogram()const{return m_maximumhistogram;}

inline void MuonHoughPattern::setEPhi(double ephi){m_ephi=ephi;}
inline void MuonHoughPattern::setERPhi(double erphi){m_erphi=erphi;}
inline void MuonHoughPattern::setETheta(double etheta){m_etheta=etheta;}
inline void MuonHoughPattern::setERTheta(double ertheta){m_ertheta=ertheta;}
inline void MuonHoughPattern::setECurvature(double ecurvature){m_ecurvature=ecurvature;}
inline void MuonHoughPattern::setMaximumHistogram(double maximumhistogram){m_maximumhistogram=maximumhistogram;}

inline void MuonHoughPattern::setWhichSegment (bool which_segment){m_whichsegment=which_segment;}
#endif // MUONHOUGHPATTERNEVENT_MUONHOUGHPATTERN_H
