/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityEMFractionTool.cxx          #
//# Package: TBRec                                    #
//#                                                   #
//# Author : Marco Bieri <mbieri@sfu.ca>              #
//#                                                   #
//#                                                   #
//#####################################################

#include "TBRec/TBBeamQualityEMFractionTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/AlgTool.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h" 

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"

#include "Identifier/Identifier.h"
#include <list>

TBBeamQualityEMFractionTool::TBBeamQualityEMFractionTool(const std::string& name,
							 const std::string& type,
							 const IInterface* parent)
  : TBBeamQualityTool(name,type,parent),
    m_StoreGate(nullptr),
    m_detStore(nullptr),
    m_caloMgr(nullptr),
    m_emecID_help(nullptr),
    m_hecID_help(nullptr),
    m_fcalID_help(nullptr),
    m_em_frac(0)
{  
  declareInterface<TBBeamQualityTool>(this);
  declareProperty("EM_fraccut",m_em_fraccut=0.2);
  declareProperty("EM_fracnumsamp",m_em_fracnumsamp);
  declareProperty("EM_fracdensamp",m_em_fracdensamp);
  declareProperty("EM_fracnumdet",m_em_fracnumdet="none");
  declareProperty("EM_fracdendet",m_em_fracdendet="none");
}

TBBeamQualityEMFractionTool::~TBBeamQualityEMFractionTool()
{ }

StatusCode TBBeamQualityEMFractionTool::initializeTool()
{ //init
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG
      << "initialize"
      << endreq;
  
  StatusCode sc;
  
  // accessing Store Gate
  sc = service( "StoreGateSvc", m_StoreGate);
  if( sc.isFailure() ){
    log << MSG::FATAL << "Unable to locate the StoreGateSvc Service" <<endreq;
    return sc;
  }
  
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure())
    {
      log << MSG::FATAL << " Cannot locate DetectorStore " << std::endl;
      return StatusCode::FAILURE;
    }
  
  m_hecID_help = NULL;
  m_emecID_help = NULL;
  m_fcalID_help = NULL;
  
  /*
  // retrieve detector description manager for LAr subsystem
  const DataHandle<LArDetDescrManager> m_larMgr;
  sc = m_detStore->retrieve(m_larMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR << "unable to retrieve LArDetDescrManager from detector store"<< endreq;
    return sc;
  }
  
  //obtainine Identifier helpers
  m_hecID_help = m_larMgr->get_hec_id();
  if (!m_hecID_help) {
    log << MSG::ERROR << "unable to obtain hec id " << endreq;
    return StatusCode::FAILURE;
  }
  m_emecID_help = m_larMgr->get_em_id();
  if (!m_emecID_help) {
    log << MSG::ERROR << "unable to obtain emec id " << endreq;
    return StatusCode::FAILURE;
  }
  
  m_fcalID_help = m_larMgr->get_fcal_id();
  if (!m_fcalID_help) {
    log << MSG::ERROR << "unable to obtain fcal id " << endreq;
    return StatusCode::FAILURE;
  }
  */
  
  
  // retrieve detector description manager for LAr subsystem
  const DataHandle<CaloIdManager> m_caloMgr;
  sc = m_detStore->retrieve(m_caloMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR << "unable to retrieve CaloIdManager from detector store"<< endreq;
    return sc;
  }
  
  //obtainine Identifier helpers
  m_hecID_help = m_caloMgr->getHEC_ID();
  if (!m_hecID_help) {
    log << MSG::ERROR << "unable to obtain hec id " << endreq;
    return StatusCode::FAILURE;
  }
  m_emecID_help = m_caloMgr->getEM_ID();
  if (!m_emecID_help) {
    log << MSG::ERROR << "unable to obtain emec id " << endreq;
    return StatusCode::FAILURE;
  }
  
  m_fcalID_help = m_caloMgr->getFCAL_ID();
  if (!m_fcalID_help) {
    log << MSG::ERROR << "unable to obtain fcal id " << endreq;
    return StatusCode::FAILURE;
  }
  
  
  
  log<<MSG::DEBUG<<"Got to before p_toolSvc"<<endreq;
  
  if (m_em_fracnumdet=="HEC") {
    m_em_fracnumsamp.push_back(8);
    m_em_fracnumsamp.push_back(9);
    m_em_fracnumsamp.push_back(10);
  }
  if (m_em_fracdendet=="HEC") {
    m_em_fracdensamp.push_back(8);
    m_em_fracdensamp.push_back(9);
    m_em_fracdensamp.push_back(10);
  }
  if (m_em_fracnumdet=="EMEC") {
    m_em_fracnumsamp.push_back(6);
    m_em_fracnumsamp.push_back(7);
  }
  if (m_em_fracdendet=="EMEC") {
    m_em_fracdensamp.push_back(6);
    m_em_fracdensamp.push_back(7);
  }
  if (m_em_fracnumdet=="FCAL") {
    m_em_fracnumsamp.push_back(21);
    m_em_fracnumsamp.push_back(22);
    m_em_fracnumsamp.push_back(23);
  }
  if (m_em_fracdendet=="FCAL") {
    m_em_fracdensamp.push_back(21);
    m_em_fracdensamp.push_back(22);
    m_em_fracdensamp.push_back(23);
  }
  
  return StatusCode::SUCCESS;
}//init

