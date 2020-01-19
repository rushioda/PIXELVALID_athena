#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TileConditions/TileInfoLoader.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondToolOfc.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileConditions/TileCellNoiseTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileExpertToolEmscale.h"
#include "TileConditions/TileCondToolIntegrator.h"
#include "TileConditions/TileCondToolMuID.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolPulseShape.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileConditions/TileCondToolNoiseRawChn.h"
#include "TileConditions/TileCondProxyWrapper.h"
#include "TileConditions/TileDCSSvc.h"
#include "TileConditions/TileCondDCS_Data.h"
#include "TileConditions/TileCondToolAutoCr.h"
#include "TileConditions/TileCondToolTMDB.h"
#include "TileConditions/TileCondToolDspThreshold.h"


DECLARE_SERVICE_FACTORY( TileInfoLoader )
DECLARE_SERVICE_FACTORY( TileCablingSvc )
DECLARE_SERVICE_FACTORY( TileDCSSvc )
DECLARE_TOOL_FACTORY( TileCondIdTransforms )
DECLARE_TOOL_FACTORY( TileBadChanTool )
DECLARE_TOOL_FACTORY( TileCondToolOfc )
DECLARE_TOOL_FACTORY( TileCondToolOfcCool )
DECLARE_TOOL_FACTORY( TileCellNoiseTool )
DECLARE_TOOL_FACTORY( TileCondToolEmscale )
DECLARE_TOOL_FACTORY( TileExpertToolEmscale )
DECLARE_TOOL_FACTORY( TileCondToolIntegrator)
DECLARE_TOOL_FACTORY( TileCondToolMuID)
DECLARE_TOOL_FACTORY( TileCondToolTiming )
DECLARE_TOOL_FACTORY( TileCondToolPulseShape )
DECLARE_TOOL_FACTORY( TileCondToolNoiseSample )
DECLARE_TOOL_FACTORY( TileCondToolAutoCr )
DECLARE_TOOL_FACTORY( TileCondToolNoiseRawChn )
DECLARE_TOOL_FACTORY( TileCondToolTMDB )
DECLARE_TOOL_FACTORY( TileCondToolDspThreshold )
DECLARE_TOOL_FACTORY( TileCondProxyCoolFlt )
DECLARE_TOOL_FACTORY( TileCondProxyCoolBch )
DECLARE_TOOL_FACTORY( TileCondProxyCoolOfc )
DECLARE_TOOL_FACTORY( TileCondProxyFileFlt )
DECLARE_TOOL_FACTORY( TileCondProxyFileBch )

DECLARE_FACTORY_ENTRIES(TileConditions) {
  DECLARE_SERVICE( TileInfoLoader )
  DECLARE_SERVICE( TileCablingSvc )
  DECLARE_SERVICE( TileDCSSvc )
  DECLARE_ALGTOOL( TileCondIdTransforms )
  DECLARE_ALGTOOL( TileBadChanTool )
  DECLARE_ALGTOOL( TileCondToolOfc )
  DECLARE_ALGTOOL( TileCondToolOfcCool )
  DECLARE_ALGTOOL( TileCellNoiseTool )
  DECLARE_ALGTOOL( TileCondToolEmscale )
  DECLARE_ALGTOOL( TileExpertToolEmscale )
  DECLARE_ALGTOOL( TileCondToolIntegrator )
  DECLARE_ALGTOOL( TileCondToolMuID )
  DECLARE_ALGTOOL( TileCondToolTiming )
  DECLARE_ALGTOOL( TileCondToolPulseShape )
  DECLARE_ALGTOOL( TileCondToolNoiseSample )
  DECLARE_ALGTOOL( TileCondToolAutoCr )
  DECLARE_ALGTOOL( TileCondToolNoiseRawChn )
  DECLARE_ALGTOOL( TileCondProxyFltCoolFlt )
  DECLARE_ALGTOOL( TileCondProxyFltCoolBch )
  DECLARE_ALGTOOL( TileCondProxyFltCoolOfc )
  DECLARE_ALGTOOL( TileCondProxyFltFileFlt )
  DECLARE_ALGTOOL( TileCondProxyFltFileBch )

}

