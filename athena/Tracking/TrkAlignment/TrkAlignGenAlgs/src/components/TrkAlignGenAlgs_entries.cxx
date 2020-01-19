#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkAlignGenAlgs/AlignAlg.h"
#include "TrkAlignGenAlgs/SelectEventNumber.h"
#include "TrkAlignGenAlgs/AlignSolve.h"
#include "TrkAlignGenAlgs/AlignTrackCollSplitter.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, AlignAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, SelectEventNumber )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, AlignSolve )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, AlignTrackCollSplitter )

DECLARE_FACTORY_ENTRIES( TrkAlignGenAlgs )
{
  DECLARE_NAMESPACE_ALGORITHM( Trk, AlignAlg )
  DECLARE_NAMESPACE_ALGORITHM( Trk, SelectEventNumber )
  DECLARE_NAMESPACE_ALGORITHM( Trk, AlignSolve )
  DECLARE_NAMESPACE_ALGORITHM( Trk, AlignTrackCollSplitter )
}
