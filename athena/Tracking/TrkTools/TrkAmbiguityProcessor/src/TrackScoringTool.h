/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRKTRACKSCORINGTOOL_H
#define TRKTRACKSCORINGTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <vector>

namespace Trk 
{

class Track;
class ITrackSummaryTool;
class TrackSummary;

/**Concrete implementation of the ITrackScoringTool pABC*/
 class TrackScoringTool : virtual public ITrackScoringTool, public AthAlgTool
 {

   public:
	TrackScoringTool(const std::string&,const std::string&,const IInterface*);
	virtual ~TrackScoringTool ();
	virtual StatusCode initialize();
	virtual StatusCode finalize  ();
	/** create a score based on how good the passed track is*/
	TrackScore score( const Track& track, const bool suppressHoleSearch ) override;

	/** create a score based on how good the passed TrackSummary is*/
	TrackScore simpleScore( const Track& track, const TrackSummary& trackSummary ) override;

   private:

	/**\todo make this const, once createSummary method is const*/
	ToolHandle<ITrackSummaryTool> m_trkSummaryTool;

	/**holds the scores assigned to each Trk::SummaryType from the track's Trk::TrackSummary*/
	std::vector<TrackScore> m_summaryTypeScore;
  }; 
}
#endif 
