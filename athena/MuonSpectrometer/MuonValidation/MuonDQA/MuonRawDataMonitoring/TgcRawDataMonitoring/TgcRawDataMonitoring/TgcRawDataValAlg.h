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

#ifndef TgcRawDataValAlg_H
#define TgcRawDataValAlg_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataLink.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonTrigCoinData/TgcCoinDataContainer.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TAxis.h"

#include <sstream>
#include <string.h>
#include <vector>
#include <map>

using namespace std;

class TFile;
class Identifier;

// Maximum Collection and Prd
static const int m_maxColl =  1200;
static const int m_maxPrd  = 50000;
 
template <class ConcreteAlgorithm> class AlgFactory;
/////////////////////////////////////////////////////////////////////////////

class TgcRawDataValAlg: public ManagedMonitorToolBase {
 
public:

  TgcRawDataValAlg ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~TgcRawDataValAlg();

  StatusCode initialize(); 
  //virtual StatusCode bookHistograms();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:
  // Stores
  //StoreGateSvc*   m_eventStore;
  StoreGateSvc*   m_detStore;
  ActiveStoreSvc* m_activeStore;
  
  // Log and Debug Streams
  mutable MsgStream m_log;       //!< Gaudi message stream - msgstream as private member (-> speed)
  bool              m_debuglevel;//!< private member to control debug messages  
  // Muon Detector Manager
  const MuonGM::MuonDetectorManager* m_muonMgr;
  // TGC Id Helper
  const TgcIdHelper* m_tgcIdHelper;  
  
  // event properties
  int m_event;
  int m_lumiblock;
  int m_BCID;
    
  //histograms directory names
  std::string generic_path_tgcmonitoring;
  
  // Keys and Locations for retrieving collections
  std::string m_tgcPrepDataContainerName;
  std::string m_tgcPrepDataPreviousContainerName;
  std::string m_tgcPrepDataNextContainerName;
  std::string m_outputCoinCollectionLocation;
  
  // Enum style indexes
  static const int PREV=0, CURR=1, NEXT=2, TOTA=3; //PCNT index
  static const int WIRE=0, STRP=1; //ws index
  
  StatusCode fillEventInfo();
  
  // TGC Cabling
  //const ITGCcablingSvc* m_cabling;
  
  //Function for histogram booking and parameterd for fitting
  
  ///////////////////////////////////////////////////////////////////////////
  // ReadContainer
  ///////////////////////////////////////////////////////////////////////////
  
  // Event variables
  int m_nEvent;       // event counter
  int m_nColl;        // number of collections in the event
  int m_nPrd;          // number of prd in the event
  int m_nHits[2][2];  //[ac][ws] number of hits per side, per type in event
  
  // Vectors of TgcElement ids per chamber, wire/strip, BCID, layer
  // in case of multiple hits from the same channel, only the first is added
  vector<Identifier> m_hitIdVects[3][2][2][6][48][9];//[pcn][ac][ws][eta][phi48][layer]
  
  // Vectors of variables read from the PRD Container
  vector<int>    m_hitStation[2][2];   //[ac][ws]
  vector<int>    m_hitLayer[2][2];     //[ac][ws]
  vector<int>    m_hitChannel[2][2];   //[ac][ws]
  vector<int>    m_hitSector[2][2];    //[ac][ws]
  vector<int>    m_hitSectorPhi[2][2]; //[ac][ws]
  vector<int>    m_hitEtaIndex[2][2];  //[ac][ws]
  vector<int>    m_hitPhiIndex[2][2];  //[ac][ws]
  vector<int>    m_hitPhi48[2][2];     //[ac][ws]
  vector<int>    m_hitPhi4[2][2];      //[ac][ws]
  vector<double> m_hitPosR[2][2];      //[ac][ws]
  vector<double> m_hitPosPhi[2][2];    //[ac][ws]
  
  // read Tgc PRD Container
  void readTgcPrepDataContainer(const Muon::TgcPrepDataContainer *tgc_prep_container, int pcn);
  
  
  ///////////////////////////////////////////////////////////////////////////
  // SetOffset
  ///////////////////////////////////////////////////////////////////////////
  
  // offset for 1/48 phi division so that channel numbers can be directly compared
  int m_offset[3][6];  //[TGC]1-3 , [Eta]: 0:forward 1-5:endcap
  // EIFI offset not used directly, but used for calculating nWire/StripEIFI....
  int m_offsetEIFI[4][12];  //[sect]:Sector, [sectorPhi]: 0-5:FI 6-11:EI 
  
