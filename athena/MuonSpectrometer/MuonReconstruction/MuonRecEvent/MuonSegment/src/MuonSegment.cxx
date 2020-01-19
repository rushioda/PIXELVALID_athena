/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegment/MuonSegment.h"
// TrkEvent
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "CxxUtils/sincos.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#include <float.h>
#include <iostream>
namespace Muon {

const float MuonSegment::kNoValue = FLT_MAX;
    
MuonSegment::MuonSegment() 
: 
Segment(),
SpaceTimePointBase(kNoValue,kNoValue,1.),
m_globalPosition(  ),
m_globalDirection(  ), 
m_localDirection(  ),
m_associatedSurface( 0 ),
m_cachedRots( 0 )
{}
    
MuonSegment::MuonSegment(const MuonSegment& seg) 
: 
Segment(seg), 
SpaceTimePointBase(seg),
m_globalPosition( seg.m_globalPosition ),
m_globalDirection( seg.m_globalDirection ),
m_localDirection ( seg.m_localDirection ),
m_associatedSurface( 0 ),
m_cachedRots( 0 )
{
    m_associatedSurface = (seg.m_associatedSurface->associatedDetectorElement()) ?
    seg.m_associatedSurface : seg.m_associatedSurface->clone();
    
    // create vector of contained Rots and fill it
    copyMeasurementsToROTs();
}

MuonSegment& MuonSegment::operator=(const MuonSegment& seg)
{
    if (this!=&seg){
        Trk::Segment::operator=(seg);
        Trk::SpaceTimePointBase::operator=(seg);
        m_globalPosition = seg.m_globalPosition;
        m_globalDirection = seg.m_globalDirection;
        m_localDirection = seg.m_localDirection;

        // delete & clone the surface only if there's no associated DetectorElement 
        if (0!=m_associatedSurface && 0==m_associatedSurface->associatedDetectorElement()) delete m_associatedSurface;
        m_associatedSurface = (seg.m_associatedSurface->associatedDetectorElement()) ?
            seg.m_associatedSurface : seg.m_associatedSurface->clone(); 
        // clear rots
        clearRotVector();
        copyMeasurementsToROTs();
    }
    return (*this);
}

MuonSegment::MuonSegment( const Trk::LocalParameters& locpars, 
                          const Amg::MatrixX& locerr,       
                          Trk::PlaneSurface* psf,                 
                          DataVector<const Trk::MeasurementBase>* cmeas,
                          Trk::FitQuality* fqual,
                          Segment::Author author)
    :
    Segment( locpars, locerr, cmeas, fqual,author ), 
    SpaceTimePointBase(kNoValue,kNoValue,1.),
    m_globalPosition(),
    m_globalDirection(),
    m_localDirection(),
    m_associatedSurface( psf ),
    m_cachedRots(0)  
{
  recalculateCache();
  psf->globalToLocalDirection(m_globalDirection,m_localDirection);
  copyMeasurementsToROTs();
}


MuonSegment::MuonSegment(   const Amg::Vector2D& locSegPos,     
                            const Trk::LocalDirection& locSegDir,       
                            const Amg::MatrixX& locErr,        
                            Trk::PlaneSurface* psf,                     
                            DataVector<const Trk::MeasurementBase>* cmeas, 
                            Trk::FitQuality* fqual,
                            Segment::Author author)
    :
  Segment( Trk::LocalParameters(), locErr, cmeas, fqual,author ), 
  SpaceTimePointBase(kNoValue,kNoValue,1.),
  m_globalPosition( ),
  m_globalDirection(),
  m_localDirection(locSegDir),      
  m_associatedSurface( psf ),
  m_cachedRots( 0 )
{
  psf->localToGlobalDirection(locSegDir,m_globalDirection);

  Amg::Vector2D lpos(locSegPos[Trk::locX], locSegPos[Trk::locY]);
  m_associatedSurface->localToGlobal(lpos, m_globalDirection, m_globalPosition);

  double phi = m_globalDirection.phi();
  double theta = m_globalDirection.theta();
  std::vector<Trk::DefinedParameter > pars;
  pars.push_back( Trk::DefinedParameter( locSegPos[Trk::locX], Trk::locX) );
  pars.push_back( Trk::DefinedParameter( locSegPos[Trk::locY], Trk::locY) );
  pars.push_back( Trk::DefinedParameter( phi, Trk::phi) );
  pars.push_back( Trk::DefinedParameter( theta, Trk::theta) );
  m_localParams = Trk::LocalParameters( pars );
  copyMeasurementsToROTs();
}
 
MuonSegment::~MuonSegment() 
{
    if ( 0!=m_associatedSurface && !m_associatedSurface->associatedDetectorElement()) delete m_associatedSurface;
    clearRotVector();
}

void MuonSegment::clearRotVector()
{
  delete m_cachedRots;
} 

void MuonSegment::recalculateCache(){

 CxxUtils::sincos scphi(m_localParams[Trk::phi]);
 CxxUtils::sincos sctheta(m_localParams[Trk::theta]);

 double sintheta = sctheta.sn;
 double costheta = sctheta.cs;
 double sinphi = scphi.sn;
 double cosphi = scphi.cs;
  m_globalDirection = Amg::Vector3D( cosphi*sintheta,
				     sinphi*sintheta,
				     costheta );
  Amg::Vector2D lpos(m_localParams[Trk::locX], m_localParams[Trk::locY]);
  m_associatedSurface->localToGlobal(lpos, m_globalPosition, m_globalPosition);
}

void MuonSegment::copyMeasurementsToROTs() const
{
  if (! hasContainedMeasurements() ) return;
//  std::cout<<"copyMeasurementsToROTs :"<<this<<" with measurements: "<<containedMeasurements().size()<<std::endl;
  m_cachedRots = new std::vector<const Trk::RIO_OnTrack*>;
  m_cachedRots->reserve(containedMeasurements().size());
  for (const Trk::MeasurementBase* m : containedMeasurements()) {
    // cast to RIO_OnTrack
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(m);
    if( rot ) m_cachedRots->push_back(rot);    
  }
  //std::cout<<"copyMeasurementsToROTs :"<<this<<" with rots: "<<m_cachedRots->size()<<std::endl;
}

MsgStream& MuonSegment::dump( MsgStream& out ) const
{
    out << "Muon::MuonSegment " << std::endl;
    out << "  - global position      : " << std::endl<<Amg::toString(globalPosition())<<std::endl;
    out << std::endl<<localDirection()<<std::endl;
    out << "  - local parameters     : " << std::endl<<localParameters()<<std::endl;
    out << "  - local Err Matrix     : " ;
    out << std::endl<<Amg::toString(localCovariance())<<std::endl;
    if (0!=m_fitQuality) 
        out << "  - fit Quality          : " << std::endl<<(*fitQuality())<<std::endl;
    else
        out << "  - fit Quality          : NO " <<std::endl;
    out << "  - associated Surface   : " << std::endl<<associatedSurface()<<std::endl;
    out << "  - t0 (error) : "<<time()<<"("<<errorTime()<<")"<<std::endl;
    out << "  - it contains   : " << numberOfContainedROTs() << " RIO_OnTrack object" << std::endl;
    unsigned int numRoT=1;
    std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = m_cachedRots->begin();
    std::vector<const Trk::RIO_OnTrack*>::const_iterator rotEnd  = m_cachedRots->end();
    for (; rotIter!=rotEnd; ++rotIter)
        out << "RoT "<<numRoT++<<std::endl<<**rotIter<<std::endl;
    return out;
}  

std::ostream& MuonSegment::dump( std::ostream& out ) const
{
    out << "Muon::MuonSegment " << std::endl;
    out << "  - global position      : " << std::endl<<Amg::toString(globalPosition())<<std::endl;
    out << std::endl<<localDirection()<<std::endl;
    out << "  - local parameters     : " << std::endl<<localParameters()<<std::endl;
    out << "  - local Err Matrix     : " ;
    out << std::endl<<Amg::toString(localCovariance())<<std::endl;
    if (0!=m_fitQuality) 
        out << "  - fit Quality          : " << std::endl<<(*fitQuality())<<std::endl;
    else
        out << "  - fit Quality          : NO " <<std::endl;
    out << "  - associated Surface   : " << std::endl<<associatedSurface()<<std::endl;
    out << "  - t0 (error) : "<<time()<<"("<<errorTime()<<")"<<std::endl;
    out << "  - it contains   : " << numberOfContainedROTs() << " RIO_OnTrack object" << std::endl;
    unsigned int numRoT=1;
    std::vector<const Trk::RIO_OnTrack*>::const_iterator rotIter = m_cachedRots->begin();
    std::vector<const Trk::RIO_OnTrack*>::const_iterator rotEnd  = m_cachedRots->end();
    for (; rotIter!=rotEnd; ++rotIter)
        out << "RoT "<<numRoT++<<std::endl<<**rotIter<<std::endl;
    return out;
}

}


