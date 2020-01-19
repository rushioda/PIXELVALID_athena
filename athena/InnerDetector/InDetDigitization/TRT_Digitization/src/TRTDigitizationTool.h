/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DIGITIZATION_TRTDIGITIZATIONTOOL_H
#define TRT_DIGITIZATION_TRTDIGITIZATIONTOOL_H
/** @file TRTDigitizationTool.h
 * @brief a sample implementation of IPileUpTool to test the framework
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */
#include <vector>
#include <list>
#include <utility> /* pair */

#include "xAODEventInfo/EventInfo.h"  /*SubEvent*/
#include "PileUpTools/PileUpToolBase.h"

#include "TRTDigit.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRawData/TRT_RDO_Container.h"
//#include "CommissionEvent/ComTime.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "HepPDT/ParticleDataTable.hh"

#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

#include "CLHEP/Random/RandomEngine.h"

class PileUpMergeSvc;
class ITRT_PAITool;
class ITRT_SimDriftTimeTool;
class IAtRndmGenSvc;
class TRT_ID;
class TRTProcessingOfStraw;
class TRTElectronicsProcessing;
class TRTDigCondBase;
class TRTNoise;
class ITRT_StrawNeighbourSvc;

#include "HitManagement/TimedHitCollection.h"
class TRTUncompressedHit;
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
//class TRTUncompressedHitCollection;
namespace InDetDD {
  class TRT_DetectorManager;
}

class TRTDigSettings;

static const InterfaceID IID_ITRTDigitizationTool ("TRTDigitizationTool",1,0);

class TRTDigitizationTool : virtual public IPileUpTool, public PileUpToolBase {
public:
  static const InterfaceID& interfaceID();
  TRTDigitizationTool( const std::string& type, const std::string& name, const IInterface* parent );

  /** Destructor */
  ~TRTDigitizationTool();

  ///called at the end of the subevts loop. Not (necessarily) able to access SubEvents
  StatusCode mergeEvent();
  ///called for each active bunch-crossing to process current SubEvents bunchXing is in ns
  StatusCode processBunchXing( int bunchXing,
			       SubEventIterator bSubEvents,
			       SubEventIterator eSubEvents );
  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;

  StatusCode prepareEvent( const unsigned int /*nInputEvents*/ );

  /** Initialize */
  virtual StatusCode initialize();

  /**
   * Perform digitization:
   * - Get G4 hits
   * - Process hits straw by straw [call @c ProcessStraw() ]
   * - Add noise
   * - Create RDO collection
   */
  virtual StatusCode processAllSubEvents();

  /** Finalize */
  StatusCode finalize();

private:

  CLHEP::HepRandomEngine * m_pHRengine;

  Identifier getIdentifier( int hitID,
			    IdentifierHash& hashId,
			    Identifier& layerID,
			    bool& statusok ) const;

  StatusCode update( IOVSVC_CALLBACK_ARGS );        // Update of database entries.
  StatusCode ConditionsDependingInitialization();

  StatusCode lateInitialize();
  StatusCode processStraws(std::set<int>& sim_hitids, std::set<Identifier>& simhitsIdentifiers);
  StatusCode createAndStoreRDOs();

  // The straw's gas mix: 1=Xe, 2=Kr, 3=Ar
  int StrawGasType(Identifier& TRT_Identifier) const;

  bool particleFlagQueryBit(int bitposition, unsigned short particleFlag) const;
  unsigned int getRegion(int hitID);
  double getCosmicEventPhase();

  std::vector<std::pair<unsigned int, int> > m_seen;
  std::vector<TRTDigit> m_vDigits; /**< Vector of all digits */
  std::string m_dataObjectName; /**< Name of the hits collections */
  std::string m_outputRDOCollName; /**< name of the output RDOs. */
  std::string m_outputSDOCollName; /**< name of the output SDOs. */
  bool m_printOverrideableSettings;
  bool m_printUsedDigSettings;
  ToolHandle<ITRT_PAITool> m_TRTpaiToolXe;
  ToolHandle<ITRT_PAITool> m_TRTpaiToolAr;
  ToolHandle<ITRT_PAITool> m_TRTpaiToolKr;
  ToolHandle<ITRT_SimDriftTimeTool> m_TRTsimdrifttimetool;
  ServiceHandle<PileUpMergeSvc> m_mergeSvc;      /**< PileUp Merge service */

  TRTElectronicsProcessing * m_pElectronicsProcessing;
  TRTProcessingOfStraw* m_pProcessingOfStraw;
  TRTDigCondBase* m_pDigConditions;
  TRTNoise* m_pNoise;
  TRT_RDO_Container* m_container;   /**< RDO container */

  //unsigned int m_timer_eventcount;
  ServiceHandle <IAtRndmGenSvc> m_atRndmGenSvc;  /**< Random number service */
  ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;

  const InDetDD::TRT_DetectorManager* m_manager;
  TRTDigSettings* m_settings;
  const TRT_ID* m_trt_id;       /**< TRT Id Helper */
  std::list<TRTUncompressedHitCollection*> m_trtHitCollList;
  TimedHitCollection<TRTUncompressedHit>* m_thpctrt;
  bool m_alreadyPrintedPDGcodeWarning;
  double m_minCrossingTimeSDO;
  double m_maxCrossingTimeSDO;
  double m_minpileuptruthEkin;

  // const  ComTime* m_ComTime;

  const HepPDT::ParticleDataTable* m_particleTable;
  int m_dig_vers_from_condDB;
  std::string m_digverscontainerkey;
  bool m_first_event;
  bool m_condDBdigverfoldersexists;

  int m_HardScatterSplittingMode;
  bool m_HardScatterSplittingSkipper;

  int m_UseGasMix;
  double m_cosmicEventPhase;     // local replacement for the comTime service
  unsigned short m_particleFlag; // 16-bit flag indicating the presence of up to 16 types of particle hitting the straw.
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_sumSvc; // added by Sasha for Argon
  IntegerProperty                                    m_vetoThisBarcode;

};

inline const InterfaceID& TRTDigitizationTool::interfaceID()
{
  return IID_ITRTDigitizationTool;
}

#endif
