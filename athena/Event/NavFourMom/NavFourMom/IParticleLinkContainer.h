///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** IParticleLinkContainer.h
 *  @author: Karsten Koeneke (DESY)
 *  @date July 2009
 *  @brief this defines a vector of ElementLinks to IParticleContainer
 *****************************************************************/

#ifndef NAVFOURMOM_IPARTICLELINKCONTAINER_H
#define NAVFOURMOM_IPARTICLELINKCONTAINER_H 1

// STL includes
#include <vector>

// DataModel includes
#include "DataModel/ElementLink.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "EventKernel/IParticle.h"
#include "NavFourMom/IParticleLink.h"
#include "NavFourMom/IParticleContainer.h"

// This defines the new container
class IParticleLinkContainer : public std::vector< IParticleLink >
{
};

// This defines the ID which is used by StoreGate
CLASS_DEF( IParticleLinkContainer , 1340008338 , 1 )




// This defines how to convert an INav4MomLinkContainer to an INavigable4MomentumCollection
class IParticleLinkContainerToIParticleContainerConverter
  : public SG::CopyConversion<IParticleLinkContainer,
                              IParticleContainer>
{
public:
  virtual void convert ( const IParticleLinkContainer& src,
                         IParticleContainer& dst ) const
  {
    size_t sz = src.size();
    if (dst.size() != sz) {
      dst.clear (SG::VIEW_ELEMENTS);
      dst.reserve (sz);
      for (size_t i = 0; i < sz; i++) {
        const IParticle* p = *(src[i]).cptr();
        dst.push_back (const_cast<IParticle*> (p));
      }
    }
  }
};

// This tells StoreGate about the conversion
SG_ADD_COPY_CONVERSION (IParticleLinkContainer, IParticleLinkContainerToIParticleContainerConverter);


#endif // NAVFOURMOM_IPARTICLELINKCONTAINER_H

