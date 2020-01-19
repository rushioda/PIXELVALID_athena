/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StandardComponent_H
#define StandardComponent_H

#include "MuonGeoModel/Component.h"
#include <string>

namespace MuonGM {

class StandardComponent: public Component  {
public:
    StandardComponent();
    StandardComponent(const StandardComponent& c);
    StandardComponent& operator=(const StandardComponent& c);

    double posx;
    double posy;
    double posz;
    double deadx;
    double deady;
    double dead3;
    double excent;
    int iswap;
    int index;
    double GetThickness() const;
    friend std::ostream& operator<<(std::ostream& os,const StandardComponent& c);
};
} // namespace MuonGM

#endif
