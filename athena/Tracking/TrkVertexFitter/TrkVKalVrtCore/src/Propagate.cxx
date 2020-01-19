/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Creates interface vkalPropagator object which contains pointers to real 
//  propagators ( external or internal)
//
//  Single myPropagator object of vkalPropagator type is created in CFit.cxx
//------------------------------------------------------------------------
// Header include
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include "TrkVKalVrtCore/Propagator.h"
#include "TrkVKalVrtCore/VKalVrtBMag.h"
#include <math.h>
//-------------------------------------------------
#include <iostream>

namespace Trk {

extern vkalMagFld      myMagFld;


extern void cfnewp(long int*, double*, double*, double*, double*, double*);
extern void cferpr(long int*, double*, double*, double*, double*, double*);
extern void cfnewpm (double*, double*, double*, double*, double*, double*);

//------------------------------------------------------------------------
//  Old propagator functions:
//      CFNEWP  - doesn't use magnetic field at all. Only track curvature.
//                So it's symmetric for back and forward propagation
//      CFNEWPM - use exact nonuniform magnetic field so is not symmetric.
//                Used for forward propagation  (0,0,0) -> REF.
//                Returns perigee parameters with respect to REF 
//                (then in new reference system with center at REF!!!)
//
//  Usually all package routines propagate from (0,0,0) to REF.
//  Only XYZTRP propagates from REF to (0,0,0) and then uses BACKPROPAGATION


//   Propagation now is done from  RefStart point to RefEnd point
//

//   A final position after propagation is PERIGEE assuming that a REF(target)
//   is a CENTER OF NEW COORDINATE SYSTEM  with axes parallel to initial ones


//-------------------------------------------------------------------------
//   Simple propagator in constant field
//   Due to a translation invariance propagation is done in relative coordinates
//   assuming that starting point is (0,0,0)
//
   void PropagateSTD(long int, long int Charge, 
                     double *ParOld, double *CovOld, double *RefStart,
                     double *RefEnd, double *ParNew, double *CovNew) 
   {
      double Way,closePoint[3],Goal[3];
      Goal[0]=RefEnd[0]-RefStart[0];
      Goal[1]=RefEnd[1]-RefStart[1];
      Goal[2]=RefEnd[2]-RefStart[2];
      cfnewp( &Charge, ParOld, &Goal[0], &Way, ParNew, closePoint); 
      if(CovOld != 0) cferpr( &Charge, ParOld, &Goal[0], &Way, CovOld, CovNew);
      if(Charge==0){                      // Correction for different magnetic field values in Ini and End
        double vBx,vBy,vBz,vBzn;
        myMagFld.getMagFld(RefStart[0],RefStart[1],RefStart[2],vBx,vBy,vBz);
        myMagFld.getMagFld(RefEnd[0],RefEnd[1],RefEnd[2],vBx,vBy,vBzn);
        double Corr = vBzn/vBz;
        ParNew[4]  *= Corr;
        if(CovOld != 0){
	  CovNew[10] *= Corr;
	  CovNew[11] *= Corr;
	  CovNew[12] *= Corr;
	  CovNew[13] *= Corr;
	  CovNew[14] *= Corr*Corr;
        }
      }
   }


//--------------------------------------------------------------------------------
//   Runge-Kutta propagator in nonuniform field
//
   void PropagateRKM(long int Charge, double *ParOld, double *CovOld, double *RefStart, 
                                      double *RefEnd, double *ParNew, double *CovNew)
   {
      double Way;
      double closePoint[3],Goal[3];
      Goal[0]=RefEnd[0]-RefStart[0];
      Goal[1]=RefEnd[1]-RefStart[1];
      Goal[2]=RefEnd[2]-RefStart[2];
      cfnewp( &Charge, ParOld, &Goal[0], &Way, ParNew, closePoint); 
      if(CovOld != 0)cferpr( &Charge, ParOld, &Goal[0], &Way, CovOld, CovNew);

// double Save[5];  if(Way > 100){for (int ii=0; ii<5;ii++) Save[ii]=ParNew[ii];}
// double Save[4];  if(Way > 10.){for (int ii=0; ii<3;ii++) Save[ii]=closePoint[ii];Save[3]=ParNew[0];}
 
      if ( Charge != 0) cfnewpm( ParOld, RefStart, RefEnd, &Way, ParNew, closePoint);

//if(Way > 10){
//  if(fabs(ParNew[3]- Save[3])>0.5 ) std::cout<<" ERROR ="<<ParNew[3]<<", "<<Save[3]<<",Way="<<Way<<'\n';
//       std::cout <<" Way="<<Way<<'\n';
//       std::cout <<" Old="<<Save[0]<<", "<<Save[1]<<", "<<Save[2]<<", "<<Save[3]<<'\n';
//       std::cout <<" New="<<closePoint[0]<<", "<<closePoint[1]<<", "<<closePoint[2]<<", "<<ParNew[0]<<'\n';
// for (int ii=0; ii<3;ii++) {
//   if(fabs(Save[ii]-ParNew[ii])>1.e-4) {
//       std::cout <<" Error="<<ii<<", "<<fabs(Save[ii]-ParNew[ii])<<", "<<Way<<'\n';
//       std::cout <<" Way="<<Way<<'\n';
//       for (int jj=0; jj<5;jj++) std::cout <<" Save="<<Save[jj]<<'\n';
//       for (int jj=0; jj<5;jj++) std::cout <<" ParNew="<<ParNew[jj]-Save[jj]<<'\n';
//       for (int jj=0; jj<5;jj++) std::cout <<" ParOld="<<ParOld[jj]<<'\n';
//       break;
//   }
// }
//}
      if(Charge==0){                      // Correction for different magnetic field values in Ini and End
        double vBx,vBy,vBz,vBzn;
        myMagFld.getMagFld(RefStart[0],RefStart[1],RefStart[2],vBx,vBy,vBz);
        myMagFld.getMagFld(RefEnd[0],RefEnd[1],RefEnd[2],vBx,vBy,vBzn);
        double Corr = vBzn/vBz;
        ParNew[4]  *= Corr;
        if(CovOld != 0){
	  CovNew[10] *= Corr;
	  CovNew[11] *= Corr;
	  CovNew[12] *= Corr;
	  CovNew[13] *= Corr;
	  CovNew[14] *= Corr*Corr;
        }
      }
   }

//----------------------------------------------------------------------------------------
//                         Propagator object 
//
   vkalPropagator::vkalPropagator() 
   {
      m_functionProp  = Trk::PropagateSTD;
      m_typePropagator = 0;
      m_objectProp = 0;
   }
   vkalPropagator::~vkalPropagator() {}

