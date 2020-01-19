/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexRecoTool.h"
#include "GaudiKernel/MsgStream.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "xAODTracking/Vertex.h"
#include "CLHEP/Random/RandFlat.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

#define MAXPLANES 100

/*
  MS vertex reconstruction routine
  See documentation at https://cds.cern.ch/record/1455664 and https://cds.cern.ch/record/1520894
  Takes Tracklets as input and creates vertices in the MS volume
*/

namespace Muon {


//** ----------------------------------------------------------------------------------------------------------------- **//


  MSVertexRecoTool::MSVertexRecoTool (const std::string& type, const std::string& name,
				      const IInterface* parent)
    : 
    AthAlgTool(type, name, parent),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_mdtIdHelper(0),
    m_rpcIdHelper(0),
    m_tgcIdHelper(0),
    m_PI(3.1415927),
    m_rndmEngine(0),
    m_rndmSvc(0)
  {
    declareInterface<IMSVertexRecoTool>(this);
    
    declareProperty("xAODVertexContainer",m_xAODContainerName="MSDisplacedVertex");//name of SG container

    // nominal phi angle for tracklets
    declareProperty("TrackPhiAngle",m_TrackPhiAngle = 0.0);
    // chi^2 probability cut
    declareProperty("VxChi2Probability",m_VxChi2ProbCUT = 0.05);
    // distance between two adjacent planes
    declareProperty("VertexPlaneDist",m_VxPlaneDist = 200.);
    // position of last radial plane
    declareProperty("VertexMaxRadialPlane",m_VertexMaxRadialPlane = 7000.);
    // position of first radial plane
    declareProperty("VertexMinRadialPlane",m_VertexMinRadialPlane = 3500.);
    // minimum occupancy to be considered "high occupancy"
    declareProperty("MinimumHighOccupancy",m_ChamberOccupancyMin = 0.25);
    // number of high occupancy chambers required to be signal like
    declareProperty("MinimumNumberOfHighOccupancy",m_minHighOccupancyChambers = 2);
    declareProperty("UseOldMSVxEndcapMethod",m_useOldMSVxEndcapMethod = false);
    declareProperty("MaxTollDist",m_MaxTollDist = 300);

    // options to calculate vertex systematic uncertainty from the tracklet reco uncertainty
    declareProperty("DoSystematicUncertainty",m_doSystematics = false);
    declareProperty("BarrelTrackletUncertainty",m_BarrelTrackletUncert = 0.1);
    declareProperty("EndcapTrackletUncertainty",m_EndcapTrackletUncert = 0.1);
    declareProperty("RndmEngine",m_rndmEngineName,"Random track killing parameter: engine used");

    // cuts to prevent excessive processing timing 
    declareProperty("MaxGlobalTracklets", m_maxGlobalTracklets = 40);
    declareProperty("MaxClusterTracklets", m_maxClusterTracklets = 50);

    //extrapolator
    declareProperty("MyExtrapolator", m_extrapolator );
  }


//** ----------------------------------------------------------------------------------------------------------------- **//

    
  StatusCode MSVertexRecoTool::initialize() {
    
    if( AthAlgTool::initialize().isFailure() ) {
      ATH_MSG_ERROR( "Failed to initialize AthAlgTool" );
      return StatusCode::FAILURE;
    }

    if(detStore()->retrieve(m_mdtIdHelper,"MDTIDHELPER").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve the mdtIdHelper" );
      return StatusCode::FAILURE;
    }

    if(detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve the rpcIdHelper" );
      return StatusCode::FAILURE;
    }

    if(detStore()->retrieve(m_tgcIdHelper,"TGCIDHELPER").isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve the tgcIdHelper" );
      return StatusCode::FAILURE;
    }

    if(m_doSystematics) {
      m_rndmEngine = m_rndmSvc->GetEngine("TrackletKiller");
      if(m_rndmEngine==0) {
	ATH_MSG_FATAL( "Could not get RndmEngine" );
	return StatusCode::FAILURE;
      }
    }

    if(m_extrapolator.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Extrapolator could not be retrieved" );
      return StatusCode::FAILURE;
    }
    
    m_PI = 3.1415927;
    
    return StatusCode::SUCCESS;
  }
  

//** ----------------------------------------------------------------------------------------------------------------- **//


  MSVertexRecoTool::~MSVertexRecoTool() {}


//** ----------------------------------------------------------------------------------------------------------------- **//
  

