/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDSelectionTool.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonRecToolInterfaces/IMuonRecoValidationTool.h"

// #include "TrkToolInterfaces/IResidualPullCalculator.h"
// #include "TrkEventPrimitives/ResidualPull.h"

namespace Muon {

 MuonPRDSelectionTool::MuonPRDSelectionTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_mdtCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
    m_clusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_recoValidationTool(""), // ("Muon::MuonRecoValidationTool/MuonRecoValidationTool"),
    // m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),    
    m_distanceToTubeCut(1000.),
    m_secondCoordinateCut(1500.)
  {
    declareInterface<IMuonPRDSelectionTool>(this);

    declareProperty("MuonIdHelperTool",m_idHelper );    
    declareProperty("MdtDriftCircleOnTrackCreator",m_mdtCreator);
    declareProperty("MuonClusterOnTrackCreator",m_clusterCreator);
    declareProperty("MuonRecoValidationTool",m_recoValidationTool);
    
  }

 MuonPRDSelectionTool::~MuonPRDSelectionTool() { }

  StatusCode MuonPRDSelectionTool::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode MuonPRDSelectionTool::initialize() {

    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_mdtCreator.retrieve());
    ATH_CHECK(m_clusterCreator.retrieve());
    if( !m_recoValidationTool.empty() ) ATH_CHECK(m_recoValidationTool.retrieve());
    // ATH_CHECK(m_pullCalculator.retrieve());

