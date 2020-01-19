/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          JetFitterRoutines.h  -  Description
                             -------------------

    begin : Februar 2007
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch
    changes: new!
 
  2007 (c) Atlas Detector Software

  Look at the header file for more information.
     
 ***************************************************************************/

#include <iostream>

#include "TrkJetVxFitter/JetFitterRoutines.h"
#include "VxVertex/RecVertex.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/RecVertexPositions.h"
#include "VxJetVertex/VxClusteringTable.h"
#include "VxJetVertex/JetVtxParamDefs.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include "TrkJetVxFitter/KalmanVertexOnJetAxisUpdator.h"
#include "TrkJetVxFitter/KalmanVertexOnJetAxisSmoother.h"
#include "TrkJetVxFitter/JetFitterHelper.h"
#include "TrkJetVxFitter/JetFitterInitializationHelper.h"
#include "VxJetVertex/VxJetFitterDebugInfo.h"

#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkJetVxFitter/Utilities.h"

#include <TMath.h>

#include "TrkJetVxFitter/TrkDistanceFinderNeutralCharged.h"
#include "TrkJetVxFitter/TrkDistanceFinderNeutralNeutral.h"


//#define JetFitterRoutines_DEBUG2
//#define JetFitterRoutines_DEBUG

namespace Trk
{

  JetFitterRoutines::JetFitterRoutines(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p),
    m_initializationHelper("Trk::JetFitterInitializationHelper"),
    m_helper("Trk::JetFitterHelper"),
    m_updator("Trk::KalmanVertexOnJetAxisUpdator"),
    m_smoother("Trk::KalmanVertexOnJetAxisSmoother"),
    m_minDistanceFinder("Trk::TrkDistanceFinderNeutralCharged/TrkDistanceFinderNeutralCharged"),
    m_minDistanceFinderNeutral("Trk::TrkDistanceFinderNeutralNeutral/TrkDistanceFinderNeutralNeutral"),
    m_fast(false),
    m_maxDRshift(.5),
    m_noPrimaryVertexRefit(false),
    m_maxR(1150.),//max R of ID
    m_maxZ(2727.)//max Z of ID
  { 
    declareProperty("KalmanVertexOnJetAxisUpdator",m_updator);
    declareProperty("KalmanVertexOnJetAxisSmoother",m_smoother);
    declareProperty("JetFitterHelper",m_helper);
    declareProperty("JetFitterInitializationHelper",m_initializationHelper);
    declareProperty("JetFitterMinimumDistanceFinder",m_minDistanceFinder);
    declareProperty("JetFitterMinimumDistanceFinderNeutral",m_minDistanceFinderNeutral);
    declareProperty("ID_maxR",m_maxR);
    declareProperty("ID_maxZ",m_maxZ);
    declareInterface< JetFitterRoutines >(this) ;
    declareProperty("BeFast",m_fast);
    declareProperty("maxDRshift",m_maxDRshift);
    declareProperty("noPrimaryVertexRefit",m_noPrimaryVertexRefit);

  }


    
  JetFitterRoutines::~JetFitterRoutines() {}


