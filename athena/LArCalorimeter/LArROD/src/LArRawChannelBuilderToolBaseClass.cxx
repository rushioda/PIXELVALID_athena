/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLBASECLASS
#define LARROD_LARRAWCHANNELBUILDERTOOLBASECLASS

#include "GaudiKernel/MsgStream.h"

#include "LArROD/LArRawChannelBuilderToolBaseClass.h"
#include "LArROD/LArRawChannelBuilderStatistics.h"
#include "LArROD/LArRawChannelBuilderDriver.h"

#include <iomanip>
#include <cmath>

LArRawChannelBuilderToolBaseClass::LArRawChannelBuilderToolBaseClass(const std::string& type,
								     const std::string& name,
								     const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_parent(NULL),
  m_helper(NULL),
  m_detStore(NULL),
  m_larCablingSvc(0)
  ,m_isSC(false)
{
declareProperty("IsSuperCell",              m_isSC  = false);
}

LArRawChannelBuilderToolBaseClass::~LArRawChannelBuilderToolBaseClass() {
  delete m_helper;
}

void
LArRawChannelBuilderToolBaseClass::printSummary()
{
  MsgStream log(msgSvc(), name());
  ulonglong sum=0;
  for( unsigned int i=0; i<m_helper->returnMaxErrors(); i++ )
    sum+=m_helper->returnErrorCount(i);
  
  log << MSG::INFO << "Printing Summary for tool " << name() << " :" << endreq;
  log << MSG::INFO
      << std::setw(40) << " " << " : "
      << std::setw(12) << " total "
      << std::setw(10) << " percent " << "  "
      << std::setw(12) << " per event "
      << std::setw(12) << " RMS "
      << endreq;

  double inv_sum = sum > 0 ? 1./sum : 1;
  for( unsigned int i=0; i<m_helper->returnMaxErrors(); i++ )
    {
      double fraction = 0.0;
      double perEvent = 0.0;
      double rmsPerEv = 0.0;
      if(sum>0)
        fraction=100 * double(m_helper->returnErrorCount(i)) * inv_sum;
      if(m_helper->returnEvents()>0)
        {
          perEvent = double(m_helper->returnErrorCountPerEvent(i)) / double(m_helper->returnEvents());
          rmsPerEv = double(m_helper->returnErrorCountSqPerEvent(i)) / double(m_helper->returnEvents());
          rmsPerEv = sqrt(std::max(0.,rmsPerEv-perEvent*perEvent));
        }
      
      log << MSG::INFO
	  << std::setw(40)
	  << m_helper->returnErrorString(i) << " : "
	  << std::setw(12)
	  << m_helper->returnErrorCount(i)
	  << std::setw(10)
	  << std::fixed
	  << std::setprecision(4)
	  << fraction
	  << " %"
	  << std::setw(12)
	  << std::fixed
	  << std::setprecision(4)
	  << perEvent
	  << std::setw(12)
	  << std::fixed
	  << std::setprecision(4)
	  << rmsPerEv
	  << endreq;
    }
}

void
LArRawChannelBuilderToolBaseClass::finalEventHidden()
{
  m_helper->resetErrorcountPerEvent();
  finalEvent();
}

Identifier
LArRawChannelBuilderToolBaseClass::currentID( void )
{
  if(m_parent->curr_id==0)
    {
      try {
        m_parent->curr_id = m_larCablingSvc->cnvToIdentifier(m_parent->curr_chid);
      } catch ( LArID_Exception & except ) {
	MsgStream log(msgSvc(), name());
        log << MSG::INFO
	    << "A larCablingSvc exception was caught for channel 0x!"
	    << MSG::hex << m_parent->curr_chid.get_compact() << MSG::dec << endreq;
        // log << MSG::INFO<<m_onlineHelper->print_to_string(curr_chid)<<endreq;
        m_parent->curr_id=0;
      }
    }
  return m_parent->curr_id;
}


#endif
