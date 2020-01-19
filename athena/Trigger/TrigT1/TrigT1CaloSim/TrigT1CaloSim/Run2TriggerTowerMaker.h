// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// Run2TriggerTowerMaker class description
// ================================================
//
// THIS TEXT TO BE REPLACED BY ATLAS STANDARD FORMAT
//
//
// class: Run2TriggerTowerMaker
//
// Description:
//
//The Run2TriggerTowerMaker class takes calorimeter cells from the TES and
// forms Trigger Towers, which it then places back into the TES  The calorimeter
// cells can be produced either by GEANT or other fast simulation packages
// - this is defined by setting a parameter
// CellType to 1 for CaloCells, 2 to reprocess TriggerTowers and 3 for LAr/Tile TTL1 input (a simulation of analogue towers);
//
// ................................................................
//

#ifndef TRIGT1CALOSIM_RUN2TRIGGERTOWERMAKER_H
#define TRIGT1CALOSIM_RUN2TRIGGERTOWERMAKER_H

// STL
#include <array>
#include <map>
#include <string>
#include <utility> // for std::pair, std::make_pair
#include <vector>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"

//Calorimeter tower includes
#include "LArRawEvent/LArTTL1Container.h"
#include "TileEvent/TileTTL1Container.h"

//LVL1 Calo trigger includes
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"
#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaults.h"

// EDM include(s)
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

// forward decl(s)
class IAtRndmGenSvc;
class ILumiBlockMuTool;
class CaloLVL1_ID;
class CaloTriggerTowerService;
class L1CaloCondSvc;
class L1CaloPprChanCalibContainer;
class L1CaloPprChanDefaults;
class L1CaloDisabledTowersContainer;
class L1CaloPpmDeadChannelsContainer;

namespace CLHEP { class HepRandomEngine; }
namespace TrigConf { class ILVL1ConfigSvc; }

namespace LVL1
{
class IL1TriggerTowerTool;
class IL1CaloMappingTool;
class InternalTriggerTower;
class TriggerTower;

class Run2TriggerTowerMaker : public AthAlgorithm, virtual public IIncidentListener
{
public:
  //-------------------------
  // Constructors/Destructors
  //-------------------------
  Run2TriggerTowerMaker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Run2TriggerTowerMaker();

  // These are disallowed
  Run2TriggerTowerMaker(const Run2TriggerTowerMaker&) = delete;
  Run2TriggerTowerMaker& operator=(const Run2TriggerTowerMaker&) = delete;

  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void handle(const Incident&);

private:
  // constants
  constexpr static int s_FIRLENGTH = 5; // number of FIR coefficients
  constexpr static int s_MEV = 1000; // scale for converting ET -> counts
  constexpr static unsigned int m_maxIetaBins = 51;   // max numbers of eta regions if we choose a granularity of 0.1 in eta

  // enums  -- SIZE must be the last entry in each enum
  enum CellTypes{CELL=1, TRIGGERTOWERS=2, TTL1=3, SIZE=4};

  // properties
  // user-defined names for random number engines - keep streams distinct
  std::string m_digiEngine;

  // Input for this Algorithm, meaning in enum CellTypes 
  int m_cellType;

  // pedestal uncertainty and ADC noise
  double m_adcVar;

  // digitisation, FIR, LUT calibration parameters
  double m_adcStep;
  double m_gainCorr;
  bool m_decorateFIR;

  //Flag to enable zero suppression (makes algorithm simulation much faster
  bool m_ZeroSuppress;

  // locations within the TES to store collections of Trigger Towers
  std::string m_outputLocation;

  //  location of input TriggerTowers (for reprocessing)
  std::string m_inputTTLocation;

  // location of LAr TTL1 data
  std::string m_EmTTL1ContainerName;
  std::string m_HadTTL1ContainerName;
  std::string m_TileTTL1ContainerName;

