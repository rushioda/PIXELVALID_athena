/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @class IAddressProvider
 * @brief interface for IOA providers
 *
 * @author <pcalafiura@lbl.gov> - ATLAS Collaboration
 *
 * $Id: IAddressProvider.h,v 1.9 2003-04-16 01:12:23 calaf Exp $
 ***************************************************************************/

#ifndef ATHENAKERNEL_IADDRESSPROVIDER_H
# define ATHENAKERNEL_IADDRESSPROVIDER_H

//<<<<<< INCLUDES                                                       >>>>>>
#include <list>
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/StoreID.h"

//<<<<<< FORWARD DECLARATIONS                                           >>>>>>
namespace SG {
  class TransientAddress;
}

//<<<<<< CLASS DECLARATIONS                                             >>>>>>
class IAddressProvider {
public:
  typedef std::list<SG::TransientAddress*> tadList;
  typedef tadList::iterator tadListIterator;

  ///get all addresses from Provider : Called before Begin Event
  virtual StatusCode preLoadAddresses(StoreID::type /* storeID */,
				      tadList& /* list */) {
     return StatusCode::SUCCESS;
  }

  /// get all new addresses from Provider for this Event.
  virtual StatusCode loadAddresses(StoreID::type /* storeID */,
				   tadList& /* list */) {
     return StatusCode::SUCCESS;
  }

  /// update a transient Address
  virtual StatusCode updateAddress(StoreID::type storeID,
				   SG::TransientAddress* pTAd) = 0;

  virtual ~IAddressProvider() {}
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // ATHENAKERNEL_IADDRESSPROVIDER_H



