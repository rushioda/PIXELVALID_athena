/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellContainerMergerTool
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  David Rousseau
CREATED:  May 12,2004

PURPOSE:  merge the CaloCellContainer into the main one

********************************************************************/

#include "CaloRec/CaloCellContainerMergerTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellContainerMergerTool::CaloCellContainerMergerTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AthAlgTool(type, name, parent),
   m_caloCellsName("")
{
  declareInterface<ICaloCellMakerTool>(this);
  declareProperty ("CaloCellsName",m_caloCellsName);
  declareProperty ("CaloNums",m_caloNums);
  m_caloNums.clear();
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellContainerMergerTool::initialize()
{
  MsgStream  log(msgSvc(),name());
 
  //check calo number specified
  if (m_caloNums.size()==0) {
    log << MSG::WARNING << " no calo specified. bool hasCalo(caloNum) will not be set!!!  " << endreq;
  }     
  else if  (m_caloNums.size() > static_cast<unsigned int>(CaloCell_ID::NSUBCALO) ) 
    {
      log << MSG::ERROR << " More than " 
	  << m_caloNums.size() 
	  << " calos specified. Must be wrong. Stop." << endreq;
      return StatusCode::FAILURE;
    }
  else 
    {
      for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
	if (m_caloNums[index]<0 || m_caloNums[index]>=static_cast<int>(CaloCell_ID::NSUBCALO) ) {
	  log << MSG::ERROR << "Invalid calo specification:" 
	      << m_caloNums[index] << "Stop." << endreq ;
	  return StatusCode::FAILURE;	
	}	
      }
    }


  return StatusCode::SUCCESS;

}

StatusCode CaloCellContainerMergerTool::process(CaloCellContainer * theCont )
{
  MsgStream  log(msgSvc(),name());

  StatusCode sc ;

  const CaloCellContainer * theCellContainer ; 
  sc = evtStore()->retrieve(theCellContainer,m_caloCellsName);



  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR 
	<< "Cannot retrieve CaloCellContainer "
	<< m_caloCellsName
	<< ". Ignore."
	<< endreq;
    return StatusCode::FAILURE;
  } 

  CaloCellContainer::const_iterator itrCell=theCellContainer->begin();
  CaloCellContainer::const_iterator endCell=theCellContainer->end();
  for (;itrCell!=endCell;++itrCell){
      
    //copy cell with the correct concrete type (LAr and Tile)
    CaloCell * theCaloCell=(*itrCell)->clone();

    //copy each cell with the correct concrete type
    theCont->push_back(theCaloCell);
  }

  //set hasCalo(caloNum) bools
  for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
    CaloCell_ID::SUBCALO caloNum=static_cast<CaloCell_ID::SUBCALO>(m_caloNums[index]);
  
    //specify that a given calorimeter has been filled
    if (theCellContainer->hasCalo(caloNum) )
      {
	log << MSG::WARNING << "CaloCellContainer has already been filled with calo " 
	    << caloNum << endreq ;    
      }
      
    theCont->setHasCalo(caloNum);
  }


  return StatusCode::SUCCESS ;
}








