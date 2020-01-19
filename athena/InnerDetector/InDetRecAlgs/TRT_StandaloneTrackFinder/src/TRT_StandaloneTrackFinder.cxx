/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class InDet::TRT_StandaloneTrackFinder
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Version 1.0 09/28/2007 Thomas Koffas
// update by Markus Elsing
///////////////////////////////////////////////////////////////////

#include "TRT_StandaloneTrackFinder/TRT_StandaloneTrackFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"

///Track events
#include "TrkSegment/TrackSegment.h"

#include "InDetRecToolInterfaces/ITRT_SegmentToTrackTool.h"
#include "CxxUtils/make_unique.h"
using CLHEP::GeV;

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_StandaloneTrackFinder::TRT_StandaloneTrackFinder
(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_nprint(0),
    m_segToTrackTool("InDet::TRT_SegmentToTrackTool"),
    m_Segments("TRTSegments"),
    m_finalTracks("TRTStandaloneTracks")
{
  
  m_minNumDriftCircles = 15                                   ;       //Minimum number of drift circles for TRT segment tracks
  m_minPt              = 1.0 * GeV                            ;       //pt cut    
  m_matEffects         = 0                                    ;
  m_resetPRD           = false                                ;       //Reset PRD association tool during sub-detector pattern

  declareProperty("InputSegmentsLocation"      ,m_Segments     ); //Input track collection
  declareProperty("OutputTracksLocation"       ,m_finalTracks  ); //Output track collection
  declareProperty("MinNumDriftCircles"         ,m_minNumDriftCircles); //Minimum number of drift circles for TRT segment tracks
  declareProperty("MinPt"                      ,m_minPt             ); //Minimum Pt in preselection
  declareProperty("MaterialEffects"            ,m_matEffects        ); //Particle hypothesis during track fitting
  declareProperty("ResetPRD"                   ,m_resetPRD          ); //Reset PRD association tool during sub-detector pattern
  declareProperty("OldTransitionLogic"         ,m_oldLogic = true   ); //use old transition logic for hits 
  declareProperty("TRT_SegToTrackTool"         ,m_segToTrackTool    );
}

InDet::TRT_StandaloneTrackFinder::~TRT_StandaloneTrackFinder()
{}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_StandaloneTrackFinder::initialize()
{
  StatusCode  sc;

  msg(MSG::DEBUG) << "Initializing TRT_StandaloneTrackFinder" << endreq;

  sc = m_segToTrackTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_segToTrackTool << endreq;
    return StatusCode::FAILURE;
  }else{
    msg() << MSG::INFO << "Retrieved tool " << m_segToTrackTool << endreq;
  }


  // Get output print level
  //
  if(msgLvl(MSG::DEBUG)) {
    m_nprint=0; msg(MSG::DEBUG) << (*this) << endreq;
  }

  //Global counters. See the include file for definitions
  m_nTrtSegTotal       = 0;
  m_nUsedSegTotal      = 0;
  m_nRejectedSegTotal  = 0;
  m_nTrtSegGoodTotal   = 0;
  m_nSegFailedTotal    = 0;
  m_nTrkScoreZeroTotal = 0; 
  m_nTrkSegUsedTotal   = 0;
  m_nTRTTrkTotal       = 0;

  return sc;

}

