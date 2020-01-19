/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <iostream>
#include "TrkVKalVrtCore/ForCFT.h"
#include "TrkVKalVrtCore/Derivt.h"
#include "TrkVKalVrtCore/WorkArray.h"
#include "TrkVKalVrtCore/CommonPars.h"
#include "TrkVKalVrtCore/TrkVKalVrtCore.h"

namespace Trk {


extern WorkArray workarray_;
extern ForCFT forcft_;
extern DerivT derivt_;


#define workarray_1 workarray_
#define forcft_1 forcft_
#define derivt_1 derivt_

/* ************************************************************************/
/*                                                                        */
/* ************************************************************************/
/*  Created     :  1-AUG-1994   Author : V.KOSTYUKHIN (IHEP) */
/*  Original version of vertex fit : PXFVTX by P. Billoir (CDF) */
/* ********* */
/*  Arguments   : Input  : NTRK             :  number of tracks */
/* 			   ICH(1:NTRK)      :  charge */
/*                         PAR(1:5,1:NTRK)  :  "perigee" parameters */
/*                          1 : epsilon (impact par. in xy projection, with sign)*/
/*                          2 : z coordinate */
/*                          3 : theta angle */
/*                          4 : phi angle */
/*                          5 : 1/R (R = radius of curvature, with sign) */
/*                                   0.0029979246d0/Pt  if charge=0 */
/*                         WGT(1:15,1:NTRK) :  weight matrix on PAR */
/*                                   :in case of zero charge WGT(15) must be */
/*                                   :error of 0.0029979246d0/Pt with correlation*/
/*                         XYZ(1:3)  :  approximate coordinates of the vertex*/
/*                         PAR0(1:3) :  approximate parameters of tracks at*/
/*                                      the vertex(result of pxfvtx fit) */
/*                Output : XYZF(1:3) :  fitted coordinates of the vertex */
/*                         PARF(1:3,1:NTRK) : fitted parameters at THE vertex*/
/*                          1 : theta */
/*                          2 : phi */
/*                          3 : 1/R */
/*                         CHI2      :  chi2 of the fit */
/*                         CHI2TR(I)=CONTRIBUTION OF TRACK I TO THE */
/*                                      TOTAL CHI2 */

/*  Errors      :  IERR = 1 :  too many tracks (NTRK > NTRMAX) */
/*                 IERR = 2 :  covariance or weight matrix not positive */
/**************************************************************************/



  double calcChi2Addition(VKVertex * vk, double wgtvrtd[6], double xyztmp[3]){

    extern double cfVrtDstSig( VKVertex * vk, bool );

    double Chi2t=0., signif;
    if ( vk->useApriorVertex) {
	    double d1 = vk->apriorV[0] - xyztmp[0];
	    double d2 = vk->apriorV[1] - xyztmp[1];
	    double d3 = vk->apriorV[2] - xyztmp[2];
	    Chi2t  +=  wgtvrtd[0]*d1*d1 + wgtvrtd[2]*d2*d2 + wgtvrtd[5]*d3*d3 
	          + 2.*(d2*d1*wgtvrtd[1] + d3*(d1*wgtvrtd[3] + d2*wgtvrtd[4]));
    }
    if (vk->passNearVertex){
           signif = cfVrtDstSig( vk, vk->passWithTrkCov);
	   Chi2t += signif*signif;
    }
    return Chi2t;
  }



