#include "MuonJiveXML/CSCClusterRetriever.h"
#include "MuonJiveXML/TgcPrepDataRetriever.h"
#include "MuonJiveXML/CscPrepDataRetriever.h"
#include "MuonJiveXML/MdtPrepDataRetriever.h"
#include "MuonJiveXML/RpcPrepDataRetriever.h"
//#include "MuonJiveXML/MuidTrackRetriever.h"
#include "MuonJiveXML/TrigRpcDataRetriever.h"
#include "MuonJiveXML/TrigMuonROIRetriever.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(CSCClusterRetriever)
DECLARE_TOOL_FACTORY(TgcPrepDataRetriever)
DECLARE_TOOL_FACTORY(CscPrepDataRetriever)
DECLARE_TOOL_FACTORY(MdtPrepDataRetriever)
DECLARE_TOOL_FACTORY(RpcPrepDataRetriever)
//DECLARE_TOOL_FACTORY(MuidTrackRetriever)
DECLARE_TOOL_FACTORY(TrigRpcDataRetriever)
DECLARE_TOOL_FACTORY(TrigMuonROIRetriever)



DECLARE_FACTORY_ENTRIES(MuonJiveXML) {
  DECLARE_ALGTOOL(CSCClusterRetriever)
  DECLARE_ALGTOOL(TgcPrepDataRetriever)
  DECLARE_ALGTOOL(CscPrepDataRetriever)
  DECLARE_ALGTOOL(MdtPrepDataRetriever)
  DECLARE_ALGTOOL(RpcPrepDataRetriever)
//  DECLARE_ALGTOOL(MuidTrackRetriever)
  DECLARE_ALGTOOL(TrigRpcDataRetriever)
  DECLARE_ALGTOOL(TrigMuonROIRetriever)
}
