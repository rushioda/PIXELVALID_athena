/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ModulesConstructionH62004
#include "LArGeoH62004Algs/ModulesConstructionH62004.h"
#include "LArGeoH62004Algs/ExcluderConstructionH62004.h"
#include "LArGeoH62004Algs/HECConstructionH62004.h"
#include "LArGeoH62004Algs/FCALConstructionH62004.h"

// need if we want tu use EMEC from LArGeoTBEC:
#include "LArGeoTBEC/EMECModuleConstruction.h"
//#include "LArGeoEndcap/EMECConstruction.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include <string>
#include <cmath>
#include <iostream>

#include "LArG4RunControl/LArGeoTB2004Options.h"

float LArGeo::ModulesConstructionH62004::s_dX[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_dY[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_dZ[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_angleX[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_angleY[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_angleZ[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_shiftX[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_shiftY[NUM_LEAK];
float LArGeo::ModulesConstructionH62004::s_shiftZ[NUM_LEAK];

LArGeo::ModulesConstructionH62004::ModulesConstructionH62004():m_ModulesPhys(0),m_Options(0),m_fcalVisLimit(-1)
{
// fill the static arrays, if called first time
//
  //const double thick = 1.*CLHEP::cm;
  static bool first = true;
  if(first){
    first = false;
    for(int i=0; i<NUM_LEAK; ++i) {
       s_dX[i] = s_dY[i] = s_dZ[i] = 0.;
       s_shiftX[i] = s_shiftY[i] = s_shiftZ[i] = 0.;
       s_angleX[i] = s_angleY[i] = s_angleZ[i] = 0.;
    }

    s_dX[0] = s_dX[1] = 1.*CLHEP::cm; s_dY[0]= s_dY[1] = 31.6*CLHEP::cm; s_dZ[0] = s_dZ[1] = 50.*CLHEP::cm; 
    s_shiftX[0] = -26.*CLHEP::cm; s_shiftX[1] = -s_shiftX[0];
    s_shiftY[0] = s_shiftY[1] = 85.1*CLHEP::cm; s_shiftZ[0] = s_shiftZ[1] = -3.*CLHEP::cm;
    s_angleX[0] = s_angleX[1] = 4.668*CLHEP::deg; s_angleZ[0] = s_angleZ[1] = 0.*CLHEP::deg;
    s_angleY[0] = -30.*CLHEP::deg; s_angleY[1] = -s_angleY[0]; 
    s_dX[2] = 98.1*CLHEP::cm; s_dY[2] = 98.2*CLHEP::cm; s_dZ[2] = 30.6*CLHEP::cm;
    s_shiftX[2] = 0.*CLHEP::cm; s_shiftY[2] = 89.7*CLHEP::cm; s_shiftZ[2] = -42.*CLHEP::cm;
    s_angleX[2] = 94.668*CLHEP::deg; s_angleY[2] = 0.; s_angleZ[2] = 90.*CLHEP::degree;
    
    
    s_dX[3] = 1.*CLHEP::cm; s_dY[3] = 43.*CLHEP::cm; s_dZ[3] = 40.*CLHEP::cm;
    s_dX[4] = s_dX[3]; s_dY[4] = s_dY[3]; s_dZ[4] = s_dZ[3];
    s_shiftX[3] = -58.5*CLHEP::cm; s_shiftY[3] = 12.2*CLHEP::cm; s_shiftZ[3] = 5.*CLHEP::cm;
    s_shiftX[4] = -s_shiftX[3]; s_shiftY[4] = s_shiftY[3]; s_shiftZ[4] = s_shiftZ[3];
    s_angleX[3] = s_angleX[4] = 4.668*CLHEP::deg; s_angleY[3] = -45.*CLHEP::deg; 
    s_angleY[4] = -s_angleY[3]; 
    s_angleZ[3] = 0.*CLHEP::deg; 
    s_angleZ[4] = -s_angleZ[3];
    s_dX[5] = 130.*CLHEP::cm; s_dY[5] = 131.*CLHEP::cm; s_dZ[5] = 43.*CLHEP::cm; 
    s_shiftX[5] = 0.*CLHEP::cm; s_shiftY[5] = 18.1*CLHEP::cm; s_shiftZ[5] = -62.*CLHEP::cm;
    s_angleX[5] = 94.668*CLHEP::deg; s_angleY[5] = 0.*CLHEP::deg;
    s_angleZ[5] = 90.*CLHEP::deg;

    s_dX[6] = s_dX[7] = 1.*CLHEP::cm; s_dY[6] = s_dY[7] = 27.*CLHEP::cm; s_dZ[6] = s_dZ[7] = 40.*CLHEP::cm; 
    s_shiftX[6] = -58.*CLHEP::cm; s_shiftY[6] = s_shiftY[7] = -57.85*CLHEP::cm; s_shiftZ[6] = s_shiftZ[7] = -1.*CLHEP::cm;
    s_shiftX[7] = - s_shiftX[6]; 
    s_angleX[6] = s_angleX[7] = 4.668*CLHEP::deg; s_angleY[6] = -45.*CLHEP::deg; s_angleZ[6] = s_angleZ[7] = 0.*CLHEP::deg;
    s_angleY[7] = -s_angleY[6];
    s_dX[8] = 130.*CLHEP::cm; s_dY[8] = 131.*CLHEP::cm; s_dZ[8] = 27.*CLHEP::cm;
    s_shiftX[8] = 0.*CLHEP::cm; s_shiftY[8] = -51.9*CLHEP::cm; s_shiftZ[8] = -67.*CLHEP::cm;
    s_angleX[8] = 94.668*CLHEP::degree; s_angleY[8] = 0.*CLHEP::degree; s_angleZ[8] = 90.*CLHEP::degree;
    s_dX[9] = 1.*CLHEP::cm; s_dY[9] = 82.*CLHEP::cm; s_dZ[9] = 44.5*CLHEP::cm;
    s_shiftX[9] = 0.*CLHEP::cm; s_shiftY[9] = -89.0*CLHEP::cm; s_shiftZ[9] = 32.5*CLHEP::cm;
    s_angleX[9] = 4.668*CLHEP::degree; s_angleY[9] = 0.*CLHEP::degree; s_angleZ[9] = 90.*CLHEP::degree;    
    
    s_dX[10] = s_dX[11] =  1.*CLHEP::cm; s_dY[10] = s_dY[11] = 41.5*CLHEP::cm; s_dZ[10] = s_dZ[11] = 20.3*CLHEP::cm;
    s_shiftX[10] = -15.4*CLHEP::cm; s_shiftY[10] = s_shiftY[11] = 14.50*CLHEP::cm; s_shiftZ[10] = s_shiftZ[11] = -39.*CLHEP::cm;
    s_shiftX[11] = - s_shiftX[10];
    s_angleX[10] = s_angleX[11] = 4.668*CLHEP::degree; s_angleY[10] = -45.*CLHEP::degree; s_angleZ[10] = 0.*CLHEP::degree;    
    s_angleY[11] = -s_angleY[10]; s_angleZ[11] = -s_angleZ[10];    
  
    s_dX[12] = s_dX[13] = 1.*CLHEP::cm; s_dY[12] = s_dY[13] = 27.*CLHEP::cm; s_dZ[12] = s_dZ[13] = 20.3*CLHEP::cm;
    s_shiftX[12] = -15.4*CLHEP::cm; s_shiftY[12] = s_shiftY[13] = -54.4*CLHEP::cm; s_shiftZ[12] = s_shiftZ[13] = -43.8*CLHEP::cm;
    s_shiftX[13] = -s_shiftX[12];
    s_angleX[12]  = s_angleX[13] = 4.668*CLHEP::degree; s_angleY[12] = -45.*CLHEP::degree; s_angleZ[12] = 0.*CLHEP::degree;
    s_angleY[13] = -s_angleY[12]; s_angleZ[13] = -s_angleZ[12];
      
    s_dX[14] = s_dX[15] = 1.*CLHEP::cm; s_dY[14] = s_dY[15] = 12.*CLHEP::cm; s_dZ[14] = s_dZ[15] = 25.3*CLHEP::cm;
    s_shiftX[14] = -19.5*CLHEP::cm; s_shiftY[14] = s_shiftY[15] = -93.5*CLHEP::cm; s_shiftZ[14] = s_shiftZ[15] = -46.5*CLHEP::cm;
    s_shiftX[15] = -s_shiftX[14];
    s_angleX[14] = s_angleX[15] = 4.668*CLHEP::degree; s_angleY[14] = -45.*CLHEP::degree; s_angleZ[14] = s_angleZ[15] = 0.*CLHEP::degree;
    s_angleY[15] = -s_angleY[14]; 
    
    s_dX[16] = 59.5*CLHEP::cm; s_dY[16] = 60.0*CLHEP::cm; s_dZ[16] = 12.0*CLHEP::cm;
    s_shiftX[16] = 0.*CLHEP::cm; s_shiftY[16] = -91.5*CLHEP::cm; s_shiftZ[16] = -73.5*CLHEP::cm;
    s_angleX[16] = 94.668*CLHEP::degree; s_angleY[16] = 0.*CLHEP::degree; s_angleZ[16] = 90.*CLHEP::degree;
    s_dX[17] = 0.3*CLHEP::cm; s_dY[17] = 35.*CLHEP::cm; s_dZ[17] = 25.*CLHEP::cm;
    s_shiftX[17] = 0.*CLHEP::cm; s_shiftY[17] = -107.0*CLHEP::cm; s_shiftZ[17] = -40.*CLHEP::cm;
    s_angleX[17] = 4.668*CLHEP::degree; s_angleY[17] = 0.*CLHEP::degree; s_angleZ[17] = 90.*CLHEP::degree;
     
  }
  //StoreGateSvc* detStore;

  StatusCode status;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("DetectorStore", m_detectorStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in ModulesConstruction, cannot access DetectorStore");
  }

  status = m_detectorStore->retrieve(m_Options, "LArGeoTB2004Options");

  if(!status.isSuccess() ) {
     std::cout << "ModulesConstructionH62004:\tCan't access LArGeoTB2004Options, using default values\n";
  }

  if (StatusCode::SUCCESS != m_detectorStore->retrieve(m_materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in ModulesConstruction, cannot access Material manager");
  }

}

LArGeo::ModulesConstructionH62004::~ModulesConstructionH62004() {}

GeoVFullPhysVol* LArGeo::ModulesConstructionH62004::GetEnvelope()
{

  if (m_ModulesPhys) return (m_ModulesPhys);
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in ModulesConstructionH62004, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "ModulesConstructionH62004");
  log << MSG::INFO;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of H62004 GeoModel definition      +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  GeoMaterial *LAr  = m_materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in ModulesConstruction, std::LiquidArgon is not found.");

  // How to get Rohacell ????
  // Taking Margret's one
   // Define Rohacell Foam. 
  // Rohacell foam has density: 0.11g/cm3
  std::string name;
  double density;
  GeoElement*  C=m_materialManager->getElement("Carbon");
  GeoElement*  H=m_materialManager->getElement("Hydrogen");
  GeoElement*  O=m_materialManager->getElement("Oxygen");
  GeoElement*  N=m_materialManager->getElement("Nitrogen");
  GeoMaterial* Rohacell = new GeoMaterial(name="Rohacell", density=0.112*CLHEP::g/CLHEP::cm3);
  Rohacell->add(C,0.6465);
  Rohacell->add(H,0.07836);
  Rohacell->add(O,0.19137);
  Rohacell->add(N,0.08377);
  Rohacell->lock();

  /*
  a = 12.957*CLHEP::g/CLHEP::mole;                                                       
  density = 0.112*g/cm3;                                              
  z = 6.18;
  G4Material* Rohacell = new G4Material(name="Rohacell",z, a, density);
  */

  GeoMaterial  *Alu = m_materialManager->getMaterial("std::Aluminium");
  if (!Alu) throw std::runtime_error("Error in ModulesConstruction, std::Aluminium is not found.");

  // DB numbers:
  double bryr_y = 200.0*CLHEP::cm;
  double bcry_rlar = 125.5*CLHEP::cm;
  double bcry_phistart = 0.*CLHEP::degree;
  double bcry_phiend = 360.*CLHEP::degree;
  //double EMECdzende         =  63.2*CLHEP::cm;   // Can not get from parameters
  //double Zall = 62.6*cm; // Excluder dimension

  double   bepo_tx = 180.0*CLHEP::degree;
  double   bepo_tz = 90.0*CLHEP::degree;
  double   bepo_tz_e = ( M_PI / 4 )*CLHEP::rad;
  double   bepo_ty = 90.0*CLHEP::degree;
  double   bepo_Beta = 4.668*CLHEP::degree;
  double   bepo_z_e = -42.86*CLHEP::cm; // 43 CLHEP::cm * cos(4.668)
  double   bepo_emec_shift = 2.5*CLHEP::cm;
  double   bepo_excluder_shift = 34.4*CLHEP::cm;
  //double   bepo_hec_shift = 63.6*CLHEP::cm; // relative position of HEC versus EMEC
  //double   bepo_y_a = bcry_rlar-bepo_emec_shift-bepo_hec_shift;
  double   bepo_y_ex = bcry_rlar-bepo_excluder_shift;
  double   bepo_y_hecshift = 6.*CLHEP::mm;
  double   bepo_y_emecshift = 11.*CLHEP::mm;
  double   bepo_x = -2.75*CLHEP::mm;
  double   bepo_x_e = -17.*CLHEP::mm;
//  double   bepo_x = 13.25*CLHEP::mm;
//  double   bepo_x_e = -3.*CLHEP::mm;
//  double   bepo_x = 17.25*CLHEP::mm;
//  double   bepo_x_e = -3.*CLHEP::mm;
  //double   bepo_z = -48.24*CLHEP::cm; // 48.4 CLHEP::cm * cos(4.668)
  double   bepo_y_e = bcry_rlar-bepo_emec_shift;
  double   bepo_pz = 45.0*CLHEP::degree;

  std::string baseName = "LArGeoTB::LeakageDet::";

  double ylen;
  if(m_Options && m_Options->isRun1()) {
     ylen = bryr_y;
  } else {
     ylen = bryr_y - 200.*CLHEP::mm;
  }
  GeoTubs *shapeMother = new GeoTubs( 0.0*CLHEP::cm, bcry_rlar, ylen, bcry_phistart,bcry_phiend);
  GeoLogVol *logMother = new GeoLogVol(baseName + "LAr", shapeMother, LAr);

  m_ModulesPhys = new GeoFullPhysVol(logMother);

  std::string tag = std::string("MODULES_POS");
  StatusCode status;
  StoredPhysVol *sPhysVol = new StoredPhysVol(m_ModulesPhys);
  status=m_detectorStore->record(sPhysVol,tag);
  if(!status.isSuccess()) throw std::runtime_error ((std::string("Cannot store")+tag).c_str());

// position leakage detectors
/*    
  for(int cal = 0; cal <= 5; ++cal){
    for(int dir = 0; dir <= 2; ++dir){
      for(int sid = 0; sid <= 1; ++sid){
        GeoLogVol *leak_log = construct(sid, dir, cal);
        if(leak_log == 0) continue;
        leak_log->SetSensitiveDetector(LeakSDdetector);
	leak_log->SetVisAttributes(G4VisAttributes::Invisible);
//	leak_log->SetVisAttributes(new G4VisAttributes(G4Color(0.7,0.1,0.7)));
        HepGeom::Transform3D leak_pos = position(sid, dir, cal);
        G4VPhysicalVolume *leak_phys =
        new G4PVPlacement(leak_pos,
                          leak_log,
                          leak_log->GetName(),
                          logMother,
                          false,
                          10000+GetID(sid,dir,cal)); 
      }
    }
  }
*/  
 
// position EMEC excluder
  
  ExcluderConstructionH62004 excluder(0);
  /*if(excluder != 0)*/{
//     std::cout<<"ModulesConstructionH62004::GetEnvelope positioning Excluder"<<std::endl;
    GeoVFullPhysVol* excluderEnvelope = excluder.GetEnvelope();
    /*if(excluderEnvelope != 0)*/{
      CLHEP::HepRotation rot2; 
//      rot2.rotateZ(bepo_tz+bepo_tz_e);
      rot2.rotateZ(bepo_tz);
      rot2.rotateX(bepo_ty);
      rot2.rotateX(bepo_Beta);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
      m_ModulesPhys->add(new GeoTransform(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,bepo_y_ex,bepo_z_e+42.*CLHEP::mm))));
      m_ModulesPhys->add(excluderEnvelope);
    }
  }
  

// position FCAL excluder
  
 
  ExcluderConstructionH62004 fcexcluder(1);
  /*if(fcexcluder != 0)*/{
//     std::cout<<"ModulesConstructionH62004::GetEnvelope positioning Excluder"<<std::endl;
    GeoVFullPhysVol* fcexcluderEnvelope = fcexcluder.GetEnvelope();
    if(fcexcluderEnvelope != 0){
      CLHEP::HepRotation rot2; 
      rot2.rotateZ(bepo_tx);
      rot2.rotateX(-bepo_ty);
      rot2.rotateX(0.8*bepo_Beta);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
//      m_ModulesPhys->add(new GeoTransform(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,bepo_y_ex-138.*CLHEP::mm,-477.3*CLHEP::mm))));
      m_ModulesPhys->add(new GeoTransform(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,bepo_y_ex-146.*CLHEP::mm,-412.0*CLHEP::mm))));
      m_ModulesPhys->add(fcexcluderEnvelope);
    }
  }


// position Front excluder
  
  ExcluderConstructionH62004 frontexcluder(2);

  /*if(frontexcluder != 0)*/{
    GeoVFullPhysVol* frontexcluderEnvelope = frontexcluder.GetEnvelope();
    if(frontexcluderEnvelope != 0){
      CLHEP::HepRotation rot2; 
//      rot2.rotateZ((90.-18.)*CLHEP::degree);
      rot2.rotateZ((90.)*CLHEP::degree);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
      m_ModulesPhys->add(new GeoTransform(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,0.,20.*CLHEP::mm))));
      m_ModulesPhys->add(frontexcluderEnvelope);
      /*
      G4VPhysicalVolume* frontexcluderPhysical =
//	    new G4PVPlacement(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,0.,220.*CLHEP::mm)), // Translation 
	    new G4PVPlacement(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,0.,20.*CLHEP::mm)), // Translation 
			      frontexcluderEnvelope,                     // Logical volume
			      frontexcluderEnvelope->GetName(),          // Name 
			      logMother,                        // Mother volume 
			      false,                            // Boolean volume?  
			      0);                               // Copy number
			      */
    }
  }


// position Back excluder
  
  ExcluderConstructionH62004 backexcluder(3);

  /*if(backexcluder != 0)*/{
    GeoVFullPhysVol* backexcluderEnvelope = backexcluder.GetEnvelope();
    if(backexcluderEnvelope != 0){
      CLHEP::HepRotation rot2; 
      rot2.rotateZ((-90.-29.)*CLHEP::degree);
      m_ModulesPhys->add(new GeoSerialIdentifier(0));
      m_ModulesPhys->add(new GeoTransform(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,0.,0.*CLHEP::mm))));
      m_ModulesPhys->add(backexcluderEnvelope);
      /*
      G4VPhysicalVolume* backexcluderPhysical =
	    new G4PVPlacement(HepGeom::Transform3D(rot2,CLHEP::Hep3Vector(0.,0.,0.*CLHEP::mm)), // Translation 
			      backexcluderEnvelope,                     // Logical volume
			      backexcluderEnvelope->GetName(),          // Name 
			      logMother,                        // Mother volume 
			      false,                            // Boolean volume?  
			      0);                               // Copy number
			      */
    }
  }
  
  

  // Let's try assembly volume

 // G4AssemblyVolume* assemblyMother = new G4AssemblyVolume();

  // Transformation for assembly - should be putted to each piece now.
  CLHEP::HepRotation rota;
  rota.rotateZ(-bepo_pz);
  rota.rotateX(bepo_ty);
  rota.rotateX(bepo_Beta);
  