   basePropagator::basePropagator() {}
   basePropagator::~basePropagator() {}

   void vkalPropagator::setTypeProp(int Type){
     m_typePropagator = Type;
     if(m_typePropagator < 0) m_typePropagator=0;
     if(m_typePropagator > 3) m_typePropagator=0;
   }

   void vkalPropagator::setPropagator(addrPropagator newProp) 
   {
      m_functionProp = newProp;
      m_objectProp = 0;
      m_typePropagator = 2;
   }
   
   void vkalPropagator::setPropagator(basePropagator* newProp) 
   {
      m_functionProp = 0;
      m_objectProp = newProp;
      m_typePropagator = 3;
   }

   bool vkalPropagator::checkTarget(double *RefNew) const 
   {
      if ( m_typePropagator == 0 ) return  true;
      if ( m_typePropagator == 1 ) return  true;
      if ( m_typePropagator == 2 ) return  true;     
      if ( m_typePropagator == 3 ) return m_objectProp->checkTarget(RefNew);
      return  true;
   }
 
   void vkalPropagator::Propagate(long int TrkID, long int Charge, 
                                  double *ParOld, double *CovOld, double *RefOld, 
                                  double *RefNew, double *ParNew, double *CovNew) const
   {
//std::cout<<" VKal PROP="<<typePropagator<<", "<<Charge<<'\n';
     if( RefOld[0]==RefNew[0] && RefOld[1]==RefNew[1] && RefOld[2]==RefNew[2]){
       for (int i=0; i<5;  i++) ParNew[i]=ParOld[i];
       if(CovOld != 0) { for (int i=0; i<15; i++) CovNew[i]=CovOld[i];}
       return;
     }
     if ( m_typePropagator == 0 ) Trk::PropagateSTD( TrkID, Charge, ParOld, CovOld, RefOld, RefNew, ParNew, CovNew);
     if ( m_typePropagator == 1 ) Trk::PropagateRKM( Charge, ParOld, CovOld, RefOld, RefNew, ParNew, CovNew);
     if ( m_typePropagator == 2 ) m_functionProp( TrkID, Charge, ParOld, CovOld, RefOld, RefNew, ParNew, CovNew);     
     if ( m_typePropagator == 3 ) {
	if( Charge == 0 ) {
	  Trk::PropagateSTD( TrkID,Charge, ParOld, CovOld, RefOld, RefNew, ParNew, CovNew);
	}else{
//std::cout<<"Propagation dist="<<RefOld[0]<<" - "<<RefNew[0]<<'\n';
//Trk::PropagateRKM( Charge, ParOld, CovOld, RefOld, RefNew, ParNew, CovNew);// Test
//std::cout<<" RKM ="<<ParNew[0]<<", "<<ParNew[1]<<", "<<ParNew[2]<<", "<<ParNew[3]<<", "<<ParNew[4]<<'\n';
//std::cout<<" ATLrefn="<<RefNew[0]<<", "<<RefNew[1]<<", "<<RefNew[2]<<'\n';
          m_objectProp->Propagate( TrkID, Charge, ParOld, CovOld, RefOld, RefNew, ParNew, CovNew);     
//std::cout<<" ATLpar ="<<ParNew[0]<<", "<<ParNew[1]<<", "<<ParNew[2]<<", "<<ParNew[3]<<", "<<ParNew[4]<<'\n';
                   // Protection against propagation fail
	  if( ParNew[0]==0. && ParNew[1]==0. && ParNew[2]==0. && ParNew[3]==0. && ParNew[4]==0.){
            Trk::PropagateRKM( Charge, ParOld, CovOld, RefOld, RefNew, ParNew, CovNew);
	  }
        }
     }
//std::cout<<" refn="<<RefNew[0]<<", "<<RefNew[1]<<", "<<RefNew[2]<<", "<<typePropagator <<'\n';
//std::cout<<" refo="<<RefOld[0]<<", "<<RefOld[1]<<", "<<RefOld[2]<<'\n';
//std::cout<<" prob="<<ParOld[0]<<", "<<ParOld[1]<<", "<<ParOld[2]<<", "<<ParOld[3]<<", "<<ParOld[4]<<'\n';
//std::cout<<" prop="<<ParNew[0]<<", "<<ParNew[1]<<", "<<ParNew[2]<<", "<<ParNew[3]<<", "<<ParNew[4]<<'\n';
     return;
   }

