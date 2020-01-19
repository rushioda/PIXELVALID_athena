/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"


namespace Trk
{
	LinkToXAODTrackParticle::LinkToXAODTrackParticle() : ElementLink<xAOD::TrackParticleContainer>()
	{}

	LinkToXAODTrackParticle::LinkToXAODTrackParticle ( ElementLink<xAOD::TrackParticleContainer>& link ) : ElementLink<xAOD::TrackParticleContainer> ( link )
	{}

	LinkToXAODTrackParticle::~LinkToXAODTrackParticle()
	{}

        const TrackParameters* LinkToXAODTrackParticle::parameters() const
	{
		if ( isValid() )
		{
			const xAOD::TrackParticle * trk = this->cachedElement();
			if ( 0 != trk ) return dynamic_cast<const Trk::TrackParameters* > ( &trk->perigeeParameters() );
			return 0;
		}
		return 0;
	}//end of parameters method

        Trk::LinkToXAODTrackParticle* Trk::LinkToXAODTrackParticle::clone() const
	{
		return new LinkToXAODTrackParticle ( *this );
	}

	Trk::LinkToXAODTrackParticle& Trk::LinkToXAODTrackParticle::operator= ( const Trk::LinkToXAODTrackParticle& rhs )
	{
		if ( this!=&rhs )
		{
			ElementLink<xAOD::TrackParticleContainer>::operator= ( rhs );
		}
		return *this;
	}
}//end of namespace definitions