//  HepGeom::Transform3D trans(rota, CLHEP::Hep3Vector(0.,bepo_y_e,bepo_z_e));
  HepGeom::Transform3D trans(rota, CLHEP::Hep3Vector(0.,bepo_y_e,bepo_z_e+65.*CLHEP::mm));
  
//positions emec

  // Z-positions DB values !!!!

  double HECzStart         =  427.7*CLHEP::cm;
  double EMECzStart         = 364.1*CLHEP::cm;
  double FCALzStart         =  466.85*CLHEP::cm;
  double FCALzEnd           =  588.28*CLHEP::cm;

  if((!m_Options) || m_Options->isEmec()){
     CLHEP::HepRotation MrotEmec ;
     MrotEmec.rotateZ(bepo_tz_e); 
     // original value:
//     CLHEP::Hep3Vector pos3Emec(    0*CLHEP::mm,   9.0*CLHEP::mm ,   55.9*CLHEP::mm);
//     CLHEP::Hep3Vector pos3Emec(    3.636*CLHEP::mm,   9.0*CLHEP::mm ,   55.9*CLHEP::mm);
//     CLHEP::Hep3Vector pos3Emec(    bepo_x_e,   9.*CLHEP::mm ,   61.*CLHEP::mm);
       CLHEP::Hep3Vector pos3Emec( (bepo_x_e - bepo_y_emecshift )/2./sin(bepo_tz_e), (bepo_x_e + bepo_y_emecshift )/2./sin(bepo_tz_e), 61.*CLHEP::mm);
//     CLHEP::Hep3Vector pos3Emec(    0.*CLHEP::mm, bepo_x_e,   61.*CLHEP::mm);

//     std::cout<<"ModulesConstructionH62004 calling EMECModuleConstruction....."<<std::endl;
  //use this line for physical construction of the EMEC inner wheel only:
//     EMECModuleConstruction*  emecConstruction = new EMECModuleConstruction(true, true, false);
     EMECModuleConstruction emecConstruction(true, true, false);

     GeoVFullPhysVol* emecEnvelope = emecConstruction.GetEnvelope();
     m_ModulesPhys->add(new GeoNameTag("LAr"));
     m_ModulesPhys->add( new GeoTransform(trans) );
     m_ModulesPhys->add( new GeoTransform( HepGeom::Transform3D(MrotEmec, pos3Emec) ) );
     m_ModulesPhys->add(emecEnvelope);
//     std::cout<<"ModulesConstructionH62004  EMECModuleConstruction done...."<<std::endl;
  }


  if((!m_Options) || m_Options->isHec()){
    //positions hec

    HECConstructionH62004 hec;
    /*if(hec != 0)*/{
      GeoVPhysVol* hecEnvelope = hec.GetEnvelope();
      if(hecEnvelope != 0){
        CLHEP::HepRotation rot;
//        rot.rotateZ(-bepo_pz);
//        rot.rotateX(bepo_ty);
//        rot.rotateX(bepo_Beta);
  
//        HepGeom::Transform3D hpos = HepGeom::Transform3D(rot,CLHEP::Hep3Vector(bepo_x,bepo_y_a,bepo_z));
//        CLHEP::Hep3Vector hecshift(0.,6.*CLHEP::mm,HECzStart-EMECzStart);
          CLHEP::Hep3Vector hecshift((bepo_x - bepo_y_hecshift)/2./sin(bepo_tz_e), (bepo_x + bepo_y_hecshift)/2./sin(bepo_tz_e), HECzStart-EMECzStart);
//        CLHEP::Hep3Vector hecshift(-21.*CLHEP::mm, bepo_x, HECzStart-EMECzStart);
//        CLHEP::Hep3Vector hecshift(-5.*CLHEP::mm, bepo_x, HECzStart-EMECzStart);
//        CLHEP::Hep3Vector hecshift(0.*CLHEP::mm, bepo_x, HECzStart-EMECzStart);
//       CLHEP::HepRotation norot;
        HepGeom::Transform3D hectrans(rot,hecshift);
        m_ModulesPhys->add( new GeoTransform(trans) );
        m_ModulesPhys->add(new GeoTransform(hectrans));
        m_ModulesPhys->add(hecEnvelope);

//       assemblyMother->AddPlacedVolume( hecEnvelope, hectrans );
      }
    }
  }
   
  if((!m_Options) || m_Options->isFcal()){ 
    FCALConstructionH62004 fcal;
    /*if(fcal != 0)*/{
      fcal.setFCALVisLimit(m_fcalVisLimit); 
      GeoVPhysVol* fcalEnvelope = fcal.GetEnvelope();
      if(fcalEnvelope != 0){
        CLHEP::HepRotation rotFCal;
       // rotFCal.rotateY(0.*CLHEP::deg);
       //  rotFCal.rotateZ(-bepo_pz);
       //  rotFCal.rotateX(bepo_ty);
       //  rotFCal.rotateX(bepo_Beta);
//        CLHEP::Hep3Vector fcalshift(0.,-7.*CLHEP::mm,FCALzStart-EMECzStart+(FCALzEnd-FCALzStart)/2.);
        CLHEP::Hep3Vector fcalshift(9.*CLHEP::mm,0.*CLHEP::mm,FCALzStart-EMECzStart+(FCALzEnd-FCALzStart)/2.);
        HepGeom::Transform3D fcaltrans(rotFCal,fcalshift);
        m_ModulesPhys->add( new GeoTransform(trans) );
        m_ModulesPhys->add(new GeoTransform(fcaltrans));
        m_ModulesPhys->add(fcalEnvelope);
      }
    }
  }
  
  if((!m_Options) || m_Options->isColdnose()){
  // Position a cold nose
  //
  
  double box_x = (650./2.)*CLHEP::mm;
  double box_y = (356./2.)*CLHEP::mm;
  double box_z = (50.8/2.)*CLHEP::mm;
  double btot = 494.*CLHEP::mm;
  double tub_r = 452.*CLHEP::mm;
  double tub1_dr = 8.*CLHEP::mm;
  double tub1_z = (1320./2.)*CLHEP::mm;
  double cyl_dr = 10.*CLHEP::mm;
  double cyl_r1 = 262.*CLHEP::mm;
  double cyl_r2 = 336.5*CLHEP::mm;
  double cyl_z = (912./2.)*CLHEP::mm;
  double cyl_shift = (10. + 25.)*CLHEP::mm; 
//  double NoseZshift = -360.*CLHEP::mm;
  double NoseZshift = -63.1*CLHEP::mm;
//  double NoseYshift = 94.4*CLHEP::mm;
//  double NoseYshift = 96.4*CLHEP::mm;
  double NoseYshift = 98.4*CLHEP::mm;
//  double NoseXshift = -195.*CLHEP::mm;
  double NoseXshift = -94.4*CLHEP::mm;

  GeoBox* Box1 = new GeoBox(box_x, box_y, box_z);
  double alpha = acos(box_x/tub_r);
  double ax = M_PI - 2*alpha;
  GeoTubs* Tub= new GeoTubs(0., tub_r, box_z, alpha, ax);

  CLHEP::HepRotation tRot;
//  tRot.rotateX(90*CLHEP::degree);
  CLHEP::Hep3Vector TubShift(0.,btot-box_y-tub_r,0.);
  HepGeom::Transform3D TubTrans(tRot,TubShift);
  const GeoShapeUnion &uSolid = (*Box1).add((*Tub)<<TubTrans);

  GeoTubs* Tub1 = new GeoTubs(tub_r, tub_r+tub1_dr, tub1_z, alpha, ax);  
//  CLHEP::Hep3Vector TubShift2(0.,btot-box_y-tub_r,-tub1_z+box_z);
  CLHEP::Hep3Vector TubShift2(0.,-135.5*CLHEP::mm,-tub1_z+box_z);
//  CLHEP::Hep3Vector TubShift2(0.,0.,0.);
//  tRot.rotateX(90*CLHEP::degree);
  HepGeom::Transform3D UnTrans(tRot,TubShift2);
  const GeoShapeUnion &uSolid2 = uSolid.add((*Tub1)<<UnTrans);

  GeoCons* Cone = new GeoCons(cyl_r2, cyl_r1, cyl_r2+cyl_dr, cyl_r1+cyl_dr, cyl_z, M_PI/4.,M_PI/2.);
//  CLHEP::Hep3Vector CylShift(0.,-box_y+cyl_shift,cyl_z+box_z-3.*CLHEP::mm);
  CLHEP::Hep3Vector CylShift(0.,-box_y+cyl_shift,cyl_z+box_z-5.*CLHEP::mm);
//  CLHEP::Hep3Vector CylShift(0.,0.,cyl_z+box_z);
  tRot.rotateX(1.*CLHEP::degree);
//  tRot.rotateZ(-90*CLHEP::degree);
  HepGeom::Transform3D CylTrans(tRot,CylShift);
  const GeoShapeUnion &uSolid3 =  uSolid2.add((*Cone)<<CylTrans);

  GeoLogVol* Cnose = new GeoLogVol("LArGeoTB::LeakageDet::ColdNose", &uSolid3 , Alu);
  GeoPhysVol *PhysCnose = new GeoPhysVol(Cnose);
//  cnose->SetVisAttributes(G4VisAttributes::Invisible);
  CLHEP::HepRotation CRotation;
//  CRotation.rotateZ(-bepo_pz);
  /*
  CRotation.rotateX(bepo_ty);
  CRotation.rotateZ(bepo_tx);
  CRotation.rotateX(bepo_Beta);
  */
  CRotation.rotateX(M_PI);
  CRotation.rotateZ(-3*M_PI/4.);
//  CLHEP::Hep3Vector tShift(NoseXshift,0.,FCALzStart-EMECzStart+NoseZshift);
  CLHEP::Hep3Vector tShift(NoseXshift,NoseYshift,FCALzStart-EMECzStart+NoseZshift);
//  CLHEP::Hep3Vector tShift(0.,NoseYshift,NoseZshift);
  HepGeom::Transform3D CTrans(CRotation,tShift);
  m_ModulesPhys->add( new GeoTransform(trans) ) ;
  m_ModulesPhys->add(new GeoTransform(CTrans));
  m_ModulesPhys->add(PhysCnose);
//  assemblyMother->AddPlacedVolume( cnose, CTrans );
  /*
  G4VPhysicalVolume* nosePhysical = new
        G4PVPlacement(HepGeom::Transform3D(CRotation,tShift),
                      cnose, cnose->GetName(),logMother, false, 0);                
  */ 

  // Do an imprint of assembly:
  
  /*
  CLHEP::HepRotation rota;
  rota.rotateZ(-bepo_pz);
  rota.rotateX(bepo_ty);
  rota.rotateX(bepo_Beta);
  
  HepGeom::Transform3D trans(rota, CLHEP::Hep3Vector(0.,bepo_y_e,bepo_z_e));
  
  assemblyMother->MakeImprint( logMother, trans );  
  */
  } 
  return m_ModulesPhys;
}

