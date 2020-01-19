/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_SEGMENTSTOTRACK_H
#define TRT_SEGMENTSTOTRACK_H

//======================================================
//
// Algorithm for creating Tracks out of TrackSegments
// by feeding them through a TrackFitter
//
// Author: Christian Schmitt <Christian.Schmitt@cern.ch>
//
//=======================================================
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class PRD_MultiTruthCollection;
class AtlasDetectorID;
class TRT_ID;

namespace Trk {
  class ITrackFitter;
  class IExtrapolator;
  class Track;
  class Segment;
}

namespace InDet{
  class TRT_DriftCircle;
}


/** @class TRT_SegmentsToTrack

    Algorithm that creates Tracks out of TrackSegments by feeding them
    to a TrackFitter

    @author Christian.Schmitt@Cern.ch
*/
namespace InDet 
{
 class TRT_SegmentsToTrack : public AthAlgorithm
  {
  public:
    TRT_SegmentsToTrack(const std::string& name, ISvcLocator* pSvcLocator);
    ~TRT_SegmentsToTrack();
    
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    
    int getNumberReal(const InDet::TRT_DriftCircle*); //!< Get the number of truth particles associated with this hit


    double getRealFraction(const Trk::Segment *segment); //!< Get the fraction of truth hits on this segment
    double getRealFractionTRT(const Trk::Track *track); //!< Get the fraction of truth TRT hits on this Track
    
    double getNoiseProbability(const Trk::Track *track); //!< Get the fraction of noise TRT hits on this Track
    
    int nTRTHits(const Trk::Track *track); //!< Count number of TRT Hits on track
    
    int nHTHits(const Trk::Track *track); //!< Count number of TRT HT Hits on track
    
    
    bool validateTrack(const Trk::Track *track); //!< Check that this track has the right properties (moves towards positive z and radially inwards)
    
    void combineSegments(void);


    std::string m_inputSegmentCollectionName; //!< Name of the TrackSegment Collection to read in
    std::string m_outputTrackCollectionName;  //!< Name of the TrackCollection to write out
    
    ToolHandle<Trk::ITrackFitter> m_trackFitter;   //!< The TrackFitter

    ToolHandle<Trk::IExtrapolator> m_extrapolator; //!< The Extrapolator    
    
    double m_noiseCut;                        //!< All tracks with a TRT Noise fraction larger than this variable will be thrown away
    int  m_minTRTHits;                        //!< All tracks with less Hits (after the track fit) will be thrown away
    
    const AtlasDetectorID*                       m_idHelper; //<! Detector ID helper
    
    const TRT_ID* m_trtid ;
    
    std::string m_multiTruthCollectionTRTName; //!< Name of the TRT MultiTruthCollection
    
    int m_nTracksReal;                        //!< Counter for real reconstructed Tracks
    int m_nTracksFake;                        //!< Counter for fake reconstructed Track
    
    double m_noiseratio;                      //!< average percentage of noise in real tracks
    
    int m_events;                             //!< Event counter
    std::map<int,int> m_MapReal;              //!< Map of hits and real tracks
    std::map<int,int> m_MapFake;              //!< Map of hits and fake tracks
    
    bool m_materialEffects;                   //!< Flag to switch on Material Effects in the Fitter
    bool m_outlierRemoval;                    //!< Flag to switch on the outlier removal in the track fitter
    

    bool m_combineSegments;                   //!< Try to combine segments from Barrel and Endcap
    std::string m_barrelSegments;             //!< Name of Barrel segment collection
    std::string m_endcapSegments;             //!< Name of Endcap segment collection
    
    std::string m_BECCollectionName;  //!< Name of the combined (TRT Barrel+EC) TrackCollection to write out

    std::string m_dummy;
    bool m_dummy_bool;

    const PRD_MultiTruthCollection *m_truthCollectionTRT; //!< Truth information for the TRT
    
    
    int m_n_combined_fit;

    

  };
}

#endif 
