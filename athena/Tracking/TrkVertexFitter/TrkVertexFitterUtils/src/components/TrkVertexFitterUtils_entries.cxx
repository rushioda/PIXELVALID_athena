#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkVertexFitterUtils/KalmanVertexUpdator.h"
#include "TrkVertexFitterUtils/KalmanVertexTrackUpdator.h"
#include "TrkVertexFitterUtils/FullLinearizedTrackFactory.h"
#include "TrkVertexFitterUtils/Chi2TrackCompatibilityEstimator.h"
#include "TrkVertexFitterUtils/DetAnnealingMaker.h"
#include "TrkVertexFitterUtils/DummyAnnealingMaker.h"
#include "src/ImpactPoint3dEstimator.h"
#include "src/NeutralParticleParameterCalculator.h"
#include "TrkVertexFitterUtils/TrackToVertexIPEstimator.h"
#include "TrkVertexFitterUtils/Z0PVTrackCompatibilityEstimator.h"


using namespace Trk;

DECLARE_TOOL_FACTORY( KalmanVertexUpdator )
DECLARE_TOOL_FACTORY( KalmanVertexTrackUpdator )
DECLARE_TOOL_FACTORY( FullLinearizedTrackFactory )
DECLARE_TOOL_FACTORY( DummyAnnealingMaker )
DECLARE_TOOL_FACTORY( Chi2TrackCompatibilityEstimator )
DECLARE_TOOL_FACTORY( DetAnnealingMaker )
DECLARE_TOOL_FACTORY( ImpactPoint3dEstimator )
DECLARE_TOOL_FACTORY( NeutralParticleParameterCalculator)
DECLARE_TOOL_FACTORY( TrackToVertexIPEstimator )
DECLARE_TOOL_FACTORY( Z0PVTrackCompatibilityEstimator )

DECLARE_FACTORY_ENTRIES( TrkVertexFitterUtils )
{
    DECLARE_TOOL( KalmanVertexUpdator );  
    DECLARE_TOOL( KalmanVertexTrackUpdator );
    DECLARE_TOOL( FullLinearizedTrackFactory );
    DECLARE_TOOL( DummyAnnealingMaker );
    DECLARE_TOOL( Chi2TrackCompatibilityEstimator );
    DECLARE_TOOL( DetAnnealingMaker );
    DECLARE_TOOL( ImpactPoint3dEstimator );
    DECLARE_TOOL( NeutralParticleParameterCalculator );
    DECLARE_TOOL( TrackToVertexIPEstimator );
    DECLARE_TOOL( Z0PVTrackCompatibilityEstimator );
}

