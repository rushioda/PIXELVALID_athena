/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "PixelGeoModel/GeoPixelStaveRing.h"

#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "PixelGeoModel/GeoPixelModule.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"


#include <algorithm>
using std::max;
using namespace std;

GeoPixelStaveRing::GeoPixelStaveRing()
{
  m_ringName="staveRing";
  m_ringPosition="AC";
  
  m_zPosition =0;
  m_innerRadius = 0.;
  m_outerRadius = 0.;

  m_physVol = 0;
}

GeoVPhysVol* GeoPixelStaveRing::SetParametersAndBuild(std::string ringName, std::string ringPos)
{
  m_ringName=ringName;
  m_ringPosition=ringPos;
  m_physVol = Build();

  return m_physVol;
}

GeoVPhysVol* GeoPixelStaveRing::Build(){

  gmt_mgr->msg(MSG::INFO) <<"Build detailed stave ring support : "<<m_ringName<<"  "<<m_ringPosition<<endmsg;

  double safety = 0.001*CLHEP::mm; 
  bool isBLayer = false;
  if(gmt_mgr->GetLD() == 0) isBLayer = true;
  GeoPixelSiCrystal theSensor(isBLayer);
  GeoPixelModule pm(theSensor);
  // Ladder geometry

  double endBlockFixingPoint= gmt_mgr->IBLStaveMechanicalStaveEndBlockFixPoint();
  double totalStaveLength = gmt_mgr->IBLStaveLength();

  if(m_ringPosition=="AC")
    {
      // Stave ring geometry
      double ringWidth = gmt_mgr->IBLSupportRingWidth();
      m_innerRadius = gmt_mgr->IBLSupportRingInnerRadius();
      m_outerRadius = gmt_mgr->IBLSupportRingOuterRadius();
      double fixingPoint = gmt_mgr->IBLSupportMechanicalStaveRingFixPoint();

      // SES (dec 2012 - IBL pcache ): values hard coded as the one defined in ATLAS-IBL-02-01-00
      //   to get rid off the overlap between staverings ans ladder with ATLAS-IBL-02-00-00 values
      if(m_innerRadius<29.56) m_innerRadius=29.56;     // instead of 29.2
      if(m_outerRadius>31.85) m_outerRadius=31.85;     // instead of 32.

      double endBlockPosition = totalStaveLength*0.5+endBlockFixingPoint;
      double ringPosition = -ringWidth*0.5+fixingPoint;
      m_zPosition = endBlockPosition-ringPosition;

      double IPTouterRadius = gmt_mgr->IBLServiceGetMaxRadialPosition("IPT","simple",m_zPosition,m_zPosition)+safety;
      gmt_mgr->msg(MSG::DEBUG)<<"IBL stave ring "<<m_zPosition<<" "<<m_innerRadius<<"  "<<IPTouterRadius<<endmsg;
      if(IPTouterRadius>m_innerRadius) m_innerRadius=IPTouterRadius;

      // create log and phys volumes
      gmt_mgr->msg(MSG::DEBUG)<<"-> IBL stave ring "<<m_zPosition<<" "<<m_innerRadius<<"  "<<m_outerRadius<<endmsg;
      const GeoTube* ring_tube = new GeoTube(m_innerRadius,m_outerRadius,ringWidth*0.5);
      const GeoMaterial* ring_material_weight = mat_mgr->getMaterialForVolume("pix::StaveRing_IBLwght",ring_tube->volume());
      gmt_mgr->msg(MSG::DEBUG)<<"IBL stave ring weighted material : "<<(ring_material_weight==0)<<endmsg;

      GeoLogVol * logVol = 0;
      if(ring_material_weight)
	logVol = new GeoLogVol(m_ringName,ring_tube,ring_material_weight);
      else {
	const GeoMaterial* ring_material = mat_mgr->getMaterial("pix::StaveRing_IBL");
	logVol = new GeoLogVol(m_ringName,ring_tube,ring_material);
      }
      GeoPhysVol * logVolPV = new GeoPhysVol(logVol);
  gmt_mgr->msg(MSG::INFO) <<"Build detailed stave ring support - logVol : "<<logVol->getName()<<endmsg;

      return logVolPV;
    }

  // Stave ring geometry
  double ringWidth = gmt_mgr->IBLSupportMidRingWidth();
  m_innerRadius = gmt_mgr->IBLSupportMidRingInnerRadius();
  m_outerRadius = gmt_mgr->IBLSupportMidRingOuterRadius();
  
  m_zPosition = 0.0;
  
  // create log and phys volumes
  const GeoTube* ring_tube = new GeoTube(m_innerRadius,m_outerRadius,ringWidth*0.5);
  const GeoMaterial* ring_material_weight = mat_mgr->getMaterialForVolume("pix::StaveRingMid_IBLwght",ring_tube->volume());
  GeoLogVol *logVol=0;
  if(ring_material_weight)
    logVol = new GeoLogVol(m_ringName,ring_tube,ring_material_weight);
  else {
    const GeoMaterial* ring_material = mat_mgr->getMaterial("pix::MiddleRing_IBL");
    logVol = new GeoLogVol(m_ringName,ring_tube,ring_material);
  }

  GeoPhysVol * logVolPV = new GeoPhysVol(logVol); 

  gmt_mgr->msg(MSG::INFO) <<"Build detailed stave ring support - logVol : "<<logVol->getName()<<endmsg;

  return logVolPV;

}

