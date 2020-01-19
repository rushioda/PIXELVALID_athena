#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkTruthCreatorTools/DetailedTrackTruthBuilder.h"
#include "TrkTruthCreatorTools/ElasticTruthTrajectoryBuilder.h"
#include "TrkTruthCreatorTools/DecayInFlyTruthTrajectoryBuilder.h"
#include "TrkTruthCreatorTools/TruthMatchRatio.h"
#include "TrkTruthCreatorTools/TruthMatchTanimoto.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DetailedTrackTruthBuilder )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ElasticTruthTrajectoryBuilder )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, DecayInFlyTruthTrajectoryBuilder )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TruthMatchRatio )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, TruthMatchTanimoto )

DECLARE_FACTORY_ENTRIES( TrkTruthCreatorTools ) 
{
  DECLARE_NAMESPACE_TOOL( Trk, DetailedTrackTruthBuilder )
  DECLARE_NAMESPACE_TOOL( Trk, ElasticTruthTrajectoryBuilder )
  DECLARE_NAMESPACE_TOOL( Trk, DecayInFlyTruthTrajectoryBuilder )
  DECLARE_NAMESPACE_TOOL( Trk, TruthMatchRatio )
  DECLARE_NAMESPACE_TOOL( Trk, TruthMatchTanimoto )
}
