/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file SGTools/src/exceptions.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Exceptions that can be thrown by SGTools.
 */


#include "SGTools/exceptions.h"
#include "GaudiKernel/System.h"
#include <sstream>
#include <string>


namespace SG {


/// Helper: Format exception string.
std::string excBadDataProxyCast_format (CLID id, const std::type_info& tid)
{
  std::ostringstream os;
  os << "Bad cast of DataProxy with CLID " << id
     << " to type " << System::typeinfoName (tid);
  return os.str();
}


/**
 * @brief Constructor.
 * @param id CLID of the DataProxy.
 * @param tid Type to which we're trying to convert the object.
 */
ExcBadDataProxyCast::ExcBadDataProxyCast (CLID id, const std::type_info& tid)
  : m_what (excBadDataProxyCast_format (id, tid))
{
}


/**
 * @brief Return the message for this exception.
 */
const char* ExcBadDataProxyCast::what() const throw()
{
  return m_what.c_str();
}


/**
 * @brief Throw an ExcBadDataProxyCast exception.
 * @param id CLID of the DataProxy.
 * @param tid Type to which we're trying to convert the object.
 */
void throwExcBadDataProxyCast (CLID id, const std::type_info& tid)
{
  throw ExcBadDataProxyCast (id, tid);
}


//*************************************************************************


/// Helper: Format exception string.
std::string excProxyCollision_format (CLID id,
                                      const std::string& key,
                                      CLID primary_id,
                                      const std::string& primary_key)
{
  std::ostringstream os;
  os << "ExcProxyCollision: proxy collision for clid/key "
     << id << " / " << key
     << " (primary " << primary_id << " / " << primary_key << ").";
  return os.str();
}


/**
 * @brief Constructor.
 * @param id CLID we're trying to set on the dummy proxy.
 * @param key Key we're trying to set on the dummy proxy.
 * @param primary_id CLID of the existing proxy.
 * @param primary_key Key of the existing proxy.
 */
ExcProxyCollision::ExcProxyCollision (CLID id,
                                      const std::string& key,
                                      CLID primary_id,
                                      const std::string& primary_key)
  : std::runtime_error (excProxyCollision_format (id,
                                                  key,
                                                  primary_id,
                                                  primary_key))
{
}


} // namespace SG
