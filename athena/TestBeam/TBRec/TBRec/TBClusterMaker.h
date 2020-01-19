/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBCLUSTERMAKER_H
#define TBCLUSTERMAKER_H
/**
 * @class TBClusterMaker
 * @version \$Id: TBClusterMaker.h,v 1.2 2009-04-17 09:51:30 pavol Exp $
 * @author Andrey Minaenko <minaenko@ihep.ru>
 * @date 01-September-2005
 * @brief simple cone cluster maker for TB data analysis.
 *
 * Make clusters using simple cone algorithm with the sampling dependent 
 * cone size
 * Like all other cluster maker tools this class derives from
 * CaloClusterMakerTool.  */

class StoreGateSvc; 
class CaloDetDescrManager; 
class CaloDetDescrElement;
class ICalorimeterNoiseTool;

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloGeoHelpers/CaloSampling.h"

#include <map>
#include <vector>
#include <string>

class TBClusterMaker: public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:    
  
  TBClusterMaker(const std::string& type, const std::string& name,
		       const IInterface* parent);
  
  StatusCode execute(xAOD::CaloClusterContainer* theClusters);
  StatusCode initialize();
  StatusCode finalize();
  
 private: 
  
  /**
   * @brief Vectors containing the list of used samplings and 
   * corresponding cone cuts; list of used calorimeters **/
  std::vector<CaloSampling::CaloSample> m_samplings;
  std::vector<float> m_coneCuts;
  std::vector<CaloCell_ID::SUBCALO> m_calos;

  /**
   * @brief Map of cone cuts for calorimeter samplings **/
  std::map<CaloSampling::CaloSample, float> m_samplingConeCuts;

  /**
   * @brief Threshold cut on cell energy in sigma noise units **/
  float m_cellCut;

  /**
   * @brief Threshold cut on seed cell energy in sigma noise units to find 
   the 1st approximation of cluster \f$ \eta, \phi \f$ **/
  float m_seedCut;

  /**
   * @brief Maximal claster position shift at the current step to stop 
   iterations **/
  float m_deltaR;

  /**
   * @brief Maximal number of iterations to find cluster position **/
  int m_maxIter;

  /**
   * @brief Flag to fix cluster position ( \f$ \eta,\phi \f$) from JO file **/
  bool m_fixClusterPosition;

  /**
   * @brief Cluster \f$ \eta,\phi \f$) set in JO file **/
  float m_eta0;
  float m_phi0;

  /** True if cell enrgy is in ADC counts, default = FALSE **/
  bool m_CellEnergyInADC;

  /** Counters */
  int m_numSeedCellNotFound;
  int m_numCluIterationsConverged;
  int m_numCluIterationsNonConverged;

  /** Services */
  StoreGateSvc* m_eventStore;
  StoreGateSvc* m_detectorStore;
  const CaloDetDescrManager* m_calo_DDM;
  const CaloCell_ID* m_calo_id;
  IToolSvc* m_toolSvc;
  ICalorimeterNoiseTool* m_noiseTool;
  
  /** Names */
  std::string m_caloCellContainerName;
  std::string m_clusterContainerName;
  std::string m_noiseToolName; 
  std::vector<std::string> m_samplingNames;

  std::map<std::string, CaloSampling::CaloSample> m_samplingFromNameLookup;
  std::map<CaloSampling::CaloSample, CaloCell_ID::SUBCALO> m_caloLookup;
  std::map<CaloSampling::CaloSample, float> m_adcToMeV;

  /** Setup lookup tables */
  StatusCode setupLookupTables();
};

#endif // TBCLUSTERMAKER_H
