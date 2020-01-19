/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_SGHITINPUT_H
#define FTK_SGHITINPUT_H

/* 2011-03-16 Guido Volpi  (guido.volpi@cern.ch) starting 
 * from DumpSP class in Trigger/TrigFTK/FastTrackSimWrap package.
 */
   

#include "TrigFTKSim/FTK_SGHitInputI.h"   
#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/FTKRawHit.h"
#include "TrigFTKSim/FTKPMap.h"


#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrackSummaryTool/TrackSummaryTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>

/** This class interface the ID hits with the FTK simulation
    implemented in Athena. Original code */   
class FTK_SGHitInput : virtual public FTK_SGHitInputI, public AthAlgTool,
		       public FTKDataInput {
public:
  typedef std::map<Identifier,int> HitIndexMap;

private:
  typedef enum { TAU_PARENT_BIT , B_PARENT_BIT , PION_PARENT_BIT , PION_IMMEDIATE_PARENT_BIT , NBITS } Bits;
  typedef std::bitset<NBITS> ParentBitmask;

  const ParentBitmask construct_truth_bitmap( const HepMC::GenParticle* p ) const;
  mutable MsgStream m_log;

  AtlasDetectorID* m_idHelper;
  StoreGateSvc*  m_storeGate;
  StoreGateSvc*  m_detStore;
  StoreGateSvc*  m_evtStore;

  const PixelID*   m_pixelId;
  const SCT_ID*    m_sctId;
  const TRT_ID*    m_trtId;

  const InDetDD::SiDetectorManager*     m_PIX_mgr;
  const InDetDD::SiDetectorManager*     m_SCT_mgr;    
  const InDetDD::SiDetectorManager*     m_TRT_mgr;    

  const InDet::SiClusterContainer*  m_pixelContainer;
  const InDet::SiClusterContainer*  m_sctContainer;

  const HepPDT::ParticleDataTable*  m_particleDataTable;

  // job configuration 
  ToolHandle<Trk::ITruthToTrack>            m_truthToTrack; //!< tool to create track parameters from a gen particle

  ToolHandle<Trk::IExtrapolator> m_extrapolator;
  ServiceHandle<IBeamCondSvc> m_beamSpotSvc;

  std::string  	 m_pixelClustersName;
  std::string	 m_sctClustersName;
  std::string    m_pixelSpacePointsName;
  std::string    m_sctSpacePointsName;
  std::string    m_overlapSpacePointsName;
  std::string    m_tracksTruthName;
  double         m_maxEta;
  double         m_minPt;

  std::string	 m_spacePointsName;
  //std::string    m_outFileName;
  std::string    m_outFileNameRawHits;

  //option//
  bool           m_dumpHitsOnTracks;
  bool           m_dumpSpacePoints;
  bool           m_dumpTruthIntersections;
  bool           m_useOfflineTrackSelectorTool;
  bool		 m_outputBeamSpotToWrapper;
  bool		 m_useSimpleCuts;
  bool		 m_logBeamSpotOutput;

  bool           m_readTruthTracks; // if true the truth tracks are read
  bool           m_UseNominalOrigin; // if true truth values are always with respect to (0,0,0)

  bool		 m_dooutFileRawHits; // dump raw hit into bz2 file

  // output
  // ================================================================
  mutable boost::shared_ptr<boost::iostreams::filtering_ostream> ofl;
  mutable boost::shared_ptr<boost::iostreams::filtering_ostream> oflraw;

  void read_raw_silicon( HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap ) ; // dump raw silicon data to text file and populate hitIndexMap for rec. track processing
  void read_truth_tracks() ; // dump old track-truth matching format to text file


public:
  FTK_SGHitInput(const std::string&, const std::string&, const IInterface*);
  virtual ~FTK_SGHitInput() {;}

  StatusCode initialize();
  StatusCode finalize();


  virtual int init(bool *good=0x0); // initialize for the FTKDataInput variables
  virtual int readData();
  virtual FTKDataInput* reference() { return dynamic_cast<FTKDataInput*>(this); }

  virtual const char* getCurrentFileName() { return "StoreGate"; }
};

#endif // FTK_SHHITINPUT_H