  // require all calorimeters, or allow running with some missing?
  bool m_requireAllCalos;

  // database keys for the variousfolders
  std::string m_chanCalibKey;
  std::string m_chanDefaultsKey;
  std::string m_disabledTowersKey;
  std::string m_deadChannelsKey;

  // Tools/Services
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
  ServiceHandle<IAtRndmGenSvc> m_rndGenSvc;
  ServiceHandle<L1CaloCondSvc> m_condSvc;
  CLHEP::HepRandomEngine* m_rndmADCs; // non owning ptr

  ToolHandle<IL1TriggerTowerTool> m_TTtool;
  ToolHandle<IL1CaloMappingTool> m_mappingTool;
  ToolHandle<ILumiBlockMuTool> m_lumiBlockMuTool;

  const CaloLVL1_ID* m_caloId; //non-owning ptr

  // Global calibration scale (MeV/count, to optimise performance)
  double m_digitScale;
  // Global LUT scales
  double m_cpLutScale;
  double m_jepLutScale;
  // conversion factor from tile amplitude to MeV
  double m_TileToMeV;
  // TileTTL1 may come with a pedestal - need to subtract if so
  double m_TileTTL1Ped;

  // flag whether we reprocess data
  // if yes, special treatment of the pedestal correction is necessary
  bool m_isDataReprocessing;

  // non-owning pointers
  L1CaloPprChanCalibContainer* m_chanCalibContainer = nullptr;
  L1CaloDisabledTowersContainer* m_disabledTowersContainer = nullptr;
  L1CaloPpmDeadChannelsContainer* m_deadChannelsContainer = nullptr;
  L1CaloPprChanDefaults m_chanDefaults;

  std::unique_ptr<xAOD::TriggerTowerContainer> m_xaodTowers;
  std::unique_ptr<xAOD::TriggerTowerAuxContainer> m_xaodTowersAux;
  std::vector<std::vector<double>> m_xaodTowersAmps; // stores the Amps (vector<double>) for each xaodTower
  std::size_t m_curIndex = 0u;

  /**
     instead of calculating the expression:
     double theta =2.*atan(exp(-fabs(cell_eta)));
     cell_energy=sin(theta)*cell_energy;
     for each em and had TT again, we will use a hash table
  */
  std::array<double, m_maxIetaBins> m_sinThetaHash;

  /** Compute L1CaloCoolChannelId (including support for old geometries) */
  L1CaloCoolChannelId channelId(double eta, double phi, int layer);


  /** gets collection of input TriggerTowers for reprocessing */
  StatusCode getTriggerTowers();
  /** fetch Calorimeter Towers */
  StatusCode getCaloTowers();

  /** Convert analogue pulses to digits */
  void digitize();

  /** Simulate PreProcessing on analogue amplitudes */
  StatusCode preProcess();

  /** Stores Trigger Towers in the TES, at a
      location defined in m_outputLocation.<p>
      Returns FAILURE if the towers could not be saved. **/
  StatusCode store();

  /** extract amplitudes from TTL1 */
  void processLArTowers(const LArTTL1Container * );
  void processTileTowers(const TileTTL1Container *);

  /** functions to extract eta, phi coordinates from calo tower identifiers **/
  double IDeta(const Identifier& id, const CaloLVL1_ID* caloId);
  double IDphi(const Identifier& id, const CaloLVL1_ID* caloId);

  /** Functions to simulate processing of tower signals **/
  std::vector<int> ADC(L1CaloCoolChannelId channel, const std::vector<double>& amps) const;
  int EtRange(int et, unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh) const;

  // void preProcessLayer(int layer, int eventBCID, InternalTriggerTower* tower, std::vector<int>& etResultVector, std::vector<int>& bcidResultVector);
  StatusCode preProcessTower(xAOD::TriggerTower* tower, int eventBCID);

  int etaToElement(float feta, int layer) const;
};

} // namespace LVL1
#endif
