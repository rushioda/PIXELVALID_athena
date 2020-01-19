/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          FitterStatusCode.cxx  -  description
                             -------------------
    email                : sebastian.fleischmann -at- cern.ch
 ***************************************************************************/


#include "TrkFitterUtils/FitterStatusCode.h"
#include "GaudiKernel/MsgStream.h"
//#include <iostream>
Trk::FitterStatusCode::FitterStatusCode( )
        :
        //StatusCode( ),
        m_code( 0 ) {}

Trk::FitterStatusCode::FitterStatusCode( FitCode thecode )
        :
        //StatusCode( (thecode>1)?StatusCode::SUCCESS : StatusCode::FAILURE ),
        m_code( thecode ) {}

Trk::FitterStatusCode::FitterStatusCode( const FitterStatusCode& rhs )
        :
        //StatusCode( (rhs.m_code>1)?StatusCode::SUCCESS : StatusCode::FAILURE ),
        m_code( rhs.m_code ) {}

Trk::FitterStatusCode::~FitterStatusCode() {}

bool Trk::FitterStatusCode::isFailure() const {
    if (m_code>1) return true;
    return false;
}

Trk::FitterStatusCode::operator unsigned long() const  {
    return m_code;
}

Trk::FitterStatusCode::operator StatusCode() const  {
    return StatusCode( (m_code>1)?StatusCode::SUCCESS : StatusCode::FAILURE );
}

Trk::FitterStatusCode& Trk::FitterStatusCode::operator=(unsigned long value)   {
    m_code = value;
    return *this;
}

Trk::FitterStatusCode& Trk::FitterStatusCode::operator=(const FitterStatusCode& rhs) {
    // coverity complains about a missing self assigmment protection.
    // But self assignment of integers is not problematic and
    // adding a comparison just for this rare case does not seem useful.
    //if (&rhs != this ) {
    m_code = rhs.m_code;
    //}
    return *this;
}

unsigned long Trk::FitterStatusCode::getCode() const {
    return m_code;
}

std::string Trk::FitterStatusCode::getString() const {
    switch(m_code)
    {
    case Success:
       return "success";
    case StraightTrackModelUsed:
       return "success, using straight track model";
    case BadInput:
       return "bad input";
    case UpdateFailure:
       return "parameter update failure";
    case FitQualityFailure:
       return "fit quality production failed";
    case MissingCovariance:
       return "covariance missing";
    case ExtrapolationFailure:
       return "extrapolation failed";
    case ExtrapolationFailureDueToSmallMomentum:
       return "extrapolation failed due to small momentum";
    case CombineStatesFailure:
       return "fit states combination failed";
    case WeaklyConstraintPars:
       return "parameters only weakly constrained";
    case OutlierLogicFailure:
       return "outlier logic failed";
    case PerigeeMakingFailure:
       return "perigee could not be produced";
    case ForwardFilterFailure:
       return "forward filter failed";
    case SmootherFailure:
       return "smoother failed";
    case FewFittableMeasurements:
       return "too few fittable measurements";
    case MatrixInversionFailure:
       return "matrix inversion failed";
    case NoConvergence:
       return "fit did not converge";
    case InvalidAngles:
       return "invalid results for theta, phi";
    default :
       return "unknown";
    };
}

MsgStream& operator<<(MsgStream& out, const Trk::FitterStatusCode& fitStatCode){
    out << fitStatCode.getString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const Trk::FitterStatusCode& fitStatCode) {
    out << fitStatCode.getString();
    return out;
}

// bool operator< ( const Trk::FitterStatusCode& a, const Trk::FitterStatusCode& b ) {
//  return a.m_code < b.m_code;
// }
//
// bool operator> ( const Trk::FitterStatusCode& a, const Trk::FitterStatusCode& b ) {
// return a.m_code > b.m_code;
// }
