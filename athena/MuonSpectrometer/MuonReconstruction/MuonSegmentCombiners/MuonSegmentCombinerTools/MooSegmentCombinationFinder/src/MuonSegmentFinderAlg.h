/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOSEGMENTFINDERS_MUOSEGMENTFINDERALGS_H
#define MOOSEGMENTFINDERS_MUOSEGMENTFINDERALGS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonSegmentMakerToolInterfaces/IMuonPatternCalibration.h"
#include "MuonSegmentMakerToolInterfaces/IMuonPatternSegmentMaker.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinderTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentOverlapRemovalTool.h"

namespace Muon {
  class IMuonClusterSegmentFinder;
}

class MsgStream;
class StoreGateSvc;
class ICscSegmentFinder;

class MuonSegmentFinderAlg : public AthAlgorithm
{
 public:
  MuonSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MuonSegmentFinderAlg();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();


 private:

  ToolHandle<Muon::MuonIdHelperTool>              m_idHelperTool;   //<! Id helper tool  
  ToolHandle<Muon::MuonEDMPrinterTool>            m_printer;   //<! helper printer tool  
  ToolHandle<Muon::IMuonPatternCalibration>       m_patternCalibration;
  ToolHandle<Muon::IMuonPatternSegmentMaker>      m_patternSegmentMaker;
  ToolHandle<Muon::IMuonSegmentMaker>             m_segmentMaker;     
  ToolHandle<Muon::IMuonClusterSegmentFinder>     m_clusterSegMaker;
  ToolHandle<Muon::IMuonSegmentOverlapRemovalTool> m_segmentOverlapRemovalTool;
  ToolHandle<Muon::IMuonClusterOnTrackCreator>    m_clusterCreator;  //<! pointer to muon cluster rio ontrack creator
  ToolHandle<Muon::IMuonClusterSegmentFinderTool> m_clusterSegMakerNSW;
  ToolHandle<Muon::IMuonTruthSummaryTool>         m_truthSummaryTool;
  ToolHandle<ICscSegmentFinder>                  m_csc2dSegmentFinder;
  ToolHandle<ICscSegmentFinder>                  m_csc4dSegmentFinder;

  std::string m_segmentCollectionName;
  
  void createSegmentsWithMDTs(Muon::MuonPatternCombination* patt,std::vector<const Muon::MuonSegment*>& segs );
  void createSegmentsFromClusters(Muon::MuonPatternCombination* patt,std::vector<const Muon::MuonSegment*>& segs);

  bool m_printSummary;
  bool m_useNSWMode;
  
  /** selection flags for cluster based segment finding */
  bool                m_doTGCClust;
  bool                m_doRPCClust;
};


#endif

/*  LocalWords:  ifndef
 */
