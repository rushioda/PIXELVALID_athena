#include "GaudiKernel/DeclareFactoryEntries.h"
#include "PFlowUtils/RetrievePFOTool.h"
#include "PFlowUtils/WeightPFOTool.h"
#include "PFlowUtils/ParticleFlowEventFilter_r207.h"

DECLARE_NAMESPACE_TOOL_FACTORY( CP, RetrievePFOTool)
DECLARE_NAMESPACE_TOOL_FACTORY( CP, WeightPFOTool)
DECLARE_ALGORITHM_FACTORY(ParticleFlowEventFilter_r207)

DECLARE_FACTORY_ENTRIES(PFlowUtils){
  DECLARE_NAMESPACE_TOOL (CP, RetrievePFOTool)
  DECLARE_NAMESPACE_TOOL (CP, WeightPFOTool)
  DECLARE_ALGORITHM(ParticleFlowEventFilter_r207)  
}
