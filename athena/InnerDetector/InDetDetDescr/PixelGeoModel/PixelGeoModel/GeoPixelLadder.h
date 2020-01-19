/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELLADDER_H
#define GEOPIXELLADDER_H

#include "PixelGeoModel/GeoVPixelFactory.h"
class GeoLogVol;
class GeoPixelSiCrystal;
class GeoPixelStaveSupport;

class GeoPixelLadder : public GeoVPixelFactory {
 public:
  GeoPixelLadder(GeoPixelSiCrystal& theSensor,
		 GeoPixelStaveSupport * staveSupport);
  virtual GeoVPhysVol* Build();
  double thickness() const {return m_thickness;}
  double thicknessP() const {return m_thicknessP;}
  double thicknessN() const {return m_thicknessN;}
  double width() const {return m_width;}


 private:

  double calcThickness();
  double calcWidth(); 

  const GeoLogVol* m_theLadder;
  GeoPixelSiCrystal& m_theSensor;
  GeoPixelStaveSupport * m_staveSupport;
  double m_thickness;
  double m_thicknessP;
  double m_thicknessN;
  double m_width;
};

#endif
