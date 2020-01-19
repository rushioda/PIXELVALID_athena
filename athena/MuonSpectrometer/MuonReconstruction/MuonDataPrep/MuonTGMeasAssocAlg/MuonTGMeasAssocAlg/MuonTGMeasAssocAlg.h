/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGMeasAssocAlg.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef MUONTGMEASASSOCALG_MUONTGMEASASSOCALG_H
#define MUONTGMEASASSOCALG_MUONTGMEASASSOCALG_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkTrack/Track.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSegment/SegmentCollection.h"
#include <fstream>

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"
#include "MuonTGRecTools/MuonTGHits.h"
#include "MuonTGRecTools/MuonTGSegments.h"

// Amg
#include "GeoPrimitives/GeoPrimitives.h"

class StoreGateSvc;

namespace Muon{

 /** @class MuonTGMeasAssocAlg

     associates hits/segments with active layers from the (muon) tracking geometry
          
     @author Sarka.Todorova@cern.ch
     */
 
class MuonTGMeasAssocAlg : public AthAlgorithm  {

public:

  typedef Trk::Track Track;
  typedef std::pair<const Trk::Layer*,std::vector<Identifier>*> PairOfLayerID;
  typedef std::pair<const Trk::Layer*,std::vector<const Trk::PrepRawData*>*> PairOfLayerPrd;
  typedef std::pair<const Trk::Layer*,std::vector<const Muon::MdtPrepData*>*> PairOfLayerMdtPrd;
  typedef std::pair<const Trk::Layer*,std::vector<const Muon::RpcPrepData*>*> PairOfLayerRpcPrd;
  typedef std::pair<const Trk::Layer*,std::vector<const Muon::CscPrepData*>*> PairOfLayerCscPrd;
  typedef std::pair<const Trk::Layer*,std::vector<const Muon::TgcPrepData*>*> PairOfLayerTgcPrd;

  /** Constructor with parameters */
  MuonTGMeasAssocAlg(const std::string &name,ISvcLocator *pSvcLocator);

  /** Destructor */
  ~MuonTGMeasAssocAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode retrieveMeasurements();
  StatusCode storeMeasurements();
  StatusCode storeSegments();

private:
  std::vector<PairOfLayerPrd*>* createMdtHitCollectionLayers() const;
  std::vector<PairOfLayerPrd*>* createRpcHitCollectionLayers() const;
  std::vector<PairOfLayerPrd*>* createCscHitCollectionLayers() const;
  std::vector<PairOfLayerPrd*>* createTgcHitCollectionLayers() const;
  StatusCode createStationHitCollection() const;
  StatusCode createStationSegmentCollection() const;
  void createStationMap(const Trk::TrackingVolume* vol) const;
  void misAlignStations() const;
  void reAlignStations() const;
  const Trk::Layer* associatedLayer(int techn, Identifier id) const;
  const Trk::Layer* associatedLayer(const Trk::TrackingVolume* trVol, Identifier id) const;
  Identifier getStationId( Identifier id ) const;
  
  // --- job options
  bool  m_mdtIn;
  bool  m_rpcIn;
  bool  m_cscIn;
  bool  m_tgcIn;
  bool  m_segmentsIn;
  bool  m_writeTgHits;
  bool  m_writeTgSegments;
  bool  m_reAlign;
  bool  m_misAlign;
  bool  m_allowGeomAssoc;

  mutable const Trk::TrackingGeometry* m_trackingGeometry;
  std::string  m_trackingGeometryName;
  
  ToolHandle<Muon::IMuonTGMeasTool> m_muonTgTool;
  
  const MdtIdHelper* m_mdtIdHelper;
  const RpcIdHelper* m_rpcIdHelper;
  const CscIdHelper* m_cscIdHelper;
  const TgcIdHelper* m_tgcIdHelper;
  const MuonGM::MuonDetectorManager* m_muonMgr;

  std::string  m_inputSegmentCollectionMoore;  
  std::string  m_inputSegmentCollectionMoMu;  
  std::string  m_inputSegmentCollectionMBoy;  
  std::string  m_inputMdtPrdCollection;  
  std::string  m_inputRpcPrdCollection;  
  std::string  m_inputTgcPrdCollection;  
  std::string  m_inputCscPrdCollection;  
  
  // -- algorithm members
  StoreGateSvc                    *m_StoreGate;         //!< transient event store providing the tracks
  ActiveStoreSvc*                  m_activeStore;
  mutable std::vector<PairOfLayerPrd*>*       m_mdtHits;
  mutable std::vector<PairOfLayerPrd*>*       m_rpcHits;
  mutable std::vector<PairOfLayerPrd*>*       m_cscHits;
  mutable std::vector<PairOfLayerPrd*>*       m_tgcHits;
  mutable MuonTGHits                      *m_allHits;
  mutable MuonTGSegments                  *m_allSegments;
  ToolHandle<Trk::IExtrapolator>        m_extrapolator;
  mutable std::map<Identifier,std::pair<const MuonGM::MuonStation*,const Trk::DetachedTrackingVolume*> >  m_stationMap;
  mutable std::vector<std::pair<Identifier, Amg::Transform3D> > m_misAlignDiff;     // vector holding required misalignment
};

 inline StatusCode MuonTGMeasAssocAlg::retrieveMeasurements() {
   // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "MuonTGMeasAssocAlg::retrieveMeasurements()" << endmsg;
  //
  //if (m_allHits) delete m_allHits;
  if (m_mdtHits) delete m_mdtHits;
  if (m_rpcHits) delete m_rpcHits;
  if (m_cscHits) delete m_cscHits;
  if (m_tgcHits) delete m_tgcHits;

  if (m_mdtIn) m_mdtHits = createMdtHitCollectionLayers();  
  if (m_rpcIn) m_rpcHits = createRpcHitCollectionLayers();  
  if (m_cscIn) m_cscHits = createCscHitCollectionLayers();  
  if (m_tgcIn) m_tgcHits = createTgcHitCollectionLayers();  
  StatusCode sc = createStationHitCollection();

  log << MSG::DEBUG << "MuonTGMeasAssocAlg::collected hits from " << m_allHits->size() << " stations"  << endmsg;
  
  if (m_allHits->size()) return StatusCode::SUCCESS;

  return StatusCode::FAILURE;  
 }
 
} 

#endif //MUONTGMEASASSOCALG_MUONTGMEASASSOCALG_H

