/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 //////////////////////////////////////////////////////
// BasicVtxValidationNtupleTool.cxx
//   Source file for class BasicVtxValidationNtupleTool
//////////////////////////////////////////////////////
// (c) ATLAS Detector software
//////////////////////////////////////////////////////
// Tatjana.Lenz@cern.ch
//////////////////////////////////////////////////////

//Gaudi
#include "GaudiKernel/ITHistSvc.h" 
#include "TTree.h"

// Trk
#include "TrkVertexFitterValidationTools/BasicVtxValidationNtupleTool.h" 
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTrack/LinkToTrack.h"

//Vertexing 
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"


#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "xAODEventInfo/EventInfo.h"

//CLHEP
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// constructor
Trk::BasicVtxValidationNtupleTool::BasicVtxValidationNtupleTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
        :
        AthAlgTool(t,n,p),
        m_ntupleFileName("/NTUPLES"),
        m_ntupleDirName("VtxFitterValidation"),
        m_ntupleTreeName("EventInfo"),
        m_ntupleVtxTreeName("VxCandidate"),
        m_ntupleTrkAtVxTreeName("TrueTracksAtVertex")
{

    declareInterface<IVtxValidationNtupleTool>(this);
    // Declare the properties
    declareProperty("NtupleFileName",		      m_ntupleFileName,	       "Ntuple file handle");
    declareProperty("NtupleDirectoryName",	      m_ntupleDirName,	       "Directory name for ntuple tree");
    declareProperty("NtupleTreeName",		      m_ntupleTreeName,	       "Name of the event info ntuple tree");
    declareProperty("NtupleVtxTreeName",	      m_ntupleVtxTreeName,     "Name of the vtx ntuple tree");
    declareProperty("NtupleTrkAtVxTreeName",	      m_ntupleTrkAtVxTreeName, "Name of the tracks at vertex ntuple tree");
}

// destructor
Trk::BasicVtxValidationNtupleTool::~BasicVtxValidationNtupleTool() {}

//////////////////
/// initialize
////////////////

