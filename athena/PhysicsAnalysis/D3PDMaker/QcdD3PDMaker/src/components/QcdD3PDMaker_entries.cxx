#include "GaudiKernel/DeclareFactoryEntries.h"
#include "QcdD3PDMaker/preprocessing/QcdD3PDPreProcessing.h"
#include "QcdD3PDMaker/fillers/MuonSpShowerFillerTool.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY (QcdD3PD, QcdD3PDPreProcessing)
DECLARE_NAMESPACE_TOOL_FACTORY      (QcdD3PD, MuonSpShowerFillerTool)

DECLARE_FACTORY_ENTRIES(QcdD3PDMaker) {
  DECLARE_NAMESPACE_ALGORITHM (QcdD3PD, QcdD3PDPreProcessing)
  DECLARE_NAMESPACE_TOOL(QcdD3PD, MuonSpShowerFillerTool) 
}
