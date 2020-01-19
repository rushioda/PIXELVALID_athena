/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloDummyCorrection.cxx,v 1.1 2009-04-19 03:58:47 ssnyder Exp $
/* @file  CaloDummyCorrection.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2009
 * @brief Dummy correction tool, used to get dummy constants objects
 *        written to the database.
 */


#include "CaloDummyCorrection.h"
#include "AthenaKernel/errorcheck.h"


/**
 * @brief Constructor.
 * @param type The type of this tool.
 * @param name The name of this tool.
 * @param parent The parent of this tool.
 */
using xAOD::CaloCluster;
CaloDummyCorrection::CaloDummyCorrection(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  : CaloClusterCorrection (type, name, parent)
{
}


/**
 * @brief Standard initialization method.
 */
StatusCode CaloDummyCorrection::initialize()
{
  // Need the explicit std::string here to avoid matching
  // the template in AlgTool.
  CHECK( setProperty ("isDummy", std::string("1")) );
  return CaloClusterCorrection::initialize();
}


// derived class implement the real correction.
// (Does nothing here.)
void CaloDummyCorrection::makeCorrection(const EventContext& /*ctx*/,
                                         CaloCluster*) const
{
}


/**
 * @brief Method to set a property value.
 * @param p The property name/value to set.
 *
 * Defined here as required by @c ToolWithConstantsMixin.
 */
StatusCode
CaloDummyCorrection::setProperty (const Property& p)
{
  // Ignore non-existing properties.
  std::string v;
  if (getProperty (p.name(), v).isSuccess())
    CHECK( CaloClusterCorrection::setProperty (p) );
  return StatusCode::SUCCESS;
}


