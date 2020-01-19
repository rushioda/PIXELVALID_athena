/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNT_TBRecBase.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloIdManager.h"


CBNT_TBRecBase::CBNT_TBRecBase(const std::string& name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator), m_initialized(false), m_nt(NULL), m_log(NULL), 
  m_detStore(NULL), m_emId(NULL), m_hecId(NULL), m_fcalId(NULL),m_onlineId(NULL),
  m_larCablingSvc("LArCablingService")
{
}

CBNT_TBRecBase::~CBNT_TBRecBase() {
  delete m_log;
}

StatusCode CBNT_TBRecBase::initialize() {
  m_log=new MsgStream(msgSvc(), name());
  
  *m_log << MSG::DEBUG << "Initializing CBNT_TBRecBase base class" << endreq;
  
  StatusCode sc=service("DetectorStore",m_detStore);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  m_fcalId=caloIdMgr->getFCAL_ID();
  m_hecId=caloIdMgr->getHEC_ID();

  if (!m_emId) {
    (*m_log) << MSG::ERROR << "Could not access lar EM ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (!m_fcalId) {
    (*m_log) << MSG::ERROR << "Could not access lar FCAL ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (!m_hecId) {
    (*m_log) << MSG::ERROR << "Could not access lar HEC ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_detStore->retrieve(m_onlineId, "LArOnlineID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }
    else {
      (*m_log) << MSG::DEBUG << " Found the LArOnlineID helper. " << endreq;
    }

  sc = m_larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << " Can't get LArCablingSvc." << endreq;
    return sc;
  }

  m_initialized=true;
  return StatusCode::SUCCESS;
}

StatusCode CBNT_TBRecBase::pre_execute() {

  StatusCode sc;

  if(!m_initialized) {

    if (m_ntpath.size()==0 || m_ntTitle.size()==0) {
      *m_log << MSG::ERROR << "Need to set variable 'm_ntpath' and 'm_ntTitle' in constructor of deriving class!" << endreq;
      return StatusCode::FAILURE;
    }
 
    size_t i=m_ntpath.rfind('/');
    if (i==std::string::npos) {
      *m_log << MSG::ERROR << "Expected at least on '/' in path " << m_ntpath << endreq;
      return StatusCode::FAILURE;
    }
    std::string basepath(m_ntpath.begin(),m_ntpath.begin()+i);
    //std::cout << "Basepath" << basepath << std::endl;
 
    // retrive pointer to THistSvc
    ITHistSvc * tHistSvc = 0;
    sc = Gaudi::svcLocator()->service("THistSvc", tHistSvc);
    if (sc.isFailure()) {
       *m_log << MSG::ERROR << "Unable to retrieve pointer to THistSvc" << endreq;
       return sc;
    } 
    // get TTree
    sc = tHistSvc->getTree(m_ntpath,m_nt);
    if (sc.isFailure()) {
       *m_log << MSG::ERROR << "Unable to retrieve TTree : " << m_ntpath << endreq;
       return sc;
    }
    /*
    NTupleFilePtr file1(ntupleSvc(),basepath);
    if (!file1){
      (*m_log)  << MSG::ERROR << "Could not get NTupleFilePtr with path " << basepath << " failed" << endreq;
      return StatusCode::FAILURE;
    }
    NTuplePtr nt(ntupleSvc(),m_ntpath);
    if (!nt) {
      nt=ntupleSvc()->book(m_ntpath,CLID_ColumnWiseTuple,m_ntTitle);
    }
    if (!nt){
      (*m_log)  << MSG::ERROR << "Booking of NTuple at "<< m_ntpath << " and name " << m_ntTitle << " failed" << endreq;
      return StatusCode::FAILURE; 
    }
    m_nt=nt;
    */ 
    //std::cout << "Ntuple ptr:" << m_nt << std::endl;

    // subalgorithm initialisation
    try {
      sc = this->CBNT_initialize();
    }
    catch( const std::exception& Exception ) {
      *m_log << MSG::ERROR << " Standard exception " 
                  << Exception.what() 
                  << " caught from sub-algorithm::CBNT_initialize () :" << endreq ;
    }
    catch (...) {
              *m_log << MSG::ERROR << " Unknown exception " 
         << " caught from sub-algorithm::CBNT_initialize () :" << endreq ;
   }

   if (sc.isFailure())
     {
              *m_log << MSG::ERROR << "CBNT_initialize() failed" << endreq;
               return sc;
     }

    m_initialized=true;
  }
  
  // clear data members
  try {
            sc = this->CBNT_clear();
  }
  catch( const std::exception& Exception ) {
   *m_log << MSG::ERROR << " Standard exception " 
        << Exception.what() 
        << " caught from sub-algorithm::CBNT_clear () :" << endreq ;
  }
  catch (...) {
   *m_log << MSG::ERROR << " Unknown exception " 
               << " caught from sub-algorithm::CBNT_clear () :" << endreq ;
  }

  if (sc.isFailure()) {
      *m_log << MSG::ERROR << "CBNT_clear() failed" << endreq;
       return sc;
  }
 
  return StatusCode::SUCCESS ;
}


StatusCode CBNT_TBRecBase::execute() {

  StatusCode sc;

   // run pre-execution
  try {
            sc = this->pre_execute();
  }
        
  catch( const std::exception& Exception ) {
   *m_log << MSG::ERROR << " Standard exception " 
               << Exception.what() 
               << " caught from sub-algorithm::CBNT_pre_execute (). Disable !" << endreq ;
    sc = this->setProperty(BooleanProperty( "Enable",false ) );
    return StatusCode::FAILURE;
   
  }
  catch (...) {
    *m_log << MSG::ERROR << " Unknown exception " 
         << " caught from sub-algorithm::CBNT_pre_execute (). Disable !" << endreq ;
     sc = this->setProperty(BooleanProperty( "Enable",false ) );
     return StatusCode::FAILURE;
  }
 
  if (sc.isFailure()) {
    *m_log << MSG::ERROR << "CBNT_pre_execute() failed. Disable !" << endreq;
     sc = this->setProperty(BooleanProperty( "Enable",false ) );
     return sc;
  }
           
  // now subalgorithm execution
  try {
     sc = this->CBNT_execute();
  }
 
  catch( const std::exception& Exception ) {
    *m_log << MSG::ERROR << " Standard exception " 
         << Exception.what() 
         << " caught from sub-algorithm::CBNT_execute () :" << endreq ;
     return StatusCode::FAILURE;
  }
  catch (...) {
    *m_log << MSG::ERROR << " Unknown exception " 
         << " caught from sub-algorithm::CBNT_execute () :" << endreq ;
     return StatusCode::FAILURE;
  }

  return sc;
}


StatusCode CBNT_TBRecBase::finalize() {

  StatusCode sc;

  //now subalgorithm finalisation
  try {
    sc = this->CBNT_finalize();
  }
  catch( const std::exception& Exception ) {
    *m_log << MSG::ERROR << " Standard exception " 
         << Exception.what() 
         << " caught from sub-algorithm::CBNT_finalize () :" << endreq ;
  }
  catch (...) {
     *m_log << MSG::ERROR << " Unknown exception " 
          << " caught from sub-algorithm::CBNT_finalize () :" << endreq ;
  }
 
  return sc;
}
