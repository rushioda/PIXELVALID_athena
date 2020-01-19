#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonAlignGenTools/CommonGeometryManagerTool.h"
#include "MuonAlignGenTools/MuonGeometryManagerTool.h"
#include "MuonAlignGenTools/MdtGeometryManagerTool.h"
#include "MuonAlignGenTools/CscGeometryManagerTool.h"
#include "MuonAlignGenTools/TgcGeometryManagerTool.h"
#include "MuonAlignGenTools/MuonFillNtupleTool.h"
#include "MuonAlignGenTools/MdtAlignDBTool.h"
#include "MuonAlignGenTools/CscAlignDBTool.h"
#include "MuonAlignGenTools/MuonAlignModuleTool.h"
#include "MuonAlignGenTools/MuonTrackPreProcessor.h"
#include "MuonAlignGenTools/MuonTrackCollectionProvider.h"
#include "MuonAlignGenTools/MuonAlignHelperTool.h"
#include "MuonAlignGenTools/MuonAlignRefitTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonAlignModuleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CommonGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MdtGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CscGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TgcGeometryManagerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonFillNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonAlignDBTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MdtAlignDBTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, CscAlignDBTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonAlignHelperTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonAlignRefitTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonTrackPreProcessor )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonTrackCollectionProvider )
//DECLARE_NAMESPACE_TOOL_FACTORY( Muon, UnbiasedResidualTool )
//DECLARE_NAMESPACE_ALGORITHM_FACTORY( Muon, UnbiasedMuonResiduals )

DECLARE_FACTORY_ENTRIES( MuonAlignGenTools )
{
  DECLARE_NAMESPACE_TOOL( Muon, MuonAlignModuleTool )
  DECLARE_NAMESPACE_TOOL( Muon, CommonGeometryManagerTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonGeometryManagerTool )
  DECLARE_NAMESPACE_TOOL( Muon, MdtGeometryManagerTool )
  DECLARE_NAMESPACE_TOOL( Muon, CscGeometryManagerTool )
  DECLARE_NAMESPACE_TOOL( Muon, TgcGeometryManagerTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonFillNtupleTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonAlignDBTool )
  DECLARE_NAMESPACE_TOOL( Muon, MdtAlignDBTool )
  DECLARE_NAMESPACE_TOOL( Muon, CscAlignDBTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonAlignHelperTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonAlignRefitTool )
  DECLARE_NAMESPACE_TOOL( Muon, MuonTrackPreProcessor )
  DECLARE_NAMESPACE_TOOL( Muon, MuonTrackCollectionProvider )
  //DECLARE_NAMESPACE_TOOL( Muon, UnbiasedResidualTool )
  //DECLARE_NAMESPACE_ALGORITHM( Muon, UnbiasedMuonResiduals )
}