  // number of wires/strips in each TGC station
  int m_nWireTGCSingleLayerInPhi48Division[3]; //[TGC]1-3
  int m_nStripTGCSingleLayerInPhi48Division[3];//[TGC]1-3
  int m_nWireTGCEIFISingleLayerInSect4Division[4]; //[sect]EIFI only
  int m_nStripTGCEIFISingleLayerInSect4Division[4];//[sect]EIFI only
  
  // limits on difference of channel numbers (after offset has been applied)
  // used when calculating efficiency by searching for hits in different layers at same eta
  int m_dchmin[9][9][2][2];//[+layer][-layer][ws][ac]
  int m_dchmax[9][9][2][2];//[+layer][-layer][ws][ac]
  
  // Offsets used to compare channels between layers inside stations
  int m_SLBoffset[2][2][6][9];//[ws][ac][eta][layer]
  
  // functions to clear/set the values of the above variables
  void clearVectorsArrays();
  void setOffset();
  void setChannelDifferenceRange();
  
  
  ///////////////////////////////////////////////////////////////////////////
  // NumberOfHits
  ///////////////////////////////////////////////////////////////////////////
  
  // single bin histogram for counting events
  TH1* tgceventcounter;

  // Hits per event histograms
  TH1* tgcevents;
  TH1* tgcnumberofhits[2];//[ac]
  TH1* tgcnumberofwirehits;
  TH1* tgcnumberofstriphits;

  // Hit rate across whole of midstation per lumiblock
  TH2* tgctripletdoublethitsinlbvssect[2];// profile lumiblock, FE, phi
  
  // book histograms
  StatusCode bookHistogramsNumberOfHits();
  
  // fill histograms
  StatusCode fillNumberOfHits();
  
  
  ///////////////////////////////////////////////////////////////////////////
  // Profile
  ///////////////////////////////////////////////////////////////////////////
  
  // flag for whether to monitor profile histograms
  bool m_mon_profile;
  // numbering version, 0:sector based, 1:phi based
  int m_numberingVersion;
  
  // profile histograms, filled with unique number for each element in a layer-phi48 section
  TH1* tgcwireprofilephi48[2][7][48]; //[ac][layer][phi48]
  TH1* tgcstripprofilephi48[2][7][48];//[ac][layer][phi48]
  
  // profile and occupancy maps filled in procHistograms and PostProcessor
  TH2* tgcprofilemap[2][2];  //[ac][ws]
  TH2* tgcoccupancymap[2][2];//[ac][ws]
  
  // coincidence of wires and strip chamber maps
  TH2* tgcwirestripcoin[2];//[ac]
  TH2* tgcwirestripcoinlowstat[2];//[ac]
  
  // book histograms
  StatusCode bookHistogramsProfile();
  StatusCode bookHistogramsProfile2();
  
  // fill histograms
  StatusCode fillProfile();
  
  
  ///////////////////////////////////////////////////////////////////////////
  // Efficiency (only Midstation is possible)
  ///////////////////////////////////////////////////////////////////////////
  
  // efficiency per layer-wire/strip
  TH1* tgceff[2];     //[ac]
  TH1* tgceffnum[2];  //[ac]
  TH1* tgceffdenom[2];//[ac]
  
  // efficiency per sector-layer
  TH1* tgceffsector[2][2];     //[ac][ws]
  TH1* tgceffsectornum[2][2];  //[ac][ws]
  TH1* tgceffsectordenom[2][2];//[ac][ws]
  
  // efficiency maps (current)
  TH2* tgceffmap[2][2];     //[ac][ws]
  TH2* tgceffmapnum[2][2];  //[ac][ws]
  TH2* tgceffmapdenom[2][2];//[ac][ws]
  
  // efficiency maps for BCID next/prev, (current denominator is used)
  TH2* tgceffmapbc[2][2][2];   //[ac][ws][prev/next]
  TH2* tgceffmapnumbc[2][2][2];//[ac][ws][prev/next]
  
  // calculates efficiency for given side, ws, chamber-layer 
  // is called by fillEfficiency after checks
  void calculateEfficiency(int ac, int ws, int eta, int phi48, int layer);
  
  // book histograms
  StatusCode bookHistogramsEfficiency();
  
