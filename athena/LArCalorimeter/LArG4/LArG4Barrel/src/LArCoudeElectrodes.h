/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARREL_LArCoudeElectrodes_H
#define LARG4BARREL_LArCoudeElectrodes_H

#include "PhysicalVolumeAccessor.h"
#include <string>

class LArCoudeElectrodes {
private:
  static PhysicalVolumeAccessor* s_theCoudes;
  double m_xcent[1024][15];
  double m_ycent[1024][15];
  double m_phirot[1024][15];
  bool m_filled;
  static LArCoudeElectrodes* s_instance;
public:
  static LArCoudeElectrodes* GetInstance(std::string strDetector="") ;
  double XCentCoude(int stackid, int cellid) const;
  double YCentCoude(int stackid, int cellid) const;
  double PhiRot(int stackid, int cellid) const;
protected:
  LArCoudeElectrodes(std::string strDetector="") ;
};

#endif // LARG4BARREL_LArCoudeElectrodes_H
