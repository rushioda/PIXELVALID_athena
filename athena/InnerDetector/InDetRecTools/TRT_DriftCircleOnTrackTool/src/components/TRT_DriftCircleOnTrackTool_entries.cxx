#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool.h"
#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackUniversalTool.h"
#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackRecalibrateTool.h"
#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackNoDriftTimeTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(InDet, TRT_DriftCircleOnTrackTool)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, TRT_DriftCircleOnTrackUniversalTool)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, TRT_DriftCircleOnTrackRecalibrateTool)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, TRT_DriftCircleOnTrackNoDriftTimeTool)

DECLARE_FACTORY_ENTRIES(TRT_DriftCircleOnTrackTool) {
  DECLARE_NAMESPACE_TOOL(InDet, TRT_DriftCircleOnTrackTool)
  DECLARE_NAMESPACE_TOOL(InDet, TRT_DriftCircleOnTrackUniversalTool)
  DECLARE_NAMESPACE_TOOL(InDet, TRT_DriftCircleOnTrackRecalibrateTool)
  DECLARE_NAMESPACE_TOOL(InDet, TRT_DriftCircleOnTrackNoDriftTimeTool)
}
