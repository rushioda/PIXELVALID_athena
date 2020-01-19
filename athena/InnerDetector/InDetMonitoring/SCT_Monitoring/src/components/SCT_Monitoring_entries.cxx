#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SCT_Monitoring/SCTMotherTrigMonTool.h"
#include "SCT_Monitoring/SCTTracksMonTool.h"
#include "SCT_Monitoring/SCTLorentzMonTool.h"
#include "SCT_Monitoring/SCTRatioNoiseMonTool.h"
#include "SCT_Monitoring/SCTErrMonTool.h"
#include "SCT_Monitoring/SCTHitEffMonTool.h"
#include "SCT_Monitoring/SCTHitsNoiseMonTool.h"
#include "SCT_Monitoring/SCTSummaryMonTool.h"

using namespace SCT_Monitoring;

DECLARE_TOOL_FACTORY(SCTMotherTrigMonTool)
DECLARE_TOOL_FACTORY(SCTTracksMonTool)
DECLARE_TOOL_FACTORY(SCTLorentzMonTool)
DECLARE_TOOL_FACTORY(SCTRatioNoiseMonTool)
DECLARE_TOOL_FACTORY(SCTErrMonTool)
DECLARE_TOOL_FACTORY(SCTHitEffMonTool)
DECLARE_TOOL_FACTORY(SCTHitsNoiseMonTool)
DECLARE_TOOL_FACTORY(SCTSummaryMonTool)


DECLARE_FACTORY_ENTRIES(SCT_Monitoring) {
  DECLARE_ALGTOOL(SCTMotherTrigMonTool)
  DECLARE_ALGTOOL(SCTTracksMonTool)
  DECLARE_ALGTOOL(SCTLorentzMonTool)
  DECLARE_ALGTOOL(SCTRatioNoiseMonTool)
  DECLARE_ALGTOOL(SCTErrMonTool)
  DECLARE_ALGTOOL(SCTHitEffMonTool)
  DECLARE_ALGTOOL(SCTHitsNoiseMonTool)
  DECLARE_ALGTOOL(SCTSummaryMonTool)
}

