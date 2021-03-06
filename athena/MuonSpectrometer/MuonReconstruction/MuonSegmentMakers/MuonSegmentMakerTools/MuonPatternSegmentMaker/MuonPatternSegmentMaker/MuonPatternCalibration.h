/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONPATTERNCALIBRATION_H
#define MUON_MUONPATTERNCALIBRATION_H

#include <string>
#include <map>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonSegmentMakerToolInterfaces/IMuonPatternCalibration.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

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
  class MuonIdHelperTool;
  class MuonEDMPrinterTool;
  class IMuonPatternSegmentAssociationTool;

  class MuonPatternCalibration : virtual public IMuonPatternCalibration, public AthAlgTool
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
  
    typedef IMuonPatternCalibration::ROTsPerRegion::iterator  ROTsPerRegionIt;
    typedef IMuonPatternCalibration::ROTsPerRegion::const_iterator  ROTsPerRegionCit;

    struct EtaPhiHits {
      EtaPhiHits() : neta(0), nphi(0) {}
      int neta;
      int nphi;
    };
  
  public:
    MuonPatternCalibration(const std::string&, const std::string&, const IInterface*);
    virtual ~MuonPatternCalibration();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    void calibrate( const MuonPatternCombination& pat, IMuonPatternCalibration::ROTsPerRegion& hitsPerRegion) const;
    int  getRegionId( const Identifier& id ) const ;
    void clearRotsPerRegion( IMuonPatternCalibration::ROTsPerRegion& hitsPerRegion ) const;
    bool checkForPhiMeasurements( const MuonPatternCombination& pat ) const;

  private:
    
    void createRegionMap( const MuonPatternCombination& pat, RegionMap& regionMap, bool hasPhiMeasurements ) const;
    void printRegionMap( const RegionMap& regionMap) const;

    void calibrateRegionMap( const RegionMap& regionMap, IMuonPatternCalibration::ROTsPerRegion& hitsPerRegion ) const;


    void insertCluster( const MuonCluster& mdt, RegionMap& regionMap, 
			const Amg::Vector3D& patpose, const Amg::Vector3D& patdire,
			bool hasPhiMeasurements ) const;

    void insertMdt( const MdtPrepData& clus, RegionMap& regionMap, 
		    const Amg::Vector3D& patpose, const Amg::Vector3D& patdire,
		    bool hasPhiMeasurements ) const;



    void retrieveTriggerHitContainers() const;

    ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreator; //<! pointer to mdt rio ontrack creator
    ToolHandle<IMuonClusterOnTrackCreator>    m_clusterCreator;  //<! pointer to muon cluster rio ontrack creator
    ToolHandle<MuonEDMPrinterTool>            m_printer;         //<! tool to print EDM objects
    ToolHandle<MuonIdHelperTool>              m_idHelperTool;    //<! tool to interpret and print Identifiers
    StoreGateSvc*       m_storeGate;                //!< Pointer to store gate

    std::string m_keyRpc;
    std::string m_keyTgc;
  
    const MuonGM::MuonDetectorManager*  m_detMgr;

    const RpcIdHelper*                  m_rpcIdHelper;
    const TgcIdHelper*                  m_tgcIdHelper;
    const CscIdHelper*                  m_cscIdHelper;
    const MdtIdHelper*                  m_mdtIdHelper;

    MsgStream* m_log;       //<! pointer to message stream
    bool m_debug; 
    bool m_verbose;
    
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

#endif //MUON_MUONPATTERNCALIBRATION_H
