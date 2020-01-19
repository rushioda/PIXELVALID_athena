#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkDetDescrAlgs/MaterialMapping.h"
#include "TrkDetDescrAlgs/MaterialManipulation.h"
#include "TrkDetDescrAlgs/MaterialValidation.h"

using namespace Trk;

DECLARE_ALGORITHM_FACTORY( MaterialMapping )
DECLARE_ALGORITHM_FACTORY( MaterialManipulation )
DECLARE_ALGORITHM_FACTORY( MaterialValidation )


/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkDetDescrAlgs )
{
    DECLARE_ALGORITHM( MaterialMapping )
    DECLARE_ALGORITHM( MaterialManipulation )
    DECLARE_ALGORITHM( MaterialValidation )
}