StatusCode Trk::BasicVtxValidationNtupleTool::initialize() {
 
    // Retrieve the Histo Service
    ITHistSvc* hist_svc;

    StatusCode status;

    status=service("THistSvc",hist_svc);
    if(status.isFailure())
    {
	    msg (MSG::ERROR) <<  "Could not find HistService" << endreq;
	    return status;
    }

   //registering the event info tree
    tree = new TTree(TString(m_ntupleTreeName), "Event info output");
    std::string fullNtupleName = m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
    status = hist_svc->regTree(fullNtupleName, tree);
    if (status.isFailure()) {
	     msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
	     return status;
    }
    //registering the vtx tree --> VxCandidate info
    vtx_tree = new TTree(TString(m_ntupleVtxTreeName), "VxCandidate output");
    std::string fullVtxNtupleName = m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleVtxTreeName;
    status = hist_svc->regTree(fullVtxNtupleName, vtx_tree);
    if (status.isFailure()) {
	    msg(MSG::ERROR) << "Unable to register TTree : " << fullVtxNtupleName << endreq;
	    return status;
    }
    //registering the true trackAtVertex tree
    trk_at_vxtree = new TTree(TString(m_ntupleTrkAtVxTreeName), "True tracks at vertex info output");
    std::string fullTrkAtVxNtupleName = m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTrkAtVxTreeName;
    status = hist_svc->regTree(fullTrkAtVxNtupleName, trk_at_vxtree);
    if (status.isFailure()) {
	    msg(MSG::ERROR) << "Unable to register TTree : " << fullTrkAtVxNtupleName << endreq;
	    return status;
    }
    //event info tree
    tree->Branch("RunNumber", &m_runNumber, "runNum/I");       // Run number
    tree->Branch("EventNumber", &m_eventNumber, "eventNum/I");   // Event number
    tree->Branch ("numVertices", &m_numVertices, "numVtx/I");

    //VxCandidate-->vtx tree
    vtx_tree->Branch ("vertex_x", &m_x, "vtx_x/F");
    vtx_tree->Branch ("vertex_y", &m_y, "vtx_y/F");
    vtx_tree->Branch ("vertex_z", &m_z, "vtx_z/F");

    vtx_tree->Branch ("vertex_error_x", &m_err_x, "vtx_err_x/F");
    vtx_tree->Branch ("vertex_error_y", &m_err_y, "vtx_err_y/F");
    vtx_tree->Branch ("vertex_error_z", &m_err_z, "vtx_err_z/F");

    vtx_tree->Branch ("vertex_chi2_per_ndf", &m_chi2, "vtx_chi2/F");
    vtx_tree->Branch ("vertex_chi2_prob", &m_chi2prob, "vtx_chi2prob/F");

    vtx_tree->Branch ("num_tracks_per_vertex", &m_numTracksPerVertex, "trk_per_vtx/I");

    //VxCandidate-->trks tree
    m_d0 = new std::vector<float>();
    m_z0 = new std::vector<float>();
    m_phi0 = new std::vector<float>();
    m_theta = new std::vector<float>();
    m_qOverP = new std::vector<float>();

    m_err_d0 = new std::vector<float>();
    m_err_z0 = new std::vector<float>();
    m_err_phi0 = new std::vector<float>();
    m_err_theta = new std::vector<float>();
    m_err_qOverP = new std::vector<float>();

    m_chi2_per_track = new std::vector<float>();

    m_initial_d0 = new std::vector<float>();
    m_initial_z0 = new std::vector<float>();
    m_initial_phi0 = new std::vector<float>();
    m_initial_theta = new std::vector<float>();
    m_initial_qOverP = new std::vector<float>();

    m_err_initial_d0 = new std::vector<float>();
    m_err_initial_z0 = new std::vector<float>();
    m_err_initial_phi0 = new std::vector<float>();
    m_err_initial_theta = new std::vector<float>();
    m_err_initial_qOverP = new std::vector<float>();

    vtx_tree->Branch ("track_d0", &m_d0);
    vtx_tree->Branch ("track_z0", &m_z0);
    vtx_tree->Branch ("track_phi0", &m_phi0);
    vtx_tree->Branch ("track_theta", &m_theta);
    vtx_tree->Branch ("track_qOverP", &m_qOverP);
    
    vtx_tree->Branch ("track_err_d0", &m_err_d0);
    vtx_tree->Branch ("track_err_z0", &m_err_z0);
    vtx_tree->Branch ("track_err_phi0", &m_err_phi0);
    vtx_tree->Branch ("track_err_theta", &m_err_theta);
    vtx_tree->Branch ("track_err_qOverP", &m_err_qOverP);

    vtx_tree->Branch ("chi2_per_track", &m_chi2_per_track);
         
    vtx_tree->Branch ("track_initial_d0", &m_initial_d0);
    vtx_tree->Branch ("track_initial_z0", &m_initial_z0);
    vtx_tree->Branch ("track_initial_phi0", &m_initial_phi0);
    vtx_tree->Branch ("track_initial_theta", &m_initial_theta);
    vtx_tree->Branch ("track_initial_qOverP", &m_initial_qOverP);

    vtx_tree->Branch ("track_err_initial_d0", &m_err_initial_d0);
    vtx_tree->Branch ("track_err_initial_z0", &m_err_initial_z0);
    vtx_tree->Branch ("track_err_initial_phi0", &m_err_initial_phi0);
    vtx_tree->Branch ("track_err_initial_theta", &m_err_initial_theta);
    vtx_tree->Branch ("track_err_initial_qOverP", &m_err_initial_qOverP);	


    //vxCandidate->true tracks tree
    m_vxprod_x = new std::vector<float>();
    m_vxprod_y= new std::vector<float>();
    m_vxprod_z = new std::vector<float>();

    m_vxparticle_id = new std::vector<int>();
    m_vxparent_id = new std::vector<int>();

    trk_at_vxtree->Branch ("vtx_track_ prod_x", &m_vxprod_x);
    trk_at_vxtree->Branch ("vtx_track_prod_y", &m_vxprod_y);
    trk_at_vxtree->Branch ("vtx_track_prod_z", &m_vxprod_z);

    trk_at_vxtree->Branch ("vtx_track_particle_id", &m_vxparticle_id);
    trk_at_vxtree->Branch ("vtx_track_parent_id", &m_vxparent_id);

    trk_at_vxtree->Branch ("vtx_track_num_tracks", &m_vxnum_trks, "vx_numTrks/I");

    return StatusCode::SUCCESS;
}

///////////////////////////////////////
/// finalize
///////////////////////////////////////
StatusCode Trk::BasicVtxValidationNtupleTool::finalize() {

    msg(MSG::DEBUG) << "start finalize() in " << name() << endreq;
    return StatusCode::SUCCESS;
}

