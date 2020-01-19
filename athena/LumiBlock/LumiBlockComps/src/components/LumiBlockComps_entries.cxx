#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LumiBlockComps/CreateLumiBlockCollectionFromFile.h"
//#include "LumiBlockComps/CreateAANTFromLumiBlockCollection.h"
#include "LumiBlockComps/LumiBlockMetaDataTool.h"

#ifndef XAOD_ANALYSIS
//full-athena-only components
#include "LumiBlockComps/LumiBlockMuTool.h"
#include "LumiBlockComps/LuminosityTool.h"
#include "LumiBlockComps/TrigLivefractionTool.h"
#include "LumiBlockComps/LumiCalcSvc.h"
#include "LumiBlockComps/LumiBlockMuWriter.h"
#include "LumiBlockComps/LumiBlockTester.h"
#endif

DECLARE_ALGORITHM_FACTORY( CreateLumiBlockCollectionFromFile )
//DECLARE_ALGORITHM_FACTORY( CreateAANTFromLumiBlockCollection )
#ifndef XAOD_ANALYSIS
DECLARE_ALGORITHM_FACTORY( LumiBlockMuWriter )
DECLARE_ALGORITHM_FACTORY( LumiBlockTester )
DECLARE_TOOL_FACTORY( LumiBlockMuTool )
DECLARE_TOOL_FACTORY( LuminosityTool )
DECLARE_TOOL_FACTORY( TrigLivefractionTool )
DECLARE_SERVICE_FACTORY( LumiCalcSvc )
#endif
DECLARE_TOOL_FACTORY( LumiBlockMetaDataTool )

DECLARE_FACTORY_ENTRIES(LumiBlockComps) {
  DECLARE_ALGORITHM( CreateLumiBlockCollectionFromFile );
  //  DECLARE_ALGORITHM( CreateAANTFromLumiBlockCollection );
  DECLARE_ALGTOOL( LumiBlockMetaDataTool);

#ifndef XAOD_ANALYSIS
  DECLARE_ALGORITHM( LumiBlockMuWriter );
  DECLARE_ALGORITHM( LumiBlockTester );
  DECLARE_ALGTOOL( LumiBlockMuTool);
  DECLARE_ALGTOOL( LuminosityTool);
  DECLARE_ALGTOOL( TrigLivefractionTool);
  DECLARE_SERVICE( LumiCalcSvc );
#endif
}

