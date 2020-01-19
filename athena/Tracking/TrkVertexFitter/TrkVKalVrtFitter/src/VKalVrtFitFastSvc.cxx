/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrkVKalVrtFitter/VKalVrtAtlas.h"
//-------------------------------------------------
//
#include<iostream>
#include<list>
#include "TrkTrack/TrackInfo.h"


namespace Trk {
 extern void vkvfast_( double* , double* , double* , double*);
 extern void vkvFastV( double* , double* , double* vRef, double dbmag, double*);
 extern vkalPropagator  myPropagator;
}
//
//__________________________________________________________________________
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


namespace Trk{

//-----------------------------------------------------------------------------------------
//  Standard (old) code
//



  StatusCode TrkVKalVrtFitter::VKalVrtFitFast(const std::vector<const Track*>& InpTrk,
                                              Amg::Vector3D& Vertex) {
//
//--- Magnetic field
//
    if(!isFieldInitialized)setInitializedField();  //to allow callback for init
    if(m_useMagFieldRotation) return StatusCode::FAILURE;
    if(m_PropagatorType <=1 ){  Trk::myPropagator.setTypeProp(m_PropagatorType);   // needed for reenterability
    }else{                      Trk::myPropagator.setPropagator(m_fitPropagator);} // needed for reenterability
//
//  Convert particles and setup reference frame
//
    long int ntrk=0; 
    StatusCode sc = CvtTrkTrack(InpTrk,ntrk);
    if(sc.isFailure() || ntrk<1 ) return StatusCode::FAILURE; 
    double fx,fy,m_BMAG_CUR;
    m_fitField->getMagFld(0.,0.,0.,fx,fy,m_BMAG_CUR);
    if(fabs(m_BMAG_CUR) < 0.1) m_BMAG_CUR=0.1;
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::list<double> xx,yy,zz;
    Vertex[0]=Vertex[1]=Vertex[2]=0.;
//
//
    double xyz0[3]={ -m_refFrameX, -m_refFrameY, -m_refFrameZ};
    if(ntrk==2){	 
    //Trk::vkvfast_(&m_apar[0][0],&m_apar[1][0],&m_BMAG_CUR,out);
      Trk::vkvFastV(&m_apar[0][0],&m_apar[1][0], xyz0, m_BMAG_CUR, out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
          //Trk::vkvfast_(&m_apar[i][0],&m_apar[j][0],&m_BMAG_CUR,out);
            Trk::vkvFastV(&m_apar[i][0],&m_apar[j][0], xyz0, m_BMAG_CUR, out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	xx.sort(); yy.sort(); zz.sort();

	std::list<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex[0]= out[0] + m_refFrameX;
    Vertex[1]= out[1] + m_refFrameY;
    Vertex[2]= out[2] + m_refFrameZ;


    return StatusCode::SUCCESS;
  }


  StatusCode TrkVKalVrtFitter::VKalVrtFitFast(const std::vector<const xAOD::TrackParticle*>& InpTrk,
                                              Amg::Vector3D& Vertex) {
//
//--- Magnetic field
//
    if(!isFieldInitialized)setInitializedField();  //to allow callback for init
    if(m_useMagFieldRotation) return StatusCode::FAILURE;
    if(m_PropagatorType <=1 ){    Trk::myPropagator.setTypeProp(m_PropagatorType);    // needed for reenterability
    }else{                        Trk::myPropagator.setPropagator(m_fitPropagator); } // needed for reenterability
//
//  Convert particles and setup reference frame
//
    long int ntrk=0; 
    StatusCode sc = CvtTrackParticle(InpTrk,ntrk);
    if(sc.isFailure() || ntrk<1 ) return StatusCode::FAILURE; 
    double fx,fy,m_BMAG_CUR;
    m_fitField->getMagFld(0.,0.,0.,fx,fy,m_BMAG_CUR);
    if(fabs(m_BMAG_CUR) < 0.1) m_BMAG_CUR=0.1;
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::list<double> xx,yy,zz;
    Vertex[0]=Vertex[1]=Vertex[2]=0.;
//
//
    double xyz0[3]={ -m_refFrameX, -m_refFrameY, -m_refFrameZ};
    if(ntrk==2){	 
    //Trk::vkvfast_(&m_apar[0][0],&m_apar[1][0],&m_BMAG_CUR,out);
      Trk::vkvFastV(&m_apar[0][0],&m_apar[1][0], xyz0, m_BMAG_CUR, out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
          //Trk::vkvfast_(&m_apar[i][0],&m_apar[j][0],&m_BMAG_CUR,out);
            Trk::vkvFastV(&m_apar[i][0],&m_apar[j][0], xyz0, m_BMAG_CUR, out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	xx.sort(); yy.sort(); zz.sort();

	std::list<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex[0]= out[0] + m_refFrameX;
    Vertex[1]= out[1] + m_refFrameY;
    Vertex[2]= out[2] + m_refFrameZ;


    return StatusCode::SUCCESS;
  }


  StatusCode TrkVKalVrtFitter::VKalVrtFitFast(const std::vector<const TrackParticleBase*>& InpTrk,
                                              Amg::Vector3D& Vertex) {
//
//--- Magnetic field
//
    if(!isFieldInitialized)setInitializedField();  //to allow callback for init
    if(m_useMagFieldRotation) return StatusCode::FAILURE;
    if(m_PropagatorType <=1 ){  Trk::myPropagator.setTypeProp(m_PropagatorType);    // needed for reenterability
    }else{                      Trk::myPropagator.setPropagator(m_fitPropagator); } // needed for reenterability
//
//  Convert particles and setup reference frame
//
    long int ntrk=0; 
    StatusCode sc = CvtTrackParticle(InpTrk,ntrk);
    if(sc.isFailure() || ntrk<1 ) return StatusCode::FAILURE; 
    double fx,fy,m_BMAG_CUR;
    m_fitField->getMagFld(0.,0.,0.,fx,fy,m_BMAG_CUR);
    if(fabs(m_BMAG_CUR) < 0.1) m_BMAG_CUR=0.1;
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::list<double> xx,yy,zz;
    Vertex[0]=Vertex[1]=Vertex[2]=0.;
//
//
    double xyz0[3]={ -m_refFrameX, -m_refFrameY, -m_refFrameZ};
    if(ntrk==2){	 
    //Trk::vkvfast_(&m_apar[0][0],&m_apar[1][0],&m_BMAG_CUR,out);
      Trk::vkvFastV(&m_apar[0][0],&m_apar[1][0], xyz0, m_BMAG_CUR, out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
          //Trk::vkvfast_(&m_apar[i][0],&m_apar[j][0],&m_BMAG_CUR,out);
            Trk::vkvFastV(&m_apar[i][0],&m_apar[j][0], xyz0, m_BMAG_CUR, out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	xx.sort(); yy.sort(); zz.sort();

	std::list<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex[0]= out[0] + m_refFrameX;
    Vertex[1]= out[1] + m_refFrameY;
    Vertex[2]= out[2] + m_refFrameZ;


    return StatusCode::SUCCESS;
  }


  StatusCode TrkVKalVrtFitter::VKalVrtFitFast(const std::vector<const TrackParameters*>& InpTrk,
                                              Amg::Vector3D& Vertex) {
//
//--- Magnetic field
//
    if(!isFieldInitialized)setInitializedField();  //to allow callback for init
    if(m_PropagatorType <=1 ){  Trk::myPropagator.setTypeProp(m_PropagatorType);     // needed for reenterability
    }else{                      Trk::myPropagator.setPropagator(m_fitPropagator); }  // needed for reenterability
//
//  Convert particles and setup reference frame
//
    long int ntrk=0; 
    StatusCode sc = CvtTrackParameters(InpTrk,ntrk);
    if(sc.isFailure() || ntrk<1 ) return StatusCode::FAILURE; 
    double fx,fy,m_BMAG_CUR;
    m_fitField->getMagFld(0.,0.,0.,fx,fy,m_BMAG_CUR);
    if(fabs(m_BMAG_CUR) < 0.1) m_BMAG_CUR=0.1;
//
//------ Variables and arrays needed for fitting kernel
//
    double out[3];
    int i,j;
    std::list<double> xx,yy,zz;
    Vertex[0]=Vertex[1]=Vertex[2]=0.;
//
//
    double xyz0[3]={ -m_refFrameX, -m_refFrameY, -m_refFrameZ};
    if(ntrk==2){	 
    //Trk::vkvfast_(&m_apar[0][0],&m_apar[1][0],&m_BMAG_CUR,out);
      Trk::vkvFastV(&m_apar[0][0],&m_apar[1][0], xyz0, m_BMAG_CUR, out);
    } else {
      for( i=0;      i<ntrk-1; i++){
	 for( j=i+1; j<ntrk;   j++){
          //Trk::vkvfast_(&m_apar[i][0],&m_apar[j][0],&m_BMAG_CUR,out);
            Trk::vkvFastV(&m_apar[i][0],&m_apar[j][0], xyz0, m_BMAG_CUR, out);
	    xx.push_back(out[0]);
	    yy.push_back(out[1]);
	    zz.push_back(out[2]);
	  }
	}
	int n=xx.size();
	xx.sort(); yy.sort(); zz.sort();

	std::list<double>::iterator it1,it2;
	it1=it2=xx.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[0]=0.5*( (*it1) + (*it2) );
		
	it1=it2=yy.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[1]=0.5*( (*it1) + (*it2) );

	it1=it2=zz.begin();
	for(i=0; i<((n+1)/2); i++,it1++){}; for(i=0; i<(n/2+1);i++,it2++){};
	out[2]=0.5*( (*it1) + (*it2) );

    }
    Vertex[0]= out[0] + m_refFrameX;
    Vertex[1]= out[1] + m_refFrameY;
    Vertex[2]= out[2] + m_refFrameZ;


    return StatusCode::SUCCESS;
  }


}

