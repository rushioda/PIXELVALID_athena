/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/WorkArray.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"
#include <iostream>

namespace Trk {


extern WorkArray workarray_;
extern DerivT derivt_;


#define derivt_1 derivt_
#define workarray_1 workarray_


#define ader_ref(a_1,a_2) workarray_1.ader[(a_2)*(NTrkM*3+3) + (a_1) - (NTrkM*3+4)]


/* -------------------------------------------------------------- */
/*  RETURN FULL ERROR MATRIX AFTER THE FIT                        */
/*  ERRMTX SHOULD HAVE AT LEAST (3*NTRK+3)*(3*NTRK+4)/2. ELEMENTS */


int fiterm(long int NTRK, double  *errmtx)
{
    int ii=0, i, j;
    int lim = (NTRK+1)*3;
    if(workarray_.existFullCov){
      for (j = 1; j <= lim; ++j) {          /* COLUMN */
	 for (i = 1; i <= j; ++i) {        /* ROW */
	    errmtx[ii] = ader_ref(i, j);
	    ++ii;
	 }
      }
      return 0;
    }else{
      return -1;
    }
}


/* ------------------------------------------------------- */
/*  RETURN ERROR OF ANY VARIABLE AFTER THE FIT */

void cferrany_(long int *ntrk, double  *deriv, double  *covar)
{
    (*covar) = 0.;

    if (deriv==0) return;
    --deriv;

    int lim, ic, jc;

    lim = ((*ntrk) + 1) * 3;
    for (ic = 1; ic <= lim; ++ic) {
	for (jc = 1; jc <= lim; ++jc) {
	    (*covar) += deriv[ic] * ader_ref(ic, jc) * deriv[jc];
	}
    }
} 

#undef ader_ref







/* ---------------------------------------------------------- */
/* Entry for error matrix calculation after successful fit    */
/* Error matrix has a form V(X,Y,Z,PX,PY,PZ)                  */
/* ADER - full covariance matrix after fit in form            */
/*        (x,y,z,track1(1:3),track2(1:3),......)              */

#define ader_ref(a_1,a_2) ader[(a_2)*(NTrkM*3+3) + (a_1) - (NTrkM*3+4)]
#define verr_ref(a_1,a_2) verr[(a_2)*6 + (a_1) - 7]
#define dcv_ref(a_1,a_2)  dcv[(a_2)*6 + (a_1) - 7]


#define useWeightScheme 1

int getFullVrtCov(VKVertex * vk, double *ader, double *dcv, double *verr)
{
    extern void scaleg(double *, double *, long int  ,long int );

    int i,j,ic1,ic2;

    long int ic, jc, it, jt;
    double  cnt = 1e8;


    extern void dsinv(long int *, double  *, long int , long int *);
    extern void FullMTXfill( VKVertex* , double *);
    extern void vkSVDCmp(double**, int, int, double*, double**);

    TWRK    * t_trk=0;
    long int NTRK = vk->TrackList.size();
    long int IERR=0;
    long int NVar = (NTRK + 1) * 3;
    if(vk->passNearVertex) {     /*  Fit is with "pass near" constraint and then */
                                 /*     matrix is already present                */
    } else if ( vk->ConstraintList.size()>0  && useWeightScheme ) {
/*  Full matrix inversion i */
//
        FullMTXfill( vk, ader);
        Vect3DF th0t,tf0t;
        for(ic1=0; ic1<(int)vk->ConstraintList.size();ic1++){
          for(ic2=0; ic2<vk->ConstraintList[ic1]->NCDim; ic2++){
            th0t = vk->ConstraintList[ic1]->h0t[ic2];
            ader_ref(1, 1) += cnt * th0t.X * th0t.X;
            ader_ref(2, 1) += cnt * th0t.Y * th0t.X;
            ader_ref(3, 1) += cnt * th0t.Z * th0t.X;
            ader_ref(1, 2) += cnt * th0t.X * th0t.Y;
            ader_ref(2, 2) += cnt * th0t.Y * th0t.Y;
            ader_ref(3, 2) += cnt * th0t.Z * th0t.Y;
            ader_ref(1, 3) += cnt * th0t.X * th0t.Z;
            ader_ref(2, 3) += cnt * th0t.Y * th0t.Z;
            ader_ref(3, 3) += cnt * th0t.Z * th0t.Z;
	    for (it = 1; it <= NTRK; ++it) {
                tf0t = vk->ConstraintList[ic1]->f0t[it-1][ic2];
		ader_ref(1, it * 3 + 1) += cnt * th0t.X * tf0t.X;
		ader_ref(2, it * 3 + 1) += cnt * th0t.Y * tf0t.X;
		ader_ref(3, it * 3 + 1) += cnt * th0t.Z * tf0t.X;
		ader_ref(1, it * 3 + 2) += cnt * th0t.X * tf0t.Y;
		ader_ref(2, it * 3 + 2) += cnt * th0t.Y * tf0t.Y;
		ader_ref(3, it * 3 + 2) += cnt * th0t.Z * tf0t.Y;
		ader_ref(1, it * 3 + 3) += cnt * th0t.X * tf0t.Z;
		ader_ref(2, it * 3 + 3) += cnt * th0t.Y * tf0t.Z;
		ader_ref(3, it * 3 + 3) += cnt * th0t.Z * tf0t.Z;
	    }
	  }
	}


        for(ic1=0; ic1<(int)vk->ConstraintList.size();ic1++){
          for(ic2=0; ic2<vk->ConstraintList[ic1]->NCDim; ic2++){
	    for (it = 1; it <= NTRK; ++it) {
	      for (jt = it; jt <= NTRK; ++jt) {
                Vect3DF tf0ti = vk->ConstraintList[ic1]->f0t[it-1][ic2];
                Vect3DF tf0tj = vk->ConstraintList[ic1]->f0t[jt-1][ic2];
	        ader_ref(it*3 + 1, jt*3 + 1) += cnt * tf0ti.X * tf0tj.X;
	        ader_ref(it*3 + 2, jt*3 + 1) += cnt * tf0ti.Y * tf0tj.X;
	        ader_ref(it*3 + 3, jt*3 + 1) += cnt * tf0ti.Z * tf0tj.X;
	        ader_ref(it*3 + 1, jt*3 + 2) += cnt * tf0ti.X * tf0tj.Y;
	        ader_ref(it*3 + 2, jt*3 + 2) += cnt * tf0ti.Y * tf0tj.Y;
	        ader_ref(it*3 + 3, jt*3 + 2) += cnt * tf0ti.Z * tf0tj.Y;
	        ader_ref(it*3 + 1, jt*3 + 3) += cnt * tf0ti.X * tf0tj.Z;
	        ader_ref(it*3 + 2, jt*3 + 3) += cnt * tf0ti.Y * tf0tj.Z;
	        ader_ref(it*3 + 3, jt*3 + 3) += cnt * tf0ti.Z * tf0tj.Z;
	      }
	    }	    
	  }
	}
/* symmetrisation */
	for (i=1; i<=NVar-1; ++i) {
	    for (j = i+1; j<=NVar; ++j) {
		ader_ref(j,i) = ader_ref(i,j);
	    }
	}
//-------------------------------------------------------------------------
/* several checks for debugging */
//std::cout.precision(12);
//        for(ic1=0; ic1<(int)vk->ConstraintList.size();ic1++){
//          for(ic2=0; ic2<vk->ConstraintList[ic1]->NCDim; ic2++){
//            th0t = vk->ConstraintList[ic1]->h0t[ic2];
//std::cout<<"h0t="<<th0t.X<<", "<<th0t.Y<<", "<<th0t.Z<<'\n';
//	    for (it = 1; it <= NTRK; ++it) {
//                tf0t = vk->ConstraintList[ic1]->f0t[it-1][ic2];
//std::cout<<"f0t="<<tf0t.X<<", "<<tf0t.Y<<", "<<tf0t.Z<<'\n';
//        } } }
//if(NTRK==2){
//  for(i=1; i<=NVar; i++){std::cout<<" newmtx=";
//    for(j=1; j<=NVar; j++)std::cout<<ader_ref(j,i)<<", "; std::cout<<'\n';}
//}
//-------------------------------------------------------------------------
/* weight matrix ready.Invert */
        double * Scale=new double[NVar]; scaleg(ader, Scale, NVar, NTrkM*3+3);                  // Balance matrix
        double **ta = new double*[NVar+1]; for(i=0; i<NVar+1; i++) ta[i] = new double[NVar+1];  // Make a copy 
 	for (i=1; i<=NVar; ++i) for (j = i; j<=NVar; ++j) ta[i][j] = ta[j][i] = ader_ref(i,j);  // for failure treatment
	dsinv(&NVar, ader, NTrkM*3+3, &IERR);
	if ( IERR != 0) {
          double **tv = new double*[NVar+1]; for(i=0; i<NVar+1; i++) tv[i] = new double[NVar+1];
          double **tr = new double*[NVar+1]; for(i=0; i<NVar+1; i++) tr[i] = new double[NVar+1];
          double  *tw = new double[NVar+1];
          vkSVDCmp( ta, NVar, NVar, tw, tv);
          double tmax=0; 
          for(i=1; i<NVar+1; i++) if(fabs(tw[i])>tmax)tmax=fabs(tw[i]); 
          //for(i=1; i<NVar+1; i++) std::cout<<tw[i]<<"; "; std::cout<<'\n'; 
          for(i=1; i<NVar+1; i++) if(fabs(tw[i])/tmax < 1.e-15) tw[i]=0.;
          for(i=1; i<=NVar; i++){ for(j=1; j<=NVar; j++){
            tr[i][j]=0.; for(int k=1; k<=NVar; k++)  if(tw[k]!=0.) tr[i][j] += ta[i][k]*tv[j][k]/tw[k];
          }} 
	  for (i=1; i<=NVar; ++i) for (j=1; j<=NVar; ++j) ader_ref(i,j)=tr[i][j];
          for(i=0; i<NVar+1; i++) {delete[] tv[i];delete[] tr[i];} 
	  delete[] tv; delete[] tr; delete[] tw;	
	  IERR=0; //return IERR;
        }
 	for (i=1; i<=NVar; ++i) for (j = 1; j<=NVar; ++j) ader_ref(i,j)*=Scale[i-1]*Scale[j-1];
        delete[] Scale; //Restore scale
        for(i=0; i<NVar+1; i++) delete[] ta[i];
        delete[] ta;                 //Clean memory
/* ---------------------------------------- */
    } else {
/* ---------------------------------------- */
/* Simple and fast without constraints */
	for (i=1; i<=NVar; i++) {
	    for (j=1; j<=NVar; j++) {
		ader_ref(i,j)=0.;
	    }
	}
        double vcov[6]={vk->fitVcov[0],vk->fitVcov[1],vk->fitVcov[2],vk->fitVcov[3],vk->fitVcov[4],vk->fitVcov[5]};
	ader_ref(1,1) = vcov[0];
	ader_ref(1,2) = vcov[1];
	ader_ref(2,2) = vcov[2];
	ader_ref(1,3) = vcov[3];
	ader_ref(2,3) = vcov[4];
	ader_ref(3,3) = vcov[5];
	ader_ref(2,1) = ader_ref(1,2);
	ader_ref(3,1) = ader_ref(1,3);
	ader_ref(3,2) = ader_ref(2,3);

	for (it=1; it<=NTRK; it++) {
            t_trk=vk->tmpArr[it-1];
	    ader_ref(1, it*3 + 1) = -vcov[0] * t_trk->wbci[0] 
	                           - vcov[1] * t_trk->wbci[1] 
				   - vcov[3] * t_trk->wbci[2];
	    ader_ref(2, it*3 + 1) = -vcov[1] * t_trk->wbci[0] 
	                           - vcov[2] * t_trk->wbci[1] 
				   - vcov[4] * t_trk->wbci[2];
	    ader_ref(3, it*3 + 1) = -vcov[3] * t_trk->wbci[0] 
	                           - vcov[4] * t_trk->wbci[1] 
				   - vcov[5] * t_trk->wbci[2];
	    ader_ref(1, it*3 + 2) = -vcov[0] * t_trk->wbci[3] 
	                           - vcov[1] * t_trk->wbci[4] 
		                   - vcov[3] * t_trk->wbci[5];
	    ader_ref(2, it*3 + 2) = -vcov[1] * t_trk->wbci[3] 
	                           - vcov[2] * t_trk->wbci[4] 
				   - vcov[4] * t_trk->wbci[5];
	    ader_ref(3, it*3 + 2) = -vcov[3] * t_trk->wbci[3] 
	                           - vcov[4] * t_trk->wbci[4] 
				   - vcov[5] * t_trk->wbci[5];
	    ader_ref(1, it*3 + 3) = -vcov[0] * t_trk->wbci[6] 
	                           - vcov[1] * t_trk->wbci[7] 
				   - vcov[3] * t_trk->wbci[8];
	    ader_ref(2, it*3 + 3) = -vcov[1] * t_trk->wbci[6] 
	                           - vcov[2] * t_trk->wbci[7] 
				   - vcov[4] * t_trk->wbci[8];
	    ader_ref(3, it*3 + 3) = -vcov[3] * t_trk->wbci[6]  
		                   - vcov[4] * t_trk->wbci[7] 
				   - vcov[5] * t_trk->wbci[8];
	    ader_ref(it*3 + 1, 1) = ader_ref(1, it*3 + 1);
	    ader_ref(it*3 + 1, 2) = ader_ref(2, it*3 + 1);
	    ader_ref(it*3 + 1, 3) = ader_ref(3, it*3 + 1);
	    ader_ref(it*3 + 2, 1) = ader_ref(1, it*3 + 2);
	    ader_ref(it*3 + 2, 2) = ader_ref(2, it*3 + 2);
	    ader_ref(it*3 + 2, 3) = ader_ref(3, it*3 + 2);
	    ader_ref(it*3 + 3, 1) = ader_ref(1, it*3 + 3);
	    ader_ref(it*3 + 3, 2) = ader_ref(2, it*3 + 3);
	    ader_ref(it*3 + 3, 3) = ader_ref(3, it*3 + 3);
	}


	for (it = 1; it<=NTRK; ++it) {
            t_trk=vk->tmpArr[it-1];
	    for (jt=1; jt<=NTRK; ++jt) {
	        int j3   = jt*3;
	        int i3   = it*3;
		ader_ref( i3+1, j3+1) = -t_trk->wbci[0]*ader_ref(1, j3+1) - t_trk->wbci[1]*ader_ref(2, j3+1) - t_trk->wbci[2]*ader_ref(3, j3+1);
		ader_ref( i3+2, j3+1) = -t_trk->wbci[3]*ader_ref(1, j3+1) - t_trk->wbci[4]*ader_ref(2, j3+1) - t_trk->wbci[5]*ader_ref(3, j3+1);
		ader_ref( i3+3, j3+1) = -t_trk->wbci[6]*ader_ref(1, j3+1) - t_trk->wbci[7]*ader_ref(2, j3+1) - t_trk->wbci[8]*ader_ref(3, j3+1);
		ader_ref( i3+1, j3+2) = -t_trk->wbci[0]*ader_ref(1, j3+2) - t_trk->wbci[1]*ader_ref(2, j3+2) - t_trk->wbci[2]*ader_ref(3, j3+2);
		ader_ref( i3+2, j3+2) = -t_trk->wbci[3]*ader_ref(1, j3+2) - t_trk->wbci[4]*ader_ref(2, j3+2) - t_trk->wbci[5]*ader_ref(3, j3+2);
		ader_ref( i3+3, j3+2) = -t_trk->wbci[6]*ader_ref(1, j3+2) - t_trk->wbci[7]*ader_ref(2, j3+2) - t_trk->wbci[8]*ader_ref(3, j3+2);
		ader_ref( i3+1, j3+3) = -t_trk->wbci[0]*ader_ref(1, j3+3) - t_trk->wbci[1]*ader_ref(2, j3+3) - t_trk->wbci[2]*ader_ref(3, j3+3);
		ader_ref( i3+2, j3+3) = -t_trk->wbci[3]*ader_ref(1, j3+3) - t_trk->wbci[4]*ader_ref(2, j3+3) - t_trk->wbci[5]*ader_ref(3, j3+3);
		ader_ref( i3+3, j3+3) = -t_trk->wbci[6]*ader_ref(1, j3+3) - t_trk->wbci[7]*ader_ref(2, j3+3) - t_trk->wbci[8]*ader_ref(3, j3+3);
		if (it == jt) {
		    ader_ref( i3+1, i3+1) += t_trk->wci[0];
		    ader_ref( i3+1, i3+2) += t_trk->wci[1];
		    ader_ref( i3+2, i3+1) += t_trk->wci[1];
		    ader_ref( i3+2, i3+2) += t_trk->wci[2];
		    ader_ref( i3+1, i3+3) += t_trk->wci[3];
		    ader_ref( i3+3, i3+1) += t_trk->wci[3];
		    ader_ref( i3+2, i3+3) += t_trk->wci[4];
		    ader_ref( i3+3, i3+2) += t_trk->wci[4];
		    ader_ref( i3+3, i3+3) += t_trk->wci[5];
		}
	    }
	}
//for(int ii=1; ii<=9; ii++)std::cout<<ader_ref(ii,ii)<<", "; std::cout<<" fast full m NEW"<<'\n';        
        if( vk->ConstraintList.size()>0  && !useWeightScheme ){
//---------------------------------------------------------------------
// Covariance matrix with constraints a la Avery.
// ader_ref() should contain nonconstraint covariance matrix
//---------------------------------------------------------------------
          long int totNC=0;  //total number of constraints
          std::vector<std::vector< Vect3DF> > tf0t;  // derivative collectors
          std::vector< Vect3DF >              th0t;  // derivative collectors
          std::vector< double >               taa;   // derivative collectors
          std::vector< Vect3DF > tmpVec;
          for(int ii=0; ii<(int)vk->ConstraintList.size();ii++){
             totNC += vk->ConstraintList[ii]->NCDim;
             for(ic=0; ic<(int)vk->ConstraintList[ii]->NCDim; ic++){
               taa.push_back(  vk->ConstraintList[ii]->aa[ic] );
               th0t.push_back( vk->ConstraintList[ii]->h0t[ic] );
               tmpVec.clear();
               for(it=0; it<(int)vk->ConstraintList[ii]->f0t.size(); it++){
	         tmpVec.push_back( vk->ConstraintList[ii]->f0t[it][ic] );
              }
	      tf0t.push_back( tmpVec );
            }
          }
// R,RC[ic][i]
	  double **R =new double*[totNC]; for(ic=0; ic<totNC; ic++) R[ic]=new double[NVar]; 
	  double **RC=new double*[totNC]; for(ic=0; ic<totNC; ic++)RC[ic]=new double[NVar]; 
	  double *RCRt=new double[totNC*totNC];
	  for(ic=0; ic<totNC; ic++){
	    R[ic][0]=th0t[ic].X;
	    R[ic][1]=th0t[ic].Y;
	    R[ic][2]=th0t[ic].Z;
	    for(it=1; it<=NTRK; it++){
	      R[ic][it*3+0]=tf0t[ic][it-1].X;
	      R[ic][it*3+1]=tf0t[ic][it-1].Y;
	      R[ic][it*3+2]=tf0t[ic][it-1].Z;
            }
	  }
// R*Cov matrix
          for(ic=0; ic<totNC; ic++){
	    for(j=0; j<NVar; j++){ RC[ic][j]=0;
	      for(i=0; i<NVar; i++) RC[ic][j] += R[ic][i]*ader_ref(i+1,j+1);
            }
          }
// R*Cov*Rt matrix        -  Lagrange multiplyers errors
          for(ic=0; ic<totNC; ic++){
            for(jc=0; jc<totNC; jc++){  RCRt[ic*totNC + jc] =0.;
	      for(i=0; i<NVar; i++) RCRt[ic*totNC + jc] += RC[ic][i]*R[jc][i];
	    }
          }
	  dsinv(&totNC, RCRt, totNC, &IERR);
	  if ( IERR != 0) return IERR;
// Correction matrix
	 for(i=0; i<NVar; i++){
	   for(j=0; j<NVar; j++){  double COR=0.;
             for(ic=0; ic<totNC; ic++){
               for(jc=0; jc<totNC; jc++){
	         COR += RC[ic][i]*RC[jc][j]*RCRt[ic*totNC +jc];
	       }
	     }
	     ader_ref(i+1, j+1) -= COR;
	   }
	 }
// Delete temporary matrices
         for(ic=0; ic<totNC; ic++) delete[]  R[ic];
         delete[] R;
         for(ic=0; ic<totNC; ic++) delete[] RC[ic];
         delete[] RC;
	 delete[] RCRt;
//for(int ii=1; ii<=9; ii++)std::cout<<ader_ref(ii,ii)<<", "; std::cout<<" avery full m NEW"<<'\n';        
       }  //end of Avery matrix



    }  // End of global IF() for matrix type selection
    
//if(NTRK==2){
//  for(i=1; i<=NVar; i++){std::cout<<" new covfull=";
//    for(j=1; j<=NVar; j++)std::cout<<ader_ref(j,i)<<", "; std::cout<<'\n';}
//}

/* --Conversion to (X,Y,Z,Px,Py,Pz) form */
    for (i = 1; i <= 6; ++i) {
	for (j = 1; j <= 6; ++j) {
	    verr_ref(i,j) = 0.;
	    for (ic=1; ic<=NVar; ++ic) {
	        if(dcv_ref(i, ic)==0.) continue;
		for (jc=1; jc<=NVar; ++jc) {
	            if(dcv_ref(j, jc)==0.) continue;
		    verr_ref(i, j) += dcv_ref(i, ic) * ader_ref(ic, jc) * dcv_ref(j, jc);
		}
	    }
	}
    }
//for(int ii=1; ii<=6; ii++)std::cout<<verr_ref(ii,ii)<<", "; std::cout<<" final m NEW"<<'\n';        
    workarray_.existFullCov = 1;
    return 0;
} 
#undef dcv_ref
#undef verr_ref
#undef ader_ref

#undef useWeightScheme

} /* End of VKalVrtCore namespace*/

