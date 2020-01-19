/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTTRT_DIGITIZATION_FASTTRT_DIGITIZATIONTOOL_H
#define FASTTRT_DIGITIZATION_FASTTRT_DIGITIZATIONTOOL_H
/** @file TRTFastDigitizationTool.h
 * @brief a sample implementation of IPileUpTool to test the framework
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */

#include "EventInfo/PileUpEventInfo.h"
#include "PileUpTools/PileUpToolBase.h"
#include "FastTRT_Digitization/ITRTFastDigitizationTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"

#include "GaudiKernel/RndmGenerators.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "TrkParameters/TrackParameters.h"

#include "Identifier/Identifier.h"
#include "CLHEP/Random/RandGauss.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "StoreGate/WriteHandle.h"

#include <utility>
#include <vector>
#include <map>
#include <cmath>

class PileUpMergeSvc;
class IAtRndmGenSvc;
class TRT_ID;
class TRTUncompressedHit;

namespace InDetDD {
  class TRT_DetectorManager;
}

class TRT_ID;
class StoreGateSvc;
class ITRT_DriftFunctionTool;


class TRTFastDigitizationTool : public PileUpToolBase, virtual public ITRTFastDigitizationTool {
public:
  TRTFastDigitizationTool( const std::string &type, const std::string &name, const IInterface *parent );

  ///called at the end of the subevts loop. Not (necessarily) able to access
  ///SubEvents
  StatusCode mergeEvent();

  ///called for each active bunch-crossing to process current SubEvents
  /// bunchXing is in ns
  StatusCode processBunchXing( int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents );

  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  StatusCode prepareEvent( const unsigned int /*nInputEvents*/ );

  ///alternative interface which uses the PileUpMergeSvc to obtain all
  ///the required SubEvents.
  StatusCode processAllSubEvents();

  /** Initialize */
  virtual StatusCode initialize();

  /** Finalize */
  StatusCode finalize();

private:

  StatusCode produceDriftCircles();
  StatusCode createOutputContainers();

  Identifier getIdentifier( int hitID, IdentifierHash &hash, Identifier &layer_id, bool &status ) const;

  StatusCode createAndStoreRIOs();

  static double getDriftRadiusFromXYZ( const TimedHitPtr< TRTUncompressedHit > &hit );
  HepGeom::Point3D< double > getGlobalPosition( const TimedHitPtr< TRTUncompressedHit > &hit );
  bool isArgonStraw( const Identifier &straw_id ) const;
  int gasType( const Identifier &straw_id ) const;
  double getProbHT( int particleEncoding, float kineticEnergy, const Identifier &straw_id, double driftRadiusLoc, double hitGlobalPosition ) const;
  static double HTProbabilityElectron_high_pt( double eta );
  static double HTProbabilityElectron_low_pt( double eta );
  static double HTProbabilityMuon_5_20( double eta );
  static double HTProbabilityMuon_60( double eta );
  static double strawEfficiency( double driftRadius, int BEC = 0 );
  static double correctionHT( double momentum, Trk::ParticleHypothesis hypothesis );
  double particleMass( int i ) const;

  // Tools and Services
  ToolHandle< ITRT_DriftFunctionTool > m_trtDriftFunctionTool;
  bool m_useTrtElectronPidTool;                                           // false: use Tina's parametrization
  ToolHandle< Trk::ITRT_ElectronPidTool > m_trtElectronPidTool;
  ServiceHandle< ITRT_StrawStatusSummarySvc > m_trtStrawStatusSummarySvc; // Argon / Xenon
  ServiceHandle< PileUpMergeSvc > m_mergeSvc;                             // PileUp Merge service
  ServiceHandle< IAtRndmGenSvc > m_atRndmGenSvc;                          // Random number service
  CLHEP::HepRandomEngine *m_randomEngine;
  std::string m_randomEngineName;                                         // Name of the random number stream

  // INPUT
  std::string m_trtHitCollectionKey;
  std::list< TRTUncompressedHitCollection * > m_trtHitCollList;

  // OUTPUT 
  SG::WriteHandle< InDet::TRT_DriftCircleContainer > m_trtDriftCircleContainer;
  SG::WriteHandle< PRD_MultiTruthCollection > m_trtPrdTruth;

  TimedHitCollection< TRTUncompressedHit > *m_thpctrt;
  std::multimap< Identifier, InDet::TRT_DriftCircle * > m_driftCircleMap;

  // Helpers
  const InDetDD::TRT_DetectorManager *m_trt_manager;
  const TRT_ID *m_trt_id;                                                 // TRT Id Helper

  // Split configuration
  int m_HardScatterSplittingMode;                                         // Process all TRT_Hits or just those from signal or background events
  bool m_HardScatterSplittingSkipper;
  IntegerProperty m_vetoThisBarcode;
};

#endif // FASTTRT_DIGITIZATION_FASTTRT_DIGITIZATIONTOOL_H