    return StatusCode::SUCCESS;
  }



  bool MuonPRDSelectionTool::calibrateAndSelect( const MuonSystemExtension::Intersection& intersection, const MuonLayerPrepRawData& layerPrepRawData, MuonLayerROTs& layerROTs ) const {

    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << " intersection: in sector " << intersection.layerSurface.sector 
                      << " region " << MuonStationIndex::regionName(intersection.layerSurface.regionIndex)
                      << " layer " << MuonStationIndex::layerName(intersection.layerSurface.layerIndex) 
                      << " r " << intersection.trackParameters->position().perp() << " z " << intersection.trackParameters->position().z()
                      << " phi " << intersection.trackParameters->position().phi() 
                      << " local (" << intersection.trackParameters->parameters()[Trk::loc1] << "," << intersection.trackParameters->parameters()[Trk::loc2] << ")";
      if( intersection.trackParameters->covariance() ) {
        msg(MSG::DEBUG) << " err (" << Amg::error(*intersection.trackParameters->covariance(),Trk::loc1) << "," << Amg::error(*intersection.trackParameters->covariance(),Trk::loc2) << ")";
      }
      
      if( !layerPrepRawData.mdts.empty() )  msg(MSG::DEBUG) << " MDTs " << layerPrepRawData.mdts.size();
      if( !layerPrepRawData.rpcs.empty() )  msg(MSG::DEBUG) << " RPCs "  << layerPrepRawData.rpcs.size();
      if( !layerPrepRawData.tgcs.empty() )  msg(MSG::DEBUG) << " TGCs "  << layerPrepRawData.tgcs.size();
      if( !layerPrepRawData.cscs.empty() )  msg(MSG::DEBUG) << " CSCs "  << layerPrepRawData.cscs.size();
      if( !layerPrepRawData.stgcs.empty() ) msg(MSG::DEBUG) << " STGCs "  << layerPrepRawData.stgcs.size();
      if( !layerPrepRawData.mms.empty() )   msg(MSG::DEBUG) << " MMs "  << layerPrepRawData.mms.size();
      msg(MSG::DEBUG) << endmsg;
    }
    
    // loop over MDT collections
    for( auto col : layerPrepRawData.mdts ){
      calibrateAndSelectMdt(intersection,*col,layerROTs);
    }

    // loop over RPC collections
    for( auto col : layerPrepRawData.rpcs ){
      calibrateAndSelectCluster(intersection,*col,layerROTs);
    }

    // loop over TGC collections
    for( auto col : layerPrepRawData.tgcs ){
      calibrateAndSelectCluster(intersection,*col,layerROTs);
    }

    return true;
  }


  const MdtDriftCircleOnTrack* MuonPRDSelectionTool::calibrateAndSelect( const MuonSystemExtension::Intersection& intersection, const MdtPrepData& mdt ) const {

    // calculate intersection with tube
    const Amg::Vector3D& direction = intersection.trackParameters->momentum();
    Amg::Vector3D intersect = intersectMDT(mdt,intersection.trackParameters->position(),direction,true);

    // get the error in the precision plane
    double err_precision = Amg::error(*intersection.trackParameters->covariance(),Trk::locX);

    // calculate local position of the intersection in tube frame
    const Identifier& id = mdt.identify();
    const MuonGM::MdtReadoutElement* detEl = mdt.detectorElement();
    const Trk::Surface& surface = mdt.detectorElement()->surface(id);
    Amg::Vector2D localPosition;
    if( !surface.globalToLocal(intersect,direction,localPosition) ){
      ATH_MSG_VERBOSE(" globalToLocal failed for " << m_idHelper->toString(id) );
      return 0;
    }
	    
    // fill validation content
    if( !m_recoValidationTool.empty() ) m_recoValidationTool->add(intersection,mdt,localPosition.x(),err_precision );
    
    // bound checks
    double tubeHalfLen = 0.5*detEl->getActiveTubeLength( m_idHelper->mdtIdHelper().tubeLayer(id),m_idHelper->mdtIdHelper().tube(id) );
    double distanceAlongTube = localPosition[Trk::locZ];

    if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " Intersected " << m_idHelper->toString(id) << " distance to wire " << localPosition[Trk::locR] 
                                                 << " error " << err_precision << " along tube (%) " << distanceAlongTube/tubeHalfLen;

    if( std::abs(distanceAlongTube) > tubeHalfLen + m_secondCoordinateCut ) {
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " outside tube second coordinate range, dropping " << endmsg;
      return 0;
    }

    if( std::abs(localPosition[Trk::locR]) > m_distanceToTubeCut ) {
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " too far from wire, dropping " << endmsg;
      return 0;
    }
    
    // shift intersection within tube 
    if( distanceAlongTube > tubeHalfLen ){
      localPosition[Trk::locZ] = distanceAlongTube < 0 ? -tubeHalfLen : tubeHalfLen;
      surface.localToGlobal(localPosition,direction,intersect);
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " outside tube, shifting";
    }
    if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << endmsg;

    // calibrate hit
    const MdtDriftCircleOnTrack* mdtROT = m_mdtCreator->createRIO_OnTrack( mdt, intersect,  &direction ); 
    if( !mdtROT ) ATH_MSG_VERBOSE(" Failed to calibrate " << m_idHelper->toString(id));
    return mdtROT;    
  }

  const MuonClusterOnTrack* MuonPRDSelectionTool::calibrateAndSelect( const Trk::TrackParameters& pars, const MuonCluster& clus ) const {
    
    // basic info
    const Identifier& id = clus.identify();
    const Trk::Surface& surf = clus.detectorElement()->surface(id);

    // calculate intersection
    const Amg::Vector3D& position  = pars.position();
    const Amg::Vector3D& direction = pars.momentum();
    Trk::Intersection slIntersection = clus.detectorElement()->surface(id).straightLineIntersection(position,direction,false,false);
    Amg::Vector3D intersect = slIntersection.position;

    
    // get local position
    Amg::Vector2D localPosition;
    if( !surf.globalToLocal(intersect,direction,localPosition) ){
      ATH_MSG_VERBOSE(" globalToLocal failed for " << m_idHelper->toString(id) );
      return 0;
    }

    if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " Intersected " << m_idHelper->toString(id) << " local position " << localPosition[Trk::loc1] << " " << localPosition[Trk::loc2];

    if( !surf.insideBounds(localPosition, m_distanceToTubeCut, m_secondCoordinateCut) ) {
      if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << " outside bounds, dropping " << endmsg;
      return 0;
    }
    if( msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) << endmsg;

    const MuonClusterOnTrack* cluster = m_clusterCreator->createRIO_OnTrack( clus, intersect ); 
    if( !cluster ){
      ATH_MSG_VERBOSE("  --- cluster creation failed ");
      return 0;
    }
    
    return cluster;    
  }

  const MuonClusterOnTrack* MuonPRDSelectionTool::calibrateAndSelect( const MuonSystemExtension::Intersection& intersection, const MuonCluster& clus ) const {
    return calibrateAndSelect(*intersection.trackParameters,clus);
  }

  Amg::Vector3D MuonPRDSelectionTool::intersectMDT( const MdtPrepData& mdt, const Amg::Vector3D& position, const Amg::Vector3D& direction, bool usePlane ) const {

    if( usePlane ){
      const Identifier& id = mdt.identify();
      const MuonGM::MdtReadoutElement* detEl = mdt.detectorElement();
      const Amg::Vector3D& tubePos = mdt.globalPosition();
      // if there is a phi measurement in the pattern use the global direction to calculate the intersect with the tube
      // use the intersect to calculate the second coordinate
      const Amg::Transform3D amdbToGlobal = detEl->AmdbLRSToGlobalTransform();
	    
      // calculate intersect pattern measurement plane
      const Amg::Vector3D& planeposition = tubePos;
	    
      // always project on plane with normal in radial direction
      Amg::Vector3D planenormal = !m_idHelper->isEndcap(id) ? amdbToGlobal.linear()*Amg::Vector3D(0.,0.,1.) : amdbToGlobal.linear()*Amg::Vector3D(0.,1.,0.);
	    
      double denom = direction.dot(planenormal);
      double u = (planenormal.dot(planeposition - position))/denom;
      Amg::Vector3D  piOnPlane = ( position + u * direction);
      return piOnPlane;
      //Amg::Vector3D lpiOnPlane = amdbToGlobal.inverse()*piOnPlane;
      //Amg::Vector3D ltubePos   = amdbToGlobal.inverse()*tubePos;
	    
      //return amdbToGlobal*lpiOnPlane;//Amg::Vector3D( lpiOnPlane.x(), ltubePos.y(), ltubePos.z() );
    }
    Trk::Intersection intersection = mdt.detectorElement()->surface(mdt.identify()).straightLineIntersection(position,direction,false,false);
    return intersection.position;
  }



}
 
