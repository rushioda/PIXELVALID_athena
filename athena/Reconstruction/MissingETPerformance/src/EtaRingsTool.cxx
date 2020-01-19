/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/AlgFactory.h"

//#include "CLHEP/Units/PhysicalConstants.h"

#include "MissingETPerformance/EtaRingsTool.h"

//------------------------------------------------------------------------------
EtaRingsTool::EtaRingsTool( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent )
  : AthAlgTool( type, name, parent ) {
  declareInterface<EtaRingsTool>( this );

  declareProperty("FolderName",           m_folderName="");
}

//------------------------------------------------------------------------------

StatusCode EtaRingsTool::CBNT_initialize() {
  StatusCode sc = service("THistSvc", m_thistSvc);

  msg() << MSG::DEBUG << "EtaRingsTool CBNT_initialize() has been called" << endreq;

  if (sc.isFailure()) {
     msg() << MSG::ERROR
          << "Unable to retrieve pointer to THistSvc"
          << endreq;
     return sc;
  }

  //all layers
  h_MET_X_V_eta = new TH2D("h_MET_X_V_eta","MET_X_V_eta",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta ",h_MET_X_V_eta);

  h_MET_Y_V_eta = new TH2D("h_MET_Y_V_eta","MET_Y_V_eta",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta ",h_MET_Y_V_eta);

  //by layer
  h_MET_X_V_eta_PreSamplerB = new TH2D("h_MET_X_V_eta_PreSamplerB","MET_X_V_eta_PreSamplerB",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_PreSamplerB ",h_MET_X_V_eta_PreSamplerB);

  h_MET_X_V_eta_EMB1 = new TH2D("h_MET_X_V_eta_EMB1","MET_X_V_eta_EMB1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_EMB1 ",h_MET_X_V_eta_EMB1);

  h_MET_X_V_eta_EMB2 = new TH2D("h_MET_X_V_eta_EMB2","MET_X_V_eta_EMB2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_EMB2 ",h_MET_X_V_eta_EMB2);

  h_MET_X_V_eta_EMB3 = new TH2D("h_MET_X_V_eta_EMB3","MET_X_V_eta_EMB3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_EMB3 ",h_MET_X_V_eta_EMB3);

  h_MET_X_V_eta_PreSamplerE = new TH2D("h_MET_X_V_eta_PreSamplerE","MET_X_V_eta_PreSamplerE",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_PreSamplerE ",h_MET_X_V_eta_PreSamplerE);

  h_MET_X_V_eta_EME1 = new TH2D("h_MET_X_V_eta_EME1","MET_X_V_eta_EME1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_EME1 ",h_MET_X_V_eta_EME1);

  h_MET_X_V_eta_EME2 = new TH2D("h_MET_X_V_eta_EME2","MET_X_V_eta_EME2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_EME2 ",h_MET_X_V_eta_EME2);

  h_MET_X_V_eta_EME3 = new TH2D("h_MET_X_V_eta_EME3","MET_X_V_eta_EME3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_EME3 ",h_MET_X_V_eta_EME3);

  h_MET_X_V_eta_HEC0 = new TH2D("h_MET_X_V_eta_HEC0","MET_X_V_eta_HEC0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_HEC0 ",h_MET_X_V_eta_HEC0);

  h_MET_X_V_eta_HEC1 = new TH2D("h_MET_X_V_eta_HEC1","MET_X_V_eta_HEC1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_HEC1 ",h_MET_X_V_eta_HEC1);

  h_MET_X_V_eta_HEC2 = new TH2D("h_MET_X_V_eta_HEC2","MET_X_V_eta_HEC2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_HEC2 ",h_MET_X_V_eta_HEC2);

  h_MET_X_V_eta_HEC3 = new TH2D("h_MET_X_V_eta_HEC3","MET_X_V_eta_HEC3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_HEC3 ",h_MET_X_V_eta_HEC3);

  h_MET_X_V_eta_TileBar0 = new TH2D("h_MET_X_V_eta_TileBar0","MET_X_V_eta_TileBar0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileBar0 ",h_MET_X_V_eta_TileBar0);

  h_MET_X_V_eta_TileBar1 = new TH2D("h_MET_X_V_eta_TileBar1","MET_X_V_eta_TileBar1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileBar1 ",h_MET_X_V_eta_TileBar1);

  h_MET_X_V_eta_TileBar2 = new TH2D("h_MET_X_V_eta_TileBar2","MET_X_V_eta_TileBar2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileBar2 ",h_MET_X_V_eta_TileBar2);

  h_MET_X_V_eta_TileGap1 = new TH2D("h_MET_X_V_eta_TileGap1","MET_X_V_eta_TileGap1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileGap1 ",h_MET_X_V_eta_TileGap1);

  h_MET_X_V_eta_TileGap2 = new TH2D("h_MET_X_V_eta_TileGap2","MET_X_V_eta_TileGap2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileGap2 ",h_MET_X_V_eta_TileGap2);

  h_MET_X_V_eta_TileGap3 = new TH2D("h_MET_X_V_eta_TileGap3","MET_X_V_eta_TileGap3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileGap3 ",h_MET_X_V_eta_TileGap3);

  h_MET_X_V_eta_TileExt0 = new TH2D("h_MET_X_V_eta_TileExt0","MET_X_V_eta_TileExt0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileExt0 ",h_MET_X_V_eta_TileExt0);

  h_MET_X_V_eta_TileExt1 = new TH2D("h_MET_X_V_eta_TileExt1","MET_X_V_eta_TileExt1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileExt1 ",h_MET_X_V_eta_TileExt1);

  h_MET_X_V_eta_TileExt2 = new TH2D("h_MET_X_V_eta_TileExt2","MET_X_V_eta_TileExt2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_TileExt2 ",h_MET_X_V_eta_TileExt2);

  h_MET_X_V_eta_FCAL0 = new TH2D("h_MET_X_V_eta_FCAL0","MET_X_V_eta_FCAL0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_FCAL0 ",h_MET_X_V_eta_FCAL0);

  h_MET_X_V_eta_FCAL1 = new TH2D("h_MET_X_V_eta_FCAL1","MET_X_V_eta_FCAL1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_FCAL1 ",h_MET_X_V_eta_FCAL1);

  h_MET_X_V_eta_FCAL2 = new TH2D("h_MET_X_V_eta_FCAL2","MET_X_V_eta_FCAL2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_X_V_eta_FCAL2 ",h_MET_X_V_eta_FCAL2);


  h_MET_Y_V_eta_PreSamplerB = new TH2D("h_MET_Y_V_eta_PreSamplerB","MET_Y_V_eta_PreSamplerB",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_PreSamplerB ",h_MET_Y_V_eta_PreSamplerB);

  h_MET_Y_V_eta_EMB1 = new TH2D("h_MET_Y_V_eta_EMB1","MET_Y_V_eta_EMB1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_EMB1 ",h_MET_Y_V_eta_EMB1);

  h_MET_Y_V_eta_EMB2 = new TH2D("h_MET_Y_V_eta_EMB2","MET_Y_V_eta_EMB2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_EMB2 ",h_MET_Y_V_eta_EMB2);

  h_MET_Y_V_eta_EMB3 = new TH2D("h_MET_Y_V_eta_EMB3","MET_Y_V_eta_EMB3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_EMB3 ",h_MET_Y_V_eta_EMB3);

  h_MET_Y_V_eta_PreSamplerE = new TH2D("h_MET_Y_V_eta_PreSamplerE","MET_Y_V_eta_PreSamplerE",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_PreSamplerE ",h_MET_Y_V_eta_PreSamplerE);

  h_MET_Y_V_eta_EME1 = new TH2D("h_MET_Y_V_eta_EME1","MET_Y_V_eta_EME1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_EME1 ",h_MET_Y_V_eta_EME1);

  h_MET_Y_V_eta_EME2 = new TH2D("h_MET_Y_V_eta_EME2","MET_Y_V_eta_EME2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_EME2 ",h_MET_Y_V_eta_EME2);

  h_MET_Y_V_eta_EME3 = new TH2D("h_MET_Y_V_eta_EME3","MET_Y_V_eta_EME3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_EME3 ",h_MET_Y_V_eta_EME3);

  h_MET_Y_V_eta_HEC0 = new TH2D("h_MET_Y_V_eta_HEC0","MET_Y_V_eta_HEC0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_HEC0 ",h_MET_Y_V_eta_HEC0);

  h_MET_Y_V_eta_HEC1 = new TH2D("h_MET_Y_V_eta_HEC1","MET_Y_V_eta_HEC1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_HEC1 ",h_MET_Y_V_eta_HEC1);

  h_MET_Y_V_eta_HEC2 = new TH2D("h_MET_Y_V_eta_HEC2","MET_Y_V_eta_HEC2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_HEC2 ",h_MET_Y_V_eta_HEC2);

  h_MET_Y_V_eta_HEC3 = new TH2D("h_MET_Y_V_eta_HEC3","MET_Y_V_eta_HEC3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_HEC3 ",h_MET_Y_V_eta_HEC3);

  h_MET_Y_V_eta_TileBar0 = new TH2D("h_MET_Y_V_eta_TileBar0","MET_Y_V_eta_TileBar0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileBar0 ",h_MET_Y_V_eta_TileBar0);

  h_MET_Y_V_eta_TileBar1 = new TH2D("h_MET_Y_V_eta_TileBar1","MET_Y_V_eta_TileBar1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileBar1 ",h_MET_Y_V_eta_TileBar1);

  h_MET_Y_V_eta_TileBar2 = new TH2D("h_MET_Y_V_eta_TileBar2","MET_Y_V_eta_TileBar2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileBar2 ",h_MET_Y_V_eta_TileBar2);

  h_MET_Y_V_eta_TileGap1 = new TH2D("h_MET_Y_V_eta_TileGap1","MET_Y_V_eta_TileGap1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileGap1 ",h_MET_Y_V_eta_TileGap1);

  h_MET_Y_V_eta_TileGap2 = new TH2D("h_MET_Y_V_eta_TileGap2","MET_Y_V_eta_TileGap2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileGap2 ",h_MET_Y_V_eta_TileGap2);

  h_MET_Y_V_eta_TileGap3 = new TH2D("h_MET_Y_V_eta_TileGap3","MET_Y_V_eta_TileGap3",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileGap3 ",h_MET_Y_V_eta_TileGap3);

  h_MET_Y_V_eta_TileExt0 = new TH2D("h_MET_Y_V_eta_TileExt0","MET_Y_V_eta_TileExt0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileExt0 ",h_MET_Y_V_eta_TileExt0);

  h_MET_Y_V_eta_TileExt1 = new TH2D("h_MET_Y_V_eta_TileExt1","MET_Y_V_eta_TileExt1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileExt1 ",h_MET_Y_V_eta_TileExt1);

  h_MET_Y_V_eta_TileExt2 = new TH2D("h_MET_Y_V_eta_TileExt2","MET_Y_V_eta_TileExt2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_TileExt2 ",h_MET_Y_V_eta_TileExt2);

  h_MET_Y_V_eta_FCAL0 = new TH2D("h_MET_Y_V_eta_FCAL0","MET_Y_V_eta_FCAL0",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_FCAL0 ",h_MET_Y_V_eta_FCAL0);

  h_MET_Y_V_eta_FCAL1 = new TH2D("h_MET_Y_V_eta_FCAL1","MET_Y_V_eta_FCAL1",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_FCAL1 ",h_MET_Y_V_eta_FCAL1);

  h_MET_Y_V_eta_FCAL2 = new TH2D("h_MET_Y_V_eta_FCAL2","MET_Y_V_eta_FCAL2",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET_Y_V_eta_FCAL2 ",h_MET_Y_V_eta_FCAL2);

  ///////////////////////////////////////////////////////////////////////
  ///n sigma/////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  //all layers
  h_MET_X_V_eta_nsigma = new TH2D("h_MET_X_V_eta_nsigma","MET_X_V_eta_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_nsigma ",h_MET_X_V_eta_nsigma);

  h_MET_Y_V_eta_nsigma = new TH2D("h_MET_Y_V_eta_nsigma","MET_Y_V_eta_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_nsigma ",h_MET_Y_V_eta_nsigma);

  //by layer
  h_MET_X_V_eta_PreSamplerB_nsigma = new TH2D("h_MET_X_V_eta_PreSamplerB_nsigma","MET_X_V_eta_PreSamplerB_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_PreSamplerB_nsigma ",h_MET_X_V_eta_PreSamplerB_nsigma);

  h_MET_X_V_eta_EMB1_nsigma = new TH2D("h_MET_X_V_eta_EMB1_nsigma","MET_X_V_eta_EMB1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_EMB1_nsigma ",h_MET_X_V_eta_EMB1_nsigma);

  h_MET_X_V_eta_EMB2_nsigma = new TH2D("h_MET_X_V_eta_EMB2_nsigma","MET_X_V_eta_EMB2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_EMB2_nsigma ",h_MET_X_V_eta_EMB2_nsigma);

  h_MET_X_V_eta_EMB3_nsigma = new TH2D("h_MET_X_V_eta_EMB3_nsigma","MET_X_V_eta_EMB3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_EMB3_nsigma ",h_MET_X_V_eta_EMB3_nsigma);

  h_MET_X_V_eta_PreSamplerE_nsigma = new TH2D("h_MET_X_V_eta_PreSamplerE_nsigma","MET_X_V_eta_PreSamplerE_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_PreSamplerE_nsigma ",h_MET_X_V_eta_PreSamplerE_nsigma);

  h_MET_X_V_eta_EME1_nsigma = new TH2D("h_MET_X_V_eta_EME1_nsigma","MET_X_V_eta_EME1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_EME1_nsigma ",h_MET_X_V_eta_EME1_nsigma);

  h_MET_X_V_eta_EME2_nsigma = new TH2D("h_MET_X_V_eta_EME2_nsigma","MET_X_V_eta_EME2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_EME2_nsigma ",h_MET_X_V_eta_EME2_nsigma);

  h_MET_X_V_eta_EME3_nsigma = new TH2D("h_MET_X_V_eta_EME3_nsigma","MET_X_V_eta_EME3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_EME3_nsigma ",h_MET_X_V_eta_EME3_nsigma);

  h_MET_X_V_eta_HEC0_nsigma = new TH2D("h_MET_X_V_eta_HEC0_nsigma","MET_X_V_eta_HEC0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_HEC0_nsigma ",h_MET_X_V_eta_HEC0_nsigma);

  h_MET_X_V_eta_HEC1_nsigma = new TH2D("h_MET_X_V_eta_HEC1_nsigma","MET_X_V_eta_HEC1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_HEC1_nsigma ",h_MET_X_V_eta_HEC1_nsigma);

  h_MET_X_V_eta_HEC2_nsigma = new TH2D("h_MET_X_V_eta_HEC2_nsigma","MET_X_V_eta_HEC2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_HEC2_nsigma ",h_MET_X_V_eta_HEC2_nsigma);

  h_MET_X_V_eta_HEC3_nsigma = new TH2D("h_MET_X_V_eta_HEC3_nsigma","MET_X_V_eta_HEC3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_HEC3_nsigma ",h_MET_X_V_eta_HEC3_nsigma);

  h_MET_X_V_eta_TileBar0_nsigma = new TH2D("h_MET_X_V_eta_TileBar0_nsigma","MET_X_V_eta_TileBar0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileBar0_nsigma ",h_MET_X_V_eta_TileBar0_nsigma);

  h_MET_X_V_eta_TileBar1_nsigma = new TH2D("h_MET_X_V_eta_TileBar1_nsigma","MET_X_V_eta_TileBar1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileBar1_nsigma ",h_MET_X_V_eta_TileBar1_nsigma);

  h_MET_X_V_eta_TileBar2_nsigma = new TH2D("h_MET_X_V_eta_TileBar2_nsigma","MET_X_V_eta_TileBar2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileBar2_nsigma ",h_MET_X_V_eta_TileBar2_nsigma);

  h_MET_X_V_eta_TileGap1_nsigma = new TH2D("h_MET_X_V_eta_TileGap1_nsigma","MET_X_V_eta_TileGap1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileGap1_nsigma ",h_MET_X_V_eta_TileGap1_nsigma);

  h_MET_X_V_eta_TileGap2_nsigma = new TH2D("h_MET_X_V_eta_TileGap2_nsigma","MET_X_V_eta_TileGap2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileGap2_nsigma ",h_MET_X_V_eta_TileGap2_nsigma);

  h_MET_X_V_eta_TileGap3_nsigma = new TH2D("h_MET_X_V_eta_TileGap3_nsigma","MET_X_V_eta_TileGap3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileGap3_nsigma ",h_MET_X_V_eta_TileGap3_nsigma);

  h_MET_X_V_eta_TileExt0_nsigma = new TH2D("h_MET_X_V_eta_TileExt0_nsigma","MET_X_V_eta_TileExt0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileExt0_nsigma ",h_MET_X_V_eta_TileExt0_nsigma);

  h_MET_X_V_eta_TileExt1_nsigma = new TH2D("h_MET_X_V_eta_TileExt1_nsigma","MET_X_V_eta_TileExt1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileExt1_nsigma ",h_MET_X_V_eta_TileExt1_nsigma);

  h_MET_X_V_eta_TileExt2_nsigma = new TH2D("h_MET_X_V_eta_TileExt2_nsigma","MET_X_V_eta_TileExt2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_TileExt2_nsigma ",h_MET_X_V_eta_TileExt2_nsigma);

  h_MET_X_V_eta_FCAL0_nsigma = new TH2D("h_MET_X_V_eta_FCAL0_nsigma","MET_X_V_eta_FCAL0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_FCAL0_nsigma ",h_MET_X_V_eta_FCAL0_nsigma);

  h_MET_X_V_eta_FCAL1_nsigma = new TH2D("h_MET_X_V_eta_FCAL1_nsigma","MET_X_V_eta_FCAL1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_FCAL1_nsigma ",h_MET_X_V_eta_FCAL1_nsigma);

  h_MET_X_V_eta_FCAL2_nsigma = new TH2D("h_MET_X_V_eta_FCAL2_nsigma","MET_X_V_eta_FCAL2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_X_V_eta_FCAL2_nsigma ",h_MET_X_V_eta_FCAL2_nsigma);


  h_MET_Y_V_eta_PreSamplerB_nsigma = new TH2D("h_MET_Y_V_eta_PreSamplerB_nsigma","MET_Y_V_eta_PreSamplerB_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_PreSamplerB_nsigma ",h_MET_Y_V_eta_PreSamplerB_nsigma);

  h_MET_Y_V_eta_EMB1_nsigma = new TH2D("h_MET_Y_V_eta_EMB1_nsigma","MET_Y_V_eta_EMB1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_EMB1_nsigma ",h_MET_Y_V_eta_EMB1_nsigma);

  h_MET_Y_V_eta_EMB2_nsigma = new TH2D("h_MET_Y_V_eta_EMB2_nsigma","MET_Y_V_eta_EMB2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_EMB2_nsigma ",h_MET_Y_V_eta_EMB2_nsigma);

  h_MET_Y_V_eta_EMB3_nsigma = new TH2D("h_MET_Y_V_eta_EMB3_nsigma","MET_Y_V_eta_EMB3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_EMB3_nsigma ",h_MET_Y_V_eta_EMB3_nsigma);

  h_MET_Y_V_eta_PreSamplerE_nsigma = new TH2D("h_MET_Y_V_eta_PreSamplerE_nsigma","MET_Y_V_eta_PreSamplerE_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_PreSamplerE_nsigma ",h_MET_Y_V_eta_PreSamplerE_nsigma);

  h_MET_Y_V_eta_EME1_nsigma = new TH2D("h_MET_Y_V_eta_EME1_nsigma","MET_Y_V_eta_EME1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_EME1_nsigma ",h_MET_Y_V_eta_EME1_nsigma);

  h_MET_Y_V_eta_EME2_nsigma = new TH2D("h_MET_Y_V_eta_EME2_nsigma","MET_Y_V_eta_EME2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_EME2_nsigma ",h_MET_Y_V_eta_EME2_nsigma);

  h_MET_Y_V_eta_EME3_nsigma = new TH2D("h_MET_Y_V_eta_EME3_nsigma","MET_Y_V_eta_EME3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_EME3_nsigma ",h_MET_Y_V_eta_EME3_nsigma);

  h_MET_Y_V_eta_HEC0_nsigma = new TH2D("h_MET_Y_V_eta_HEC0_nsigma","MET_Y_V_eta_HEC0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_HEC0_nsigma ",h_MET_Y_V_eta_HEC0_nsigma);

  h_MET_Y_V_eta_HEC1_nsigma = new TH2D("h_MET_Y_V_eta_HEC1_nsigma","MET_Y_V_eta_HEC1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_HEC1_nsigma ",h_MET_Y_V_eta_HEC1_nsigma);

  h_MET_Y_V_eta_HEC2_nsigma = new TH2D("h_MET_Y_V_eta_HEC2_nsigma","MET_Y_V_eta_HEC2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_HEC2_nsigma ",h_MET_Y_V_eta_HEC2_nsigma);

  h_MET_Y_V_eta_HEC3_nsigma = new TH2D("h_MET_Y_V_eta_HEC3_nsigma","MET_Y_V_eta_HEC3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_HEC3_nsigma ",h_MET_Y_V_eta_HEC3_nsigma);

  h_MET_Y_V_eta_TileBar0_nsigma = new TH2D("h_MET_Y_V_eta_TileBar0_nsigma","MET_Y_V_eta_TileBar0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileBar0_nsigma ",h_MET_Y_V_eta_TileBar0_nsigma);

  h_MET_Y_V_eta_TileBar1_nsigma = new TH2D("h_MET_Y_V_eta_TileBar1_nsigma","MET_Y_V_eta_TileBar1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileBar1_nsigma ",h_MET_Y_V_eta_TileBar1_nsigma);

  h_MET_Y_V_eta_TileBar2_nsigma = new TH2D("h_MET_Y_V_eta_TileBar2_nsigma","MET_Y_V_eta_TileBar2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileBar2_nsigma ",h_MET_Y_V_eta_TileBar2_nsigma);

  h_MET_Y_V_eta_TileGap1_nsigma = new TH2D("h_MET_Y_V_eta_TileGap1_nsigma","MET_Y_V_eta_TileGap1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileGap1_nsigma ",h_MET_Y_V_eta_TileGap1_nsigma);

  h_MET_Y_V_eta_TileGap2_nsigma = new TH2D("h_MET_Y_V_eta_TileGap2_nsigma","MET_Y_V_eta_TileGap2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileGap2_nsigma ",h_MET_Y_V_eta_TileGap2_nsigma);

  h_MET_Y_V_eta_TileGap3_nsigma = new TH2D("h_MET_Y_V_eta_TileGap3_nsigma","MET_Y_V_eta_TileGap3_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileGap3_nsigma ",h_MET_Y_V_eta_TileGap3_nsigma);

  h_MET_Y_V_eta_TileExt0_nsigma = new TH2D("h_MET_Y_V_eta_TileExt0_nsigma","MET_Y_V_eta_TileExt0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileExt0_nsigma ",h_MET_Y_V_eta_TileExt0_nsigma);

  h_MET_Y_V_eta_TileExt1_nsigma = new TH2D("h_MET_Y_V_eta_TileExt1_nsigma","MET_Y_V_eta_TileExt1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileExt1_nsigma ",h_MET_Y_V_eta_TileExt1_nsigma);

  h_MET_Y_V_eta_TileExt2_nsigma = new TH2D("h_MET_Y_V_eta_TileExt2_nsigma","MET_Y_V_eta_TileExt2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_TileExt2_nsigma ",h_MET_Y_V_eta_TileExt2_nsigma);

  h_MET_Y_V_eta_FCAL0_nsigma = new TH2D("h_MET_Y_V_eta_FCAL0_nsigma","MET_Y_V_eta_FCAL0_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_FCAL0_nsigma ",h_MET_Y_V_eta_FCAL0_nsigma);

  h_MET_Y_V_eta_FCAL1_nsigma = new TH2D("h_MET_Y_V_eta_FCAL1_nsigma","MET_Y_V_eta_FCAL1_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_FCAL1_nsigma ",h_MET_Y_V_eta_FCAL1_nsigma);

  h_MET_Y_V_eta_FCAL2_nsigma = new TH2D("h_MET_Y_V_eta_FCAL2_nsigma","MET_Y_V_eta_FCAL2_nsigma",100,-5.,5.,1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_Y_V_eta_FCAL2_nsigma ",h_MET_Y_V_eta_FCAL2_nsigma);

  ///////////////////////////////////////////////////
  //event MET plots
  ///////////////////////////////////////////////////
  h_METx = new TH1D("h_METx","METx",1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/METx",h_METx);

  h_METy = new TH1D("h_METy","METy",1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/METy",h_METy);

  h_METphi = new TH1D("h_METphi","METphi",63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/METphi",h_METphi);

  h_MET = new TH1D("h_MET","MET",1000,0.0,100.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/MET",h_MET);

  h_METx_nsigma = new TH1D("h_METx_nsigma","METx_nsigma",1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/METx_nsigma",h_METx_nsigma);

  h_METy_nsigma = new TH1D("h_METy_nsigma","METy_nsigma",1000,-50.0,50.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/METy_nsigma",h_METy_nsigma);

  h_METphi_nsigma = new TH1D("h_METphi_nsigma","METphi_nsigma",63,-3.15,3.15);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/METphi_nsigma",h_METphi_nsigma);

  h_MET_nsigma = new TH1D("h_MET_nsigma","MET_nsigma",1000,0.0,100.0);
  sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/MET_nsigma",h_MET_nsigma);

  std::string sample_names[24] = {
    "PreSamplerB",
    "EMB1",
    "EMB2",
    "EMB3",
    "PreSamplerE",
    "EME1",
    "EME2",
    "EME3",
    "HEC0",
    "HEC1",
    "HEC2",
    "HEC3",
    "TileBar0",
    "TileBar1",
    "TileBar2",
    "TileGap1",
    "TileGap2",
    "TileGap3",
    "TileExt0",
    "TileExt1",
    "TileExt2",
    "FCAL0",
    "FCAL1",
    "FCAL2"};

  std::string name1;
  std::string name2;
  std::string name3;
  std::string name4;
  std::string name5;
  std::string name6;
  std::string name7;
  std::string name8;

  for (int ih= 0; ih < 24 ; ih++){
    name1 = std::string("h_METx_") + sample_names[ih];
    h_METx_bl.push_back(new TH1D(name1.c_str(), name1.c_str() , 1000,-50.0,50.0));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/" + name1,h_METx_bl.at(ih));

    name2 = std::string("h_METy_") + sample_names[ih];
    h_METy_bl.push_back(new TH1D(name2.c_str(), name2.c_str() , 1000,-50.0,50.0));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/" + name2,h_METy_bl.at(ih));

    name7 = std::string("h_METphi_") + sample_names[ih];
    h_METphi_bl.push_back(new TH1D(name7.c_str(), name7.c_str() , 63,-3.15,3.15));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/" + name7,h_METphi_bl.at(ih));

    name3 = std::string("h_MET_") + sample_names[ih];
    h_MET_bl.push_back(new TH1D(name3.c_str(), name3.c_str() , 1000,-50.0,50.0));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/" + name3,h_MET_bl.at(ih));

    name4 = std::string("h_METx_nsigma_") + sample_names[ih];
    h_METx_nsigma_bl.push_back(new TH1D(name4.c_str(), name4.c_str() , 1000,-50.0,50.0));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/" + name4,h_METx_nsigma_bl.at(ih));

    name5 = std::string("h_METy_nsigma_") + sample_names[ih];
    h_METy_nsigma_bl.push_back(new TH1D(name5.c_str(), name5.c_str() , 1000,-50.0,50.0));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/" + name5,h_METy_nsigma_bl.at(ih));

    name8 = std::string("h_METphi_nsigma_") + sample_names[ih];
    h_METphi_nsigma_bl.push_back(new TH1D(name8.c_str(), name8.c_str() , 63,-3.15,3.15));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/" + name8,h_METphi_nsigma_bl.at(ih));

    name6 = std::string("h_MET_nsigma_") + sample_names[ih];
    h_MET_nsigma_bl.push_back(new TH1D(name6.c_str(), name6.c_str() , 1000,-50.0,50.0));
    sc = m_thistSvc->regHist("/AANT/" + m_folderName + "EtaRings/NSigma/" + name6,h_MET_nsigma_bl.at(ih));
  }
  if (sc.isFailure()) {}
  return sc;
}

StatusCode EtaRingsTool::initialize() {
  msg() << MSG::DEBUG << "EtaRingsTool initialize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode EtaRingsTool::execute(MissingETData *data) {

  /** check that the input and the output containers are defined */
  StatusCode sc = StatusCode::SUCCESS;

  msg() << MSG::DEBUG << "EtaRingsTool execute() has been called" << endreq;

  sc = etaRings(data);

  if (sc.isFailure()) {}
  return sc;
}

StatusCode EtaRingsTool::etaRings(MissingETData *data) {

  msg() << MSG::DEBUG << "in etaRings() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  double metx = data->met_from_calo_cells_x();
  double mety = data->met_from_calo_cells_y();
  double metphi = atan2(data->met_from_calo_cells_y(),data->met_from_calo_cells_x());
  double met = sqrt(pow(metx,2) + pow(mety,2));
  double metx_nsigma = data->met_from_calo_cells_sigmacut_x();
  double mety_nsigma = data->met_from_calo_cells_sigmacut_y();
  double metphi_nsigma = atan2(data->met_from_calo_cells_sigmacut_y(),data->met_from_calo_cells_sigmacut_x());
  double met_nsigma = sqrt(pow(metx_nsigma,2) + pow(mety_nsigma,2));

  h_METx->Fill(metx/1000.);
  h_METy->Fill(mety/1000.);
  h_METphi->Fill(metphi);
  h_MET->Fill(met/1000.);

  h_METx_nsigma->Fill(metx_nsigma/1000.);
  h_METy_nsigma->Fill(mety_nsigma/1000.);
  h_METphi_nsigma->Fill(metphi_nsigma);
  h_MET_nsigma->Fill(met_nsigma/1000.);

  for (unsigned int i = 0; i < 24; ++i) {
    double metx_bl = data->met_from_calo_cells_x_by_layer(i);
    double mety_bl = data->met_from_calo_cells_y_by_layer(i);
    double metphi_bl = atan2(mety_bl,metx_bl);
    double met_bl = sqrt(pow(metx_bl,2) + pow(mety_bl,2));
    double metx_nsigma_bl = data->met_from_calo_cells_sigmacut_x_by_layer(i);
    double mety_nsigma_bl = data->met_from_calo_cells_sigmacut_y_by_layer(i);
    double metphi_nsigma_bl = atan2(mety_nsigma_bl,metx_nsigma_bl);
    double met_nsigma_bl = sqrt(pow(metx_nsigma_bl,2) + pow(mety_nsigma_bl,2));

    h_METx_bl.at(i)->Fill(metx_bl/1000.);
    h_METy_bl.at(i)->Fill(mety_bl/1000.);
    h_METphi_bl.at(i)->Fill(metphi_bl);
    h_MET_bl.at(i)->Fill(met_bl/1000.);

    h_METx_nsigma_bl.at(i)->Fill(metx_nsigma_bl/1000.);
    h_METy_nsigma_bl.at(i)->Fill(mety_nsigma_bl/1000.);
    h_METphi_nsigma_bl.at(i)->Fill(metphi_nsigma_bl);
    h_MET_nsigma_bl.at(i)->Fill(met_nsigma_bl/1000.);
  }

  for (int i=0; i < 100; ++i) {
    double eta = ((double)i)/10.0 - 4.95;

    h_MET_X_V_eta->Fill(eta,data->eta_rings_all_samples(i)->etx()/1000.);
    h_MET_Y_V_eta->Fill(eta,data->eta_rings_all_samples(i)->ety()/1000.);

    h_MET_X_V_eta_PreSamplerB->Fill(eta,data->eta_rings_by_sample(0,i)->etx()/1000.);
    h_MET_X_V_eta_EMB1->Fill(eta,data->eta_rings_by_sample(1,i)->etx()/1000.);
    h_MET_X_V_eta_EMB2->Fill(eta,data->eta_rings_by_sample(2,i)->etx()/1000.);
    h_MET_X_V_eta_EMB3->Fill(eta,data->eta_rings_by_sample(3,i)->etx()/1000.);
    h_MET_X_V_eta_PreSamplerE->Fill(eta,data->eta_rings_by_sample(4,i)->etx()/1000.);
    h_MET_X_V_eta_EME1->Fill(eta,data->eta_rings_by_sample(5,i)->etx()/1000.);
    h_MET_X_V_eta_EME2->Fill(eta,data->eta_rings_by_sample(6,i)->etx()/1000.);
    h_MET_X_V_eta_EME3->Fill(eta,data->eta_rings_by_sample(7,i)->etx()/1000.);
    h_MET_X_V_eta_HEC0->Fill(eta,data->eta_rings_by_sample(8,i)->etx()/1000.);
    h_MET_X_V_eta_HEC1->Fill(eta,data->eta_rings_by_sample(9,i)->etx()/1000.);
    h_MET_X_V_eta_HEC2->Fill(eta,data->eta_rings_by_sample(10,i)->etx()/1000.);
    h_MET_X_V_eta_HEC3->Fill(eta,data->eta_rings_by_sample(11,i)->etx()/1000.);
    h_MET_X_V_eta_TileBar0->Fill(eta,data->eta_rings_by_sample(12,i)->etx()/1000.);
    h_MET_X_V_eta_TileBar1->Fill(eta,data->eta_rings_by_sample(13,i)->etx()/1000.);
    h_MET_X_V_eta_TileBar2->Fill(eta,data->eta_rings_by_sample(14,i)->etx()/1000.);
    h_MET_X_V_eta_TileGap1->Fill(eta,data->eta_rings_by_sample(15,i)->etx()/1000.);
    h_MET_X_V_eta_TileGap2->Fill(eta,data->eta_rings_by_sample(16,i)->etx()/1000.);
    h_MET_X_V_eta_TileGap3->Fill(eta,data->eta_rings_by_sample(17,i)->etx()/1000.);
    h_MET_X_V_eta_TileExt0->Fill(eta,data->eta_rings_by_sample(18,i)->etx()/1000.);
    h_MET_X_V_eta_TileExt1->Fill(eta,data->eta_rings_by_sample(19,i)->etx()/1000.);
    h_MET_X_V_eta_TileExt2->Fill(eta,data->eta_rings_by_sample(20,i)->etx()/1000.);
    h_MET_X_V_eta_FCAL0->Fill(eta,data->eta_rings_by_sample(21,i)->etx()/1000.);
    h_MET_X_V_eta_FCAL1->Fill(eta,data->eta_rings_by_sample(22,i)->etx()/1000.);
    h_MET_X_V_eta_FCAL2->Fill(eta,data->eta_rings_by_sample(23,i)->etx()/1000.);

    h_MET_Y_V_eta_PreSamplerB->Fill(eta,data->eta_rings_by_sample(0,i)->ety()/1000.);
    h_MET_Y_V_eta_EMB1->Fill(eta,data->eta_rings_by_sample(1,i)->ety()/1000.);
    h_MET_Y_V_eta_EMB2->Fill(eta,data->eta_rings_by_sample(2,i)->ety()/1000.);
    h_MET_Y_V_eta_EMB3->Fill(eta,data->eta_rings_by_sample(3,i)->ety()/1000.);
    h_MET_Y_V_eta_PreSamplerE->Fill(eta,data->eta_rings_by_sample(4,i)->ety()/1000.);
    h_MET_Y_V_eta_EME1->Fill(eta,data->eta_rings_by_sample(5,i)->ety()/1000.);
    h_MET_Y_V_eta_EME2->Fill(eta,data->eta_rings_by_sample(6,i)->ety()/1000.);
    h_MET_Y_V_eta_EME3->Fill(eta,data->eta_rings_by_sample(7,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC0->Fill(eta,data->eta_rings_by_sample(8,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC1->Fill(eta,data->eta_rings_by_sample(9,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC2->Fill(eta,data->eta_rings_by_sample(10,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC3->Fill(eta,data->eta_rings_by_sample(11,i)->ety()/1000.);
    h_MET_Y_V_eta_TileBar0->Fill(eta,data->eta_rings_by_sample(12,i)->ety()/1000.);
    h_MET_Y_V_eta_TileBar1->Fill(eta,data->eta_rings_by_sample(13,i)->ety()/1000.);
    h_MET_Y_V_eta_TileBar2->Fill(eta,data->eta_rings_by_sample(14,i)->ety()/1000.);
    h_MET_Y_V_eta_TileGap1->Fill(eta,data->eta_rings_by_sample(15,i)->ety()/1000.);
    h_MET_Y_V_eta_TileGap2->Fill(eta,data->eta_rings_by_sample(16,i)->ety()/1000.);
    h_MET_Y_V_eta_TileGap3->Fill(eta,data->eta_rings_by_sample(17,i)->ety()/1000.);
    h_MET_Y_V_eta_TileExt0->Fill(eta,data->eta_rings_by_sample(18,i)->ety()/1000.);
    h_MET_Y_V_eta_TileExt1->Fill(eta,data->eta_rings_by_sample(19,i)->ety()/1000.);
    h_MET_Y_V_eta_TileExt2->Fill(eta,data->eta_rings_by_sample(20,i)->ety()/1000.);
    h_MET_Y_V_eta_FCAL0->Fill(eta,data->eta_rings_by_sample(21,i)->ety()/1000.);
    h_MET_Y_V_eta_FCAL1->Fill(eta,data->eta_rings_by_sample(22,i)->ety()/1000.);
    h_MET_Y_V_eta_FCAL2->Fill(eta,data->eta_rings_by_sample(23,i)->ety()/1000.);

    ///////////////////////////////////////////////////////////////////////
    ///n sigma/////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    h_MET_X_V_eta_nsigma->Fill(eta,data->eta_rings_all_samples_nsigma(i)->etx()/1000.);
    h_MET_Y_V_eta_nsigma->Fill(eta,data->eta_rings_all_samples_nsigma(i)->ety()/1000.);

    h_MET_X_V_eta_PreSamplerB_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(0,i)->etx()/1000.);
    h_MET_X_V_eta_EMB1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(1,i)->etx()/1000.);
    h_MET_X_V_eta_EMB2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(2,i)->etx()/1000.);
    h_MET_X_V_eta_EMB3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(3,i)->etx()/1000.);
    h_MET_X_V_eta_PreSamplerE_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(4,i)->etx()/1000.);
    h_MET_X_V_eta_EME1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(5,i)->etx()/1000.);
    h_MET_X_V_eta_EME2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(6,i)->etx()/1000.);
    h_MET_X_V_eta_EME3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(7,i)->etx()/1000.);
    h_MET_X_V_eta_HEC0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(8,i)->etx()/1000.);
    h_MET_X_V_eta_HEC1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(9,i)->etx()/1000.);
    h_MET_X_V_eta_HEC2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(10,i)->etx()/1000.);
    h_MET_X_V_eta_HEC3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(11,i)->etx()/1000.);
    h_MET_X_V_eta_TileBar0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(12,i)->etx()/1000.);
    h_MET_X_V_eta_TileBar1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(13,i)->etx()/1000.);
    h_MET_X_V_eta_TileBar2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(14,i)->etx()/1000.);
    h_MET_X_V_eta_TileGap1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(15,i)->etx()/1000.);
    h_MET_X_V_eta_TileGap2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(16,i)->etx()/1000.);
    h_MET_X_V_eta_TileGap3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(17,i)->etx()/1000.);
    h_MET_X_V_eta_TileExt0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(18,i)->etx()/1000.);
    h_MET_X_V_eta_TileExt1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(19,i)->etx()/1000.);
    h_MET_X_V_eta_TileExt2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(20,i)->etx()/1000.);
    h_MET_X_V_eta_FCAL0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(21,i)->etx()/1000.);
    h_MET_X_V_eta_FCAL1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(22,i)->etx()/1000.);
    h_MET_X_V_eta_FCAL2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(23,i)->etx()/1000.);

    h_MET_Y_V_eta_PreSamplerB_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(0,i)->ety()/1000.);
    h_MET_Y_V_eta_EMB1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(1,i)->ety()/1000.);
    h_MET_Y_V_eta_EMB2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(2,i)->ety()/1000.);
    h_MET_Y_V_eta_EMB3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(3,i)->ety()/1000.);
    h_MET_Y_V_eta_PreSamplerE_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(4,i)->ety()/1000.);
    h_MET_Y_V_eta_EME1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(5,i)->ety()/1000.);
    h_MET_Y_V_eta_EME2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(6,i)->ety()/1000.);
    h_MET_Y_V_eta_EME3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(7,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(8,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(9,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(10,i)->ety()/1000.);
    h_MET_Y_V_eta_HEC3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(11,i)->ety()/1000.);
    h_MET_Y_V_eta_TileBar0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(12,i)->ety()/1000.);
    h_MET_Y_V_eta_TileBar1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(13,i)->ety()/1000.);
    h_MET_Y_V_eta_TileBar2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(14,i)->ety()/1000.);
    h_MET_Y_V_eta_TileGap1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(15,i)->ety()/1000.);
    h_MET_Y_V_eta_TileGap2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(16,i)->ety()/1000.);
    h_MET_Y_V_eta_TileGap3_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(17,i)->ety()/1000.);
    h_MET_Y_V_eta_TileExt0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(18,i)->ety()/1000.);
    h_MET_Y_V_eta_TileExt1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(19,i)->ety()/1000.);
    h_MET_Y_V_eta_TileExt2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(20,i)->ety()/1000.);
    h_MET_Y_V_eta_FCAL0_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(21,i)->ety()/1000.);
    h_MET_Y_V_eta_FCAL1_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(22,i)->ety()/1000.);
    h_MET_Y_V_eta_FCAL2_nsigma->Fill(eta,data->eta_rings_by_sample_nsigma(23,i)->ety()/1000.);

  }//loop over eta rings

  if (sc.isFailure()) {}
  return sc;
}//end etaRings()

//------------------------------------------------------------------------------
StatusCode EtaRingsTool::finalize() {
  msg() << MSG::DEBUG << "EtaRingsTool finalize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
EtaRingsTool::~EtaRingsTool() {}
