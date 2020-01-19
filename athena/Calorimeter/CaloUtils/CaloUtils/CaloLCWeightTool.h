/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOLCWEIGHTTOOL_H
#define CALOUTILS_CALOLCWEIGHTTOOL_H
/**
 * @class CaloLCWeightTool
 * @version \$Id: CaloLCWeightTool.h,v 1.8 2009-01-27 09:09:14 gunal Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 26-August-2009
 * @brief calculates hadronic cell weights based on cluster and cell quantities
 *
 * concrete class implementing a IClusterCellWeightTool to calculate
 * the H1-type cell hadronic weights for cells inside a cluster.  The
 * cluster moments and its energy are also used to derive the weights
 * - therefore the weighting is not called cell-by-cell, but for the
 * entire cluster This tool reads its data from pool containing
 * TProfile2D based weighting data. */

#include "CaloInterface/IClusterCellWeightTool.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ToolHandle.h" 
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

class CaloCell_ID;
class CaloDetDescrManager;
class ICalorimeterNoiseTool;
class StoreGateSvc;
class CaloCluster;

class CaloLCWeightTool : public AthAlgTool, virtual public IClusterCellWeightTool
{
 public:

  virtual ~CaloLCWeightTool();

  StatusCode weight(xAOD::CaloCluster* theCluster);
  StatusCode initialize();

  virtual StatusCode LoadConditionsData(IOVSVC_CALLBACK_ARGS);

  CaloLCWeightTool(const std::string& type, 
		   const std::string& name,
		   const IInterface* parent);
  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

 private:

  /**
   * @brief name of the key for had cell weights */
  std::string m_key;

  /**
   * @brief minimal signal/elec_noise ratio for a cell to be weighted
   *
   * Only cells with |energy| above this value times the RMS of the electronics
   * noise are considered in weighting. */
  double  m_signalOverNoiseCut;

  /**
   * @brief look for em-probability moment and apply relative weight only
   *
   * The classification provides the probability p for the current
   * cluster to be em-like. Hadronic weights are applied with the
   * additional hadronic probablity factor (1-p) to all clusters for
   * the cases EM and HAD. */
  bool  m_useHadProbability;

  /**
   * @brief data object containing the hadronic weights
   *
   * This object contains the actual data used for calibration. */

  const DataHandle<CaloLocalHadCoeff> m_data;

  /**
   * @brief map to indices inside the data for individual samplings
   *
   * Since not all samplings might have weighting coefficients this map
   * holds the index (for valid) or -1 (for invalid) for each sampling. */

  std::vector<int> m_isampmap;

  /**
   * @brief interpolate correction coefficients */
  bool m_interpolate;

  /**
   * @brief update also sampling variables */
  bool m_updateSamplingVars;
  
  /** 
   * @brief vector of names of dimensions in look-up tables to interpolate */
  std::vector<std::string>  m_interpolateDimensionNames;

  /** 
   * @brief actual set of dimension id's to interpolate */
  //std::vector<CaloLocalHadDefs::LocalHadDimensionId> m_interpolateDimensions;
  std::vector<int> m_interpolateDimensions;


  const CaloCell_ID* m_calo_id;
  const CaloDetDescrManager* m_calo_dd_man; 
  
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
};

#endif


