/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Authors:  A. Ishikawa(Kobe)
// Jun. 2008
// 
// DESCRIPTION
// Subject: correlation btw MDT hits vs TGC RoI -->Offline Muon Data Quality
////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// MuonDetDesc
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "Identifier/Identifier.h"

//mdt stuff
#include "MuonCalibIdentifier/MuonFixedId.h"
 
#include "MdtVsTgcRawDataMonitoring/MdtVsTgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
#include <algorithm>

using namespace std;


void
MdtVsTgcRawDataValAlg::correlation(const Muon::MdtPrepDataContainer* mdt_hit_container, 
                                   const Muon::TgcCoinDataContainer* tgccontainer){

  if (m_debuglevel) m_log<<MSG::DEBUG<<"inside correlation"<<endreq;
  //StatusCode sc=StatusCode::SUCCESS;

  //loop over TGC RoI container
  Muon::TgcCoinDataContainer::const_iterator it_end=tgccontainer->end();
  for( Muon::TgcCoinDataContainer::const_iterator it=tgccontainer->begin();
       it!=it_end;
       ++it){
  

    if (m_debuglevel) m_log<<MSG::DEBUG<< "size of tgc collection is " << (*it) -> size() << endreq;

    //loop over TGC RoI collection
    Muon::TgcCoinDataCollection::const_iterator itc_end=(*it)->end();
    for( Muon::TgcCoinDataCollection::const_iterator itc=(*it)->begin();
         itc!= itc_end;
         ++itc){

      Muon::TgcCoinData* tcd=*itc;
      Identifier tgcid=(*itc)->identify();

      if( tcd->type() != Muon::TgcCoinData::TYPE_SL )continue;

      int ac=(tcd->isAside()==false);//isNotAside a:0, c:1
      int ef=(tcd->isForward()==false);//isNotForward f:0, e:1
      int phi48=tcd->phi();//48(24)
      int roi=tcd->roi();
      int roieta;//1-53
      int roiphi;//1-192

      roi2etaphi(*tcd, roieta, roiphi);

      //int tgcMdtSector=roiphi2mdtSector(roiphi,ef);

      const MuonGM::TgcReadoutElement*  pReadoutElementTGC = m_muonMgr->getTgcReadoutElement(tgcid);
      const Amg::Vector3D pos = pReadoutElementTGC->channelPos(tgcid);

      float tgcEta = abs(pos.eta());
      float tgcPhi = pos.phi();
      if(tgcPhi<0)tgcPhi+=2*M_PI;


      if(m_debuglevel){
        m_log<<MSG::DEBUG
             <<"ac "<<ac
             <<" ef "<<ef
             <<" phi48 "<<phi48
             <<" roi "<<roi
             <<" roieta "<<roieta
             <<" roiphi "<<roiphi
             <<" tgcEta "<<tgcEta
             <<" tgcPhi "<<tgcPhi
             <<endreq;
      }


      //loop over MDT container
      Muon::MdtPrepDataContainer::const_iterator containerIt;
      Muon::MdtPrepDataContainer::const_iterator container_end=mdt_hit_container->end();
      for (containerIt = mdt_hit_container->begin() ; 
           containerIt != container_end ; 
           ++containerIt){ 
    
        Identifier mdt_id = (*containerIt)->identify();
        // Field           Range               Notes
        // ==============================================================================
        // StationName     unsigned integer    maps to "BIL", "EMS" ,etc.
        // StationEta      [-6,-1]             backward endcap (-1 at lowest R)
        //                 [-8,8]              barrel (increases with Z)
        //                 [-6,-1]+[1,6]       forward endcap (1 at lowest R)
        // StationPhi      [1,8]               increases with phi
        // Technology      [0]                 maps to "MDT"
        // Multilayer      [1,2]               barrel: increases with R
        //                                     endcap: increases with |Z|
        // TubeLayer       [1,4]               barrel: increases with R
        //                                     endcap: increases with |Z|
        // Tube            [1,n]               barrel: increases with |Z|
        //                                     endcap: increases with R
        // ==============================================================================

        int mdtStationName      =   int(m_mdtIdHelper->stationName(mdt_id)) ;

        //SN     Layer Tube Radial
        //13:EIL 2x4   x54  x4
        //49:EIS 2x4   x36  x2
        //17:EML 2x3   x64  x5
        //18:EMS 2x3   x64  x5
        //20:EOL 2x3   x48  x6
        //21:EOS 2x3   x48  x6

        //only Endcap MDT
        //if(mdtStationName!=13 && mdtStationName!=49 && mdtStationName!=17 && mdtStationName!=18 && mdtStationName!=20 && mdtStationName!=21 )continue;

        //if (m_debuglevel){
        //m_log<<MSG::DEBUG
        ///     <<"mmdtStationName "<<mdtStationName
        //    <<endreq;
        //}

        //only Endcap middle MDT
        if(mdtStationName!=17 && mdtStationName!=18 )continue;

        int mdtStationEta       =   int(m_mdtIdHelper->stationEta(mdt_id))  ;//backward:[-6,-1], forward:[1,6], (1 or -1 at lowest R)
        int mdtStationPhi       =   int(m_mdtIdHelper->stationPhi(mdt_id))  ;//[1:8]
        int mdtAC = (mdtStationEta<0);//a:0, c:1

        float mdtSector=mdtStationPhi*2.-1.;
        if(mdtStationName==18)mdtSector+=1;
        double mdtSectorPhi = (mdtSector-1.)*M_PI/8.;

        //same Side
        if(ac!=mdtAC)continue;
        if (m_debuglevel) m_log<<MSG::DEBUG<< "size of mdt collection is " << (*containerIt) -> size() << endreq;

        if (m_debuglevel){
          m_log<<MSG::DEBUG
               <<"mdtStationName "<<mdtStationName
               <<" mdtStationEta "<<mdtStationEta
               <<" mdtStationPhi "<<mdtStationPhi
               <<" mdtSectorPhi "<<mdtSectorPhi
               <<endreq;
        }
        //if( itgcstationEta!=RoIEta || itgcstationPhi!=RoIPhi48 || end_or_for!=RoIEF || a_or_c!=RoISide )continue;

        //loop over MDT PRD Collection
        Muon::MdtPrepDataCollection::const_iterator collection_it_end=(*containerIt)->end();


        int tmp[2][4][64];
        for(int i=0;i<2;i++)
          for(int j=0;j<4;j++)
            for(int k=0;k<64;k++)
              tmp[i][j][k]=0;

        for(Muon::MdtPrepDataCollection::const_iterator mdtCollection=(*containerIt)->begin();
            mdtCollection!= collection_it_end;
            ++mdtCollection){

          Identifier mdt_id2 = (*mdtCollection)->identify();

          int mdtMultiLayer       =   int(m_mdtIdHelper->multilayer(mdt_id2));
          int mdtTubeLayer        =   int(m_mdtIdHelper->tubeLayer(mdt_id2));
          int mdtTube             =   int(m_mdtIdHelper->tube(mdt_id2));
          int mdtTubeIdForEM = (abs(mdtStationEta)-1)*64 + mdtTube -1;

          if(m_debuglevel){
            m_log<<MSG::DEBUG
                 <<"mdtMultiLayer "<<mdtMultiLayer
                 <<" mdtTubeLayer "<<mdtTubeLayer
                 <<" mdtTube "<<mdtTube
                 <<" mdtTubeIdForEM "<<mdtTubeIdForEM
                 <<endreq;
          }

          if(tmp[mdtMultiLayer-1][mdtTubeLayer-1][mdtTube-1]==1)continue;
          tmp[mdtMultiLayer-1][mdtTubeLayer-1][mdtTube-1]=1;

          int adc = (*mdtCollection)->adc();
          int tdc = (*mdtCollection)->tdc();

          if(adc < m_MdtAdcCut )continue;

          const MuonGM::MdtReadoutElement*  pReadoutElementMDT = m_muonMgr->getMdtReadoutElement(mdt_id2);
          const Amg::Vector3D mdtgPos = pReadoutElementMDT->tubePos(mdt_id2); //global position of the wire
          float mdtEta = abs(mdtgPos.eta());
          float mdtPhi = mdtgPos.phi();
          float mdtr = mdtgPos.perp();
          float mdtz = mdtgPos.z();
          if(mdtPhi<0)mdtPhi+=2*M_PI;

          if (m_debuglevel){
            m_log<<MSG::DEBUG
                 <<" Name "<<mdtStationName
                 <<" Eta "<<mdtStationEta
                 <<" Phi "<<mdtStationPhi
                 <<" MultiLayer "<<mdtMultiLayer
                 <<" TubeLayer "<<mdtTubeLayer
                 <<" Tube "<<mdtTube
                 <<" TubeIdForEM "<<mdtTubeIdForEM
                 <<" Eta "<<mdtEta
                 <<" Phi "<<mdtPhi
                 <<" r "<<mdtr
                 <<" z "<<mdtz
                 <<" Sec "<<mdtSector
                 <<" SecPhi "<<mdtSectorPhi
                 <<" ADC "<<adc
                 <<" TDC "<<tdc
                 <<endreq;
          }
          
        }//MDT collection
      }//MDT container
    }//TGC collection
  }//TGC container
}
