#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../CscRdoToCscPrepDataTool.h"
#include "../CSC_RawDataProviderTool.h"
#include "../CscRdoContByteStreamTool.h"
#include "../CscROD_Decoder.h"
#include "../CscRDO_Decoder.h"
#include "../CscDigitToCscRDOTool.h"
  
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CscRdoToCscPrepDataTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CscRdoContByteStreamTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CSC_RawDataProviderTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CscROD_Decoder)
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CscRDO_Decoder)
DECLARE_TOOL_FACTORY( CscDigitToCscRDOTool )

DECLARE_FACTORY_ENTRIES( MuonCSC_CnvTools )
{
  DECLARE_NAMESPACE_TOOL( Muon, CscRdoToCscPrepDataTool )
  DECLARE_NAMESPACE_TOOL( Muon, CscRdoContByteStreamTool )
  DECLARE_NAMESPACE_TOOL( Muon, CSC_RawDataProviderTool )
  DECLARE_NAMESPACE_TOOL( Muon, CscROD_Decoder)
  DECLARE_NAMESPACE_TOOL( Muon, CscRDO_Decoder)
  DECLARE_TOOL( CscDigitToCscRDOTool )
}

