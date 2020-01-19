/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "InDetPrepRawData/SiCluster.h"
// geometry
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetAlignGenTools/InDetAlignHitQualSelTool.h"

using namespace std ;

InDetAlignHitQualSelTool::InDetAlignHitQualSelTool( const std::string& t
                                                  , const std::string& n
                                                  , const IInterface*  p
                                                  )
  : AthAlgTool(t,n,p)
  , m_rejectOutliers( true )
  , m_maxClusterSize( 5 )
  , m_rejectEdgeChannels( true )
  , m_rejectGangedPixels( false )
  , m_maxIncidAngle( 0.8 ) //!< corresponds to 45 deg. -- recomm. by pixel ppl
  , m_acceptIBLHits( true )
  , m_acceptPixelHits( true )
  , m_acceptSCTHits( true )
{
  declareInterface<IInDetAlignHitQualSelTool>(this) ;
  declareProperty( "RejectOutliers",         m_rejectOutliers     ) ;
  declareProperty( "MaxClusterSize",         m_maxClusterSize     ) ;
  declareProperty( "RejectEdgeChannels",     m_rejectEdgeChannels ) ;
  declareProperty( "RejectGangedPixels",     m_rejectGangedPixels ) ;
  declareProperty( "MaxIncidAngle",          m_maxIncidAngle      ) ;
  declareProperty( "AcceptIBLHits",          m_acceptIBLHits    ) ;
  declareProperty( "AcceptPixelHits",        m_acceptPixelHits    ) ;
  declareProperty( "AcceptSCTHits",          m_acceptSCTHits    ) ;
}


InDetAlignHitQualSelTool::~InDetAlignHitQualSelTool() {}


StatusCode InDetAlignHitQualSelTool::initialize() {
  StatusCode sc = AlgTool::initialize() ;
  if( sc.isFailure() ) return sc ;
  // get DetectorStore service
  sc = detStore().retrieve() ;
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "DetectorStore service not found !" );
    return sc ;
  } else {
    ATH_MSG_DEBUG( "DetectorStore retrieved!" );
  }
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()){
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT ID is : "<< m_sctID <<endreq ;
  
  if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()){
    msg(MSG::FATAL) << "Could not get PIXEL ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel ID is : " << m_pixelid << endreq;
  
  // get pixel manager
  sc = detStore()->retrieve( m_PIXManager, "Pixel" ) ;
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not get PIXManager !" ) ;
    return sc;
  }
  // get SCT manager
  sc = detStore()->retrieve( m_SCTManager, "SCT" ) ;
  if( sc.isFailure() ) {
    ATH_MSG_ERROR( "Could not get SCTManager !" ) ;
    return sc;
  }
  ATH_MSG_DEBUG( "initialize() successful in " << name() ) ; 

  
  return StatusCode::SUCCESS ;
}


StatusCode InDetAlignHitQualSelTool::finalize() {
  ATH_MSG_DEBUG( "finalize() successful in " << name() ) ; 
  return AlgTool::finalize() ;
}

const Trk::RIO_OnTrack* InDetAlignHitQualSelTool::getGoodHit( const Trk::TrackStateOnSurface* tsos ) const {
  ATH_MSG_DEBUG( "** getGoodHit ** Dealing with a new tsos ** START ** " ) ;

  if( tsos == NULL ) {
    ATH_MSG_ERROR( "0 pointer passed for TSOS!" ) ;
    return 0 ;
  }
  if( !tsos->type(Trk::TrackStateOnSurface::Measurement) ) {
    ATH_MSG_DEBUG( "not a hit, cast to MeasurementBase will fail, so reject" ) ;
    return 0 ;
  }
  if( m_rejectOutliers && tsos->type(Trk::TrackStateOnSurface::Outlier) ) {
    ATH_MSG_DEBUG( "outlier, reject" ) ;
    return 0 ;
  }
  const Trk::MeasurementBase* measBase = tsos->measurementOnTrack() ;
  if( measBase == NULL) {
    ATH_MSG_DEBUG( "tsos->measurementOnTrack() returned 0 pointer" ) ;
    return 0 ;
  }
  
  const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>( measBase ) ;
  if( hit == NULL ) {
    ATH_MSG_DEBUG( "dynamic_cast <const Trk::RIO_OnTrack*>( measBase ) returned 0 pointer" ) ;
    return 0 ;
  }

  const Trk::PrepRawData* prd = hit->prepRawData() ;
  if( prd == NULL ) {
    ATH_MSG_WARNING( "hit->prepRawData() method failed" ) ;
    return 0 ;
  }
  
  
  if( m_rejectGangedPixels && isGangedPixel( prd ) ) return 0 ;
  const vector<Identifier> idVec = prd->rdoList() ;

  // cut on the cluster size
  if( m_maxClusterSize > 0 && !isGoodClusterSize( idVec ) ) return 0 ;

  // cut on edge channels
  if( m_rejectEdgeChannels && isEdgeChannel( idVec ) ) return 0 ;
  // cut on the track incidence angle alpha
  const Trk::TrackParameters* trkPar = tsos->trackParameters() ;
  if( trkPar == NULL ) {
    ATH_MSG_WARNING( "tsos->trackParameters() returned 0 pointer" ) ;
    return 0 ;
  }
  const InDetDD::SiDetectorElement *detEle
    = dynamic_cast<const InDetDD::SiDetectorElement*>( hit->detectorElement() ) ;
  if( detEle == NULL ) {
    ATH_MSG_WARNING( "hit cast to SiDetectorElement returned 0 pointer" ) ;
    return 0 ;
  }
  if( !isGoodAngle( trkPar, detEle ) ) return 0 ;  
  return hit ;
}

