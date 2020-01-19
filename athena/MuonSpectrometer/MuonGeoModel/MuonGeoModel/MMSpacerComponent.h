/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMSpacerComponent_H
#define MMSpacerComponent_H

#include "MuonGeoModel/StandardComponent.h"
#include <string>
#include <iostream>

namespace MuonGM {

class MMSpacerComponent: public StandardComponent {

public:
   MMSpacerComponent();
   friend std::ostream & operator << (std::ostream& os, const MMSpacerComponent & c);
};

} // namespace MuonGM

#endif
