#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonCondTool/MuonAlignmentDbTool.h"
#include "MuonCondTool/MultiLevelAlignmentDbTool.h"
#include "MuonCondTool/MDT_DCSConditionsTool.h"
#include "MuonCondTool/MDT_DCSConditionsRun2Tool.h"
#include "MuonCondTool/RpcDetectorStatusDbTool.h"
#include "MuonCondTool/MDT_DeadTubeConditionsTool.h"
#include "MuonCondTool/TGC_STATUSConditionsTool.h"
#include "MuonCondTool/MDT_MapConversion.h"
#include "MuonCondTool/CSC_DCSConditionsTool.h"
#include "MuonCondTool/RPC_DCSConditionsTool.h"
#include "MuonCondTool/MDT_DQConditionsTool.h"
#include "MuonCondTool/MuonAlignmentErrorDbTool.h"

DECLARE_TOOL_FACTORY( MuonAlignmentDbTool )
DECLARE_TOOL_FACTORY( MultiLevelAlignmentDbTool )
DECLARE_TOOL_FACTORY( MDT_DCSConditionsTool )
DECLARE_TOOL_FACTORY( CSC_DCSConditionsTool )
DECLARE_TOOL_FACTORY( RPC_DCSConditionsTool )
DECLARE_TOOL_FACTORY( TGC_STATUSConditionsTool )
DECLARE_TOOL_FACTORY( RpcDetectorStatusDbTool )
DECLARE_TOOL_FACTORY( MDT_DeadTubeConditionsTool )
DECLARE_TOOL_FACTORY( MDT_DQConditionsTool )
DECLARE_TOOL_FACTORY( MDT_MapConversion )
DECLARE_TOOL_FACTORY( MuonAlignmentErrorDbTool )
DECLARE_TOOL_FACTORY( MDT_DCSConditionsRun2Tool )

DECLARE_FACTORY_ENTRIES( MuonCondTool ){
   DECLARE_TOOL( MuonAlignmentDbTool ) 
   DECLARE_TOOL( MultiLevelAlignmentDbTool )
   DECLARE_TOOL( MDT_DCSConditionsTool )
   DECLARE_TOOL( CSC_DCSConditionsTool )
   DECLARE_TOOL( MDT_DeadTubeConditionsTool )
   DECLARE_TOOL( MDT_DQConditionsTool )
   DECLARE_TOOL( MDT_MapConversion )
   DECLARE_TOOL( RpcDetectorStatusDbTool )
   DECLARE_TOOL( RPC_DCSConditionsTool )
   DECLARE_TOOL( TGC_STATUSConditionsTool ) 
   DECLARE_TOOL( MuonAlignmentErrorDbTool )
   DECLARE_TOOL( MDT_DCSConditionsRun2Tool )	
}

