/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackFindingEvent/MuPatTrack.h"

#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"

namespace Muon {

  // Static members
  unsigned int MuPatTrack::s_maxNumberOfInstantiations = 0;
  unsigned int MuPatTrack::s_numberOfInstantiations = 0;
  unsigned int MuPatTrack::s_numberOfCopies = 0;
  unsigned int MuPatTrack::s_processingStageStringMaxLen = 0;
  std::vector<std::string> MuPatTrack::s_processingStageStrings;

// Static functions
  void MuPatTrack::resetMaxNumberOfInstantiations() {
    s_maxNumberOfInstantiations = 0;
  }

  void MuPatTrack::resetNumberOfCopies() {
    s_numberOfCopies = 0;
  }

unsigned int MuPatTrack::processingStageStringMaxLen() {
  if ( s_processingStageStringMaxLen == 0 ) {
    initProcessingStageStrings();
    // get the maximum string length (for nicely formatted printout)
    for ( unsigned int i = 0; i < (unsigned int)NumberOfProcessingStages; ++i ) {
      unsigned int len = s_processingStageStrings[i].length();
      if ( len > s_processingStageStringMaxLen ) s_processingStageStringMaxLen = len;
    }
  }
  return s_processingStageStringMaxLen;
}


void MuPatTrack::initProcessingStageStrings() {
  if ( s_processingStageStrings.empty() ) {
    s_processingStageStrings.resize(NumberOfProcessingStages+1);
    s_processingStageStrings[Unknown] = "Unknown";
    s_processingStageStrings[InitialLoop] = "InitialLoop"; 
    s_processingStageStrings[LayerRecovery] = "LayerRecov"; 
    s_processingStageStrings[ExtendedWithSegment] = "ExtWSegment"; 
    s_processingStageStrings[SegmentRecovery] = "SegmentRecov"; 
    s_processingStageStrings[FitRemovedSegment] = "FitRmSegment"; 
    s_processingStageStrings[RefitRemovedSegment] = "RefitRmSegment"; 
    s_processingStageStrings[AmbiguityCreateCandidateFromSeeds] = "AmbiCreate"; 
    s_processingStageStrings[AmbiguitySelectCandidates] = "AmbiSelect"; 
    s_processingStageStrings[MatchFail] = "MatchFail"; 
    s_processingStageStrings[FitFail] = "FitFail"; 
    s_processingStageStrings[FitWorse] = "FitWorse"; 
    s_processingStageStrings[UnassociatedEM] = "UnassocEM"; 
    s_processingStageStrings[FitRemovedLayer] = "FitRmLayer"; 
    s_processingStageStrings[TrackSelector] = "TrackSelect"; 
    s_processingStageStrings[KeptUntilEndOfCombi] = "--KEPT--";
    s_processingStageStrings[NumberOfProcessingStages] = "OutOfBounds";

  }
}

const std::string& MuPatTrack::processingStageString( MuPatTrack::ProcessingStage stage ) {
  if ( s_processingStageStrings.empty() ) initProcessingStageStrings();
  unsigned int index = stage;
  if ( index >= s_processingStageStrings.size() ) {
    return s_processingStageStrings[NumberOfProcessingStages];
  }

  return s_processingStageStrings[index];
}


  // member functions
  MuPatTrack::MuPatTrack( const std::vector<MuPatSegment*>& segments, const Trk::Track* track , MuPatSegment* seedSeg )
   : MuPatCandidateBase()
   , created(Unknown)
   , lastSegmentChange(Unknown)
   , m_segments(segments)
   , m_track(track)
   , m_seedSeg(0)
   , m_ownTrack(true)
   , mboyInfo(0) 
  {
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " new track " << this << std::endl;
#endif
    // increase segment counters
    modifySegmentCounters(+1);
//  m_hasMomentum = hasMomentum(*track);
    m_hasMomentum = hasMomentum();
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
#endif
    // now update links between tracks and segments
    updateSegments(true);
    m_excludedSegments.push_back(0);
    m_excludedSegments.clear();
    if (seedSeg) m_seedSeg = seedSeg;
    else if (segments.size()) m_seedSeg = segments[0];
  }

