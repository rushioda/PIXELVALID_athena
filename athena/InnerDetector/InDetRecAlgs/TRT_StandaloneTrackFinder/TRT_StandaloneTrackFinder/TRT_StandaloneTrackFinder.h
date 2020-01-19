/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
   Header file for class TRT_StandaloneTrackFinder
  (c) ATLAS Detector software
  Algorithm for Trk::Track production in TRT only
  Version 1.0: 09/28/2007
  Authors    : Thomas Koffas, Markus Elsing
  email      : Thomas.Koffas@cern.ch
**********************************************************************************/

#ifndef TRT_StandaloneTrackFinder_H
#define TRT_StandaloneTrackFinder_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

///Track Collection to store the tracks
#include "TrkTrack/TrackCollection.h"

///Needed for the TRT track segments
#include "TrkSegment/SegmentCollection.h"
#include "StoreGate/DataHandle.h"
namespace InDet {

  /**
  @class TRT_StandaloneTrackFinder
  
  InDet::TRT_StandaloneTrackFinde is an algorithm which produces tracks
  from stand-alone TRT segments with no Si extension
  @author Thomas.Koffas@cern.ch     
  */

  class ITRT_SegmentToTrackTool;

  class TRT_StandaloneTrackFinder : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** Standard Algotithm methods                                   */
      ///////////////////////////////////////////////////////////////////

      TRT_StandaloneTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_StandaloneTrackFinder();
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status                    */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    private:

      ///////////////////////////////////////////////////////////////////
      /* Private data                                                */
      ///////////////////////////////////////////////////////////////////
     
      int                                m_nprint             ;
      int                                m_ntracks            ;  //!< Number of tracks found

      int                                m_minNumDriftCircles ;  //!< Minimum number of drift circles for TRT segment tracks
      double                             m_minPt              ;  //!< Minimum pt cut for TRT only (used in preselection * 0.9)
      bool                               m_resetPRD           ;  //!< Reset PRD association tool during the sub-detector pattern
      int                                m_matEffects         ;  //!< Particle hypothesis for track fitting
      bool                               m_oldLogic           ;  //!< use old transition region hit logic 

      ToolHandle< ITRT_SegmentToTrackTool > m_segToTrackTool; //!< Segment to track tool

      SG::ReadHandle< Trk::SegmentCollection >         m_Segments      ;  //!< TRT segments to use

      /**Tracks that will be passed out of AmbiProcessor. 
	 Recreated anew each time process() is called*/ 
      SG::WriteHandle<TrackCollection> m_finalTracks;

      /** Global Counters for final algorithm statistics */
      int m_nTrtSeg          ;  //!< Number of input TRT segments to be investigated per event
      int m_nUsedSeg         ;  //!< Number of TRT segments excluded at input (by BackTracking tracks)
      int m_nRejectedSeg     ;  //!< Number of segments rejected in selection at input 
      int m_nTrtSegGood      ;  //!< Number of input TRT segments after cuts per event
      int m_nSegFailed       ;  //!< Number of segments failing to translate to a track (inclusing refit)

      /** Total in counters */

      int m_nTrtSegTotal          ;  //!< Number of input TRT segments to be investigated per event
      int m_nUsedSegTotal         ;  //!< Number of TRT segments excluded at input (by BackTracking tracks)
      int m_nRejectedSegTotal     ;  //!< Number of segments reduce in selection at input 
      int m_nTrtSegGoodTotal      ;  //!< Number of input TRT segments after cuts per event
      int m_nSegFailedTotal       ;  //!< Number of segments failing to translate to a track 
      int m_nTrkScoreZeroTotal    ;  //!< Number of tracks rejected by score zero
      int m_nTrkSegUsedTotal      ;  //!< Number of excluded segments by other TRT segments
      int m_nTRTTrkTotal          ;  //!< Number of TRT-only tracks on output

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const TRT_StandaloneTrackFinder&);
  std::ostream& operator << (std::ostream&,const TRT_StandaloneTrackFinder&); 
}
#endif // TRT_StandaloneTrackFinder_H
