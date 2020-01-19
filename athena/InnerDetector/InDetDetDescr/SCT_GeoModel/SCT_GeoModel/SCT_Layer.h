/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// CPW 17/8/06
// Version using new model of services from A. Tricoli
// 
#ifndef SCT_GEOMODEL_SCT_LAYER_H
#define SCT_GEOMODEL_SCT_LAYER_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;

class SCT_Bracket;
class SCT_Clamp;
class SCT_CoolingEnd;
class SCT_Flange;
class SCT_Harness;
class SCT_Module;
class SCT_Ski;
class SCT_SkiAux;
class SCT_SkiPowerTape;
class SCT_SupportCyl;
class SCT_FSIEndJewel;
class SCT_FSIScorpion;
class SCT_FSIFibreMask;
class SCT_Identifier;

class SCT_Layer: public SCT_UniqueComponentFactory
{
public:
  SCT_Layer(const std::string & name,
	    int iLayer,
	    const SCT_Module * module);

  ~SCT_Layer();
  //Explicitly disallow copy, assign to appease coverity
  SCT_Layer(const SCT_Layer &) = delete;
  SCT_Layer & operator=(const SCT_Layer &) = delete;

  virtual GeoVPhysVol * build(SCT_Identifier id) const;


public:
  int    skisPerLayer() const {return m_skisPerLayer;}  
  double tilt()         const {return m_tilt;}
  int    stereoSign()   const {return m_stereoSign;}
  double radius()       const {return m_radius;}
  
  double innerRadius() const {return m_innerRadius;}
  double outerRadius() const {return m_outerRadius;}
  double length()      const {return m_cylinderLength;}

  double skiPhiStart() const {return m_skiPhiStart;}

 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  void activeEnvelopeExtent(double & rmin, double & rmax);
  double calcSkiPhiOffset();

  // Layer number
  int    m_iLayer;

  int    m_skisPerLayer;
  double m_tilt;
  int    m_stereoSign;
  double m_radius;
  double m_safety;
  
  double m_innerRadius;
  double m_outerRadius;
  double m_cylinderLength;
  double m_activeLength;

  double m_innerRadiusActive;
  double m_outerRadiusActive;

  double m_outerRadiusOfSupport;

  double m_skiPhiStart;
  double m_skiAuxPhiStart;
  
  double m_bracketPhiOffset;
  double m_phiRefModule;

  bool   m_includeFSI;
  int    m_nRepeatEndJewel;
  double m_phiEndJewel;
  double m_zEndJewel;
  int    m_nRepeatScorpion;
  double m_phiScorpion;
  double m_zScorpion;

  const SCT_Module       * m_module;
  const SCT_Ski          * m_ski;
  const SCT_Clamp        * m_clamp;
  const SCT_CoolingEnd   * m_coolingEnd;
  const SCT_Bracket      * m_bracket;
  const SCT_Harness      * m_harness;
  const SCT_SkiPowerTape * m_skiPowerTape;
  const SCT_SkiAux       * m_skiAux;
  const SCT_Flange       * m_flange;
  const SCT_SupportCyl   * m_supportCyl;
  const SCT_FSIEndJewel  * m_endJewel;
  const SCT_FSIScorpion  * m_scorpion;
  const SCT_FSIFibreMask * m_fibreMask;

};

#endif // SCT_GEOMODEL_SCT_LAYER_H