  MuPatTrack::MuPatTrack( MuPatSegment* segment, const Trk::Track* track )
   : MuPatCandidateBase()
   , created(Unknown)
   , lastSegmentChange(Unknown)
   , m_track(track)
   , m_seedSeg(0)
   , m_ownTrack(true)
   , mboyInfo(0) 
  {
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " new track " << this << std::endl;
#endif
    m_segments.reserve(3);
    m_segments.push_back(segment);
    // increase segment counters
    modifySegmentCounters(+1);
//  m_hasMomentum = hasMomentum(*track);
    m_hasMomentum = hasMomentum();
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
#endif
    // now update links between tracks and segments
    updateSegments(true);
    m_excludedSegments.push_back(0);
    m_excludedSegments.clear();
  }

  MuPatTrack::MuPatTrack( MuPatSegment* segment1,  MuPatSegment* segment2, const Trk::Track* track , MuPatSegment* seedSeg )
   : MuPatCandidateBase()
   , created(Unknown)
   , lastSegmentChange(Unknown)
   , m_track(track)
   , m_seedSeg(0)
   , m_ownTrack(true)
   , mboyInfo(0) 
  {
#ifdef MCTB_OBJECT_POINTERS
   std::cout << " new track " << this << std::endl;
#endif
    m_segments.reserve(3);
    m_segments.push_back(segment1);
    m_segments.push_back(segment2);
    // increase segment counters
    modifySegmentCounters(+1);
//  m_hasMomentum = hasMomentum(*track);
    m_hasMomentum = hasMomentum();
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
#endif
    // now update links between tracks and segments
    updateSegments(true);
    m_excludedSegments.push_back(0);
    m_excludedSegments.clear();
    if (seedSeg) m_seedSeg = seedSeg;
    else m_seedSeg = segment1?segment1:segment2;
  }

  MuPatTrack::~MuPatTrack(){
    // decrease segment counters
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " delete trackcan " << this << std::endl;
#endif
    modifySegmentCounters(-1);
    if (m_ownTrack) delete m_track;
    if (mboyInfo) delete mboyInfo;
#ifdef MCTB_OBJECT_COUNTERS
    removeInstance();
#endif
    // now update links between tracks and segments
    updateSegments(false);
  }

  MuPatTrack::MuPatTrack( const MuPatTrack& can ) 
   : MuPatCandidateBase(can)
   , created(can.created)
   , lastSegmentChange(can.lastSegmentChange)
   , m_segments(can.m_segments)
   , m_excludedSegments(can.m_excludedSegments)
   , m_track(can.m_track)
   , m_seedSeg(can.m_seedSeg)
   , m_ownTrack(false) 
  {
#ifdef MCTB_OBJECT_POINTERS
    std::cout << " ctor track " << this << std::endl;
#endif
    m_hasMomentum = can.m_hasMomentum;
    // increase segment counters
    modifySegmentCounters(+1);
    if (!can.mboyInfo){
      mboyInfo=0;
    } else {
      mboyInfo = new MboyInfo();
      mboyInfo->m_MboyStatus = can.mboyInfo->m_MboyStatus ;
  
      mboyInfo->m_ISC0 = can.mboyInfo->m_ISC0 ;
  
      mboyInfo->m_DZT1 = can.mboyInfo->m_DZT1 ;
      mboyInfo->m_DS1  = can.mboyInfo->m_DS1  ;
      mboyInfo->m_DZT2 = can.mboyInfo->m_DZT2 ;
      mboyInfo->m_DS2  = can.mboyInfo->m_DS2  ;
      
      mboyInfo->m_SMU  = can.mboyInfo->m_SMU  ;
  
      mboyInfo->m_CFI  = can.mboyInfo->m_CFI  ;
      mboyInfo->m_SFI  = can.mboyInfo->m_SFI  ;
      mboyInfo->m_CFA  = can.mboyInfo->m_CFA  ;
      mboyInfo->m_SFA  = can.mboyInfo->m_SFA  ;
  
      mboyInfo->m_Z1   = can.mboyInfo->m_Z1   ;
      mboyInfo->m_T1   = can.mboyInfo->m_T1   ;
      mboyInfo->m_S1   = can.mboyInfo->m_S1   ;

      mboyInfo->m_Z2   = can.mboyInfo->m_Z2   ;
      mboyInfo->m_T2   = can.mboyInfo->m_T2   ;
      mboyInfo->m_S2   = can.mboyInfo->m_S2   ;

      mboyInfo->m_ndof = can.mboyInfo->m_ndof ;
      mboyInfo->m_chi2 = can.mboyInfo->m_chi2 ;
    }
#ifdef MCTB_OBJECT_COUNTERS
    addInstance();
    ++s_numberOfCopies;
#endif
    // now update links between tracks and segments
    updateSegments(true);
  }

