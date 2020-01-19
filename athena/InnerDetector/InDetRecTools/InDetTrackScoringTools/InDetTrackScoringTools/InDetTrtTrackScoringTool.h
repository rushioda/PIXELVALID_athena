/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef INDETTRTTRACKSCORINGTOOL_H
#define INDETTRTTRACKSCORINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include <vector>
#include <string>

namespace Trk {
  class Track;
  class TrackSummary;
}

namespace MagField {
  class IMagFieldSvc;
}

class TRT_ID   ;

namespace InDet {

class ITrtDriftCircleCutTool;

/**Concrete implementation of the ITrackScoringTool pABC*/
class InDetTrtTrackScoringTool : virtual public Trk::ITrackScoringTool, public AthAlgTool
{

public:
  InDetTrtTrackScoringTool(const std::string&,const std::string&,const IInterface*);
  virtual ~InDetTrtTrackScoringTool ();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  /** create a score based on how good the passed track is*/
  Trk::TrackScore score( const Trk::Track& track, const bool suppressHoleSearch );

  /** create a score based on how good the passed TrackSummary is*/
  Trk::TrackScore simpleScore( const Trk::Track& track, const Trk::TrackSummary& trackSum );
  
  Trk::TrackScore TRT_ambigScore( const Trk::Track& track, const Trk::TrackSummary& trackSum );
	
private:

  void setupTRT_ScoreModifiers();
  
  /** Decide whether standalone TRT tracks pass the minimum hit requirement. */
  bool isGoodTRT(const Trk::Track&);
  
  /**ID TRT helper*/
  const TRT_ID* m_trtId;
  
  //these are used for ScoreModifiers 
  int m_maxSigmaChi2, m_maxTRT_Hits, m_maxLogProb, m_maxTrtRatio, m_maxTrtFittedRatio;
  
  std::vector<double> m_factorSigmaChi2, m_factorLogProb, m_factorTrtRatio, m_factorTrtFittedRatio;
  
  std::vector<double> m_boundsSigmaChi2, m_boundsLogProb, m_boundsTrtRatio, m_boundsTrtFittedRatio;

  
  /** use the scoring tuned to Ambiguity processing or not */
  bool m_useAmbigFcn;
  bool m_useLogProbBins;
  bool m_useSigmaChi2;
  
  /**holds the scores assigned to each Trk::SummaryType from the track's Trk::TrackSummary*/
  std::vector<Trk::TrackScore> m_summaryTypeScore;
  
  /**\todo make this const, once createSummary method is const*/
  ToolHandle<Trk::ITrackSummaryTool> m_trkSummaryTool;
  /** Returns minimum number of expected TRT drift circles depending on eta. */
  ToolHandle<ITrtDriftCircleCutTool> m_selectortool;
  /** to query magnetic field configuration */
  ServiceHandle<MagField::IMagFieldSvc>  m_magFieldSvc;

  
  /** cuts for selecting good tracks*/
  int    m_minTRTonTrk;      //!< minimum number of TRT hits
  double m_maxEta;           //!< maximal Eta cut
  bool   m_fieldOn;          //!< do we have field on ?
  double m_ptmin;            //!< Minimum Pt
  bool   m_parameterization; //!< use parameterization to cut instead of custom cut  
  bool   m_oldLogic;         //!< use old transition hit logic
  double m_minTRTprecision;  //!< minimum fraction of TRT precision hits

};


}
#endif 
