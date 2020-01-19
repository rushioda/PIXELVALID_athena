/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCComponent_H
#define sTGCComponent_H

#include "MuonGeoModel/StandardComponent.h"
#include <string>
#include <iostream>

namespace MuonGM {

class sTGCComponent: public StandardComponent {

public:
   sTGCComponent();
   friend std::ostream & operator << (std::ostream& os, const sTGCComponent & c);

   double yCutout;
   std::string subType;
};

} // namespace MuonGM

#endif
