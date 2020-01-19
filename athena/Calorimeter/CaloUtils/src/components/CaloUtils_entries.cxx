#include "CaloUtils/LocalNoiseSuppressionTool.h"
#include "CaloUtils/CaloCellWeightCorrection.h"
#include "CaloUtils/CaloLCClassificationTool.h"
#include "CaloUtils/EstimEMClusterClassificationTool.h"
#include "CaloUtils/CaloLCWeightTool.h"
#include "CaloUtils/CaloLCOutOfClusterTool.h"
#include "CaloUtils/CaloLCDeadMaterialTool.h"
#include "CaloUtils/CaloTopoTowerBuilderTool.h"
#include "CaloUtils/CaloTowerBuilderTool.h"
#include "CaloUtils/xAODClusterCompressor.h"
#include "../CaloTowerStoreTestAlg.h"
#include "../CaloTowerBuilderToolTestAlg.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( CaloCellWeightCorrection )
DECLARE_TOOL_FACTORY( LocalNoiseSuppressionTool )
DECLARE_TOOL_FACTORY( CaloLCClassificationTool )
DECLARE_TOOL_FACTORY( EstimEMClusterClassificationTool )
DECLARE_TOOL_FACTORY( CaloLCWeightTool )
DECLARE_TOOL_FACTORY( CaloLCOutOfClusterTool )
DECLARE_TOOL_FACTORY( CaloLCDeadMaterialTool )
DECLARE_TOOL_FACTORY( CaloTopoTowerBuilderTool )
DECLARE_TOOL_FACTORY( CaloTowerBuilderTool )
DECLARE_TOOL_FACTORY( xAODClusterCompressor )

DECLARE_ALGORITHM_FACTORY( CaloTowerStoreTestAlg )
DECLARE_ALGORITHM_FACTORY( CaloTowerBuilderToolTestAlg )

DECLARE_FACTORY_ENTRIES(CaloUtils) {
    DECLARE_TOOL( CaloCellWeightCorrection )
    DECLARE_TOOL( LocalNoiseSuppressionTool )
    DECLARE_TOOL( CaloLCClassificationTool )
    DECLARE_TOOL( EstimEMClusterClassificationTool )
    DECLARE_TOOL( CaloLCWeightTool )
    DECLARE_TOOL( CaloLCOutOfClusterTool )
    DECLARE_TOOL( CaloLCDeadMaterialTool )
    DECLARE_TOOL( CaloTopoTowerBuilderTool )
    DECLARE_TOOL( CaloTowerBuilderTool )
    DECLARE_TOOL( xAODClusterCompressor )
    DECLARE_ALGORITHM( CaloTowerStoreTestAlg )
    DECLARE_ALGORITHM( CaloTowerBuilderToolTestAlg )
}
