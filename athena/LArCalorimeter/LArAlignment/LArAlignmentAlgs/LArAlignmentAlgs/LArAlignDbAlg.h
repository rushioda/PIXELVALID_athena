/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LArCalorimeter/LArAlignment/LArAloignmentAlgs package
 -----------------------------------------
 Copyright (C) 2005 by ATLAS Collaboration
 ***************************************************************************/

#ifndef LARALIGNDBALG_LARALIGNDBALG_H
#define LARALIGNDBALG_LARALIGNDBALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

class EventInfo;
class IIOVRegistrationSvc;

/**
 ** Algorithm for writing LAr alignment constants to Cond DB and reading them back.
 **/

class LArAlignDbAlg: public AthAlgorithm 
{
 public:
  LArAlignDbAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArAlignDbAlg();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  StatusCode createCondObjects();
  StatusCode printCondObjects();
  StatusCode streamOutCondObjects();
  StatusCode registerCondObjects();
  
  StatusCode registerIOV(const CLID& clid);
  
  BooleanProperty           m_writeCondObjs;
  BooleanProperty           m_regIOV;
  StringProperty            m_streamName;
  StringProperty            m_inpFile;
  StringProperty            m_outpFile;
  StringProperty            m_outpTag;
  
  const EventInfo*          m_evt;

  ServiceHandle<IIOVRegistrationSvc>   m_regSvc;
  ToolHandle<IAthenaOutputStreamTool>  m_streamer;  
};

#endif // LARALIGNDBALG_LARALIGNDBALG_H