  // fill histograms
  StatusCode fillEfficiency();
  
  
  ///////////////////////////////////////////////////////////////////////////
  // XYView
  ///////////////////////////////////////////////////////////////////////////
  
  // XY View histograms, filled with (per chamber) each wire's R and each strip's Phi
  TH2* tgcxyview[2];        //[ac]
  TH2* tgcxyviewlayer[2][9];//[ac][layer]
  
  // book histograms
  StatusCode bookHistogramsXYView();
  
  // fill histograms
  StatusCode fillXYView();
  
  
  ///////////////////////////////////////////////////////////////////////////
  // Summary
  ///////////////////////////////////////////////////////////////////////////
  
  // summary of bin values from efficiency maps
  TH1* tgcsummaryofeffpergasgap[2][2];      //[ac][ws]
  TH1* tgcsummaryofeffperchambertype[2][17];//[ws][TYPE]
  
  // summary of bin values from occupancy maps
  TH1* tgcsummaryoflog10wireoccupancypergasgap[2];       //[ac]
  TH1* tgcsummaryoflog10stripoccupancypergasgap[2];      //[ac]
  TH1* tgcsummaryoflog10wireoccupancyperchambertype[19]; //[TYPE]
  TH1* tgcsummaryoflog10stripoccupancyperchambertype[19];//[TYPE]
  
  // book histograms
  StatusCode bookHistogramsSummary();
  
  // histograms filled in PostProcessor
  
  ///////////////////////////////////////////////////////////////////////////
  // LowStat
  ///////////////////////////////////////////////////////////////////////////
  
  // book histograms
  StatusCode bookHistogramsLowStat();
  
  // histograms filled in PostProcessor
  
  ///////////////////////////////////////////////////////////////////////////
  // Functions
  ///////////////////////////////////////////////////////////////////////////
  
    //Private function to initialize the chamber ID
  void tgcchamberId();
  
  // Returns hard coded number of wires for a station index.
  // Layer variable alters the number of Wires for TGC1 slightly for layers 1 vs 2&3
  int getNumberOfWires(const int istationName, const int istationEta, const int istationPhi = 0 ) const;
  int getNumberOfWires(const int istationName, const int layer, const int istationEta, const int istationPhi ) const;
  
  // Gets an unique number for each wire in a sector-layer
  int getWireNumberPerPhi48(Identifier id)const;
  int getWireNumberPerPhi48(int tgc, int ws, int channel, int eta)const;
  
  // Get layer [0:8], sector[0:11], eta[0:5], phi [0:3]
  void getSectorBasedNumbering(int StationName, int GasGap, int StationEta, int StationPhi,//input (from idHelper)
                               int& layer, int& sector, int& eta, int& phi) const;//output
  // Get tgc [0:3], gasgap[0:2], eta[0:5], phi [0:47]
  void getEtaPhiBasedNumbering(int StationName, int GasGap, int StationEta, int StationPhi,//input (from idHelper)
                             int& tgc, int& gasgap, int& eta, int& phi) const;//output
  
  // Get sector index and phi index inside sector from phi48 value
  void phi48ToSectorPhi4(int phi48,//input [0:47]
                         int& sector, int& phi4);//output [0:11] [0:3]
  // Convert EI phi index[0:20] to EIFI overall index[0:23]
  int EIphi21toEIFIphi24(int phi21) const;
  // Convert StationName[41:48] to TGC index[0:3]
  int StationName2TgcIndex(int stationName);
  
  // Gets unique number for each each chamber in the Midstation
  int getTripletDoubletID(int stationName, int sector, int sectorPhi);
  // Gets number of wires/strips for each bin in the occupancy maps
  int nWireStripMap(int ws, int binx, int biny);
  // Gets indexes for Chamber maps, input eta and phi are the ones used to fill m_hitIdVects
  void getChamberMapIndexes(int eta, int phi48, int layer, int &binx, int &biny);
  
  // compares IDs of channels to see whether they line up in eta
  int compareID(int ch0,// innermost channel index
                int ch1,// outermost channel index
                int dmin = -1 ,// lower limit of difference
                int dmax =  0);// upper limit of difference
  
  // functions to blank out non-existent chambers
  void putBox(TH2* h2, float x1, float y1, float x2, float y2);
  void BlankPhi24(TH2 *h2, int binx);
  void BlankStationMap(TH2 *h2);
  void BlankStripStationMap(TH2 *h2);
};

#endif 
