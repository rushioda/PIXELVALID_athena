/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArGeoBarrel/BarrelDMConstruction.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoSerialDenominator.h"  
#include "GeoModelKernel/GeoXF.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

// For transforms:
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"

// For units:
#include "CLHEP/Units/PhysicalConstants.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

// For Functions:

// For functions:
#include "CLHEP/GenericFunctions/Abs.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"
#include "CLHEP/GenericFunctions/Sqrt.hh"
#include "CLHEP/GenericFunctions/ATan.hh"
#include "CLHEP/GenericFunctions/Rectangular.hh"
#include "CLHEP/GenericFunctions/Mod.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/FixedConstant.hh"

using namespace Genfun;
using namespace GeoXF;







// Constructor;
LArGeo::BarrelDMConstruction ::BarrelDMConstruction()
{
}

// Destructor:
LArGeo::BarrelDMConstruction::~BarrelDMConstruction() {
}
  
void LArGeo::BarrelDMConstruction::create(GeoFullPhysVol* envelope) 
{
  // Various initializations - Detector Store, Material Manager

  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in BarrelDMConstruction, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "BarrelDMConstruction"); 
    
  
  log << MSG::INFO;
  
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of Barrel DM GeoModel definition   +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    
  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in BarrelDMConstruction, cannot access DetectorStore");
  }

  IGeoModelSvc *geoModel;
  IRDBAccessSvc* rdbAccess;
  
  if(svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("Error in BarrelDMConstruction, cannot access GeoModelSvc");
  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("Error in BarrelDMConstruction, cannot access RDBAccessSvc");

  DecodeVersionKey larVersionKey(geoModel, "LAr");
  
  DataHandle<StoredMaterialManager> materialManager;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in BarrelDMConstruction, stored MaterialManager is not found.");

  }

  // following actions depend on the tag of LArBarrelDM node
  //
  // 1. LArBarrelDM-00 - DUMMY tag! works as a flag for building initial
  // version of the description with hardwired numbers
  //
  // 2. LArBarrelDM-01 and -02 are dummy tags as well.
  // 
  // 3. The first DB tag which is actually used by this code is LArBarrelDM-03
  //
  //   .... Hope that future tags will be used in a 'normal' way


  std::string strDMTopTag = rdbAccess->getChildTag("LArBarrelDM",larVersionKey.tag(),larVersionKey.node(),false);

  if(strDMTopTag=="")
    throw std::runtime_error("Error in BarrelDMConstruction, empty BarrelDM tag!");
  else if(strDMTopTag=="LArBarrelDM-00")
  {
    // Get some standard materials
    GeoMaterial *air        = materialManager->getMaterial("std::Air");
    const GeoMaterial *alu        = materialManager->getMaterial("std::Aluminium");
    //const GeoMaterial *c        = materialManager->getMaterial("std::Carbon");
    //const GeoMaterial *cu        = materialManager->getMaterial("std::Copper");
    GeoMaterial *matXeCO2O2 = materialManager->getMaterial("trt::XeCO2O2");
    GeoMaterial *shieldSteel = materialManager->getMaterial("shield::ShieldSteel");
    GeoMaterial *matRubber = materialManager->getMaterial("sct::Rubber");
    
    // Get required elements
    GeoElement* silicon = materialManager->getElement("Silicon");
    GeoElement* oxygen = materialManager->getElement("Oxygen");
    GeoElement* hydrogen = materialManager->getElement("Hydrogen");
    GeoElement* carbon = materialManager->getElement("Carbon");
    GeoElement* copper = materialManager->getElement("Copper");
    
    
    // Define some custom materials - That will move to the GeomDB
    //Fiberglass
    GeoMaterial *matFiberglass = new GeoMaterial("SiO2",2.20*CLHEP::gram/CLHEP::cm3);
    matFiberglass->add(silicon,silicon->getA()/(silicon->getA()+2*oxygen->getA()));
    matFiberglass->add(oxygen,2*oxygen->getA()/(silicon->getA()+2*oxygen->getA()));
    matFiberglass->lock();
    
    //Epoxy Resin
    GeoMaterial *matEpoxyResin = new GeoMaterial("Epoxy", 1.9*CLHEP::gram/CLHEP::cm3);
    matEpoxyResin->add(hydrogen,     14*hydrogen->getA()   / (14*hydrogen->getA() + 4*oxygen->getA()+ 8*carbon->getA()));
    matEpoxyResin->add(oxygen,        4*oxygen->getA()     / (14*hydrogen->getA() + 4*oxygen->getA()+ 8*carbon->getA()));
    matEpoxyResin->add(carbon,        8*carbon->getA()     / (14*hydrogen->getA() + 4*oxygen->getA()+ 8*carbon->getA()));
    matEpoxyResin->add(silicon,       1*silicon->getA()     / (5*carbon->getA() + 8*hydrogen->getA() + 4*oxygen->getA()+ 1*silicon->getA()));
    matEpoxyResin->lock();

    //FEBBoards
    GeoMaterial *matFEBBoards = new GeoMaterial("FEBBoards", 4.03*CLHEP::gram/CLHEP::cm3);
    matFEBBoards->add(matFiberglass, 0.52);
    matFEBBoards->add(copper, 0.28);
    matFEBBoards->add(matEpoxyResin, 0.20);
    matFEBBoards->lock();
    
    //SERVICES:CABLES, TUBES ETC...//
    
    //Water
    GeoMaterial *matWater = new GeoMaterial("Water", 1*CLHEP::gram/CLHEP::cm3);
    matWater->add(hydrogen,     2*hydrogen->getA()   / (2*hydrogen->getA() + 1*oxygen->getA()));
    matWater->add(oxygen,       1*oxygen->getA()     / (2*hydrogen->getA() + 1*oxygen->getA()));
    matWater->lock();
    
    //InDetServices
    GeoMaterial* matLArServices = new GeoMaterial("LArServices", 4.03*CLHEP::gram/CLHEP::cm3);
    matLArServices->add(shieldSteel, 0.20);
    matLArServices->add(copper, 0.60);
    matLArServices->add(matRubber, 0.10);
    matLArServices->add(air, 0.02);
    matLArServices->add(matXeCO2O2, 0.03); //new gas mixture from TRT
    matLArServices->add(matWater, 0.05);
    matLArServices->lock();
    
    //----------
    
    //----------- Building Front-end crates --------------------
    
    GeoBox     *Pedestal = new GeoBox(71, 400.05, 248.65);
    GeoBox     *Ped1     = new GeoBox(67, 397.05, 245.65);
    GeoTube    *Ped2     = new GeoTube(0, 150, 75);
    GeoTube    *Ped3     = new GeoTube(0, 2775, 300);   //, -75*CLHEP::deg, 150*CLHEP::deg); // 0, 2775, 300, -8.2*CLHEP::deg, 16.4*CLHEP::deg)
    
    //GeoLogVol  *lvped3   = new GeoLogVol("LAr::DM::PED3",Ped3,air);
    //GeoPhysVol *ped3   = new GeoPhysVol(lvped3);
    //envelope->add(ped3);
    
    const GeoShape & CratePed=((*Pedestal).subtract(*Ped1).
			       subtract((*Ped2)  <<HepGeom::TranslateY3D(-200.025)*HepGeom::RotateY3D(90*CLHEP::deg)).
			       subtract((*Ped3)  <<HepGeom::TranslateX3D(-2815)).
			       subtract((*Ped2)  <<HepGeom::TranslateY3D(200.025)*HepGeom::RotateY3D(90*CLHEP::deg)));
    
    
    GeoLogVol  *lvped   = new GeoLogVol("LAr::DM::PED",&CratePed,air);
    GeoPhysVol *pedestal   = new GeoPhysVol(lvped);
    //envelope->add(pedestal);
    
    GeoBox     *Crate1   = new GeoBox(244.5, 400.05, 255.05);
    GeoBox     *Crate2   = new GeoBox(250, 396.87, 245.55);
    GeoBox     *Crate3   = new GeoBox(186.5, 3.175, 245.55);
    const GeoShape & FEBCrate=(*Crate1).subtract(*Crate2).add((*Crate3)  <<HepGeom::TranslateX3D(-6.7));
    
    
    GeoLogVol  *lvcrate = new GeoLogVol("LAr::DM::CRATE",&FEBCrate,alu);
    GeoPhysVol *crate   = new GeoPhysVol(lvcrate);
    
    
    //---------- Build Boards and Cooling plates--------
    
    GeoBox     *Boxb1   = new GeoBox(205.472, 396.87, 245.55);
    GeoBox     *Boxb2   = new GeoBox(205.472, 3.175, 245.55);
    const GeoShape & BoardEnvelope= (*Boxb1).subtract(*Boxb2);
    GeoLogVol  *lvbenv = new GeoLogVol("LAr::DM::BOARDENVELOPE",&BoardEnvelope,air);
    GeoPhysVol *boardenvelope   = new GeoPhysVol(lvbenv);
    
    
    GeoBox     *Board   = new GeoBox(204.472, 1.0287, 244.475);
    GeoLogVol  *lvboard = new GeoLogVol("LAr::DM::FEBBOARD",Board,matFEBBoards); //Board Material go here
    GeoPhysVol *board   = new GeoPhysVol(lvboard);
    
    GeoBox     *Plate1   = new GeoBox(197, 7.5, 240);
    GeoBox     *Plate2   = new GeoBox(200, 5.5, 250);
    const GeoShape & CoolPlates= (*Plate1).subtract(*Plate2);
    GeoLogVol  *lvplate = new GeoLogVol("LAr::DM::COOLPLATE",&CoolPlates,alu);
    GeoPhysVol *plate   = new GeoPhysVol(lvplate);
    
    GeoBox     *BoardPannel  = new GeoBox(0.5, 9.5925, 244.475);
    GeoLogVol  *lvboardp     = new GeoLogVol("LAr::DM::BOARDPANNEL",BoardPannel,alu);
    GeoPhysVol *pannel       = new GeoPhysVol(lvboardp);
    
    const unsigned int NCrates=16;
    Variable       i;
    GENFUNCTION    f = (360*CLHEP::deg/NCrates)*i;
    GENFUNCTION    f1 = (360*CLHEP::deg/NCrates)*i+315*CLHEP::deg;
    GENFUNCTION    f2 = (360*CLHEP::deg/NCrates)*i+157.5*CLHEP::deg;
    GENFUNCTION    g = i*19.685;
    
    //(f=22.5|| f=45|| f=67.5|| f=180|| f=203.5|| f=225|| f=247.5|| f=270|| f=337.5|| f=360)
    
    //-------------- Place volumes in envelope ----------------------------
    
    //boards
    TRANSFUNCTION xfb1 = Pow(HepGeom::TranslateY3D(1.0),g)*HepGeom::TranslateY3D(19.685);
    TRANSFUNCTION xfb2 = Pow(HepGeom::TranslateY3D(1.0),-g)*HepGeom::TranslateY3D(-19.685);
    GeoSerialTransformer *stb1 = new GeoSerialTransformer(board,&xfb1, (NCrates+3));
    GeoSerialTransformer *stb2 = new GeoSerialTransformer(board,&xfb2, (NCrates+3));
    boardenvelope->add(stb1);
    boardenvelope->add(stb2);
    
    //coolingplates
    TRANSFUNCTION xfcp1 = Pow(HepGeom::TranslateY3D(1.0),g)*HepGeom::TranslateY3D(19.685);
    TRANSFUNCTION xfcp2 = Pow(HepGeom::TranslateY3D(1.0),-g)*HepGeom::TranslateY3D(-19.685);
    GeoSerialTransformer *stcp1 = new GeoSerialTransformer(plate,&xfcp1, (NCrates+3));
    GeoSerialTransformer *stcp2 = new GeoSerialTransformer(plate,&xfcp2, (NCrates+3));
    boardenvelope->add(stcp1);
    boardenvelope->add(stcp2);
    
    //boardpannels
    TRANSFUNCTION xfp1 = Pow(HepGeom::TranslateY3D(1.0),g)*HepGeom::TranslateY3D(19.685)*HepGeom::TranslateX3D(204.972);
    TRANSFUNCTION xfp2 = Pow(HepGeom::TranslateY3D(1.0),-g)*HepGeom::TranslateY3D(-19.685)*HepGeom::TranslateX3D(204.972);
    GeoSerialTransformer *stp1 = new GeoSerialTransformer(pannel,&xfp1, (NCrates+3));
    GeoSerialTransformer *stp2 = new GeoSerialTransformer(pannel,&xfp2, (NCrates+3));
    boardenvelope->add(stp1);
    boardenvelope->add(stp2);
    
    //crates
    TRANSFUNCTION xfc1 = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(3141.25)*HepGeom::TranslateZ3D(3135.05); 
    TRANSFUNCTION xfc2 = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(3141.25)*HepGeom::TranslateZ3D(-3135.05);
    GeoSerialTransformer *stc1 = new GeoSerialTransformer(crate,&xfc1, NCrates);
    GeoSerialTransformer *stc2 = new GeoSerialTransformer(crate,&xfc2, NCrates);
    envelope->add(stc1);
    envelope->add(stc2);
    
    //pedestal
    TRANSFUNCTION xfped1 = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(2825.75)*HepGeom::TranslateZ3D(3135.05); 
    TRANSFUNCTION xfped2 = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(2825.75)*HepGeom::TranslateZ3D(-3135.05);
    GeoSerialTransformer *stped1 = new GeoSerialTransformer(pedestal,&xfped1, NCrates);
    GeoSerialTransformer *stped2 = new GeoSerialTransformer(pedestal,&xfped2, NCrates);
    envelope->add(stped1);
    envelope->add(stped2);
    
    //boardenvelopes
    TRANSFUNCTION xfe1 = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(3180.278)*HepGeom::TranslateZ3D(3135.05);
    TRANSFUNCTION xfe2 = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(3180.278)*HepGeom::TranslateZ3D(-3135.05);
    GeoSerialTransformer *ste1 = new GeoSerialTransformer(boardenvelope,&xfe1, NCrates);
    GeoSerialTransformer *ste2 = new GeoSerialTransformer(boardenvelope,&xfe2, NCrates);
    envelope->add(ste1);
    envelope->add(ste2);
    
    
    //----------- Building envelope for Cables and Tubes --------------
    
    GeoTrd   *Trd1air  = new GeoTrd(123.5, 123.5, 167, 245.43, 117.65);
    GeoTrap  *Trapair  = new GeoTrap(178.33, 39.596*CLHEP::deg, 0, 167, 53.5, 53.5, 0, 167, 123.5, 123.5, 0);
    GeoTrd   *Trd2air  = new GeoTrd(53.5, 53.5, 280, 548, 677.5); 
    GeoBox   *Box   = new GeoBox(280, 280, 100); 
    
    GeoTrd   *Trd1alu  = new GeoTrd(5, 5, 167, 245.43, 117.65);
    GeoTrap  *Trapalu  = new GeoTrap(178.33, 45.5*CLHEP::deg, 0, 167, 5, 5, 0, 167, 5, 5, 0);
    GeoTrd   *Trd2alu  = new GeoTrd(5, 5, 280, 548, 677.5);
    
    HepGeom::Transform3D Cut1Box  = HepGeom::Translate3D(-295.5, 500, -473.563)*HepGeom::RotateX3D(-20*CLHEP::deg);
    HepGeom::Transform3D Cut2Box  = HepGeom::Translate3D(-295.5, -500, -473.563)*HepGeom::RotateX3D(20*CLHEP::deg);
    
    
    
    const GeoShape & Envelopes= (*Trd1air).
      // add((*Trapair)  <<HepGeom::Translate3D(-147.5, 0, -295.25)).
      add((*Trd2air)  <<HepGeom::Translate3D(-295.5, 0,  -1151.063)). 
      subtract((*Box)  <<HepGeom::Transform3D(Cut1Box)).
      subtract((*Box)  <<HepGeom::Transform3D(Cut2Box)).
      add((*Trapair)  <<HepGeom::Translate3D(-147.5, 0, -295.25));
    GeoLogVol  *lv          = new GeoLogVol("LAr::DM::Envelopes",&Envelopes,matLArServices); // Services material go here
    GeoPhysVol *envelopes    = new GeoPhysVol(lv);
    
    const GeoShape & Baseplates= (*Trd1alu).add((*Trapalu)  <<HepGeom::Translate3D(-180.5, 0, -295.25));
    GeoLogVol  *lvbis          = new GeoLogVol("LAr::DM::Baseplates",&Baseplates,alu);
    GeoPhysVol *baseplates    = new GeoPhysVol(lvbis);
    
    const GeoShape & SectorPlates= ((*Trd2alu)  <<HepGeom::Translate3D(-366, 0, -1151.063)).
      subtract((*Box)  <<HepGeom::Transform3D(Cut1Box)).
      subtract((*Box)  <<HepGeom::Transform3D(Cut2Box));
    GeoLogVol  *lvbiss          = new GeoLogVol("LAr::DM::Sectorplates",&SectorPlates,alu);
    GeoPhysVol *sectorplates    = new GeoPhysVol(lvbiss);
    
    
    //envelopes
    TRANSFUNCTION xf3a = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(-631.63)*HepGeom::TranslateX3D(3175.44)*HepGeom::TranslateZ3D(3165.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION xf4a = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(631.63)*HepGeom::TranslateX3D(-3175.44)*HepGeom::TranslateZ3D(-3165.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg);
    GeoSerialTransformer *st3 = new GeoSerialTransformer(envelopes,&xf3a, NCrates);
    GeoSerialTransformer *st4 = new GeoSerialTransformer(envelopes,&xf4a, NCrates);
    envelope->add(st3);
    envelope->add(st4);
    
    //baseplates
    TRANSFUNCTION xf3b = Pow(HepGeom::RotateZ3D(1.0),f1)*HepGeom::TranslateY3D(-631.63)*HepGeom::TranslateX3D(3175.44)*HepGeom::TranslateZ3D(3044.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION xf4b = Pow(HepGeom::RotateZ3D(1.0),(f1+22.5*CLHEP::deg))*HepGeom::TranslateY3D(631.63)*HepGeom::TranslateX3D(-3175.44)*HepGeom::TranslateZ3D(-3044.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg);
    GeoSerialTransformer *st3bis = new GeoSerialTransformer(baseplates,&xf3b, (NCrates-11));
    GeoSerialTransformer *st4bis = new GeoSerialTransformer(baseplates,&xf4b, (NCrates-11));
    envelope->add(st3bis);
    envelope->add(st4bis);
    
    TRANSFUNCTION xf5b = Pow(HepGeom::RotateZ3D(1.0),f2)*HepGeom::TranslateY3D(-631.63)*HepGeom::TranslateX3D(3175.44)*HepGeom::TranslateZ3D(3044.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION xf6b = Pow(HepGeom::RotateZ3D(1.0),(f2-22.5*CLHEP::deg))*HepGeom::TranslateY3D(631.63)*HepGeom::TranslateX3D(-3175.44)*HepGeom::TranslateZ3D(-3044.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg);
    GeoSerialTransformer *st5bis = new GeoSerialTransformer(baseplates,&xf5b, (NCrates-11));
    GeoSerialTransformer *st6bis = new GeoSerialTransformer(baseplates,&xf6b, (NCrates-11));
    envelope->add(st5bis);
    envelope->add(st6bis);
    
    //sectorplates
    TRANSFUNCTION xf3bb = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(-631.63)*HepGeom::TranslateX3D(3175.44)*HepGeom::TranslateZ3D(3044.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION xf4bb = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(631.63)*HepGeom::TranslateX3D(-3175.44)*HepGeom::TranslateZ3D(-3044.5)*HepGeom::RotateZ3D(-11.25*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg);
    GeoSerialTransformer *st3biss = new GeoSerialTransformer(sectorplates,&xf3bb, NCrates);
    GeoSerialTransformer *st4biss = new GeoSerialTransformer(sectorplates,&xf4bb, NCrates);
    envelope->add(st3biss);
    envelope->add(st4biss);
  }
  else if(strDMTopTag=="LArBarrelDM-01" || strDMTopTag=="LArBarrelDM-02")
  {
  // Get some standard materials
  GeoMaterial *air        = materialManager->getMaterial("std::Air"); //0.001214 CLHEP::g/CLHEP::cm3
  GeoMaterial *alu        = materialManager->getMaterial("std::Aluminium"); //2.7 CLHEP::g/CLHEP::cm3
  GeoMaterial *shieldSteel = materialManager->getMaterial("shield::ShieldSteel"); //8 CLHEP::g/CLHEP::cm3
  GeoMaterial *matCO2 = materialManager->getMaterial("trt::CO2"); //0.001842 CLHEP::g/CLHEP::cm3
  GeoMaterial *matKapton = materialManager->getMaterial("std::Kapton"); // 1.42*CLHEP::gram/CLHEP::cm3
  GeoMaterial *matC3F8 = materialManager->getMaterial("std::C3F8"); //1.032*CLHEP::gram/CLHEP::cm3

  // Get required elements
  GeoElement* silicon = materialManager->getElement("Silicon");
  GeoElement* oxygen = materialManager->getElement("Oxygen");
  GeoElement* hydrogen = materialManager->getElement("Hydrogen");
  GeoElement* carbon = materialManager->getElement("Carbon");
  GeoElement* copper = materialManager->getElement("Copper");
  GeoElement* nitrogen = materialManager->getElement("Nitrogen"); 
  GeoElement* fluorine = materialManager->getElement("Fluorine");

  // Define some custom materials - That will move to the GeomDB

  
  //C6F14
  GeoMaterial *matC6F14 = new GeoMaterial("C6F14",1.68*CLHEP::gram/CLHEP::cm3);
  matC6F14->add(carbon,   6*carbon->getA()   / (6*carbon->getA() + 14*fluorine->getA()));
  matC6F14->add(fluorine, 14*fluorine->getA() / (6*carbon->getA() + 14*fluorine->getA()));
  matC6F14->lock();
    
  //Water
  GeoMaterial *matWater = new GeoMaterial("Water", 1*CLHEP::gram/CLHEP::cm3);
  matWater->add(hydrogen,     2*hydrogen->getA()   / (2*hydrogen->getA() + 1*oxygen->getA()));
  matWater->add(oxygen,       1*oxygen->getA()     / (2*hydrogen->getA() + 1*oxygen->getA()));
  matWater->lock();

  //Nitrogen
  GeoMaterial *matN2 = new GeoMaterial("N2", 0.0012506*CLHEP::gram/CLHEP::cm3);
  matN2->add(nitrogen,1);
  matN2->lock();

  //Fiberglass
  GeoMaterial *matFiberglass = new GeoMaterial("SiO2",2.20*CLHEP::gram/CLHEP::cm3);
  matFiberglass->add(silicon,silicon->getA()/(silicon->getA()+2*oxygen->getA()));
  matFiberglass->add(oxygen,2*oxygen->getA()/(silicon->getA()+2*oxygen->getA()));
  matFiberglass->lock();

  //Epoxy Resin
  GeoMaterial *matEpoxyResin = new GeoMaterial("Epoxy:C8H14O4Si", 1.9*CLHEP::gram/CLHEP::cm3);
  matEpoxyResin->add(hydrogen,     14*hydrogen->getA()   / (14*hydrogen->getA() + 4*oxygen->getA()+ 8*carbon->getA()+ 1*silicon->getA()));
  matEpoxyResin->add(oxygen,        4*oxygen->getA()     / (14*hydrogen->getA() + 4*oxygen->getA()+ 8*carbon->getA()+ 1*silicon->getA()));
  matEpoxyResin->add(carbon,        8*carbon->getA()     / (14*hydrogen->getA() + 4*oxygen->getA()+ 8*carbon->getA()+ 1*silicon->getA()));
  matEpoxyResin->add(silicon,       1*silicon->getA()    / (14*hydrogen->getA() + 4*oxygen->getA()+ 8*carbon->getA()+ 1*silicon->getA()));
  matEpoxyResin->lock();

  //FEBoards
  GeoMaterial *matFEBoards = new GeoMaterial("FEBoards", 4.03*CLHEP::gram/CLHEP::cm3);
  matFEBoards->add(matFiberglass, 0.52);
  matFEBoards->add(copper, 0.28);
  matFEBoards->add(matEpoxyResin, 0.20);
  matFEBoards->lock();

  //BoardsEnvelope (FEBoards + Cooling Plates + Water + Air)
  GeoMaterial* matBoardsEnvelope = new GeoMaterial("BoardsEnvelope", 0.932*CLHEP::gram/CLHEP::cm3);
  matBoardsEnvelope->add(matFEBoards, 0.4147);
  matBoardsEnvelope->add(matWater, 0.0736);
  matBoardsEnvelope->add(air, 0.0008);
  matBoardsEnvelope->add(alu,0.5109 );
  matBoardsEnvelope->lock();
  
  //InDetServices !!! Provisoire !!!
  double density1 = 1.*CLHEP::gram/CLHEP::cm3;
  if (strDMTopTag=="LArBarrelDM-02") density1 = 1.7*CLHEP::gram/CLHEP::cm3;
  GeoMaterial* matLArServices1 = new GeoMaterial("LArServices1", density1);
  matLArServices1->add(copper, .60);
  matLArServices1->add(shieldSteel, .05);
  matLArServices1->add(matKapton, .20);
  matLArServices1->add(matC3F8, .051);
  matLArServices1->add(matC6F14, .051);
  matLArServices1->add(air, .016);
  matLArServices1->add(matN2, .016);
  matLArServices1->add(matCO2, .016);
  matLArServices1->lock();

  //InDetServices !!! Provisoire !!!
  double density2 = 2.*CLHEP::gram/CLHEP::cm3;
  if (strDMTopTag=="LArBarrelDM-02") density2 = 3.4*CLHEP::gram/CLHEP::cm3;
  GeoMaterial* matLArServices2 = new GeoMaterial("LArServices2", density2);
  matLArServices2->add(copper, .60);
  matLArServices2->add(shieldSteel, .05);
  matLArServices2->add(matKapton, .20);
  matLArServices2->add(matC3F8, .051);
  matLArServices2->add(matC6F14, .051);
  matLArServices2->add(air, .016);
  matLArServices2->add(matN2, .016);
  matLArServices2->add(matCO2, .016);
  matLArServices2->lock();

//  std::cout << " -- LArServices1 density,x0,lambda " << matLArServices1->getDensity()/(g/cm3) << " "
//     << matLArServices1->getRadLength() << " " << matLArServices1->getIntLength() << std::endl;
//  std::cout << " -- LArServices2 density,x0,lambda " << matLArServices2->getDensity()/(g/cm3) << " "
//     << matLArServices2->getRadLength() << " " << matLArServices2->getIntLength() << std::endl;
  
  const unsigned int NCrates=16;
  const double Alfa=360*CLHEP::deg/NCrates;
  const double Enda=1155;
  const double Endb=1695.2;
  const double Endc=2771.6;
  const double DYa=1155*tan(Alfa/2);
  const double DYb=1695.2*tan(Alfa/2);
  const double DYc=2771.6*tan(Alfa/2);
  Variable       i;
  GENFUNCTION    f = Alfa*i;
 
  ////----------- Building Front-end crates --------------------


  // ----- build pedestal -----
  GeoBox     *Pedestal = new GeoBox(71, 383.18, 248.65);
  GeoBox     *Ped1     = new GeoBox(67, 380.18, 245.65);
  GeoTube    *Ped2     = new GeoTube(0, 150, 75);
  GeoTube    *Ped3     = new GeoTube(0, 2775, 300);  
  const GeoShape & CratePed=((*Pedestal).subtract(*Ped1).
			     subtract((*Ped2)  <<HepGeom::TranslateY3D(-200.025)*HepGeom::RotateY3D(90*CLHEP::deg)).
			     subtract((*Ped3)  <<HepGeom::TranslateX3D(-2800)).
		             subtract((*Ped2)  <<HepGeom::TranslateY3D(200.025)*HepGeom::RotateY3D(90*CLHEP::deg)));
  
  GeoLogVol  *lvped   = new GeoLogVol("LAr::DM::Ped",&CratePed,alu);
  GeoPhysVol *pedestal   = new GeoPhysVol(lvped);
			
  // ----- build crates -----
  GeoBox     *Crate1   = new GeoBox(244.5, 400.05, 255.05);
  GeoBox     *Crate2   = new GeoBox(250, 396.87, 245.55);
  GeoBox     *Crate3   = new GeoBox(186.5, 3.175, 245.55);
  const GeoShape & FEBCrate=(*Crate1).subtract(*Crate2).add((*Crate3)  <<HepGeom::TranslateX3D(-6.7));
  
  GeoLogVol  *lvcrate = new GeoLogVol("LAr::DM::Crate",&FEBCrate,alu);
  GeoPhysVol *crate   = new GeoPhysVol(lvcrate);
  
  // ----- build boardenvelopes -----
  GeoBox     *BoardEnvelope   = new GeoBox(204, 194, 244);
  GeoLogVol  *lvbenv = new GeoLogVol("LAr::DM::FEBoard",BoardEnvelope,matBoardsEnvelope);
  GeoPhysVol *boardenvelope   = new GeoPhysVol(lvbenv);
  
   
  //-------------- Place volumes in envelope ----------------------------

  //Crates
  TRANSFUNCTION crA = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(3141.25)*HepGeom::TranslateZ3D(3135.05); 
  TRANSFUNCTION crC = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(3141.25)*HepGeom::TranslateZ3D(-3135.05);
  GeoSerialTransformer *crtA = new GeoSerialTransformer(crate,&crA, NCrates);
  GeoSerialTransformer *crtC = new GeoSerialTransformer(crate,&crC, NCrates);
  envelope->add(crtA);
  envelope->add(crtC);
  
  //Pedestals
  TRANSFUNCTION pedA = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(2825.75)*HepGeom::TranslateZ3D(3135.05); 
  TRANSFUNCTION pedC = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(2825.75)*HepGeom::TranslateZ3D(-3135.05);
  GeoSerialTransformer *pedtA = new GeoSerialTransformer(pedestal,&pedA, NCrates);
  GeoSerialTransformer *pedtC = new GeoSerialTransformer(pedestal,&pedC, NCrates);
  envelope->add(pedtA);
  envelope->add(pedtC);

  //FEBoards
  TRANSFUNCTION feb1A = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(200.25)*HepGeom::TranslateX3D(3181.25)*HepGeom::TranslateZ3D(3135.05);
  TRANSFUNCTION feb2A = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(-200.25)*HepGeom::TranslateX3D(3181.25)*HepGeom::TranslateZ3D(3135.05);
  TRANSFUNCTION feb1C = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(200.25)*HepGeom::TranslateX3D(3181.25)*HepGeom::TranslateZ3D(-3135.05);
  TRANSFUNCTION feb2C = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(-200.25)*HepGeom::TranslateX3D(3181.25)*HepGeom::TranslateZ3D(-3135.05);
  GeoSerialTransformer *febt1A = new GeoSerialTransformer(boardenvelope,&feb1A, NCrates);
  GeoSerialTransformer *febt1C = new GeoSerialTransformer(boardenvelope,&feb1C, NCrates);
  GeoSerialTransformer *febt2A = new GeoSerialTransformer(boardenvelope,&feb2A, NCrates);
  GeoSerialTransformer *febt2C = new GeoSerialTransformer(boardenvelope,&feb2C, NCrates);
  envelope->add(febt1A);
  envelope->add(febt1C);
  envelope->add(febt2A);
  envelope->add(febt2C);
  
  ////----------- Building envelopes and support plates for Cables and Tubes --------------
 
  // transforms
  GeoBox   *Box   = new GeoBox(280, 280, 100);
 
  HepGeom::Transform3D Cut3Boxe  = HepGeom::Translate3D(0, 548, 711)*HepGeom::RotateX3D(-20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
  HepGeom::Transform3D Cut4Boxe  = HepGeom::Translate3D(0, -548,711)*HepGeom::RotateX3D(20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
  HepGeom::Transform3D Cut3Boxp  = HepGeom::Translate3D(0, 548, 850)*HepGeom::RotateX3D(-20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
  HepGeom::Transform3D Cut4Boxp  = HepGeom::Translate3D(0, -548,850)*HepGeom::RotateX3D(20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);

  // ----- build base envelopes -----
  GeoTrd   *Trd1air  = new GeoTrd(123.5, 123.5, 167, 305, 287.5); 
  GeoLogVol  *lvbe          = new GeoLogVol("LAr::DM::BaseEnvelopes",Trd1air,matLArServices1);
  GeoPhysVol *baseenvelopes    = new GeoPhysVol(lvbe);

  // ----- build bridge envelopes -----
  GeoTrap  *Trapair  = new GeoTrap(201.70, 45.35*CLHEP::deg, 0, 160, 52.95, 52.95, 0, 160, 123.5, 123.5, 0);
  GeoLogVol  *lvbre          = new GeoLogVol("LAr::DM::BridgeEnvelopes",Trapair,matLArServices1);
  GeoPhysVol *bridgeenvelopes    = new GeoPhysVol(lvbre);

  // ----- build sector envelopes -----
  GeoTrd   *Trdair1  = new GeoTrd(52.95, 52.95, DYa, DYb, (Endb-Enda)/2);// (52.95, 52.95, 229.7, 335.83, 269.6)
  GeoLogVol  *lvse1          = new GeoLogVol("LAr::DM::SectorEnvelopes1",Trdair1,matLArServices2);
  GeoPhysVol *sectorenvelopes1    = new GeoPhysVol(lvse1); 


  GeoTrd   *Trdair2  = new GeoTrd(52.95, 52.95, DYb, DYc, (Endc-Endb)/2);//(52.95, 52.95, 335.83, 548.5, 538.2)
  const GeoShape & SectorEnvelope= ((*Trdair2).
				     subtract((*Box)  <<HepGeom::Transform3D(Cut3Boxe)).
				     subtract((*Box)  <<HepGeom::Transform3D(Cut4Boxe)));

  const GeoShape & SectorEnvelopes= ((SectorEnvelope).
                                    add(SectorEnvelope  << HepGeom::TranslateY3D(-(DYb+DYc)*cos(Alfa/2)*cos(Alfa/2))*HepGeom::TranslateZ3D(-(DYb+DYc)*0.5*sin(Alfa))*HepGeom::RotateX3D(Alfa))); 

  GeoLogVol  *lvse2r          = new GeoLogVol("LAr::DM::SectorEnvelopes2r",&SectorEnvelopes,matLArServices1);
  GeoPhysVol *sectorenvelopes2r    = new GeoPhysVol(lvse2r);  // for right-handed splice boxes sideA
  
  GeoLogVol  *lvse2l          = new GeoLogVol("LAr::DM::SectorEnvelopes2l",&SectorEnvelopes,matLArServices1);
  GeoPhysVol *sectorenvelopes2l    = new GeoPhysVol(lvse2l);  // for left-handed splice boxes
  
  GeoLogVol  *lvse2          = new GeoLogVol("LAr::DM::SectorEnvelopes2",&SectorEnvelopes,matLArServices1);
  GeoPhysVol *sectorenvelopes2    = new GeoPhysVol(lvse2);  // no splice boxes

  // ----- build base plates -----
  GeoTrd   *Trd1alu  = new GeoTrd(5, 5, 167, 305, 287.5);
  GeoLogVol  *lvbp          = new GeoLogVol("LAr::DM::BasePlates",Trd1alu,alu);
  GeoPhysVol *baseplates    = new GeoPhysVol(lvbp);

  // ----- build bridge plates -----
  GeoTrap  *Trapalu  = new GeoTrap(201.70, 49.92*CLHEP::deg, 0, 160, 5, 5, 0, 160, 5, 5, 0); 
  GeoLogVol  *lvbrp          = new GeoLogVol("LAr::DM::BridgePlates",Trapalu,alu);
  GeoPhysVol *bridgeplates    = new GeoPhysVol(lvbrp);
  

  // ----- build sector plates -----
  GeoTrd   *Trd2alu  = new GeoTrd(5, 5, 280, 548, 677.5);
  const GeoShape & SectorPlates= ((*Trd2alu).
                                   subtract((*Box)  <<HepGeom::Transform3D(Cut3Boxp)).
				   subtract((*Box)  <<HepGeom::Transform3D(Cut4Boxp)));
  GeoLogVol  *lvsp          = new GeoLogVol("LAr::DM::SectorPlates",&SectorPlates,alu);
  GeoPhysVol *sectorplates    = new GeoPhysVol(lvsp);

  //---------- Build Splice boxes for InDet optical fibers--------
   
  GeoTrap  *GeoTrap1  = new GeoTrap(237.5, 0, 0, 307, 47.5, 47.5, 0, 259.17, 47.5, 47.5, 0);
  GeoBox   *Box1   = new GeoBox(50, 244.80, 150);  
  const GeoShape & SpliceBox = ((*GeoTrap1).
				subtract(*Box1 << HepGeom::TranslateZ3D(193.88)*HepGeom::TranslateY3D(-223.49)*HepGeom::RotateX3D(41.592*CLHEP::deg)));

  GeoTransform *xtr = new GeoTransform (HepGeom::TranslateZ3D(39.57)*HepGeom::TranslateY3D(-452.12)*HepGeom::TranslateX3D(5.40)*HepGeom::RotateX3D(191.25*CLHEP::deg));
  sectorenvelopes2r->add(xtr);
  GeoLogVol  *lvspbr     = new GeoLogVol("LAr::DM::SPliceBoxr",&SpliceBox,alu); 
  GeoPhysVol *spliceboxr       = new GeoPhysVol(lvspbr);
  sectorenvelopes2r->add(spliceboxr);
  
  GeoTransform *xtl = new GeoTransform (HepGeom::TranslateZ3D(39.57)*HepGeom::TranslateY3D(-452.12)*HepGeom::TranslateX3D(5.40)*HepGeom::RotateY3D(-180*CLHEP::deg)*HepGeom::RotateX3D(-(Alfa/2)));
  sectorenvelopes2l->add(xtl);
  GeoLogVol  *lvspbl     = new GeoLogVol("LAr::DM::SpliceBoxl",&SpliceBox,alu);  
  GeoPhysVol *spliceboxl       = new GeoPhysVol(lvspbl);
  sectorenvelopes2l->add(spliceboxl);
  
  
  ////
  GeoTrd   *Trd1  = new GeoTrd(44.5, 44.5, 0, 214.72, 94.20);
  GeoTrap  *GeoTrap2  = new GeoTrap(149, 0, 0, 126.215, 44.5, 44.5, 0, 95, 44.5, 44.5, 0);
  GeoTrap  *GeoTrap3  = new GeoTrap(72, 0, 0, 294.5, 44.5, 44.5, 0, 279.396, 44.5, 44.5, 0);
  
  GeoTransform *xt1 = new GeoTransform (HepGeom::TranslateY3D(-53)*HepGeom::RotateX3D(42.25*CLHEP::deg));
  spliceboxr->add(xt1);
  spliceboxl->add(xt1);
  GeoLogVol  *lt1     = new GeoLogVol("LAr::DM::TBox1",Trd1,air);
  GeoPhysVol *tbox1       = new GeoPhysVol(lt1);
  spliceboxr->add(tbox1);
  spliceboxl->add(tbox1);
  
  GeoTransform *xt2 = new GeoTransform (HepGeom::TranslateZ3D(78)*HepGeom::TranslateY3D(154));
  spliceboxr->add(xt2);
  spliceboxl->add(xt2);
  GeoLogVol  *lt2     = new GeoLogVol("LAr::DM::TBox2",GeoTrap2,air);
  GeoPhysVol *tbox2       = new GeoPhysVol(lt2);
  spliceboxr->add(tbox2);
  spliceboxl->add(tbox2);
  
  GeoTransform *xt3 = new GeoTransform (HepGeom::TranslateZ3D(-155.81)); 
  spliceboxr->add(xt3);
  spliceboxl->add(xt3);
  GeoLogVol  *lt3     = new GeoLogVol("LAr::DM::TBox3",GeoTrap3,air);
  GeoPhysVol *tbox3       = new GeoPhysVol(lt3);
  spliceboxr->add(tbox3);
  spliceboxl->add(tbox3);


  //-------------- Place volumes in LAr Envelope -------------------
 
  //sectorPlates
  TRANSFUNCTION spA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(2095)*HepGeom::TranslateZ3D(3410.1)*HepGeom::RotateY3D(90*CLHEP::deg);
  TRANSFUNCTION spC = Pow(HepGeom::RotateZ3D(1.0),f+(Alfa/2))*HepGeom::TranslateX3D(2095)*HepGeom::TranslateZ3D(-3410.1)*HepGeom::RotateY3D(90*CLHEP::deg);
  GeoSerialTransformer *sptA = new GeoSerialTransformer(sectorplates,&spA, NCrates);
  GeoSerialTransformer *sptC = new GeoSerialTransformer(sectorplates,&spC, NCrates);
  envelope->add(sptA);
  envelope->add(sptC);

  //bridgePlates
  TRANSFUNCTION brpA1 = Pow(HepGeom::RotateZ3D(1.0),f-(5*Alfa/2))*HepGeom::TranslateX3D(2974.5)*HepGeom::TranslateZ3D(3170.1)*HepGeom::RotateZ3D(90*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg)*HepGeom::RotateX3D(90*CLHEP::deg);
  TRANSFUNCTION brpA2 = Pow(HepGeom::RotateZ3D(1.0),f+(13*Alfa/2))*HepGeom::TranslateX3D(2974.5)*HepGeom::TranslateZ3D(3170.1)*HepGeom::RotateZ3D(90*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg)*HepGeom::RotateX3D(90*CLHEP::deg); 
  TRANSFUNCTION brpC1 = Pow(HepGeom::RotateZ3D(1.0),f-(5*Alfa/2))*HepGeom::TranslateX3D(2974.5)*HepGeom::TranslateZ3D(-3170.1)*HepGeom::RotateZ3D(-90*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(-90*CLHEP::deg);
  TRANSFUNCTION brpC2 = Pow(HepGeom::RotateZ3D(1.0),f+(13*Alfa/2))*HepGeom::TranslateX3D(2974.5)*HepGeom::TranslateZ3D(-3170.1)*HepGeom::RotateZ3D(-90*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(-90*CLHEP::deg);   GeoSerialTransformer *brptA1 = new GeoSerialTransformer(bridgeplates,&brpA1, 5);
  GeoSerialTransformer *brptA2 = new GeoSerialTransformer(bridgeplates,&brpA2, 5);
  GeoSerialTransformer *brptC1 = new GeoSerialTransformer(bridgeplates,&brpC1, 5);
  GeoSerialTransformer *brptC2 = new GeoSerialTransformer(bridgeplates,&brpC2, 5);
  envelope->add(brptA1);
  envelope->add(brptA2);
  envelope->add(brptC1);
  envelope->add(brptC2);

  //basePlates
  TRANSFUNCTION bpA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(3464)*HepGeom::TranslateZ3D(2930.6)*HepGeom::RotateY3D(90*CLHEP::deg); 
  TRANSFUNCTION bpC = Pow(HepGeom::RotateZ3D(1.0),f+(Alfa/2))*HepGeom::TranslateX3D(3464)*HepGeom::TranslateZ3D(-2930.6)*HepGeom::RotateY3D(90*CLHEP::deg);
  GeoSerialTransformer *bptA = new GeoSerialTransformer(baseplates,&bpA, NCrates);
  GeoSerialTransformer *bptC = new GeoSerialTransformer(baseplates,&bpC, NCrates);
  envelope->add(bptA);
  envelope->add(bptC);

  //sectorEnvelopes
  TRANSFUNCTION seA1 = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  TRANSFUNCTION seC1 = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  GeoSerialTransformer *setA1 = new GeoSerialTransformer(sectorenvelopes1,&seA1, NCrates);
  GeoSerialTransformer *setC1 = new GeoSerialTransformer(sectorenvelopes1,&seC1, NCrates);
  envelope->add(setA1); 
  envelope->add(setC1);
  
  TRANSFUNCTION seA2r = Pow(HepGeom::RotateZ3D(1.0),8*f-(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  TRANSFUNCTION seA2l = Pow(HepGeom::RotateZ3D(1.0),8*f+(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  TRANSFUNCTION seA2 = Pow(HepGeom::RotateZ3D(1.0),4*f+(Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  TRANSFUNCTION seC2 = Pow(HepGeom::RotateZ3D(1.0),4*f+(Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  TRANSFUNCTION seC2r = Pow(HepGeom::RotateZ3D(1.0),8*f-(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  TRANSFUNCTION seC2l = Pow(HepGeom::RotateZ3D(1.0),8*f+(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-3468.05)*HepGeom::RotateY3D(90*CLHEP::deg);
  GeoSerialTransformer *setA2r = new GeoSerialTransformer(sectorenvelopes2r,&seA2r, 2);
  GeoSerialTransformer *setA2l = new GeoSerialTransformer(sectorenvelopes2l,&seA2l, 2);
  GeoSerialTransformer *setA2 = new GeoSerialTransformer(sectorenvelopes2,&seA2, 4);
  GeoSerialTransformer *setC2 = new GeoSerialTransformer(sectorenvelopes2,&seC2, 4);
  GeoSerialTransformer *setC2r = new GeoSerialTransformer(sectorenvelopes2r,&seC2r, 2);
  GeoSerialTransformer *setC2l = new GeoSerialTransformer(sectorenvelopes2l,&seC2l, 2);
  envelope->add(setA2r);
  envelope->add(setA2l);
  envelope->add(setA2);
  envelope->add(setC2);
  envelope->add(setC2r);
  envelope->add(setC2l);

  //bridgeEnvelopes
  TRANSFUNCTION breA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(2974.532)*HepGeom::TranslateZ3D(3263.65)*HepGeom::RotateZ3D(90*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg)*HepGeom::RotateX3D(90*CLHEP::deg);
  TRANSFUNCTION breC = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(2974.532)*HepGeom::TranslateZ3D(-3263.65)*HepGeom::RotateZ3D(-90*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(-90*CLHEP::deg);
  GeoSerialTransformer *bretA = new GeoSerialTransformer(bridgeenvelopes,&breA, NCrates);
  GeoSerialTransformer *bretC = new GeoSerialTransformer(bridgeenvelopes,&breC, NCrates);
  envelope->add(bretA);
  envelope->add(bretC);

  //baseEnvelopes
  TRANSFUNCTION beA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(3464)*HepGeom::TranslateZ3D(3059.2)*HepGeom::RotateY3D(90*CLHEP::deg); 
  TRANSFUNCTION beC = Pow(HepGeom::RotateZ3D(1.0),f+(Alfa/2))*HepGeom::TranslateX3D(3464)*HepGeom::TranslateZ3D(-3059.2)*HepGeom::RotateY3D(90*CLHEP::deg);
  GeoSerialTransformer *betA = new GeoSerialTransformer(baseenvelopes,&beA, NCrates);
  GeoSerialTransformer *betC = new GeoSerialTransformer(baseenvelopes,&beC, NCrates);
  envelope->add(betA);
  envelope->add(betC);

  
  // GeoMaterial::add(GeoMaterial*) does not take ownership of the
  // material passed as a parameter.  Thus, something like
  //   mat1* = new GeoMaterial (...);
  //   ...
  //   mat2->add (mat1);
  // will leak mat1.
  // Make sure such materials get deleted.
  // Do a ref/unref just in case something changes above and they
  // end up owned by someone.
  matFEBoards->ref();    matFEBoards->unref();
  matC6F14->ref();       matC6F14->unref();
  matWater->ref();       matWater->unref();
  matEpoxyResin->ref();  matEpoxyResin->unref();
  matFiberglass->ref();  matFiberglass->unref();
  matN2->ref();          matN2->unref();
  } 
  else
  {
    // Use Geometry Database
    DecodeVersionKey keyLAr(geoModel,"LAr");
    IRDBRecordset_ptr BarrelDMTraps = rdbAccess->getRecordsetPtr("BarrelDMTraps",keyLAr.tag(),keyLAr.node());
    IRDBRecordset_ptr BarrelDMTrds = rdbAccess->getRecordsetPtr("BarrelDMTrds",keyLAr.tag(),keyLAr.node());
    IRDBRecordset_ptr BarrelDMTubes = rdbAccess->getRecordsetPtr("BarrelDMTubes",keyLAr.tag(),keyLAr.node());
    IRDBRecordset_ptr BarrelDMBoxes = rdbAccess->getRecordsetPtr("BarrelDMBoxes",keyLAr.tag(),keyLAr.node());

    std::map<std::string, unsigned int> tubeMap;  
    for (unsigned int i=0; i<BarrelDMTubes->size(); i++)     
    {
      std::string key = (*BarrelDMTubes)[i]->getString("TUBENAME");
      tubeMap[key] = i;
    }
    std::map<std::string, unsigned int> boxMap;  
    for (unsigned int j=0; j<BarrelDMBoxes->size(); j++)     
    {
      std::string key = (*BarrelDMBoxes)[j]->getString("BOXNAME");
      boxMap[key] = j;
    }
    std::map<std::string, unsigned int> trdMap;  
    for (unsigned int k=0; k<BarrelDMTrds->size(); k++)     
    {
      std::string key = (*BarrelDMTrds)[k]->getString("TRDNAME");
      trdMap[key] = k;
    }
    std::map<std::string, unsigned int> trapMap;  
    for (unsigned int l=0; l<BarrelDMTraps->size(); l++)     
    {
      std::string key = (*BarrelDMTraps)[l]->getString("TRAPNAME");
      trapMap[key] = l;
    }

    unsigned int recordIndex;
    
    // Get materials
    GeoMaterial *air               = materialManager->getMaterial("std::Air"); //0.001214 CLHEP::g/CLHEP::cm3
    GeoMaterial *alu               = materialManager->getMaterial("std::Aluminium"); //2.7 CLHEP::g/CLHEP::cm3
    GeoMaterial* matBoardsEnvelope = materialManager->getMaterial("LAr::BoardsEnvelope");// 0.932*CLHEP::gram/CLHEP::cm3);
    GeoMaterial* matLArServices1   = materialManager->getMaterial("LAr::LArServices1");// 1.020*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices2   = materialManager->getMaterial("LAr::LArServices2");// 0.955*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices3   = materialManager->getMaterial("LAr::LArServices3");// 1.005*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices4   = materialManager->getMaterial("LAr::LArServices4");// 0.460*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices5   = materialManager->getMaterial("LAr::LArServices5");// 0.480*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices6   = materialManager->getMaterial("LAr::LArServices6");// 1.000*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices7   = materialManager->getMaterial("LAr::LArServices7");// 0.935*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices8   = materialManager->getMaterial("LAr::LArServices8");// 1.070*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices9   = materialManager->getMaterial("LAr::LArServices9");// 1.020*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices10  = materialManager->getMaterial("LAr::LArServices10");// 0.995*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices11  = materialManager->getMaterial("LAr::LArServices11");// 0.835*CLHEP::gram/CLHEP::cm3 
    GeoMaterial* matLArServices12  = materialManager->getMaterial("LAr::LArServices12");// 0.640*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices13  = materialManager->getMaterial("LAr::LArServices13");// 0.690*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices14  = materialManager->getMaterial("LAr::LArServices14");// 0.825*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices15  = materialManager->getMaterial("LAr::LArServices15");// 0.875*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices16  = materialManager->getMaterial("LAr::LArServices16");// 1.035*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices17  = materialManager->getMaterial("LAr::LArServices17");// 0.035*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices18  = materialManager->getMaterial("LAr::LArServices18");// 0.240*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices19  = materialManager->getMaterial("LAr::LArServices19");// 0.469*CLHEP::gram/CLHEP::cm3
    GeoMaterial* matLArServices20  = materialManager->getMaterial("LAr::LArServices20");// 0.353*CLHEP::gram/CLHEP::cm3

    const unsigned int NCrates=16;
    const double Alfa=360*CLHEP::deg/NCrates;
    const double Enda=1155;
    const double Endb=1695.2;
    const double Endc=2771.6;
    const double inv_Endab = 1. / (Endb - Enda);
    const double DYa=1155*tan(Alfa/2);
    const double DYb=1695.2*tan(Alfa/2);
    const double DYc=2771.6*tan(Alfa/2);
    Variable       i;
    GENFUNCTION    f = Alfa*i;
    
    ////----------- Building Front-end crates --------------------

    recordIndex = tubeMap["Ped2"];
    double ped2zhlen = (*BarrelDMTubes)[recordIndex]->getDouble("ZHLEN");
    double ped2minr = (*BarrelDMTubes)[recordIndex]->getDouble("MINR");
    double ped2maxr = (*BarrelDMTubes)[recordIndex]->getDouble("MAXR");
    double ped2ytr = (*BarrelDMTubes)[recordIndex]->getDouble("YTR");
    recordIndex = tubeMap["Ped3"];
    double ped3zhlen = (*BarrelDMTubes)[recordIndex]->getDouble("ZHLEN");
    double ped3minr = (*BarrelDMTubes)[recordIndex]->getDouble("MINR");
    double ped3maxr = (*BarrelDMTubes)[recordIndex]->getDouble("MAXR");
    double ped3xtr = (*BarrelDMTubes)[recordIndex]->getDouble("XTR");
    
    recordIndex = boxMap["Pedest"];
    double pedesthlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double pedesthwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double pedesthhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    double pedestxtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
    double pedestztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");
    recordIndex = boxMap["Ped1"];
    double ped1hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double ped1hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double ped1hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    recordIndex = boxMap["Crate1"];
    double crate1hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double crate1hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double crate1hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    double crate1xtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
    double crate1ztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");
    recordIndex = boxMap["Crate2"];
    double crate2hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double crate2hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double crate2hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    recordIndex = boxMap["Crate3"];
    double crate3hlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double crate3hwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double crate3hhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    double crate3xtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
    recordIndex = boxMap["BoardE"];
    double BoardEhlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double BoardEhwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double BoardEhhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    double BoardExtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
    double BoardEytr = (*BarrelDMBoxes)[recordIndex]->getDouble("YTR");
    double BoardEztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");
    recordIndex = boxMap["Box"];
    double Boxhlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double Boxhwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double Boxhhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    double Boxxtr = (*BarrelDMBoxes)[recordIndex]->getDouble("XTR");
    double Boxytr = (*BarrelDMBoxes)[recordIndex]->getDouble("YTR");
    double Boxztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");
    double Boxxrot = (*BarrelDMBoxes)[recordIndex]->getDouble("XROT");
    recordIndex = boxMap["SplBox"];
    double SplBoxhlen = (*BarrelDMBoxes)[recordIndex]->getDouble("HLEN");
    double SplBoxhwdt = (*BarrelDMBoxes)[recordIndex]->getDouble("HWDT");
    double SplBoxhhgt = (*BarrelDMBoxes)[recordIndex]->getDouble("HHGT");
    double SplBoxytr = (*BarrelDMBoxes)[recordIndex]->getDouble("YTR");
    double SplBoxztr = (*BarrelDMBoxes)[recordIndex]->getDouble("ZTR");
    double SplBoxxrot = (*BarrelDMBoxes)[recordIndex]->getDouble("XROT");
  
    // ----- build pedestal -----
    GeoBox     *Pedestal = new GeoBox(pedesthlen, pedesthwdt, pedesthhgt);
    GeoBox     *Ped1     = new GeoBox(ped1hlen, ped1hwdt, ped1hhgt);
    GeoTube    *Ped2     = new GeoTube(ped2minr, ped2maxr, ped2zhlen);
    GeoTube    *Ped3     = new GeoTube(ped3minr,ped3maxr , ped3zhlen);  
    const GeoShape & CratePed=((*Pedestal).subtract(*Ped1).
			       subtract((*Ped2)  <<HepGeom::TranslateY3D(-ped2ytr)*HepGeom::RotateY3D(90*CLHEP::deg)).
			       subtract((*Ped3)  <<HepGeom::TranslateX3D(-ped3xtr)).
			       subtract((*Ped2)  <<HepGeom::TranslateY3D(ped2ytr)*HepGeom::RotateY3D(90*CLHEP::deg)));
  
    GeoLogVol  *lvped   = new GeoLogVol("LAr::DM::Ped",&CratePed,alu);
    GeoPhysVol *pedestal   = new GeoPhysVol(lvped);
    
    // ----- build crates -----
    GeoBox     *Crate1   = new GeoBox(crate1hlen, crate1hwdt, crate1hhgt);
    GeoBox     *Crate2   = new GeoBox(crate2hlen, crate2hwdt, crate2hhgt);
    GeoBox     *Crate3   = new GeoBox(crate3hlen, crate3hwdt, crate3hhgt);
    const GeoShape & FEBCrate=(*Crate1).subtract(*Crate2).add((*Crate3)  <<HepGeom::TranslateX3D(-crate3xtr));
    
    GeoLogVol  *lvcrate = new GeoLogVol("LAr::DM::Crate",&FEBCrate,alu);
    GeoPhysVol *crate   = new GeoPhysVol(lvcrate);
    
    // ----- build boardenvelopes -----
    GeoBox     *BoardEnvelope   = new GeoBox(BoardEhlen, BoardEhwdt, BoardEhhgt);
    GeoLogVol  *lvbenv = new GeoLogVol("LAr::DM::FEBoard",BoardEnvelope,matBoardsEnvelope);
    GeoPhysVol *boardenvelope   = new GeoPhysVol(lvbenv);
    
    //-------------- Place volumes in envelope ----------------------------
    
    //Crates
    TRANSFUNCTION crA = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(crate1xtr)*HepGeom::TranslateZ3D(crate1ztr); 
    TRANSFUNCTION crC = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(crate1xtr)*HepGeom::TranslateZ3D(-crate1ztr);
    GeoSerialTransformer *crtA = new GeoSerialTransformer(crate,&crA, NCrates);
    GeoSerialTransformer *crtC = new GeoSerialTransformer(crate,&crC, NCrates);
    envelope->add(crtA);
    envelope->add(crtC);
    
    //Pedestals
    TRANSFUNCTION pedA = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(pedestxtr)*HepGeom::TranslateZ3D(pedestztr); 
    TRANSFUNCTION pedC = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateX3D(pedestxtr)*HepGeom::TranslateZ3D(-pedestztr);
    GeoSerialTransformer *pedtA = new GeoSerialTransformer(pedestal,&pedA, NCrates);
    GeoSerialTransformer *pedtC = new GeoSerialTransformer(pedestal,&pedC, NCrates);
    envelope->add(pedtA);
    envelope->add(pedtC);
    
    //FEBoards
    TRANSFUNCTION feb1A = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(BoardEytr)*HepGeom::TranslateX3D(BoardExtr)*HepGeom::TranslateZ3D(BoardEztr);
    TRANSFUNCTION feb2A = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(-BoardEytr)*HepGeom::TranslateX3D(BoardExtr)*HepGeom::TranslateZ3D(BoardEztr);
    TRANSFUNCTION feb1C = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(BoardEytr)*HepGeom::TranslateX3D(BoardExtr)*HepGeom::TranslateZ3D(-BoardEztr);
    TRANSFUNCTION feb2C = Pow(HepGeom::RotateZ3D(1.0),f)*HepGeom::TranslateY3D(-BoardEytr)*HepGeom::TranslateX3D(BoardExtr)*HepGeom::TranslateZ3D(-BoardEztr);
    GeoSerialTransformer *febt1A = new GeoSerialTransformer(boardenvelope,&feb1A, NCrates);
    GeoSerialTransformer *febt1C = new GeoSerialTransformer(boardenvelope,&feb1C, NCrates);
    GeoSerialTransformer *febt2A = new GeoSerialTransformer(boardenvelope,&feb2A, NCrates);
    GeoSerialTransformer *febt2C = new GeoSerialTransformer(boardenvelope,&feb2C, NCrates);
    envelope->add(febt1A);
    envelope->add(febt1C);
    envelope->add(febt2A);
    envelope->add(febt2C);
    
    ////----------- Building envelopes and support plates for Cables and Tubes --------------
    recordIndex = trdMap["SecP"];
    double SecPxhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN1");
    double SecPxhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN2");
    double SecPyhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN1");//
    double SecPyhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN2");//
    double SecPzhlen = (*BarrelDMTrds)[recordIndex]->getDouble("ZHLEN");//
    double SecPxtr = (*BarrelDMTrds)[recordIndex]->getDouble("XTR");
    double SecPztr = (*BarrelDMTrds)[recordIndex]->getDouble("ZTR");
    recordIndex = trdMap["BaseP"];
    double BasePxhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN1");
    double BasePxhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN2");
    double BasePyhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN1");
    double BasePyhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN2");
    double BasePzhlen = (*BarrelDMTrds)[recordIndex]->getDouble("ZHLEN");
    double BasePxtr = (*BarrelDMTrds)[recordIndex]->getDouble("XTR");
    double BasePztr = (*BarrelDMTrds)[recordIndex]->getDouble("ZTR");
    recordIndex = trdMap["SecE1"];
    double SecE1xhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN1");
    double SecE1xhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN2");
    double SecE1ztr = (*BarrelDMTrds)[recordIndex]->getDouble("ZTR");
    recordIndex = trdMap["SecE2"];
    double SecE2xhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN1");
    double SecE2xhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN2");
    double SecE2ztr = (*BarrelDMTrds)[recordIndex]->getDouble("ZTR");
    recordIndex = trdMap["BaseE"];
    double BaseExhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN1");
    double BaseExhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN2");
    double BaseEyhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN1");
    double BaseEyhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN2");
    double BaseEzhlen = (*BarrelDMTrds)[recordIndex]->getDouble("ZHLEN");//
    double BaseExtr = (*BarrelDMTrds)[recordIndex]->getDouble("XTR");
    double BaseEztr = (*BarrelDMTrds)[recordIndex]->getDouble("ZTR");
    recordIndex = trdMap["Spb0"];
    double Spb0xhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN1");
    double Spb0xhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("XHLEN2");
    double Spb0yhlen1 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN1");
    double Spb0yhlen2 = (*BarrelDMTrds)[recordIndex]->getDouble("YHLEN2");
    double Spb0zhlen = (*BarrelDMTrds)[recordIndex]->getDouble("ZHLEN");
    double Spb0ytr = (*BarrelDMTrds)[recordIndex]->getDouble("YTR");
    double Spb0xrot = (*BarrelDMTrds)[recordIndex]->getDouble("XROT");
    recordIndex = trapMap["BridgeP"];
    double BridgePzhlen = (*BarrelDMTraps)[recordIndex]->getDouble("ZHLEN");
    double BridgePtheta = (*BarrelDMTraps)[recordIndex]->getDouble("THETA");
    double BridgePphi = (*BarrelDMTraps)[recordIndex]->getDouble("PHI");
    double BridgePyzn = (*BarrelDMTraps)[recordIndex]->getDouble("YZN");
    double BridgePxynzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZN");
    double BridgePxypzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZN");
    double BridgePangn = (*BarrelDMTraps)[recordIndex]->getDouble("ANGN");
    double BridgePyzp = (*BarrelDMTraps)[recordIndex]->getDouble("YZP");
    double BridgePxynzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZP");
    double BridgePxypzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZP");
    double BridgePangp = (*BarrelDMTraps)[recordIndex]->getDouble("ANGP");
    double BridgePxtr = (*BarrelDMTraps)[recordIndex]->getDouble("XTR");
    double BridgePztr = (*BarrelDMTraps)[recordIndex]->getDouble("ZTR");
    recordIndex = trapMap["BridgeE"];
    double BridgeEzhlen = (*BarrelDMTraps)[recordIndex]->getDouble("ZHLEN");
    double BridgeEtheta = (*BarrelDMTraps)[recordIndex]->getDouble("THETA");
    double BridgeEphi = (*BarrelDMTraps)[recordIndex]->getDouble("PHI");
    double BridgeEyzn = (*BarrelDMTraps)[recordIndex]->getDouble("YZN");
    double BridgeExynzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZN");
    double BridgeExypzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZN");
    double BridgeEangn = (*BarrelDMTraps)[recordIndex]->getDouble("ANGN");
    double BridgeEyzp = (*BarrelDMTraps)[recordIndex]->getDouble("YZP");
    double BridgeExynzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZP");
    double BridgeExypzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZP");
    double BridgeEangp = (*BarrelDMTraps)[recordIndex]->getDouble("ANGP");
    double BridgeExtr = (*BarrelDMTraps)[recordIndex]->getDouble("XTR");
    double BridgeEztr = (*BarrelDMTraps)[recordIndex]->getDouble("ZTR");
    recordIndex = trapMap["Spb1"];
    double Spb1zhlen = (*BarrelDMTraps)[recordIndex]->getDouble("ZHLEN");
    double Spb1theta = (*BarrelDMTraps)[recordIndex]->getDouble("THETA");
    double Spb1phi = (*BarrelDMTraps)[recordIndex]->getDouble("PHI");
    double Spb1yzn = (*BarrelDMTraps)[recordIndex]->getDouble("YZN");
    double Spb1xynzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZN");
    double Spb1xypzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZN");
    double Spb1angn = (*BarrelDMTraps)[recordIndex]->getDouble("ANGN");
    double Spb1yzp = (*BarrelDMTraps)[recordIndex]->getDouble("YZP");
    double Spb1xynzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZP");
    double Spb1xypzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZP");
    double Spb1angp = (*BarrelDMTraps)[recordIndex]->getDouble("ANGP");
    double Spb1xtr = (*BarrelDMTraps)[recordIndex]->getDouble("XTR");
    double Spb1ytr = (*BarrelDMTraps)[recordIndex]->getDouble("YTR");
    double Spb1ztr = (*BarrelDMTraps)[recordIndex]->getDouble("ZTR");
    double Spb1xrot = (*BarrelDMTraps)[recordIndex]->getDouble("XROT");
    recordIndex = trapMap["Spb2"];
    double Spb2zhlen = (*BarrelDMTraps)[recordIndex]->getDouble("ZHLEN");
    double Spb2theta = (*BarrelDMTraps)[recordIndex]->getDouble("THETA");
    double Spb2phi = (*BarrelDMTraps)[recordIndex]->getDouble("PHI");
    double Spb2yzn = (*BarrelDMTraps)[recordIndex]->getDouble("YZN");
    double Spb2xynzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZN");
    double Spb2xypzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZN");
    double Spb2angn = (*BarrelDMTraps)[recordIndex]->getDouble("ANGN");
    double Spb2yzp = (*BarrelDMTraps)[recordIndex]->getDouble("YZP");
    double Spb2xynzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZP");
    double Spb2xypzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZP");
    double Spb2angp = (*BarrelDMTraps)[recordIndex]->getDouble("ANGP");
    double Spb2ytr = (*BarrelDMTraps)[recordIndex]->getDouble("YTR");
    double Spb2ztr = (*BarrelDMTraps)[recordIndex]->getDouble("ZTR");
    recordIndex = trapMap["Spb3"];
    double Spb3zhlen = (*BarrelDMTraps)[recordIndex]->getDouble("ZHLEN");
    double Spb3theta = (*BarrelDMTraps)[recordIndex]->getDouble("THETA");
    double Spb3phi = (*BarrelDMTraps)[recordIndex]->getDouble("PHI");
    double Spb3yzn = (*BarrelDMTraps)[recordIndex]->getDouble("YZN");
    double Spb3xynzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZN");
    double Spb3xypzn = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZN");
    double Spb3angn = (*BarrelDMTraps)[recordIndex]->getDouble("ANGN");
    double Spb3yzp = (*BarrelDMTraps)[recordIndex]->getDouble("YZP");
    double Spb3xynzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYNZP");
    double Spb3xypzp = (*BarrelDMTraps)[recordIndex]->getDouble("XYPZP");
    double Spb3angp = (*BarrelDMTraps)[recordIndex]->getDouble("ANGP");
    double Spb3ztr = (*BarrelDMTraps)[recordIndex]->getDouble("ZTR");

    // transforms
    GeoBox   *Box   = new GeoBox(Boxhlen, Boxhwdt, Boxhhgt);
 
    HepGeom::Transform3D Cut3Boxe  = HepGeom::Translate3D(Boxxtr, Boxytr, Boxztr)*HepGeom::RotateX3D(-20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    HepGeom::Transform3D Cut4Boxe  = HepGeom::Translate3D(Boxxtr, -Boxytr,Boxztr)*HepGeom::RotateX3D(20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    HepGeom::Transform3D Cut3Boxp  = HepGeom::Translate3D(Boxxtr, Boxytr, Boxxrot)*HepGeom::RotateX3D(-20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    HepGeom::Transform3D Cut4Boxp  = HepGeom::Translate3D(Boxxtr, -Boxytr,Boxxrot)*HepGeom::RotateX3D(20*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg);
    
    // ----- build base envelopes -----
    GeoTrd   *Trd1air  = new GeoTrd(BaseExhlen1, BaseExhlen2, BaseEyhlen1, BaseEyhlen2, BaseEzhlen); 
    GeoLogVol  *lvbe          = new GeoLogVol("LAr::DM::BaseEnvelopes",Trd1air,matLArServices8); //In the end Density at least >= than SE1 because of Cryo Pipes
    GeoPhysVol *baseenvelopes    = new GeoPhysVol(lvbe);

    // ----- build bridge envelopes -----
    GeoTrap  *Trapair  = new GeoTrap(BridgeEzhlen, BridgeEtheta*CLHEP::deg, BridgeEphi, BridgeEyzn, BridgeExynzn, BridgeExypzn, BridgeEangn, BridgeEyzp, BridgeExynzp, BridgeExypzp, BridgeEangp);
    GeoLogVol  *lvbre        = new GeoLogVol("LAr::DM::BridgeEnvelopes",Trapair,matLArServices8);//In the end Density at least >= than SE1 because of Cryo Pipes
    GeoPhysVol *bridgeenvelopes    = new GeoPhysVol(lvbre);

    // ----- build sector envelopes -----
    // build 16 instances of SectorEnvelopes1 each with its own material!
    GeoTrd   *Trdair1  = new GeoTrd(SecE1xhlen1, SecE1xhlen2, DYa, DYb, (Endb-Enda)/2);
    
    GeoLogVol  *lvse1g1          = new GeoLogVol("LAr::DM::SectorEnvelopes1g1",Trdair1,matLArServices1);
    GeoPhysVol *sectorenvelopes1g1    = new GeoPhysVol(lvse1g1); 
    
    GeoLogVol  *lvse1g2          = new GeoLogVol("LAr::DM::SectorEnvelopes1g3",Trdair1,matLArServices2);
    GeoPhysVol *sectorenvelopes1g2    = new GeoPhysVol(lvse1g2); 
    
    GeoLogVol  *lvse1g3          = new GeoLogVol("LAr::DM::SectorEnvelopes1g3",Trdair1,matLArServices3);
    GeoPhysVol *sectorenvelopes1g3    = new GeoPhysVol(lvse1g3); 
    
    GeoLogVol  *lvse1g4          = new GeoLogVol("LAr::DM::SectorEnvelopes1g4",Trdair1,matLArServices4);
    GeoPhysVol *sectorenvelopes1g4    = new GeoPhysVol(lvse1g4);
    
    GeoLogVol  *lvse1g5          = new GeoLogVol("LAr::DM::SectorEnvelopes1g5",Trdair1,matLArServices5);
    GeoPhysVol *sectorenvelopes1g5    = new GeoPhysVol(lvse1g5); 
    
    GeoLogVol  *lvse1g6          = new GeoLogVol("LAr::DM::SectorEnvelopes1g6",Trdair1,matLArServices6);
    GeoPhysVol *sectorenvelopes1g6    = new GeoPhysVol(lvse1g6); 
    
    GeoLogVol  *lvse1g7          = new GeoLogVol("LAr::DM::SectorEnvelopes1g7",Trdair1,matLArServices7);
    GeoPhysVol *sectorenvelopes1g7    = new GeoPhysVol(lvse1g7); 
    
    GeoLogVol  *lvse1g8          = new GeoLogVol("LAr::DM::SectorEnvelopes1g8",Trdair1,matLArServices8);
    GeoPhysVol *sectorenvelopes1g8    = new GeoPhysVol(lvse1g8); 
    
    GeoLogVol  *lvse1g9          = new GeoLogVol("LAr::DM::SectorEnvelopes1g9",Trdair1,matLArServices9);
    GeoPhysVol *sectorenvelopes1g9    = new GeoPhysVol(lvse1g9); 
    
    GeoLogVol  *lvse1g10          = new GeoLogVol("LAr::DM::SectorEnvelopes1g10",Trdair1,matLArServices10);
    GeoPhysVol *sectorenvelopes1g10    = new GeoPhysVol(lvse1g10); 
    
    GeoLogVol  *lvse1g11          = new GeoLogVol("LAr::DM::SectorEnvelopes1g11",Trdair1,matLArServices11);
    GeoPhysVol *sectorenvelopes1g11    = new GeoPhysVol(lvse1g11); 
    
    GeoLogVol  *lvse1g12          = new GeoLogVol("LAr::DM::SectorEnvelopes1g12",Trdair1,matLArServices12);
    GeoPhysVol *sectorenvelopes1g12    = new GeoPhysVol(lvse1g12); 
    
    GeoLogVol  *lvse1g13          = new GeoLogVol("LAr::DM::SectorEnvelopes1g13",Trdair1,matLArServices13);
    GeoPhysVol *sectorenvelopes1g13    = new GeoPhysVol(lvse1g13); 
    
    GeoLogVol  *lvse1g14          = new GeoLogVol("LAr::DM::SectorEnvelopes1g14",Trdair1,matLArServices14);
    GeoPhysVol *sectorenvelopes1g14    = new GeoPhysVol(lvse1g14); 
    
    GeoLogVol  *lvse1g15          = new GeoLogVol("LAr::DM::SectorEnvelopes1g15",Trdair1,matLArServices15);
    GeoPhysVol *sectorenvelopes1g15    = new GeoPhysVol(lvse1g15); 
    
    GeoLogVol  *lvse1g16          = new GeoLogVol("LAr::DM::SectorEnvelopes1g16",Trdair1,matLArServices16);
    GeoPhysVol *sectorenvelopes1g16    = new GeoPhysVol(lvse1g16); 
    
    
    std::vector<GeoPhysVol*> se1List;
    se1List.push_back(sectorenvelopes1g1);
    se1List.push_back(sectorenvelopes1g2);
    se1List.push_back(sectorenvelopes1g3);
    se1List.push_back(sectorenvelopes1g4);
    se1List.push_back(sectorenvelopes1g5);
    se1List.push_back(sectorenvelopes1g6);
    se1List.push_back(sectorenvelopes1g7);
    se1List.push_back(sectorenvelopes1g8);
    se1List.push_back(sectorenvelopes1g9);
    se1List.push_back(sectorenvelopes1g10);
    se1List.push_back(sectorenvelopes1g11);
    se1List.push_back(sectorenvelopes1g12);
    se1List.push_back(sectorenvelopes1g13);
    se1List.push_back(sectorenvelopes1g14);
    se1List.push_back(sectorenvelopes1g15);
    se1List.push_back(sectorenvelopes1g16);

   // dedicated volumes to add in SectorEnvelopes1
   //    (a) volume in Trd shape to add slice of extra material in a given eta bin
   IRDBRecordset_ptr BarrelDMRing = rdbAccess->getRecordsetPtr("LArDMEnv1Ring",keyLAr.tag(),keyLAr.node());
   if (BarrelDMRing->size()>0)
   {
     for (unsigned int i=0;i<BarrelDMRing->size();i++) {
       double etaMin=(*BarrelDMRing)[i]->getDouble("ETAMIN");
       double etaMax=(*BarrelDMRing)[i]->getDouble("ETAMAX");
       double thicknessExtra=(*BarrelDMRing)[i]->getDouble("THICKNESS");
       std::string ringName = "LAr::DM::SectorEnvelopes1::"+(*BarrelDMRing)[i]->getString("RINGNAME");

       double radiusMin=SecE1ztr/sinh(etaMax);
       double radiusMax=SecE1ztr/sinh(etaMin);
       double dy1 = DYa + (DYb-DYa)*(radiusMin-Enda)*inv_Endab;
       double dy2 = DYa + (DYb-DYa)*(radiusMax-Enda)*inv_Endab;
       double zpos=0.5*(radiusMax+radiusMin - (Endb+Enda));
       GeoMaterial *matExtraTdr = materialManager->getMaterial((*BarrelDMRing)[i]->getString("MATERIAL"));

       //std::cout << " extraTdr: etamin,etamx " << etaMin << " " << etaMax << " thickness " << thicknessExtra << " size in Radius " << (radiusMax-radiusMin)
       //   << " size in R.phi " << dy1*2 << " " << dy2*2 << std::endl;
       //std::cout << " position in tdr1 " << zpos << std::endl;
       //std::cout << " name " << ringName << std::endl;
       GeoTrd   *extraMatTdr  = new GeoTrd(thicknessExtra/2., thicknessExtra/2., dy1, dy2, (radiusMax-radiusMin)/2);
       GeoLogVol *extraMatLog = new GeoLogVol(ringName,extraMatTdr,matExtraTdr);
       GeoPhysVol *extraMatPhys = new GeoPhysVol(extraMatLog);
       for (unsigned int isect=0;isect<se1List.size();isect++) {
         se1List[isect]->add(new GeoTransform(HepGeom::TranslateZ3D(zpos)));
         se1List[isect]->add(extraMatPhys);
       }
     }
   }

   // (b) extra material at fixed phi locations in the PPF1 area
   IRDBRecordset_ptr BarrelDMPhiBox = rdbAccess->getRecordsetPtr("LArDMEnv1PhiBox",keyLAr.tag(),keyLAr.node());
   if (BarrelDMPhiBox->size()>0)
   {
      for (unsigned int i=0;i<BarrelDMPhiBox->size();i++) {
        double eta=(*BarrelDMPhiBox)[i]->getDouble("ETA");
        double phi0=(*BarrelDMPhiBox)[i]->getDouble("PHI0");
        double deltaR=(*BarrelDMPhiBox)[i]->getDouble("DELTAR");
        double deltaRphi=(*BarrelDMPhiBox)[i]->getDouble("DELTARPHI");
        double thickness=(*BarrelDMPhiBox)[i]->getDouble("THICKNESS");
        int nphi=(*BarrelDMPhiBox)[i]->getInt("NPHI");
        int noHorizontal = (*BarrelDMPhiBox)[i]->getInt("NOHORIZ");

        GeoMaterial* matExtraPPF1 = materialManager->getMaterial((*BarrelDMPhiBox)[i]->getString("MATERIAL"));
        std::string boxName = "LAr::DM::SectorEnvelopes1::"+(*BarrelDMPhiBox)[i]->getString("BOXNAME");

        //std::cout << " extraBox name " << boxName << " eta " << eta << " phi0" << phi0 << " deltaR " << deltaR << " deltaRphi " << deltaRphi
        //  << " thickness " << thickness << " nphi " << nphi << std::endl;

        GeoBox *ppf1Box = new GeoBox(thickness/2.,deltaRphi/2.,deltaR/2.);
        GeoLogVol *ppf1Log = new GeoLogVol(boxName,ppf1Box,matExtraPPF1);
        GeoPhysVol *ppf1Phys = new GeoPhysVol(ppf1Log);

        int nPerEnv1 = nphi/NCrates;
        double dphi=2.*M_PI/((float)(nphi));
        double radius=SecE1ztr/sinh(eta);
        for (int iphi=0;iphi<nPerEnv1;iphi++) {
           double xpos=0.;
           double ypos=radius*sin(phi0+((float)(iphi))*dphi);
           double zpos=radius*cos(phi0+((float)(iphi))*dphi) - (Endb+Enda)/2.;
           for (unsigned int isect=0;isect<se1List.size();isect++) {
             // no PPF1 box around phi=0 and phi=pi
             if (noHorizontal>0 && ((isect==7 && iphi==1) || (isect==8 && iphi==0) || (isect==15 && iphi==1) || (isect==0 && iphi==0) ) ) continue;
             se1List[isect]->add(new GeoTransform(HepGeom::Translate3D(xpos,ypos,zpos)));
             se1List[isect]->add(ppf1Phys);
           }
        }
      }
    }

    
    // build 5 instances of SectorEnvelopes1 with 3 different materials!
    GeoTrd   *Trdair2  = new GeoTrd(SecE2xhlen1, SecE2xhlen2, DYb, DYc, (Endc-Endb)/2);
    const GeoShape & SectorEnvelope= ((*Trdair2).
				      subtract((*Box)  <<HepGeom::Transform3D(Cut3Boxe)).
				      subtract((*Box)  <<HepGeom::Transform3D(Cut4Boxe)));
    
    const GeoShape & SectorEnvelopes= ((SectorEnvelope).
				       add(SectorEnvelope  << HepGeom::TranslateY3D(-(DYb+DYc)*cos(Alfa/2)*cos(Alfa/2))*HepGeom::TranslateZ3D(-(DYb+DYc)*0.5*sin(Alfa))*HepGeom::RotateX3D(Alfa))); 
    
    GeoLogVol  *lvse2r          = new GeoLogVol("LAr::DM::SectorEnvelopes2r",&SectorEnvelopes,matLArServices20);
    GeoPhysVol *sectorenvelopes2r    = new GeoPhysVol(lvse2r);  // for right-handed splice boxes 
    
    GeoLogVol  *lvse2l          = new GeoLogVol("LAr::DM::SectorEnvelopes2l",&SectorEnvelopes,matLArServices20);
    GeoPhysVol *sectorenvelopes2l    = new GeoPhysVol(lvse2l);  // for left-handed splice boxes
    
    GeoLogVol  *lvse2h          = new GeoLogVol("LAr::DM::SectorEnvelopes2h",&SectorEnvelopes,matLArServices19);
    GeoPhysVol *sectorenvelopes2h    = new GeoPhysVol(lvse2h);  // no splice boxes horizontal at 0 & 180 CLHEP::deg.
    
    GeoLogVol  *lvse2vup          = new GeoLogVol("LAr::DM::SectorEnvelopes2vup",&SectorEnvelopes,matLArServices17);
    GeoPhysVol *sectorenvelopes2vup    = new GeoPhysVol(lvse2vup);  // no splice boxes vertical up at 90 CLHEP::deg
    
    GeoLogVol  *lvse2vd          = new GeoLogVol("LAr::DM::SectorEnvelopes2Vd",&SectorEnvelopes,matLArServices18);
    GeoPhysVol *sectorenvelopes2vd    = new GeoPhysVol(lvse2vd);  // no splice boxes vertical down at 270 CLHEP::deg
    
    
    // ----- build base plates -----
    GeoTrd   *Trd1alu  = new GeoTrd(BasePxhlen1, BasePxhlen2, BasePyhlen1, BasePyhlen2, BasePzhlen);
    GeoLogVol  *lvbp          = new GeoLogVol("LAr::DM::BasePlates",Trd1alu,alu);
    GeoPhysVol *baseplates    = new GeoPhysVol(lvbp);
    
    // ----- build bridge plates -----
    GeoTrap  *Trapalu  = new GeoTrap(BridgePzhlen, BridgePtheta*CLHEP::deg, BridgePphi, BridgePyzn, BridgePxynzn, BridgePxypzn, BridgePangn, BridgePyzp, BridgePxynzp, BridgePxypzp, BridgePangp); 
    GeoLogVol  *lvbrp          = new GeoLogVol("LAr::DM::BridgePlates",Trapalu,alu);
    GeoPhysVol *bridgeplates    = new GeoPhysVol(lvbrp);
    
    
    // ----- build sector plates -----
    GeoTrd   *Trd2alu  = new GeoTrd(SecPxhlen1, SecPxhlen2, SecPyhlen1, SecPyhlen2, SecPzhlen );///
    const GeoShape & SectorPlates= ((*Trd2alu).
				    subtract((*Box)  <<HepGeom::Transform3D(Cut3Boxp)).
				    subtract((*Box)  <<HepGeom::Transform3D(Cut4Boxp)));
    GeoLogVol  *lvsp          = new GeoLogVol("LAr::DM::SectorPlates",&SectorPlates,alu);
    GeoPhysVol *sectorplates    = new GeoPhysVol(lvsp);
    
    //---------- Build Splice boxes for InDet optical fibers--------
    
    GeoTrap  *GeoTrap1  = new GeoTrap(Spb1zhlen, Spb1theta, Spb1phi, Spb1yzn, Spb1xynzn, Spb1xypzn, Spb1angn, Spb1yzp, Spb1xynzp, Spb1xypzp, Spb1angp);
    GeoBox   *Box1   = new GeoBox(SplBoxhlen, SplBoxhwdt, SplBoxhhgt);  
    const GeoShape & SpliceBox = ((*GeoTrap1).
				  subtract(*Box1 << HepGeom::TranslateZ3D(SplBoxztr)*HepGeom::TranslateY3D(-SplBoxytr)*HepGeom::RotateX3D(SplBoxxrot*CLHEP::deg)));
    
    GeoTransform *xtr = new GeoTransform (HepGeom::TranslateZ3D(Spb1ztr)*HepGeom::TranslateY3D(-Spb1ytr)*HepGeom::TranslateX3D(Spb1xtr)*HepGeom::RotateX3D(Spb1xrot*CLHEP::deg));
    sectorenvelopes2r->add(xtr);
    GeoLogVol  *lvspbr     = new GeoLogVol("LAr::DM::SPliceBoxr",&SpliceBox,alu); 
    GeoPhysVol *spliceboxr       = new GeoPhysVol(lvspbr);
    sectorenvelopes2r->add(spliceboxr);
    
    GeoTransform *xtl = new GeoTransform (HepGeom::TranslateZ3D(Spb1ztr)*HepGeom::TranslateY3D(-Spb1ytr)*HepGeom::TranslateX3D(Spb1xtr)*HepGeom::RotateY3D(-180*CLHEP::deg)*HepGeom::RotateX3D(-(Alfa/2)));
    sectorenvelopes2l->add(xtl);
    GeoLogVol  *lvspbl     = new GeoLogVol("LAr::DM::SpliceBoxl",&SpliceBox,alu);  
    GeoPhysVol *spliceboxl       = new GeoPhysVol(lvspbl);
    sectorenvelopes2l->add(spliceboxl);
    
    ////
    GeoTrd   *Trd1  = new GeoTrd(Spb0xhlen1, Spb0xhlen2, Spb0yhlen1, Spb0yhlen2, Spb0zhlen);
    GeoTrap  *GeoTrap2  = new GeoTrap(Spb2zhlen, Spb2theta, Spb2phi, Spb2yzn, Spb2xynzn, Spb2xypzn, Spb2angn, Spb2yzp, Spb2xynzp, Spb2xypzp, Spb2angp);
    GeoTrap  *GeoTrap3  = new GeoTrap(Spb3zhlen, Spb3theta, Spb3phi, Spb3yzn, Spb3xynzn, Spb3xypzn, Spb3angn, Spb3yzp, Spb3xynzp, Spb3xypzp, Spb3angp);
    
    GeoTransform *xt1 = new GeoTransform (HepGeom::TranslateY3D(-Spb0ytr)*HepGeom::RotateX3D(Spb0xrot*CLHEP::deg));
    spliceboxr->add(xt1);
    spliceboxl->add(xt1);
    GeoLogVol  *lt1     = new GeoLogVol("LAr::DM::TBox1",Trd1,air);
    GeoPhysVol *tbox1       = new GeoPhysVol(lt1);
    spliceboxr->add(tbox1);
    spliceboxl->add(tbox1);
    
    GeoTransform *xt2 = new GeoTransform (HepGeom::TranslateZ3D(Spb2ztr)*HepGeom::TranslateY3D(Spb2ytr));
    spliceboxr->add(xt2);
    spliceboxl->add(xt2);
    GeoLogVol  *lt2     = new GeoLogVol("LAr::DM::TBox2",GeoTrap2,air);
    GeoPhysVol *tbox2       = new GeoPhysVol(lt2);
    spliceboxr->add(tbox2);
    spliceboxl->add(tbox2);
    
    GeoTransform *xt3 = new GeoTransform (HepGeom::TranslateZ3D(-Spb3ztr)); 
    spliceboxr->add(xt3);
    spliceboxl->add(xt3);
    GeoLogVol  *lt3     = new GeoLogVol("LAr::DM::TBox3",GeoTrap3,air);
    GeoPhysVol *tbox3       = new GeoPhysVol(lt3);
    spliceboxr->add(tbox3);
    spliceboxl->add(tbox3);
    
    //-------------- Place volumes in LAr Envelope -------------------
    
    //sectorPlates
    TRANSFUNCTION spA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(SecPxtr)*HepGeom::TranslateZ3D(SecPztr)*HepGeom::RotateY3D(90*CLHEP::deg);///
    TRANSFUNCTION spC = Pow(HepGeom::RotateZ3D(1.0),f+(Alfa/2))*HepGeom::TranslateX3D(SecPxtr)*HepGeom::TranslateZ3D(-SecPztr)*HepGeom::RotateY3D(90*CLHEP::deg);///
    GeoSerialTransformer *sptA = new GeoSerialTransformer(sectorplates,&spA, NCrates);
    GeoSerialTransformer *sptC = new GeoSerialTransformer(sectorplates,&spC, NCrates);
    envelope->add(sptA);
    envelope->add(sptC);
    
    //bridgePlates
    TRANSFUNCTION brpA1 = Pow(HepGeom::RotateZ3D(1.0),f-(5*Alfa/2))*HepGeom::TranslateX3D(BridgePxtr)*HepGeom::TranslateZ3D(BridgePztr)*HepGeom::RotateZ3D(90*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg)*HepGeom::RotateX3D(90*CLHEP::deg);
    TRANSFUNCTION brpA2 = Pow(HepGeom::RotateZ3D(1.0),f+(13*Alfa/2))*HepGeom::TranslateX3D(BridgePxtr)*HepGeom::TranslateZ3D(BridgePztr)*HepGeom::RotateZ3D(90*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg)*HepGeom::RotateX3D(90*CLHEP::deg); 
    TRANSFUNCTION brpC1 = Pow(HepGeom::RotateZ3D(1.0),f-(5*Alfa/2))*HepGeom::TranslateX3D(BridgePxtr)*HepGeom::TranslateZ3D(-BridgePztr)*HepGeom::RotateZ3D(-90*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(-90*CLHEP::deg);
    TRANSFUNCTION brpC2 = Pow(HepGeom::RotateZ3D(1.0),f+(13*Alfa/2))*HepGeom::TranslateX3D(BridgePxtr)*HepGeom::TranslateZ3D(-BridgePztr)*HepGeom::RotateZ3D(-90*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(-90*CLHEP::deg);   GeoSerialTransformer *brptA1 = new GeoSerialTransformer(bridgeplates,&brpA1, 5);
    GeoSerialTransformer *brptA2 = new GeoSerialTransformer(bridgeplates,&brpA2, 5);
    GeoSerialTransformer *brptC1 = new GeoSerialTransformer(bridgeplates,&brpC1, 5);
    GeoSerialTransformer *brptC2 = new GeoSerialTransformer(bridgeplates,&brpC2, 5);
    envelope->add(brptA1);
    envelope->add(brptA2);
    envelope->add(brptC1);
    envelope->add(brptC2);
    
    //basePlates
    TRANSFUNCTION bpA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(BasePxtr)*HepGeom::TranslateZ3D(BasePztr)*HepGeom::RotateY3D(90*CLHEP::deg); 
    TRANSFUNCTION bpC = Pow(HepGeom::RotateZ3D(1.0),f+(Alfa/2))*HepGeom::TranslateX3D(BasePxtr)*HepGeom::TranslateZ3D(-BasePztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    GeoSerialTransformer *bptA = new GeoSerialTransformer(baseplates,&bpA, NCrates);
    GeoSerialTransformer *bptC = new GeoSerialTransformer(baseplates,&bpC, NCrates);
    envelope->add(bptA);
    envelope->add(bptC);
    
    //sectorEnvelopes1
    //counter-clockwise from top if taking sideA for reference (clockwise for sideC)
    TRANSFUNCTION seA1G5 = Pow(HepGeom::RotateZ3D(1.0),f+(9*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G5 = Pow(HepGeom::RotateZ3D(1.0),f+(9*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G6 = Pow(HepGeom::RotateZ3D(1.0),f+(11*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G6 = Pow(HepGeom::RotateZ3D(1.0),f+(11*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G7 = Pow(HepGeom::RotateZ3D(1.0),f+(13*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G7 = Pow(HepGeom::RotateZ3D(1.0),f+(13*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G8 = Pow(HepGeom::RotateZ3D(1.0),f+(15*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G8 = Pow(HepGeom::RotateZ3D(1.0),f+(15*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G9 = Pow(HepGeom::RotateZ3D(1.0),f+(17*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G9 = Pow(HepGeom::RotateZ3D(1.0),f+(17*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G10 = Pow(HepGeom::RotateZ3D(1.0),f+(19*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G10 = Pow(HepGeom::RotateZ3D(1.0),f+(19*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G11 = Pow(HepGeom::RotateZ3D(1.0),f+(21*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G11 = Pow(HepGeom::RotateZ3D(1.0),f+(21*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G12 = Pow(HepGeom::RotateZ3D(1.0),f+(23*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G12 = Pow(HepGeom::RotateZ3D(1.0),f+(23*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    //clockwise from top if taking sideA for reference (counter-clockwise for sideC)
    TRANSFUNCTION seA1G4 = Pow(HepGeom::RotateZ3D(1.0),f+(7*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G4 = Pow(HepGeom::RotateZ3D(1.0),f+(7*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G3 = Pow(HepGeom::RotateZ3D(1.0),f+(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G3 = Pow(HepGeom::RotateZ3D(1.0),f+(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G2 = Pow(HepGeom::RotateZ3D(1.0),f+(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G2 = Pow(HepGeom::RotateZ3D(1.0),f+(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G1 = Pow(HepGeom::RotateZ3D(1.0),f+(1*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G1 = Pow(HepGeom::RotateZ3D(1.0),f+(1*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G16 = Pow(HepGeom::RotateZ3D(1.0),f-(1*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G16 = Pow(HepGeom::RotateZ3D(1.0),f-(1*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G15 = Pow(HepGeom::RotateZ3D(1.0),f-(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G15 = Pow(HepGeom::RotateZ3D(1.0),f-(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G14 = Pow(HepGeom::RotateZ3D(1.0),f-(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G14 = Pow(HepGeom::RotateZ3D(1.0),f-(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA1G13 = Pow(HepGeom::RotateZ3D(1.0),f-(7*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC1G13 = Pow(HepGeom::RotateZ3D(1.0),f-(7*Alfa/2))*HepGeom::TranslateX3D((Endb+Enda)/2)*HepGeom::TranslateZ3D(-SecE1ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    
    GeoSerialTransformer *setA1G5 = new GeoSerialTransformer(sectorenvelopes1g5,&seA1G5, 1);
    GeoSerialTransformer *setC1G5 = new GeoSerialTransformer(sectorenvelopes1g5,&seC1G5, 1);
    GeoSerialTransformer *setA1G6 = new GeoSerialTransformer(sectorenvelopes1g6,&seA1G6, 1);
    GeoSerialTransformer *setC1G6 = new GeoSerialTransformer(sectorenvelopes1g6,&seC1G6, 1);
    GeoSerialTransformer *setA1G7 = new GeoSerialTransformer(sectorenvelopes1g7,&seA1G7, 1);
    GeoSerialTransformer *setC1G7 = new GeoSerialTransformer(sectorenvelopes1g7,&seC1G7, 1);
    GeoSerialTransformer *setA1G8 = new GeoSerialTransformer(sectorenvelopes1g8,&seA1G8, 1);
    GeoSerialTransformer *setC1G8 = new GeoSerialTransformer(sectorenvelopes1g8,&seC1G8, 1);
    GeoSerialTransformer *setA1G9 = new GeoSerialTransformer(sectorenvelopes1g9,&seA1G9, 1);
    GeoSerialTransformer *setC1G9 = new GeoSerialTransformer(sectorenvelopes1g9,&seC1G9, 1);
    GeoSerialTransformer *setA1G10 = new GeoSerialTransformer(sectorenvelopes1g10,&seA1G10, 1);
    GeoSerialTransformer *setC1G10 = new GeoSerialTransformer(sectorenvelopes1g10,&seC1G10, 1);
    GeoSerialTransformer *setA1G11 = new GeoSerialTransformer(sectorenvelopes1g11,&seA1G11, 1);
    GeoSerialTransformer *setC1G11 = new GeoSerialTransformer(sectorenvelopes1g11,&seC1G11, 1);
    GeoSerialTransformer *setA1G12 = new GeoSerialTransformer(sectorenvelopes1g12,&seA1G12, 1);
    GeoSerialTransformer *setC1G12 = new GeoSerialTransformer(sectorenvelopes1g12,&seC1G12, 1);
    
    GeoSerialTransformer *setA1G4 = new GeoSerialTransformer(sectorenvelopes1g4,&seA1G4, 1);
    GeoSerialTransformer *setC1G4 = new GeoSerialTransformer(sectorenvelopes1g4,&seC1G4, 1);
    GeoSerialTransformer *setA1G3 = new GeoSerialTransformer(sectorenvelopes1g3,&seA1G3, 1);
    GeoSerialTransformer *setC1G3 = new GeoSerialTransformer(sectorenvelopes1g3,&seC1G3, 1);
    GeoSerialTransformer *setA1G2 = new GeoSerialTransformer(sectorenvelopes1g2,&seA1G2, 1);
    GeoSerialTransformer *setC1G2 = new GeoSerialTransformer(sectorenvelopes1g2,&seC1G2, 1);
    GeoSerialTransformer *setA1G1 = new GeoSerialTransformer(sectorenvelopes1g1,&seA1G1, 1);
    GeoSerialTransformer *setC1G1 = new GeoSerialTransformer(sectorenvelopes1g1,&seC1G1, 1);
    GeoSerialTransformer *setA1G16 = new GeoSerialTransformer(sectorenvelopes1g16,&seA1G16, 1);
    GeoSerialTransformer *setC1G16 = new GeoSerialTransformer(sectorenvelopes1g16,&seC1G16, 1);
    GeoSerialTransformer *setA1G15 = new GeoSerialTransformer(sectorenvelopes1g15,&seA1G15, 1);
    GeoSerialTransformer *setC1G15 = new GeoSerialTransformer(sectorenvelopes1g15,&seC1G15, 1);
    GeoSerialTransformer *setA1G14 = new GeoSerialTransformer(sectorenvelopes1g14,&seA1G14, 1);
    GeoSerialTransformer *setC1G14 = new GeoSerialTransformer(sectorenvelopes1g14,&seC1G14, 1);
    GeoSerialTransformer *setA1G13 = new GeoSerialTransformer(sectorenvelopes1g13,&seA1G13, 1);
    GeoSerialTransformer *setC1G13 = new GeoSerialTransformer(sectorenvelopes1g13,&seC1G13, 1);
  
    envelope->add(setA1G5);
    envelope->add(setC1G5);
    envelope->add(setA1G6);
    envelope->add(setC1G6);
    envelope->add(setA1G7);
    envelope->add(setC1G7);
    envelope->add(setA1G8);
    envelope->add(setC1G8);
    envelope->add(setA1G9);
    envelope->add(setC1G9);
    envelope->add(setA1G10);
    envelope->add(setC1G10);
    envelope->add(setA1G11);
    envelope->add(setC1G11);
    envelope->add(setA1G12);
    envelope->add(setC1G12);
    
    envelope->add(setA1G4);
    envelope->add(setC1G4);
    envelope->add(setA1G3);
    envelope->add(setC1G3);
    envelope->add(setA1G2);
    envelope->add(setC1G2);
    envelope->add(setA1G1);
    envelope->add(setC1G1);
    envelope->add(setA1G16);
    envelope->add(setC1G16);
    envelope->add(setA1G15);
    envelope->add(setC1G15);
    envelope->add(setA1G14);
    envelope->add(setC1G14);
    envelope->add(setA1G13);
    envelope->add(setC1G13);
    
    //sectorEnvelopes2
    TRANSFUNCTION seA2r = Pow(HepGeom::RotateZ3D(1.0),8*f-(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA2l = Pow(HepGeom::RotateZ3D(1.0),8*f+(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC2r = Pow(HepGeom::RotateZ3D(1.0),8*f-(3*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC2l = Pow(HepGeom::RotateZ3D(1.0),8*f+(5*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA2Vup = Pow(HepGeom::RotateZ3D(1.0),f+(9*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA2Vd = Pow(HepGeom::RotateZ3D(1.0),f-(7*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seA2H = Pow(HepGeom::RotateZ3D(1.0),8*f+(Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC2Vup = Pow(HepGeom::RotateZ3D(1.0),f+(9*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC2Vd = Pow(HepGeom::RotateZ3D(1.0),f-(7*Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    TRANSFUNCTION seC2H = Pow(HepGeom::RotateZ3D(1.0),8*f+(Alfa/2))*HepGeom::TranslateX3D((Endb+Endc)/2)*HepGeom::TranslateZ3D(-SecE2ztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    
    GeoSerialTransformer *setA2r = new GeoSerialTransformer(sectorenvelopes2r,&seA2r, 2);
    GeoSerialTransformer *setA2l = new GeoSerialTransformer(sectorenvelopes2l,&seA2l, 2);
    GeoSerialTransformer *setC2r = new GeoSerialTransformer(sectorenvelopes2r,&seC2r, 2);
    GeoSerialTransformer *setC2l = new GeoSerialTransformer(sectorenvelopes2l,&seC2l, 2);
    GeoSerialTransformer *setA2Vup = new GeoSerialTransformer(sectorenvelopes2vup,&seA2Vup, 1);
    GeoSerialTransformer *setA2Vd = new GeoSerialTransformer(sectorenvelopes2vd,&seA2Vd, 1);
    GeoSerialTransformer *setA2H = new GeoSerialTransformer(sectorenvelopes2h,&seA2H, 2); 
    GeoSerialTransformer *setC2Vup = new GeoSerialTransformer(sectorenvelopes2vup,&seC2Vup, 1);
    GeoSerialTransformer *setC2Vd = new GeoSerialTransformer(sectorenvelopes2vd,&seC2Vd, 1);
    GeoSerialTransformer *setC2H = new GeoSerialTransformer(sectorenvelopes2h,&seC2H, 2);
    
    envelope->add(setA2r);
    envelope->add(setA2l);
    envelope->add(setC2r);
    envelope->add(setC2l);
    envelope->add(setA2Vup);
    envelope->add(setA2Vd);
    envelope->add(setA2H);
    envelope->add(setC2Vup);
    envelope->add(setC2Vd);
    envelope->add(setC2H);
    
    //bridgeEnvelopes
    TRANSFUNCTION breA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(BridgeExtr)*HepGeom::TranslateZ3D(BridgeEztr)*HepGeom::RotateZ3D(90*CLHEP::deg)*HepGeom::RotateY3D(90*CLHEP::deg)*HepGeom::RotateX3D(90*CLHEP::deg);
    TRANSFUNCTION breC = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(BridgeExtr)*HepGeom::TranslateZ3D(-BridgeEztr)*HepGeom::RotateZ3D(-90*CLHEP::deg)*HepGeom::RotateY3D(-90*CLHEP::deg)*HepGeom::RotateX3D(-90*CLHEP::deg);
    GeoSerialTransformer *bretA = new GeoSerialTransformer(bridgeenvelopes,&breA, NCrates);
    GeoSerialTransformer *bretC = new GeoSerialTransformer(bridgeenvelopes,&breC, NCrates);
    envelope->add(bretA);
    envelope->add(bretC);
    
    //baseEnvelopes
    TRANSFUNCTION beA = Pow(HepGeom::RotateZ3D(1.0),f-(Alfa/2))*HepGeom::TranslateX3D(BaseExtr)*HepGeom::TranslateZ3D(BaseEztr)*HepGeom::RotateY3D(90*CLHEP::deg); 
    TRANSFUNCTION beC = Pow(HepGeom::RotateZ3D(1.0),f+(Alfa/2))*HepGeom::TranslateX3D(BaseExtr)*HepGeom::TranslateZ3D(-BaseEztr)*HepGeom::RotateY3D(90*CLHEP::deg);
    GeoSerialTransformer *betA = new GeoSerialTransformer(baseenvelopes,&beA, NCrates);
    GeoSerialTransformer *betC = new GeoSerialTransformer(baseenvelopes,&beC, NCrates);
    envelope->add(betA);
    envelope->add(betC);
    
//     matFEBoards->ref();    matFEBoards->unref();
//     matC6F14->ref();       matC6F14->unref();
//     matWater->ref();       matWater->unref();
//     matEpoxyResin->ref();  matEpoxyResin->unref();
//     matFiberglass->ref();  matFiberglass->unref();
//     matN2->ref();          matN2->unref();
  }
}



         