StatusCode TBBeamQualityEMFractionTool::accept(std::vector<std::string> m_particles)
{//accept
  
  MsgStream log(msgSvc(),name());
  
  log << MSG::DEBUG
      << "TBBeamQualityEMFractionTool: Started Accept"
      << endreq;
  
  StatusCode sc;
  
  log << MSG::DEBUG << "m_em_fracnumdet: "<<m_em_fracnumdet<<endreq;
  log << MSG::DEBUG << "m_em_fracdendet: "<<m_em_fracdendet<<endreq;
  log << MSG::DEBUG << " called for:  "<<m_particles.size()<<" particles"<<endreq;
  
  // setting up layers (h6)
  
  // setting tmp variables
  // resetting the numerator and denominato to 0
  
  //long chan =0;
  float numenergy=0;
  float denenergy=0;
  for (int i=0;i<500;++i) {
    m_Mlayer[i]=0;
  }  
  
  //Accesing the CaloCellContainer
  const CaloCellContainer* cellContainer;
  
  sc = m_StoreGate->retrieve(cellContainer);
  if (sc.isFailure()) {
    log << MSG::ERROR << "couldn't get the calo cells from storegate" << endreq;
    log << MSG::ERROR << "here is what is in storegate: " << m_StoreGate->dump() << endreq;
    return StatusCode::FAILURE;
  }
  
  CaloCellContainer::const_iterator ifirst = cellContainer->begin();
  CaloCellContainer::const_iterator ilast = cellContainer->end();
  
  for (; ifirst!=ilast; ifirst++){//iterating
    
    //obtaining cell pointer
    const CaloCell* cell_ptr = *ifirst;
    
    //obtaining dde pointer
    const CaloDetDescrElement* caloDDE_ptr = cell_ptr->caloDDE();
    
    //obtaining SubCalo
    //const CaloCell_ID::SUBCALO subcalo = caloDDE_ptr->getSubCalo();
    
    //obtaining sampling (layer)
    const CaloCell_ID::CaloSample sampling = caloDDE_ptr->getSampling();
    
    // This map will have the layers as an index and increment the energy
    // Does this for ALL layers ( all detectors )
    
    m_Mlayer[sampling]+=cell_ptr->energy();
    
  } //iterating
  
  // picking out layers of interest and filling numerator and denominator energies.
  
  for (unsigned int i=0;i<m_em_fracnumsamp.size();++i) {
    numenergy+=m_Mlayer.find(m_em_fracnumsamp[i])->second;
  }  
  
  for (unsigned int i=0;i<m_em_fracdensamp.size();++i) {
    denenergy+=m_Mlayer.find(m_em_fracdensamp[i])->second;
  } 
  
  if (denenergy!=0) {
    m_em_frac=numenergy/denenergy;
    
    if (m_em_frac<m_em_fraccut) {
      return StatusCode::SUCCESS;
    }
    else {
      return StatusCode::FAILURE;  
    }
  }
  else {
    return StatusCode::FAILURE;
  }
}//accept
