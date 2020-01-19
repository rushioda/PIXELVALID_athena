
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DiTauRec/DiTauBuilder.h"
#include "DiTauRec/DiTauToolBase.h"
#include "DiTauRec/SeedJetBuilder.h"
#include "DiTauRec/SubjetBuilder.h"
#include "DiTauRec/ElMuFinder.h"
#include "DiTauRec/VertexFinder.h"
#include "DiTauRec/DiTauTrackFinder.h"
#include "DiTauRec/CellFinder.h"
#include "DiTauRec/IDVarCalculator.h"

DECLARE_ALGORITHM_FACTORY( DiTauBuilder )
DECLARE_TOOL_FACTORY( SeedJetBuilder )
DECLARE_TOOL_FACTORY( SubjetBuilder )
DECLARE_TOOL_FACTORY( ElMuFinder )
DECLARE_TOOL_FACTORY( VertexFinder )
DECLARE_TOOL_FACTORY( DiTauTrackFinder )
DECLARE_TOOL_FACTORY( CellFinder )
DECLARE_TOOL_FACTORY( IDVarCalculator )

DECLARE_FACTORY_ENTRIES( DiTauRec ) 
{
  DECLARE_ALGORITHM( DiTauBuilder );
  DECLARE_TOOL( SeedJetBuilder );
  DECLARE_TOOL( SubjetBuilder );
  DECLARE_TOOL( ElMuFinder );
  DECLARE_TOOL( VertexFinder );
  DECLARE_TOOL( DiTauTrackFinder );
  DECLARE_TOOL( CellFinder );
  DECLARE_TOOL( IDVarCalculator );
}