/////////////////////////////////////
/// fill VxCandidate data
////////////////////////////////////

StatusCode Trk::BasicVtxValidationNtupleTool::fillVxCandidateData (const Trk::VxCandidate& vxCandidate) const {

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "in fillVxCandidateData(vxCandidate)"  << endreq;
    //vertex position & errors
    m_x = vxCandidate.recVertex().position()[0];
    m_y = vxCandidate.recVertex().position()[1];
    m_z = vxCandidate.recVertex().position()[2];
    m_err_x = sqrt(vxCandidate.recVertex().covariancePosition()(Trk::x,Trk::x));
    m_err_y = sqrt(vxCandidate.recVertex().covariancePosition()(Trk::y,Trk::y));
    m_err_z = sqrt(vxCandidate.recVertex().covariancePosition()(Trk::z,Trk::z));
    //fit qualities
    if (vxCandidate.recVertex().fitQuality().numberDoF() > 0.)
    {     
      double chi2 = vxCandidate.recVertex().fitQuality().chiSquared();
      int ndf = vxCandidate.recVertex().fitQuality().numberDoF();
      Genfun::CumulativeChiSquare myCumulativeChiSquare(ndf);
      m_chi2prob=1.-myCumulativeChiSquare(chi2);
      m_chi2 = chi2/ndf;
    } else { m_chi2prob = 1.; m_chi2 = 99999.;}
    // for storage of tracks used for fit, check if pointer exist
    if (vxCandidate.vxTrackAtVertex()) {
    int numTracksPerVertex = vxCandidate.vxTrackAtVertex()->size();
    m_numTracksPerVertex = numTracksPerVertex;
    //loop over tracks
    for ( int counter=0; counter < m_numTracksPerVertex; ++counter)
    {
      Trk::VxTrackAtVertex* trackAtVtx = (*vxCandidate.vxTrackAtVertex())[counter];
      const Trk::Perigee* perigee = dynamic_cast<const Trk::Perigee*>(trackAtVtx->perigeeAtVertex());
      if (!perigee) {
         ATH_MSG_ERROR ("Track parameters are not a perigee");
         return StatusCode::FAILURE;
      }

      //get perigee at vertex & errors
      m_chi2_per_track->push_back(trackAtVtx->trackQuality().chiSquared());
      m_d0->push_back(perigee->parameters()[Trk::d0]);
      m_z0->push_back(perigee->parameters()[Trk::z0]);
      m_phi0->push_back(perigee->parameters()[Trk::phi]);
      m_theta->push_back(perigee->parameters()[Trk::theta]);
      m_qOverP->push_back(perigee->parameters()[Trk::qOverP]);

      m_err_d0->push_back(Amg::error(*(perigee->covariance()),Trk::d0));
      m_err_z0->push_back(Amg::error(*(perigee->covariance()),Trk::z0));
      m_err_phi0->push_back(Amg::error(*(perigee->covariance()),Trk::phi));
      m_err_theta->push_back(Amg::error(*(perigee->covariance()),Trk::theta));
      m_err_qOverP->push_back(Amg::error(*(perigee->covariance()),Trk::qOverP));

      //initial perigee at ATLAS (0,0,0) & errors
      const Trk::Perigee* initialPerigee = dynamic_cast<const Trk::Perigee*>(trackAtVtx->initialPerigee());      
      if (!initialPerigee) {
         ATH_MSG_ERROR ("Track parameters are not a perigee");
         return StatusCode::FAILURE;
      }
      m_initial_d0->push_back(initialPerigee->parameters()[Trk::d0]);
      m_initial_z0->push_back(initialPerigee->parameters()[Trk::z0]);
      m_initial_phi0->push_back(initialPerigee->parameters()[Trk::phi]);
      m_initial_theta->push_back(initialPerigee->parameters()[Trk::theta]);
      m_initial_qOverP->push_back(initialPerigee->parameters()[Trk::qOverP]);

      m_err_initial_d0->push_back(Amg::error(*(initialPerigee->covariance()),Trk::d0));
      m_err_initial_z0->push_back(Amg::error(*(initialPerigee->covariance()),Trk::z0));
      m_err_initial_phi0->push_back(Amg::error(*(initialPerigee->covariance()),Trk::phi));
      m_err_initial_theta->push_back(Amg::error(*(initialPerigee->covariance()),Trk::theta));
      m_err_initial_qOverP->push_back(Amg::error(*(initialPerigee->covariance()),Trk::qOverP));

    }// loop over all tracks
   }
  
   vtx_tree->Fill();

   //clear
   m_chi2_per_track->clear();

   m_d0->clear();
   m_z0->clear();
   m_phi0->clear();
   m_theta->clear();
   m_qOverP->clear();

   m_err_d0->clear();
   m_err_z0->clear();
   m_err_phi0->clear();
   m_err_theta->clear();
   m_err_qOverP->clear();
			       
   m_initial_d0->clear();
   m_initial_z0->clear();
   m_initial_phi0->clear();
   m_initial_theta->clear();
   m_initial_qOverP->clear();
	       
   m_err_initial_d0->clear();
   m_err_initial_z0->clear();
   m_err_initial_phi0->clear();
   m_err_initial_theta->clear();
   m_err_initial_qOverP->clear();

   return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////
// fill true tracks at vertex data
////////////////////////////////////////////////////////////

StatusCode Trk::BasicVtxValidationNtupleTool::fillTrueTrackAtVertexInfo(const Trk::VxCandidate& vxCandidate, const TrackCollection& trk_coll, const TrackTruthCollection& trk_true_coll) const {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in fillTrueTrackAtVertexInfo(VxCandidate)"  << endreq;

    StatusCode status;


    //fill TrackAtVertex info
    if (vxCandidate.vxTrackAtVertex()) {
      int numTracksPerVertex = vxCandidate.vxTrackAtVertex()->size();
      for (int counter = 0; counter < numTracksPerVertex; ++counter)
      {
        Trk::VxTrackAtVertex* trackAtVtx = (*(vxCandidate.vxTrackAtVertex()))[counter];
        ElementLink<TrackCollection> tracklink;
        Trk::ITrackLink* trklink = trackAtVtx->trackOrParticleLink();
        Trk::LinkToTrack* linkToTrack = dynamic_cast<Trk::LinkToTrack*>(trklink);
        if (!linkToTrack) {
          ATH_MSG_ERROR ("Not a link to track.");
          return StatusCode::FAILURE;
        }
        const Trk::Track* theTrack = linkToTrack->cachedElement(); 
        tracklink.setElement(const_cast<Trk::Track*>(theTrack));
        tracklink.setStorableObject(trk_coll);
        const ElementLink<TrackCollection> tracklink2=tracklink;
        TrackTruthCollection::const_iterator found = trk_true_coll.find(tracklink2);
        if (found == trk_true_coll.end()) {
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " truth is missing" << endreq;
        }
        else { 
            TrackTruth trk_truth=found->second;
            const HepMC::GenParticle * particle;
            particle = trk_truth.particleLink();
            HepMC:: GenVertex* prod_vtx = particle->production_vertex();

            //fill parent id
            HepMC::GenVertex::particle_iterator   parent_iter = prod_vtx->particles_begin(HepMC::parents);
            m_vxparent_id->push_back((*parent_iter)->pdg_id());
            m_vxparticle_id->push_back(particle->pdg_id());

            //fill prod vertex
            m_vxprod_x->push_back(prod_vtx->position().x());
            m_vxprod_y->push_back(prod_vtx->position().y());
            m_vxprod_z->push_back(prod_vtx->position().z());

        }
      }// loop over all tracks
      m_vxnum_trks = numTracksPerVertex;
    } else { 
       if (msgLvl(MSG::DEBUG)) msg (MSG::DEBUG) << "VxCandidate without tracks!!" << endreq; 
       return 0; 
    }
    
    trk_at_vxtree->Fill();
    
      //clear
    m_vxprod_x->clear();
    m_vxprod_y->clear();
    m_vxprod_z->clear();

    m_vxparticle_id->clear();
    m_vxparent_id->clear();

    return StatusCode::SUCCESS;
}

StatusCode Trk::BasicVtxValidationNtupleTool::fillEventInfo(int& numRecVtx) const {
    // ---------------------------------------
    // reset Vtx counter if new event
   const xAOD::EventInfo* eventInfo;
   if ((evtStore()->retrieve(eventInfo)).isFailure()) { 
     if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve event info" << endreq; }

   if (m_lastEventNumber!=eventInfo->eventNumber())  m_lastEventNumber = eventInfo->eventNumber();
   m_eventNumber = eventInfo->eventNumber();
   m_runNumber=eventInfo->runNumber();
   m_numVertices = numRecVtx;
   tree->Fill();
   return StatusCode::SUCCESS;
}
