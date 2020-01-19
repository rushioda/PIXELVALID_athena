/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloCellEnergyCorr2Ntuple.h
//

#ifndef CALOCONDPHYSALGS_CALOCELLENERGYCORR2NTUPLE_H
#define CALOCONDPHYSALGS_CALOCELLENERGYCORR2NTUPLE_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloInterface/ICaloMBAverageTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

class AthenaAttributeList;

class CaloCellEnergyCorr2Ntuple : public AthAlgorithm {

  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloCellEnergyCorr2Ntuple(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CaloCellEnergyCorr2Ntuple();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();
    /** standard Athena-Algorithm method */
    StatusCode          stop();
    
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  //=== blob storage
  ITHistSvc* m_thistSvc;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  const CaloCell_ID*       m_calo_id;

  std::string m_key;
  std::string  m_FolderName;
  const DataHandle<AthenaAttributeList> m_AttrListColl;

  int m_Hash;
  int m_OffId;
  float m_eta;
  float m_phi;
  int m_layer;
  int m_detector;
  float m_corr;
  TTree* m_tree;

  int m_runNumber;
  int m_lumiBlock;

};
#endif
