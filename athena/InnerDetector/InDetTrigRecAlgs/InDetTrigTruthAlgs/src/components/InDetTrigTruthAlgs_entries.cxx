#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigTruthAlgs/TrigTrackParticleTruthMaker.h"
#include "InDetTrigTruthAlgs/PRD_TrigMultiTruthMaker.h"
#include "InDetTrigTruthAlgs/InDetTrigDetailedTrackTruthMaker.h"
#include "TrkTrack/Track.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigTrackParticleTruthMaker )
DECLARE_ALGORITHM_FACTORY( PRD_TrigMultiTruthMaker )
DECLARE_ALGORITHM_FACTORY( InDetTrigDetailedTrackTruthMaker )

DECLARE_FACTORY_ENTRIES( InDetTrigTruthAlgs ) 
{
  DECLARE_ALGORITHM( TrigTrackParticleTruthMaker )
  DECLARE_ALGORITHM( PRD_TrigMultiTruthMaker )
  DECLARE_ALGORITHM( InDetTrigDetailedTrackTruthMaker )
}
