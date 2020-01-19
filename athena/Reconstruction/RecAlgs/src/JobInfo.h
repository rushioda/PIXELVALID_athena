/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECALGS_JOBINFO_H
#define RECALGS_JOBINFO_H

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "RVersion.h"
#include <string>

/** @class JobInfo
    

@author Rolf Seuster <seuster AT cern.ch>
*/

class ISvcLocator;

class JobInfo : public AthAlgorithm
{
 public:
  
  /** Standard Athena-Algorithm Constructor */
  JobInfo(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** Default Destructor */
  ~JobInfo() {};
  
  /** standard Athena-Algorithm method */
  StatusCode          initialize()
  { return StatusCode::SUCCESS; };
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize()
  { return StatusCode::SUCCESS; };

 private:
  
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  int m_events;
  
  int m_last_entries;
  
  int m_max_entries;
  
  bool m_no_warnings;
#endif
  
  bool m_printFATAL;
};

#endif 
