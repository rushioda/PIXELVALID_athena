/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPERFMON_ZMUMUEVENT_H
#define IDPERFMON_ZMUMUEVENT_H

//==============================================================================
// Include files...
//==============================================================================
#include "CLHEP/Vector/LorentzVector.h"
#include "InDetPerformanceMonitoring/MuonSelector.h"

#include "InDetPerformanceMonitoring/EventAnalysis.h"
#include "InDetPerformanceMonitoring/PerfMonServices.h"

//==============================================================================
// Forward class declarations...
//==============================================================================
class TrackParticle;

//==============================================================================
// Class declaration...
//==============================================================================
class ZmumuEvent : public EventAnalysis
{
 public:
  ZmumuEvent();
  ~ZmumuEvent();

  enum
  {
    MUON1,
    MUON2,
    MUON3_OVR1,
    MUON4_OVR2,
    NUM_MUONS
  };

  enum
  {
    CENTRAL,
    FORWARD,
    BACKWARD,
    UNDEF
  };

  enum ZTYPE
  {
    MS,     // Just the muon system ( uncorrected )
    ME,     // The adjusted muon system properties ( corr. for cal. )
    ID,     // Using the ID system.
    CB,     // Using both the muon & ID system.
    NUM_TYPES
  };

  virtual void Init();
  virtual bool Reco();

  // Public access methods
  unsigned int  getNumberOfTaggedMuons()         {  return m_numberOfFullPassMuons; }
  const std::string   getRegion() const ;

  const xAOD::Muon*      getCombMuon(  unsigned int uPart )   { return (uPart < NUM_MUONS) ? m_pxRecMuon[uPart] : NULL;  }
  const xAOD::TrackParticle*  getMSTrack (  unsigned int uPart )   { return (uPart < NUM_MUONS) ? m_pxMSTrack[uPart] : NULL;  }
  const xAOD::TrackParticle*  getIDTrack (  unsigned int uPart )   { return (uPart < NUM_MUONS) ? m_pxIDTrack[uPart] : NULL;  }
  const xAOD::TrackParticle*  getLooseIDTk( unsigned int uPart );

  float getPtImbalance( ZTYPE eType );

  const float& getZPt(   ZTYPE eType )                  {  return m_fZPt[eType];            }
  const float& getZEta(  ZTYPE eType )                  {  return m_fZEtaDir[eType];        }
  const float& getZPhi(  ZTYPE eType )                  {  return m_fZPhiDir[eType];        }
  const float& getLeptonOpeningAngle( ZTYPE eType )     {  return m_fMuonDispersion[eType]; }
  const float& getZMass( ZTYPE eType )                  {  return m_fInvariantMass[eType];  }

  int  getZCharge( ZTYPE eType );
  bool EventPassed() {    return m_passedSelectionCuts;   }

  unsigned int getPosMuon( ZTYPE eType );
  unsigned int getNegMuon( ZTYPE eType );

  void doIsoSelection(bool doIso) {
    m_xMuonID.doIsoSelection(doIso);
  }

  void setContainer( PerfMonServices::CONTAINERS container) { m_container = container; };

 protected:
  virtual void BookHistograms();

 private:
  typedef EventAnalysis PARENT;

  // Private methods
  void  Clear();
  bool  EventSelection();
  void  ReconstructKinematics();
  void  RecordMuon( const xAOD::Muon* pxMuon );

  // Active mu-cuts for the analysis
  MuonSelector            m_xMuonID;
  PerfMonServices::CONTAINERS m_container;

  // Tag Setup variables
  unsigned int m_uMuonTags;
  unsigned int m_uTrackMatch;
  bool m_bLooseMatch;
  double m_etaCut;
  bool m_doDebug;
  // Member variables : Mostly to store relevant muon data for quick access.
  unsigned int     m_numberOfFullPassMuons;
  bool             m_passedSelectionCuts;

  const            xAOD::Muon*      m_pxRecMuon[NUM_MUONS];
  const            xAOD::TrackParticle*  m_pxMETrack[NUM_MUONS];  // Pointer to muon spectro ( corr. )
  const            xAOD::TrackParticle*  m_pxMSTrack[NUM_MUONS];      // Pointer to muon spectro
  const            xAOD::TrackParticle*  m_pxIDTrack[NUM_MUONS];       // Pointer to ID track

  // Keep kinematic information on the Z
  float m_fZPt[NUM_TYPES];
  float m_fZEtaDir[NUM_TYPES];
  float m_fZPhiDir[NUM_TYPES];
  float m_fInvariantMass[NUM_TYPES];
  float m_fMuonDispersion[NUM_TYPES];

  // Graphs
  enum HISTOS_1D
  {
    ZMASS_MUON, ZMASS_MUONADJ, ZMASS_TRACK, ZMASS_COMB,
    NUM_1HISTOS
  };
};
//==============================================================================
#endif
