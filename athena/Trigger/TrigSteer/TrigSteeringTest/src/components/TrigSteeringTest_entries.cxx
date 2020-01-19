#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigSteeringTest/dummyAlgo.h"
#include "TrigSteeringTest/dummyAlgoForCalibration.h"
#include "TrigSteeringTest/dummyAlgoHLTErrorCode.h"
#include "TrigSteeringTest/dummyHypo.h"
#include "TrigSteeringTest/newDummyAlgo2To1.h"
#include "TrigSteeringTest/newDummyAlgoAllTEs.h"
#include "TrigSteeringTest/newDummyAlgoScan.h"
#include "TrigSteeringTest/AlgoOverlap.h"
#include "TrigSteeringTest/dummyMonitoredFex.h"
#include "TrigSteeringTest/DummyFexWithAcceptInput.h"
#include "TrigSteeringTest/dummyFexWithRobReqPrep.h"
#include "TrigSteeringTest/DummyFexUsingTopo.h"
#include "TrigSteeringTest/DummyComboUsingTopo.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, dummyAlgo )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, dummyAlgoHLTErrorCode )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, dummyHypo )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, newDummyAlgo2To1 )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, newDummyAlgoAllTEs )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, newDummyAlgoScan )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, AlgoOverlap )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, dummyMonitoredFex )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( PESA, dummyAlgoForCalibration )
DECLARE_ALGORITHM_FACTORY( DummyFexWithAcceptInput )
DECLARE_ALGORITHM_FACTORY( dummyFexWithRobReqPrep )
DECLARE_ALGORITHM_FACTORY( DummyFexUsingTopo )
DECLARE_ALGORITHM_FACTORY( DummyComboUsingTopo )

DECLARE_FACTORY_ENTRIES(TrigSteeringTest) {
    DECLARE_NAMESPACE_ALGORITHM( PESA, dummyAlgo )
    DECLARE_NAMESPACE_ALGORITHM( PESA, dummyAlgoHLTErrorCode )
    DECLARE_NAMESPACE_ALGORITHM( PESA, dummyHypo )
    DECLARE_NAMESPACE_ALGORITHM( PESA, newDummyAlgo2To1 )
    DECLARE_NAMESPACE_ALGORITHM( PESA, newDummyAlgoAllTEs )
    DECLARE_NAMESPACE_ALGORITHM( PESA, newDummyAlgoScan )
    DECLARE_NAMESPACE_ALGORITHM( PESA, AlgoOverlap )
    DECLARE_NAMESPACE_ALGORITHM( PESA, dummyMonitoredFex )
    DECLARE_NAMESPACE_ALGORITHM( PESA, dummyAlgoForCalibration )
    DECLARE_ALGORITHM( DummyFexWithAcceptInput )
    DECLARE_ALGORITHM( dummyFexWithRobReqPrep )
    DECLARE_ALGORITHM( DummyFexUsingTopo )
    DECLARE_ALGORITHM( DummyComboUsingTopo )
}