int LArGeo::ModulesConstructionH62004::GetID(int side, int dir, int calo)
{
     int i;
     if(calo<0 || calo >5) return 0;
     if(side<0 || side >1) return 0;

     switch(dir) {
        case 0: {
                   if(calo<=2) i=1; else i=5-calo;
                   return 3*calo+i+side;
                }
        case 2: {
                   if(side==1) return 0;
                   if(calo==2) return 10;
                   else if(calo==5) return 18;
                   else return 0;
                }
        case 1: {
                   if(side==1) return 0;
                   if(calo==3 || calo==4) return 0;
                   if(calo==5) i=2; else i=3;
                   return 3*calo+i;
                }

        default : {return 0;}
     }

}


GeoLogVol * LArGeo::ModulesConstructionH62004::construct(int side, int dir, int calo)
{
//--------- Material definition for leakage detectors ---------
  //double density, pressure, temperature;
  std::string name;
  
  //density     = universe_mean_density;    //from PhysicalConstants.h
  //pressure    = 3.e-18*pascal;
  //temperature = 2.73*CLHEP::kelvin;
  GeoMaterial* Vacuum = m_materialManager->getMaterial("std::Vacuum");
  if (!Vacuum) throw std::runtime_error("Error in ModulesConstruction, std::Vacuum is not found.");

//----------------- construct ID and name
  int myID = GetID(side,dir,calo);
  if(myID == 0 || myID > NUM_LEAK) return 0;
  if(s_dX[myID-1] == 0 && s_dY[myID-1] == 0 && s_dZ[myID-1] == 0) return 0;
  name = "LArGeoTB::LeakageDet::";  
  switch(calo){
  case 0:
    name += "emec";
    break;
  case 1:
    name += "hec1";
    break;
  case 2:
    name += "hec2";
    break;
  case 3:
    name += "fcal1";
    break;
  case 4:
    name += "fcal2";
    break;
  case 5:
    name += "coldtc";
    break;
  default:
    std::cerr << "Wrong calo for leakage !!" << std::endl;
    return 0;
  }
  switch(dir){
  case 0:
    name += "_side";
    break;
  case 1:
    name += "_up";
    break;
  case 2:
    name += "_back";
    break;
  default:
    std::cerr << "Wrong direction for leakage !!" << std::endl;
    return 0;
  }
  if(dir != 1){
    switch(side){
    case 0:
      name += "_left";
      break;
    case 1:
      name += "_right";
      break;
    default:
      std::cerr << "Wrong side for leakage !!" << std::endl;
      return 0;
    }
  }

//  std::cout<<"ModulesConstructionH62004::construct: "<<myID<<": "<<name<<std::endl;

//------------------ now construct shape and logical volume ---------------
  GeoLogVol *volume_log;
  if(myID == 6 || myID == 9 || myID == 17) {
     GeoTubs *tub = new GeoTubs(s_dX[myID-1],s_dY[myID-1],s_dZ[myID-1],-43.*CLHEP::degree,86.*CLHEP::degree);
     volume_log = new GeoLogVol(name,tub,Vacuum); 
  } else if(myID == 3) {
     GeoTubs *tub = new GeoTubs(s_dX[myID-1],s_dY[myID-1],s_dZ[myID-1],-32.*CLHEP::degree,64.*CLHEP::degree);
     volume_log = new GeoLogVol(name,tub,Vacuum); 
#if 0 // impossible case...
  } else if(myID == 19) {
    GeoTrd *trd = new GeoTrd(s_dX[myID-1]-16.*CLHEP::cm,s_dX[myID-1],s_dY[myID-1],s_dY[myID-1],s_dZ[myID-1]);
    volume_log = new GeoLogVol(name,trd,Vacuum); 
#endif
  } else {
    GeoBox *mbox = new GeoBox(s_dX[myID-1], s_dY[myID-1], s_dZ[myID-1]);
    volume_log = new GeoLogVol(name,mbox,Vacuum);
  }
  return volume_log;
}

HepGeom::Transform3D LArGeo::ModulesConstructionH62004::position(int side, int dir, int calo)
{
  HepGeom::Transform3D leak_position;

  int myID = GetID(side,dir,calo);
  if(myID == 0) {
    std::cerr << "Error! First construct the leakage detector !!" << std::endl;
  }
  assert(myID>0 && myID <= NUM_LEAK);
  if(myID > NUM_LEAK) return leak_position;

  CLHEP::HepRotation rotM;
  rotM.rotateZ(s_angleZ[myID-1]);
  rotM.rotateY(s_angleY[myID-1]);
  rotM.rotateX(s_angleX[myID-1]);

  leak_position = HepGeom::Transform3D(rotM,CLHEP::Hep3Vector(s_shiftX[myID-1],s_shiftY[myID-1],s_shiftZ[myID-1]));

  return leak_position;
}
     		      
