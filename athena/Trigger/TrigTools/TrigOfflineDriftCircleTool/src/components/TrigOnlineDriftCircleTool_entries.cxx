#include "GaudiKernel/DeclareFactoryEntries.h"

#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TRT_Cabling/ITRT_CablingSvc.h"
#include "TrigOfflineDriftCircleTool/TrigTRT_DriftCircleProviderTool.h"
#include "TrigOfflineDriftCircleTool/TrigTRT_DCCollByteStreamTool.h"


DECLARE_TOOL_FACTORY(TrigTRT_DriftCircleProviderTool)
DECLARE_TOOL_FACTORY(TrigTRT_DCCollByteStreamTool)

DECLARE_FACTORY_ENTRIES(TrigOfflineDriftCircleTool) {
  DECLARE_TOOL(TrigTRT_DriftCircleProviderTool);
  DECLARE_TOOL(TrigTRT_DCCollByteStreamTool);
}
