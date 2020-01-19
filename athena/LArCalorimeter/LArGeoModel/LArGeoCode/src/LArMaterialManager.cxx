/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoCode/LArMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "LArGeoCode/DatabaseAccessTool.h"
#include <algorithm>

// #define DEBUGGEO

LArMaterialManager::LArMaterialManager(StoreGateSvc* detStore):
  m_storedManager(0)
{
  // Get the Message Service:
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  if(status.isFailure())
    throw std::runtime_error("LArMaterialManager: cannot initialize message service");

  DataHandle<StoredMaterialManager> materialManager;
  status = detStore->retrieve(materialManager, std::string("MATERIALS"));
  if(status.isFailure())
    throw std::runtime_error("Error in LArMaterialManager, list of precalculated materials is absent..");

  m_storedManager = materialManager;
}

LArMaterialManager::~LArMaterialManager()
{
}

void LArMaterialManager::buildMaterials()
{
  MsgStream msg(m_msgSvc, "LArMaterialManager");
#ifdef DEBUGGEO
  msg << "**** in Material Manager " << endreq;
#endif

  GeoMaterial *Copper  = m_storedManager->getMaterial("std::Copper");
  if (!Copper) throw std::runtime_error("Error in LArMaterialManager, std::Copper is not found.");
#ifdef DEBUGGEO
  msg << "Copper radiation length " << Copper->getRadLength() << " "
            << Copper->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
#endif


  GeoMaterial *Iron  = m_storedManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in LArMaterialManager, std::Iron is not found.");

  GeoMaterial *Lead  = m_storedManager->getMaterial("std::Lead");
  if (!Lead) throw std::runtime_error("Error in LArMaterialManager, std::Lead is not found.");
#ifdef DEBUGGEO
  msg << MSG::INFO<< "Lead radiation length " << Lead->getRadLength() << " "
             << Lead->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
#endif


  GeoMaterial *LAr  = m_storedManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in LArMaterialManager, std::LiquidArgon is not found.");

#ifdef DEBUGGEO
  msg << MSG::INFO<< "LAr radiation length " << LAr->getRadLength() << " "
            << LAr->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
#endif

  GeoMaterial *Air  = m_storedManager->getMaterial("std::Air");
  if (!Air) throw std::runtime_error("Error in LArMaterialManager, std::Air is not found.");

  GeoMaterial *Kapton  = m_storedManager->getMaterial("std::Kapton");
  if (!Kapton) throw std::runtime_error("Error in LArMaterialManager, std::Kapton is not found.");
#ifdef DEBUGGEO
  msg << MSG::INFO<< "Kapton radiation length " << Kapton->getRadLength() <<  " "
            << Kapton->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
  for (size_t i = 0; i< Kapton->getNumElements();i++) {
    msg << MSG::INFO << int (Kapton->getFraction(i)*100) << "% \t"  << Kapton->getElement(i)->getName() << endreq;
    }


#endif

  GeoMaterial *Glue  = m_storedManager->getMaterial("LAr::Glue");
  if (!Glue) throw std::runtime_error("Error in LArMaterialManager, LAr::Glue is not found.");
#ifdef DEBUGGEO
  msg << MSG::INFO<< "Glue   radiation length " << Glue->getRadLength() << " "
            << Glue->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
  for (size_t i = 0; i< Glue->getNumElements();i++) {
    msg << MSG::INFO << int (Glue->getFraction(i)*100) << "% \t"  << Glue->getElement(i)->getName() << endreq;
    }

#endif

  GeoMaterial *G10  = m_storedManager->getMaterial("LAr::G10");
  if (!G10) throw std::runtime_error("Error in LArMaterialManager, LAr::G10 is not found.");
#ifdef DEBUGGEO
  msg << MSG::INFO<< "G10    radiation length " << G10->getRadLength() << " "
            << G10->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
  for (size_t i = 0; i< G10->getNumElements();i++) {
    msg << MSG::INFO << int (G10->getFraction(i)*100) << "% \t"  << G10->getElement(i)->getName() << endreq;
    }

#endif

  DatabaseAccessTool dB;
  //----------------------------------------------------------------------------------------//
  // LAR...                                                                                 //
  //----------------------------------------------------------------------------------------//
  {

    //**** GU: be careful
    // In GeoMaterial::add, the fractions are per MASS
    //
    double Tggl, Tgfe, Tgpb, Thgl, Thfe, Thpb, Thcu, Thfg, ThMBcu, ThMBG10;

    // thin and thick lead absorbers
    double Totalthick,Totalmass,Fracpb,Fracfe,Fracgl;
    double Totalthicke,Totalmasse,FracCu,FracKap;
    // contraction from warm to cold

    double contract=dB.getDouble("ColdContraction","ColdContraction-00", "ABSORBERCONTRACTION"); // LArEMBAbsorberContraction

    // first thin absorbers
    Tggl=dB.getDouble("BarrelGeometry","BarrelGeometry-00","TGGL")*CLHEP::cm*contract; // LArEMBThinAbsGlue
    Tgfe=dB.getDouble("BarrelGeometry","BarrelGeometry-00","TGFE")*CLHEP::cm*contract; // LArEMBThinAbsIron
    Tgpb=dB.getDouble("BarrelGeometry","BarrelGeometry-00","TGPB")*CLHEP::cm*contract; // LArEMBThinAbsLead
    Totalthick = Tggl+Tgfe+Tgpb;
    Totalmass = (Tgpb*Lead->getDensity()+Tgfe*Iron->getDensity()+Tggl*Glue->getDensity());
    //***GU below are the fraction per mass
    Fracpb = (Tgpb*Lead->getDensity())/Totalmass;
    Fracfe = (Tgfe*Iron->getDensity())/Totalmass;
    Fracgl = (Tggl*Glue->getDensity())/Totalmass;
    double density = Totalmass/Totalthick/(contract*contract*contract);

    msg << MSG::DEBUG <<"---- THIN absorber characteristics: ----" << endreq;
    msg << MSG::DEBUG <<"  Fraction pb,fe,gl: "<<Fracpb<<","<<Fracfe<<"," <<Fracgl<< endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness: "<<Totalmass<<" ," <<Totalthick<< endreq;
    msg << MSG::DEBUG<<" Contraction " << contract << endreq;
    msg << MSG::DEBUG <<"  Thinabs Density =  "<< density*(CLHEP::cm3/CLHEP::g) << endreq;

    GeoMaterial* Thin_abs = new GeoMaterial("Thinabs",density);
    Thin_abs->add(Lead,Fracpb);
    Thin_abs->add(Iron,Fracfe);
    Thin_abs->add(Glue,Fracgl);
    m_storedManager->addMaterial("LAr", Thin_abs);

#ifdef DEBUGGEO
  msg << MSG::INFO<< "ThinAbs radiation length " << Thin_abs->getRadLength() << endreq;
#endif

    // then thick absorbers
    Thgl=dB.getDouble("BarrelGeometry","BarrelGeometry-00","THGL")*CLHEP::cm*contract; // LArEMBThickAbsGlue
    Thfe=dB.getDouble("BarrelGeometry","BarrelGeometry-00","THFE")*CLHEP::cm*contract; // LArEMBThickAbsIron
    Thpb=dB.getDouble("BarrelGeometry","BarrelGeometry-00","THPB")*CLHEP::cm*contract; // LArEMBThickAbsLead

    Totalthick = Thgl+Thfe+Thpb;
    Totalmass = (Thpb*Lead->getDensity()+Thfe*Iron->getDensity()+Thgl*Glue->getDensity());
    //**GU below are the fractions per mass
    Fracpb = (Thpb*Lead->getDensity())/Totalmass;
    Fracfe = (Thfe*Iron->getDensity())/Totalmass;
    Fracgl = (Thgl*Glue->getDensity())/Totalmass;
    density = Totalmass/Totalthick/(contract*contract*contract);

    msg << MSG::DEBUG <<"---- THICK absorber characteristics: ----" << endreq;
    msg << MSG::DEBUG <<"  Fraction pb,fe,gl: "<<Fracpb<<","<<Fracfe<<","<<Fracgl << endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness: "<<Totalmass<<" ,"<<Totalthick << endreq;
    msg << MSG::DEBUG <<"  Thickabs Density =  " << density*(CLHEP::cm3/CLHEP::g) << endreq;

    GeoMaterial* Thick_abs = new GeoMaterial("Thickabs",density);
    Thick_abs->add(Lead,Fracpb);
    Thick_abs->add(Iron,Fracfe);
    Thick_abs->add(Glue,Fracgl);
    m_storedManager->addMaterial("LAr",Thick_abs);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "ThickAbs radiation length " << Thick_abs->getRadLength() << endreq;
#endif

    // electrode =mixture Kapton+Cu
    Thcu=dB.getDouble("BarrelGeometry","BarrelGeometry-00","THCU")*CLHEP::cm; // LArEMBThickElecCopper
    Thfg=dB.getDouble("BarrelGeometry","BarrelGeometry-00","THFG")*CLHEP::cm; // LArEMBThickElecKapton
    Totalthicke = Thcu+Thfg;
    Totalmasse = (Thcu*Copper->getDensity()+Thfg*Kapton->getDensity());
    //**GU below are the fractions per mass
    FracCu = (Thcu*Copper->getDensity())/Totalmasse;
    FracKap = (Thfg*Kapton->getDensity())/Totalmasse;
    //    density = Totalmasse/Totalthicke;

    double contract_e=1./1.003625; // From Fares (J.T.)
    density = (Totalmasse/Totalthicke)/(contract_e*contract_e*contract_e);

    msg << MSG::DEBUG <<"---- Electrode characteristics: ----" << endreq;
    msg << MSG::DEBUG <<"  Fraction Cu, Kapton: " << FracCu << ","<< FracKap << endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness:"<<Totalmasse<<" ,"<<Totalthicke<< endreq;
    msg << MSG::DEBUG <<"  Electrode Density =  " << density*(CLHEP::cm3/CLHEP::g) << endreq;

    GeoMaterial* Kapton_Cu = new GeoMaterial("KaptonC",density);
    Kapton_Cu->add(Copper,FracCu);
    Kapton_Cu->add(Kapton,FracKap);
    m_storedManager->addMaterial("LAr",Kapton_Cu);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "Electrode radiation length " << Kapton_Cu->getRadLength() << endreq;
#endif

    //  material for Cables/electronics (mixture of Kapton and copper)
    //  density = 2.440*CLHEP::g/CLHEP::cm3;
    //**GU get fractions per mass
    double frmassCu = dB.getDouble("BarrelAccordionCables","BarrelAccordionCables-00","PERCU");  // LArEMBmasspercentCu
    double frmassKap= dB.getDouble("BarrelAccordionCables","BarrelAccordionCables-00","PERKAP"); // LArEMBmasspercentKap
//GU 28 July 2005 recompute correctly density
    const double frmassKapOverCu = frmassKap / frmassCu;
    density = Copper->getDensity()*(1.+frmassKapOverCu)
             /(1.+frmassKapOverCu*Copper->getDensity()/Kapton->getDensity());
    GeoMaterial* Cable_elect = new GeoMaterial("Cables",density);
    double fractionmass;
    Cable_elect->add(Copper, fractionmass=frmassCu*CLHEP::perCent);
    Cable_elect->add(Kapton, fractionmass=frmassKap*CLHEP::perCent);
    m_storedManager->addMaterial("LAr", Cable_elect);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "Cable radiation length " << Cable_elect->getRadLength() << endreq;
#endif

    // material for motherboard
    // Mother_board is defined as a mixture of epox_G10 (C8 H14 O4) and Copper
    ThMBcu  = dB.getDouble("BarrelMotherboards","BarrelMotherboards-00","THICU")*CLHEP::cm;  // LArEMBCuThickness
    ThMBG10 = dB.getDouble("BarrelMotherboards","BarrelMotherboards-00","THIG10")*CLHEP::cm; // LArEMBG10Thickness
    double TotalthickMBe = ThMBcu+ThMBG10;
    double TotalmassMBe = (ThMBcu*Copper->getDensity()+ThMBG10*G10->getDensity());
    double FracMBCu = (ThMBcu*Copper->getDensity())/TotalmassMBe;
    double FracMBG10 = (ThMBG10*G10->getDensity())/TotalmassMBe;
    density = TotalmassMBe/TotalthickMBe;
    msg << MSG::DEBUG <<"---- Mother Board characteristics: ----" << endreq;
    msg << MSG::DEBUG <<"  Fraction Cu, G10: " << FracMBCu << ","
	             << FracMBG10 << endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness:"
	             << TotalmassMBe <<" ," <<TotalthickMBe<< endreq;
    msg << MSG::DEBUG <<"  M_board Density =  "<<density*(CLHEP::cm3/CLHEP::g) << endreq;
    GeoMaterial*  Moth_elect = new GeoMaterial("MBoards",density);
    // ****GU:   use fraction per masses of G10 and Cu
    Moth_elect->add(G10,FracMBG10);
    Moth_elect->add(Copper,FracMBCu);
    m_storedManager->addMaterial("LAr", Moth_elect);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "MotherBoard radiation length " << Moth_elect->getRadLength() << endreq;
#endif

//==GU  28 July 2005
//  implement latest definitions from hard coded geometry
    GeoElement* Si = m_storedManager->getElement("Silicon");
    GeoElement *O = m_storedManager->getElement("Oxygen");

    density = dB.getDouble("BarrelMotherboards", "BarrelMotherboards-00", "DG10")*(CLHEP::g/CLHEP::cm3);   //LArEMBEpoxyVolumicMass
    GeoMaterial* SiO2 = new GeoMaterial("SiO2",density);
    double fractionSi=28.09/(28.09+2*16.0);
    SiO2->add(Si,fractionSi);
    double fractionO=2.*16.0/(28.09+2.*16.0);
    SiO2->add(O,fractionO);
    SiO2->lock();
// Gten for the bars of the calorimeter= mixture of regular G10 and SiO2
    density=1.72*CLHEP::g/CLHEP::cm3;    // should be replaced by number from database
    GeoMaterial* Gten_bar = new GeoMaterial("G10_bar",density);
    Gten_bar->add(G10,0.38);    // should be replaced by number from database
    Gten_bar->add(SiO2,0.62);   // should be replaced by number from database
    m_storedManager->addMaterial("LAr",Gten_bar);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "fracionSi,fracionO2 " << fractionSi << " " << fractionO << endreq;
  msg << MSG::INFO<< "SiO2 density " << SiO2->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
  msg << MSG::INFO<< "SiO2   radiation length " << SiO2->getRadLength() << endreq;
  msg << MSG::INFO<< "G10bar radiation length " << Gten_bar->getRadLength() << endreq;
#endif

// material for the effective M_PIn+summing board effect
    double ThSBCu = 0.28*CLHEP::mm;      // should be replaced by number from database
    double ThSBAr = 9.72*CLHEP::mm;      // should be replaced by number from database
    double TotalThickSB = ThSBCu+ThSBAr;
    double dcu = Copper->getDensity();
    double dar = LAr->getDensity();
    const double TotalMassSB = ThSBCu*dcu + ThSBAr*dar;
    const double inv_TotalMassSB = 1. /TotalMassSB;
    double fracSBCu = ThSBCu*dcu*inv_TotalMassSB;
    double fracSBAr = ThSBAr*dar*inv_TotalMassSB;
    density = TotalMassSB/TotalThickSB;
    GeoMaterial* Summing_board = new GeoMaterial("SBoard",density);
    Summing_board->add(LAr,fracSBAr);
    Summing_board->add(Copper,fracSBCu);
    m_storedManager->addMaterial("LAr",Summing_board);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "SBoard radiation length " << Summing_board->getRadLength() << endreq;
#endif

//==end GU

  // Make sure these get deleted, if there's nothing referencing them.
  SiO2->ref();
  SiO2->unref();
  }

  //----------------------------------------------------------------------------------------//
  // LAR Endcap only                                                                        //
  //----------------------------------------------------------------------------------------//
  {

    // ----------------------
    // Vacuum for TB cryostat
    // ----------------------

    GeoElement *H = m_storedManager->getElement( "Hydrogen" );

    GeoMaterial *Vacuum = new GeoMaterial( "Vacuum", CLHEP::universe_mean_density );
    Vacuum->add( H, 1. );
    m_storedManager->addMaterial("LAr", Vacuum );
#ifdef DEBUGGEO
    msg << MSG::INFO<< "Vacuum radiation length " << Vacuum->getRadLength() << " "
                    << Vacuum->getDensity()/(CLHEP::g/CLHEP::cm3) << endreq;
#endif


    // ----------------------------
    // Materials for EMEC absorbers
    // ----------------------------

    double Tggl, Tgfe, Tgpb, Thgl, Thfe, Thpb;
    double Totalthick,Totalmass,Fracpb,Fracfe,Fracgl;

    // contraction from warm to cold
    double contract=dB.getDouble("ColdContraction","ColdContraction-00", "ABSORBERCONTRACTION"); // LArEMBAbsorberContraction

    //
    // EMEC thin absorbers
    //

/*  Tggl = 0.30 * CLHEP::mm;
    Tgfe = 0.40 * CLHEP::mm;
    Tgpb = 1.70 * CLHEP::mm; */

    Tggl = 0.20 * CLHEP::mm;
    Tgfe = 0.40 * CLHEP::mm;
    Tgpb = 1.69 * CLHEP::mm;

    Totalthick = Tggl+Tgfe+Tgpb;
    Totalmass = (Tgpb*Lead->getDensity()+Tgfe*Iron->getDensity()+Tggl*Glue->getDensity());
    // Fractions per mass
    Fracpb = (Tgpb*Lead->getDensity())/Totalmass;
    Fracfe = (Tgfe*Iron->getDensity())/Totalmass;
    Fracgl = (Tggl*Glue->getDensity())/Totalmass;
    double density = Totalmass/Totalthick/(contract*contract*contract);
    msg << MSG::DEBUG <<"---- EMEC THIN absorber characteristics: ----" << endreq;
    msg << MSG::DEBUG <<"  Thickness pb,fe,gl,[mm]="<<Tgpb<<" "<<Tgfe<<" "<<Tggl << endreq;
    msg << MSG::DEBUG <<"  Fraction  pb,fe,gl     ="<<Fracpb<<","<<Fracfe<<"," <<Fracgl << endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness  ="<<Totalmass<<" ," <<Totalthick << endreq;
    msg << MSG::DEBUG <<"  Thinabs Density        ="<< density*(CLHEP::cm3/CLHEP::g) << endreq;

    msg << MSG::DEBUG << "---- EMEC THIN absorber characteristics: ----" << endreq;
    msg << MSG::DEBUG <<"  Thickness pb,fe,gl,[mm]="<<Tgpb<<" "<<Tgfe<<" "<<Tggl  << endreq;
    msg << MSG::DEBUG <<"  Fraction  pb,fe,gl     ="<<Fracpb<<","<<Fracfe<<"," <<Fracgl  << endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness  ="<<Totalmass<<" ," <<Totalthick  << endreq;
    msg << MSG::DEBUG <<"  Thinabs Density        ="<< density*(CLHEP::cm3/CLHEP::g)  << endreq;


    GeoMaterial* Thin_abs = new GeoMaterial("EMEC_Thinabs",density);
    Thin_abs->add(Lead,Fracpb);
    Thin_abs->add(Iron,Fracfe);
    Thin_abs->add(Glue,Fracgl);
    m_storedManager->addMaterial("LAr", Thin_abs);

#ifdef DEBUGGEO
  msg << MSG::INFO<< "EMEC thinAbs radiation length " << Thin_abs->getRadLength() << endreq;
#endif

    //
    // EMEC thick absorbers
    //

/*    Thgl = 0.30 * CLHEP::mm;
    Thfe = 0.40 * CLHEP::mm;
    Thpb = 2.20 * CLHEP::mm; */

    Thgl = 0.20 * CLHEP::mm;
    Thfe = 0.40 * CLHEP::mm;
    Thpb = 2.20 * CLHEP::mm;

    Totalthick = Thgl+Thfe+Thpb;
    Totalmass = (Thpb*Lead->getDensity()+Thfe*Iron->getDensity()+Thgl*Glue->getDensity());
    // Fractions per mass
    Fracpb = (Thpb*Lead->getDensity())/Totalmass;
    Fracfe = (Thfe*Iron->getDensity())/Totalmass;
    Fracgl = (Thgl*Glue->getDensity())/Totalmass;
    density = Totalmass/Totalthick/(contract*contract*contract);

    msg << MSG::DEBUG <<"---- EMEC THICK absorber characteristics: ----"<<endreq;
    msg << MSG::DEBUG <<"  Thickness pb,fe,gl[mm]="<<Thpb<<" "<<Thfe<<" "<<Thgl<<endreq;
    msg << MSG::DEBUG <<"  Fraction  pb,fe,gl:    "<<Fracpb<<","<<Fracfe<<","<<Fracgl<<endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness: "<<Totalmass<<" ,"<<Totalthick<<endreq;
    msg << MSG::DEBUG <<"  Thickabs Density =     "<<density*(CLHEP::cm3/CLHEP::g) <<endreq;

    GeoMaterial* Thick_abs = new GeoMaterial("EMEC_Thickabs",density);
    Thick_abs->add(Lead,Fracpb);
    Thick_abs->add(Iron,Fracfe);
    Thick_abs->add(Glue,Fracgl);
    m_storedManager->addMaterial("LAr",Thick_abs);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "EMEC thickAbs radiation length " << Thick_abs->getRadLength() << endreq;
#endif

	//
	// EMEC shell = iron + glue, identical for inner and outer absorbers
	//
    Thgl = 0.20 * CLHEP::mm;
    Thfe = 0.40 * CLHEP::mm;

    Totalthick = Thgl+Thfe;
    Totalmass = (Thfe*Iron->getDensity()+Thgl*Glue->getDensity());
    // Fractions per mass
    Fracfe = (Thfe*Iron->getDensity())/Totalmass;
    Fracgl = (Thgl*Glue->getDensity())/Totalmass;
    density = Totalmass/Totalthick/(contract*contract*contract);

    msg << MSG::DEBUG <<"---- EMEC absorber shell characteristics: ----"<<endreq;
    msg << MSG::DEBUG <<"  Thickness fe,gl[mm]="<<Thfe<<" "<<Thgl<<endreq;
    msg << MSG::DEBUG <<"  Fraction  fe,gl:    "<<Fracfe<<","<<Fracgl<<endreq;
    msg << MSG::DEBUG <<"  Total mass, Thickness: "<<Totalmass<<" ,"<<Totalthick<<endreq;
    msg << MSG::DEBUG <<"  Thickabs Density =     "<<density*(CLHEP::cm3/CLHEP::g) <<endreq;

    GeoMaterial* EMEC_shell = new GeoMaterial("EMEC_shell",density);
    EMEC_shell->add(Iron,Fracfe);
    EMEC_shell->add(Glue,Fracgl);
    m_storedManager->addMaterial("LAr",EMEC_shell);


    // ----------------------------
    // Materials for EMEC barrettes
    // ----------------------------

    double TgG10, ThG10;
    double FracG10;

    //
    // EMEC Outer Wheel barrette
    //

    //!! Check whether G10 or G10_bar is to be used!!!!

/*    Tggl = 0.30 * CLHEP::mm;
    Tgfe = 0.40 * CLHEP::mm;
    TgG10 =1.70 * CLHEP::mm;*/

    Tggl = 0.20 * CLHEP::mm;
    Tgfe = 0.40 * CLHEP::mm;
    TgG10 =1.69 * CLHEP::mm;

    Totalthick = Tggl+Tgfe+TgG10;
    Totalmass = (TgG10*G10->getDensity()+Tgfe*Iron->getDensity()+Tggl*Glue->getDensity());
    // Fractions per mass
    FracG10 = (TgG10*G10->getDensity())/Totalmass;
    Fracfe = (Tgfe*Iron->getDensity())/Totalmass;
    Fracgl = (Tggl*Glue->getDensity())/Totalmass;
    //    density = Totalmass/Totalthick;
    density = (Totalmass/Totalthick)/(contract*contract*contract);

    GeoMaterial* G10FeOuter = new GeoMaterial("G10FeOuter",density);
    G10FeOuter->add(G10,FracG10);
    G10FeOuter->add(Iron,Fracfe);
    G10FeOuter->add(Glue,Fracgl);
    m_storedManager->addMaterial("LAr", G10FeOuter);

#ifdef DEBUGGEO
  msg << MSG::INFO<< "EMEC G10FeOuter radiation length " << G10FeOuter->getRadLength() << endreq;
#endif

    //
    // EMEC Inner Wheel barrette
    //

/*    Thgl = 0.30 * CLHEP::mm;
    Thfe = 0.40 * CLHEP::mm;
    ThG10 =2.20 * CLHEP::mm;*/

    Thgl = 0.20 * CLHEP::mm;
    Thfe = 0.40 * CLHEP::mm;
    ThG10 =2.20 * CLHEP::mm;

    Totalthick = Thgl+Thfe+ThG10;
    Totalmass = (ThG10*G10->getDensity()+Thfe*Iron->getDensity()+Thgl*Glue->getDensity());
    // Fractions per mass
    FracG10 = (ThG10*G10->getDensity())/Totalmass;
    Fracfe = (Thfe*Iron->getDensity())/Totalmass;
    Fracgl = (Thgl*Glue->getDensity())/Totalmass;
    //    density = Totalmass/Totalthick;
    density = (Totalmass/Totalthick)/(contract*contract*contract);

    GeoMaterial* G10FeInner = new GeoMaterial("G10FeInner",density);
    G10FeInner->add(G10,FracG10);
    G10FeInner->add(Iron,Fracfe);
    G10FeInner->add(Glue,Fracgl);
    m_storedManager->addMaterial("LAr",G10FeInner);
#ifdef DEBUGGEO
  msg << MSG::INFO<< "EMEC G10FeInner radiation length " << G10FeInner->getRadLength() << endreq;
#endif

  }
}
