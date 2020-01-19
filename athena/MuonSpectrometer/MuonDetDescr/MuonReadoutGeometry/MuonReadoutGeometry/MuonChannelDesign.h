/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 properties of a plane based detector allowing for a stereo angle
 ----------------------------------------------------------------------
***************************************************************************/

#ifndef MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H
#define MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonGM {

  struct MuonChannelDesign {
  public:
    enum Type {
      etaStrip=0, ///< 0 (eta strips, locX || eta, including stereo angle - MM, sTGC strips)
      phiStrip=1, ///< 1 (phi strips  locX || phi, no stereo angle        - sTGC wire/gangs)
      pad=2       ///< 2 (pads        locX || eta,                        - sTGC pads)
    };
  public:
    int    type;
    int    nch;
    double sAngle;        //  
    double inputPitch;
    double inputWidth;
    double inputLength;
    double deadI;
    double deadO;
    double deadS;
    double signY;
    //Amg::Vector2D firstChannelPos;
    double firstPos;
    double xSize;
    double minYSize;
    double maxYSize;
    double thickness;

    /** channel transform */
    //HepGeom::Transform3D  channelTransform( int channel ) const;

    /** distance to readout */
    double distanceToReadout( const Amg::Vector2D& pos ) const;
    //double distanceToReadout( double locY ) const;

    /** distance to channel - residual */
    double distanceToChannel( const Amg::Vector2D& pos, int nChannel=0 ) const;
    //double distanceToReadout( double locY ) const;

    /** calculate local channel number, range 1=nstrips like identifiers. Returns -1 if out of range */
    int channelNumber( const Amg::Vector2D& pos ) const;
    //int stripNumber( double locX ) const;

    /** calculate local channel position for a given channel number */
    bool  channelPosition( int channel, Amg::Vector2D& pos  ) const;

    /** calculate local channel width */
    double  channelWidth( Amg::Vector2D pos  ) const;

    /** calculate local stereo angle */
    double  stereoAngle( int channel  ) const;

    /** calculate channel length for a given channel number */
    double  channelLength( int channel ) const;

    /** thickness of gas gap */
    double gasGapThickness() const;
  };

  inline double MuonChannelDesign::distanceToReadout( const Amg::Vector2D& pos ) const {
    
    int chNum = channelNumber( pos );

    if (chNum <0 ) return -1.;
    Amg::Vector2D chPos;
    if (!channelPosition( chNum, chPos) ) return -1.;

    double sA = stereoAngle(chNum);
 
    Amg::Vector2D chLoc( (pos.x()-chPos.x())*cos(sA)-(pos.y()-chPos.y())*sin(sA),
			      + (pos.x()-chPos.x())*sin(sA)+(pos.y()-chPos.y())*cos(sA) );

    double stripL = channelLength(chNum); 
   
    double dist = signY>0 ? chLoc.y()+0.5*stripL : 0.5*stripL-chLoc.y();
    if( dist < 0. ) return 0.;
    if( dist > stripL ) return stripL;
    return dist;

  }

  inline double MuonChannelDesign::distanceToChannel( const Amg::Vector2D& pos, int chNum ) const {
    
    // if channel number not provided, get the nearest channel ( mostly for validation purposes )
    bool validMode = false;

    if (chNum < 1 || chNum> nch ) {
      chNum =  channelNumber( pos);
      validMode = true;
    }

    Amg::Vector2D chPos;
    if (!channelPosition( chNum, chPos) ) return -10000.;

    double sA = stereoAngle(chNum);
 
    Amg::Vector2D chLoc( (pos.x()-chPos.x())*cos(sA)-(pos.y()-chPos.y())*sin(sA),
			      + (pos.x()-chPos.x())*sin(sA)+(pos.y()-chPos.y())*cos(sA) );

    if ( validMode && fabs(chLoc.x()) > 0.5*channelWidth( pos) ) {
      std::cout << "problem in identification of the channel: distance to nearest channel, channel width:"
		<<chLoc.x()<<","<< channelWidth(pos) << std::endl;  
    }
   
    return chLoc.x();
  }

  inline int MuonChannelDesign::channelNumber( const Amg::Vector2D& pos ) const {
 
    if (type==MuonChannelDesign::etaStrip) {      // "eta"  orientation , assumes constant stereo angle

      double xMfirst = firstPos;
      double xMid = pos.x() - pos.y()*tan(sAngle);
      int chNum = int( cos(sAngle)*(xMid - xMfirst)/inputPitch+1.5 );
      if (chNum<1) return -1;
      if (chNum>nch) return -1;     // used also for calculation of the number of strips
      return chNum;

    } else if (type==MuonChannelDesign::phiStrip) {   // "phi" orientation, local coordinates rotated

      // find transverse pannel size for a given locX 
      int chNum = int( (pos.x()-firstPos)/inputPitch +1.5 ) ;
      if (chNum<1) return -1;
      if (chNum>nch) return -1;  
      return chNum;   

    } else if(type==MuonChannelDesign::pad) {
      // DG-2015-11-27 to be implemented
      return -1;
    }
    return -1;

  }


  inline bool MuonChannelDesign::channelPosition( int st, Amg::Vector2D& pos  ) const {
    if( st < 1 ) return false;
    if( st > nch ) return false;

    double dY = 0.5*(maxYSize-minYSize-2.*deadS);

    if ( type==MuonChannelDesign::phiStrip ) {   // swap coordinates on return

      double locY = firstPos+ (st-1)*inputPitch;

      double locX = 0.;

      if (fabs(locY)>0.5*(minYSize-deadS)) {
         
        //double gangLength = (0.5*maxYSize - fabs(locY))/dY * xSize;

        locX = 0.5*(xSize-deadO-deadI)*( 1. - (0.5*(maxYSize-deadS) -fabs(locY))/dY )+0.5*(deadI-deadO) ;

      }

      pos[0] = locY;
      pos[1] = locX;
 
      return true; 

    } else if ( type==MuonChannelDesign::etaStrip ) {

      if (sAngle==0.) {

	double x = firstPos + inputPitch*(st-1);
	pos[0] = x;
	pos[1] = 0;
	return true;

      }

      // strip central position
      double xMid = firstPos + (st-1)*inputPitch/cos(sAngle);

      std::vector<std::pair<double,double> > stripEnd;
      // intersection with the xUp boundary
      double yU = (0.5*xSize-deadO-xMid)/tan(sAngle);
      if (fabs(yU)<=0.5*maxYSize-deadS) stripEnd.push_back(std::pair<double,double>(0.5*xSize-deadO,yU));  
      // intersection with the xLow boundary
      double yD =-(0.5*xSize-deadI+xMid)/tan(sAngle);  
      if (fabs(yD)<=0.5*minYSize-deadS) stripEnd.push_back(std::pair<double,double>(-0.5*xSize+deadI,yD));  
      // intersection with phi boundaries
      double xP = (xMid+0.5*tan(sAngle)*(dY+minYSize-deadS))/(1-tan(sAngle)*dY/(xSize-deadO-deadI));
      double xM = (xMid-0.5*tan(sAngle)*(dY+minYSize-deadS))/(1+tan(sAngle)*dY/(xSize-deadO-deadI));
      double yP = 0.5*minYSize-deadS+dY*(xP/(xSize-deadO-deadI)+0.5);
      double yM =-0.5*minYSize+deadS-dY*(xM/(xSize-deadO-deadI)+0.5);
      if (xP<=0.5*xSize-deadO && xP>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xP,yP));
      if (xM<=0.5*xSize-deadO && xM>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xM,yM));
      //std::cout <<"intersections:"<<stripEnd.size()<< std::endl;  

      if (stripEnd.size()==2) {

	double mX = 0.5*(stripEnd[0].first  +stripEnd[1].first);
	double mY = 0.5*(stripEnd[0].second +stripEnd[1].second);
      
	pos[0] = mX;
	pos[1] = mY;
	
	return true;
      } else return false;

    } else if(type==MuonChannelDesign::pad) {
      // DG-2015-11-27 todo
    }

    return false;       

  }

  inline double MuonChannelDesign::stereoAngle( int /*st*/) const {
    
    // to be coded for TGC wire gangs and sTGC pads
 
    //if (sin(sAngle)>0.5) {
    //  double yUp = -0.5*maxYSize + (st-0.5) * maxYSize/nch;
    //  double yDn = -0.5*minYSize + (st-0.5) * minYSize/nch;
    //  return atan((yUp-yDn)/xSize);
    //}
 
    return sAngle; 
    
  }

  inline double MuonChannelDesign::channelLength( int st) const {

    double dY = 0.5*(maxYSize-minYSize-2*deadS);
    
    if ( type==MuonChannelDesign::phiStrip ) {

      double locY = firstPos+ (st-1)*inputPitch;

      double gangLength = xSize-deadO-deadI ;

      if (fabs(locY)>0.5*minYSize-deadS) {
         
        gangLength = (0.5*maxYSize-deadS - fabs(locY))/dY * (xSize-deadI-deadO);

      }
 
      return gangLength; 
      
    } else if ( type==MuonChannelDesign::etaStrip ) {
 
      if (sAngle==0.)  return inputLength +2*(st-0.5)*dY/nch;

      // strip central position
      double xMid = firstPos + (st-1)*inputPitch/cos(sAngle);

      std::vector<std::pair<double,double> > stripEnd;
      // intersection with the xUp boundary
      double yU = (0.5*xSize-deadO-xMid)/tan(sAngle);
      if (fabs(yU)<=0.5*maxYSize-deadS) stripEnd.push_back(std::pair<double,double>(0.5*xSize-deadO,yU));  
      // intersection with the xLow boundary
      double yD =-(0.5*xSize-deadI+xMid)/tan(sAngle);  
      if (fabs(yD)<=0.5*minYSize-deadS) stripEnd.push_back(std::pair<double,double>(-0.5*xSize+deadI,yD));  
      // intersection with phi boundaries
      double xP = (xMid+0.5*tan(sAngle)*(dY+minYSize-deadS))/(1-tan(sAngle)*dY/(xSize-deadI-deadO));
      double xM = (xMid-0.5*tan(sAngle)*(dY+minYSize-deadS))/(1+tan(sAngle)*dY/(xSize-deadI-deadO));
      double yP = 0.5*minYSize-deadS+dY*(xP/(xSize-deadI-deadO)+0.5);
      double yM =-0.5*minYSize+deadS-dY*(xM/(xSize-deadI-deadO)+0.5);
      if (xP<=0.5*xSize-deadO && xP>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xP,yP));
      if (xM<=0.5*xSize-deadO && xM>=-0.5*xSize+deadI) stripEnd.push_back(std::pair<double,double>(xM,yM));
      //std::cout <<"intersections:"<<stripEnd.size()<< std::endl;  

      if ( stripEnd.size()==2 ) {
    
	double difX = stripEnd[0].first-stripEnd[1].first;
	double difY = stripEnd[0].second-stripEnd[1].second;
	return sqrt(difX*difX+difY*difY);
 
      } else return inputLength; 

    } else if(type==MuonChannelDesign::pad) {
      // DG-2015-11-23 todo
    }

    return inputLength;     

  }

  inline double MuonChannelDesign::channelWidth( Amg::Vector2D /*pos*/ ) const {
    // TODO : calculate for TGCs

    return inputWidth;

  }

  inline double MuonChannelDesign::gasGapThickness() const {

    return thickness;

  }

}
#endif // MUONTRACKINGGEOMETRY_MUONCHANNELDESIGN_H
