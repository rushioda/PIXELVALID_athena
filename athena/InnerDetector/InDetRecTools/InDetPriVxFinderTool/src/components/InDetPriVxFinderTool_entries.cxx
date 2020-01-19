#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetPriVxFinderTool/InDetPriVxFinderTool.h"
#include "InDetPriVxFinderTool/InDetAdaptivePriVxFinderTool.h"
#include "InDetPriVxFinderTool/InDetIterativePriVxFinderTool.h"
#include "InDetPriVxFinderTool/InDetAdaptiveMultiPriVxFinderTool.h"
#include "InDetPriVxFinderTool/InDetMultiPriVxFinderTool.h"
#include "TrkParticleBase/TrackParticleBase.h"


using namespace InDet;

DECLARE_TOOL_FACTORY( InDetPriVxFinderTool )
DECLARE_TOOL_FACTORY( InDetAdaptivePriVxFinderTool )
DECLARE_TOOL_FACTORY( InDetIterativePriVxFinderTool )
DECLARE_TOOL_FACTORY( InDetAdaptiveMultiPriVxFinderTool )
DECLARE_TOOL_FACTORY( InDetMultiPriVxFinderTool )

DECLARE_FACTORY_ENTRIES( InDetPriVxFinderTool )
{
    DECLARE_TOOL( InDetPriVxFinderTool )
    DECLARE_TOOL( InDetAdaptivePriVxFinderTool )
    DECLARE_TOOL( InDetIterativePriVxFinderTool )
    DECLARE_TOOL( InDetAdaptiveMultiPriVxFinderTool )
    DECLARE_TOOL( InDetMultiPriVxFinderTool )

}

