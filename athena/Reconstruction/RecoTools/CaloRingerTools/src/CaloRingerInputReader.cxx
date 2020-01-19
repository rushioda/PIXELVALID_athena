/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerInputReader.cxx 667886 2015-05-18 17:26:59Z wsfreund $
// =============================================================================
#include "CaloRingerInputReader.h"

#include "AthenaKernel/errorcheck.h"

namespace Ringer {

// =============================================================================
CaloRingerInputReader::CaloRingerInputReader(const std::string& type,
                                 const std::string& name,
                                 const ::IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  // declare interface
  declareInterface<ICaloRingerInputReader>(this);

  // Whether builder tool is available
  declareProperty("builderAvailable", m_builderAvailable, 
      "Whether Builder Tool is available.");
  
  // The CaloRingsBuilder Tool
  declareProperty("crBuilder", m_crBuilder, "CaloRingsBuilder Tool" );

  // The input container key 
  declareProperty("inputKey", m_inputKey, "The input container key" );

}

// =============================================================================
CaloRingerInputReader::~CaloRingerInputReader()
{ }

// =============================================================================
StatusCode CaloRingerInputReader::initialize()
{

  ATH_MSG_DEBUG("Initializing " << name() << "");

  if ( m_builderAvailable ) {
    CHECK( m_crBuilder.retrieve() );
  }

  m_builderAvailable = !m_crBuilder.empty();

  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode CaloRingerInputReader::finalize()
{
  return StatusCode::SUCCESS;
}

// =============================================================================
StatusCode CaloRingerInputReader::execute()
{ 
  return StatusCode::SUCCESS;
}

} // namespace Ringer

