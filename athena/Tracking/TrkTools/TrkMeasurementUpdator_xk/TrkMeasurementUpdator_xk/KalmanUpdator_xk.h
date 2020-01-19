/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// KalmanUpdator_xk.h
//   Header file for Kalman Fitter Updator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 7/6/2005 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRK_KALMANUPDATOR_XK_H
#define TRK_KALMANUPDATOR_XK_H

#include "TrkToolInterfaces/IUpdator.h"
#include "TrkToolInterfaces/IPatternParametersUpdator.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

  /**
     @class KalmanUpdator_xk 
  
     Trk::KalmanUpdator_xk is a set of tools for adding and removing
     measurements to/from the state vector using xKalman algorithms
     @author Igor.Gavrilenko@cern.ch     
  */

  class LocalParameters       ;
  class PatternTrackParameters;

  class KalmanUpdator_xk : virtual public IUpdator,
    virtual public IPatternParametersUpdator,
    public AthAlgTool
    { 
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
 
    public:	
      
      ///////////////////////////////////////////////////////////////////
      // Standard Athena tool methods
      ///////////////////////////////////////////////////////////////////

      KalmanUpdator_xk
	(const std::string&,const std::string&,const IInterface*);
      virtual ~KalmanUpdator_xk    ();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      // /////////////////////////////////////////////////////////////////
      // Main public methods for kalman filter updator tool
      // /////////////////////////////////////////////////////////////////
 
      // /////////////////////////////////////////////////////////////////
      // Add and remove without Xi2 calculation
      // /////////////////////////////////////////////////////////////////
    
      //! add without chi2 calculation, PRD-level, EDM track parameters
      virtual const TrackParameters* addToState 
        (const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&) const;
      //! add without chi2 calculation, ROT-level, EDM track parameters
      virtual const TrackParameters* addToState 
        (const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const;

      //! add without chi2 calculation, PRD-level, pattern track parameters
      virtual bool                   addToState 
        (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
         PatternTrackParameters&) const;
      //! add without chi2 calculation, ROT-level, pattern track parameters
      virtual bool                   addToState 
        (PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
         PatternTrackParameters&) const;
      //! add without chi2 calculation, PRD-level, pattern track parameters, specifically 1D
      virtual bool                   addToStateOneDimension 
        (PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
         PatternTrackParameters&) const;

      ///////////////////////////////////////////////////////////////////
      // Remove without Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual const TrackParameters* removeFromState 
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&) const;
      virtual const TrackParameters* removeFromState 
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const;

      virtual bool removeFromState 
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&) const;
      virtual bool removeFromState 
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 PatternTrackParameters&) const;

      ///////////////////////////////////////////////////////////////////
      // Add  with Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual const TrackParameters* addToState 
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const;
      virtual const TrackParameters* addToState 
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const;
      virtual std::pair<AmgVector(5),AmgSymMatrix(5)>* updateParameterDifference 
	(const AmgVector(5)&,const AmgSymMatrix(5)&, const Amg::VectorX&, const Amg::MatrixX&, 
	 const int&,Trk::FitQualityOnSurface*&,bool) const; 
  
      virtual bool addToState 
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const;
      virtual bool addToState 
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const;
      virtual bool                   addToStateOneDimension 
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const;

      ///////////////////////////////////////////////////////////////////
      // Remove with Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual const TrackParameters* removeFromState 
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const;
      virtual const TrackParameters* removeFromState 
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 FitQualityOnSurface*&) const;

      virtual bool removeFromState 
	(PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const;
      virtual bool removeFromState 
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 PatternTrackParameters&,double&,int&) const;

      ///////////////////////////////////////////////////////////////////
      // Combine two state with or without Xi2 calculation
      ///////////////////////////////////////////////////////////////////

      virtual const TrackParameters* combineStates   
	(const TrackParameters&, const TrackParameters&) const;
      virtual const TrackParameters* combineStates   
	(const TrackParameters&, const TrackParameters&, 
	 FitQualityOnSurface*&) const;

      virtual bool combineStates   
	(PatternTrackParameters&,PatternTrackParameters&,PatternTrackParameters&) const;
      virtual bool combineStates   
	(PatternTrackParameters&,PatternTrackParameters&,PatternTrackParameters&,
	 double&) const;

      ///////////////////////////////////////////////////////////////////
      // Xi2 calculation
      ///////////////////////////////////////////////////////////////////
      
      virtual const FitQualityOnSurface* predictedStateFitQuality  
	(const TrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&) const;
      virtual const FitQualityOnSurface* predictedStateFitQuality  
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const;
      virtual const FitQualityOnSurface* fullStateFitQuality 
	(const TrackParameters&,const Amg::Vector2D&,  const Amg::MatrixX&) const;
      virtual const FitQualityOnSurface* fullStateFitQuality 
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&) const;
      virtual const FitQualityOnSurface*  predictedStateFitQuality
	(const TrackParameters&,const TrackParameters&) const;

      virtual bool predictedStateFitQuality  
	(const PatternTrackParameters&,const Amg::Vector2D&  ,const Amg::MatrixX&,int&,double&) 
	const;
      virtual bool predictedStateFitQuality  
	(const PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,int&,double&) 
	const;
      virtual bool fullStateFitQuality 
	(const PatternTrackParameters&,const Amg::Vector2D&,  const Amg::MatrixX&,int&,double&)
	const;
      virtual bool fullStateFitQuality 
	(const PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,int&,double&)
	const;
      virtual bool  predictedStateFitQuality
	(const PatternTrackParameters&,const PatternTrackParameters&,double&) const;

      ///////////////////////////////////////////////////////////////////
      // let the client tools know how the assumptions on the initial
      // precision for non-measured track parameters are configured 
      ///////////////////////////////////////////////////////////////////
      
      virtual const std::vector<double> initialErrors() const;

    protected:
 
      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      ///////////////////////////////////////////////////////////////////
      // Updators
      ///////////////////////////////////////////////////////////////////
      
      const TrackParameters* update 
	(const TrackParameters&,const Amg::Vector2D&,const Amg::MatrixX&,
	 FitQualityOnSurface*&,int,bool) const;

      const TrackParameters* update 
	(const TrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 FitQualityOnSurface*&,int,bool) const;

      bool                   update
	(PatternTrackParameters&,const Amg::Vector2D&,const Amg::MatrixX&,
	 int,bool,PatternTrackParameters&,double&,int&) const;

      bool                   updateOneDimension
	(PatternTrackParameters&,const Amg::Vector2D&,const Amg::MatrixX&,
	 int,bool,PatternTrackParameters&,double&) const;

      bool                   update
	(PatternTrackParameters&,const LocalParameters&,const Amg::MatrixX&,
	 int,bool,PatternTrackParameters&,double&,int&) const;

      ///////////////////////////////////////////////////////////////////
      // Xi2 calculation
      ///////////////////////////////////////////////////////////////////
      
      bool predictedStateFitQuality  
	(const double*,const Amg::Vector2D&,const Amg::MatrixX&,int&,double&) const;

      bool fullStateFitQuality
	(const double*,const Amg::Vector2D&,const Amg::MatrixX&,int&,double&) const;

      ///////////////////////////////////////////////////////////////////
      // Converters
      ///////////////////////////////////////////////////////////////////
 
      bool trackParametersToUpdator
	(const        TrackParameters&,double*,double*) const;

      bool trackParametersToUpdator
	(const PatternTrackParameters&,double*,double*) const;

      bool localParametersToUpdator
	(const LocalParameters&,const Amg::MatrixX&,int&,int&,double*,double*) const;
      
      const TrackParameters* updatorToTrackParameters
	(const TrackParameters&,double*,double*) const; 

      ///////////////////////////////////////////////////////////////////
      // Update no measured track parameters
      ///////////////////////////////////////////////////////////////////
      
      bool updateNoMeasuredWithOneDim
	(double*,double*,double*,double*) const;

      bool updateNoMeasuredWithTwoDim
	(double*,double*,double*,double*) const;

      bool updateNoMeasuredWithAnyDim
	(double*,double*,double*,double*,int) const;

      ///////////////////////////////////////////////////////////////////
      // Update measured track parameters
      ///////////////////////////////////////////////////////////////////

      bool updateWithOneDim
	(int,bool,double*,double*,double*,double*,double&) const;

      bool updateWithTwoDim
	(int,bool,double*,double*,double*,double*,double&) const;

      bool updateWithTwoDimParameters
	(int,bool,double*,double*,double*,double*,double&) const;

      bool updateWithFiveDim
	(    bool,double*,double*,double*,double*,double&) const;

      bool updateWithAnyDim
	(int,bool,double*,double*,double*,double*,double&,
	 int,int) const;

      bool updateWithOneDimWithBoundary
	(int,bool,double*,double*,double*,double*,double&) const;

      bool updateWithTwoDimWithBoundary
	(int,bool,double*,double*,double*,double*,double&) const;

      bool invert (int,double*,double*) const;
      bool invert2(    double*,double*) const;
      bool invert3(    double*,double*) const;
      bool invert4(    double*,double*) const;
      bool invert5(    double*,double*) const;

      double Xi2    (int,double*,double*) const;
      double Xi2for1(    double*,double*) const;
      double Xi2for2(    double*,double*) const;
      double Xi2for3(    double*,double*) const;
      double Xi2for4(    double*,double*) const;
      double Xi2for5(    double*,double*) const;

      int  differenceParLoc(int,double*,double*,double*) const;
      void differenceLocPar(int,double*,double*,double*) const;
      void testAngles(double*,double*) const;

      void mapKeyProduction();

      ///////////////////////////////////////////////////////////////////
      // Protected data members
      ///////////////////////////////////////////////////////////////////

      std::vector<double> m_cov0;
      unsigned int m_key[ 33];
      unsigned int m_map[160];
      double   m_covBoundary;
    };

  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

} // end of namespace

#endif // TRK_KALMANUPDATOR_XK_H
