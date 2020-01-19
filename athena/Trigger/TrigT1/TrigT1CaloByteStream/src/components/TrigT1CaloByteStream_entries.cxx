#include "GaudiKernel/DeclareFactoryEntries.h"
#include "DataModel/DataVector.h"



// ============================================================================
// RUN1:
// ============================================================================

#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/CPMTower.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/TriggerTower.h"


#include "../CpByteStreamV1Cnv.h"
#include "../CpmRoiByteStreamV1Cnv.h"
#include "../CpReadByteStreamV1Cnv.h"
#include "../JepByteStreamV1Cnv.h"
#include "../JepReadByteStreamV1Cnv.h"
#include "../JepRoiByteStreamV1Cnv.h"
#include "../JepRoiReadByteStreamV1Cnv.h"

// Both
#include "../CpReadByteStreamV1V2Cnv.h"
#include "../JepReadByteStreamV1V2Cnv.h"


// #include "TrigT1CaloEvent/CMXEtSums.h"
// #include "TrigT1CaloEvent/CMXJetTob.h"
// #include "TrigT1CaloEvent/CMXJetHits.h"
// #include "TrigT1CaloEvent/CMXRoI.h"
// #include "TrigT1CaloEvent/JEMTobRoI.h"


// ============================================================================
// RUN1 & RUN2
// ============================================================================
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

#include "../PpmByteStreamV1Cnv.h"
#include "../PpmByteStreamV1Tool.h"
#include "../xaod/PpmByteStreamReadV1V2Tool.h"
#include "../xaod/PpmByteStreamAuxCnv.h"
#include "../xaod/PpmByteStreamxAODCnv.h"


// ============================================================================
// RUN2
// ============================================================================
#include "TrigT1CaloEvent/CMXRoI.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"

#include "../JepRoiReadByteStreamV2Cnv.h"
#include "../JepRoiByteStreamV2Cnv.h"


#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"

#include "../CpByteStreamV2Tool.h"
#include "../xaod/CpmTowerByteStreamAuxCnv.h"
#include "../xaod/CpmTowerByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// CMXCPTOB
// ----------------------------------------------------------------------------
#include "xAODTrigL1Calo/CMXCPTob.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPTobAuxContainer.h"

#include "../xaod/CmxCpTobByteStreamAuxCnv.h"
#include "../xaod/CmxCpTobByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// CMXCPHITS
// ----------------------------------------------------------------------------
#include "xAODTrigL1Calo/CMXCPHits.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsAuxContainer.h"

#include "../xaod/CmxCpHitsByteStreamAuxCnv.h"
#include "../xaod/CmxCpHitsByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// CMXJETTOB
// ----------------------------------------------------------------------------
#include "xAODTrigL1Calo/CMXJetTob.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"

#include "../xaod/CmxJetTobByteStreamAuxCnv.h"
#include "../xaod/CmxJetTobByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// CMXJETHITS
// ----------------------------------------------------------------------------
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsAuxContainer.h"

#include "../xaod/CmxJetHitsByteStreamAuxCnv.h"
#include "../xaod/CmxJetHitsByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// CMXETSUMS
// ----------------------------------------------------------------------------
#include "../xaod/CmxEtSumsByteStreamAuxCnv.h"
#include "../xaod/CmxEtSumsByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// JEMETSUMS
// ----------------------------------------------------------------------------
#include "../xaod/JemEtSumsByteStreamAuxCnv.h"
#include "../xaod/JemEtSumsByteStreamxAODCnv.h"


// ----------------------------------------------------------------------------
// CPMTOBROI
// ----------------------------------------------------------------------------
#include "../xaod/CpmTobRoiByteStreamAuxCnv.h"
#include "../xaod/CpmTobRoiByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// JEMTOBROI
// ----------------------------------------------------------------------------
#include "../xaod/JemTobRoiByteStreamAuxCnv.h"
#include "../xaod/JemTobRoiByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// JETELEMENT
// ----------------------------------------------------------------------------
#include "../xaod/JetElementByteStreamAuxCnv.h"
#include "../xaod/JetElementByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// CMXROI
// ----------------------------------------------------------------------------
#include "../xaod/CmxRoIByteStreamAuxCnv.h"
#include "../xaod/CmxRoIByteStreamxAODCnv.h"

// ----------------------------------------------------------------------------
// RODHEADER
// ----------------------------------------------------------------------------
#include "../xaod/RodHeaderByteStreamAuxCnv.h"
#include "../xaod/RodHeaderByteStreamxAODCnv.h"

// ============================================================================

#include "../RodHeaderByteStreamCnv.h"
#include "../L1CaloErrorByteStreamCnv.h"

// Post-LS1

#include "../CpmRoiByteStreamV2Tool.h"
#include "../JepByteStreamV2Tool.h"
#include "../JepRoiByteStreamV2Tool.h"

// Pre-LS1
#include "../CpByteStreamV1Tool.h"
#include "../CpmRoiByteStreamV1Tool.h"
#include "../JepByteStreamV1Tool.h"
#include "../JepRoiByteStreamV1Tool.h"
// Both
#include "../RodHeaderByteStreamTool.h"
#include "../L1CaloErrorByteStreamTool.h"

