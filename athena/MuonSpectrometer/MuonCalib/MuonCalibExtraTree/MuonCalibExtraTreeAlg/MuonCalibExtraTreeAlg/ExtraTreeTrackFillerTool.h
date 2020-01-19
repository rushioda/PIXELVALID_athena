/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  MuonCalib_ExtraTreeTrackFillerTool_h
#define MuonCalib_ExtraTreeTrackFillerTool_h

#include "MuonCalibExtraTreeAlg/IExtraTreeFillerTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"

#include <vector>
#include <set>

class MdtIdHelper;
namespace Trk {
  class ITrackHoleSearchTool;
  class IResidualPullCalculator;
  class MeasurementBase;
  class CompetingRIOsOnTrack;
  class TrackStateOnSurface;
  class RIO_OnTrack;
}
namespace MuonGM {
  class MuonDetectorManager;
}

namespace MuonCalib {

class IIdToFixedIdTool;
class ISegmentOnTrackSelector;

class ExtraTreeTrackFillerTool: virtual public IExtraTreeFillerTool, public AthAlgTool {
 public:
  ExtraTreeTrackFillerTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode writeTracks(unsigned int &index);

 protected:
  ToolHandle<IIdToFixedIdTool> m_idToFixedIdTool;
  ToolHandle<Trk::IResidualPullCalculator   >   m_pullCalculator;    //!< 	private:
  const MdtIdHelper  *m_muonIdHelper;                                //<! 		
  std::string m_trackCollectionKey;
  std::vector<int> m_segment_authors;
  std::set<int> m_segment_authors_set;
  int m_author;
  const MuonGM::MuonDetectorManager *m_detMgr;
//retrieve all tools
  StatusCode retrieveTools();
//store one track
  void handleTrack(const Trk::Track *tracks, unsigned int &index, int author, bool storeHits);
 private:
  inline void storeHole( const Trk::TrackStateOnSurface *tsos, const unsigned int &index);
  inline void storeScatteringCenter(const Trk::TrackStateOnSurface *tsos, const unsigned int &index);
  inline void storeMeasurement(const Trk::MeasurementBase* measurement, const Trk::TrackStateOnSurface *tsos, unsigned int &index);
  inline bool storeTrack(const Trk::Track* track, int author, const std::set<int> &seg_authors);
  inline double errorCompetingRot( const Trk::CompetingRIOsOnTrack* );
  inline double errorRot( const Trk::RIO_OnTrack* );
  inline bool handleTracks(const TrackCollection *tracks, unsigned int &index, const int &author);
};

} //namespace MuonCalib
#endif
