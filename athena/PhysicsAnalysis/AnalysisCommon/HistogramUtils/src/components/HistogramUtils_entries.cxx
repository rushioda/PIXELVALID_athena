#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../HistogramTool.h"
#include "../HistAlg.h"

using namespace HistogramUtils;

DECLARE_TOOL_FACTORY( HistogramToolTH1 )
DECLARE_TOOL_FACTORY( HistogramToolTH2 )
DECLARE_TOOL_FACTORY( HistogramToolTH3 )

DECLARE_ALGORITHM_FACTORY( HistAlg )



DECLARE_FACTORY_ENTRIES( HistogramUtils )
{
  DECLARE_TOOL( HistogramToolTH1 );
  DECLARE_TOOL( HistogramToolTH2 );
  DECLARE_TOOL( HistogramToolTH3 );
  
  DECLARE_ALGORITHM( HistAlg );
}

