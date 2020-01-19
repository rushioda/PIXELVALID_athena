/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MultiLayer_H
#define MultiLayer_H

#include "MuonGeoModel/DetectorElement.h"

class GeoFullPhysVol;

namespace MuonGM {

class MultiLayer: public DetectorElement {
public:
   std::string logVolName;
   int nrOfLayers;
   int nrOfTubes;       // tubes in a layer
   double tubePitch;    // tube pitch in a layer
   double width;
   double length;
   double thickness;
   double mdtthickness;
   double longWidth;    // for trapezoidal layers
   int nrOfSteps;       // for trapezoidal layers, 
			// nr of steps in the staircase
   MultiLayer(std::string n);
   GeoFullPhysVol *build();
   double yy[4];
   double xx[4];
   void print();

   int cutoutNsteps;           // how many sub-multilayers there are along y-amdb
   int cutoutNtubes[5];        // how many tubes in the sub-multilayer [i]
   double cutoutXtubes[5];     // where is the centre of the tubes in sub-ml[i] along x-amdb
   double cutoutYmax[5];       // max Y(amdb) of this sub-multilayer
   double cutoutTubeLength[5]; // tube length
   bool cutoutFullLength[5];   // true if this region is outside the cutout
   bool cutoutAtAngle;      // true if this station has cutouts at an angle; //EMS1,3 and BOS6
    
   void setGeoVersion(int ver) {m_geo_version = ver;}
   int getGeoVersion() {return m_geo_version;}
   
   private:
   int m_geo_version;

};
} // namespace MuonGM

#endif
