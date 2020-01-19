// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloRec/CaloConstCellMaker.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Create a @c CaloConstCellContainer.
 */


#ifndef CALOREC_CALOCONSTCELLMAKER_H
#define CALOREC_CALOCONSTCELLMAKER_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class IChronoStatSvc;
class ICaloConstCellMakerTool;


/**
 * @brief Create a @c CaloConstCellContainer.
 *
 * This is similar to @c CaloCellMaker, except that it works on const
 * cell pointers.  This algorithm is thus appropriate for copying
 * cell pointers between containers.
 */
class CaloConstCellMaker
  : public AthAlgorithm
{
public:
  /**
   * @brief Constructor.
   * @param name Algorithm name.
   * @param pSvcLocator Gaudi service locator.
   */
  CaloConstCellMaker(const std::string& name, ISvcLocator* pSvcLocator);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;

  /// Standard Gaudi execute method.
  virtual StatusCode execute() override;


private:
  /// Property: Will the new CellContainer will own its cells (default no)?
  int m_ownPolicy;

  /// Property: List of tools to run.
  ToolHandleArray<ICaloConstCellMakerTool> m_caloCellMakerTools;
  
  /// Property: Output container name.
  std::string m_caloCellsOutputName;

  /// For timekeeping.
  ServiceHandle<IChronoStatSvc> m_chrono;

  /// Count events processed, for statistics.
  unsigned m_evCounter;
};


#endif // CALOREC_CALOCONSTCELLMAKER_H