  MuPatTrack& MuPatTrack::operator=(const MuPatTrack& can) { 
    if( &can != this ){
      
#ifdef MCTB_OBJECT_POINTERS
      std::cout << " operator= track " << this << std::endl;
#endif
      // now update links between tracks and segments, remove old links
      updateSegments(false);

      // decrease old segment counters
      modifySegmentCounters(-1);

      // copy members
      MuPatCandidateBase::operator=(can);
      created = can.created;
      lastSegmentChange = can.lastSegmentChange;
      m_segments = can.m_segments;
      m_excludedSegments = can.m_excludedSegments;

      if (m_ownTrack) delete m_track;
      m_track = can.m_track;
      m_chambers = can.m_chambers;
      m_stations = can.m_stations;
      m_ownTrack = false;
      m_hasMomentum = can.m_hasMomentum;
      m_seedSeg = can.m_seedSeg;

      // increase new segment counters
      modifySegmentCounters(+1);

      if (!can.mboyInfo){
        mboyInfo=0;
      } else {
        mboyInfo = new MboyInfo();
        mboyInfo->m_MboyStatus = can.mboyInfo->m_MboyStatus ;
  
        mboyInfo->m_ISC0 = can.mboyInfo->m_ISC0 ;
  
        mboyInfo->m_DZT1 = can.mboyInfo->m_DZT1 ;
        mboyInfo->m_DS1  = can.mboyInfo->m_DS1  ;
        mboyInfo->m_DZT2 = can.mboyInfo->m_DZT2 ;
        mboyInfo->m_DS2  = can.mboyInfo->m_DS2  ;
  
        mboyInfo->m_SMU  = can.mboyInfo->m_SMU  ;
  
        mboyInfo->m_CFI  = can.mboyInfo->m_CFI  ;
        mboyInfo->m_SFI  = can.mboyInfo->m_SFI  ;
        mboyInfo->m_CFA  = can.mboyInfo->m_CFA  ;
        mboyInfo->m_SFA  = can.mboyInfo->m_SFA  ;
  
        mboyInfo->m_Z1   = can.mboyInfo->m_Z1   ;
        mboyInfo->m_T1   = can.mboyInfo->m_T1   ;
        mboyInfo->m_S1   = can.mboyInfo->m_S1   ;

        mboyInfo->m_Z2   = can.mboyInfo->m_Z2   ;
        mboyInfo->m_T2   = can.mboyInfo->m_T2   ;
        mboyInfo->m_S2   = can.mboyInfo->m_S2   ;

        mboyInfo->m_ndof = can.mboyInfo->m_ndof ;
        mboyInfo->m_chi2 = can.mboyInfo->m_chi2 ;
      }

#ifdef MCTB_OBJECT_COUNTERS
      ++s_numberOfCopies;
#endif
    // now update links between tracks and segments, add new segments
    updateSegments(true);

    }
    return *this; 
  }

  bool MuPatTrack::hasMomentum() const {
    if( !m_track ) return false;
    return hasMomentum(*m_track);
  }


  bool MuPatTrack::hasMomentum( const Trk::Track& track ) const {

    // use track info if set properly  
    if( track.info().trackProperties(Trk::TrackInfo::StraightTrack) ) return false;  

    bool hasMom = false;
    const Trk::Perigee* pp = track.perigeeParameters();
    if( pp ){
      const AmgSymMatrix(5)* cov = pp->covariance();
      if( cov ){
	// sum covariance terms of momentum, use it to determine whether fit was SL fit
	double momCov = 0.;
	for( int i=0;i<4;++i ) momCov += fabs((*cov)(4,i));
	for( int i=0;i<4;++i ) momCov += fabs((*cov)(i,4));
	if( momCov > 1e-10 ) {
	  hasMom = true;
	}
      }
    }
    return hasMom;
  }

  void MuPatTrack::updateTrack( const Trk::Track* track ) {
    if (m_ownTrack && m_track != track) delete m_track;
    m_track = track;
    m_ownTrack = true;
  }

