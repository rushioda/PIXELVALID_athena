#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TileCalibAlgs/TileDigiNoiseCalibAlg.h"
#include "TileCalibAlgs/TileRawChNoiseCalibAlg.h"
#include "TileCalibAlgs/TileTopCalibAlg.h"
#include "TileCalibAlgs/TileLaserCalibAlg.h"
#include "TileCalibAlgs/TileLaserDefaultCalibTool.h"
#include "TileCalibAlgs/TileLaserLinearityCalibTool.h"
#include "TileCalibAlgs/TileCisDefaultCalibTool.h"
#include "TileCalibAlgs/TileTriggerDefaultCalibTool.h"
#include "TileCalibAlgs/TileOFC2DBAlg.h"
#include "TileCalibAlgs/TileMuId2DBAlg.h"
#include "TileCalibAlgs/TileTOFTool.h"
#include "TileCalibAlgs/TileLaserTimingTool.h"
#include "TileCalibAlgs/TileInfoDump.h"
#include "TileCalibAlgs/TileExpertDump.h"

DECLARE_ALGORITHM_FACTORY( TileTopCalibAlg )
DECLARE_ALGORITHM_FACTORY( TileLaserCalibAlg )
DECLARE_ALGORITHM_FACTORY( TileDigiNoiseCalibAlg )
DECLARE_ALGORITHM_FACTORY( TileRawChNoiseCalibAlg )
DECLARE_ALGORITHM_FACTORY( TileOFC2DBAlg )
DECLARE_ALGORITHM_FACTORY( TileMuId2DBAlg )
DECLARE_ALGORITHM_FACTORY( TileInfoDump )
DECLARE_ALGORITHM_FACTORY( TileExpertDump )

DECLARE_TOOL_FACTORY( TileLaserDefaultCalibTool )
DECLARE_TOOL_FACTORY( TileLaserLinearityCalibTool )
DECLARE_TOOL_FACTORY( TileCisDefaultCalibTool )
DECLARE_TOOL_FACTORY( TileTriggerDefaultCalibTool )
DECLARE_TOOL_FACTORY( TileTOFTool )
DECLARE_TOOL_FACTORY( TileLaserTimingTool )

DECLARE_FACTORY_ENTRIES(TileCalibAlgs) {
  DECLARE_ALGORITHM( TileTopCalibAlg )
  DECLARE_ALGORITHM( TileLaserCalibAlg )
  DECLARE_ALGORITHM( TileDigiNoiseCalibAlg )
  DECLARE_ALGORITHM( TileRawChNoiseCalibAlg )
  DECLARE_ALGORITHM( TileOFC2DBAlg )
  DECLARE_ALGORITHM( TileMuId2DBAlg )
  DECLARE_ALGORITHM( TileInfoDump )
  DECLARE_ALGORITHM( TileExpertDump )
  DECLARE_TOOL( TileLaserDefaultCalibTool )
  DECLARE_TOOL( TileLaserLinearityCalibTool )
  DECLARE_TOOL( TileCisDefaultCalibTool )
  DECLARE_TOOL( TileTriggerDefaultCalibTool )
  DECLARE_TOOL( TileTOFTool )
  DECLARE_TOOL( TileLaserTimingTool )
}

