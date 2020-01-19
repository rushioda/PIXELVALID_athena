#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "JpsiUpsilonTools/JpsiPlus1Track.h"
#include "JpsiUpsilonTools/JpsiPlus2Tracks.h"
//#include "JpsiUpsilonTools/JpsiEEFinder.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"
#include "JpsiUpsilonTools/JpsiAlg.h"
#include "JpsiUpsilonTools/JpsiExample.h"
//#include "JpsiUpsilonTools/JpsiEEAlg.h"
//#include "JpsiUpsilonTools/JpsiEEExample.h"
//#include "JpsiUpsilonTools/JpsiJpsiEEExample.h"

using namespace Analysis;

DECLARE_TOOL_FACTORY( JpsiFinder )
DECLARE_TOOL_FACTORY( JpsiPlus1Track )
DECLARE_TOOL_FACTORY( JpsiPlus2Tracks )
//DECLARE_TOOL_FACTORY( JpsiEEFinder )
DECLARE_TOOL_FACTORY( PrimaryVertexRefitter )
DECLARE_ALGORITHM_FACTORY( JpsiAlg )
//DECLARE_ALGORITHM_FACTORY( JpsiEEAlg ) 
DECLARE_ALGORITHM_FACTORY( JpsiExample )
//DECLARE_ALGORITHM_FACTORY( JpsiEEExample )
//DECLARE_ALGORITHM_FACTORY( JpsiJpsiEEExample )

DECLARE_FACTORY_ENTRIES( JpsiUpsilonTools ) {
    DECLARE_ALGORITHM( JpsiAlg )
    //DECLARE_ALGORITHM( JpsiEEAlg )
    DECLARE_ALGORITHM( JpsiExample )
    //DECLARE_ALGORITHM( JpsiEEExample )
    //DECLARE_ALGORITHM( JpsiJpsiEEExample )
    DECLARE_TOOL( Analysis::JpsiFinder )
    DECLARE_TOOL( Analysis::JpsiPlus1Track )
    DECLARE_TOOL( Analysis::JpsiPlus2Tracks )
    //DECLARE_TOOL( Analysis::JpsiEEFinder )
    DECLARE_TOOL( Analysis::PrimaryVertexRefitter )
}

