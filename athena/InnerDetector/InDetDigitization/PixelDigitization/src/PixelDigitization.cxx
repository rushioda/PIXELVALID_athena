/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// PixelDigitization.cxx
//   Implementation file for class PixelDigitization
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////

#include "PixelDigitization.h"
#include "PixelDigitizationTool.h"


// Constructor with parameters:
PixelDigitization::PixelDigitization(const std::string &name,
                                     ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_pixelDigitizationTool("PixelDigitizationTool")
{
  declareProperty("DigitizationTool",        m_pixelDigitizationTool, "PixelDigitizationTool Name");
}

PixelDigitization::~PixelDigitization()
{

}

// Initialize method:
StatusCode PixelDigitization::initialize()
{
  ATH_MSG_DEBUG ( "initialize()" );
  if (m_pixelDigitizationTool.retrieve().isFailure())
    {
      ATH_MSG_FATAL ( "Could not retrieve IPixelDigitizationTool");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG ( "Successfully retreived IPixelDigitizaitonTool." );
  return StatusCode::SUCCESS;
}
	
// Execute method:
StatusCode PixelDigitization::execute()
{
  ATH_MSG_DEBUG ( "execute()" );
  return m_pixelDigitizationTool->processAllSubEvents();
}

StatusCode PixelDigitization::finalize()
{
  ATH_MSG_DEBUG ( "finalize()" );
  return StatusCode::SUCCESS;
}
