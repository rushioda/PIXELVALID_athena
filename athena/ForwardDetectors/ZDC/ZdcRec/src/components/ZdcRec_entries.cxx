#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ZdcRec/ZdcRec.h"
#include "ZdcRec/ZdcRecV2.h"
#include "ZdcRec/ZdcRecV3.h"
#include "ZdcRec/ZdcRecChannelTool.h"
#include "ZdcRec/ZdcRecChannelToolV2.h"
#include "ZdcRec/ZdcRecNoiseTool.h"
#include "ZdcRec/ZdcSignalSinc.h"

DECLARE_ALGORITHM_FACTORY 	(ZdcRec)
DECLARE_ALGORITHM_FACTORY 	(ZdcRecV2)
DECLARE_ALGORITHM_FACTORY 	(ZdcRecV3)
DECLARE_TOOL_FACTORY 		(ZdcRecChannelTool)
DECLARE_TOOL_FACTORY 		(ZdcRecChannelToolV2)
DECLARE_TOOL_FACTORY 		(ZdcRecNoiseTool)

DECLARE_FACTORY_ENTRIES (ZdcRec) {
  	 DECLARE_TOOL      (ZdcRecChannelTool)
  	 DECLARE_TOOL      (ZdcRecChannelToolV2)
  	 DECLARE_TOOL      (ZdcRecNoiseTool)
	 DECLARE_ALGORITHM (ZdcRec)
	 DECLARE_ALGORITHM (ZdcRecV2)
	 DECLARE_ALGORITHM (ZdcRecV3)
}

