#include "src/AFP_TDSimHitCollectionCnv.h"
#include "src/AFP_TDDigiCollectionCnv.h"

#include "src/AFP_SIDSimHitCollectionCnv.h"
#include "src/AFP_SiDigiCollectionCnv.h"

#include "src/AFP_TDLocRecoEvCollectionCnv.h"
#include "src/AFP_SIDLocRecoEvCollectionCnv.h"

DECLARE_ALGORITHM_FACTORY( AFP_TDSimHitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( AFP_TDDigiCollectionCnv )
DECLARE_ALGORITHM_FACTORY( AFP_SIDSimHitCollectionCnv )
DECLARE_ALGORITHM_FACTORY( AFP_SiDigiCollectionCnv )

DECLARE_ALGORITHM_FACTORY( AFP_TDLocRecoEvCollectionCnv )
DECLARE_ALGORITHM_FACTORY( AFP_SIDLocRecoEvCollectionCnv )

DECLARE_FACTORY_ENTRIES(AFP_EventAthenaPool) {
	DECLARE_ALGORITHM( AFP_TDSimHitCollectionCnv )
        DECLARE_ALGORITHM( AFP_TDDigiCollectionCnv )
	DECLARE_ALGORITHM( AFP_SIDSimHitCollectionCnv )
	  DECLARE_ALGORITHM( AFP_SiDigiCollectionCnv )
	
	DECLARE_ALGORITHM( AFP_TDLocRecoEvCollectionCnv )
	DECLARE_ALGORITHM( AFP_SIDLocRecoEvCollectionCnv )
}

