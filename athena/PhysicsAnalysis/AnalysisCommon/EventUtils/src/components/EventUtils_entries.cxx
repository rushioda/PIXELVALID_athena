#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../ParticleSortingTool.h"
#include "../ParticleSortingAlg.h"
#include "../AddVarTool.h"
#include "../AddVarAlg.h"
#include "../CutTool.h"
#include "../CutAlg.h"
#include "../ParticleSelectionTool.h"
#include "../ParticleSelectionAlg.h"
#include "../ParticleCombinerTool.h"
#include "../ParticleCombinerAlg.h"
#include "../EventQualityFilterAlg.h"
//#include "../SkimDecisionRunPeriodAwareFilterAlg.h"

DECLARE_TOOL_FACTORY( ParticleSortingTool )
DECLARE_ALGORITHM_FACTORY( ParticleSortingAlg )
DECLARE_TOOL_FACTORY( AddVarTool )
DECLARE_ALGORITHM_FACTORY( AddVarAlg )
DECLARE_TOOL_FACTORY( CutTool )
DECLARE_ALGORITHM_FACTORY( CutAlg )
DECLARE_TOOL_FACTORY( ParticleSelectionTool )
DECLARE_ALGORITHM_FACTORY( ParticleSelectionAlg )
DECLARE_TOOL_FACTORY( ParticleCombinerTool )
DECLARE_ALGORITHM_FACTORY( ParticleCombinerAlg )
DECLARE_ALGORITHM_FACTORY( EventQualityFilterAlg )
//DECLARE_ALGORITHM_FACTORY( SkimDecisionRunPeriodAwareFilterAlg )

DECLARE_FACTORY_ENTRIES( EventUtils )
{
  DECLARE_TOOL( ParticleSortingTool );
  DECLARE_ALGORITHM( ParticleSortingAlg );
  DECLARE_TOOL( AddVarTool );
  DECLARE_ALGORITHM( AddVarAlg );
  DECLARE_TOOL( CutTool );
  DECLARE_ALGORITHM( CutAlg );
  DECLARE_TOOL( ParticleSelectionTool );
  DECLARE_ALGORITHM( ParticleSelectionAlg );
  DECLARE_TOOL( ParticleCombinerTool );
  DECLARE_ALGORITHM( ParticleCombinerAlg );
  DECLARE_ALGORITHM( EventQualityFilterAlg );
  //DECLARE_ALGORITHM( SkimDecisionRunPeriodAwareFilterAlg );
}
