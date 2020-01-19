///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** INav4MomLinkContainer.h
 *  @author: Karsten Koeneke
 *  @date July 2009
 *  @brief this defines a vector of ElementLinks to INavigable4MomentumCollection
 *****************************************************************/

#ifndef NAVFOURMOM_INAV4MOMLINKCONTAINER_H
#define NAVFOURMOM_INAV4MOMLINKCONTAINER_H 1

// STL includes
#include <vector>

// DataModel includes
#include "DataModel/ElementLink.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "NavFourMom/INav4MomLink.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"


// This defines the new container
class INav4MomLinkContainer : public std::vector< INav4MomLink >
{
};

// This defines the ID which is used by StoreGate
CLASS_DEF( INav4MomLinkContainer , 1194917823 , 1 )


// This defines how to convert an INav4MomLinkContainer to an INavigable4MomentumCollection
class INav4MomLinkContainerToINavigable4MomentumCollectionConverter
  : public SG::CopyConversion<INav4MomLinkContainer,
                              INavigable4MomentumCollection>
{
public:
  virtual void convert ( const INav4MomLinkContainer& src,
                         INavigable4MomentumCollection& dst ) const
  {
    size_t sz = src.size();
    if (dst.size() != sz) {
      dst.clear (SG::VIEW_ELEMENTS);
      dst.reserve (sz);
      for (size_t i = 0; i < sz; i++) {
        const INavigable4Momentum* p = *(src[i]).cptr();
        dst.push_back (const_cast<INavigable4Momentum*> (p));
      }
    }
  }
};

// This tells StoreGate about the conversion
SG_ADD_COPY_CONVERSION (INav4MomLinkContainer, INav4MomLinkContainerToINavigable4MomentumCollectionConverter);


#endif // NAVFOURMOM_INAV4MOMLINKCONTAINER_H

