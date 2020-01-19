#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1CaloCalibUtils/L1CaloHVCorrectionsForDB.h"
#include "TrigT1CaloCalibUtils/L1CaloHVDummyContainers.h"
#include "TrigT1CaloCalibUtils/L1CaloLinearCalibration.h"
#include "TrigT1CaloCalibUtils/L1CaloPedestalMaker.h"
#include "TrigT1CaloCalibUtils/L1CaloPedestalGenerator.h"
#include "TrigT1CaloCalibUtils/L1CaloRampMaker.h"
#include "TrigT1CaloCalibUtils/L1CaloTriggerTowerSelector.h"
#include "TrigT1CaloCalibUtils/L1CaloPprPhos4ShapeMaker.h"
#include "TrigT1CaloCalibUtils/L1CaloPprMonitoring.h"

DECLARE_ALGORITHM_FACTORY( L1CaloHVCorrectionsForDB )
DECLARE_ALGORITHM_FACTORY( L1CaloHVDummyContainers )
DECLARE_ALGORITHM_FACTORY( L1CaloLinearCalibration )
DECLARE_ALGORITHM_FACTORY( L1CaloPedestalMaker )
DECLARE_ALGORITHM_FACTORY( L1CaloPedestalGenerator )
DECLARE_ALGORITHM_FACTORY( L1CaloRampMaker )
DECLARE_ALGORITHM_FACTORY( L1CaloTriggerTowerSelector )
DECLARE_ALGORITHM_FACTORY( L1CaloPprPhos4ShapeMaker )
DECLARE_ALGORITHM_FACTORY( L1CaloPprMonitoring )

#include "../L1CaloDumpRampData.h"
DECLARE_ALGORITHM_FACTORY( L1CaloDumpRampData )

DECLARE_FACTORY_ENTRIES( TrigT1CaloCalibUtils )
{
  DECLARE_ALGORITHM( L1CaloDumpRampData );
  DECLARE_ALGORITHM( L1CaloHVCorrectionsForDB )
  DECLARE_ALGORITHM( L1CaloHVDummyContainers )
  DECLARE_ALGORITHM( L1CaloLinearCalibration )
  DECLARE_ALGORITHM( L1CaloPedestalMaker )
  DECLARE_ALGORITHM( L1CaloPedestalGenerator )
  DECLARE_ALGORITHM( L1CaloRampMaker )
  DECLARE_ALGORITHM( L1CaloTriggerTowerSelector )
  DECLARE_ALGORITHM( L1CaloPprPhos4ShapeMaker )
  DECLARE_ALGORITHM( L1CaloPprMonitoring )
}