   void vkalPropagator::Propagate( VKTrack * trk, double *RefOld, double *RefNew,
                                   double *ParNew, double *CovNew) const
   {
     if( RefOld[0]==RefNew[0] && RefOld[1]==RefNew[1] && RefOld[2]==RefNew[2]){
       for (int i=0; i<5;  i++) ParNew[i]=trk->refPerig[i];
       for (int i=0; i<15; i++) CovNew[i]=trk->refCovar[i];
       return;
     }
     long int TrkID = trk->Id; long int Charge = trk->Charge;
     if ( m_typePropagator == 0 ) Trk::PropagateSTD( TrkID, Charge, trk->refPerig, trk->refCovar, RefOld, RefNew, ParNew, CovNew);
     if ( m_typePropagator == 1 ) Trk::PropagateRKM(        Charge, trk->refPerig, trk->refCovar, RefOld, RefNew, ParNew, CovNew);
     if ( m_typePropagator == 2 ) m_functionProp(      TrkID, Charge, trk->refPerig, trk->refCovar, RefOld, RefNew, ParNew, CovNew);     
     if ( m_typePropagator == 3 ) {
	if( Charge == 0 ) {
	  Trk::PropagateSTD( TrkID,Charge, trk->refPerig, trk->refCovar, RefOld, RefNew, ParNew, CovNew);
	}else{
          m_objectProp->Propagate( TrkID, Charge, trk->refPerig, trk->refCovar, RefOld, RefNew, ParNew, CovNew);     
	  if( ParNew[0]==0. && ParNew[1]==0. && ParNew[2]==0. && ParNew[3]==0. && ParNew[4]==0.){
            Trk::PropagateRKM( Charge,trk->refPerig, trk->refCovar, RefOld, RefNew, ParNew, CovNew);
	  }
        }
     }
//std::cout<<" refn="<<RefNew[0]<<", "<<RefNew[1]<<", "<<RefNew[2]<<", "<<typePropagator <<'\n';
//std::cout<<" refo="<<RefOld[0]<<", "<<RefOld[1]<<", "<<RefOld[2]<<'\n';
//std::cout<<" prob="<<trk->refPerig[0]<<", "<<trk->refPerig[1]<<", "<<trk->refPerig[2]<<", "<<trk->refPerig[3]<<", "<<trk->refPerig[4]<<'\n';
//std::cout<<" prop="<<ParNew[0]<<", "<<ParNew[1]<<", "<<ParNew[2]<<", "<<ParNew[3]<<", "<<ParNew[4]<<'\n';
     return;
   }



} // end of namespace bracket