bool InDetAlignHitQualSelTool::isGoodSiHit( const Trk::TrackStateOnSurface* tsos ) const {
  bool isSiliconHit = false;
  bool isPixelHit = false;
  bool isIBLHit = false;

  if( tsos == NULL ) {
    ATH_MSG_ERROR( "0 pointer passed for TSOS!" ) ;
    return false ;
  }
  if( !tsos->type(Trk::TrackStateOnSurface::Measurement) ) {
    ATH_MSG_DEBUG( "not a hit, cast to MeasurementBase will fail --> keep it anyway" ) ;
    return false ;
  }

  if( m_rejectOutliers && tsos->type(Trk::TrackStateOnSurface::Outlier) ) {
    ATH_MSG_DEBUG( "outlier --> keep it" ) ;
    return false ;
  }
  const Trk::MeasurementBase* measBase = tsos->measurementOnTrack() ;
  if( measBase == NULL) {
    ATH_MSG_DEBUG( "tsos->measurementOnTrack() returned 0 pointer" ) ;
    return false ;
  }
  
  const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>( measBase ) ;
  if( hit == NULL ) {
    ATH_MSG_DEBUG( "dynamic_cast <const Trk::RIO_OnTrack*>( measBase ) returned 0 pointer" ) ;
    return false ;
  }

  const Trk::PrepRawData* prd = hit->prepRawData() ;
  if( prd == NULL ) {
    ATH_MSG_WARNING( "hit->prepRawData() method failed" ) ;
    return false ;
  }
  
  
  const Identifier & hitId = hit->identify();
  if (m_sctID->is_sct(hitId)) {
    ATH_MSG_DEBUG( " this is a SCT hit - SCT - SCT - ");
    isSiliconHit = true;
  }
  if (m_pixelid->is_pixel(hitId)) {
    ATH_MSG_DEBUG( " this is a PIX hit - PIX - PIX - ");
    isSiliconHit = true;
    isPixelHit = true; // assume that is pixel hit
    // but check if it is IBL
    if (m_pixelid->layer_disk(hitId) == 0 && m_pixelid->barrel_ec(hitId) == 0 ) {isIBLHit = true; isPixelHit = false;}
  }
  
  if (!isSiliconHit) {
    ATH_MSG_DEBUG( "This is not a silicon hit. Keep it as good" ) ;
    return true;
  }

  // accept IBL hits ?
  if (!m_acceptIBLHits && isIBLHit) {
    ATH_MSG_INFO( "this is an IBL hit --> user wants to drop it" ) ;
    return false;
  }

  // accept pixel hits ?
  if (!m_acceptPixelHits && isPixelHit) {
    ATH_MSG_INFO( "this is a pixel hit --> user wants to drop it" ) ;
    return false;
  }

  // accept SCT hits ?
  if (!m_acceptSCTHits) {
    if (isSiliconHit && m_sctID->is_sct(hitId)) {
      ATH_MSG_INFO( "this is a SCT hit --> user wants to drop it" ) ;
      return false;
    }
  }

  if( m_rejectGangedPixels && isGangedPixel( prd ) ) return false ;
  const vector<Identifier> idVec = prd->rdoList() ;

  // cut on the cluster size
  if( m_maxClusterSize > 0 && !isGoodClusterSize( idVec ) ) return false ;

  // cut on edge channels
  if( m_rejectEdgeChannels && isEdgeChannel( idVec ) ) return false ;

  // cut on the track incidence angle alpha
  const Trk::TrackParameters* trkPar = tsos->trackParameters() ;
  if( trkPar == NULL ) {
    ATH_MSG_WARNING( "tsos->trackParameters() returned 0 pointer" ) ;
    return false ;
  }
  // incidence angle
  const InDetDD::SiDetectorElement *detEle
    = dynamic_cast<const InDetDD::SiDetectorElement*>( hit->detectorElement() ) ;
  if( detEle == NULL ) {
    ATH_MSG_WARNING( "hit cast to SiDetectorElement returned 0 pointer" ) ;
    return false ;
  }
  if( !isGoodAngle( trkPar, detEle ) ) return false ;  

  return true ;
}