      long int vtcfit( VKVertex * vk)
{
    

    double chi2i, dxyz[3], xyzt[3], eigv[8];
    double dpipj[3][3]	/* was [3][3] */;
    double drdpy[2][3]	/* was [2][3] */;
    double drdvy[2][3]	/* was [2][3] */;
    double cosf, sinf, cotth;
    double pyv[3][3] /*was [3][3]*/;
    double vyv[3][3] /*was [3][3]*/;


    double d11, d12, d21, d22, d41, d42;
    double e12, e13, e21, e22, e23, e43, wa[6];
    double tv[3], uu, vv;

    long int ic, jj, it, jt, ii, kt;
    long int  i__, j, k, l;

    extern void dsinv(long int *, double *, long int , long int *);
    //extern void digx(double *, double *, double *, long int , long int );
    extern void vkGetEigVal(double ci[], double d[], int n);
    extern int cfdinv(double *, double *, long int);
    extern void makePostFit(VKVertex *, double [], double);
    extern int vtcfitc( VKVertex * );
    extern void FullMTXfill( VKVertex* , double *);
    extern int FullMCNSTfill(VKVertex*, double*, double *);

 
    double  dw11, dw12, dw13, dw14, dw15, dw21, dw22, dw23, dw24, dw25,
	    dw31, dw32, dw33, dw34, dw35, ew11, ew12, ew13, ew14, ew15, 
	    ew21, ew22, ew23, ew24, ew25, ew31, ew32, ew33, ew34, ew35;
    double  stv[3], wgtvrtd[6];



#define ader_ref(a_1,a_2) workarray_1.ader[(a_2)*(NTrkM*3+3) + (a_1) - (NTrkM*3+4)]
#define cvder_ref(a_1,a_2) vk->FVC.cvder[(a_2)*2 + (a_1) - 3]
#define dcv_ref(a_1,a_2) vk->FVC.dcv[(a_2)*6 + (a_1) - 7]

/* ************************************************************************/
/*  Created     :  14-MAY-2008   Author : V.KOSTYUKHIN (INFN) */
/* ********* */
/*  Arguments   : Input  : NTRK             :  number of tracks */
/* 			   ICH(1:NTRK)      :  charge */
/*                         PAR(1:5,1:NTRK)  :  "perigee" parameters */
/*                          1 : epsilon (impact par. in xy projection, with sign)*/
/*                          2 : z coordinate */
/*                          3 : theta angle */
/*                          4 : phi angle */
/*                          5 : 1/R (R = radius of curvature, with sign) */
/*                                   0.0029979246d0/Pt  if charge=0 */
/*                         WGT(1:15,1:NTRK) :  weight matrix on PAR */
/*                                   :in case of zero charge WGT(15) must be */
/*                                   :error of 0.0029979246d0/Pt with correlation*/
/*                         XYZ(1:3)  :  approximate coordinates of the vertex*/
/*                         PAR0(1:3) :  approximate parameters of tracks at*/
/*                                      the vertex(result of pxfvtx fit) */
/*                Output : XYZF(1:3) :  fitted coordinates of the vertex */
/*                         PARF(1:3,1:NTRK) : fitted parameters at THE vertex*/
/*                          1 : theta */
/*                          2 : phi */
/*                          3 : 1/R */
/*                         CHI2      :  chi2 of the fit */
/*                         CHI2TR(I)=CONTRIBUTION OF TRACK I TO THE */
/*                                      TOTAL CHI2 */

/*  Errors      :  IERR = 1 :  too many tracks (NTRK > NTRMAX) */
/*                 IERR = 2 :  covariance or weight matrix not positive */
/**************************************************************************/

/* -------------------------------------------- */
/* Fit results needed in other calculations     */
/* -------------------------------------------- */
/* For close to vertex constraint */
/* For step constraint */
/* -- Derivatives and others for constraint */
/* ---------------------------------------------------------------------- */

    long int IERR = 0;
    long int NTRK = vk->TrackList.size();
    double xyz[3]={vk->iniV[0],vk->iniV[1],vk->iniV[2]};
    double twb[9];
    double twci[6];
    double twbci[9];
    double xyzf[3];

    if ( NTRK > NTrkM ) return 1;

// Dynamic arrays are created already in CFit
    double *dphi 	= workarray_1.myWorkArrays->get_dphi();
    double *deps	= workarray_1.myWorkArrays->get_deps();
    double *drho  	= workarray_1.myWorkArrays->get_drho();
    double *dtet 	= workarray_1.myWorkArrays->get_dtet();
    double *dzp 	= workarray_1.myWorkArrays->get_dzp();

    double phip,zp,eps;

    
    for (int i=0; i<3; ++i) { tv[i] = 0.; stv[i] = 0.;}
    for (int i=0; i<6; ++i) { vk->wa[i] = 0.; wa[i] = 0.;}
    for (k=0; k<2; k++) { for(j=0; j<3; j++) drdvy[k][j] = drdpy[k][j] = 0.;};
    for (k=0; k<3; k++) { for(j=0; j<3; j++)   pyv[k][j] =   vyv[k][j] = 0.;};

/* =================================================================== */
/*   loop over the tracks */

    if (vk->passNearVertex) { 
                                /* dR/dV*Y and dR/dV*Y*dR/dV f */
	drdvy[0][0]= cvder_ref(1, 1)*vk->FVC.ywgt[0] + cvder_ref(2, 1)*vk->FVC.ywgt[1];
	drdvy[1][0]= cvder_ref(1, 1)*vk->FVC.ywgt[1] + cvder_ref(2, 1)*vk->FVC.ywgt[2];
	drdvy[0][1]= cvder_ref(1, 2)*vk->FVC.ywgt[0] + cvder_ref(2, 2)*vk->FVC.ywgt[1];
	drdvy[1][1]= cvder_ref(1, 2)*vk->FVC.ywgt[1] + cvder_ref(2, 2)*vk->FVC.ywgt[2];
	drdvy[0][2]= cvder_ref(1, 3)*vk->FVC.ywgt[0] + cvder_ref(2, 3)*vk->FVC.ywgt[1];
	drdvy[1][2]= cvder_ref(1, 3)*vk->FVC.ywgt[1] + cvder_ref(2, 3)*vk->FVC.ywgt[2];
	for (ii = 1; ii <= 3; ++ii) {                        /* Matrix dR/dV*Y*dR/dV */
	    for (jj = 1; jj <= 3; ++jj) {
		vyv[ii-1][jj-1] = drdvy[0][ii-1]*cvder_ref(1, jj) + drdvy[1][ii-1]*cvder_ref(2, jj);
	    }
	}
    }

    chi2i = 0.;
    VKTrack * trk; TWRK * t_trk;
    for ( kt = 0; kt < NTRK; ++kt) {
       trk = vk->TrackList[kt];
       double theta_ini =trk->iniP[0];
       double phi_ini   =trk->iniP[1];
       double invR_ini  =trk->iniP[2];
/*  "perigee" parameters EPS and ZP if the trajectory goes through XYZ */
/*   and its theta,phi,1/R at perigee are equal to the values at input*/
	cotth = 1. / tan( theta_ini );    /*   theta at vertex */
	cosf = cos(phi_ini);
	sinf = sin(phi_ini);
	uu = xyz[0]*cosf + xyz[1]*sinf;
	vv = xyz[1]*cosf - xyz[0]*sinf;
	if ( trk->Charge ) {
	     eps     = -vv - uu*uu * invR_ini / 2.;
	      zp     = -uu * (1. - vv * invR_ini) * cotth;   //xyz[2] is added later to gain precision
	    phip     = -uu * invR_ini;                       //phi_ini is added later to gain precision
	} else {
	     eps     = -vv;
	      zp     = -uu * cotth;
	    phip     = 0.;
	}

/*   contribution of this track to chi2 with initial values */

	deps[kt] = trk->a0()    - eps;
	 dzp[kt] = trk->z()     - xyz[2];      // Precision
	dtet[kt] = trk->theta() - theta_ini;
	dphi[kt] = trk->phi()   - phi_ini;     // Precision
	drho[kt] = trk->invR()  - invR_ini;
	       dzp[kt] -= zp; dphi[kt] -= phip;
        zp   += xyz[2];   //To gain precision
	phip += phi_ini;  //To gain precision
        while(dphi[kt] >  M_PI)dphi[kt]-=2.*M_PI;
        while(dphi[kt] < -M_PI)dphi[kt]+=2.*M_PI;
//std::cout.precision(11);
//std::cout<<" newpar="<<deps[kt]<<", "<<dzp[kt]<<", "<<dtet[kt]<<", "<<dphi[kt]<<", "<<drho[kt]<<", "<<trk->Id<<'\n';

	chi2i +=     trk->WgtM[0] * deps[kt]*deps[kt] 
	        + 2.*trk->WgtM[1] * deps[kt]* dzp[kt] 
		+    trk->WgtM[2] *  dzp[kt]* dzp[kt];

/*   derivatives (deriv1) of perigee param. w.r.t. X,Y,Z (vertex) uu=Q, vv=R */
	d11 =  sinf             - (                 uu*cosf   *invR_ini);
	d12 = -cosf             - (                 uu*sinf   *invR_ini);
	d21 = -cosf * cotth     + (  (vv*cosf-uu*sinf)*cotth  *invR_ini);
	d22 = -sinf * cotth     + (  (vv*sinf+uu*cosf)*cotth  *invR_ini);
	//double d23 = 1.;        //VK for reference
	d41 = -cosf * invR_ini;
	d42 = -sinf * invR_ini;
	if (trk->Charge == 0) {
	    d11 =  sinf;
	    d12 = -cosf;
	    d21 = -cosf * cotth;
	    d22 = -sinf * cotth;
	    d41 = 0.;
	    d42 = 0.;
	}

/*   matrix DW = (deriv1)t * weight */
	dw11 = d11 * trk->WgtM[0] + d21 * trk->WgtM[1] + d41 * trk->WgtM[6];
	dw12 = d11 * trk->WgtM[1] + d21 * trk->WgtM[2] + d41 * trk->WgtM[7];
	dw13 = d11 * trk->WgtM[3] + d21 * trk->WgtM[4] + d41 * trk->WgtM[8];
	dw14 = d11 * trk->WgtM[6] + d21 * trk->WgtM[7] + d41 * trk->WgtM[9];
	dw15 = d11 * trk->WgtM[10]+ d21 * trk->WgtM[11]+ d41 * trk->WgtM[13];


	dw21 = d12 * trk->WgtM[0] + d22 * trk->WgtM[1] + d42 * trk->WgtM[6];
	dw22 = d12 * trk->WgtM[1] + d22 * trk->WgtM[2] + d42 * trk->WgtM[7];
	dw23 = d12 * trk->WgtM[3] + d22 * trk->WgtM[4] + d42 * trk->WgtM[8];
	dw24 = d12 * trk->WgtM[6] + d22 * trk->WgtM[7] + d42 * trk->WgtM[9];
	dw25 = d12 * trk->WgtM[10]+ d22 * trk->WgtM[11]+ d42 * trk->WgtM[13];
	dw31 = trk->WgtM[1];
	dw32 = trk->WgtM[2];
	dw33 = trk->WgtM[4];
	dw34 = trk->WgtM[7];
	dw35 = trk->WgtM[11];

/*  summation of  DW * DPAR  to vector TV */
	tv[0]  +=  dw11 * deps[kt] + dw12 * dzp[kt];
	tv[1]  +=  dw21 * deps[kt] + dw22 * dzp[kt];
	tv[2]  +=  dw31 * deps[kt] + dw32 * dzp[kt];
	tv[0]  +=  dw13 * dtet[kt] + dw14 * dphi[kt] + dw15 * drho[kt];
	tv[1]  +=  dw23 * dtet[kt] + dw24 * dphi[kt] + dw25 * drho[kt];
	tv[2]  +=  dw33 * dtet[kt] + dw34 * dphi[kt] + dw35 * drho[kt];

	stv[0] +=  dw11 * deps[kt] + dw12 * dzp[kt];
	stv[1] +=  dw21 * deps[kt] + dw22 * dzp[kt];
	stv[2] +=  dw31 * deps[kt] + dw32 * dzp[kt];
	stv[0] +=  dw13 * dtet[kt] + dw14 * dphi[kt] + dw15*drho[kt];
	stv[1] +=  dw23 * dtet[kt] + dw24 * dphi[kt] + dw25*drho[kt];
	stv[2] +=  dw33 * dtet[kt] + dw34 * dphi[kt] + dw35*drho[kt]; 

/*   derivatives (deriv2) of perigee param. w.r.t. theta,phi,1/R (vertex) uu=Q, vv=R */
	e12 =  uu - invR_ini * vv * uu;
	e13 = -uu*uu / 2.;
	e21 =  uu *(1. - vv*invR_ini) * (cotth*cotth + 1.);
	e22 = -vv*cotth  + (vv*vv-uu*uu)*invR_ini*cotth;
	e23 =  uu*vv*cotth;
	e43 = -uu + 2.*uu*vv*invR_ini;
/*  if straight line, set to zero derivatives w.r.t. the curvature */
/*  and curvature terms in derivatives */
	if (trk->Charge == 0) {
	    e12 =  uu;
	    e13 =  0.;
	    e21 =  uu * (cotth*cotth + 1.);
	    e22 = -vv * cotth;
	    e23 =  0.;
	    e43 =  0.;
	}

/*   matrix EW = (deriv2)t * weight */
	ew11 = e21 * trk->WgtM[1] + trk->WgtM[3];
	ew12 = e21 * trk->WgtM[2] + trk->WgtM[4];
	ew13 = e21 * trk->WgtM[4] + trk->WgtM[5];
	ew14 = e21 * trk->WgtM[7] + trk->WgtM[8];
	ew15 = e21 * trk->WgtM[11]+ trk->WgtM[12];
	ew21 = e12 * trk->WgtM[0] + e22 * trk->WgtM[1] + trk->WgtM[6];
	ew22 = e12 * trk->WgtM[1] + e22 * trk->WgtM[2] + trk->WgtM[7];
	ew23 = e12 * trk->WgtM[3] + e22 * trk->WgtM[4] + trk->WgtM[8];
	ew24 = e12 * trk->WgtM[6] + e22 * trk->WgtM[7] + trk->WgtM[9];
	ew25 = e12 * trk->WgtM[10]+ e22 * trk->WgtM[11]+ trk->WgtM[13];
	ew31 = e13 * trk->WgtM[0] + e23 * trk->WgtM[1] + e43 * trk->WgtM[6] + trk->WgtM[10];
	ew32 = e13 * trk->WgtM[1] + e23 * trk->WgtM[2] + e43 * trk->WgtM[7] + trk->WgtM[11];
	ew33 = e13 * trk->WgtM[3] + e23 * trk->WgtM[4] + e43 * trk->WgtM[8] + trk->WgtM[12];
	ew34 = e13 * trk->WgtM[6] + e23 * trk->WgtM[7] + e43 * trk->WgtM[9] + trk->WgtM[13];
	ew35 = e13 * trk->WgtM[10]+ e23 * trk->WgtM[11]+ e43 * trk->WgtM[13]+ trk->WgtM[14];

/*   computation of vector  TT = EW * DPAR */
        t_trk=vk->tmpArr[kt];
	t_trk->tt[0]  = ew11*deps[kt] + ew12*dzp[kt];
	t_trk->tt[1]  = ew21*deps[kt] + ew22*dzp[kt];
	t_trk->tt[2]  = ew31*deps[kt] + ew32*dzp[kt];
	t_trk->tt[0] += ew13*dtet[kt] + ew14*dphi[kt] + ew15*drho[kt];
	t_trk->tt[1] += ew23*dtet[kt] + ew24*dphi[kt] + ew25*drho[kt];
	t_trk->tt[2] += ew33*dtet[kt] + ew34*dphi[kt] + ew35*drho[kt];
	if ( vk->passNearVertex ) {
	    for (j = 1; j <= 2; ++j) {                             /* Derivatives dR/dQi */
		i__=kt+1;
	        int i3=i__*3;
		t_trk->drdp[j-1][0] = cvder_ref(j,4) * dcv_ref(4,i3 + 1) + cvder_ref(j,5) * dcv_ref(5,i3 + 1);
		t_trk->drdp[j-1][1] = cvder_ref(j,4) * dcv_ref(4,i3 + 2) + cvder_ref(j,5) * dcv_ref(5,i3 + 2);
		t_trk->drdp[j-1][2] = cvder_ref(j,4) * dcv_ref(4,i3 + 3) + cvder_ref(j,5) * dcv_ref(5,i3 + 3);
		t_trk->drdp[j-1][0] +=  cvder_ref(j, 6) * dcv_ref(6, i3 + 1);
		t_trk->drdp[j-1][1] +=  cvder_ref(j, 6) * dcv_ref(6, i3 + 2);
		t_trk->drdp[j-1][2] +=  cvder_ref(j, 6) * dcv_ref(6, i3 + 3);
	    }
/* Matrix dR/dQ */
	    drdpy[0][0] = t_trk->drdp[0][0] * vk->FVC.ywgt[0] + t_trk->drdp[1][0] * vk->FVC.ywgt[1];
	    drdpy[1][0] = t_trk->drdp[0][0] * vk->FVC.ywgt[1] + t_trk->drdp[1][0] * vk->FVC.ywgt[2];
	    drdpy[0][1] = t_trk->drdp[0][1] * vk->FVC.ywgt[0] + t_trk->drdp[1][1] * vk->FVC.ywgt[1];
	    drdpy[1][1] = t_trk->drdp[0][1] * vk->FVC.ywgt[1] + t_trk->drdp[1][1] * vk->FVC.ywgt[2];
	    drdpy[0][2] = t_trk->drdp[0][2] * vk->FVC.ywgt[0] + t_trk->drdp[1][2] * vk->FVC.ywgt[1];
	    drdpy[1][2] = t_trk->drdp[0][2] * vk->FVC.ywgt[1] + t_trk->drdp[1][2] * vk->FVC.ywgt[2];
	    t_trk->tt[0] -= drdpy[0][0]*vk->FVC.rv0[0] + drdpy[1][0]*vk->FVC.rv0[1];
	    t_trk->tt[1] -= drdpy[0][1]*vk->FVC.rv0[0] + drdpy[1][1]*vk->FVC.rv0[1];
	    t_trk->tt[2] -= drdpy[0][2]*vk->FVC.rv0[0] + drdpy[1][2]*vk->FVC.rv0[1];
	    for (ii = 1; ii <= 3; ++ii) {             /* Matrix dR/dQi*Y*dR/dV */
		for (jj = 1; jj <= 3; ++jj) {
		    pyv[jj-1][ii-1] = drdpy[0][ii-1] * cvder_ref(1, jj) + drdpy[1][ii-1] * cvder_ref(2, jj);
		}
	    }
	}

/*   summation of  (deriv1)t * weight * (deriv1)  to  matrix WA */
	wa[0] += dw11 * d11 + dw12 * d21 + dw14 * d41;
	wa[1] += dw11 * d12 + dw12 * d22 + dw14 * d42;
	wa[2] += dw21 * d12 + dw22 * d22 + dw24 * d42;
	wa[3] += dw12;
	wa[4] += dw22;
	wa[5] += dw32;
	vk->wa[0] = vk->wa[0] + dw11 * d11 + dw12 * d21 + dw14 * d41;
	vk->wa[1] = vk->wa[1] + dw11 * d12 + dw12 * d22 + dw14 * d42;
	vk->wa[2] = vk->wa[2] + dw21 * d12 + dw22 * d22 + dw24 * d42;
	vk->wa[3] += dw12;
	vk->wa[4] += dw22;
	vk->wa[5] += dw32;

/*   computation of matrix  WB = (deriv1)t * weight * (deriv2) */
	twb[0] = dw12 * e21 + dw13;
	twb[1] = dw22 * e21 + dw23;
	twb[2] = dw32 * e21 + dw33;
	twb[3] = dw11 * e12 + dw12 * e22 + dw14;
	twb[4] = dw21 * e12 + dw22 * e22 + dw24;
	twb[5] = dw31 * e12 + dw32 * e22 + dw34;
	twb[6] = dw11 * e13 + dw12 * e23 + dw14 * e43 + dw15;
	twb[7] = dw21 * e13 + dw22 * e23 + dw24 * e43 + dw25;
	twb[8] = dw31 * e13 + dw32 * e23 + dw34 * e43 + dw35;
	if ( vk->passNearVertex ) {
	    twb[0] +=  pyv[0][0];
	    twb[1] +=  pyv[1][0];
	    twb[2] +=  pyv[2][0];
	    twb[3] +=  pyv[0][1];
	    twb[4] +=  pyv[1][1];
	    twb[5] +=  pyv[2][1];
	    twb[6] +=  pyv[0][2];
	    twb[7] +=  pyv[1][2];
	    twb[8] +=  pyv[2][2];
	}

/*   computation of matrix  WC = (deriv2)t * weight * (deriv2) */
	vk->tmpArr[kt]->wc[0] = ew12 * e21 + ew13;
	vk->tmpArr[kt]->wc[1] = ew22 * e21 + ew23;
	vk->tmpArr[kt]->wc[3] = ew32 * e21 + ew33;
	vk->tmpArr[kt]->wc[2] = ew21 * e12 + ew22 * e22 + ew24;
	vk->tmpArr[kt]->wc[4] = ew31 * e12 + ew32 * e22 + ew34;
	vk->tmpArr[kt]->wc[5] = ew31 * e13 + ew32 * e23 + ew34 * e43 + ew35;

/*   computation of matrices  WCI = (WC)**(-1) and  WBCI = WB * WCI */
	IERR=cfdinv(&(vk->tmpArr[kt]->wc[0]), twci, -3);
	if (IERR) {IERR=cfdinv(&(vk->tmpArr[kt]->wc[0]), twci, 3);}
	if (IERR) 		return IERR;


	twbci[0] = twb[0]*twci[0] + twb[3]*twci[1] + twb[6]*twci[3];
	twbci[1] = twb[1]*twci[0] + twb[4]*twci[1] + twb[7]*twci[3];
	twbci[2] = twb[2]*twci[0] + twb[5]*twci[1] + twb[8]*twci[3];
	twbci[3] = twb[0]*twci[1] + twb[3]*twci[2] + twb[6]*twci[4];
	twbci[4] = twb[1]*twci[1] + twb[4]*twci[2] + twb[7]*twci[4];
	twbci[5] = twb[2]*twci[1] + twb[5]*twci[2] + twb[8]*twci[4];
	twbci[6] = twb[0]*twci[3] + twb[3]*twci[4] + twb[6]*twci[5];
	twbci[7] = twb[1]*twci[3] + twb[4]*twci[4] + twb[7]*twci[5];
	twbci[8] = twb[2]*twci[3] + twb[5]*twci[4] + twb[8]*twci[5];

/*   subtraction of  WBCI * (WB)t  from matrix WA */
	wa[0] = wa[0] - twbci[0]*twb[0] - twbci[3]*twb[3] - twbci[6]*twb[6];
	wa[1] = wa[1] - twbci[0]*twb[1] - twbci[3]*twb[4] - twbci[6]*twb[7];
	wa[2] = wa[2] - twbci[1]*twb[1] - twbci[4]*twb[4] - twbci[7]*twb[7];
	wa[3] = wa[3] - twbci[0]*twb[2] - twbci[3]*twb[5] - twbci[6]*twb[8];
	wa[4] = wa[4] - twbci[1]*twb[2] - twbci[4]*twb[5] - twbci[7]*twb[8];
	wa[5] = wa[5] - twbci[2]*twb[2] - twbci[5]*twb[5] - twbci[8]*twb[8];

/*   subtraction of  WBCI * TT  from vector  TV */
	tv[0] = tv[0] - twbci[0] * vk->tmpArr[kt]->tt[0] - twbci[3] * vk->tmpArr[kt]->tt[1] - twbci[6] * vk->tmpArr[kt]->tt[2];
	tv[1] = tv[1] - twbci[1] * vk->tmpArr[kt]->tt[0] - twbci[4] * vk->tmpArr[kt]->tt[1] - twbci[7] * vk->tmpArr[kt]->tt[2];
	tv[2] = tv[2] - twbci[2] * vk->tmpArr[kt]->tt[0] - twbci[5] * vk->tmpArr[kt]->tt[1] - twbci[8] * vk->tmpArr[kt]->tt[2];
//Save to vertex ....
        for( int tpn=0; tpn<9; tpn++){
	   vk->tmpArr[kt]->wb[tpn]   = twb[tpn];
	   if(tpn<6)vk->tmpArr[kt]->wci[tpn]  = twci[tpn];
	   vk->tmpArr[kt]->wbci[tpn] = twbci[tpn];
        }
    }
    if (vk->useApriorVertex) {
	for (ic = 0; ic < 3; ++ic) {   xyzt[ic] = vk->apriorV[ic] - vk->refIterV[ic] - xyz[ic];}
	for (ic = 0; ic < 6; ++ic) {wgtvrtd[ic] = vk->apriorVWGT[ic];}
	wa[0] += wgtvrtd[0];
	wa[1] += wgtvrtd[1];
	wa[2] += wgtvrtd[2];
	wa[3] += wgtvrtd[3];
	wa[4] += wgtvrtd[4];
	wa[5] += wgtvrtd[5];
	vk->wa[0] += wgtvrtd[0];
	vk->wa[1] += wgtvrtd[1];
	vk->wa[2] += wgtvrtd[2];
	vk->wa[3] += wgtvrtd[3];
	vk->wa[4] += wgtvrtd[4];
	vk->wa[5] += wgtvrtd[5];
	tv[0]  = tv[0]  + xyzt[0] * wgtvrtd[0] + xyzt[1] * wgtvrtd[1] + xyzt[2] * wgtvrtd[3];
	tv[1]  = tv[1]  + xyzt[0] * wgtvrtd[1] + xyzt[1] * wgtvrtd[2] + xyzt[2] * wgtvrtd[4];
	tv[2]  = tv[2]  + xyzt[0] * wgtvrtd[3] + xyzt[1] * wgtvrtd[4] + xyzt[2] * wgtvrtd[5];
	stv[0] = stv[0] + xyzt[0] * wgtvrtd[0] + xyzt[1] * wgtvrtd[1] + xyzt[2] * wgtvrtd[3];
	stv[1] = stv[1] + xyzt[0] * wgtvrtd[1] + xyzt[1] * wgtvrtd[2] + xyzt[2] * wgtvrtd[4];
	stv[2] = stv[2] + xyzt[0] * wgtvrtd[3] + xyzt[1] * wgtvrtd[4] + xyzt[2] * wgtvrtd[5];
    }
//Save T vector(see Billoir...)
    vk->T[0]=stv[0];vk->T[1]=stv[1];vk->T[2]=stv[2];

//std::cout<<" newwa="<<wa[0]<<", "<<wa[1]<<", "<<wa[2]<<", "<<wa[3]<<", "<<wa[4]<<", "<<wa[5]<<'\n';

/*   solution of the linear system */
    if ( !vk->passNearVertex ) {           /* No correction for these constraints */
	                               /* because solution is calculated with full error matrix*/

     if( vk->ConstraintList.size() == 0){
        double EigThreshold = 5.e-8;         /* Def. for fit without constraints 5.e-8*/
        vkGetEigVal(wa, eigv, 3);
        if (eigv[0] < 0.) { wa[0] -= 1.*eigv[0];  wa[2] -= 1.*eigv[0]; wa[5] -= 1.*eigv[0]; }
        if (eigv[0] < eigv[2] * EigThreshold) {
	  wa[0] += (eigv[2] * EigThreshold - eigv[0]);
	  wa[2] += (eigv[2] * EigThreshold - eigv[0]);
 	  wa[5] += (eigv[2] * EigThreshold - eigv[0]);
        }
      }else{
        vkGetEigVal(vk->wa, eigv, 3);
	double EigAddon=0.;
        wa[0] += eigv[2] * 1.e-12 -EigAddon;  //Permanent addition works better than addition
        wa[2] += eigv[2] * 1.e-12 -EigAddon;  // for small eigenvalues only.
        wa[5] += eigv[2] * 1.e-12 -EigAddon;
        vk->wa[0] += eigv[2] * 1.e-12 -EigAddon;  //Permanent addition works better than addition
        vk->wa[2] += eigv[2] * 1.e-12 -EigAddon;  // for small eigenvalues only.
        vk->wa[5] += eigv[2] * 1.e-12 -EigAddon;
      }
    }
/*   covariance matrix on vertex coordinates */
    IERR=cfdinv(wa, &(vk->fitVcov[0]), -3);
    if (IERR) {IERR=cfdinv(wa,&(vk->fitVcov[0]) , 3);}  // last safety if mechanism above doesn't work
    if ( !vk->passNearVertex ) {  if (IERR) return IERR; } // If nothing helps - detect failure (not for passNearVertex!!!)
    else {vk->fitVcov[0]=1.;vk->fitVcov[1]=0;vk->fitVcov[2]=1.;vk->fitVcov[3]=0;vk->fitVcov[4]=0;vk->fitVcov[5]=1;}

/*   corrections to vertex coordinates */
    dxyz[0] = vk->fitVcov[0]*tv[0] + vk->fitVcov[1]*tv[1] + vk->fitVcov[3]*tv[2];
    dxyz[1] = vk->fitVcov[1]*tv[0] + vk->fitVcov[2]*tv[1] + vk->fitVcov[4]*tv[2];
    dxyz[2] = vk->fitVcov[3]*tv[0] + vk->fitVcov[4]*tv[1] + vk->fitVcov[5]*tv[2];
    for (j = 0; j < 3; ++j) {
	vk->dxyz0[j] = dxyz[j];
	vk->fitV[j]  = xyzf[j] =  vk->iniV[j] + dxyz[j];
    }
    //std::cout<< "NVertex Old="<<vk->iniV[0]<<", "<<vk->iniV[1]<<", "<<vk->iniV[2]<<" CloseV="<<vk->passNearVertex<<'\n';
    //std::cout<< "NVertex shift="<<dxyz[0]<<", "<<dxyz[1]<<", "<<dxyz[2]<<'\n';






/*-----------------------------------------------------------*/
/*   corrections to track parameters and covariance matrices */

    for (kt = 0; kt < NTRK; ++kt) {                    /*   variation on PAR is  WCI * TT - (WBCI)t * DXYZ */
        for( int tpn=0; tpn<9; tpn++){
          if(tpn<6)twci[tpn]  = vk->tmpArr[kt]->wci[tpn];
          twbci[tpn] = vk->tmpArr[kt]->wbci[tpn];
        }
        double tt1=vk->tmpArr[kt]->tt[0];
        double tt2=vk->tmpArr[kt]->tt[1];
        double tt3=vk->tmpArr[kt]->tt[2];
	vk->tmpArr[kt]->parf0[0] = twci[0]*tt1 + twci[1]*tt2 + twci[3]*tt3 - twbci[0]*dxyz[0] - twbci[1]*dxyz[1] - twbci[2]*dxyz[2];
	vk->tmpArr[kt]->parf0[1] = twci[1]*tt1 + twci[2]*tt2 + twci[4]*tt3 - twbci[3]*dxyz[0] - twbci[4]*dxyz[1] - twbci[5]*dxyz[2];
	vk->tmpArr[kt]->parf0[2] = twci[3]*tt1 + twci[4]*tt2 + twci[5]*tt3 - twbci[6]*dxyz[0] - twbci[7]*dxyz[1] - twbci[8]*dxyz[2];
        if( (vk->TrackList[kt]->iniP[2] + vk->tmpArr[kt]->parf0[2]) * vk->TrackList[kt]->iniP[2] < 0 ){
	       vk->tmpArr[kt]->parf0[2]= - vk->TrackList[kt]->iniP[2]/4.; } // VK 15.12.2009 - Protection against change of sign
	vk->TrackList[kt]->fitP[0] = vk->TrackList[kt]->iniP[0] + vk->tmpArr[kt]->parf0[0];
	vk->TrackList[kt]->fitP[1] = vk->TrackList[kt]->iniP[1] + vk->tmpArr[kt]->parf0[1];
	vk->TrackList[kt]->fitP[2] = vk->TrackList[kt]->iniP[2] + vk->tmpArr[kt]->parf0[2];
        //std::cout<< "NTrack  shift="<<vk->tmpArr[kt]->parf0[0]<<", "<<vk->tmpArr[kt]->parf0[1]<<", "
	//                                                            <<vk->tmpArr[kt]->parf0[2]<<'\n';
    }
  
/* ------------------------------------------------------------ */
/*  The same solution but through full matrix */
/* ------------------------------------------------------------ */
    if ( vk->passNearVertex ) {
        //std::cout <<" Std="<<vk->dxyz0[0]<<", "<<vk->dxyz0[1]<<", "<<vk->dxyz0[2]<<'\n';
	stv[0] = stv[0] - drdvy[0][0] * vk->FVC.rv0[0] - drdvy[1][0] * vk->FVC.rv0[1];
	stv[1] = stv[1] - drdvy[0][1] * vk->FVC.rv0[0] - drdvy[1][1] * vk->FVC.rv0[1];
	stv[2] = stv[2] - drdvy[0][2] * vk->FVC.rv0[0] - drdvy[1][2] * vk->FVC.rv0[1];
//Save T vector(see Billoir...)
        vk->T[0]=stv[0];vk->T[1]=stv[1];vk->T[2]=stv[2];
//
	vk->wa[0] += vyv[0][0];
	vk->wa[1] += vyv[1][0];
	vk->wa[2] += vyv[1][1];
	vk->wa[3] += vyv[2][0];
	vk->wa[4] += vyv[2][1];
	vk->wa[5] += vyv[2][2];
	FullMTXfill(vk, workarray_1.ader);
	if ( vk->passNearVertex ) {
	  for (it = 1; it <= NTRK; ++it) {
	    drdpy[0][0] = vk->tmpArr[it-1]->drdp[0][0] * vk->FVC.ywgt[0] + vk->tmpArr[it-1]->drdp[1][0] * vk->FVC.ywgt[1];
            drdpy[1][0] = vk->tmpArr[it-1]->drdp[0][0] * vk->FVC.ywgt[1] + vk->tmpArr[it-1]->drdp[1][0] * vk->FVC.ywgt[2];
	    drdpy[0][1] = vk->tmpArr[it-1]->drdp[0][1] * vk->FVC.ywgt[0] + vk->tmpArr[it-1]->drdp[1][1] * vk->FVC.ywgt[1];
	    drdpy[1][1] = vk->tmpArr[it-1]->drdp[0][1] * vk->FVC.ywgt[1] + vk->tmpArr[it-1]->drdp[1][1] * vk->FVC.ywgt[2];
	    drdpy[0][2] = vk->tmpArr[it-1]->drdp[0][2] * vk->FVC.ywgt[0] + vk->tmpArr[it-1]->drdp[1][2] * vk->FVC.ywgt[1];
	    drdpy[1][2] = vk->tmpArr[it-1]->drdp[0][2] * vk->FVC.ywgt[1] + vk->tmpArr[it-1]->drdp[1][2] * vk->FVC.ywgt[2];
	    for (jt = 1; jt <= NTRK; ++jt) {   /* Matrix */
		    for (k = 0; k < 3; ++k) {
			for (l = 0; l < 3; ++l) {
			    dpipj[k][l] = 0.;
			    for (j = 0; j < 2; ++j) {
				dpipj[k][l] +=  vk->tmpArr[jt-1]->drdp[j][k] * drdpy[j][l];
			    }
			}
		    }
		    for (k = 1; k <= 3; ++k) {
			for (l = 1; l <= 3; ++l) {
			    ader_ref(it * 3 + k, jt * 3 + l) +=  dpipj[l-1][k-1];
			}
		    }
	    }
	  }
	}
//VK 27-Sep-2006     Add eigenvalue limitation for 7,8,9,10,13,14 constraints
        double ArrTmp[9]; int ktmp=0;
	//for (k=1; k<=3; ++k) {     for (l=1; l<=3; ++l)  { ArrTmp[(k-1)*3+l-1] = ader_ref(k,l); }}
        //digx(ArrTmp, eigv, work, 3, 0);
	for (k=1; k<=3; ++k) {     for (l=1; l<=k; ++l)  { ArrTmp[ktmp++] = ader_ref(k,l); }}
        vkGetEigVal(ArrTmp, eigv, 3);
	  double EigAddon=0.;
	  if( eigv[0]<0 ){EigAddon=eigv[0];std::cout<<" EIGENVALUE after negative="<<eigv[0]<<'\n';}
	  for (k = 1; k <=3; ++k) {   
	     ader_ref(k,k) = ader_ref(k,k) - EigAddon + eigv[2] * 1.e-18 ;    
	  }
//----------------------------------------------------------------------------------
	long int NParam = NTRK*3 + 3;
	dsinv(&NParam, workarray_1.ader, NTrkM*3+3, &IERR);
	if ( IERR != 0) {
        std::cout << " Bad problem in CFIT inversion ierr="<<IERR<<", "<<eigv[2]<<'\n'; return IERR;
	} else {
            double *fortst = new double[NTrkM*3+3];
	    for (j = 0; j < 3; ++j) {
		fortst[j] = stv[j];
		for (ii=0; ii<NTRK; ++ii) { fortst[ii*3 +3 +j] = vk->tmpArr[ii]->tt[j];}
	    }
	    for (j=0; j<3; ++j) {
		dxyz[j] = 0.;
		for (ii=0; ii<NParam; ++ii) {dxyz[j] += ader_ref(j+1, ii+1) * fortst[ii];}
		vk->dxyz0[j]  = dxyz [j];
	        vk->fitV[j] = vk->iniV[j] + dxyz[j];
	    }
	    double tparf0[3];
	    for (it = 1; it <= NTRK; ++it) {
                t_trk=vk->tmpArr[it-1];
		for (j=0; j<3; ++j) {
                   tparf0[j] = 0.;
		   for (ii = 1; ii <= NParam; ++ii) { tparf0[j] +=  ader_ref(it*3+j+1, ii)*fortst[ii-1];}
		}
//std::cout<<tparf0[0]<<", "<<tparf0[1]<<", "<<tparf0[2]<<", "<<'\n';
//std::cout<<t_trk->parf0[0]<<", "<<t_trk->parf0[1]<<", "<<t_trk->parf0[2]<<" it= "<<it<<'\n';
		for (j=0; j<3; ++j) { t_trk->parf0[j] = tparf0[j];
	                              vk->TrackList[it-1]->fitP[j] = vk->TrackList[it-1]->iniP[j] + tparf0[j];}
	    }
	    vk->fitVcov[0] = ader_ref(1, 1);
	    vk->fitVcov[1] = ader_ref(2, 1);
	    vk->fitVcov[2] = ader_ref(2, 2);
	    vk->fitVcov[3] = ader_ref(3, 1);
	    vk->fitVcov[4] = ader_ref(3, 2);
	    vk->fitVcov[5] = ader_ref(3, 3);
	    delete[]  fortst;
	}
        //std::cout<< "NVertex Full="<<vk->fitV[0]<<", "<<vk->fitV[1]<<", "<<vk->fitV[2]<<'\n';
        //std::cout<< "NVertex Full shft="<<dxyz[0]<<", "<<dxyz[1]<<", "<<dxyz[2]<<'\n';
        //double *tmpA=new double[3+3*NTRK+20];
        //IERR = FullMCNSTfill( vk, workarray_.ader, tmpA);
        //delete[] tmpA;
    }
/* ------------------------------------------------------------ */
/*           End of the solution with full matrix               */
/*								*/
/*          Now constraints 					*/
/* ------------------------------------------------------------ */
    double dCoefNorm = 0;
    if ( vk->ConstraintList.size() > 0) {
        IERR =  vtcfitc( vk );
	if (IERR != 0)   return IERR;
///* dxyz0 - shift of vertex           due to fit without constraints */
///* parf0 - shift of track parameters due to fit without constraints */
///* vecB - shift due to fit without constraints                      */
///* vecA - final shift (fit+constraints)                             */
///* vector perpendicular to vecA is vecC = vecA + (A,A)/(A,B) * vecB */ 
        double* tmpB=new double[3+3*NTRK]; double* tmpA=new double[3+3*NTRK];
	for (ii=0; ii<3; ++ii) {
	    tmpA[ii]   =  vk->fitV[ii] - vk->iniV[ii];  
	    tmpB[ii]   =  vk->dxyz0[ii];
	}
	for ( it=0; it<NTRK; ++it) {
	    tmpA[0 +3*it+3] =  vk->TrackList[it]->fitP[0] - vk->TrackList[it]->iniP[0];
	    tmpA[1 +3*it+3] =  vk->TrackList[it]->fitP[1] - vk->TrackList[it]->iniP[1];
	    tmpA[2 +3*it+3] =  vk->TrackList[it]->fitP[2] - vk->TrackList[it]->iniP[2];
	    tmpB[0 +3*it+3] =  vk->tmpArr[it]->parf0[0];
	    tmpB[1 +3*it+3] =  vk->tmpArr[it]->parf0[1];
	    tmpB[2 +3*it+3] =  vk->tmpArr[it]->parf0[2];
	}
	double scalAA=0., scalAB=0.;
        for (ii=0; ii<3+3*NTRK; ii++) scalAA += tmpA[ii]*tmpA[ii];
        for (ii=0; ii<3+3*NTRK; ii++) scalAB += tmpA[ii]*tmpB[ii];
        if(scalAB != 0.) dCoefNorm = -scalAA/scalAB;
	//double scalCC;
        //for (ii=0; ii<3+3*NTRK; ii++) scalCC += (tmpA[ii]+dCoefNorm*tmpB[ii])*(tmpA[ii]+dCoefNorm*tmpB[ii]); // Length of perp vector
	delete[] tmpA; delete[] tmpB;

    //std::cout<<"VRTOLD="<<vk->fitV[0] - vk->iniV[0]<<", "<<vk->fitV[1] - vk->iniV[1]<<", "
    //                                                     <<vk->fitV[2] - vk->iniV[2]<<'\n';
    //for ( it=0; it<NTRK; ++it) std::cout<<"TRKOLD="<<vk->TrackList[it]->fitP[0] - vk->TrackList[it]->iniP[0]<<", "
    //                                               <<vk->TrackList[it]->fitP[1] - vk->TrackList[it]->iniP[1]<<", "
    //                                               <<vk->TrackList[it]->fitP[2] - vk->TrackList[it]->iniP[2]<<'\n';
    //tmpA=new double[3+3*NTRK+20];
    //IERR = FullMCNSTfill( vk, workarray_.ader, tmpA);
    //delete[] tmpA;
   }

/*   chi2 with fitted values */

    for (it = 0; it < NTRK; ++it) {    //Check if curvature sign is changed or change in Pt is too big
      if(vk->TrackList[it]->Id >= 0){
        double Ratio=vk->TrackList[it]->fitP[2]/vk->TrackList[it]->Perig[4]; if(fabs(Ratio)<1.)Ratio=1./Ratio;
        if(Ratio<0. || Ratio > vkalAllowedPtChange ){
          if(fabs(vk->TrackList[it]->fitP[2])<fabs(vk->TrackList[it]->Perig[4]) || Ratio<0 ){	
             vk->TrackList[it]->fitP[2]=vk->TrackList[it]->Perig[4]/vkalAllowedPtChange;
          }else{
             vk->TrackList[it]->fitP[2]=vk->TrackList[it]->Perig[4]*vkalAllowedPtChange;
          }
	}
      }
    }

    makePostFit( vk , wgtvrtd, dCoefNorm);

    return 0;

} 


