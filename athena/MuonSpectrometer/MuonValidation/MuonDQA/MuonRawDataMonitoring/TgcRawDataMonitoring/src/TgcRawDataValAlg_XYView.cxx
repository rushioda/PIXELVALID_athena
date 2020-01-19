/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcRawDataValAlg
// Authors:  N. Benekos(Illinois), M. Ishino(ICEPP), H.Kurashige (Kobe), A.Ishikawa (Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

// Cabling Service
//#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "Identifier/Identifier.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
//#include "MuonDQAUtils/TGCDQAUtils.h"
 
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <TError.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
#include <math.h>

using namespace std;

StatusCode
TgcRawDataValAlg::bookHistogramsXYView(){
  //Following Histograms are booked
  //
  // XY_View_A
  // XY_View_C
  // XY_View_A_Layer[1-9]
  // XY_View_C_Layer[1-9]

  m_log << MSG::INFO << "bookHistogramsXYView" << endreq;       

  StatusCode sc=StatusCode::SUCCESS;

  MonGroup tgcprd_shift_a( this, generic_path_tgcmonitoring + "/TGCEA", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_shift_c( this, generic_path_tgcmonitoring + "/TGCEC", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_shift_ac[2]={ &tgcprd_shift_a, &tgcprd_shift_c };
  MonGroup tgcprd_expert_a( this, generic_path_tgcmonitoring + "/TGCEA", run, ATTRIB_UNMANAGED ); 
  MonGroup tgcprd_expert_c( this, generic_path_tgcmonitoring + "/TGCEC", run, ATTRIB_UNMANAGED ); 
  MonGroup* tgcprd_expert_ac[2]={ &tgcprd_expert_a, &tgcprd_expert_c };
  
  std::stringstream ss;
  std::string side[2] = {"A","C"};
  
  //====XY view====
  //    A/C side : overlay all layers
  for( int ac=0 ; ac<2 ; ac++ ){
    ss.str(""); ss<< "XY_View_" << side[ac];
    tgcxyview[ac] = new TH2F( ss.str().c_str(), (ss.str() + ";X [cm]; Y[cm]").c_str(),120,-1200,1200,120,-1200,1200 );//20cmx20cm cell
    sc=tgcprd_shift_ac[ac]->regHist(tgcxyview[ac]) ;  
    if(sc.isFailure()) { 
      m_log << MSG::FATAL << ss.str() << "Failed to register histogram " << endreq;       
      return sc;
    }

    //histograms not for for Global Montioring
    if( m_environment != AthenaMonManager::online ) {
      // Layer1-9
      for(int i=0;i<9;i++){
        int layer=i+1;
        ss.str(""); ss<<"XY_View_" << side[ac] <<"_Layer"<<layer;
        tgcxyviewlayer[ac][i] = new TH2F(ss.str().c_str(), (ss.str() + ";X [cm]; Y [cm]").c_str(),120,-1200,1200,120,-1200,1200 );//20cmx20cm cell
        sc=tgcprd_expert_ac[ac]->regHist(tgcxyviewlayer[ac][i]) ;  
        if(sc.isFailure()) { 
          m_log << MSG::FATAL << ss.str() <<" Failed to register histogram " << endreq;       
          return sc;
        }
      }//loop over layer
    }//Global monitoring
    else
      for(int i=0;i<9;i++){
        tgcxyviewlayer[ac][i] = 0;
      }

  }//loop over ac

  return sc;
}


StatusCode
TgcRawDataValAlg::fillXYView(){
  // Fill XY View histograms
  for(int ac=0;ac<2;ac++){
    // declare R and Phi hit vectors for each chamber-layer
    vector<double> Rhos[9][6][48];//[layer][etaIndex][phiIndex]
    vector<double> Phis[9][6][48];//[layer][etaIndex][phiIndex]
    
    // sort hit information into vectors
    for(int ws=0;ws<2;ws++){
      int nHits = m_hitPosR[ac][ws].size();
      for(int iHit=0;iHit<nHits;iHit++){
        int layer      = m_hitLayer[ac][ws].at(iHit);
        int stationEta = m_hitEtaIndex[ac][ws].at(iHit);
        int stationPhi = m_hitPhiIndex[ac][ws].at(iHit);
        if(layer==1){// T1, no-strip layer
          Rhos[layer][stationEta][stationPhi].push_back(m_hitPosR[ac][ws].at(iHit));
          Phis[layer][stationEta][stationPhi].push_back(m_hitPosPhi[ac][ws].at(iHit));
        }else if((layer>=0)||(layer<9)){// T1, T2, T3, T4 with strips
          if(ws==WIRE) Rhos[layer][stationEta][stationPhi].push_back(m_hitPosR[ac][ws].at(iHit));
          else         Phis[layer][stationEta][stationPhi].push_back(m_hitPosPhi[ac][ws].at(iHit));
        }
      }//hit
    }//ws
    
    // fill Histograms from sorted vectors
    for(int l=0;l<9;l++){// layer
      for(int etaIndex=0;etaIndex<6;etaIndex++){// stationEta
        for(int phiIndex=0;phiIndex<48;phiIndex++){// stationPhi
          int nRho=Rhos[l][etaIndex][phiIndex].size();
          int nPhi=Phis[l][etaIndex][phiIndex].size();
          if((nRho==0)||(nPhi==0)) continue;
          
          for(int iRho=0;iRho<nRho;iRho++){// Rho hits in chamber
            double R=Rhos[l][etaIndex][phiIndex].at(iRho);
            R/=10;// convert to cm
            for(int iPhi=0;iPhi<nPhi;iPhi++){// Phi hits in chamber
              double Phi=Phis[l][etaIndex][phiIndex].at(iPhi);
              
              double X=R*cos(Phi);
              double Y=R*sin(Phi);
              
              if(l!=1)// exclude layer 2 (no strips) from overall histogram
                tgcxyview[ac]->Fill(X,Y);
              
              //histograms not for for Global Montioring
              if(m_environment!=AthenaMonManager::online)
                tgcxyviewlayer[ac][l]->Fill(X,Y);
            }// chamberPhiHits
          }// chamberRhoHits
        }// phiIndex
      }// etaIndex
    }// layer
    
  }//ac
  
  return StatusCode::SUCCESS;
}
