/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdCylinderServices.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"

#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"


#include <sstream>
#include <cmath>

#include <iostream>

SCT_FwdCylinderServices::SCT_FwdCylinderServices(const std::string & name,
						 double rmin,
						 double rmax,
						 double length)
  : SCT_SharedComponentFactory(name), m_innerRadius(rmin), m_outerRadius(rmax), m_length(length)
{
  getParameters();
  m_physVolume = build(); 
  
}

SCT_FwdCylinderServices::~SCT_FwdCylinderServices()
{
}

void
SCT_FwdCylinderServices::getParameters()
{
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();

  // Retrieve parameters for each service    
  for (int iType = 0; iType < parameters->fwdNumCylinderServiceTypes(); iType++) {
    if(parameters->fwdCylinderServiceName(iType) == "CoolingPipe") {
      m_coolingDeltaR = parameters->fwdCylinderServiceDeltaR(iType);
      m_coolingRPhi = parameters->fwdCylinderServiceRPhi(iType);
      m_coolingMaterialName = parameters->fwdCylinderServiceMaterial(iType);
    }
    if(parameters->fwdCylinderServiceName(iType) == "LMT") {
      m_lmtDeltaR = parameters->fwdCylinderServiceDeltaR(iType);
      m_lmtRPhi = parameters->fwdCylinderServiceRPhi(iType);
      m_lmtMaterialName = parameters->fwdCylinderServiceMaterial(iType);
    }
    if(parameters->fwdCylinderServiceName(iType) == "LMTCooling") {
      m_lmtCoolingDeltaR = parameters->fwdCylinderServiceDeltaR(iType);
      m_lmtCoolingRPhi = parameters->fwdCylinderServiceRPhi(iType);
      m_lmtCoolingMaterialName = parameters->fwdCylinderServiceMaterial(iType);
    }
    if(parameters->fwdCylinderServiceName(iType) == "Fibres") {
      m_fibreDeltaR = parameters->fwdCylinderServiceDeltaR(iType);
      m_fibreRPhi = parameters->fwdCylinderServiceRPhi(iType);
      m_fibreMaterialName = parameters->fwdCylinderServiceMaterial(iType);
    }
    if(parameters->fwdCylinderServiceName(iType) == "NPipe") {
      m_nPipeDeltaR = parameters->fwdCylinderServiceDeltaR(iType);
      m_nPipeRPhi = parameters->fwdCylinderServiceRPhi(iType);
      m_nPipeMaterialName = parameters->fwdCylinderServiceMaterial(iType);
    }
    if(parameters->fwdCylinderServiceName(iType) == "Rail") {
      m_railDeltaR = parameters->fwdCylinderServiceDeltaR(iType);
      m_railRPhi = parameters->fwdCylinderServiceRPhi(iType);
      m_railMaterialName = parameters->fwdCylinderServiceMaterial(iType);
    }
  }

  // Retrieve angles for each service
  for (int iLoc = 0; iLoc < parameters->fwdNumCylinderServiceLocs(); iLoc++) {
    if(parameters->fwdCylinderServiceLocName(iLoc) == "CoolingPipe") {
      m_coolingLocAngle.push_back(parameters->fwdCylinderServiceLocAngle(iLoc));
    }
    if(parameters->fwdCylinderServiceLocName(iLoc) == "LMT") {
      m_lmtLocAngle.push_back(parameters->fwdCylinderServiceLocAngle(iLoc));
    }
    if(parameters->fwdCylinderServiceLocName(iLoc) == "LMTCooling") {
      m_lmtCoolingLocAngle.push_back(parameters->fwdCylinderServiceLocAngle(iLoc));
    }
    if(parameters->fwdCylinderServiceLocName(iLoc) == "Fibres") {
      m_fibreLocAngle.push_back(parameters->fwdCylinderServiceLocAngle(iLoc));
    }
    if(parameters->fwdCylinderServiceLocName(iLoc) == "NPipe") {
      m_nPipeLocAngle.push_back(parameters->fwdCylinderServiceLocAngle(iLoc));
    }
    if(parameters->fwdCylinderServiceLocName(iLoc) == "Rail") {
      m_railLocAngle.push_back(parameters->fwdCylinderServiceLocAngle(iLoc));
    }
  }

  // Starting position for LMT Cooling is midpoint between wheels 1 and 2
  double lmtCoolingZStart = 0.5 * (parameters->fwdWheelZPosition(1) + parameters->fwdWheelZPosition(2));
  m_lmtCoolingZOffset = 0.5 * (lmtCoolingZStart - parameters->fwdSupportFrameZMin()); 

}

