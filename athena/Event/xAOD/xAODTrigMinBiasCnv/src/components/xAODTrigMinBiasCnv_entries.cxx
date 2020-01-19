// $Id:$

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../TrigSpacePointCountsCnvTool.h"
#include "../TrigSpacePointCountsCnvAlg.h"

// #include "../TrigHisto2DCnvTool.h"
// #include "../TrigHisto2DCnvAlg.h"

#include "../TrigT2MbtsBitsCnvTool.h"
#include "../TrigT2MbtsBitsCnvAlg.h"

#include "../TrigTrackCountsCnvTool.h"
#include "../TrigTrackCountsCnvAlg.h"

#include "../TrigVertexCountsCnvTool.h"
#include "../TrigVertexCountsCnvAlg.h"

#include "../TrigT2ZdcSignalsCnvTool.h"
#include "../TrigT2ZdcSignalsCnvAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigSpacePointCountsCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigSpacePointCountsCnvAlg )
// DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigHisto2DCnvTool )
// DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigHisto2DCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigT2MbtsBitsCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigT2MbtsBitsCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigTrackCountsCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigTrackCountsCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigVertexCountsCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigVertexCountsCnvAlg )
DECLARE_NAMESPACE_TOOL_FACTORY( xAODMaker, TrigT2ZdcSignalsCnvTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( xAODMaker, TrigT2ZdcSignalsCnvAlg )

DECLARE_FACTORY_ENTRIES( xAODTrigMinBiasCnv ) {

   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigSpacePointCountsCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigSpacePointCountsCnvAlg )
//    DECLARE_NAMESPACE_TOOL( xAODMaker, TrigHisto2DCnvTool )
//    DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigHisto2DCnvAlg )
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigT2MbtsBitsCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigT2MbtsBitssCnvAlg)
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigTrackCountsCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigTrackCountssCnvAlg)
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigVertexCountsCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigVertexCountssCnvAlg)
   DECLARE_NAMESPACE_TOOL( xAODMaker, TrigT2ZdcSignalsCnvTool )
   DECLARE_NAMESPACE_ALGORITHM( xAODMaker, TrigT2ZdcSignalssCnvAlg)
}