  extern double finter(double , double , double , double , double , double );
  extern double cfchi2(double *, double *, VKTrack *);
  extern double getCnstValues2( VKVertex * vk );
  extern void applyConstraints( VKVertex * vk );


  void makePostFit( VKVertex * vk, double wgtvrtd[], double dCoefNorm)
  {

  long int NTRK=vk->TrackList.size();
  double dScale=1.e10, dScaleMax=1.e12;
  double alfLowLim=0.1;
  double alfUppLim=1.5;
  double xyzt[3],xyztmp[3],chi2t[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  double alf=1.,bet=0.;
  long int ii,j, icadd=0;
  double ContribC[10];
//
//    Limit too big total shift
  double totalShift=sqrt(  (vk->fitV[0] - vk->iniV[0])*(vk->fitV[0] - vk->iniV[0])
                          +(vk->fitV[1] - vk->iniV[1])*(vk->fitV[1] - vk->iniV[1])
                          +(vk->fitV[2] - vk->iniV[2])*(vk->fitV[2] - vk->iniV[2]) );
  bool limitationMade=false;
  if(totalShift>vkalInternalStepLimit) {  
      limitationMade=true;                            // to exclude other convergence improvements 
      alfUppLim *=  vkalInternalStepLimit/totalShift;
      alfLowLim *=  vkalInternalStepLimit/totalShift;
  }
  
  int NCNST = vk->ConstraintList.size();
  int PostFitIteration=4; if (NCNST) PostFitIteration=7; double ValForChk;
  VKTrack * trk; TWRK * t_trk;

  do {                                           // Additional iterations start from here
    for (j = 1; j <= PostFitIteration; ++j) {
	int jm1=j-1;
	chi2t[jm1] = 0.; ContribC[jm1]=0.;
//--
	if (j < 4){ alf = jm1 * .5;  bet = 0.;}
	else{ if(j==4)bet=0.; if(j==5)bet=-0.02; if(j==6)bet= 0.02;}
//--
	for (ii=0; ii<3; ++ii) {     // Intermediate vertex. 
	    xyzt[ii]   = vk->iniV[ii] +   (alf+bet) * (vk->fitV[ii] - vk->iniV[ii])  +  bet * dCoefNorm*vk->dxyz0[ii];
	    xyztmp[ii] = vk->refIterV[ii] + xyzt[ii];  // current position in global ref.frame
	}

	for ( ii = 0; ii < NTRK; ++ii) {         // track parameters at intermediate vertex. Also save to cnstP for constraint
            trk=vk->TrackList[ii]; t_trk=vk->tmpArr[ii];
	    t_trk->part[0]=trk->cnstP[0]= trk->iniP[0] + (alf+bet)*(trk->fitP[0] - trk->iniP[0]) + bet*dCoefNorm * t_trk->parf0[0];
	    t_trk->part[1]=trk->cnstP[1]= trk->iniP[1] + (alf+bet)*(trk->fitP[1] - trk->iniP[1]) + bet*dCoefNorm * t_trk->parf0[1];
	    t_trk->part[2]=trk->cnstP[2]= trk->iniP[2] + (alf+bet)*(trk->fitP[2] - trk->iniP[2]) + bet*dCoefNorm * t_trk->parf0[2];
	    trk->Chi2 = cfchi2(xyzt, t_trk->part, trk );
	    chi2t[jm1] += trk->Chi2 ;
	}
        vk->setCnstV(xyzt); 
        chi2t[jm1] += calcChi2Addition( vk, wgtvrtd, xyztmp); // Constraints of Chi2 type
//
// Addition of constraints
	if (NCNST) {         //VK 25.10.2006 new mechanism for constraint treatment
            applyConstraints(vk);
	    double dCnstContrib=getCnstValues2(vk);
	    if(j == 1 ) { if( chi2t[0]>10.) {dScale = chi2t[0] / (dCnstContrib + 1/dScaleMax);}
	                               else {dScale =      10. / (dCnstContrib + 1/dScaleMax);}
	                  if(dScale > dScaleMax)  dScale=dScaleMax;
	                  if(dScale < 0.01)       dScale=0.01;
	    }
	    ContribC[jm1] = 25.*dCnstContrib*dScale;
	    if ( j != PostFitIteration) {           // Last cycle is ALWAYS without constraints
	      chi2t[jm1] += 25.*dCnstContrib*dScale;   // constraint contribution is 50*chi2(def)
	    }
	}
//Having 3 points (0,0.5,1.) find a pabolic minimum
	if (j == 3) {  alf = finter(chi2t[0], chi2t[1], chi2t[2], 0., 0.5, 1.);
	               if (chi2t[0] == chi2t[1] && chi2t[1] == chi2t[2])  alf = 1.;
 	               if (alf > alfUppLim )alf = alfUppLim;
		       if (alf < alfLowLim){
		          alf = alfLowLim;
			  PostFitIteration=4;  //Something is wrong. Don't make second optimisation
		       }
                       if(NCNST && alf>vkalInternalStepLimit/totalShift)
	                 { alf=vkalInternalStepLimit/totalShift; PostFitIteration=4; icadd=2; limitationMade=true; }
        }

//Having 3 points (0,-0.02,0.02) find a pabolic minimum
	if (j == 6) {  bet = finter(chi2t[4], chi2t[3], chi2t[5], -0.02, 0., 0.02);
	               if (chi2t[3] == chi2t[4] && chi2t[4] == chi2t[5])  bet = 0.;
	               if (bet >0.3)bet = 0.3;
                       if (bet <-0.3)bet = -0.3;
	}
//Check if minimum is really a minimum. Otherwise use bet=0. point
	if (j == 7 &&  ContribC[6]>ContribC[3]) { bet = 0.; j--; /*repeat step 7*/}   


      // Trick to cut step in case of divergence and wrong ALFA. Step==4!!!  MUST not work is case of total step limitation
      // Check if minimum is really a minimum. Otherwise use alf=0.02 or alf=0.5 or alf=1. points
      if(j==4 && alf!=alfLowLim && !limitationMade){ ValForChk=chi2t[3]; 
                             if((PostFitIteration==4) && NCNST) ValForChk+=ContribC[3];
	                     if(ValForChk>chi2t[0]*1.0001) { alf = alfLowLim; ValForChk=chi2t[0]; j--; /*repeat step 4*/
			                                     /*PostFitIteration=4;*/ } //Something is wrong. Don't make second optimisation
	                     if(ValForChk>chi2t[1]*1.0001) { alf = 0.5;       ValForChk=chi2t[1]; if(j==4) j--;/*repeat step 4*/}   
	                     if(ValForChk>chi2t[2]*1.0001) { alf = 1.0;       ValForChk=chi2t[2]; if(j==4) j--;/*repeat step 4*/}
      }
    }
/* -- Saving of final j=PostFitIteration step as output */
/*------------------------------------------------------*/
    for (j=0; j<3; ++j)vk->fitV[j] = xyzt[j];
    for (ii=0; ii<NTRK; ++ii){
        trk=vk->TrackList[ii]; t_trk=vk->tmpArr[ii];
        for (j=0; j<3; ++j) trk->fitP[j] = t_trk->part[j];
    }
    vk->Chi2 = chi2t[PostFitIteration-1]; // Chi2 from last try.

//    if (vk->passNearVertex == 7) {  
//    if (PostFitIteration == 7) {  
//std::cout<<" NBETA="<<ContribC[4]<<", "<<ContribC[5]<<", "<<ContribC[6]<<
//                                  " aa="<<vk->ConstraintList[0]->aa[0]<<", "<<vk->ConstraintList[0]->aa[1]<<'\n';
//std::cout<<"NDCOEF="<<chi2t[0]<<", "<<chi2t[1]<<", "<<chi2t[2]<<", "<<chi2t[3]<<", "
//                    <<chi2t[4]<<", "<<chi2t[5]<<", "<<chi2t[6]<<", "<<icadd<<", "<<alf<<", "<<bet<<'\n';
//std::cout<<"NDCOEF="<<chi2t[0]<<", "<<chi2t[1]<<", "<<chi2t[2]<<", "<<chi2t[3]<<", "<<icadd<<", "<<alf <<'\n';
//std::cout<<"CNSTCN="<<ContribC[0]<<", "<<ContribC[1]<<", "<<ContribC[2]<<", "<<ContribC[3]<<", "
//                    <<ContribC[4]<<", "<<ContribC[5]<<", "<<ContribC[6]<<" dScale="<<dScale<<'\n';
//    }

    dCoefNorm *= alf;   // Rescale normalisation according to found results
    ValForChk=vk->Chi2;  if (NCNST)ValForChk += ContribC[PostFitIteration-1];
    } while ( ( ValForChk>chi2t[0]*1.000001 || ValForChk>chi2t[1]*1.000001 || alf==alfLowLim)  && (++icadd<=2) );   // 2 additional iterations now



//--------------------------------------------------------------------------------------------------------------------
// Final check if there is any improvement. If not - stay with initial solution. Works only if constraints are present
//
//VK 22.02.2010 - this check is moved to fitVertex (5 such steps are allowed at maximum)
//                Cascade fitter doesn't use fitVertex -> check influence!!!
//
//    double Chi2Ini=chi2t[0]-ContribC[0];
//    if( (dScale==dScaleMax || ContribC[0]<ContribC[PostFitIteration-1]) && chi2t[PostFitIteration-1] > Chi2Ini ){
//      for (j=0; j<3; ++j)vk->fitV[j] = vk->iniV[j];
//      for (ii=0; ii<NTRK; ++ii){  trk=vk->TrackList[ii];
//        for (j=0; j<3; ++j) trk->fitP[j] = trk->iniP[j];
//      }
//      vk->Chi2 = Chi2Ini; // Initial Chi2
//    }
//--------------------------------------------------------------------------------------------------------
    if (NCNST) {                  /* Restore initial derivatives */
      vk->setCnstV(vk->iniV); 
      for (ii=0; ii<NTRK; ++ii){ trk=vk->TrackList[ii]; for (j=0;j<3;j++){trk->cnstP[j]=trk->iniP[j];}}
      applyConstraints(vk);
    }
    return;
} 





} /* End of VKalVrtCore namespace*/