// #include "../PpmByteStreamSubsetTool.h"
#include "../TriggerTowerSelectionTool.h"
#include "../TrigT1CaloDataAccess.h"
#include "../TrigT1CaloDataAccessV2.h"


namespace LVL1BS {

typedef DataVector<LVL1::JEMTobRoI>  JEMTobRoICollection;

// Pre-LS1
typedef DataVector<LVL1::CPMHits>    CPMHitsCollection;
typedef DataVector<LVL1::CMMCPHits>  CMMCPHitsCollection;
typedef DataVector<LVL1::CMMJetHits> CMMJetHitsCollection;
typedef DataVector<LVL1::CMMEtSums>  CMMEtSumsCollection;
typedef DataVector<LVL1::JEMHits>    JEMHitsCollection;
typedef DataVector<LVL1::JEMRoI>     JEMRoICollection;

// Both
typedef DataVector<LVL1::CPMTower>   CPMTowerCollection;
typedef DataVector<LVL1::JetElement> JetElementCollection;
typedef DataVector<LVL1::JEMEtSums>  JEMEtSumsCollection;

// Post-LS1

typedef JepRoiReadByteStreamV2Cnv<LVL1::CMXRoI>        JepRoiReadCRByteStreamV2CnvT;

// Pre-LS1
typedef CpReadByteStreamV1Cnv<CPMHitsCollection>       CpReadCHByteStreamV1CnvT;
typedef CpReadByteStreamV1Cnv<CMMCPHitsCollection>     CpReadCCByteStreamV1CnvT;
typedef JepReadByteStreamV1Cnv<CMMJetHitsCollection>   JepReadCJByteStreamV1CnvT;
typedef JepReadByteStreamV1Cnv<CMMEtSumsCollection>    JepReadCEByteStreamV1CnvT;
typedef JepReadByteStreamV1Cnv<JEMHitsCollection>      JepReadJHByteStreamV1CnvT;
typedef JepRoiReadByteStreamV1Cnv<JEMRoICollection>    JepRoiReadJRByteStreamV1CnvT;
typedef JepRoiReadByteStreamV1Cnv<LVL1::CMMRoI>        JepRoiReadCRByteStreamV1CnvT;

// Both
typedef JepReadByteStreamV1V2Cnv<JetElementCollection> JepReadJEByteStreamV1V2CnvT;
typedef JepReadByteStreamV1V2Cnv<JEMEtSumsCollection>  JepReadESByteStreamV1V2CnvT;


// typedef L1CaloByteStreamAuxCnv<xAOD::CPMTowerContainer, xAOD::CPMTowerAuxContainer>  L1CaloByteStreamxAODCPMTowerCnvT;
// typedef L1CaloByteStreamAuxCnv<xAOD::TriggerTowerContainer, xAOD::TriggerTowerAuxContainer> L1CaloByteStreamxAODTriggerTowerCnvT;

}

// ============================================================================
// RUN2
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepRoiByteStreamV2Cnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepRoiReadCRByteStreamV2CnvT )

// ============================================================================
// RUN1
// ============================================================================

DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpByteStreamV1Cnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpmRoiByteStreamV1Cnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpReadCHByteStreamV1CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpReadCCByteStreamV1CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepByteStreamV1Cnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepRoiByteStreamV1Cnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepReadCJByteStreamV1CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepReadCEByteStreamV1CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepReadJHByteStreamV1CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepRoiReadJRByteStreamV1CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepRoiReadCRByteStreamV1CnvT )

// ============================================================================
// RUN1 & RUN2
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpReadByteStreamV1V2Cnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepReadJEByteStreamV1V2CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JepReadESByteStreamV1V2CnvT )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, RodHeaderByteStreamCnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, L1CaloErrorByteStreamCnv )
// ============================================================================
// PPM
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, PpmByteStreamV1Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, PpmByteStreamReadV1V2Tool )

DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, PpmByteStreamV1Cnv )
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, PpmByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, PpmByteStreamAuxCnv)
// ============================================================================
// CPM
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, CpByteStreamV2Tool )

DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpmTowerByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpmTowerByteStreamAuxCnv)
// ============================================================================
// CMXCPTOB
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxCpTobByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxCpTobByteStreamAuxCnv)
// ============================================================================
// CMXCPHITS
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxCpHitsByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxCpHitsByteStreamAuxCnv)
// ============================================================================
// CMXJETTOB
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxJetTobByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxJetTobByteStreamAuxCnv)
// ============================================================================
// CMXJETHITS
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxJetHitsByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxJetHitsByteStreamAuxCnv)
// ============================================================================
// CMXETSUMS
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxEtSumsByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxEtSumsByteStreamAuxCnv)
// ============================================================================
// JEMETSUMS
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JemEtSumsByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JemEtSumsByteStreamAuxCnv)
// ============================================================================
// CPMTOBROI
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpmTobRoiByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CpmTobRoiByteStreamAuxCnv)
// ============================================================================
// JEMTOBROI
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JemTobRoiByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JemTobRoiByteStreamAuxCnv)
// ============================================================================
// JETELEMENT
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JetElementByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, JetElementByteStreamAuxCnv)
// ============================================================================
// CMXROI
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxRoIByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, CmxRoIByteStreamAuxCnv)
// ============================================================================
// RODHEADER
// ============================================================================
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, RodHeaderByteStreamxAODCnv)
DECLARE_NAMESPACE_CONVERTER_FACTORY( LVL1BS, RodHeaderByteStreamAuxCnv)
// ============================================================================

DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, CpmRoiByteStreamV2Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, JepByteStreamV2Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, JepRoiByteStreamV2Tool )
// Pre-LS1
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, CpByteStreamV1Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, CpmRoiByteStreamV1Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, JepByteStreamV1Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, JepRoiByteStreamV1Tool )

// Both
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, RodHeaderByteStreamTool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, L1CaloErrorByteStreamTool )


DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, TriggerTowerSelectionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, TrigT1CaloDataAccess )
DECLARE_NAMESPACE_TOOL_FACTORY( LVL1BS, TrigT1CaloDataAccessV2 )

DECLARE_FACTORY_ENTRIES( TrigT1CaloByteStream )
{
  //DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepByteStreamV2Cnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepRoiReadCRByteStreamV2CnvT )
  // Pre-LS1
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpByteStreamV1Cnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpmRoiByteStreamV1Cnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpReadCHByteStreamV1CnvT )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpReadCCByteStreamV1CnvT )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepByteStreamV1Cnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepRoiByteStreamV1Cnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepReadCJByteStreamV1CnvT )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepReadCEByteStreamV1CnvT )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepReadJHByteStreamV1CnvT )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepRoiReadJRByteStreamV1CnvT )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepRoiReadCRByteStreamV1CnvT )
  // Both
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpReadByteStreamV1V2Cnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepReadJEByteStreamV1V2CnvT )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JepReadESByteStreamV1V2CnvT )
  // ==========================================================================
  // PPM
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, PpmByteStreamV1Cnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, PpmByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, PpmByteStreamAuxCnv)
  DECLARE_NAMESPACE_TOOL(LVL1BS, PpmByteStreamV1V2ReadTool)
  // ==========================================================================
  // CPM 
  // ==========================================================================
  DECLARE_NAMESPACE_TOOL( LVL1BS, CpByteStreamV2Tool )

  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpmTowerByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpmTowerByteStreamAuxCnv)
  // ==========================================================================
  // CMXCPTOB
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxCpTobByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxCpTobByteStreamAuxCnv)
  // ==========================================================================
  // CMXCPHITS
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxCpHitsByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxCpHitsByteStreamAuxCnv)
  // ==========================================================================
  // CMXJETTOB
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxJetTobByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxJetTobByteStreamAuxCnv)
  // ==========================================================================
  // CMXJETHITS
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxJetHitsByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxJetHitsByteStreamAuxCnv)
  // ==========================================================================
  // CMXETSUMS
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxEtSumsByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxEtSumsByteStreamAuxCnv)
  // ==========================================================================
  // JEMETSUMS
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JemEtSumsByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JemEtSumsByteStreamAuxCnv)
  // ==========================================================================
  // CPMTOBROI
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpmTobRoiByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CpmTobRoiByteStreamAuxCnv)
  // ==========================================================================
  // JEMTOBROI
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JemTobRoiByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JemTobRoiByteStreamAuxCnv)
  // ==========================================================================
  // JETELEMENT
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JetElementByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, JetElementByteStreamAuxCnv)
  // ==========================================================================
  // CMXROI
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxRoIByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, CmxRoIByteStreamAuxCnv)
  // ==========================================================================
  // RODHEADER
  // ==========================================================================
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, RodHeadersByteStreamxAODCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, RodHeaderByteStreamAuxCnv)
  // ==========================================================================
  
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, RodHeaderByteStreamCnv )
  DECLARE_NAMESPACE_CONVERTER( LVL1BS, L1CaloErrorByteStreamCnv )

  // Post-LS1

  DECLARE_NAMESPACE_TOOL( LVL1BS, CpmRoiByteStreamV2Tool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, JepByteStreamV2Tool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, JepRoiByteStreamV2Tool )
  // Pre-LS1
  DECLARE_NAMESPACE_TOOL( LVL1BS, CpByteStreamV1Tool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, CpmRoiByteStreamV1Tool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, JepByteStreamV1Tool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, JepRoiByteStreamV1Tool )
  // Both
  DECLARE_NAMESPACE_TOOL( LVL1BS, RodHeaderByteStreamTool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, L1CaloErrorByteStreamTool )

  // DECLARE_NAMESPACE_TOOL( LVL1BS, PpmByteStreamSubsetTool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, TriggerTowerSelectionTool )
  DECLARE_NAMESPACE_TOOL( LVL1BS, TrigT1CaloDataAccess )
  DECLARE_NAMESPACE_TOOL( LVL1BS, TrigT1CaloDataAccessV2 )
}

