/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RpcLayer_H
#define RpcLayer_H

#include <string>

namespace MuonGM {

class Rpc;
class Cutout;

class RpcLayer: public DetectorElement {

public:
   double lwidth;
   double llength;
   double thickness;
   double llongWidth;   // for trapezoidal layers

   Rpc* m;
	
   RpcLayer(std::string s, Rpc* t);
   GeoVPhysVol* build();
   GeoVPhysVol* build(int cutoutson, std::vector<Cutout*>);
   void print();

};
} // namespace MuonGM

#endif
