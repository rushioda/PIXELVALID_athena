// $Id: TrigAnalysisExamples_entries.cxx 770909 2016-08-30 19:59:36Z rwhite $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// derived algorithm classes
#include "../TDTExample.h"
#include "../TagAndProbeExAlg.h"
#include "../JetMetExAlg.h"
#include "../TrigMatchExAlg.h"
#include "../TriggerAnalysisTutorial.h"
#include "../TrigExpressStreamAlg.h"
#include "../TrigEmulatorExAlg.h"
#include "../BunchCrossingExample.h"
#include "../TriggerAnalysisHelper.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig, TDTExample )
DECLARE_ALGORITHM_FACTORY(TagAndProbeExAlg)
DECLARE_ALGORITHM_FACTORY(JetMetExAlg)
DECLARE_ALGORITHM_FACTORY(TrigMatchExAlg)
DECLARE_ALGORITHM_FACTORY(TriggerAnalysisTutorial)
DECLARE_ALGORITHM_FACTORY(TrigExpressStreamAlg)
DECLARE_ALGORITHM_FACTORY(TrigEmulatorExAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trig, BunchCrossingExample )
DECLARE_NAMESPACE_TOOL_FACTORY(Trig, TriggerAnalysisHelper)
DECLARE_FACTORY_ENTRIES( TrigAnalysisExamples ) {
  DECLARE_NAMESPACE_ALGORITHM(Trig, TDTExample)
  DECLARE_ALGORITHM(TagAndProbeExAlg)
  DECLARE_ALGORITHM(JetMetExAlg)
  DECLARE_ALGORITHM(TrigMatchExAlg)
  DECLARE_ALGORITHM(TrigExpressStreamAlg)
  DECLARE_ALGORITHM(TrigEmulatorExAlg)
  DECLARE_ALGORITHM(TriggerAnalysisTutorial)
  DECLARE_NAMESPACE_ALGORITHM( Trig, BunchCrossingExample )
  DECLARE_NAMESPACE_TOOL(Trig, TriggerAnalysisHelper)
}

