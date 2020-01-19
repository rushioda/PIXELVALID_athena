// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventInfoDict.h 630576 2014-11-24 12:36:44Z krasznaa $
#ifndef XAODEVENTINFO_XAODEVENTINFODICT_H
#define XAODEVENTINFO_XAODEVENTINFODICT_H

// System include(s):
#include <vector>
#include <set>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthLinks/DataLink.h"

// Local include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/versions/EventInfo_v1.h"
#include "xAODEventInfo/versions/EventAuxInfo_v1.h"
#include "xAODEventInfo/versions/EventInfoContainer_v1.h"
#include "xAODEventInfo/versions/EventInfoAuxContainer_v1.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODEVENTINFO {

      xAOD::EventInfoContainer_v1 c1;

      DataLink< xAOD::EventInfo_v1 > dl1;
      std::vector< DataLink< xAOD::EventInfo_v1 > > dl2;

      ElementLink< xAOD::EventInfoContainer_v1 > el1;
      std::vector< ElementLink< xAOD::EventInfoContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::EventInfoContainer_v1 > > > el3;

      std::set< uint32_t > set1;
      std::vector< std::set< uint32_t > > set2;

   };
} // private namespace

#endif // XAODEVENTINFO_XAODEVENTINFODICT_H