  StatusCode MSVertexRecoTool::findMSvertices(std::vector<Tracklet>& tracklets, std::vector<MSVertex*>& vertices) {
 
    //if there are fewer than 3 tracks, vertexing not possible
    if(tracklets.size() < 3) {

      //create xAOD::vertex - the container needs to be created for every event
      xAOD::VertexContainer* xAODVxContainer = new xAOD::VertexContainer();
      if(evtStore()->record(xAODVxContainer,m_xAODContainerName).isFailure()) {
	ATH_MSG_WARNING("Failed to record the xAOD::VertexContainer with key " << m_xAODContainerName);
	return StatusCode::FAILURE;
      }
      
      xAOD::VertexAuxContainer* aux = new xAOD::VertexAuxContainer();
      if(evtStore()->record(aux,m_xAODContainerName+"Aux.").isFailure()) {
	ATH_MSG_WARNING("Failed to record the xAOD::VertexAuxContainer with key " << m_xAODContainerName);
	return StatusCode::FAILURE;
      }
      
      xAODVxContainer->setStore( aux );
      return StatusCode::SUCCESS;
    }

    if(tracklets.size() > m_maxGlobalTracklets) {
      ATH_MSG_DEBUG( "Too many tracklets found globally. Creating dummy MS vertex and exit." );
      MSVertex* dummyVtx;
      MakeDummyVertex(dummyVtx);
      vertices.push_back(dummyVtx);
      StatusCode sc = FillOutputContainer(vertices);
      if(sc.isFailure()) return StatusCode::FAILURE;
      else               return StatusCode::SUCCESS;
    }

    //group the tracks
    std::vector<Tracklet> BarrelTracklets;
    std::vector<Tracklet> EndcapTracklets;  
    for(unsigned int i=0; i<tracklets.size(); ++i) {
      if(tracklets.at(i).mdtChamber() <= 11 || tracklets.at(i).mdtChamber() == 52) 
	BarrelTracklets.push_back(tracklets.at(i));
      else 
	EndcapTracklets.push_back(tracklets.at(i));
    }    
    
    if(BarrelTracklets.size() > m_maxClusterTracklets || EndcapTracklets.size() > m_maxClusterTracklets) {
      ATH_MSG_DEBUG( "Too many tracklets found in barrel or endcap for clustering. Creating dummy MS vertex and exit" );
      MSVertex* dummyVtx;
      MakeDummyVertex(dummyVtx);
      vertices.push_back(dummyVtx);
      StatusCode sc = FillOutputContainer(vertices);
      if(sc.isFailure()) return StatusCode::FAILURE;
      else               return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG( "Running on event with " << BarrelTracklets.size() << " barrel tracklets, " 
		   << EndcapTracklets.size() << " endcap tracklets." );

    //find any clusters of tracks & decide if tracks are from single muon
    std::vector<Muon::MSVertexRecoTool::TrkCluster> BarrelClusters = findTrackClusters(BarrelTracklets);
    std::vector<Muon::MSVertexRecoTool::TrkCluster> EndcapClusters = findTrackClusters(EndcapTracklets);

    for(unsigned int i = 0; i < BarrelClusters.size(); i++) {
      if(BarrelClusters.at(i).ntrks != (int) BarrelClusters.at(i).tracks.size()) {
        ATH_MSG_INFO( "ntrks not equal to track container size; this should never happen.  Exiting quietly." );
        StatusCode sc = FillOutputContainer(vertices);
        return sc;
      }
    }
    for(unsigned int i = 0; i < EndcapClusters.size(); i++) {
      if(EndcapClusters.at(i).ntrks != (int) EndcapClusters.at(i).tracks.size()) {
        ATH_MSG_INFO( "ntrks not equal to track container size; this should never happen.  Exiting quietly." );
        StatusCode sc = FillOutputContainer(vertices);
        return sc;
      }
    }
    
    //if doSystematics, remove tracklets according to the tracklet reco uncertainty and rerun the cluster finder
    if(m_doSystematics) {
      std::vector<Tracklet> BarrelSystTracklets,EndcapSystTracklets;
      for(unsigned int i=0; i<BarrelTracklets.size(); ++i) {
        float prob = CLHEP::RandFlat::shoot( m_rndmEngine, 0, 1 );
        if(prob > m_BarrelTrackletUncert) BarrelSystTracklets.push_back(BarrelTracklets.at(i));
      }
      if(BarrelSystTracklets.size() >= 3) {
	std::vector<Muon::MSVertexRecoTool::TrkCluster> BarrelSystClusters = findTrackClusters(BarrelSystTracklets);
        for(unsigned int i=0; i<BarrelSystClusters.size(); ++i) {
          BarrelSystClusters.at(i).isSystematic = true;
          BarrelClusters.push_back(BarrelSystClusters.at(i));
        }
      }
      for(unsigned int i=0; i<EndcapTracklets.size(); ++i) {
        float prob = CLHEP::RandFlat::shoot( m_rndmEngine, 0, 1 );
        if(prob > m_EndcapTrackletUncert) EndcapSystTracklets.push_back(EndcapTracklets.at(i));
      }
      if(EndcapSystTracklets.size() >= 3) {
	std::vector<Muon::MSVertexRecoTool::TrkCluster> EndcapSystClusters = findTrackClusters(EndcapSystTracklets);
        for(unsigned int i=0; i<EndcapSystClusters.size(); ++i) {
          EndcapSystClusters.at(i).isSystematic = true;
          EndcapClusters.push_back(EndcapSystClusters.at(i));
        }
      }
    }

    ///////////////////////////////////////////VERTEX ROUTINES/////////////////////////////////////////
    //find vertices in the barrel MS (vertices using barrel tracklets)
    for(unsigned int i=0; i<BarrelClusters.size(); ++i) {
      if(BarrelClusters[i].ntrks < 3) continue;
      ATH_MSG_DEBUG( "Attempting to build vertex from " << BarrelClusters[i].ntrks << " tracklets in the barrel" );
      MSVertex* barvertex=0;
      MSVxFinder(BarrelClusters[i].tracks, barvertex);
      if(!barvertex) continue;
      if(barvertex->getChi2Probability() > 0.05) {
        HitCounter(barvertex);
        if(barvertex->getNMDT() > 250 && (barvertex->getNRPC()+barvertex->getNTGC()) > 200) {
          ATH_MSG_DEBUG( "Vertex found in the barrel with n_trk = " << barvertex->getNTracks() 
                         << " located at (eta,phi) = (" << barvertex->getPosition().eta() 
                         << ", " << barvertex->getPosition().phi() << ")" );	  
          if(BarrelClusters[i].isSystematic) barvertex->setAuthor(3);
          vertices.push_back(barvertex);
        }//end minimum good vertex criteria
        else
          delete barvertex;
      }
      else
        delete barvertex;
    }//end loop on barrel tracklet clusters

    //find vertices in the endcap MS (vertices using endcap tracklets)
    for(unsigned int i=0; i<EndcapClusters.size(); ++i) {
      if(EndcapClusters[i].ntrks < 3) continue; 
      ATH_MSG_DEBUG( "Attempting to build vertex from " << EndcapClusters[i].ntrks << " tracklets in the endcap" );

      MSVertex* endvertex=0;
      if(m_useOldMSVxEndcapMethod) 
        MSStraightLineVx_oldMethod(EndcapClusters[i].tracks, endvertex);
      else
        MSStraightLineVx(EndcapClusters[i].tracks, endvertex);

      if(!endvertex) continue;
      if(endvertex->getPosition().perp() < 10000 && fabs(endvertex->getPosition().z()) < 14000 && 
	 fabs(endvertex->getPosition().z()) > 8000 && endvertex->getNTracks() >= 3) {
	HitCounter(endvertex);
	if(endvertex->getNMDT() > 250 && (endvertex->getNRPC()+endvertex->getNTGC()) > 200) {
	  ATH_MSG_DEBUG( "Vertex found in the endcap with n_trk = " << endvertex->getNTracks() << " located at (eta,phi) = (" 
			   << endvertex->getPosition().eta() << ", " << endvertex->getPosition().phi() << ")" );	  
	  if(EndcapClusters[i].isSystematic) 
            endvertex->setAuthor(4);
	  vertices.push_back(endvertex);
	}//end minimum good vertex criteria
        else
          delete endvertex;
      }
      else
        delete endvertex;

    }//end loop on endcap tracklet clusters

    StatusCode sc = FillOutputContainer(vertices);
    if(sc.isFailure()) return StatusCode::FAILURE;
    else               return StatusCode::SUCCESS;
  }//end find vertices


//** ----------------------------------------------------------------------------------------------------------------- **//

  
  StatusCode MSVertexRecoTool::finalize() {
    return StatusCode::SUCCESS;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  Muon::MSVertexRecoTool::TrkCluster MSVertexRecoTool::ClusterizeTracks(std::vector<Tracklet>& tracks) {

      if(tracks.size() > m_maxClusterTracklets) {
          ATH_MSG_DEBUG( "Too many tracks found, returning empty cluster" );
          TrkCluster emptycluster;
          emptycluster.ntrks=0;
          emptycluster.eta=-99999.;
          emptycluster.phi=-99999.;
          for(unsigned int i=0; i<tracks.size(); ++i) emptycluster.trks[i]=0;
          emptycluster.isSystematic = false;
          return emptycluster;
      }
      TrkCluster trkClu[100];
      TrkCluster trkClu0[100];
      trkClu[0].ntrks = 0;
      trkClu[0].eta = -10;
      trkClu[0].phi = -10;
      int ncluster = 0;
      //use each tracklet as a seed for the clusters
      for(std::vector<Tracklet>::iterator trkItr=tracks.begin(); trkItr!=tracks.end(); ++trkItr) {
          TrkCluster clu;
          clu.eta = trkItr->globalPosition().eta();
          clu.phi = trkItr->globalPosition().phi();
          clu.ntrks = 0;
          clu.isSystematic = false;
          for(unsigned int i=0; i<tracks.size(); ++i) clu.trks[i]=0;

          trkClu[ncluster] = clu;
          trkClu0[ncluster] = clu;
          ++ncluster;
          if(ncluster >= 99) {
              TrkCluster emptycluster;
              emptycluster.ntrks=0;
              emptycluster.eta=-99999.;
              emptycluster.phi=-99999.;
              for(unsigned int i=0; i<tracks.size(); ++i) emptycluster.trks[i]=0;
              emptycluster.isSystematic = false;
              return emptycluster;
          }
      }
      //loop on the clusters and let the center move to find the optimal cluster centers
      for(int icl=0; icl<ncluster; ++icl) {
          bool improvement = true;
          int nitr(0);

          int ntracks(0);
          for(int jcl=0; jcl<ncluster; ++jcl) {
              float dEta = trkClu[icl].eta - trkClu0[jcl].eta;
              float dPhi = trkClu[icl].phi - trkClu0[jcl].phi;
              while(fabs(dPhi) > m_PI) {
                  if(dPhi < 0) dPhi += 2*m_PI;
                  else dPhi -= 2*m_PI;
              }
              if(fabs(dEta) < 0.7 && fabs(dPhi) < m_PI/3.) {
                  ntracks++;
                  trkClu[icl].eta = trkClu[icl].eta - dEta/ntracks;
                  trkClu[icl].phi = trkClu[icl].phi - dPhi/ntracks;
                  while(fabs(trkClu[icl].phi) > m_PI) {
                      if(trkClu[icl].phi > 0) trkClu[icl].phi -= 2*m_PI;
                      else trkClu[icl].phi += 2*m_PI;
                  }
              }
          }//end jcl loop
          //find the number of tracks in the new cluster
          double eta_avg_best = trkClu[icl].eta;
          double phi_avg_best = trkClu[icl].phi;

          while(improvement) {
              int itracks[100];
              for(int k=0; k<ncluster; ++k) itracks[k]=0;
              int ntracks2(0);
              double eta_avg    = 0.0;
              double phi_avg    = 0.0;
              double cosPhi_avg = 0.0;
              double sinPhi_avg = 0.0;

              for(int jcl=0; jcl<ncluster; ++jcl) {

                  float dEta = fabs(trkClu[icl].eta - trkClu0[jcl].eta);
                  float dPhi = trkClu[icl].phi - trkClu0[jcl].phi;

                  while(fabs(dPhi) > m_PI) {
                      if(dPhi < 0) dPhi += 2*m_PI;
                      else dPhi -= 2*m_PI;
                  }

                  if(dEta < 0.7 && fabs(dPhi) < m_PI/3.) {
                      eta_avg += trkClu0[jcl].eta;
                      cosPhi_avg += cos(trkClu0[jcl].phi);
                      sinPhi_avg += sin(trkClu0[jcl].phi);
                      ntracks2++;
                      itracks[jcl] = 1;
                  }
              }//end jcl loop

              eta_avg = eta_avg/ntracks2;
              phi_avg = atan2(sinPhi_avg,cosPhi_avg);

              if(ntracks2 > trkClu[icl].ntrks) {
                  eta_avg_best = trkClu[icl].eta;
                  phi_avg_best = trkClu[icl].phi;
                  trkClu[icl].ntrks = ntracks2;
                  for(int k=0; k<ncluster; ++k) {
                      trkClu[icl].trks[k] = itracks[k];    
                  }
                  if(nitr < 6 ){
                      trkClu[icl].eta = eta_avg;
                      trkClu[icl].phi = phi_avg;
                  } else break;

              }
              else {
                  trkClu[icl].eta = eta_avg_best;
                  trkClu[icl].phi = phi_avg_best;                   
                  improvement = false;
              }
              nitr++;
          }//end while
      }//end icl loop

      //find the best cluster
      TrkCluster BestCluster = trkClu[0];
      for(int icl=1; icl<ncluster; ++icl) {
          if(trkClu[icl].ntrks > BestCluster.ntrks) BestCluster = trkClu[icl];
      }
      //store the tracks inside the cluster
      std::vector<Tracklet> unusedTracks;
      for(std::vector<Tracklet>::iterator trkItr=tracks.begin(); trkItr!=tracks.end(); ++trkItr) {
          float dEta = fabs(BestCluster.eta - trkItr->globalPosition().eta());
          float dPhi = BestCluster.phi - trkItr->globalPosition().phi();
          while(fabs(dPhi) > m_PI) {
              if(dPhi < 0) dPhi += 2*m_PI;
              else dPhi -= 2*m_PI;
          }
          if(dEta < 0.7 && fabs(dPhi) < m_PI/3.) BestCluster.tracks.push_back( (*trkItr) );
          else unusedTracks.push_back( (*trkItr) );
      }
      //return the best cluster and the unused tracklets
      tracks = unusedTracks;
      return BestCluster;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  std::vector<Muon::MSVertexRecoTool::TrkCluster> MSVertexRecoTool::findTrackClusters(std::vector<Tracklet>& tracks) {
    std::vector<Tracklet> trks = tracks;
    std::vector<TrkCluster> clusters;
    //keep making clusters until there are no more possible
    while(true) {
      if(trks.size() < 3) break;
      TrkCluster clust = ClusterizeTracks(trks);
      if(clust.ntrks >= 3) clusters.push_back(clust);
      else break;
      if(trks.size() < 3) break;
    }
  
    if(clusters.size() == 0) {
      TrkCluster clust;
      clusters.push_back(clust);
    }
  
    return clusters;
  }  


//** ----------------------------------------------------------------------------------------------------------------- **//


  void MSVertexRecoTool::MSVxFinder(std::vector<Tracklet>& tracklets, MSVertex*& vtx) {

    int nTrkToVertex(0);
    float NominalAngle(m_TrackPhiAngle),MaxOpenAngle(0.20+m_TrackPhiAngle);
    float aveX(0),aveY(0),aveR(0),aveZ(0);  
    float maxError(200);//maximum allowed uncertainty for tracklets [mm] (above this cut tracklets are discarded)

    //find the average position of the tracklets
    for(std::vector<Tracklet>::iterator trkItr=tracklets.begin(); trkItr!=tracklets.end(); ++trkItr) {    
      aveR += trkItr->globalPosition().perp();
      aveX += trkItr->globalPosition().x();
      aveY += trkItr->globalPosition().y();
      aveZ += trkItr->globalPosition().z();
    }

    aveX = aveX/(float)tracklets.size();
    aveY = aveY/(float)tracklets.size();
    aveZ = aveZ/(float)tracklets.size();
    aveR = aveR/(float)tracklets.size();

    float avePhi = atan2(aveY,aveX);
    while(fabs(avePhi) > m_PI) {
      if(avePhi < 0) avePhi += 2*m_PI;
      else avePhi -= 2*m_PI;
    }

    //calculate the two angles (theta & phi)
    float LoF = atan2(aveR,aveZ);//Line of Flight (theta)
    avePhi = vxPhiFinder(fabs(LoF),avePhi);

    //find the positions of the radial planes
    float Rpos[MAXPLANES];
    float RadialDist = m_VertexMaxRadialPlane - m_VertexMinRadialPlane;
    float LoFdist = fabs(RadialDist/sin(LoF));
    int nplanes = LoFdist/m_VxPlaneDist + 1;
    float PlaneSpacing = fabs(200./cos(LoF));
    for(int k=0; k<nplanes; ++k) Rpos[k] = m_VertexMinRadialPlane + PlaneSpacing*k;

    //loop on tracklets and create two types of track parameters -- nominal and phi shifted tracklets
    std::vector<const Trk::TrackParameters*> TracksForVertexing[MAXPLANES];//vector of tracklets to be used at each vertex plane
    std::vector<const Trk::TrackParameters*> TracksForErrors[MAXPLANES];//vector of tracklets to be used for uncertainty at each vertex plane
    std::vector<bool> isNeutralTrack[MAXPLANES];

    for(unsigned int i=0; i<tracklets.size(); ++i) {

      //only barrel tracklets
      if(tracklets.at(i).mdtChamber() <= 11 || tracklets.at(i).mdtChamber() == 52) {

        nTrkToVertex++;
        //coordinate transform variables
        Amg::Vector3D trkgpos(tracklets.at(i).globalPosition().perp()*cos(avePhi),
			      tracklets.at(i).globalPosition().perp()*sin(avePhi),
			      tracklets.at(i).globalPosition().z());
        float x0 = trkgpos.x();
        float y0 = trkgpos.y();
        float r0 = trkgpos.perp();

        ////////Tracks for computing the error//////
        //////coordinate transform mess
	//decide which way the tracklet gets rotated -- positive or negative phi
        float anglesign = 1.0;
        if((tracklets.at(i).globalPosition().phi() - avePhi) < 0) anglesign = -1.0;
        float NominalTrkAng = anglesign*NominalAngle;//in case there is a nominal tracklet angle
        float MaxTrkAng = anglesign*MaxOpenAngle;//the rotated tracklet phi position

	//loop over the planes
        for(int k=0; k<nplanes; ++k) {

	  //only use tracklets that start AFTER the vertex plane
	  if(Rpos[k] > tracklets.at(i).globalPosition().perp()) break;

	  //nominal tracks for vertexing
	  float Xp = Rpos[k]*cos(avePhi);
	  float Yp = Rpos[k]*sin(avePhi);       
	  //in case there is a nominal opening angle, calculate tracklet direction
	  //the tracklet must cross the candidate vertex plane at the correct phi
	  float DelR = sqrt(sq(x0-Xp)+sq(y0-Yp))/cos(NominalAngle);
	  float X1 = DelR*cos(NominalTrkAng+avePhi) + Xp;
	  float Y1 = DelR*sin(NominalTrkAng+avePhi) + Yp;
	  float R1 = sqrt(sq(X1)+sq(Y1));
	  float Norm = r0/R1;
	  X1 = X1*Norm;
	  Y1 = Y1*Norm;
	  float Dirmag = sqrt(sq(X1-Xp)+sq(Y1-Yp));
	  float Xdir = (X1-Xp)/Dirmag;
	  float Ydir = (Y1-Yp)/Dirmag;
	  float trkpx = Xdir*tracklets.at(i).momentum().perp();
	  float trkpy = Ydir*tracklets.at(i).momentum().perp();
	  float trkpz = tracklets.at(i).momentum().z();
	  //check if the tracklet has a charge & momentum measurement -- if not, set charge=1 so extrapolator will work
	  float charge = tracklets.at(i).charge(); 
	  if(fabs(charge) < 0.1) {
	    charge = 1; //for "straight" tracks, set charge = 1	   	
	    isNeutralTrack[k].push_back(true);
	  }
	  else isNeutralTrack[k].push_back(false);

	  //store the tracklet as a Trk::Perigee
	  Amg::Vector3D trkmomentum(trkpx,trkpy,trkpz);
	  Amg::Vector3D trkgpos(X1,Y1,tracklets.at(i).globalPosition().z());
	  AmgSymMatrix(5) * covariance = new AmgSymMatrix(5)(tracklets.at(i).errorMatrix());
	  Trk::Perigee * myPerigee = new Trk::Perigee(0.,0.,trkmomentum.phi(),trkmomentum.theta(),charge/trkmomentum.mag(),Trk::PerigeeSurface(trkgpos),covariance);
	  TracksForVertexing[k].push_back(myPerigee);

	  //tracks for errors -- rotate the plane & recalculate the tracklet parameters
	  float xp = Rpos[k]*cos(avePhi);
	  float yp = Rpos[k]*sin(avePhi);       
	  float delR = sqrt(sq(x0-xp)+sq(y0-yp))/cos(MaxOpenAngle);
	  float x1 = delR*cos(MaxTrkAng+avePhi) + xp;
	  float y1 = delR*sin(MaxTrkAng+avePhi) + yp;
	  float r1 = sqrt(sq(x1)+sq(y1));
	  float norm = r0/r1;
	  x1 = x1*norm;
	  y1 = y1*norm;
	  float dirmag = sqrt(sq(x1-xp)+sq(y1-yp));
	  float xdir = (x1-xp)/dirmag;
	  float ydir = (y1-yp)/dirmag;
	  float errpx = xdir*tracklets.at(i).momentum().perp();
	  float errpy = ydir*tracklets.at(i).momentum().perp();
	  float errpz = tracklets.at(i).momentum().z();

	  //store the tracklet as a Trk::Perigee
	  AmgSymMatrix(5) * covariance2 = new AmgSymMatrix(5)(tracklets.at(i).errorMatrix());
	  Amg::Vector3D trkerrmom(errpx,errpy,errpz);
	  Amg::Vector3D trkerrpos(x1,y1,tracklets.at(i).globalPosition().z());
	  Trk::Perigee * errPerigee = new Trk::Perigee(0.,0.,trkerrmom.phi(),trkerrmom.theta(),charge/trkerrmom.mag(),Trk::PerigeeSurface(trkerrpos),covariance2);

	  TracksForErrors[k].push_back(errPerigee);
        }//end loop on vertex planes
      }//end selection of barrel tracks
    }//end loop on tracks

    //return if there are not enough tracklets
    if(nTrkToVertex < 3) return;
    
    //calculate the tracklet positions on each surface
    bool boundaryCheck = true;
    std::vector<float> ExtrapZ[MAXPLANES], dlength[MAXPLANES];//extrapolated position & uncertainty
    std::vector<std::pair<unsigned int, unsigned int> > UsedTracks[MAXPLANES];
    std::vector<bool> ExtrapSuc[MAXPLANES];//did the extrapolation succeed?
    std::vector<MSVertex*> vertices;
    vertices.reserve(nplanes);

    //total uncertainty at each plane
    std::vector<float> sigmaZ[MAXPLANES];

    //tracklet momentum expressed at the plane
    std::vector<Amg::Vector3D> pAtVx[MAXPLANES];

    for(int k=0; k<nplanes; ++k) {

      float rpos = Rpos[k];

      for(unsigned int i=0; i<TracksForVertexing[k].size(); ++i) {

	// at least three tracklets per plane are needed
	if (TracksForVertexing[k].size() < 3) break;

	Amg::Transform3D* surfaceTransformMatrix = new Amg::Transform3D;
	surfaceTransformMatrix->setIdentity();
	Trk::CylinderSurface cyl(surfaceTransformMatrix, rpos, 10000.);//create the surface
        //extrapolate to the surface
        std::unique_ptr<const Trk::TrackParameters> extrap_par
          ( m_extrapolator->extrapolate(*TracksForVertexing[k].at(i), cyl,Trk::anyDirection,boundaryCheck,Trk::muon) );

	const Trk::AtaCylinder* extrap = dynamic_cast<const Trk::AtaCylinder*>(extrap_par.get());

        if(extrap) {
	  //if the track is neutral just store the uncertainty due to angular uncertainty of the orignal tracklet
	  if(isNeutralTrack[k].at(i)) {	  
	    float pTot = sqrt(sq(TracksForVertexing[k].at(i)->momentum().perp())+sq(TracksForVertexing[k].at(i)->momentum().z()));
	    float dirErr = Amg::error(*TracksForVertexing[k].at(i)->covariance(),Trk::theta);
	    float extrapRdist = TracksForVertexing[k].at(i)->position().perp() - Rpos[k];
	    float sz = fabs(20*dirErr*extrapRdist*sq(pTot)/sq(TracksForVertexing[k].at(i)->momentum().perp()));	  
	    float ExtrapErr = sz;
	    if(ExtrapErr > maxError) ExtrapSuc[k].push_back(false);
	    else {
	      ExtrapSuc[k].push_back(true);
	      std::pair<unsigned int,unsigned int> trkmap(ExtrapZ[k].size(),i);
	      UsedTracks[k].push_back(trkmap);
	      ExtrapZ[k].push_back( extrap->localPosition().y() );	  
	      sigmaZ[k].push_back(sz);
	      pAtVx[k].push_back(extrap->momentum());
	      dlength[k].push_back(0); 
	    }	  
	  }//end neutral tracklets
	  //if the tracklet has a momentum measurement
	  else {
	    //now extrapolate taking into account the extra path length & differing magnetic field
	    Amg::Transform3D* srfTransMat2 = new Amg::Transform3D;
	    srfTransMat2->setIdentity();
	    Trk::CylinderSurface cyl2(srfTransMat2, rpos, 10000.);
            std::unique_ptr<const Trk::TrackParameters> extrap_par2
              ( m_extrapolator->extrapolate(*TracksForErrors[k].at(i), cyl,Trk::anyDirection,boundaryCheck,Trk::muon) );
            const Trk::AtaCylinder* extrap2 = dynamic_cast<const Trk::AtaCylinder*>(extrap_par2.get());

	    if(extrap2) {
	      float sz = Amg::error(*extrap->covariance(),Trk::locY);
	      float zdiff = extrap->localPosition().y() - extrap2->localPosition().y();	  
	      float ExtrapErr = sqrt(sq(sz)+sq(zdiff));
	      if(ExtrapErr > maxError) ExtrapSuc[k].push_back(false);
	      else {
		//iff both extrapolations succeed && error is acceptable, store the information 
		ExtrapSuc[k].push_back(true);
		std::pair<unsigned int,unsigned int> trkmap(ExtrapZ[k].size(),i);
		UsedTracks[k].push_back(trkmap);
		ExtrapZ[k].push_back( extrap->localPosition().y() );	  
		sigmaZ[k].push_back(sz);
		pAtVx[k].push_back(extrap->momentum());	  
		dlength[k].push_back(zdiff);
	      }
	    }		  
	    else ExtrapSuc[k].push_back(false);//not possible to calculate the uncertainty -- do not use tracklet in vertex
          }
        }//fi extrap
        else ExtrapSuc[k].push_back(false);//not possible to extrapolate the tracklet
      }//end loop on perigeebase
    }//end loop on radial planes

    //vertex routine
    std::vector<Amg::Vector3D> trkp[MAXPLANES];
    //loop on planes
    for(int k=0; k<nplanes; ++k) {
      if(ExtrapZ[k].size() < 3) continue;//require at least 3 tracklets to build a vertex
      //initialize the variables used in the routine
      float zLoF = Rpos[k]/tan(LoF);
      float dzLoF(10);
      float aveZpos(0),posWeight(0);    
      for(unsigned int i=0; i<ExtrapZ[k].size(); ++i) {
        float ExtrapErr = sqrt(sq(sigmaZ[k][i])+sq(dlength[k][i])+sq(dzLoF));
        if(isNeutralTrack[k][i]) ExtrapErr = sqrt(sq(sigmaZ[k][i]) + sq(dzLoF));
        aveZpos += ExtrapZ[k][i]/sq(ExtrapErr);
        posWeight += 1./sq(ExtrapErr);      
      }
      //calculate the weighted average position of the tracklets
      zLoF = aveZpos/posWeight;
      float zpossigma(dzLoF),Chi2(0),Chi2Prob(-1);
      int Nitr(0);
      std::vector<unsigned int> vxtracks;//tracklets to be used in the vertex routine
      std::vector<bool> blacklist;//tracklets that do not belong to the vertex
      for(unsigned int i=0; i<ExtrapZ[k].size(); ++i) blacklist.push_back(false);
      //minimum chi^2 iterative fit
      while(true) {
        vxtracks.clear();
        trkp[k].clear();
        int tmpnTrks(0);
        float tmpzLoF(0),tmpzpossigma(0),tmpchi2(0),posWeight(0);
        float worstdelz(0);      
        unsigned int iworst(0xC0FFEE);
	//loop on the tracklets, find the chi^2 contribution from each tracklet
        for(unsigned int i=0; i<ExtrapZ[k].size(); ++i) {
	  if(blacklist[i]) continue;
	  trkp[k].push_back(pAtVx[k][i]);
	  float delz = zLoF - ExtrapZ[k][i];
	  float ExtrapErr = sqrt(sq(sigmaZ[k][i])+sq(dlength[k][i])+sq(dzLoF));
	  float trkchi2 = sq(delz)/sq(ExtrapErr);
	  if(trkchi2 > worstdelz) {
	    iworst = i;
	    worstdelz = trkchi2;
	  }
	  tmpzLoF += ExtrapZ[k][i]/sq(ExtrapErr);
	  posWeight += 1./sq(ExtrapErr);
	  tmpzpossigma += sq(delz);
	  tmpchi2 += trkchi2;
	  tmpnTrks++;
        }//end loop on tracks
        if(tmpnTrks < 3) break;//stop searching for a vertex at this plane
        tmpzpossigma = sqrt(tmpzpossigma/(float)tmpnTrks);
        zLoF = tmpzLoF/posWeight;
        zpossigma = tmpzpossigma;
        float testChi2 = TMath::Prob(tmpchi2,tmpnTrks-1);
        if(testChi2 < m_VxChi2ProbCUT) blacklist[iworst] = true;
        else {
	  Chi2 = tmpchi2;
	  Chi2Prob = testChi2;
	  //loop on the tracklets and find all that belong to the vertex
	  for(unsigned int i=0; i<ExtrapZ[k].size(); ++i) {
	    float delz = zLoF - ExtrapZ[k][i];
	    float ExtrapErr = sqrt(sq(sigmaZ[k][i])+sq(dlength[k][i])+sq(dzLoF));
	    float trkErr = sqrt(sq(ExtrapErr)+sq(zpossigma)) + 0.001;
	    float trkNsigma = fabs(delz/trkErr);
	    if(trkNsigma < 3) vxtracks.push_back(i);
	  }
	  break;//found a vertex, stop removing tracklets!
        }
        if( Nitr >= ((int)ExtrapZ[k].size()-3) ) break;//stop searching for a vertex at this plane
        Nitr++;
      }//end while
      if(vxtracks.size() < 3) continue;
      std::vector<xAOD::TrackParticle*> vxTrackParticles;
      //create TrackParticle vector for all tracklets used in the vertex fit
      vxTrackParticles.reserve(vxtracks.size());
      for(std::vector<unsigned int>::iterator vxtrk=vxtracks.begin(); vxtrk!=vxtracks.end(); ++vxtrk) {
        for(unsigned int i=0; i<UsedTracks[k].size(); ++i) {
          if( (*vxtrk) == UsedTracks[k].at(i).first ) {
	    Tracklet trklt = tracklets.at(UsedTracks[k].at(i).second);
	    AmgSymMatrix(5) * covariance = new AmgSymMatrix(5)(trklt.errorMatrix());
	    Trk::Perigee * myPerigee = new Trk::Perigee(0.,0.,trklt.momentum().phi(),trklt.momentum().theta(),trklt.charge()/trklt.momentum().mag(),
							Trk::PerigeeSurface(trklt.globalPosition()),covariance);
	    xAOD::TrackParticle* trackparticle = new xAOD::TrackParticle();
	    trackparticle->makePrivateStore();
	    trackparticle->setFitQuality(1.,(int)trklt.mdtHitsOnTrack().size());
	    trackparticle->setTrackProperties(xAOD::TrackProperties::LowPtTrack);

	    trackparticle->setDefiningParameters(myPerigee->parameters()[Trk::d0],
						 myPerigee->parameters()[Trk::z0],
						 myPerigee->parameters()[Trk::phi0],
						 myPerigee->parameters()[Trk::theta],
						 myPerigee->parameters()[Trk::qOverP]);
	    std::vector<float> covMatrixVec;
	    Amg::compress(*covariance,covMatrixVec);
	    trackparticle->setDefiningParametersCovMatrixVec(covMatrixVec);

	    vxTrackParticles.push_back(trackparticle);

	    delete myPerigee;
      	    break;
          }
        }
      }
      Amg::Vector3D position(Rpos[k]*cos(avePhi),Rpos[k]*sin(avePhi),zLoF);
      vertices.push_back( new MSVertex(1,position,vxTrackParticles,Chi2Prob,Chi2,0,0,0) );
    }//end loop on Radial planes

    //delete the perigeebase
    for(int k=0; k<nplanes; ++k) {
      for(unsigned int i=0; i<TracksForVertexing[k].size(); ++i) delete TracksForVertexing[k].at(i);
      for(unsigned int i=0; i<TracksForErrors[k].size(); ++i) delete TracksForErrors[k].at(i);
    }

    //return an empty vertex in case none were reconstructed
    if(vertices.size() == 0) {
      return;
    }

    //loop on the vertex candidates and select the best based on max n(tracks) and max chi^2 probability
    unsigned int bestVx(0);
    for(unsigned int k=1; k<vertices.size(); ++k) {
      if(vertices[k]->getChi2Probability() < m_VxChi2ProbCUT || vertices[k]->getNTracks() < 3) continue; 
      if(vertices[k]->getNTracks() < vertices[bestVx]->getNTracks()) continue;
      if(vertices[k]->getNTracks() == vertices[bestVx]->getNTracks() && vertices[k]->getChi2Probability() < vertices[bestVx]->getChi2Probability()) continue;
      bestVx = k;
    }
    vtx = vertices[bestVx]->clone();
    //cleanup
    for(std::vector<MSVertex*>::iterator it=vertices.begin(); it!=vertices.end(); ++it) {
      delete (*it);
      (*it) = 0;
    }
    vertices.clear();

    return;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  void MSVertexRecoTool::MSStraightLineVx(std::vector<Tracklet> trks, MSVertex*& vtx) {

    // Running set of all vertices found.  The inner set is the indices of trks that are used to make the vertex
    std::set<std::set<int> > prelim_vx;

    // We don't consider all 3-tracklet combinations when a  high number of tracklets is found 
    // Faster method is used for > 40 tracklets
    if(trks.size() > 40) {
      MSStraightLineVx_oldMethod(trks, vtx);
      return;
    }

    // Okay, if we get here then we know there's 40 or fewer tracklets in the cluster.
    // Make a list of all 3-tracklet combinations that make vertices
    for(unsigned int i = 0; i < trks.size() - 2; i++) {
      for(unsigned int j = i+1; j < trks.size() - 1; j++) {
        for(unsigned int k = j+1; k < trks.size(); k++) {
          std::set<int> tmpTracks;
          tmpTracks.insert(i);
          tmpTracks.insert(j);
          tmpTracks.insert(k);

          Amg::Vector3D MyVx;
          MyVx = VxMinQuad(getTracklets(trks, tmpTracks));
          if(MyVx.perp() < 10000 && fabs(MyVx.z()) > 7000 && fabs(MyVx.z()) < 15000 && 
	     !EndcapHasBadTrack(getTracklets(trks, tmpTracks), MyVx))
            prelim_vx.insert(tmpTracks);
        }
      }
    }

    // If no preliminary vertices were found from 3 tracklets, then there is no vertex and we are done.
    if(prelim_vx.size() == 0)
      return;

    // The remaining algorithm is very time consuming for large numbers of tracklets.  To control this,
    // we run the old algorithm when there are too many tracklets and a vertex is found.
    if(trks.size() <= 20) {
      std::set<std::set<int> > new_prelim_vx = prelim_vx;
      std::set<std::set<int> > old_prelim_vx;

      int foundNewVx = true;
      while(foundNewVx) {
        foundNewVx = false;

        old_prelim_vx = new_prelim_vx;
        new_prelim_vx.clear();
 
        for(std::set<std::set<int> >::iterator itr = old_prelim_vx.begin(); itr != old_prelim_vx.end(); itr++) {
          for(unsigned int i_trks = 0; i_trks < trks.size(); i_trks++) {
            std::set<int> tempCluster = *itr;
            if(tempCluster.insert(i_trks).second) {
              Amg::Vector3D MyVx = VxMinQuad(getTracklets(trks, tempCluster));
              if(MyVx.perp() < 10000 && fabs(MyVx.z()) > 7000 && fabs(MyVx.z()) < 15000 && 
		 !EndcapHasBadTrack(getTracklets(trks, tempCluster), MyVx)) {
                new_prelim_vx.insert(tempCluster);
                prelim_vx.insert(tempCluster);
                foundNewVx = true;
              }
            }
          }
        }
      }
    }
    else {
      // Since there are 20 or more tracklets, we're going to use the old MSVx finding method.  Note that
      // if the old method fails, we do not return here; in this case a 3-tracklet vertex that was found 
      // earlier in this algorithm will be returned
      MSStraightLineVx_oldMethod(trks, vtx);
      if (vtx) return;
    }

    // Find the preliminary vertex with the maximum number of tracklets - that is the final vertex.  If
    // multiple preliminary vertices with same number of tracklets, the first one found is returned
    std::set<std::set<int> >::iterator prelim_vx_max = prelim_vx.begin();
    for(std::set<std::set<int> >::iterator itr = prelim_vx.begin(); itr != prelim_vx.end(); itr++) {
      if((*itr).size() > (*prelim_vx_max).size())
        prelim_vx_max = itr;
    }

    std::vector<Tracklet> tracklets = getTracklets(trks, *prelim_vx_max);
    // use tracklets to estimate the line of flight of decaying particle
    float aveX(0),aveY(0);
    for(std::vector<Tracklet>::iterator trkItr=tracklets.begin(); trkItr!=tracklets.end(); ++trkItr) {
      aveX += ((Tracklet)*trkItr).globalPosition().x();
      aveY += ((Tracklet)*trkItr).globalPosition().y();
    }
    float tracklet_vxphi = atan2(aveY,aveX);
    Amg::Vector3D MyVx = VxMinQuad(tracklets);
    float vxtheta = atan2(MyVx.x(),MyVx.z());
    float vxphi = vxPhiFinder(fabs(vxtheta),tracklet_vxphi);
      
    Amg::Vector3D vxpos(MyVx.x()*cos(vxphi),MyVx.x()*sin(vxphi),MyVx.z());
    std::vector<xAOD::TrackParticle*> vxTrkTracks; 
    for(std::vector<Tracklet>::iterator tracklet = tracklets.begin(); tracklet != tracklets.end(); tracklet++) {

      AmgSymMatrix(5)* covariance = new AmgSymMatrix(5)(((Tracklet)*tracklet).errorMatrix());
      Trk::Perigee* myPerigee = new Trk::Perigee(vxpos,((Tracklet)*tracklet).momentum(),0,vxpos,covariance);

      xAOD::TrackParticle* myTrack = new xAOD::TrackParticle();

      myTrack->makePrivateStore();
      myTrack->setFitQuality(1.,(int)((Tracklet)*tracklet).mdtHitsOnTrack().size());
      myTrack->setDefiningParameters(myPerigee->parameters()[Trk::d0],
				     myPerigee->parameters()[Trk::z0],
				     myPerigee->parameters()[Trk::phi0],
				     myPerigee->parameters()[Trk::theta],
				     myPerigee->parameters()[Trk::qOverP]);

      std::vector<float> covMatrixVec;
      Amg::compress(*covariance,covMatrixVec);
      myTrack->setDefiningParametersCovMatrixVec(covMatrixVec);

      vxTrkTracks.push_back(myTrack);

      delete myPerigee;
    }
    
    MSVertex vertex(2,vxpos,vxTrkTracks,1,vxTrkTracks.size(),0,0,0);
    vtx = vertex.clone();
    return;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  void MSVertexRecoTool::MakeDummyVertex(MSVertex*& vtx) {
    const Amg::Vector3D vxpos(-9.99, -9.99, -9.99);
    MSVertex *vertex = new MSVertex(-1, vxpos, 1., 1., 0, 0, 0);
    vtx = vertex;
    return;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  //vertex finding routine for the endcap
  void MSVertexRecoTool::MSStraightLineVx_oldMethod(std::vector<Tracklet> trks, MSVertex*& vtx) {
    //find the line of flight of the vpion
    float aveX(0),aveY(0),aveR(0),aveZ(0);
    for(std::vector<Tracklet>::iterator trkItr=trks.begin(); trkItr!=trks.end(); ++trkItr) {
      aveX += trkItr->globalPosition().x();
      aveY += trkItr->globalPosition().y();
      aveR += trkItr->globalPosition().perp();
      aveZ += trkItr->globalPosition().z();
    }
    float vxphi = atan2(aveY,aveX);
  
    Amg::Vector3D MyVx(0,0,0);
    std::vector<Tracklet> tracks = RemoveBadTrk(trks,MyVx);
    if(tracks.size() < 2) return;
  
    while(true) {
      MyVx = VxMinQuad(tracks);
      std::vector<Tracklet> Tracks = RemoveBadTrk(tracks,MyVx);
      if(tracks.size() == Tracks.size()) break;
      tracks = Tracks;
    }
    if(tracks.size() >= 3 && MyVx.x() > 0) {
      float vxtheta = atan2(MyVx.x(),MyVx.z());
      vxphi = vxPhiFinder(fabs(vxtheta),vxphi);
      Amg::Vector3D vxpos(MyVx.x()*cos(vxphi),MyVx.x()*sin(vxphi),MyVx.z());
      //make Trk::Track for each tracklet used in the vertex fit
      std::vector<xAOD::TrackParticle*> vxTrackParticles;	      
      for(std::vector<Tracklet>::iterator trklt=tracks.begin(); trklt!=tracks.end(); ++trklt) {
	AmgSymMatrix(5) * covariance = new AmgSymMatrix(5)(trklt->errorMatrix());
	Trk::Perigee* myPerigee = new Trk::Perigee(0.,0.,trklt->momentum().phi(),trklt->momentum().theta(),trklt->charge()/trklt->momentum().mag(),Trk::PerigeeSurface(trklt->globalPosition()),covariance);
	xAOD::TrackParticle* trackparticle = new xAOD::TrackParticle();
	trackparticle->makePrivateStore();
	trackparticle->setFitQuality(1.,(int)trklt->mdtHitsOnTrack().size());
	trackparticle->setTrackProperties(xAOD::TrackProperties::LowPtTrack);

	trackparticle->setDefiningParameters(myPerigee->parameters()[Trk::d0],
					     myPerigee->parameters()[Trk::z0],
					     myPerigee->parameters()[Trk::phi0],
					     myPerigee->parameters()[Trk::theta],
					     myPerigee->parameters()[Trk::qOverP]);
	std::vector<float> covMatrixVec;
	Amg::compress(*covariance,covMatrixVec);
	trackparticle->setDefiningParametersCovMatrixVec(covMatrixVec);

	vxTrackParticles.push_back(trackparticle);

	delete myPerigee;

      }

      MSVertex vertex(2,vxpos,vxTrackParticles,1,(float)vxTrackParticles.size(),0,0,0);
      vtx = vertex.clone();
    }
    return;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//

  
  std::vector<Tracklet> MSVertexRecoTool::RemoveBadTrk(std::vector<Tracklet> tracks, Amg::Vector3D Vx) {
    float MaxTollDist = 300;//max distance between the vertex and tracklet [mm]
    std::vector<Tracklet> Tracks;
    if(Vx.x() == 0 && Vx.z() == 0) return tracks;  
    //loop on all tracks and find the worst
    float WorstTrkDist = MaxTollDist;
    unsigned int iWorstTrk = 0xC0FFEE;
    for(unsigned int i=0; i<tracks.size(); ++i) {
      float TrkSlope = tan(tracks.at(i).getML1seg().alpha());
      float TrkInter = tracks.at(i).getML1seg().globalPosition().perp() - tracks.at(i).getML1seg().globalPosition().z()*TrkSlope;
      float dist = fabs((TrkSlope*Vx.z() - Vx.x() + TrkInter)/sqrt(sq(TrkSlope) + 1));
      if(dist > MaxTollDist && dist > WorstTrkDist) {
        iWorstTrk = i;
        WorstTrkDist = dist;
      }
    }
  
    //Remove the worst track from the list
    for(unsigned int i=0; i<tracks.size(); ++i) {
      if(i != iWorstTrk) Tracks.push_back(tracks.at(i));
    }
    return Tracks;
  }
  
  std::vector<Tracklet> MSVertexRecoTool::getTracklets(std::vector<Tracklet> trks, std::set<int> tracklet_subset) {
    std::vector<Tracklet> returnVal;
    for(std::set<int>::iterator itr = tracklet_subset.begin(); itr != tracklet_subset.end(); itr++) {
      if((unsigned int) *itr > trks.size())   
        ATH_MSG_ERROR( "ERROR - Index out of bounds in getTracklets"); 
      returnVal.push_back(trks.at(*itr));
    }
     
    return returnVal;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  bool MSVertexRecoTool::EndcapHasBadTrack(std::vector<Tracklet> tracks, Amg::Vector3D Vx) {
    float MaxTollDist = m_MaxTollDist;
    if(Vx.x() == 0 && Vx.z() == 0) return true;
    //loop on all tracks and find the worst
    float WorstTrkDist = MaxTollDist;
    for(std::vector<Tracklet>::iterator track = tracks.begin(); track != tracks.end(); track++) {
      float TrkSlope = tan(((Tracklet)*track).getML1seg().alpha());
      float TrkInter = ((Tracklet)*track).getML1seg().globalPosition().perp() - ((Tracklet)*track).getML1seg().globalPosition().z()*TrkSlope;
      float dist = fabs((TrkSlope*Vx.z() - Vx.x() + TrkInter)/sqrt(sq(TrkSlope) + 1));
      if(dist > MaxTollDist && dist > WorstTrkDist) {
        return true;
      }
    }
   
    // No tracks found that are too far, so it is okay.
    return false;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  StatusCode MSVertexRecoTool::FillOutputContainer(std::vector<MSVertex*>& vertices) {
    
    //create xAOD::vertex - the container needs to be created for every event
    xAOD::VertexContainer* xAODVxContainer = new xAOD::VertexContainer();
    if(evtStore()->record(xAODVxContainer,m_xAODContainerName).isFailure()) {
      ATH_MSG_WARNING("Failed to record the xAOD::VertexContainer with key " << m_xAODContainerName);
      return StatusCode::FAILURE;
    }
    
    xAOD::VertexAuxContainer* aux = new xAOD::VertexAuxContainer();
    if(evtStore()->record(aux,m_xAODContainerName+"Aux.").isFailure()) {
      ATH_MSG_WARNING("Failed to record the xAOD::VertexAuxContainer with key " << m_xAODContainerName);
      return StatusCode::FAILURE;
    }
    
    xAODVxContainer->setStore( aux );
    
    for(std::vector<MSVertex*>::const_iterator vxIt=vertices.begin(); vxIt!=vertices.end(); ++vxIt) {
      
      xAOD::Vertex* xAODVx = new xAOD::Vertex();
      xAODVx->makePrivateStore();
      xAODVx->setVertexType(xAOD::VxType::SecVtx);
      xAODVx->setPosition( (*vxIt)->getPosition() );
      xAODVx->setFitQuality( (*vxIt)->getChi2(), (*vxIt)->getNTracks()-1 );
      
      //dress the vertex with the hit counts
      xAODVx->auxdata< int >( "nMDT" ) = (*vxIt)->getNMDT();
      xAODVx->auxdata< int >( "nRPC" ) = (*vxIt)->getNRPC();
      xAODVx->auxdata< int >( "nTGC" ) = (*vxIt)->getNTGC();

      //store the new xAOD vertex
      xAODVxContainer->push_back(xAODVx);
    }

    //cleanup
    for(std::vector<MSVertex*>::iterator vxIt=vertices.begin(); vxIt!=vertices.end(); ++vxIt) {
      if ( (*vxIt) ) {
	delete (*vxIt);
	(*vxIt) = 0;
      }
    }
    vertices.clear();

    return StatusCode::SUCCESS;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//


  //core algorithm for endcap vertex reconstruction
  Amg::Vector3D MSVertexRecoTool::VxMinQuad(std::vector<Tracklet> tracks) {    
    float s(0.),sx(0.),sy(0.),sxy(0.),sxx(0.),d(0.);
    float sigma = 1.;
    for(unsigned int i=0; i<tracks.size(); ++i) {
      float TrkSlope = tan(tracks.at(i).getML1seg().alpha());
      float TrkInter = tracks.at(i).getML1seg().globalPosition().perp() - tracks.at(i).getML1seg().globalPosition().z()*TrkSlope;
      s += 1./(sq(sigma));
      sx += TrkSlope/(sq(sigma));
      sxx += sq(TrkSlope)/sq(sigma);
      sy += TrkInter/sq(sigma);
      sxy += (TrkSlope*TrkInter)/sq(sigma);
    }
    d = s*sxx - sq(sx);
    if(d == 0. ){
        Amg::Vector3D MyVx(0.,0.,0.); //return 0, no vertex was found.
        return MyVx;	
    }

    float Rpos = (sxx*sy - sx*sxy)/d;
    float Zpos = (sx*sy - s*sxy)/d;
      
    Amg::Vector3D MyVx(Rpos,0,Zpos);
  
    return MyVx;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//

  
  //vertex phi location -- determined from the RPC/TGC hits
  float MSVertexRecoTool::vxPhiFinder(float theta,float phi) {
    float nmeas(0);
    float sinphi(0),cosphi(0);
    float eta = -1*log(tan(0.5*theta));
    if(fabs(eta) < 1.5) {
      const Muon::RpcPrepDataContainer* rpcTES = 0;
      if(evtStore()->retrieve(rpcTES,"RPC_Measurements").isFailure() || !rpcTES) {
        ATH_MSG_WARNING( "No RpcPrepDataContainer found in SG!" );
        return 0;
      }
      Muon::RpcPrepDataContainer::const_iterator RpcItr = rpcTES->begin();
      Muon::RpcPrepDataContainer::const_iterator RpcItrE = rpcTES->end();
      for(; RpcItr != RpcItrE; ++RpcItr) {
        Muon::RpcPrepDataCollection::const_iterator rpcItr = (*RpcItr)->begin();
        Muon::RpcPrepDataCollection::const_iterator rpcItrE = (*RpcItr)->end();
        for(; rpcItr != rpcItrE; ++rpcItr) {
	  if(m_rpcIdHelper->measuresPhi((*rpcItr)->identify())) {
	    float rpcEta = (*rpcItr)->globalPosition().eta();
	    float rpcPhi = (*rpcItr)->globalPosition().phi();
	    float dphi = phi - rpcPhi;
	    if(dphi > m_PI) dphi -= 2*m_PI;
	    else if(dphi < -m_PI) dphi += 2*m_PI;
	    float deta = eta - rpcEta;
	    float DR = sqrt(sq(deta)+sq(dphi));
	    if(DR < 0.6) {
	      nmeas++;
	      sinphi += sin(rpcPhi);
	      cosphi += cos(rpcPhi);
	    }
	  }
        }
      }  
    }
    if(fabs(eta) > 0.5) {
      const Muon::TgcPrepDataContainer* tgcTES = 0;
      if(evtStore()->retrieve(tgcTES,"TGC_Measurements").isFailure()) {     
	ATH_MSG_WARNING( "No TgcPrepDataContainer found in SG!" );
	return 0;
      }
      Muon::TgcPrepDataContainer::const_iterator TgcItr = tgcTES->begin();
      Muon::TgcPrepDataContainer::const_iterator TgcItrE = tgcTES->end();
      for(; TgcItr != TgcItrE; ++TgcItr) {
        Muon::TgcPrepDataCollection::const_iterator tgcItr = (*TgcItr)->begin();
        Muon::TgcPrepDataCollection::const_iterator tgcItrE = (*TgcItr)->end();
        for(; tgcItr != tgcItrE; ++tgcItr) {
	  if(m_tgcIdHelper->isStrip((*tgcItr)->identify())) {
	    float tgcEta = (*tgcItr)->globalPosition().eta();
	    float tgcPhi = (*tgcItr)->globalPosition().phi();
	    float dphi = phi - tgcPhi;
	    if(dphi > m_PI) dphi -= 2*m_PI;
	    else if(dphi < -m_PI) dphi += 2*m_PI;
	    float deta = eta - tgcEta;
	    float DR = sqrt(sq(deta)+sq(dphi));
	    if(DR < 0.6) {
	      nmeas++;
              sinphi += sin(tgcPhi);
              cosphi += cos(tgcPhi);
	    }
	  }
        }
      }
    }	 
    float vxphi = phi;
    if(nmeas > 0) vxphi = atan2(sinphi/nmeas,cosphi/nmeas);
    return vxphi;
  }


//** ----------------------------------------------------------------------------------------------------------------- **//

    
  //count the hits (MDT, RPC & TGC) around the vertex
  void MSVertexRecoTool::HitCounter(MSVertex* MSRecoVx) {
    int nHighOccupancy(0);
    const Muon::MdtPrepDataContainer* mdtTES = 0;
    if(evtStore()->retrieve(mdtTES,"MDT_DriftCircles").isFailure()) {
      ATH_MSG_ERROR( "Unable to retrieve the MDT hits" );
    }
    const Muon::RpcPrepDataContainer* rpcTES = 0;
    if(evtStore()->retrieve(rpcTES,"RPC_Measurements").isFailure()) {
      ATH_MSG_ERROR( "Unable to retrieve the RPC hits" );
    }
    const Muon::TgcPrepDataContainer* tgcTES = 0;
    if(evtStore()->retrieve(tgcTES,"TGC_Measurements").isFailure()) {
      ATH_MSG_ERROR( "Unable to retrieve the MDT hits" );
    }

    //MDTs -- count the number around the vertex
    int nmdt(0);
    Muon::MdtPrepDataContainer::const_iterator MDTItr = mdtTES->begin();
    Muon::MdtPrepDataContainer::const_iterator MDTItrE = mdtTES->end();
    for(; MDTItr != MDTItrE; ++MDTItr) {      
      if( (*MDTItr)->size() == 0) continue;      
      Muon::MdtPrepDataCollection::const_iterator mdt = (*MDTItr)->begin();
      Muon::MdtPrepDataCollection::const_iterator mdtE = (*MDTItr)->end();
      Amg::Vector3D ChamberCenter = (*mdt)->detectorElement()->center();
      float deta = fabs(MSRecoVx->getPosition().eta() - ChamberCenter.eta());
      if(deta > 0.6) continue;
      float dphi = MSRecoVx->getPosition().phi() - ChamberCenter.phi();
      if(dphi > m_PI) dphi -= 2*m_PI;
      else if(dphi < -m_PI) dphi += 2*m_PI;
      if( fabs(dphi) > 0.6 ) continue;
      int nChHits(0);
      Identifier id = (*mdt)->identify();
      float nTubes = (m_mdtIdHelper->tubeLayerMax(id) - m_mdtIdHelper->tubeLayerMin(id) + 1)*(m_mdtIdHelper->tubeMax(id) - m_mdtIdHelper->tubeMin(id) + 1);
      for(; mdt != mdtE; ++mdt) {
        if((*mdt)->adc() < 50) continue;
        if((*mdt)->status() != 1) continue;
        if((*mdt)->localPosition()[Trk::locR] == 0.) continue;
	nChHits++;
      }
      nmdt += nChHits;
      double ChamberOccupancy = nChHits/nTubes;
      if(ChamberOccupancy > m_ChamberOccupancyMin) nHighOccupancy++;
    }

    ATH_MSG_DEBUG( "Found " << nHighOccupancy << " chambers near the MS vertex with occupancy greater than " 
		   << m_ChamberOccupancyMin );
    if(nHighOccupancy < m_minHighOccupancyChambers) return;
    
    //RPC -- count the number around the vertex
    int nrpc(0);
    Muon::RpcPrepDataContainer::const_iterator RpcItr = rpcTES->begin();
    Muon::RpcPrepDataContainer::const_iterator RpcItrE = rpcTES->end();
    for(; RpcItr != RpcItrE; ++RpcItr) {
      Muon::RpcPrepDataCollection::const_iterator rpcItr = (*RpcItr)->begin();
      Muon::RpcPrepDataCollection::const_iterator rpcItrE = (*RpcItr)->end();
      for(; rpcItr != rpcItrE; ++rpcItr) {
        float rpcEta = (*rpcItr)->globalPosition().eta();
        float rpcPhi = (*rpcItr)->globalPosition().phi();
        float dphi = MSRecoVx->getPosition().phi() - rpcPhi;
        if(dphi > m_PI) dphi -= 2*m_PI;
        else if(dphi < -m_PI) dphi += 2*m_PI;
        float deta = MSRecoVx->getPosition().eta() - rpcEta;
        float DR = sqrt(sq(deta)+sq(dphi));
        if(DR < 0.6) nrpc++;
        if(DR > 1.2) break;
      }
    }  
     //TGC -- count the number around the vertex
    int ntgc(0);
    Muon::TgcPrepDataContainer::const_iterator TgcItr = tgcTES->begin();
    Muon::TgcPrepDataContainer::const_iterator TgcItrE = tgcTES->end();
    for(; TgcItr != TgcItrE; ++TgcItr) {
      Muon::TgcPrepDataCollection::const_iterator tgcItr = (*TgcItr)->begin();
      Muon::TgcPrepDataCollection::const_iterator tgcItrE = (*TgcItr)->end();
      for(; tgcItr != tgcItrE; ++tgcItr) {
        float tgcEta = (*tgcItr)->globalPosition().eta();
        float tgcPhi = (*tgcItr)->globalPosition().phi();
        float dphi = MSRecoVx->getPosition().phi() - tgcPhi;
        if(dphi > m_PI) dphi -= 2*m_PI;
        else if(dphi < -m_PI) dphi += 2*m_PI;
        float deta = MSRecoVx->getPosition().eta() - tgcEta;
        float DR = sqrt(sq(deta)+sq(dphi));
        if(DR < 0.6) ntgc++;
        if(DR > 1.2) break;
      }
    }

    MSRecoVx->setNMDT(nmdt);
    MSRecoVx->setNRPC(nrpc);
    MSRecoVx->setNTGC(ntgc);
    
    return;
  }

}//end namespace