///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
StatusCode InDet::TRT_StandaloneTrackFinder::execute()
{
  

  //Counters. See the include file for definitions
  m_nTrtSeg       = 0;
  m_nUsedSeg      = 0;
  m_nRejectedSeg  = 0;
  m_nTrtSegGood   = 0;
  m_nSegFailed    = 0;

  //Clear all caches
  m_segToTrackTool->resetAll();

  // Clear PRD association tool
  if(m_resetPRD) m_segToTrackTool->resetAssoTool();

  ///Retrieve segments from StoreGate
  //


  if(!m_Segments.isValid()){
      ATH_MSG_FATAL ("No segment with name " << m_Segments.name() << " found in StoreGate!");
      return StatusCode::FAILURE;
  }

  // statistics...
  m_nTrtSeg = int(m_Segments->size());
  ATH_MSG_DEBUG ("TRT track container size " << m_nTrtSeg);

  // loop over segments
  ATH_MSG_DEBUG ("Begin looping over all TRT segments in the event");
  Trk::SegmentCollection::const_iterator iseg    = m_Segments->begin();
  Trk::SegmentCollection::const_iterator isegEnd = m_Segments->end();
  for(; iseg != isegEnd; ++ iseg) {

    // Get the track segment
    const Trk::TrackSegment *trackTRT = dynamic_cast<const Trk::TrackSegment*>(*iseg);

    if(!trackTRT) {

      ATH_MSG_WARNING ("No pointer to segment, should not happen !");
      continue;

    } else {

      ATH_MSG_DEBUG ("--> start evaluating new segment");

      // start with pt preselection, get the segment parameters
      const Amg::VectorX& p = trackTRT->localParameters();
      if ( fabs(sin(p(3))/p(4)) < m_minPt*0.9 ) {
	// Statistics...
	m_nRejectedSeg++;
	ATH_MSG_DEBUG ("Segment pt = " << fabs(sin(p(3))/p(4)) << " , fails pre-cut, drop it !");
	continue;
      }

      // Check if segment has already been assigned to a BackTrack
      if(m_segToTrackTool->segIsUsed(*trackTRT)) {
      	// Statistics...
	m_nUsedSeg++;
	ATH_MSG_DEBUG ("Segment excluded by BackTrack, drop it !");
	continue;
      }

      // Get the number of the TRT track segment ROTs
      int  nROTs = trackTRT->numberOfMeasurementBases();
      ATH_MSG_DEBUG ("Number Of ROTs " << nROTs);

      // requited number of drift circles
      int  minDriftCircles = m_minNumDriftCircles; 

      bool is_toLower = false; // to handle special case 

      // Cases where the min number of required TRT drift circles drops to 10
      if(nROTs <= minDriftCircles && m_oldLogic) {
	ATH_MSG_DEBUG ("Few DCs, can we recover ?");

	is_toLower = m_segToTrackTool->toLower(*trackTRT);
	if (is_toLower) {
	  ATH_MSG_DEBUG ("We recovered this one, let's try...");
	}
      }

      if((nROTs < m_minNumDriftCircles) && !is_toLower) {
	// statistics...
	m_nRejectedSeg++;
	ATH_MSG_DEBUG ("Segment fails number of DC requirements, reject it");
      }
      else {
	// statistics
        m_nTrtSegGood++;
	ATH_MSG_DEBUG ("Segment considered for further processing, enter into list");

	// Transform the original TRT segment into a track
	Trk::Track* trtSeg = m_segToTrackTool->segToTrack(*trackTRT);
	if(!trtSeg){
	  // Statistics...
	  m_nSegFailed++;
	  ATH_MSG_DEBUG ("Failed to make a track out of the TRT segment!");
	  continue;
	}

	// get all TSOS
	const DataVector<const Trk::MeasurementBase>* ttsos = trtSeg->measurementsOnTrack();
	if((int)ttsos->size()<10) {
	  // Statistics...
	  m_nSegFailed++;
	  ATH_MSG_DEBUG ("Too few ROTs on track, reject !");
          delete trtSeg;
          continue;
        }
	// add the track to list
        m_segToTrackTool->addNewTrack(trtSeg);
  
      }
    }
  }

  // now resolve tracks
  ATH_MSG_DEBUG ("Creating track container ");
  StatusCode sc= m_finalTracks.record(std::unique_ptr<TrackCollection> (m_segToTrackTool->resolveTracks()));
  if(sc.isFailure() || !m_finalTracks.isValid()){
      ATH_MSG_WARNING ("Could not save the reconstructed TRT seeded Si tracks!");
      return StatusCode::FAILURE;      
  }
  // Update the total counters
  m_nTrtSegTotal       += m_nTrtSeg;
  m_nUsedSegTotal      += m_nUsedSeg;
  m_nRejectedSegTotal  += m_nRejectedSeg;
  m_nTrtSegGoodTotal   += m_nTrtSegGood;
  m_nSegFailedTotal    += m_nSegFailed;
  m_nTrkScoreZeroTotal += m_segToTrackTool->GetnTrkScoreZero();
  m_nTrkSegUsedTotal   += m_segToTrackTool->GetnTrkSegUsed();
  m_nTRTTrkTotal       += m_segToTrackTool->GetnTRTTrk();


  
  // Print common event information
  if(msgLvl(MSG::DEBUG)) {
    m_nprint=1; msg(MSG::DEBUG) << (*this) << endreq;
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_StandaloneTrackFinder::finalize()
{
  m_nprint=2; msg(MSG::INFO)<<(*this)<<endreq;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream&  InDet::TRT_StandaloneTrackFinder::dump( MsgStream& out ) const
{
  // out<<std::endl;
  if(m_nprint)  return dumpevent(out);
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_StandaloneTrackFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_Segments.name().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  n     = 65-m_finalTracks.name().size();
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");

  out<<std::endl
     <<"|----------------------------------------------------------------------"
     <<"-------------------|"<<std::endl;
  out<<"| Location of input tracks          | "<<m_Segments.name()       <<s3<<std::endl;
  out<<"| Location of output tracks         | "<<m_finalTracks.name()    <<s4<<std::endl;
  out<<"|----------------------------------------------------------------------"
     <<"-------------------|";
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::TRT_StandaloneTrackFinder::dumpevent( MsgStream& out ) const
{
  int nTrtSeg       = m_nTrtSeg;
  int nUsedSeg      = m_nUsedSeg;
  int nRejectedSeg  = m_nRejectedSeg;
  int nTrtSegGood   = m_nTrtSegGood;
  int nSegFailed    = m_nSegFailed;
  int nTrkScoreZero = m_segToTrackTool->GetnTrkScoreZero();
  int nTrkSegUsed   = m_segToTrackTool->GetnTrkSegUsed();
  int nTRTTrk       = m_segToTrackTool->GetnTRTTrk();

  if (m_nprint > 1) {
    nTrtSeg       = m_nTrtSegTotal;
    nUsedSeg      = m_nUsedSegTotal;
    nRejectedSeg  = m_nRejectedSegTotal;
    nTrtSegGood   = m_nTrtSegGoodTotal;
    nSegFailed    = m_nSegFailedTotal;
    nTrkScoreZero = m_nTrkScoreZeroTotal;
    nTrkSegUsed   = m_nTrkSegUsedTotal;
    nTRTTrk       = m_nTRTTrkTotal;
  }

  out<<std::endl 
     <<"|-------------------------------------------------------------------|" <<std::endl
     <<"|  Investigated : " <<std::endl
     <<"| "<<std::setw(7)<<nTrtSeg       <<" input TRT segments to be investigated" <<std::endl
     <<"| "<<std::setw(7)<<nUsedSeg      <<" TRT segments excluded at input (by BackTracking tracks)" <<std::endl
     <<"| "<<std::setw(7)<<nRejectedSeg  <<" segments rejected in selection at input" <<std::endl
     <<"|-------------------------------------------------------------------|" <<std::endl
     <<"| "<<std::setw(7)<<nTrtSegGood   <<" input TRT segments after cuts" <<std::endl
     <<"| "<<std::setw(7)<<nSegFailed    <<" segments failing to translate to a track (including refit)" <<std::endl
     <<"| "<<std::setw(7)<<nTrkScoreZero <<" tracks rejected by score zero" <<std::endl
     <<"| "<<std::setw(7)<<nTrkSegUsed   <<" excluded segments by other TRT segment" <<std::endl
     <<"| "<<std::setw(7)<<nTRTTrk       <<" TRT-only tracks on output" <<std::endl
     <<"|-------------------------------------------------------------------|";
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::TRT_StandaloneTrackFinder::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////

MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::TRT_StandaloneTrackFinder& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////

std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::TRT_StandaloneTrackFinder& se)
{
  return se.dump(sl); 
}   
