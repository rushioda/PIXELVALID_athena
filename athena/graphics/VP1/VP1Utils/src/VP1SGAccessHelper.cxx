/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1SGAccessHelper                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Base/IVP1System.h"

//____________________________________________________________________
class VP1SGAccessHelper::Imp {
public:
  Imp(IVP1System * sys,const bool detStore)
    : sgcontents(sys,detStore),sg(sys?(detStore?sys->detectorStore():sys->storeGate()):0) {}
  Imp(StoreGateSvc * _sg)
    :  sgcontents(_sg), sg(_sg) {}
  VP1SGContentsHelper sgcontents;
  StoreGateSvc * sg;
};



//____________________________________________________________________
VP1SGAccessHelper::VP1SGAccessHelper( IVP1System * sys,const bool detStore )
  : VP1HelperClassBase(sys,"VP1SGAccessHelper"), d(new Imp(sys,detStore))
{
  if (!sys)
    message("ERROR: Received null system pointer (won't be able to get StoreGate pointer either)");
  else if (!d->sg)
    message("ERROR: Could not get "+QString(detStore?"storeGate":"detectorStore")+" pointer from system");
}


//____________________________________________________________________
VP1SGAccessHelper::VP1SGAccessHelper( StoreGateSvc * sg )
  : VP1HelperClassBase(0,"VP1SGContentsHelper"), d(new Imp(sg))
{
  if (!sg)
    message("ERROR: Received null storegate pointer");
}

//____________________________________________________________________
VP1SGAccessHelper::~VP1SGAccessHelper()
{
  delete d;
}

//____________________________________________________________________
StoreGateSvc * VP1SGAccessHelper::storeGate() const
{
  return d->sg;
}

//____________________________________________________________________
bool VP1SGAccessHelper::contains(const CLID& id, const QString& key) const
{
  return d->sgcontents.contains(id,key);
}
