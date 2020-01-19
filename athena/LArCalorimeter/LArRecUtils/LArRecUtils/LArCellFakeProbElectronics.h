/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_LARCELLFAKEPROBELECTRONICS_H
#define CALOREC_LARCELLFAKEPROBELECTRONICS_H

/*! \class LArCellFakeProbElectronics
 *
 *  brief Scales down the energy of cells due to simulated 
 *  failure of the readout
 *
 *  \author Kai Voss <kai.voss@cern.ch>
 *
 *  \date   February 25, 2005
 */

#include "CaloInterface/ICellWeightTool.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "LArIdentifier/LArHVCellID.h" 
//#include "LArCabling/LArHVCablingTool.h" 
#include "Identifier/HWIdentifier.h"

class StoreGateSvc; 
class LArCablingService;
class LArOnlineID;

class LArCellFakeProbElectronics :  public AthAlgTool, virtual public ICellWeightTool

{
 
 public:    
  
  LArCellFakeProbElectronics(const std::string& type, 
		    const std::string& name, 
		    const IInterface* parent) ;
  
  /*! Constructor */
  virtual StatusCode initialize() ; 
  
  /*! implementation of ICellWeightTool weighting method */ 
  virtual double wtCell(const CaloCell * theCell) ;


  
 private:

  /*! process jobOptions and add affected cells to list */
  StatusCode add_cell(int iBarrel,int iSide,int iFT,int iSlot,int iChannel, double weight);

  /*! pointer to CaloIDManager */
  const CaloCell_ID*    m_idHelper;

  /*! pointer to LArCablingService */
  LArCablingService* m_cablingService;

  /*! pointer to LAr Online ID helper */
  const LArOnlineID* m_onlineHelper;  

  /* !list of dead channels with weight */
  std::map<HWIdentifier, double> m_deadChannels;
  
  /* !poperty  which contains list of dead channels */
  std::vector<std::string> m_inputStringIDs;

  /*! Process properties set by jobOptions  */ 
  StatusCode read_problems(); 

  std::string m_contInputName;
};

#endif