GeoVPhysVol * 
SCT_FwdCylinderServices::build() 
{

  // Make envelope for the services
  SCT_MaterialManager materials;
  const GeoTube * cylinderShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  const GeoLogVol * cylinderLog = new GeoLogVol(getName(), cylinderShape, materials.gasMaterial());
  GeoPhysVol * cylinder = new GeoPhysVol(cylinderLog);

  // Create All Services
  // All except LMT Cooling run complete length of cylinder
  // LMT Cooling starts midway between wheels 1 and 2 
  // CoolingPipe, LMT and Fibres build up in z and are modelled as cone sections
  // LMT Cooling, N2 Pipes and Rails are uniform in z and modelled as tube sections
  // Cooling pipe, LMT, fibres and N2 pipe are placed at same radius
  // LMT Cooling is outside LMT
  // Rails are outside Cooling and N2 pipes

  // Cooling pipe
  double coolingRmin = m_innerRadius;
  double coolingRmax1 = coolingRmin + 0.2 * m_coolingDeltaR;
  double coolingRmax2 = coolingRmin + 1.8 * m_coolingDeltaR;
  double coolingDPhi = m_coolingRPhi / coolingRmin;
  const GeoCons* coolingShape = new GeoCons(coolingRmin, coolingRmin, coolingRmax1, coolingRmax2, 
					    0.5 * m_length, 
					    -0.5 * coolingDPhi * CLHEP::radian, coolingDPhi * CLHEP::radian);
  const GeoLogVol * coolingLog = new GeoLogVol("CoolingPipe", coolingShape, materials.getMaterialForVolume(m_coolingMaterialName, coolingShape->volume()));
  GeoPhysVol * coolingPipe = new GeoPhysVol(coolingLog);

  // Low Mass Tapes
  double lmtRmin = m_innerRadius;
  double lmtRmax1 = lmtRmin + 0.2 * m_lmtDeltaR;
  double lmtRmax2 = lmtRmin + 1.8 * m_lmtDeltaR;
  double lmtDPhi = m_lmtRPhi / lmtRmin;
  const GeoCons* lmtShape = new GeoCons(lmtRmin, lmtRmin, lmtRmax1, lmtRmax2, 0.5 * m_length, 
					-0.5 * lmtDPhi * CLHEP::radian, lmtDPhi * CLHEP::radian);
  const GeoLogVol * lmtLog = new GeoLogVol("LMT", lmtShape, materials.getMaterialForVolume(m_lmtMaterialName,lmtShape->volume()));
  GeoPhysVol * lmt = new GeoPhysVol(lmtLog);

  // LMT Cooling: must be outside LMTs
  double lmtCoolingRmin = lmtRmax2;
  double lmtCoolingRmax = lmtCoolingRmin + m_lmtCoolingDeltaR;
  double lmtCoolingDPhi = m_lmtCoolingRPhi / lmtCoolingRmin;
  double lmtLength = m_length - 2. * m_lmtCoolingZOffset;
  const GeoTubs* lmtCoolingShape = new GeoTubs(lmtCoolingRmin, lmtCoolingRmax, 0.5 * lmtLength, 
					       -0.5 * lmtCoolingDPhi * CLHEP::radian, lmtCoolingDPhi * CLHEP::radian);
  const GeoLogVol * lmtCoolingLog = new GeoLogVol("LMTCooling", lmtCoolingShape, materials.getMaterialForVolume(m_lmtCoolingMaterialName,lmtCoolingShape->volume()));
  GeoPhysVol * lmtCooling = new GeoPhysVol(lmtCoolingLog);

  // Fibres
  double fibreRmin = m_innerRadius;
  double fibreRmax1 = fibreRmin + 0.2 * m_fibreDeltaR;
  double fibreRmax2 = fibreRmin + 1.8 * m_fibreDeltaR;
  double fibreDPhi = m_fibreRPhi / fibreRmin;
  const GeoCons* fibreShape = new GeoCons(fibreRmin, fibreRmin, fibreRmax1, fibreRmax2, 0.5 * m_length,
					  -0.5 * fibreDPhi * CLHEP::radian, fibreDPhi * CLHEP::radian);
  const GeoLogVol * fibreLog = new GeoLogVol("Fibres", fibreShape, materials.getMaterialForVolume(m_fibreMaterialName,fibreShape->volume()));
  GeoPhysVol * fibres = new GeoPhysVol(fibreLog);

  // N2 Pipe
  double nPipeRmin = m_innerRadius;
  double nPipeRmax = nPipeRmin + m_nPipeDeltaR;
  double nPipeDPhi = m_nPipeRPhi / nPipeRmin;
  const GeoTubs* nPipeShape = new GeoTubs(nPipeRmin, nPipeRmax, 0.5 * m_length, 
					  -0.5 * nPipeDPhi * CLHEP::radian, nPipeDPhi * CLHEP::radian);
  const GeoLogVol * nPipeLog = new GeoLogVol("NPipe", nPipeShape, materials.getMaterialForVolume(m_nPipeMaterialName,nPipeShape->volume()));
  GeoPhysVol * nPipe = new GeoPhysVol(nPipeLog);

  // Rails: must be outside Cooling and N2 Pipes
  double railRmin = std::max(coolingRmax2, nPipeRmax);
  double railRmax = railRmin + m_railDeltaR;
  double railDPhi = m_railRPhi / railRmin;
  const GeoTubs* railShape = new GeoTubs(railRmin, railRmax,
					 0.5 * m_length, -0.5 * railDPhi * CLHEP::radian, railDPhi * CLHEP::radian);
  const GeoLogVol * railLog = new GeoLogVol("Rail", railShape, materials.getMaterialForVolume(m_railMaterialName,railShape->volume()));
  GeoPhysVol * rail = new GeoPhysVol(railLog);

  // Services are repeated for each quadrant
  for (int iquad = 0; iquad < 4; iquad++) {

    // Cooling pipe
    for (unsigned int iLoc = 0; iLoc < m_coolingLocAngle.size(); iLoc++) {
      double coolingAngle = m_coolingLocAngle[iLoc] + iquad * 90*CLHEP::degree;
      //      std::cout << "Placing cooling pipe at " << coolingAngle / CLHEP::degree << " CLHEP::degrees" << std::endl;
      cylinder->add(new GeoTransform(HepGeom::RotateZ3D(coolingAngle)));
      cylinder->add(coolingPipe);
    }

    // Low Mass Tapes and LMT Cooling are at same phi positions
    for (unsigned int iLoc = 0; iLoc < m_lmtLocAngle.size(); iLoc++) {
      double lmtAngle = m_lmtLocAngle[iLoc] + iquad * 90*CLHEP::degree;
      //      std::cout << "Placing LMT at " << lmtAngle / CLHEP::degree << " CLHEP::degrees" << std::endl;
      cylinder->add(new GeoTransform(HepGeom::RotateZ3D(lmtAngle)));
      cylinder->add(lmt);
      cylinder->add(new GeoTransform(HepGeom::RotateZ3D(lmtAngle)*HepGeom::TranslateZ3D(m_lmtCoolingZOffset)));
      cylinder->add(lmtCooling);
    }

    // Fibres are between pairs of LMTs
    for (unsigned int iLoc = 0; iLoc < m_fibreLocAngle.size(); iLoc++) {
      double fibreAngle = m_fibreLocAngle[iLoc] + iquad * 90*CLHEP::degree;
      //      std::cout << "Placing fibres at " << fibreAngle / CLHEP::degree << " CLHEP::degrees" << std::endl;
      cylinder->add(new GeoTransform(HepGeom::RotateZ3D(fibreAngle)));
      cylinder->add(fibres);
    }

    // N2 Pipes
    for (unsigned int iLoc = 0; iLoc < m_nPipeLocAngle.size(); iLoc++) {
      double nPipeAngle = m_nPipeLocAngle[iLoc] + iquad * 90*CLHEP::degree;
      //      std::cout << "Placing N2 pipe at " << nPipeAngle / CLHEP::degree << " CLHEP::degrees" << std::endl;
      cylinder->add(new GeoTransform(HepGeom::RotateZ3D(nPipeAngle)));
      cylinder->add(nPipe);
    }

    // Rails
    for (unsigned int iLoc = 0; iLoc < m_railLocAngle.size(); iLoc++) {
      double railAngle = m_railLocAngle[iLoc] + iquad * 90*CLHEP::degree;
      //      std::cout << "Placing rail at " << railAngle / CLHEP::degree << " CLHEP::degrees" << std::endl;
      cylinder->add(new GeoTransform(HepGeom::RotateZ3D(railAngle)));
      cylinder->add(rail);
    }

  } // end loop over quadrants

  return cylinder;
  
}

