// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloD3PDMaker/src/CellFillerTool.h
 * @author Francesco Lanni 
 * @date  2012
 * @brief Block filler tool for makeing hits in SuperCell, and fill ntuple.
 */

#ifndef CALOD3PDMAKER_LARSCHITFILLERTOOL_H
#define CALOD3PDMAKER_LARSCHITFILLERTOOL_H

// Gaudi/Athena include(s):
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArElecCalib/ILArfSampl.h"
#include <vector>
#include <list>
#include <map>

// Forward declaration(s):
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;
class ICaloNoiseTool;
class LArOnlineID;
class LArCablingService;
class CaloDetDescrManager ; 
class Identifier;

class CaloSuperCellDetDescrManager;
class ICaloSuperCellIDTool;

class CaloCell_SuperCell_ID; 

#include "LArSimEvent/LArHit.h"

namespace D3PD {


/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class LArSCHitFillerTool
  : public BlockFillerTool<LArHitContainer>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LArSCHitFillerTool (const std::string& type,
		const std::string& name,
		const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode initialize();
  virtual StatusCode book();
  //  virtual StatusCode  handle(const Incident&); 
  virtual StatusCode fill (const LArHitContainer& p);

private:
 
  const LArEM_ID   *m_emid;
  const LArFCAL_ID *m_fcalid;
  const LArHEC_ID  *m_hecid;
  const TileID     *m_tileid;    
  const LArOnlineID* m_onlineHelper;
  ToolHandle<LArCablingService> m_larCablingSvc;
  const DataHandle<ILArfSampl>    m_dd_fSampl;

  /// parameters
  int *m_nSC;
  std::vector<double> *m_E;
  std::vector<double> *m_eta;
  std::vector<double> *m_phi;
  std::vector<double> *m_Eoff;
  std::vector<double> *m_Et;
  std::vector<double> *m_fsampl;

  std::vector<int> *m_calo;
  std::vector<int> *m_region;
  std::vector<int> *m_sampling;
  std::vector<int> *m_ieta;
  std::vector<int> *m_jphi;
  std::vector<unsigned int> *m_offlId;

  std::vector<float> m_etaCut;
  std::vector<float> m_phiCut;
  std::vector< unsigned int > m_caloNums; 
  std::vector< unsigned int >  m_caloLayers;

  bool m_caloEtaSelection;
  bool m_caloPhiSelection;
  bool m_caloLayerSelection;
  bool m_caloSelection;


  /// Property: Offline / supercell mapping tool.
  ToolHandle<ICaloSuperCellIDTool>     m_scidtool;

  /// Geometry manager.
  const CaloSuperCellDetDescrManager* m_sc_dd_mgr;
  /// idHlper 
  const CaloCell_SuperCell_ID* m_sc_idHelper ; 

};


} // namespace D3PD


#endif // not CALOD3PDMAKER_LARSCHITFILLERTOOL_H