bool InDetAlignHitQualSelTool::getGoodHole( const Trk::TrackStateOnSurface* tsos ) const {
  if( tsos == NULL ) {
    ATH_MSG_ERROR( "0 pointer passed for TSOS!" ) ;
    return false ;
  }
  if( !tsos->type(Trk::TrackStateOnSurface::Hole) ) {
    ATH_MSG_DEBUG( "This is not a hole, reject" ) ;
    return false ;
  }
  // for holes only cut on the track incidence angle alpha
  const Trk::TrackParameters* trkPar = tsos->trackParameters() ;
  if( trkPar == NULL ) {
    ATH_MSG_WARNING( "tsos->trackParameters() returned 0 pointer" ) ;
    return false ;
  }
  const InDetDD::SiDetectorElement *detEle = dynamic_cast<const InDetDD::SiDetectorElement*>( 
    tsos->trackParameters()->associatedSurface().associatedDetectorElement() ) ;
  if( detEle == NULL ) {
    ATH_MSG_WARNING( "hole cast to SiDetectorElement returned 0 pointer" ) ;
    return false ;
  }
  if( !isGoodAngle( trkPar, detEle ) ) return false ;
  return true;
}


bool InDetAlignHitQualSelTool::isGangedPixel( const Trk::PrepRawData* prd ) const {
  const InDet::SiCluster* cluster = dynamic_cast<const InDet::SiCluster*>( prd ) ;
  if( cluster == NULL ) {
    ATH_MSG_WARNING( "dynamic_cast<const InDet::SiCluster*>( prd ) failed!" ) ;
    return false ;
  }
  if( cluster->gangedPixel() ) { //!< cut only if m_maxClusterSize set
    ATH_MSG_DEBUG( "cluster contains a ganged pixel, reject" ) ;
    return true ;
  }
  return false ;
}


bool InDetAlignHitQualSelTool::isGoodClusterSize( const std::vector<Identifier>& idVec ) const {
  int clusterSize = idVec.size() ;
  ATH_MSG_DEBUG( "clusterSize = " << clusterSize ) ;
  if( clusterSize > m_maxClusterSize ) { //!< cut only if m_maxClusterSize set
    ATH_MSG_DEBUG( "clusterSize = " << clusterSize << " > " << m_maxClusterSize << ", reject" ) ;
    return false ;
  }
  return true ;
}


bool InDetAlignHitQualSelTool::isEdgeChannel( const vector<Identifier>& idVec ) const {
  for( unsigned int i=0, i_max=idVec.size() ; i!=i_max ; ++i ) {
    if( m_SCTManager->identifierBelongs(idVec[i]) ) {
      int stripId = m_sctID->strip(idVec[i]) ;
      if( stripId == 0 || stripId == 767 ) {
        ATH_MSG_DEBUG( " SCT strip " << i << " with id " << stripId << " is an edge channel " ) ;
        return true ;
      }
      if( stripId < 0 || stripId > 767 ) {
        ATH_MSG_FATAL( " WRONG DETECTOR INFORMATION " ) ;
      }
    }
    if( m_PIXManager->identifierBelongs(idVec[i]) ) {
      int pixelIdPhi = m_pixelid->phi_index(idVec[i]) ;
      int pixelIdEta = m_pixelid->eta_index(idVec[i]) ;
      if( pixelIdPhi == 0 || pixelIdPhi == 327 || pixelIdEta == 0 || pixelIdEta == 143 ) {
        ATH_MSG_DEBUG( " pixel hit " << i << " with idPhi " << pixelIdPhi << " and idEta " << pixelIdEta << " is an edge channel " ) ;
        return true ;
      }
      if( pixelIdPhi < 0 || pixelIdPhi > 327 || pixelIdEta < 0 || pixelIdEta > 143 ) {
        ATH_MSG_FATAL( " WRONG DETECTOR INFORMATION " ) ;
      }
    }
  }
  return false ;
}

bool InDetAlignHitQualSelTool::isGoodAngle( const Trk::TrackParameters* trkPar
                                                , const InDetDD::SiDetectorElement* detEle
                                                ) const {
  const double trkIncidAngle = incidAngle( trkPar, detEle ) ;
  if( fabs(trkIncidAngle) > m_maxIncidAngle ) {
    ATH_MSG_DEBUG( "trkIncidAngle = |" << trkIncidAngle << "| > " << m_maxIncidAngle << ", reject" ) ;
    return false ;
  }
  return true;
}


double InDetAlignHitQualSelTool::incidAngle( const Trk::TrackParameters* trkPar
                                                 , const InDetDD::SiDetectorElement* detEle
                                                 ) const {
  Amg::Vector3D trkDir       = trkPar->momentum() ;
  Amg::Vector3D detElePhi    = detEle->phiAxis() ; //!< local x axis in global frame
  Amg::Vector3D detEleNormal = detEle->normal() ; //!< local z axis in global frame
  double trkDotPhi = trkDir.dot( detElePhi ) ; //!< scalar product
  double trkDotNormal = trkDir.dot( detEleNormal ) ;
  double trkIncidAngle = atan( trkDotPhi/trkDotNormal ) ;
  ATH_MSG_DEBUG( "trkIncidAngle = " << trkIncidAngle ) ;
  return trkIncidAngle ;
}
