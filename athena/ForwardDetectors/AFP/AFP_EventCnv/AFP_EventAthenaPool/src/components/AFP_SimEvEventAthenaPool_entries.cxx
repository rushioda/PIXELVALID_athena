#include "src/AFP_TDSimHitCollectionCnv.h"
#include "src/AFP_TDDigiCollectionCnv.h"
#include "src/AFP_SIDSimHitCollectionCnv.h"

DECLARE_ALGORITHM_FACTORY( AFP_TDSimHitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( AFP_TDSimDigiCollectionCnv )
DECLARE_ALGORITHM_FACTORY( AFP_SIDSimHitCollectionCnv )

DECLARE_FACTORY_ENTRIES(AFP_EventAthenaPool) {
	DECLARE_ALGORITHM( AFP_TDSimHitCollectionCnv )
        DECLARE_ALGORITHM( AFP_TDDigiCollectionCnv ) 
	DECLARE_ALGORITHM( AFP_SIDSimHitCollectionCnv )
}

