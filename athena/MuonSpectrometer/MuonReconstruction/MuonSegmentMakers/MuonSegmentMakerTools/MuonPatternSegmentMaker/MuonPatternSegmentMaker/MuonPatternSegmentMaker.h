/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPATTERNSEGMENTMAKER_MUONPATTERNSEGMENTMAKER_H
#define MUON_MUONPATTERNSEGMENTMAKER_MUONPATTERNSEGMENTMAKER_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonSegmentMakerToolInterfaces/IMuonPatternSegmentMaker.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"

#include "MuonSegment/MuonSegmentCombination.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCombination.h"


#include "Identifier/Identifier.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

class StoreGate;
class MsgStream;

class MdtPrepData;

class RpcIdHelper;
class MdtIdHelper;
class CscIdHelper;
class TgcIdHelper;
class StoreGateSvc;
class Identifier;


namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class RIO_OnTrack;
  class PrepRawData;
}

namespace Muon {
  class MuonPatternCombination;
  class MdtPrepData;
  class MuonClusterOnTrack;
  class MdtDriftCircleOnTrack;
  class MuonSegment;
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class IMuonPatternSegmentAssociationTool;

  class MuonPatternSegmentMaker : virtual public IMuonPatternSegmentMaker, public AthAlgTool
  {
  public:
    typedef std::pair<Amg::Vector3D,const MuonCluster*>  ISPrd;
    typedef std::vector< ISPrd > ISPrdVec;
    typedef ISPrdVec::const_iterator ISPrdIt;

    typedef std::pair<Amg::Vector3D,const MdtPrepData*> ISPrdMdt;
    typedef std::vector< ISPrdMdt > ISPrdMdtVec;
    typedef ISPrdMdtVec::const_iterator ISPrdMdtIt;

    typedef std::map< int, ISPrdMdtVec > RegionIdMap;
    typedef RegionIdMap::const_iterator RegionIdMapIt;
  
    struct Region {
      Amg::Vector3D regionPos;
      Amg::Vector3D regionDir;
      ISPrdVec triggerPrds;
      RegionIdMap mdtPrdsPerChamber;
    };

    typedef std::map< int, Region > RegionMap;
    typedef RegionMap::iterator     RegionMapIt;
    typedef RegionMap::const_iterator RegionMapCit;

  
    typedef std::vector< const MuonClusterOnTrack* > ClusterVec;
    typedef ClusterVec::iterator ClusterIt;
    typedef ClusterVec::const_iterator ClusterCit;

    typedef std::vector< const MdtDriftCircleOnTrack* > MdtVec;
    typedef MdtVec::iterator MdtIt;
    typedef MdtVec::iterator MdtCit;

    typedef std::vector< MdtVec > MdtVecVec;  
    typedef MdtVecVec::iterator MdtVecIt;  
    typedef MdtVecVec::iterator MdtVecCit;  
  
    struct ROTRegion{
      int regionId;
      Amg::Vector3D regionPos;
      Amg::Vector3D regionDir;
      ClusterVec clusters;
      MdtVecVec     mdts;
    };

    typedef std::vector< ROTRegion > ROTsPerRegion;
    typedef ROTsPerRegion::iterator  ROTsPerRegionIt;
    typedef ROTsPerRegion::const_iterator  ROTsPerRegionCit;

    struct EtaPhiHits {
      EtaPhiHits() : neta(0), nphi(0) {}
      int neta;
      int nphi;
    };
  
  public:
    MuonPatternSegmentMaker(const std::string&, const std::string&, const IInterface*);
    virtual ~MuonPatternSegmentMaker();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    MuonSegmentCombination* find( const MuonPatternCombination& pattern ) const;

    MuonSegmentCombinationCollection* find( const MuonPatternCombinationCollection& patternCol ) const;

  private:
    void createRegionMap( const MuonPatternCombination& pat, RegionMap& regionMap, bool hasPhiMeasurements ) const;

    void insertCluster( const MuonCluster& mdt, RegionMap& regionMap, 
			const Amg::Vector3D& patpose, const Amg::Vector3D& patdire,
			bool hasPhiMeasurements ) const;

    void insertMdt( const MdtPrepData& clus, RegionMap& regionMap, 
		    const Amg::Vector3D& patpose, const Amg::Vector3D& patdire,
		    bool hasPhiMeasurements ) const;


    void        printRegionMap( const RegionMap& regionMap) const;
    bool        checkForPhiMeasurements( const MuonPatternCombination& pat ) const;

    void        calibrateRegionMap( const RegionMap& regionMap, ROTsPerRegion& hitsPerRegion ) const;
    void        clearRotsPerRegion( ROTsPerRegion& hitsPerRegion ) const;

    const MuonSegment* isSubSet( const MuonSegment* seg1,  const MuonSegment* seg2 ) const;
    int         getRegionId( const Identifier& id ) const ;

    void retrieveTriggerHitContainers() const;

    std::string m_keyRpc;
    std::string m_keyTgc;
  
    ToolHandle<IMuonSegmentMaker>       m_segmentMaker;     //<! pointer to muon rio ontrack creator

    ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreator; //<! pointer to mdt rio ontrack creator

    ToolHandle<IMuonClusterOnTrackCreator>    m_clusterCreator;  //<! pointer to muon cluster rio ontrack creator
    ToolHandle<MuonEDMPrinterTool>            m_printer;         //<! tool to print EDM objects
    ToolHandle<MuonIdHelperTool>              m_idHelper;    //<! tool to interpret and print Identifiers
    ToolHandle<IMuonPatternSegmentAssociationTool> m_assocTool;       //<! tool to associate pattern combinations with segment combinations

    bool m_doNtuple; //<! write ntuple for standalone pattern finding
    bool m_doMultiAnalysis; //<! use neighbouring chambers during segment finding
    bool m_doFullFinder; //<! 
    double m_dropDistance; //<! hits that are further away than the distance are not added to segmentmaker input
    double m_phiAngleCut;  //<! cut on the phi opening angle between chamber and pattern
    bool m_doSummary;
    bool m_recoverTriggerHits;
    bool m_removeDoubleMdtHits;

    mutable const Muon::RpcPrepDataContainer* m_rpcPrdContainer;
    mutable const Muon::TgcPrepDataContainer* m_tgcPrdContainer;

  };

}

#endif //MUONSEGMENTMAKERALGS_MUONSEGMENTMAKERTOOL_H