  const Trk::Track& MuPatTrack::releaseTrack() const {
    m_ownTrack = false;
    return *m_track;
  }

  void MuPatTrack::addExcludedSegment( MuPatSegment* segment ) {
    m_excludedSegments.push_back(segment);
  }
  
  void MuPatTrack::addSegment( MuPatSegment* segment, const Trk::Track* newTrack ) {
    // add segment and increase counter
    m_segments.push_back(segment);
    segment->addTrack(this);
    ++segment->usedInFit;
    std::set<MuonStationIndex::ChIndex>::const_iterator chit = segment->chambers().begin();
    std::set<MuonStationIndex::ChIndex>::const_iterator chit_end = segment->chambers().end();
    for( ;chit!=chit_end;++chit ) addChamber( *chit );

    if( newTrack ){
      // delete old track, assign new
      if ( m_ownTrack && m_track != newTrack ) delete m_track;
      m_track = newTrack;
      m_ownTrack = true;
    }
//  m_hasMomentum = hasMomentum(*newTrack);
    m_hasMomentum = hasMomentum();
  }

  void MuPatTrack::modifySegmentCounters( int change ){
    // modify usedInFit counter of segment
    std::vector<MuPatSegment*>::iterator it = m_segments.begin();
    std::vector<MuPatSegment*>::iterator it_end = m_segments.end();
    for( ;it!=it_end; ++it ) { 
      //std::cout << " modifying " << *it << std::endl;
      std::set<MuonStationIndex::ChIndex>::const_iterator chit = (*it)->chambers().begin();
      std::set<MuonStationIndex::ChIndex>::const_iterator chit_end = (*it)->chambers().end();
      for( ;chit!=chit_end;++chit ) addChamber( *chit );
      (*it)->usedInFit += change;
    }
  }

  bool MuPatTrack::resetChambersOnCandidate( const std::set<MuonStationIndex::ChIndex>& chambers ){
    // loop over input chambers, check whether segments have a chamber in the list
    // remove segment if not in the list
    setChambers(chambers); // also updates station list

    bool bRemovedSegments = false;
    std::vector<MuPatSegment*>::iterator it = m_segments.begin();
    // NOTE: can not cache m_segments.end() because it may change in the loop
    while ( it != m_segments.end() ) { 
      
      bool inChamberSet = false;
      std::set<MuonStationIndex::ChIndex>::const_iterator chit = (*it)->chambers().begin();
      std::set<MuonStationIndex::ChIndex>::const_iterator chit_end = (*it)->chambers().end();
      for( ;chit!=chit_end;++chit ) {
	if( containsChamber(*chit) ){
	  inChamberSet = true;
	  break;
	}
      }
      if( inChamberSet ){
	++it;
      }else{
	(*it)->removeTrack(this);
	bRemovedSegments = true;
        it = m_segments.erase(it); // it points to next element
      }
    }

    return bRemovedSegments;
  }



  std::vector<MuonStationIndex::StIndex> MuPatTrack::stationsInOrder() {
    std::vector<MuonStationIndex::StIndex> stations;
    stations.reserve(m_segments.size());
    std::vector<MuPatSegment*>::iterator it = m_segments.begin();
    std::vector<MuPatSegment*>::iterator it_end = m_segments.end();
    for( ;it!=it_end; ++it ) stations.push_back((*it)->stIndex);
    return stations;
  }


  std::string MuPatTrack::segmentNames() const {
    std::string names;
    std::vector<MuPatSegment*>::const_iterator sit = m_segments.begin();
    std::vector<MuPatSegment*>::const_iterator sit_end = m_segments.end();
    if ( sit != sit_end ) {
      names += (*sit)->name;
      ++sit;
    }
    // rest with spaces
    while ( sit != sit_end ) {
      names += "  ";
      names += (*sit)->name;
      ++sit;
    }
    return names;
  }

  void MuPatTrack::updateSegments( bool add ) {
    std::vector<MuPatSegment*>::iterator sit = m_segments.begin();
    std::vector<MuPatSegment*>::iterator sit_end = m_segments.end();
    for( ;sit!=sit_end;++sit ){
      if( add ) (*sit)->addTrack(this);
      else      (*sit)->removeTrack(this);
    }
  }

}

