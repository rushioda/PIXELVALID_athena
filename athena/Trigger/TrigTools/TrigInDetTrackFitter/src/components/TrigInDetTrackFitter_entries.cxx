#include "TrigInDetTrackFitter/TrigInDetTrackFitter.h"
#include "TrigInDetTrackFitter/TrigInDetBremDetectionTool.h"
#include "TrigInDetTrackFitter/TrigDkfTrackMakerTool.h"
#include "TrigInDetTrackFitter/TrigL2ResidualCalculator.h"
#include "TrigInDetTrackFitter/TrigInDetOfflineTrackFitter.h"
#include "TrigInDetTrackFitter/TrigInDetCombinedTrackFitter.h"
#include "TrigInDetTrackFitter/TrigL2HighPtTrackFitter.h"
#include "TrigInDetTrackFitter/TrigL2LowPtTrackFitter.h"
#include "TrigInDetTrackFitter/TrigL2FastExtrapolationTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TrigInDetTrackFitter)
DECLARE_TOOL_FACTORY(TrigInDetBremDetectionTool)
DECLARE_TOOL_FACTORY(TrigDkfTrackMakerTool)
DECLARE_TOOL_FACTORY(TrigL2ResidualCalculator)
DECLARE_TOOL_FACTORY(TrigInDetOfflineTrackFitter)
DECLARE_TOOL_FACTORY(TrigInDetCombinedTrackFitter)
DECLARE_TOOL_FACTORY(TrigL2HighPtTrackFitter)
DECLARE_TOOL_FACTORY(TrigL2LowPtTrackFitter)
DECLARE_TOOL_FACTORY(TrigL2FastExtrapolationTool)

DECLARE_FACTORY_ENTRIES(TrigInDetTrackFitter)
{
  DECLARE_TOOL(TrigInDetTrackFitter)
  DECLARE_TOOL(TrigInDetBremDetectionTool)
  DECLARE_TOOL(TrigDkfTrackMakerTool)
  DECLARE_TOOL(TrigL2ResidualCalculator)
  DECLARE_TOOL(TrigInDetOfflineTrackFitter)
  DECLARE_TOOL(TrigInDetCombinedTrackFitter)
  DECLARE_TOOL(TrigL2HighPtTrackFitter)
  DECLARE_TOOL(TrigL2LowPtTrackFitter)
  DECLARE_TOOL(TrigL2FastExtrapolationTool)
}