  StatusCode JetFitterRoutines::initialize() {
    
    AthAlgTool::initialize().ignore();
    
    //retrieving the udator itself 	 
    StatusCode sc = m_helper.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR)<<" Unable to retrieve "<<m_helper<<endreq; 	 
      return StatusCode::FAILURE; 	 
    }

    sc = m_initializationHelper.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR)<<" Unable to retrieve "<<m_initializationHelper<<endreq; 	 
      return StatusCode::FAILURE; 	 
    }

    sc = m_minDistanceFinder.retrieve();
    if (sc.isFailure()) 
    {
      msg(MSG::ERROR)<<"Unable to retrieve " << m_minDistanceFinder << endreq;
      return StatusCode::FAILURE; 	 
    }

    sc = m_minDistanceFinderNeutral.retrieve();
    if (sc.isFailure()) 
    {
      msg(MSG::ERROR)<<"Unable to retrieve " << m_minDistanceFinderNeutral << endreq;
      return StatusCode::FAILURE; 	 
    }
    

    sc = m_updator.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR)<<" Unable to retrieve "<<m_updator<<endreq; 	 
      return StatusCode::FAILURE; 	 
    }

    sc = m_smoother.retrieve();
    if(sc.isFailure()) 	  { 	 
      msg(MSG::ERROR)<<" Unable to retrieve "<<m_smoother<<endreq; 	 
      return StatusCode::FAILURE; 	 
    }

    msg(MSG::INFO)  << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode JetFitterRoutines::finalize() {

    msg(MSG::INFO) << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;

  }

  void JetFitterRoutines::initializeToMinDistancesToJetAxis(VxJetCandidate* myJetCandidate) const {
    
    if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG ("initializingToMinDistancesToJetAxis is now implemented! Will converge faster!!! Neutrals are fully supported...");

    VertexPositions & linVertexPositions=myJetCandidate->getLinearizationVertexPositions();
    Amg::VectorX linPositions=linVertexPositions.position();
    
    
    const Trk::RecVertexPositions & recVtxPosition=myJetCandidate->getRecVertexPositions();
    const Amg::VectorX & recPosition=recVtxPosition.position();
    
    Amg::Vector3D a(recPosition[Trk::jet_xv],
                    recPosition[Trk::jet_yv],
                    recPosition[Trk::jet_zv]);
    
    Amg::Vector3D b(TMath::Cos(recPosition[Trk::jet_phi])*TMath::Sin(recPosition[Trk::jet_theta]),
                    TMath::Sin(recPosition[Trk::jet_phi])*TMath::Sin(recPosition[Trk::jet_theta]),
                    TMath::Cos(recPosition[Trk::jet_theta]));
    
    NeutralTrack myJetAxis(a,b);

    //Yes, but the seeding is more than just speed!
    const std::vector<VxVertexOnJetAxis*> & associatedVertices=myJetCandidate->getVerticesOnJetAxis();
    
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=associatedVertices.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=associatedVertices.end();
    
    if (associatedVertices.size()!=0) {//Was that your intention? to be checked... 15.03.2007
      for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;++VtxIter) {
        VxVertexOnJetAxis* myVertex=(*VtxIter);
        if (myVertex!=0) {
          
          const std::vector<VxTrackAtVertex*> & tracksAtVertex=myVertex->getTracksAtVertex();
          if (tracksAtVertex.size()>1) {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Warning in JetFitterInitializationHelper.Number of tracks at vertex is bigger than one, " << 
                "even during initialization phase. Skipping this vertex (already initialized)..." << endreq;
          } 
          else if (tracksAtVertex.size()==0)
          {
            if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No track at vertex. Internal fitter error. Contact author (GP) ... " << endreq;
          }
          else
          {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " TrackAtVertexSize is: " << tracksAtVertex.size() << endreq;
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " pointer to first element: " << tracksAtVertex[0] << endreq;
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " pointer to initialPerigee: " << tracksAtVertex[0]->initialPerigee() << endreq;

	    // RS 19.04.2011 try to fix coverity defect 22750
	    const Trk::Perigee* ptr = dynamic_cast<const Trk::Perigee*>((tracksAtVertex[0]->initialPerigee()));
            if (ptr) 
            {

              double distOnAxis=-999.;
              std::pair<Amg::Vector3D,double> result;
              try {
                result=m_minDistanceFinder->getPointAndDistance(myJetAxis,*ptr,distOnAxis);

                double R=distOnAxis*TMath::Sin(recPosition[Trk::jet_theta]);
                double Z=distOnAxis*TMath::Cos(recPosition[Trk::jet_theta]);

                if (fabs(R)>m_maxR)
                {

                  if (fabs(recPosition[Trk::jet_theta])>1e-8)
                  {
                    msg(MSG::DEBUG) << " Closest distance of track to jet axis is outside ID envelope, R=" << R << ", setting to R= " << m_maxR << endreq;
                    distOnAxis=m_maxR / fabs(TMath::Sin(recPosition[Trk::jet_theta]));
                  }
                }

                Z=distOnAxis*TMath::Cos(recPosition[Trk::jet_theta]);
                if (fabs(Z)>m_maxZ)
                {
                  if (fabs(TMath::Cos(recPosition[Trk::jet_theta]))>1e-8)
                  {
                    msg(MSG::DEBUG) << " Closest distance of track to jet axis is outside ID envelope, Z=" << Z << ", setting to Z= " << m_maxZ << endreq;
                    distOnAxis=m_maxZ / TMath::Cos(recPosition[Trk::jet_theta]);
                  }
                }
                
                linPositions[numRow(myVertex->getNumVertex())]=distOnAxis;
                  

              } catch (Error::NewtonProblem e) {
                if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Problem with Newton finder " << e.p << endreq;
              } catch (...) {
                msg(MSG::ERROR) << "Could not catch error " << endreq;
              }
              ATH_MSG_DEBUG ("initializingToMinDistancesToJetAxis for vertex number... " << 
                             myVertex->getNumVertex() << " to distance " << distOnAxis << 
                             " distance to axis " <<  result.second);
            }
            // FIXME   THIS PART IS DEAD IN NEW TRACKING EDM:
            else if (dynamic_cast<const Trk::NeutralPerigee*>((tracksAtVertex[0]->initialPerigee()))!=0)
            {
              double distOnAxis=-999.;
              std::pair<Amg::Vector3D,double> result;

              const Trk::NeutralPerigee * neutralTrack = 
                dynamic_cast<const Trk::NeutralPerigee*>((tracksAtVertex[0]->initialPerigee()));
              
              NeutralTrack myNeutralTrack(neutralTrack->position(),neutralTrack->momentum());
              
              result=m_minDistanceFinderNeutral->getPointAndDistance(myJetAxis,myNeutralTrack,distOnAxis);

              double R=distOnAxis*TMath::Sin(recPosition[Trk::jet_theta]);
              double Z=distOnAxis*TMath::Cos(recPosition[Trk::jet_theta]);

              if (fabs(R)>m_maxR)
              {
                
                if (fabs(recPosition[Trk::jet_theta])>1e-8)
                {
                  msg(MSG::DEBUG) << " Closest distance of track to jet axis is outside ID envelope, R=" << R << ", setting to R= " << m_maxR << endreq;
                  distOnAxis=m_maxR / fabs(TMath::Sin(recPosition[Trk::jet_theta]));
                }
              }
              
              Z=distOnAxis*TMath::Cos(recPosition[Trk::jet_theta]);
              if (fabs(Z)>m_maxZ)
              {
                if (fabs(TMath::Cos(recPosition[Trk::jet_theta]))>1e-8)
                {
                  msg(MSG::DEBUG) << " Closest distance of track to jet axis is outside ID envelope, Z=" << Z << ", setting to Z= " << m_maxZ << endreq;
                  distOnAxis=m_maxZ / TMath::Cos(recPosition[Trk::jet_theta]);
                }
              }
              
              linPositions[numRow(myVertex->getNumVertex())]=distOnAxis;
              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "initializingToMinDistancesToJetAxis for vertex from NEUTRAL number... " << 
                  myVertex->getNumVertex() << " to distance " << distOnAxis << " distance to axis " <<  result.second << endreq;
              
            }
            else 
            {
              if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not cast to neither CHARGED or NEUTRAL! This error is not FATAL" << endreq;
            }
          }
        } else {
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Warning in JetFitterInitializationHelper.Inconsistency found. Pointer to VxVertexOnJetAxis should be different from zero. Skipping track..." << endreq;
          throw;
        }
      }

      linVertexPositions.setPosition(linPositions);
      
    } else {
      ATH_MSG_DEBUG ("No Associated Vertices found! no initialization to minimum distance is possible.");
    }
  }

  void JetFitterRoutines::performTheFit(VxJetCandidate* myJetCandidate,
					int num_maxiterations,//default 20
					bool treat_sign_flip,//default true
					int num_signflip_maxiterations,//default 10
					double deltachi2_convergence) const {//default 0.001
    
    bool isInitialized=this->checkJetCandidate(myJetCandidate);
    
    if (!isInitialized) {
      ATH_MSG_DEBUG ("JetFitter found the candidate was not correctly initialized. Not proceeding with the fitt...");
      return;
    }
    

    
    //linearization is DONE. now do fit with all the vertices (updating them through the knowledge coming from all the tracks parameters)
    
    int num_iteration_signflip=0;
    double lastchi2=-99.;
    bool converged=false;

    if (treat_sign_flip) {
      do {

	myJetCandidate->setRecVertexPositions(RecVertexPositions(myJetCandidate->getConstraintVertexPositions()));

	msg() << myJetCandidate->getRecVertexPositions() << endreq;
	
	//linearization position is now stored in a different VertexPositions attached to the VxJetCandidate...
	m_initializationHelper->linearizeAllTracks(myJetCandidate,true);

	//now update all vertices with the new information
	updateAllVertices(myJetCandidate);

	//now do the chi2 update of all tracks after end of fit iterations
	//(this permits to invert the big weight matrix only once at the end!!)
	//	updateChi2AllVertices(myJetCandidate);
	
	num_iteration_signflip+=1;
	
	const RecVertexPositions & myRecPosition=myJetCandidate->getRecVertexPositions();

	const FitQuality & myFitQuality=myRecPosition.fitQuality();

	double actualchi2=myFitQuality.chiSquared();
#ifdef JetFitterRoutines_DEBUG
	std::cout << " last chi2 " << lastchi2 << " actual chi2 " << actualchi2 << " difference " << 
	  fabs(actualchi2-lastchi2) << " < " << deltachi2_convergence << " ? " <<  " ndf " << myFitQuality.numberDoF()<< std::endl;
#endif

	if (fabs(actualchi2-lastchi2)<deltachi2_convergence) {
	  converged=true;
	} else {

          //GP 12.3.2012 Check that no vertex is outside the ID acceptance
          copyRecoPositionsToLinearizationPositions(*myJetCandidate);

	}

	lastchi2=actualchi2;
	
      } while ((!converged)&&num_iteration_signflip<num_signflip_maxiterations);

    
      if (converged) {
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " For sign flip treatment there was convergence after " << num_iteration_signflip << endreq;
      } 
    }
    
    //    if (msgSvc()->outputLevel()== MSG::DEBUG||msgSvc()->outputLevel()== MSG::VERBOSE) {
#ifdef JetFitterRoutines_DEBUG
      std::cout << "JetFitterRoutines: after convergence with sign flip treatment: " << myJetCandidate->getRecVertexPositions() << std::endl;
#endif
      //    }

    int num_iteration=0;
    lastchi2=-99.;
    converged=false;
   
    do {
      

      myJetCandidate->setRecVertexPositions(RecVertexPositions(myJetCandidate->getConstraintVertexPositions()));


      //linearization position is now stored in a different VertexPositions attached to the VxJetCandidate...
      m_initializationHelper->linearizeAllTracks(myJetCandidate,false);
      
      //now update all vertices with the new information
      updateAllVertices(myJetCandidate);

      //now do the chi2 update of all tracks after end of fit iterations
      //(this permits to invert the big weight matrix only once at the end!!)
      //      updateChi2AllVertices(myJetCandidate);

      num_iteration+=1;
      
      const RecVertexPositions & myRecPosition=myJetCandidate->getRecVertexPositions();

      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " num_iteration (full fit): " << num_iteration << " det " << myRecPosition.covariancePosition().determinant() <<  " recVertex " << myJetCandidate->getRecVertexPositions() << endreq;

      const FitQuality & myFitQuality=myRecPosition.fitQuality();
      double actualchi2=myFitQuality.chiSquared();
      
      //	if (msgSvc()->outputLevel()== MSG::VERBOSE) {
#ifdef JetFitterRoutines_DEBUG
      std::cout << " without sign flip: last chi2 " << lastchi2 << " actual chi2 " << actualchi2 << " difference " << 
	fabs(actualchi2-lastchi2) << " < " << deltachi2_convergence << " ? " <<  " ndf " << myFitQuality.numberDoF()<< std::endl;
#endif
	  //	}

      if (fabs(actualchi2-lastchi2)<deltachi2_convergence) {
	converged=true;
      } else {
	//now set the linearization position for the next step to the actual fitted vertex
          //GP 12.3.2012 Check that no vertex is outside the ID acceptance
          copyRecoPositionsToLinearizationPositions(*myJetCandidate);
          //OLD myJetCandidate->setLinearizationVertexPositions(myJetCandidate->getRecVertexPositions());
      }
      lastchi2=actualchi2;
      
    } while ((!converged)&&num_iteration<num_maxiterations);
    
    if (converged) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Fit without sign flip treatment there was convergence after " << num_iteration << endreq;
    } 
    
    //    if (msgSvc()->outputLevel()== MSG::VERBOSE) {
    #ifdef JetFitterRoutines_DEBUG
    std::cout << "JetFitterRoutines: after convergence without sign flip treatment: " << myJetCandidate->getRecVertexPositions() << std::endl;
    #endif
      //    }
    if (num_iteration>=num_maxiterations)
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "There wasn't convergence in JetFitter after: " << num_maxiterations << endreq;
    }

    //now only the smoothing is missing as a last step... (updated momenta, chi2 + ndf of clusters,...)
    
    smoothAllVertices(myJetCandidate);
    
    Trk::VxJetFitterDebugInfo * & myDebugInfo=myJetCandidate->getDebugInfo();
    if (myDebugInfo!=0) {
      delete myDebugInfo;
    }
    myDebugInfo=new VxJetFitterDebugInfo();
    myDebugInfo->setNumFitIterations(num_iteration);
    myDebugInfo->setSignFlipNumFitIterations(num_iteration_signflip);

  }


  void JetFitterRoutines::updateAllVertices(VxJetCandidate* myJetCandidate) const {

//    std::cout << " Updating PV " << std::endl;

    int n_iteration=0;
    

    if (!m_noPrimaryVertexRefit) {
      //new iteration
      VxVertexOnJetAxis* myPrimary=myJetCandidate->getPrimaryVertex();
      const std::vector<VxTrackAtVertex*> & primaryVectorTracks=myPrimary->getTracksAtVertex();
      
      const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksBegin=primaryVectorTracks.begin();
      const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksEnd=primaryVectorTracks.end();
    
      for (std::vector<VxTrackAtVertex*>::const_iterator  primaryVectorIter=primaryVectorTracksBegin;
	   primaryVectorIter!=primaryVectorTracksEnd;++primaryVectorIter) {
	
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " RecVertexPositions before update " << myJetCandidate->getRecVertexPositions() << endreq;
	
	if ((!m_fast)) {
	  m_updator->add(*primaryVectorIter,myPrimary,myJetCandidate);
	} else {
	  m_updator->addWithFastUpdate(*primaryVectorIter,myPrimary,myJetCandidate);
	}

        const RecVertexPositions & myRecPosition=myJetCandidate->getRecVertexPositions();

        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Determinant after PRIMARY VTX update: " << n_iteration <<  " det: "  << myRecPosition.covariancePosition().determinant() <<  " recVertex " << myJetCandidate->getRecVertexPositions() << endreq;	
      }
    }

    n_iteration=0;
    
    // const RecVertexPositions & myRecPositionBeg=myJetCandidate->getRecVertexPositions();

    const std::vector<VxVertexOnJetAxis*> & associatedVertices=myJetCandidate->getVerticesOnJetAxis();
    
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=associatedVertices.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=associatedVertices.end();
    
    for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;++VtxIter) {

//      std::cout << " Updating an SV along jet axis " << std::endl;

      const std::vector<VxTrackAtVertex*> & tracksAtVertex=(*VtxIter)->getTracksAtVertex();

      const std::vector<VxTrackAtVertex*>::const_iterator TracksBegin=tracksAtVertex.begin();
      const std::vector<VxTrackAtVertex*>::const_iterator TracksEnd=tracksAtVertex.end();
      
      for (std::vector<VxTrackAtVertex*>::const_iterator  TrackVectorIter=TracksBegin;
	   TrackVectorIter!=TracksEnd;++TrackVectorIter) {
	
	if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " RecVertexPositions before update " << myJetCandidate->getRecVertexPositions() << endreq;
	
	if (!m_fast) {
	  m_updator->add(*TrackVectorIter,*VtxIter,myJetCandidate);
	} else {
	  m_updator->addWithFastUpdate(*TrackVectorIter,*VtxIter,myJetCandidate);
	}

        n_iteration+=1;

        const RecVertexPositions & myRecPosition=myJetCandidate->getRecVertexPositions();

        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Determinant after sec update: " << n_iteration <<  " det: "  << myRecPosition.covariancePosition().determinant() <<  " recVertex " << myJetCandidate->getRecVertexPositions() << endreq;
      }
    }

    const_cast<Trk::RecVertexPositions&>(myJetCandidate->getRecVertexPositions()).finalizePosition();
  }

 void JetFitterRoutines::smoothAllVertices(VxJetCandidate* myJetCandidate) const {
    //new iteration
    VxVertexOnJetAxis* myPrimary=myJetCandidate->getPrimaryVertex();

    if (!m_fast) {
      m_smoother->update(myPrimary,myJetCandidate);    
    } else {
      m_smoother->fastUpdate(myPrimary,myJetCandidate); 
    }

    const std::vector<VxVertexOnJetAxis*> & associatedVertices=myJetCandidate->getVerticesOnJetAxis();
    
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=associatedVertices.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=associatedVertices.end();
    
    for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;++VtxIter) {
      if (!m_fast) {
	m_smoother->update(*VtxIter,myJetCandidate);
      } else {
	m_smoother->fastUpdate(*VtxIter,myJetCandidate);
      }
    }
  }




  bool JetFitterRoutines::checkJetCandidate(VxJetCandidate* myJetCandidate) const {
    
    int sizeprimary(0);

    //check if primaryvertex exists
    const VxVertexOnJetAxis* myPrimary=myJetCandidate->getPrimaryVertex();
    if (myPrimary==0) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No primary vertex found in VxJetCandidate class. Initialization was not done correctly..."<< endreq; 
      return false;
    } else {
      bool ok=true;
      if (myPrimary->getNumVertex()!=-10) { 
	ok=false;
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Numvertex of primary vertex not correctly initialized. Not proceeding with the fit!" << endreq;
      }
      //      if (fabs(myPrimary->getLinearizationPosition(void))>1e-6) {
      //	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Primary vertex linearization point is not zero as it should be!" << endreq;
      //      }

      const std::vector<VxTrackAtVertex*> & primaryVectorTracks=myPrimary->getTracksAtVertex();
      
      sizeprimary=primaryVectorTracks.size();

      const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksBegin=primaryVectorTracks.begin();
      const std::vector<VxTrackAtVertex*>::const_iterator primaryVectorTracksEnd=primaryVectorTracks.end();
      
      for (std::vector<VxTrackAtVertex*>::const_iterator  primaryVectorIter=primaryVectorTracksBegin;
	   primaryVectorIter!=primaryVectorTracksEnd;++primaryVectorIter) {
	if (*primaryVectorIter==0) {
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "One of the VxTrackAtVertex is a null pointer. Not proceeding with the fit!" << endreq;
	  ok=false;
	}
      }
      if (ok==false) {
	return false;
      }
    }//end if else rimary==0
    

    bool ok=true;
    //check std::vector<VxVertexOnJetAxis*> (if pointers are not empty and if all associated tracks are not empty)
    const std::vector<VxVertexOnJetAxis*> & tracksOfVertex=myJetCandidate->getVerticesOnJetAxis();

    const std::vector<VxVertexOnJetAxis*>::const_iterator tracksBegin=tracksOfVertex.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator tracksEnd=tracksOfVertex.end();
    
    for (std::vector<VxVertexOnJetAxis*>::const_iterator tracksIter=tracksBegin;
	 tracksIter!=tracksEnd;++tracksIter) {
      if (*tracksIter==0) {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "One of the VxTrackAtVertex is a null pointer. Not proceeding with the fit!" << endreq;
	ok=false;
      } else {
	if ((*tracksIter)->getNumVertex()<0) {
	  if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "One of the VxTrackAtVertex has a not initialized numVertex. Not proceeding with the fit!" << endreq;
	  ok=false;
	}
      }
    }
    if (ok==false) {
      return false;
    }

 
    //now check if there is some track at least to do the fit...

    if (tracksOfVertex.size()==0&&sizeprimary==0) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No tracks at primary, no tracks on jet axis. Not proceeding with the fit!" << endreq;
      return false;
    }
    
    //now check if the number of tracks corrisponds to the number of components of the recVertexPositions
    const Trk::RecVertexPositions & myRecVertexPositions=myJetCandidate->getRecVertexPositions();

    const Amg::VectorX& myPosition=myRecVertexPositions.position();

    if (static_cast<unsigned int>(tracksOfVertex.size()+5)!=static_cast<unsigned int>(myPosition.rows())) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "The position matrix has " <<  myPosition.rows() << " components while " <<  tracksOfVertex.size()+5 << 
	" are expected. Not proceeding with the fit " << endreq;
    }

    const Amg::MatrixX & myErrorMatrix=myRecVertexPositions.covariancePosition();

    //check if symMatrix and myPosition are compatible (size matches)
    if (myPosition.rows()!=myErrorMatrix.rows()) {
      ATH_MSG_WARNING ("The dimension of the position vector and the covariance matrix does not match. Not performing fit...");
      return false;
    }

    //check if all the diagonal values of the covariance matrix are not zero
    for (int i=0;i<myPosition.rows();i++) {
      if (fabs(myErrorMatrix(i,i))<1e-20) {
	ATH_MSG_WARNING ("Value of cov matrix component n. " << i << " has a value smaller than 1e-8. Not considered as possible. Not performing fit...");
	return false;
      }
    }

    //well, more checks in the future!!!
    
    return true;

  }//end method

  
  std::pair<double,bool> JetFitterRoutines::fastProbabilityOfMerging(const VxVertexOnJetAxis* firstVertex,
								     const VxVertexOnJetAxis* secondVertex,
								     const VxJetCandidate* myJetCandidate) const {
    //this method is for evaluating the probability of merging in a very fast (and rough) way
    //if above a threshold you should think about evaluating the "fullProbabilityOfMerging" :-)

    const VxVertexOnJetAxis * PrimaryVertex=myJetCandidate->getPrimaryVertex();

    if (firstVertex==PrimaryVertex) {
      return fastProbabilityOfMergingWithPrimary(secondVertex,myJetCandidate);
    } else if (secondVertex==PrimaryVertex) {
      return fastProbabilityOfMergingWithPrimary(firstVertex,myJetCandidate);
    }

    return fastProbabilityOfMergingNoPrimary(firstVertex,secondVertex,myJetCandidate);
    
  }

  
  std::pair<double,bool> JetFitterRoutines::fastProbabilityOfMergingWithPrimary(const VxVertexOnJetAxis* otherVertex,
										const VxJetCandidate* myJetCandidate) const {
    

    //first get a copy of all vertex positions (this you can't avoid I fear...)
    RecVertexPositions copyOfRecVertexPositions(myJetCandidate->getRecVertexPositions());
    VertexPositions copyOfLinearizationPositions(myJetCandidate->getLinearizationVertexPositions());

    /*
    //now copy the primaryVertex and copy the tracks of otherVertex to this new primaryVertex
    VxVertexOnJetAxis primaryVertex(*myJetCandidate->getPrimaryVertex());
    m_helper->addTracksOfFirstVertexToSecondVertex(*otherVertex,primaryVertex);
    */

    double oldchi2=copyOfRecVertexPositions.fitQuality().chiSquared();
    double oldndf=copyOfRecVertexPositions.fitQuality().numberDoF();

    //#ifdef JetFitterRoutines_DEBUG2
    const Amg::VectorX & positionVector=copyOfRecVertexPositions.position();
    const Amg::MatrixX & positionCov=copyOfRecVertexPositions.covariancePosition();
    double phiold=positionVector(Trk::jet_phi);
    double thetaold=positionVector(Trk::jet_theta);
    double phierr=TMath::Sqrt(positionCov(Trk::jet_phi,Trk::jet_phi));
    double thetaerr=TMath::Sqrt(positionCov(Trk::jet_theta,Trk::jet_theta));

    //now do the merging of the second cluster to the primary vertex...
    m_helper->performKalmanConstraintToBePrimaryVertex(copyOfRecVertexPositions,
						       *otherVertex);

    double phinew=positionVector[Trk::jet_phi];
    double thetanew=positionVector[Trk::jet_theta];
    bool isshifted=
      std::pow((phinew-phiold)/phierr,2)+std::pow((thetanew-thetaold)/thetaerr,2)>m_maxDRshift*m_maxDRshift;
				   
    double chi2 = myJetCandidate->getPrimaryVertex()->fitQuality().chiSquared() + 
                  otherVertex->fitQuality().chiSquared() +
                  copyOfRecVertexPositions.fitQuality().chiSquared()-oldchi2;
    
    double ndf = copyOfRecVertexPositions.fitQuality().numberDoF()-oldndf + 
                 myJetCandidate->getPrimaryVertex()->fitQuality().numberDoF() +
                 otherVertex->fitQuality().numberDoF();

    if (chi2<0||ndf<0) {
      ATH_MSG_WARNING (" In the compatibility estimation chi2: " << chi2 << " ndf " << ndf << " giving back 0 prob ");
      return std::pair<double,bool>(0,isshifted);
    }

    return std::pair<double,bool>(TMath::Prob(chi2,(int)std::floor(ndf+0.5)),isshifted);

  }

  std::pair<double,bool> JetFitterRoutines::fastProbabilityOfMergingNoPrimary(const VxVertexOnJetAxis* firstVertex,
									      const VxVertexOnJetAxis* secondVertex,
									      const VxJetCandidate* myJetCandidate) const {
    
    //first get a copy of all vertex positions (this you can't avoid I fear...)
    RecVertexPositions copyOfRecVertexPositions(myJetCandidate->getRecVertexPositions());
    VertexPositions copyOfLinearizationPositions(myJetCandidate->getLinearizationVertexPositions());

    const FitQuality & copyOfRecVertexQuality=copyOfRecVertexPositions.fitQuality();
 
    double oldchi2=copyOfRecVertexQuality.chiSquared();
    double oldndf=copyOfRecVertexQuality.numberDoF();

    //#ifdef JetFitterRoutines_DEBUG2
    const Amg::VectorX & positionVector=copyOfRecVertexPositions.position();
    const Amg::MatrixX & positionCov=copyOfRecVertexPositions.covariancePosition();
    double phiold=positionVector(Trk::jet_phi);
    double thetaold=positionVector(Trk::jet_theta);
    double phierr=TMath::Sqrt(positionCov(Trk::jet_phi,Trk::jet_phi));
    double thetaerr=TMath::Sqrt(positionCov(Trk::jet_theta,Trk::jet_theta));

    /*
    //now copy the first vertex and copy the tracks of otherVertex to this new common vertex
    VxVertexOnJetAxis theCommonVertex(*firstVertex);
    m_helper->addTracksOfFirstVertexToSecondVertex(*secondVertex,theCommonVertex);
    */
    
    //now do the merging of the second cluster to the primary vertex...
    m_helper->performKalmanConstraintToMergeVertices(copyOfRecVertexPositions,
						     *firstVertex,
						     *secondVertex);

    double phinew=positionVector[Trk::jet_phi];
    double thetanew=positionVector[Trk::jet_theta];
    bool isshifted=std::pow((phinew-phiold)/phierr,2)+std::pow((thetanew-thetaold)/thetaerr,2)>m_maxDRshift* m_maxDRshift;

    double chi2 = firstVertex->fitQuality().chiSquared() + 
                  secondVertex->fitQuality().chiSquared() +
                  copyOfRecVertexPositions.fitQuality().chiSquared()-oldchi2;
    
    
    double ndf = copyOfRecVertexPositions.fitQuality().numberDoF()-oldndf + 
                 firstVertex->fitQuality().numberDoF()+
                 secondVertex->fitQuality().numberDoF();

    if (chi2<0||ndf<0) {
      ATH_MSG_WARNING ("In the compatibility estimation chi2: " << chi2 << " ndf " << ndf << " giving back 0 prob");
      return std::pair<double,bool>(0,isshifted);
    }

    return std::pair<double,bool>(TMath::Prob(chi2,(int)std::floor(ndf+0.5)),isshifted);


  }

  double JetFitterRoutines::fullProbabilityOfMerging(const VxVertexOnJetAxis* firstVertex,
						    const VxVertexOnJetAxis* secondVertex,
						    const VxJetCandidate* myJetCandidate,
						    int num_maxiterations,
						    bool treat_sign_flip,
						    int num_signflip_maxiterations,
						    double deltachi2_convergence) const {

    
    if (firstVertex==0||secondVertex==0||myJetCandidate==0) {
      ATH_MSG_WARNING ("zero pointer given to the full probability estimation. No estimation performed, zero prob returned ");
      return 0;
    }

    //copy the VxJetCandidate into a new object (this is expensive...)
    VxJetCandidate newJetCandidate(*myJetCandidate);


    //first find correspondence between old first and secondVertex and new ones (horrible,but...)

    std::map<const VxVertexOnJetAxis*,VxVertexOnJetAxis*> oldToNewVtxPointers;

    
    const std::vector<VxVertexOnJetAxis*> vectorOfOldJetCand=myJetCandidate->getVerticesOnJetAxis();
    const std::vector<VxVertexOnJetAxis*> vectorOfNewJetCand=newJetCandidate.getVerticesOnJetAxis();
    
    const VxVertexOnJetAxis* primaryOfFirst=myJetCandidate->getPrimaryVertex();
    VxVertexOnJetAxis* primaryOfSecond=newJetCandidate.getPrimaryVertex();

    if (primaryOfFirst==0||primaryOfSecond==0) {
      ATH_MSG_WARNING ("Empty primary vertex found when estimating fullProbOfMerging. 0 prob returned.");
      return 0;
    }

    oldToNewVtxPointers[primaryOfFirst]=primaryOfSecond;

    unsigned int sizeOfVertices=vectorOfOldJetCand.size();
    if (vectorOfNewJetCand.size()!=sizeOfVertices) {
      ATH_MSG_WARNING ("Old and new track of vertices do not match during fullProbOfMerging. 0 prob returned.");
      return 0;
    }

    for (unsigned int s=0;s<sizeOfVertices;s++) {
      const VxVertexOnJetAxis* pointer1=vectorOfOldJetCand[s];
      VxVertexOnJetAxis* pointer2=vectorOfNewJetCand[s];
      if (pointer1==0||pointer2==0) {
	ATH_MSG_WARNING ("One of the pointers of the original or copied vector of vertices is empty during fullProbOfMerging. Skipping it...");
      } else {
	oldToNewVtxPointers[pointer1]=pointer2;
      }
    }

    //now merge firstVertex and secondVertex
    VxVertexOnJetAxis* newFirstVertex=oldToNewVtxPointers[firstVertex];
    VxVertexOnJetAxis* newSecondVertex=oldToNewVtxPointers[secondVertex];

    if (newFirstVertex==0||newSecondVertex==0) {
      ATH_MSG_WARNING ("No correspondence to the given firstVertex or secondVertex in fullProbOfMerging. Returning 0 prob.");
      return 0.;
    }

    VxVertexOnJetAxis & commonVertex=m_helper->mergeVerticesInJetCandidate(*newFirstVertex,
									   *newSecondVertex,
									   newJetCandidate);

    //now you need to update the numbering scheme
    m_initializationHelper->updateTrackNumbering(&newJetCandidate);
    
    performTheFit(&newJetCandidate,num_maxiterations,
		  treat_sign_flip,
		  num_signflip_maxiterations,
		  deltachi2_convergence);

    //    const FitQuality & qualityOfMergedVertex=commonVertex.fitQuality();

#ifdef JetFitterRoutines_DEBUG
    std::cout << " End estimating full prob of merging... chi2 " << commonVertex.fitQuality().chiSquared() << " ndf " << commonVertex.fitQuality().numberDoF() << std::endl;
#endif

    return (double)TMath::Prob(commonVertex.fitQuality().chiSquared(),
			      (int)std::floor(commonVertex.fitQuality().numberDoF()+0.5));

  }

  void JetFitterRoutines::fillTableWithFullProbOfMerging(VxJetCandidate* myJetCandidate,
							 int num_maxiterations,
							 bool treat_sign_flip,
							 int num_signflip_maxiterations,
							 double deltachi2_convergence,
							 double threshold_probability) const {
    fillTableWithProbOfMerging(myJetCandidate,
			       true,
			       num_maxiterations,
			       treat_sign_flip,
			       num_signflip_maxiterations,
			       deltachi2_convergence,
			       threshold_probability);
  }

  void JetFitterRoutines::fillTableWithFastProbOfMerging(VxJetCandidate* myJetCandidate) const {
    fillTableWithProbOfMerging(myJetCandidate,false);
  }


  void JetFitterRoutines::fillTableWithProbOfMerging(VxJetCandidate* myJetCandidate,
						     bool fullcomputation,
						     int num_maxiterations,
						     bool treat_sign_flip,
						     int num_signflip_maxiterations,
						     double deltachi2_convergence,
						     double threshold_probability) const {
    
    if (myJetCandidate==0) {
      msg(MSG::WARNING) << "VxJetCandidate provided is a zero pointer. No compatibility table calculated." << endreq;
      return;
    }
    
    //first create the compatibility table object...
    Trk::VxClusteringTable* & clusteringTablePtr(myJetCandidate->getClusteringTable());
    if (clusteringTablePtr!=0) {
      delete clusteringTablePtr;
    }
    clusteringTablePtr=new Trk::VxClusteringTable();

    double highestprobability(0.);

    VxVertexOnJetAxis* primaryVertex=myJetCandidate->getPrimaryVertex();
    
    primaryVertex->setCompatibilityToPrimaryVtx(1);//stupid but assign prob 1 to primary vtx for consistency

    if (primaryVertex==0) {
      msg(MSG::WARNING) << "VxJetCandidate provided has no primary vertex. No compatibility table calculated." << endreq;
      return;
    }

    const std::vector<VxVertexOnJetAxis*> & tracksOnJetAxis=myJetCandidate->getVerticesOnJetAxis();

    //now evaluate probability of cluster forming with primary vertex
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=tracksOnJetAxis.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=tracksOnJetAxis.end();
    
    for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;
	 VtxIter!=VtxEnd;++VtxIter) {

      std::pair<double,bool> fastProbabilityAndNonLinearity=fastProbabilityOfMerging(primaryVertex,
										     *VtxIter,
										     myJetCandidate);
      
      
      ATH_MSG_VERBOSE ("Fast probability of merging between primary and " << 
	               (*VtxIter)->getNumVertex() << " is " << fastProbabilityAndNonLinearity.first);
      
#ifdef JetFitterRoutines_DEBUG2  
      std::cout  << "Fast probability of merging between primary and " << 
	(*VtxIter)->getNumVertex() << " is " << fastProbabilityAndNonLinearity.first <<  " and is max DR " << 
	fastProbabilityAndNonLinearity.second << std::endl;
#endif	

      if (fullcomputation) {
	if (fastProbabilityAndNonLinearity.first>threshold_probability) {
	  if (fastProbabilityAndNonLinearity.first>highestprobability/100.&&fastProbabilityAndNonLinearity.second==true) {
	    
	    double fullProbability=fullProbabilityOfMerging(primaryVertex,*VtxIter,
							   myJetCandidate,num_maxiterations,
							   treat_sign_flip,
							   num_signflip_maxiterations,
							   deltachi2_convergence);
	    

#ifdef JetFitterRoutines_DEBUG2     
	    std::cout << "Full probability of merging with primary is " << fullProbability << std::endl;
#endif

	    ATH_MSG_DEBUG ("Full probability of merging with primary is " << fullProbability);
	    
	    //store this fullProbability into the VxClusteringTable of myJetCandidate
	    
	    clusteringTablePtr->setCompatibilityOfTo(PairOfVxVertexOnJetAxis(primaryVertex,*VtxIter),fullProbability);
	    if (fullProbability>highestprobability) {
	      highestprobability=fullProbability;
	    }
	    (*VtxIter)->setCompatibilityToPrimaryVtx(fullProbability);
	  } else {
	    clusteringTablePtr->setCompatibilityOfTo(PairOfVxVertexOnJetAxis(primaryVertex,*VtxIter),fastProbabilityAndNonLinearity.first);
	    (*VtxIter)->setCompatibilityToPrimaryVtx(fastProbabilityAndNonLinearity.first);	    
	  }
	} else {
	  (*VtxIter)->setCompatibilityToPrimaryVtx(fastProbabilityAndNonLinearity.first);
	}
      } else {
	if (fastProbabilityAndNonLinearity.first>threshold_probability) {
	  clusteringTablePtr->setCompatibilityOfTo(PairOfVxVertexOnJetAxis(primaryVertex,*VtxIter),fastProbabilityAndNonLinearity.first);
	  if (fastProbabilityAndNonLinearity.first>highestprobability) {
	    highestprobability=fastProbabilityAndNonLinearity.first;
	  }
	}
	(*VtxIter)->setCompatibilityToPrimaryVtx(fastProbabilityAndNonLinearity.first);
      }
    }//end first iteration over tracks for compatibility with primary vertex
    
    //now all the remaining combination have to be tested...

    for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator VtxIter2=VtxBegin;
	 VtxIter2!=VtxEnd;++VtxIter2) {
      for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator VtxIter1=VtxBegin;
	   VtxIter1!=VtxIter2;++VtxIter1) {
	
	std::pair<double,bool> fastProbabilityAndNonLinearity=fastProbabilityOfMerging(*VtxIter1,
										       *VtxIter2,
										       myJetCandidate);
	
	
	
       ATH_MSG_VERBOSE ("Fast probability of merging between vtx n " <<  (*VtxIter1)->getNumVertex()<<
                        " and " << (*VtxIter2)->getNumVertex() << " is " << 
                        fastProbabilityAndNonLinearity.first << " and is max DR " <<
	                fastProbabilityAndNonLinearity.second);

#ifdef JetFitterRoutines_DEBUG2  
	std::cout << "Fast probability of merging between vtx n " <<  (*VtxIter1)->getNumVertex() << " and " << 
	  (*VtxIter2)->getNumVertex() << " is " << fastProbabilityAndNonLinearity.first << " and is max DR " << 
	  fastProbabilityAndNonLinearity.second << std::endl;
#endif	
	if (fullcomputation) {
	  if (fastProbabilityAndNonLinearity.first>threshold_probability) {
	    if (fastProbabilityAndNonLinearity.first>highestprobability/100.&&fastProbabilityAndNonLinearity.second==true) {
	      double fullProbability=fullProbabilityOfMerging(*VtxIter1,*VtxIter2,
							     myJetCandidate,num_maxiterations,
							     treat_sign_flip,
							     num_signflip_maxiterations,
							     deltachi2_convergence);
	      

#ifdef JetFitterRoutines_DEBUG2     
	      std::cout << "Full probability of merging is " << fullProbability << std::endl;
#endif

	      ATH_MSG_VERBOSE ("Full probability of merging is " << fullProbability);
	      
	      //store this fullProbability into the VxClusteringTable of myJetCandidate
	      
	      clusteringTablePtr->setCompatibilityOfTo(PairOfVxVertexOnJetAxis(*VtxIter1,*VtxIter2),fullProbability);
	      if (fullProbability>highestprobability) {
		highestprobability=fullProbability;
	      }
	    } else {
	      clusteringTablePtr->setCompatibilityOfTo(PairOfVxVertexOnJetAxis(*VtxIter1,*VtxIter2),fastProbabilityAndNonLinearity.first);
	    }
	  }
	} else {
	  if (fastProbabilityAndNonLinearity.first>threshold_probability) {
	    clusteringTablePtr->setCompatibilityOfTo(PairOfVxVertexOnJetAxis(*VtxIter1,*VtxIter2),fastProbabilityAndNonLinearity.first);
	    if (fastProbabilityAndNonLinearity.first>highestprobability) {
	      highestprobability=fastProbabilityAndNonLinearity.first;
	    }
	  }
	}
      }//end first vtx loop
    }//end second vtx loop
    
  }//end fillTableWithFullProbOfMerging() method

  void JetFitterRoutines::deleteVertexFromJetCandidate(VxVertexOnJetAxis* vertexToDelete,
						       VxJetCandidate* myJetCandidate) const {
    if (vertexToDelete==myJetCandidate->getPrimaryVertex()) {
      ATH_MSG_WARNING ("YOU ARE deleting the primary vertex. This is not possible... ");
      return;
    }

    // now you need to *delete* the second vertex in copyOfRecVertexPositions and 
    // in copyOfLinearizationPositions
    const Amg::VectorX & recPosition=myJetCandidate->getRecVertexPositions().position();
    const Amg::VectorX & linPosition=myJetCandidate->getLinearizationVertexPositions().position();
    const Amg::VectorX & constraintPosition=myJetCandidate->getConstraintVertexPositions().position();
    const Amg::MatrixX & covPosition=myJetCandidate->getRecVertexPositions().covariancePosition();
    const Amg::MatrixX & covConstraintPosition=myJetCandidate->getConstraintVertexPositions().covariancePosition();
   
    int numbVertex=numRow(vertexToDelete->getNumVertex());
    //call the function which deletes a single row (they're in the anonymous namespace, in Utilities.h)
    Amg::VectorX reducedRecPositions=deleteRowFromVector(recPosition,numbVertex);
    Amg::VectorX reducedLinPositions=deleteRowFromVector(linPosition,numbVertex);
    Amg::VectorX reducedConstraintPositions=deleteRowFromVector(constraintPosition,numbVertex);
    Amg::MatrixX reducedCovPositions=deleteRowFromSymMatrix(covPosition,numbVertex);
    Amg::MatrixX reducedConstraintCovPositions=deleteRowFromSymMatrix(covConstraintPosition,numbVertex);

    myJetCandidate->setRecVertexPositions(RecVertexPositions(reducedRecPositions,
							     reducedCovPositions,
							     0.,0.));
    myJetCandidate->setConstraintVertexPositions(RecVertexPositions(reducedConstraintPositions,
								    reducedConstraintCovPositions,
								    0.,0.));
    myJetCandidate->setLinearizationVertexPositions(VertexPositions(reducedLinPositions));



    //make a copy of the VxTrackAtVertex vector in myJetCandidate to delete some of the elements
      
    //PAY ATTENTION: here you are modifying the vector of tracks DIRECTLY...
    std::vector<VxTrackAtVertex*>* tracksAtJetCandidate(myJetCandidate->vxTrackAtVertex());
    // RS 19.04.2011 attempt to fix coverity defect 22750
    // const std::vector<VxTrackAtVertex*>::iterator TracksBegin=tracksAtJetCandidate->begin();
    // std::vector<VxTrackAtVertex*>::iterator TracksEnd=tracksAtJetCandidate->end();

    
    const std::vector<VxTrackAtVertex*> & tracksAtVtx(vertexToDelete->getTracksAtVertex());

    const std::vector<VxTrackAtVertex*>::const_iterator TracksAtVtxBegin=tracksAtVtx.begin();
    const std::vector<VxTrackAtVertex*>::const_iterator TracksAtVtxEnd=tracksAtVtx.end();

    int numberOfTracksBefore=tracksAtJetCandidate->size();
    int numberOfTracksToDelete=tracksAtVtx.size();
 
    for (std::vector<VxTrackAtVertex*>::const_iterator TracksAtVtxIter=TracksAtVtxBegin;
	 TracksAtVtxIter!=TracksAtVtxEnd;
	 ++TracksAtVtxIter) {

      // RS 19.04.2011 attempt to fix coverity defect 22750
      std::vector<VxTrackAtVertex*>::iterator TracksBegin=tracksAtJetCandidate->begin();
      std::vector<VxTrackAtVertex*>::iterator TracksEnd=tracksAtJetCandidate->end();
      for (std::vector<VxTrackAtVertex*>::iterator TracksIter=TracksBegin;TracksIter!=TracksEnd;) {
	
	if (*TracksIter==*TracksAtVtxIter) {
	  Trk::VxTrackAtVertex* oldPointer=*TracksIter;
	  TracksIter=tracksAtJetCandidate->erase(TracksIter);
	  delete oldPointer;
	  oldPointer=0;
	  TracksEnd=tracksAtJetCandidate->end();
	  break; 
	} else {
	  ++TracksIter;
	}
      }
    }

    if (numberOfTracksBefore-numberOfTracksToDelete!=(int)myJetCandidate->vxTrackAtVertex()->size()) {
      std::cout << " MISMATCH in JetFitterRoutines: the jetcandidate had: " << numberOfTracksBefore << " tracks " << 
	" and " << numberOfTracksToDelete << " to delete = " << myJetCandidate->vxTrackAtVertex()->size() << " tracks left! " << std::endl;
    }


    //now as a last step you need to delete the vertex you're not using anymore...
    
    std::vector<VxVertexOnJetAxis*> copyOfVerticesAtJetCandidate=myJetCandidate->getVerticesOnJetAxis();

    const std::vector<VxVertexOnJetAxis*>::iterator VerticesBegin=copyOfVerticesAtJetCandidate.begin();
    std::vector<VxVertexOnJetAxis*>::iterator VerticesEnd=copyOfVerticesAtJetCandidate.end();
	 
    bool found=false;
    
    //have a variable to store the vector of tracks which then need to be removed (copy)...
    //    std::vector<VxTrackAtVertex*> tracksToRemove=vertexToDelete->getTracksAtVertex();

    for (std::vector<VxVertexOnJetAxis*>::iterator VerticesIter=VerticesBegin;VerticesIter!=VerticesEnd;) {
      if ((*VerticesIter)==vertexToDelete) {
	delete *VerticesIter;
	VerticesIter=copyOfVerticesAtJetCandidate.erase(VerticesIter);
	VerticesEnd=copyOfVerticesAtJetCandidate.end();
	found=true;
	break;
      } else {
	++VerticesIter;
      }
    }
    
    if (!found) {
      msg(MSG::WARNING) << "Could not find the vertex to delete... Very strange... Check!!! " << endreq;
    }

    //update myJetCandidate with the new vector of tracks
    myJetCandidate->setVerticesOnJetAxis(copyOfVerticesAtJetCandidate);
   
    //update the numbering scheme
    m_initializationHelper->updateTrackNumbering(myJetCandidate);

  }

  void JetFitterRoutines::copyRecoPositionsToLinearizationPositions(VxJetCandidate & myJetCandidate) const
  {

    const VertexPositions & newLinVertexPositions=myJetCandidate.getRecVertexPositions();
    Amg::VectorX linPositions=newLinVertexPositions.position();
    
    const std::vector<VxVertexOnJetAxis*> & associatedVertices=myJetCandidate.getVerticesOnJetAxis();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxBegin=associatedVertices.begin();
    const std::vector<VxVertexOnJetAxis*>::const_iterator VtxEnd=associatedVertices.end();
    
    for (std::vector<VxVertexOnJetAxis*>::const_iterator VtxIter=VtxBegin;VtxIter!=VtxEnd;++VtxIter) {
      VxVertexOnJetAxis* myVertex=(*VtxIter);
      if (myVertex!=0) {
        
        double distOnAxis=linPositions[numRow(myVertex->getNumVertex())];
        
        double R=distOnAxis*TMath::Sin(linPositions[Trk::jet_theta]);
        double Z=distOnAxis*TMath::Cos(linPositions[Trk::jet_theta]);
        if (fabs(R)>m_maxR)
        {
          if (fabs(linPositions[Trk::jet_theta])>1e-8)
          {
            ATH_MSG_DEBUG (" Closest distance of track to jet axis is outside ID envelope, R=" << R << ", setting to R= " << m_maxR);
            distOnAxis=m_maxR / fabs(TMath::Sin(linPositions[Trk::jet_theta]));
          }
        }
        
        Z=distOnAxis*TMath::Cos(linPositions[Trk::jet_theta]);
        if (fabs(Z)>m_maxZ)
        {
          if (fabs(TMath::Cos(linPositions[Trk::jet_theta]))>1e-8)
          {
            msg(MSG::DEBUG) << " Closest distance of track to jet axis is outside ID envelope, Z=" << Z << ", setting to Z= " << m_maxZ << endreq;
            distOnAxis=m_maxZ / TMath::Cos(linPositions[Trk::jet_theta]);
          }
        }
        
        linPositions[numRow(myVertex->getNumVertex())]=distOnAxis;
      }
    }
    
    VertexPositions & linVertexPositions=myJetCandidate.getLinearizationVertexPositions();
    linVertexPositions.setPosition(linPositions);
    //now set the linearization position for the next step to the actual fitted vertex
    //OLD CODE BEFORE CHECKS
    //myJetCandidate->setLinearizationVertexPositions(myJetCandidate->getRecVertexPositions());
  }
  
}//end namespace
