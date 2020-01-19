
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrkVertexTools/VertexCollectionSortingTool.h"
#include "TrkVertexTools/VertexMergingTool.h"
#include "TrkVertexTools/DummyVertexSelectionTool.h" 
#include "TrkVertexTools/TruthVertexSelectionTool.h"  
#include "TrkVertexTools/VertexStoringTool.h"
 
using namespace Trk;
 
DECLARE_TOOL_FACTORY(VertexCollectionSortingTool)
DECLARE_TOOL_FACTORY(VertexMergingTool)
DECLARE_TOOL_FACTORY(DummyVertexSelectionTool)
DECLARE_TOOL_FACTORY(TruthVertexSelectionTool)
DECLARE_TOOL_FACTORY(VertexStoringTool)
    
DECLARE_FACTORY_ENTRIES(TrkVertexTools)
{
  DECLARE_TOOL(VertexCollectionSortingTool)
  DECLARE_TOOL(VertexMergingTool)
  DECLARE_TOOL(DummyVertexSelectionTool)
  DECLARE_TOOL(TruthVertexSelectionTool)
  DECLARE_TOOL(VertexStoringTool)
}
 

 
