#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../CscThresholdClusterBuilder.h"
#include "../CscThresholdClusterBuilderTool.h"
#include "../CscPeakThresholdClusterBuilderTool.h"
#include "../CalibCscStripFitter.h"
#include "../SimpleCscClusterFitter.h"
#include "../QratCscClusterFitter.h"
#include "../CscSplitClusterFitter.h"
#include "../ParabolaCscClusterFitter.h"
#include "../CscClusterUtilTool.h"
#include "../CscAlignmentTool.h"

DECLARE_ALGORITHM_FACTORY( CscThresholdClusterBuilder )
DECLARE_TOOL_FACTORY( CscThresholdClusterBuilderTool )
DECLARE_TOOL_FACTORY( CscPeakThresholdClusterBuilderTool )
DECLARE_TOOL_FACTORY( CalibCscStripFitter )
DECLARE_TOOL_FACTORY( SimpleCscClusterFitter )
DECLARE_TOOL_FACTORY( QratCscClusterFitter )
DECLARE_TOOL_FACTORY( CscSplitClusterFitter )
DECLARE_TOOL_FACTORY( ParabolaCscClusterFitter )
DECLARE_TOOL_FACTORY( CscClusterUtilTool )
DECLARE_TOOL_FACTORY( CscAlignmentTool )
  
DECLARE_FACTORY_ENTRIES(CscClusterization) {
    DECLARE_ALGORITHM ( CscThresholdClusterBuilder )
    DECLARE_TOOL ( CscThresholdClusterBuilderTool )
    DECLARE_TOOL ( CscPeakThresholdClusterBuilderTool )
    DECLARE_TOOL ( CalibCscStripFitter )
    DECLARE_TOOL ( SimpleCscClusterFitter )
    DECLARE_TOOL ( QratCscClusterFitter )
    DECLARE_TOOL ( CscSplitClusterFitter )
    DECLARE_TOOL ( ParabolaCscClusterFitter )
    DECLARE_TOOL ( CscClusterUtilTool )
    DECLARE_TOOL ( CscAlignmentTool )
}
