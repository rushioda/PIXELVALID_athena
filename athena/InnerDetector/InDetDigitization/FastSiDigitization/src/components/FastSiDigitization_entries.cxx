#include "GaudiKernel/DeclareFactoryEntries.h"

#include "FastSiDigitization/PixelFastDigitizationTool.h"
#include "FastSiDigitization/PixelFastDigitization.h"
#include "FastSiDigitization/IPixelFastDigitizationTool.h"

#include "FastSiDigitization/SCT_FastDigitizationTool.h"
#include "FastSiDigitization/SCT_FastDigitization.h"
#include "FastSiDigitization/ISCT_FastDigitizationTool.h"

#include "FastSiDigitization/SiSmearedDigitizationTool.h"
#include "FastSiDigitization/ISiSmearedDigitizationTool.h"
#include "FastSiDigitization/SiSmearedDigitization.h"

DECLARE_ALGORITHM_FACTORY(PixelFastDigitization)
DECLARE_ALGORITHM_FACTORY(SCT_FastDigitization)
DECLARE_ALGORITHM_FACTORY(SiSmearedDigitization)
DECLARE_TOOL_FACTORY(PixelFastDigitizationTool)
DECLARE_TOOL_FACTORY(SiSmearedDigitizationTool)
DECLARE_TOOL_FACTORY(SCT_FastDigitizationTool)

DECLARE_FACTORY_ENTRIES(FastSiDigitization) {
  DECLARE_ALGORITHM(PixelFastDigitization)
  DECLARE_ALGORITHM(SCT_FastDigitization)
  DECLARE_ALGORITHM(SiSmearedDigitization)
  DECLARE_TOOL(PixelFastDigitizationTool)
  DECLARE_TOOL(SiSmearedDigitizationTool)
  DECLARE_TOOL(SCT_FastDigitizationTool)

}


