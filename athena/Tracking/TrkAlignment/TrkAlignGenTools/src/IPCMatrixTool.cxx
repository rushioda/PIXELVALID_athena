/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IPCMatrixTool.cxx

// AlgTool for creating big matrix and vector, manipulation of entries, and 
// solving for alignment parameters 
// Robert Harrington, started 1/5/08 based on SiGlobalChi2Align

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"

#include "TrkAlignGenTools/IPCMatrixTool.h"

#include "TrkAlgebraUtils/IPCMat.h"

#include <string>

namespace Trk {

  //_______________________________________________________________________
  IPCMatrixTool::IPCMatrixTool(const std::string& type, const std::string& name,
			       const IInterface* parent)
    : AthAlgTool(type,name,parent)
    , m_ipcmat(0)
  {
    declareInterface<IMatrixTool>(this);
    
    declareProperty("IPCRemoval",    m_IPCremoval    = false); 
    declareProperty("IPCMatrixName", m_ipcmatMatName = "/tmp/ipcmat.dat"); 
    declareProperty("IPCVectorName", m_ipcmatVecName = "/tmp/ipcvec.dat"); 

    m_logStream = 0;
  }

  //_______________________________________________________________________
  IPCMatrixTool::~IPCMatrixTool()
  {
    if (0!=m_ipcmat) delete m_ipcmat;
    m_ipcmat = NULL;

  }
  
  //_______________________________________________________________________
  StatusCode IPCMatrixTool::initialize()
  {
    msg(MSG::DEBUG) << "initialize() of IPCMatrixTool" << endreq;  
    
    return StatusCode::SUCCESS;
  }
  
  //_______________________________________________________________________
  StatusCode IPCMatrixTool::finalize()
  {
    msg(MSG::DEBUG) << "finalize() of IPCMatrixTool" << endreq;  
    
    return StatusCode::SUCCESS;    
  }
  
  //_______________________________________________________________________
  StatusCode IPCMatrixTool::allocateMatrix(int nDoF) 
  {
    
    if (0!=m_ipcmat) ATH_MSG_ERROR("IPCMat already exists!");
    m_ipcmat = new IPCMat(msgSvc());
    
    if (m_ipcmat->init()!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Failed to initialize ipcmat ");
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO("Success allocating ipcmatrix");

    if (m_ipcmat->allocate(nDoF)!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Failed to allocate matrix of size " << nDoF);
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO("Success allocating ipcvector");
    
    return StatusCode::SUCCESS;
  }
  
  //_______________________________________________________________________
  void IPCMatrixTool::prepareBinaryFiles(int) 
  {
    
    return;
  }
  
  //_______________________________________________________________________
  bool IPCMatrixTool::accumulateFromFiles() 
  {  
     
    return false;
  }
  
  //_______________________________________________________________________
  int IPCMatrixTool::solve() 
  {  
     
     return -1;
  }
  
  //_______________________________________________________________________
  void IPCMatrixTool::addFirstDerivatives(AlVec* )
  {
    return;
  }

  //________________________________________________________________________
  void IPCMatrixTool::addSecondDerivatives(AlSymMatBase* )
  {
    return;
  }

  //________________________________________________________________________
  void IPCMatrixTool::addFirstDerivatives(std::list<int,double>& )
  {
    return;
  }

  //________________________________________________________________________
  void IPCMatrixTool::addSecondDerivatives(std::list<std::pair<int,int>,double> & )
  {
    return;
  }

} // end of namespace
