/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauFinal.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
// 
// Purpose: Create TrigTau particle and perform energy calibration.
// ********************************************************************

#ifndef T2TAUFINAL_T2TAUFINAL_H 
#define T2TAUFINAL_T2TAUFINAL_H

#include <string>
#include "TrigInterfaces/FexAlgo.h"
#include "GaudiKernel/ToolHandle.h"

namespace HLT {
  class TriggerElement;
}
class IAlgToolTau;
class TrigTauTracksInfo;
class TrigInDetTrackCollection;
class TrigRoiDescriptor;
class TrigInDetTrack;



class T2TauFinal: public HLT::FexAlgo {
  /** limit on number of tools which can be used by T2TauFinal. All tools are dedfined in this package */
  enum { MaxNumberTools = 4 }; 
  
  public:
  /** constructor */
    T2TauFinal(const std::string & name, ISvcLocator* pSvcLocator);
  /** destructor */
    ~T2TauFinal();

  /** HLT method to execute */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  /** HLT method to initialize */
  HLT::ErrorCode hltInitialize();
  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();
  
  private:

    // Properties:
  /** list of IAlgToolTau tools */
  ToolHandleArray<IAlgToolTau>   m_algTools;

  const TrigInDetTrackCollection* pTrackColl;
  const DataVector<TrigInDetTrack>* myTrackColl;

  const TrigRoiDescriptor* roiDescriptor;

  /** Eta of the seed */
  double m_Eta;
  /** Phi of the seed */
  double m_Phi;
  /** EtCalib of TrigTau created (variable for histogramming) */
  // double m_EtCalib;
  /** TrigTau::Et created (variable for histogramming) */
  double m_EtCombined;
  /** TrigTau::EtFlow created (variable for histogramming) */
  double m_simpleEtFlow;
  /** nMatchedTracks of TrigTau created (variable for histogramming) */
  double m_nMatchedTracks;
  /** dPhi between  of TrigTau created (variable for histogramming) */
  double m_dPhiTrigTau_cluster;
  /** EtCalib of TrigTau created (variable for histogramming) */
  double m_dEtaTrigTau_cluster;

  /** trkAvgDist of TrigTau created (variable for histogramming) */
  double m_trkAvgDist;
  /** etOverPtLeadTrk of TrigTau created (variable for histogramming) */
  double m_etOverPtLeadTrk;


//   /** Eta of L1 ROI */
//   double m_EtaL1;
//   /** Phi of L1 ROI */
//   double m_PhiL1;

  /** option to update RoiDescriptor after execution (important for following trigger chain steps) */
  bool        m_updateRoiDescriptor;
};

// ********************************************************************
// Class name: T2TauFinalMonOfDelta
// //
// Author: Artur Kalinowski (Regina)
// //
// Class description: This is a custom monitoring class used for
// advanced monitoring of tracks collection. The class allows monitoring
// of the dEta, dPhi and dR of tracks vs. ROI position
// "opt" parameter is used for choosing the monitored quantity.
// ********************************************************************

class T2TauFinalMonOfDelta : public IMonitoredAlgo::IGetter {
public:
  T2TauFinalMonOfDelta( const DataVector<TrigInDetTrack>*& y, const TrigRoiDescriptor*& z, int opt=1)  
    : myTracks(y), roiDescriptor(z), myOpt(opt)  {}

  /** Size of the monitored collection */
  unsigned size() const { return  myTracks!=0 ? myTracks->size():0; }
  
  /** Get method used by monitorig framework */
  double get(unsigned pos) const { 
    if(myOpt==1) return getDeltaR((* myTracks)[pos]);
    if(myOpt==2) return getDeltaEta((* myTracks)[pos]);
    if(myOpt==3) return getDeltaPhi((* myTracks)[pos]);
    return 0.0;
  }

  /** Delta R in eta,phi space between track and ROI center */
  double getDeltaR(const TrigInDetTrack* aTrack)const;
  /** Delta Eta between track and ROI center */
  double getDeltaEta(const TrigInDetTrack* aTrack)const;
  /** Delta Eta between track and ROI center */
  double getDeltaPhi(const TrigInDetTrack* aTrack)const;

private:
  const DataVector<TrigInDetTrack>*& myTracks;
  const TrigRoiDescriptor*& roiDescriptor;
  int myOpt;
};

#endif
