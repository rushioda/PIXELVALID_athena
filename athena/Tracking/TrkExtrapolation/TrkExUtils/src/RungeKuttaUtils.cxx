/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// RungeKuttaUtils.cxx , (c) ATLAS Detector software
// author Igor Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TrkExUtils/RungeKuttaUtils.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/DistanceSolution.h"
#include "TrkPatternParameters/PatternTrackParameters.h"

/////////////////////////////////////////////////////////////////////////////////
// Common transformation from local to global system coordinates for all surfaces
// for charged track parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaUtils::transformLocalToGlobal 
(bool useJac,const Trk::TrackParameters& Tp,double* P) const 
{
  const Trk::TrackParameters* pTp  = &Tp; if(!pTp) return false;

  const AmgVector(5) Vp = Tp.parameters();
  double p[5]; p[0]=Vp[0]; p[1]=Vp[1]; p[2]=Vp[2]; p[3]=Vp[3]; p[4]=Vp[4];

  return transformLocalToGlobal(useJac,&Tp.associatedSurface(),p,P);
}

/////////////////////////////////////////////////////////////////////////////////
// Common transformation from local to global system coordinates for all surfaces
// for neutral track parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaUtils::transformLocalToGlobal 
(bool useJac,const Trk::NeutralParameters& Tp,double* P) const 
{
  const Trk::NeutralParameters* pTp = &Tp; if(!pTp) return false;

  const AmgVector(5) Vp = Tp.parameters();
  double p[5]; p[0]=Vp[0]; p[1]=Vp[1]; p[2]=Vp[2]; p[3]=Vp[3]; p[4]=Vp[4];

  return transformLocalToGlobal(useJac,&Tp.associatedSurface(),p,P);
}

/////////////////////////////////////////////////////////////////////////////////
// Common transformation from local to global system coordinates for all surfaces
// for pattern parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaUtils::transformLocalToGlobal 
(bool useJac,const Trk::PatternTrackParameters& Tp,double* P) const 
{
  return transformLocalToGlobal(useJac,Tp.associatedSurface(),Tp.par(),P);
}

/////////////////////////////////////////////////////////////////////////////////
// Common transformation from global to local system coordinates for all surfaces
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToLocal 
(double* P,double* par) const 
{
  par[2]  = atan2(P[4],P[3]);
  par[3]  = acos (P[5]);
  par[4]  = P[6];
}

void Trk::RungeKuttaUtils::transformGlobalToLocal 
(const Trk::Surface* su,bool useJac, double* P,double* par,double* Jac) const 
{
  par[2]  = atan2(P[4],P[3]);
  par[3]  = acos (P[5]);
  par[4]  = P[6];

  unsigned int ty = su->type(); 

  if     (ty == Trk::Surface::Plane   ) transformGlobalToPlane   (su,useJac,P,par,Jac); 
  else if(ty == Trk::Surface::Line    ) transformGlobalToLine    (su,useJac,P,par,Jac);
  else if(ty == Trk::Surface::Cylinder) transformGlobalToCylinder(su,useJac,P,par,Jac);
  else if(ty == Trk::Surface::Perigee ) transformGlobalToLine    (su,useJac,P,par,Jac);
  else if(ty == Trk::Surface::Disc    ) transformGlobalToDisc    (su,useJac,P,par,Jac);
  else                                  transformGlobalToCone    (su,useJac,P,par,Jac);

  if(!useJac) return;

  double P3,P4, C = P[3]*P[3]+P[4]*P[4]; 
  if(C > 1.e-20) {C= 1./C ; P3 = P[3]*C; P4 =P[4]*C; C =-sqrt(C);}
  else           {C=-1.e10; P3 = 1.    ; P4 =0.    ;             }

  Jac[10] = P3*P[11]-P4*P[10];    // dPhi/dL0
  Jac[11] = P3*P[18]-P4*P[17];    // dPhi/dL1
  Jac[12] = P3*P[25]-P4*P[24];    // dPhi/dPhi
  Jac[13] = P3*P[32]-P4*P[31];    // dPhi/dThe
  Jac[14] = P3*P[39]-P4*P[38];    // dPhi/dCM
  Jac[15] = C*P[12];              // dThe/dL0
  Jac[16] = C*P[19];              // dThe/dL1
  Jac[17] = C*P[26];              // dThe/dPhi
  Jac[18] = C*P[33];              // dThe/dThe
  Jac[19] = C*P[40];              // dThe/dCM
  Jac[20] = P  [41];              // dCM /dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Plane system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToPlane
(const Trk::Surface* su,bool useJac,double* P,double* par,double* Jac) const 
{  
  const Amg::Transform3D&  T = su->transform();  

  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  double d[3] = {P[0]-T(0,3),P[1]-T(1,3),P[2]-T(2,3)};

  par[0]      = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  par[1]      = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];
  
  if(!useJac) return;

  // Condition trajectory on surface
  //
  double S[3] =  {T(0,2),T(1,2),T(2,2)};

  double    A = P[3]*S[0]+P[4]*S[1]+P[5]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  double s0 = P[ 7]*S[0]+P[ 8]*S[1]+P[ 9]*S[2];
  double s1 = P[14]*S[0]+P[15]*S[1]+P[16]*S[2]; 
  double s2 = P[21]*S[0]+P[22]*S[1]+P[23]*S[2];
  double s3 = P[28]*S[0]+P[29]*S[1]+P[30]*S[2];
  double s4 = P[35]*S[0]+P[36]*S[1]+P[37]*S[2]; 

  P[ 7]-=(s0*P[ 3]); P[ 8]-=(s0*P[ 4]); P[ 9]-=(s0*P[ 5]); 
  P[10]-=(s0*P[42]); P[11]-=(s0*P[43]); P[12]-=(s0*P[44]);
  P[14]-=(s1*P[ 3]); P[15]-=(s1*P[ 4]); P[16]-=(s1*P[ 5]);
  P[17]-=(s1*P[42]); P[18]-=(s1*P[43]); P[19]-=(s1*P[44]);
  P[21]-=(s2*P[ 3]); P[22]-=(s2*P[ 4]); P[23]-=(s2*P[ 5]);
  P[24]-=(s2*P[42]); P[25]-=(s2*P[43]); P[26]-=(s2*P[44]);
  P[28]-=(s3*P[ 3]); P[29]-=(s3*P[ 4]); P[30]-=(s3*P[ 5]);
  P[31]-=(s3*P[42]); P[32]-=(s3*P[43]); P[33]-=(s3*P[44]);
  P[35]-=(s4*P[ 3]); P[36]-=(s4*P[ 4]); P[37]-=(s4*P[ 5]);
  P[38]-=(s4*P[42]); P[39]-=(s4*P[43]); P[40]-=(s4*P[44]);

  // Jacobian production
  //
  Jac[ 0] = Ax[0]*P[ 7]+Ax[1]*P[ 8]+Ax[2]*P[ 9];                               // dL0/dL0
  Jac[ 1] = Ax[0]*P[14]+Ax[1]*P[15]+Ax[2]*P[16];                               // dL0/dL1
  Jac[ 2] = Ax[0]*P[21]+Ax[1]*P[22]+Ax[2]*P[23];                               // dL0/dPhi
  Jac[ 3] = Ax[0]*P[28]+Ax[1]*P[29]+Ax[2]*P[30];                               // dL0/dThe
  Jac[ 4] = Ax[0]*P[35]+Ax[1]*P[36]+Ax[2]*P[37];                               // dL0/dCM
  Jac[ 5] = Ay[0]*P[ 7]+Ay[1]*P[ 8]+Ay[2]*P[ 9];                               // dL1/dL0
  Jac[ 6] = Ay[0]*P[14]+Ay[1]*P[15]+Ay[2]*P[16];                               // dL1/dL1
  Jac[ 7] = Ay[0]*P[21]+Ay[1]*P[22]+Ay[2]*P[23];                               // dL1/dPhi
  Jac[ 8] = Ay[0]*P[28]+Ay[1]*P[29]+Ay[2]*P[30];                               // dL1/dThe
  Jac[ 9] = Ay[0]*P[35]+Ay[1]*P[36]+Ay[2]*P[37];                               // dL1/dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Disc system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToDisc
(const Trk::Surface* su,bool useJac,double* P,double* par,double* Jac) const 
{
  const Amg::Transform3D&  T = su->transform();  

  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  
  double d[3] = {P[0]-T(0,3),P[1]-T(1,3),P[2]-T(2,3)};

  double RC   = d[0]*Ax[0]+d[1]*Ax[1]+d[2]*Ax[2];
  double RS   = d[0]*Ay[0]+d[1]*Ay[1]+d[2]*Ay[2];
  double R2   = RC*RC+RS*RS;
  par[0]      = sqrt(R2);
  par[1]      = atan2(RS,RC);
  
  if(!useJac) return;

  // Condition trajectory on surface
  //
  double S[3] =  {T(0,2),T(1,2),T(2,2)};

  double    A = P[3]*S[0]+P[4]*S[1]+P[5]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  double s0 = P[ 7]*S[0]+P[ 8]*S[1]+P[ 9]*S[2]; 
  double s1 = P[14]*S[0]+P[15]*S[1]+P[16]*S[2]; 
  double s2 = P[21]*S[0]+P[22]*S[1]+P[23]*S[2]; 
  double s3 = P[28]*S[0]+P[29]*S[1]+P[30]*S[2]; 
  double s4 = P[35]*S[0]+P[36]*S[1]+P[37]*S[2]; 

  P[ 7]-=(s0*P[ 3]); P[ 8]-=(s0*P[ 4]); P[ 9]-=(s0*P[ 5]);
  P[10]-=(s0*P[42]); P[11]-=(s0*P[43]); P[12]-=(s0*P[44]);
  P[14]-=(s1*P[ 3]); P[15]-=(s1*P[ 4]); P[16]-=(s1*P[ 5]);
  P[17]-=(s1*P[42]); P[18]-=(s1*P[43]); P[19]-=(s1*P[44]);
  P[21]-=(s2*P[ 3]); P[22]-=(s2*P[ 4]); P[23]-=(s2*P[ 5]);
  P[24]-=(s2*P[42]); P[25]-=(s2*P[43]); P[26]-=(s2*P[44]);
  P[28]-=(s3*P[ 3]); P[29]-=(s3*P[ 4]); P[30]-=(s3*P[ 5]);
  P[31]-=(s3*P[42]); P[32]-=(s3*P[43]); P[33]-=(s3*P[44]);
  P[35]-=(s4*P[ 3]); P[36]-=(s4*P[ 4]); P[37]-=(s4*P[ 5]);
  P[38]-=(s4*P[42]); P[39]-=(s4*P[43]); P[40]-=(s4*P[44]);
 
  // Jacobian production
  //
  double Ri = 1./par[0];
  double A0 =(RC*Ax[0]+RS*Ay[0])*Ri;
  double A1 =(RC*Ax[1]+RS*Ay[1])*Ri;
  double A2 =(RC*Ax[2]+RS*Ay[2])*Ri;
  double B0 =(RC*Ay[0]-RS*Ax[0])*(Ri=1./R2);
  double B1 =(RC*Ay[1]-RS*Ax[1])*Ri;
  double B2 =(RC*Ay[2]-RS*Ax[2])*Ri;

  Jac[ 0] = A0*P[ 7]+A1*P[ 8]+A2*P[ 9];                                        // dL0/dL0
  Jac[ 1] = A0*P[14]+A1*P[15]+A2*P[16];                                        // dL0/dL1
  Jac[ 2] = A0*P[21]+A1*P[22]+A2*P[23];                                        // dL0/dPhi
  Jac[ 3] = A0*P[28]+A1*P[29]+A2*P[30];                                        // dL0/dThe
  Jac[ 4] = A0*P[35]+A1*P[36]+A2*P[37];                                        // dL0/dCM
  Jac[ 5] = B0*P[ 7]+B1*P[ 8]+B2*P[ 9];                                        // dL1/dL0
  Jac[ 6] = B0*P[14]+B1*P[15]+B2*P[16];                                        // dL1/dL1
  Jac[ 7] = B0*P[21]+B1*P[22]+B2*P[23];                                        // dL1/dPhi
  Jac[ 8] = B0*P[28]+B1*P[29]+B2*P[30];                                        // dL1/dThe
  Jac[ 9] = B0*P[35]+B1*P[36]+B2*P[37];                                        // dL1/dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Cylinder system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToCylinder
(const Trk::Surface* su,bool useJac,double* P,double* par,double* Jac) const 
{

  const Amg::Transform3D&  T = su->transform();  
 
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  double Az[3] = {T(0,2),T(1,2),T(2,2)};

  double R     = static_cast<const Trk::CylinderSurface*>(su)->bounds().r();

  double x  = P[0]-T(0,3);
  double y  = P[1]-T(1,3);
  double z  = P[2]-T(2,3);
  double RC = x*Ax[0]+y*Ax[1]+z*Ax[2];
  double RS = x*Ay[0]+y*Ay[1]+z*Ay[2];
  par[0]    = atan2(RS,RC)*R;
  par[1]    = x*Az[0]+y*Az[1]+z*Az[2]; 

  if(!useJac) return;

  // Condition trajectory on surface
  //
  double B  = par[1];
  double C  = P[3]*Az[0]+P[4]*Az[1]+P[5]*Az[2];
  double ax = P[3]-Az[0]*C; x-=(B*Az[0]);
  double ay = P[4]-Az[1]*C; y-=(B*Az[1]);
  double az = P[5]-Az[2]*C; z-=(B*Az[2]);
  double A  =(ax*x+ay*y+az*z); if(A!=0.) A=1./A; x*=A; y*=A; z*=A;

  double s0 = P[ 7]*x+P[ 8]*y+P[ 9]*z; 
  double s1 = P[14]*x+P[15]*y+P[16]*z; 
  double s2 = P[21]*x+P[22]*y+P[23]*z; 
  double s3 = P[28]*x+P[29]*y+P[30]*z; 
  double s4 = P[35]*x+P[36]*y+P[37]*z; 

  P[ 7]-=(s0*P[ 3]); P[ 8]-=(s0*P[ 4]); P[ 9]-=(s0*P[ 5]);
  P[10]-=(s0*P[42]); P[11]-=(s0*P[43]); P[12]-=(s0*P[44]);
  P[14]-=(s1*P[ 3]); P[15]-=(s1*P[ 4]); P[16]-=(s1*P[ 5]);
  P[17]-=(s1*P[42]); P[18]-=(s1*P[43]); P[19]-=(s1*P[44]);
  P[21]-=(s2*P[ 3]); P[22]-=(s2*P[ 4]); P[23]-=(s2*P[ 5]);
  P[24]-=(s2*P[42]); P[25]-=(s2*P[43]); P[26]-=(s2*P[44]);
  P[28]-=(s3*P[ 3]); P[29]-=(s3*P[ 4]); P[30]-=(s3*P[ 5]);
  P[31]-=(s3*P[42]); P[32]-=(s3*P[43]); P[33]-=(s3*P[44]);
  P[35]-=(s4*P[ 3]); P[36]-=(s4*P[ 4]); P[37]-=(s4*P[ 5]);
  P[38]-=(s4*P[42]); P[39]-=(s4*P[43]); P[40]-=(s4*P[44]);

  // Jacobian production
  //
  double A0 =(RC*Ay[0]-RS*Ax[0])*(R=1./R);
  double A1 =(RC*Ay[1]-RS*Ax[1])* R;
  double A2 =(RC*Ay[2]-RS*Ax[2])* R;

  Jac[ 0] = A0*P[ 7]+A1*P[ 8]+A2*P[ 9];                                        // dL0/dL0
  Jac[ 1] = A0*P[14]+A1*P[15]+A2*P[16];                                        // dL0/dL1
  Jac[ 2] = A0*P[21]+A1*P[22]+A2*P[23];                                        // dL0/dPhi
  Jac[ 3] = A0*P[28]+A1*P[29]+A2*P[30];                                        // dL0/dThe
  Jac[ 4] = A0*P[35]+A1*P[36]+A2*P[37];                                        // dL0/dCM
  Jac[ 5] = Az[0]*P[ 7]+Az[1]*P[ 8]+Az[2]*P[ 9];                               // dL1/dL0
  Jac[ 6] = Az[0]*P[14]+Az[1]*P[15]+Az[2]*P[16];                               // dL1/dL1 
  Jac[ 7] = Az[0]*P[21]+Az[1]*P[22]+Az[2]*P[23];                               // dL1/dPhi
  Jac[ 8] = Az[0]*P[28]+Az[1]*P[29]+Az[2]*P[30];                               // dL1/dThe
  Jac[ 9] = Az[0]*P[35]+Az[1]*P[36]+Az[2]*P[37];                               // dL1/dP4
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Straight line  system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToLine
(const Trk::Surface* su,bool useJac,double* P,double* par,double* Jac) const 
{
  const Amg::Transform3D&  T = su->transform();  
  
  double A[3] = {T(0,2),T(1,2),T(2,2)};

  double Bx = A[1]*P[5]-A[2]*P[4];
  double By = A[2]*P[3]-A[0]*P[5];
  double Bz = A[0]*P[4]-A[1]*P[3];
  double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz); Bx*=Bn; By*=Bn; Bz*=Bn;
  double x  = P[0]-T(0,3);
  double y  = P[1]-T(1,3);
  double z  = P[2]-T(2,3);
  par[0]    = x*Bx  +y*By  +z*Bz  ;
  par[1]    = x*A[0]+y*A[1]+z*A[2];

  if(!useJac) return;

  // Condition trajectory on surface
  //
  double d  = P[3]*A[0]+P[4]*A[1]+P[5]*A[2]; 
  double a  = (1.-d)*(1.+d); if(a!=0.) a=1./a;
  double X = d*A[0]-P[3], Y = d*A[1]-P[4], Z = d*A[2]-P[5];

  double d1 = P[10]*A[0]+P[11]*A[1]+P[12]*A[2];
  double d2 = P[17]*A[0]+P[18]*A[1]+P[19]*A[2];
  double d3 = P[24]*A[0]+P[25]*A[1]+P[26]*A[2];
  double d4 = P[31]*A[0]+P[32]*A[1]+P[33]*A[2];
  double d5 = P[38]*A[0]+P[39]*A[1]+P[40]*A[2];
  double s1 = (((P[ 7]*X+P[ 8]*Y+P[ 9]*Z)+x*(d1*A[0]-P[10]))+(y*(d1*A[1]-P[11])+z*(d1*A[2]-P[12])))*a;
  double s2 = (((P[14]*X+P[15]*Y+P[16]*Z)+x*(d2*A[0]-P[17]))+(y*(d2*A[1]-P[18])+z*(d2*A[2]-P[19])))*a;
  double s3 = (((P[21]*X+P[22]*Y+P[23]*Z)+x*(d3*A[0]-P[24]))+(y*(d3*A[1]-P[25])+z*(d3*A[2]-P[26])))*a;
  double s4 = (((P[28]*X+P[29]*Y+P[30]*Z)+x*(d4*A[0]-P[31]))+(y*(d4*A[1]-P[32])+z*(d4*A[2]-P[33])))*a;
  double s5 = (((P[35]*X+P[36]*Y+P[37]*Z)+x*(d5*A[0]-P[38]))+(y*(d5*A[1]-P[39])+z*(d5*A[2]-P[40])))*a;
  
  P[ 7]+=(s1*P[ 3]); P[ 8]+=(s1*P[ 4]); P[ 9]+=(s1*P[ 5]); 
  P[10]+=(s1*P[42]); P[11]+=(s1*P[43]); P[12]+=(s1*P[44]);
  P[14]+=(s2*P[ 3]); P[15]+=(s2*P[ 4]); P[16]+=(s2*P[ 5]); 
  P[17]+=(s2*P[42]); P[18]+=(s2*P[43]); P[19]+=(s2*P[44]);
  P[21]+=(s3*P[ 3]); P[22]+=(s3*P[ 4]); P[23]+=(s3*P[ 5]); 
  P[24]+=(s3*P[42]); P[25]+=(s3*P[43]); P[26]+=(s3*P[44]);
  P[28]+=(s4*P[ 3]); P[29]+=(s4*P[ 4]); P[30]+=(s4*P[ 5]); 
  P[31]+=(s4*P[42]); P[32]+=(s4*P[43]); P[33]+=(s4*P[44]);
  P[35]+=(s5*P[ 3]); P[36]+=(s5*P[ 4]); P[37]+=(s5*P[ 5]); 
  P[38]+=(s5*P[42]); P[39]+=(s5*P[43]); P[40]+=(s5*P[44]);

  // Jacobian production
  //
  Jac[ 0] = Bx  *P[ 7]+By  *P[ 8]+Bz  *P[ 9];       // dL0/dL0
  Jac[ 1] = Bx  *P[14]+By  *P[15]+Bz  *P[16];       // dL0/dL1
  Jac[ 2] = Bx  *P[21]+By  *P[22]+Bz  *P[23];       // dL0/dPhi
  Jac[ 3] = Bx  *P[28]+By  *P[29]+Bz  *P[30];       // dL0/dThe
  Jac[ 4] = Bx  *P[35]+By  *P[36]+Bz  *P[37];       // dL0/dCM
  Jac[ 5] = A[0]*P[ 7]+A[1]*P[ 8]+A[2]*P[ 9];       // dL1/dL0
  Jac[ 6] = A[0]*P[14]+A[1]*P[15]+A[2]*P[16];       // dL1/dL1
  Jac[ 7] = A[0]*P[21]+A[1]*P[22]+A[2]*P[23];       // dL1/dPhi
  Jac[ 8] = A[0]*P[28]+A[1]*P[29]+A[2]*P[30];       // dL1/dThe
  Jac[ 9] = A[0]*P[35]+A[1]*P[36]+A[2]*P[37];       // dL1/dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to local Cone  system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToCone
(const Trk::Surface* su,bool useJac,double* P,double* par,double* Jac) const 
{

  const Amg::Transform3D&  T = su->transform();  

  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  double Az[3] = {T(0,2),T(1,2),T(2,2)};

  double x  = P[0]-T(0,3);
  double y  = P[1]-T(1,3);
  double z  = P[2]-T(2,3);
  double RC = x*Ax[0]+y*Ax[1]+z*Ax[2];
  double RS = x*Ay[0]+y*Ay[1]+z*Ay[2];
  double tA = static_cast<const Trk::ConeSurface*>(su)->bounds().tanAlpha();
  par[1]    = x*Az[0]+y*Az[1]+z*Az[2]; 
  par[0]    = atan2(RS,RC)*(par[1]*tA);

  if(!useJac) return;

  Jac[ 0] = 0.;        // dL0/dL0
  Jac[ 1] = 0.;        // dL0/dL1
  Jac[ 2] = 0.;        // dL0/dPhi
  Jac[ 3] = 0.;        // dL0/dThe
  Jac[ 4] = 0.;        // dL0/dCM
  Jac[ 5] = 0.;        // dL1/dL0
  Jac[ 6] = 0.;        // dL1/dL1
  Jac[ 7] = 0.;        // dL1/dPhi
  Jac[ 8] = 0.;        // dL1/dThe
  Jac[ 9] = 0.;        // dL1/dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Main program for step estimation to surfaces
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimator
(int kind,double* Su,const double* P,bool& Q) const
{
  if(kind==1) return stepEstimatorToPlane       (Su,P,Q);
  if(kind==0) return stepEstimatorToStraightLine(Su,P,Q);
  if(kind==2) return stepEstimatorToCylinder    (Su,P,Q);
  if(kind==3) return stepEstimatorToCone        (Su,P,Q);
  return 1000000.;
} 

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Plane
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToPlane 
(double* S,const double* P,bool& Q) const
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction
  
  double  A = a[0]*S[0]+a[1]*S[1]+a[2]*S[2]; if(A==0.) {Q=false; return 1000000.;} 
  double  D = (S[3]-r[0]*S[0])-(r[1]*S[1]+r[2]*S[2]);
  Q=true;  return (D/A);
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Cylinder
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToCylinder
(double* S,const double* P,bool& Q) const 
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction

  double dx = r[0]-S[0];
  double dy = r[1]-S[1];
  double dz = r[2]-S[2];
  double B  = dx  *S[3]+dy  *S[4]+dz  *S[5];
  double C  = a[0]*S[3]+a[1]*S[4]+a[2]*S[5];
  double ax = a[0]-S[3]*C; dx-=(B*S[3]);
  double ay = a[1]-S[4]*C; dy-=(B*S[4]);
  double az = a[2]-S[5]*C; dz-=(B*S[5]);
  double A  = 2.*(ax*ax+ay*ay+az*az); if(A==0.) {Q=false; return 0.;}
  B         = 2.*(ax*dx+ay*dy+az*dz);
  double g  = dx+dy+dz;
  C         = (g-S[6])*(g+S[6])-2.*(dx*(dy+dz)+dy*dz);
  double Sq = B*B-2.*A*C;

  double Smin = -B/A, Smax = Smin;

  if( Sq > 0.) {

    Sq = sqrt(Sq)/A; if(B>0.) {Smin+=Sq; Smax-=Sq;} else {Smin-=Sq; Smax+=Sq;}
  }
  else {
    if(fabs(Smax) < .1) {Q = false; return 0.;}
  }

  Q = true;
  double inside = Smin*Smax;

  if(S[8]!=0.) {

    if(inside > 0. || S[8] > 0.) return Smin;
    if(S[7] >=0. ) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  if(inside < 0.) {

    S[8]=-1.;
    if(S[7]>=0.) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  //if(fabs(Smin) < .001) {S[8]=-1.; return Smax;} 
  
  S[8]= 1.; return Smin;
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Straight Line
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToStraightLine
(double* S,const double* P,bool& Q) const
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction
  
  double D = a[0]*S[3]+a[1]*S[4]+a[2]*S[5];
  double A = (1.-D)*(1.+D); if(A==0.) {Q=true;  return 1000000.;}
  D        = (r[0]-S[0])*(D*S[3]-a[0])
            +(r[1]-S[1])*(D*S[4]-a[1])
            +(r[2]-S[2])*(D*S[5]-a[2]);
  Q=true; return (D/A);
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to Cone
/////////////////////////////////////////////////////////////////////////////////

double Trk::RungeKuttaUtils::stepEstimatorToCone
(double* S,const double* P,bool& Q) const 
{
  const double* r = &P[0];          // Start coordinate
  const double* a = &P[3];          // Start direction

  double dx = r[0]-S[0];
  double dy = r[1]-S[1];
  double dz = r[2]-S[2];
  double A  = dx  *S[3]+dy  *S[4]+dz  *S[5];
  double B  = a[0]*S[3]+a[1]*S[4]+a[2]*S[5];
  double C  = a[0]* dx +a[1]*dy  +a[2]*dz  ;
  double k  = S[6]                         ;

  double KB   = 1.-k*B*B;
  double KABC = k*A*B-C ;
  double Smin,Smax      ;

  if(KB!=0.) {

    Smin       = KABC/KB; Smax = Smin;
    double Sq  = KABC*KABC+(k*A*A-dx*dx-dy*dy-dz*dz)*KB;
    if(Sq >= 0.) {
      Sq = sqrt(Sq)/KB; if(KABC>0.) {Smin-=Sq; Smax+=Sq;} else {Smin+=Sq; Smax-=Sq;}
      int n = 2;
      if( A+B*Smin < 0.) {--n; Smin = Smax;}
      if( A+B*Smax < 0.) {--n; Smax = Smin;}
      if(!n) {Q = false; return 0.;}
      
    }
    else         {
      Q = false; return 0.;
    }
  }
  else      {
    Smin = (dx*dx+dy*dy+dz*dz-k*A*A)/(2.*KABC); Smax = Smin;
    if( A+B*Smin < 0.) {Q = false; return 0.;}
    
  }

  Q = true;
  double inside = Smin*Smax;

  if(S[8]!=0.) {

    if(inside > 0. || S[8] > 0.) return Smin;
    if(S[7] >=0. ) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  if(inside < 0.) {

    S[8]=-1.;
    if(S[7]>=0.) {
      if(Smin>=0.) return Smin;
      return Smax;
    }
    if(Smin<=0.) return Smin;
    return Smax;
  }

  S[8]= 1.; return Smin;
}

/////////////////////////////////////////////////////////////////////////////////
// Step estimation to surfaces
//
// Input iformation
//
// SU    - input vector surfaces and conditions for boundary check
// DN    - is work map
// Pinp  - array of track parameters in global system coordinates before some step
// Pout  - array of track parameters in global system coordinates before some step
// W     - trajectory total pass for Pinp position
// So    - max. step for straight line propagation model
// Nv    - surface number in vector SU  which we don't need to use
//
// Output information
//
// pair(step to surface,surface number from vector SU) 
// next  - condition to use Pout for next step 
/////////////////////////////////////////////////////////////////////////////////

std::pair<double,int> Trk::RungeKuttaUtils::stepEstimator 
(std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >& SU,std::multimap<double,int>& DN,
 const double* Pinp,const double* Pout,double W,double So,int Nv,bool& next) const
{
  W             = fabs(W)                                          ;
  next          = false                                            ;
  int N         = -1                                               ;
  double D [3]  = {Pout[0]-Pinp[0],Pout[1]-Pinp[1],Pout[2]-Pinp[2]};
  double Smax   = sqrt(D[0]*D[0]+D[1]*D[1]+D[2]*D[2])              ;
  double Sign   = D[0]*Pinp[3]+D[1]*Pinp[4]+D[2]*Pinp[5]           ;

  Amg::Vector3D pos( Pinp[0]  , Pinp[1]  , Pinp[2]  );
  Amg::Vector3D dir( D[0]/Smax, D[1]/Smax, D[2]/Smax);

  double Smin = 2.*Smax;

  std::list<std::pair<double,int> > LD;
  std::multimap<double,int>::iterator i = DN.begin(),ie = DN.end();

  for(; i!=ie; ++i) {
    
    if((*i).first > W+Smin) break;
    
    int j = (*i).second;
    Trk::DistanceSolution ds = SU[j].first->straightLineDistanceEstimate(pos,dir,SU[j].second);
    LD.push_back(std::make_pair(ds.currentDistance(false)+W,j));
    
    int  n = ds.numberOfSolutions(); if(!n) continue;
    
    for(int i=0; i!=n; ++i) {
      double s; i == 0 ? s=ds.first() : s=ds.second(); 
      if(s < 0. || s > Smin || (j==Nv && s < So)) continue;
      Smin = s; N = j;
    }
  }

  if(!LD.empty()) {
    
    DN.erase(DN.begin(),i);
    std::list<std::pair<double,int> >::iterator l = LD.begin(), le = LD.end();
    for(; l!=le; ++l) DN.insert((*l));
  }

  if(N < 0) {next = true; return std::make_pair(0.,-1);}

  double Sm = Smin;
  
  if(Sign < 0.) Sm = -Sm;
  if(Smin < So || (Smax-Smin)  > 2.*So) return std::make_pair(Sm,N);

  Amg::Vector3D  posn(Pout[0],Pout[1],Pout[2]);
  Amg::Vector3D  dirn(Pout[3],Pout[4],Pout[5]);

  Trk::DistanceSolution ds = SU[N].first->straightLineDistanceEstimate(posn,dirn,SU[N].second);
  int  n = ds.numberOfSolutions(); if(!n) return std::make_pair(Sm,N);
  Sm = 10000.;

  for(int i=0; i!=n; ++i) {
   
    double sa,s; i == 0 ? s = ds.first() : s = ds.second(); sa = fabs(s); 

    if(s*Sign < 0.) {
      //if(sa < So    ) {next = true; return std::make_pair(s,N);}
      next = true; return std::make_pair(s,N);
    }
    else if(sa < Sm ) {
      Sm = s; next = true;
    }
    /*
    else if(sa < fabs(Sm) && Smin >= Smax) {
      Sm = s; next = true;
    }
    */
  }
  return std::make_pair(Sm,N);
}

/////////////////////////////////////////////////////////////////////////////////
// New covariance matrix calculation from old matrix and jacobian
/////////////////////////////////////////////////////////////////////////////////

AmgSymMatrix(5)* Trk::RungeKuttaUtils::newCovarianceMatrix
(double* J,const AmgSymMatrix(5)& M) const 
{
  double V[15]={M(0,0),
		M(1,0),M(1,1),
		M(2,0),M(2,1),M(2,2),
		M(3,0),M(3,1),M(3,2),M(3,3),
		M(4,0),M(4,1),M(4,2),M(4,3),M(4,4)};

  AmgSymMatrix(5)* nM = new AmgSymMatrix(5);
  AmgSymMatrix(5)& m = (*nM);

  double a11 = (J[ 0]*V[ 0]+J[ 1]*V[ 1]+J[ 2]*V[ 3])+(J[ 3]*V[ 6]+J[ 4]*V[10]);   
  double a12 = (J[ 0]*V[ 1]+J[ 1]*V[ 2]+J[ 2]*V[ 4])+(J[ 3]*V[ 7]+J[ 4]*V[11]);   
  double a13 = (J[ 0]*V[ 3]+J[ 1]*V[ 4]+J[ 2]*V[ 5])+(J[ 3]*V[ 8]+J[ 4]*V[12]);   
  double a14 = (J[ 0]*V[ 6]+J[ 1]*V[ 7]+J[ 2]*V[ 8])+(J[ 3]*V[ 9]+J[ 4]*V[13]);   
  double a15 = (J[ 0]*V[10]+J[ 1]*V[11]+J[ 2]*V[12])+(J[ 3]*V[13]+J[ 4]*V[14]);   

  m(0,0) = (a11*J[ 0]+a12*J[ 1]+a13*J[ 2])+(a14*J[ 3]+a15*J[ 4]);
  
  double a21 = (J[ 5]*V[ 0]+J[ 6]*V[ 1]+J[ 7]*V[ 3])+(J[ 8]*V[ 6]+J[ 9]*V[10]);   
  double a22 = (J[ 5]*V[ 1]+J[ 6]*V[ 2]+J[ 7]*V[ 4])+(J[ 8]*V[ 7]+J[ 9]*V[11]);   
  double a23 = (J[ 5]*V[ 3]+J[ 6]*V[ 4]+J[ 7]*V[ 5])+(J[ 8]*V[ 8]+J[ 9]*V[12]);   
  double a24 = (J[ 5]*V[ 6]+J[ 6]*V[ 7]+J[ 7]*V[ 8])+(J[ 8]*V[ 9]+J[ 9]*V[13]);   
  double a25 = (J[ 5]*V[10]+J[ 6]*V[11]+J[ 7]*V[12])+(J[ 8]*V[13]+J[ 9]*V[14]);   

  m(1,0) = (a21*J[ 0]+a22*J[ 1]+a23*J[ 2])+(a24*J[ 3]+a25*J[ 4]);
  m(1,1) = (a21*J[ 5]+a22*J[ 6]+a23*J[ 7])+(a24*J[ 8]+a25*J[ 9]);
  m(0,1) = m(1,0);
  
  double a31 = (J[10]*V[ 0]+J[11]*V[ 1]+J[12]*V[ 3])+(J[13]*V[ 6]+J[14]*V[10]);   
  double a32 = (J[10]*V[ 1]+J[11]*V[ 2]+J[12]*V[ 4])+(J[13]*V[ 7]+J[14]*V[11]);   
  double a33 = (J[10]*V[ 3]+J[11]*V[ 4]+J[12]*V[ 5])+(J[13]*V[ 8]+J[14]*V[12]);   
  double a34 = (J[10]*V[ 6]+J[11]*V[ 7]+J[12]*V[ 8])+(J[13]*V[ 9]+J[14]*V[13]);   
  double a35 = (J[10]*V[10]+J[11]*V[11]+J[12]*V[12])+(J[13]*V[13]+J[14]*V[14]);   

  m(2,0) = (a31*J[ 0]+a32*J[ 1]+a33*J[ 2])+(a34*J[ 3]+a35*J[ 4]);
  m(2,1) = (a31*J[ 5]+a32*J[ 6]+a33*J[ 7])+(a34*J[ 8]+a35*J[ 9]);
  m(2,2) = (a31*J[10]+a32*J[11]+a33*J[12])+(a34*J[13]+a35*J[14]);
  m(0,2) = m(2,0);
  m(1,2) = m(2,1);

  double a41 = (J[15]*V[ 0]+J[16]*V[ 1]+J[17]*V[ 3])+(J[18]*V[ 6]+J[19]*V[10]);   
  double a42 = (J[15]*V[ 1]+J[16]*V[ 2]+J[17]*V[ 4])+(J[18]*V[ 7]+J[19]*V[11]);   
  double a43 = (J[15]*V[ 3]+J[16]*V[ 4]+J[17]*V[ 5])+(J[18]*V[ 8]+J[19]*V[12]);   
  double a44 = (J[15]*V[ 6]+J[16]*V[ 7]+J[17]*V[ 8])+(J[18]*V[ 9]+J[19]*V[13]);   
  double a45 = (J[15]*V[10]+J[16]*V[11]+J[17]*V[12])+(J[18]*V[13]+J[19]*V[14]);   
  
  m(3,0) = (a41*J[ 0]+a42*J[ 1]+a43*J[ 2])+(a44*J[ 3]+a45*J[ 4]);
  m(3,1) = (a41*J[ 5]+a42*J[ 6]+a43*J[ 7])+(a44*J[ 8]+a45*J[ 9]);
  m(3,2) = (a41*J[10]+a42*J[11]+a43*J[12])+(a44*J[13]+a45*J[14]);
  m(3,3) = (a41*J[15]+a42*J[16]+a43*J[17])+(a44*J[18]+a45*J[19]);
  m(0,3) = m(3,0);
  m(1,3) = m(3,1);
  m(2,3) = m(3,2);

  double a51 =                                                    J[20]*V[10];   
  double a52 =                                                    J[20]*V[11];   
  double a53 =                                                    J[20]*V[12];   
  double a54 =                                                    J[20]*V[13];   
  double a55 =                                                    J[20]*V[14];   

  m(4,0) = (a51*J[ 0]+a52*J[ 1]+a53*J[ 2])+(a54*J[ 3]+a55*J[ 4]);
  m(4,1) = (a51*J[ 5]+a52*J[ 6]+a53*J[ 7])+(a54*J[ 8]+a55*J[ 9]);
  m(4,2) = (a51*J[10]+a52*J[11]+a53*J[12])+(a54*J[13]+a55*J[14]);
  m(4,3) = (a51*J[15]+a52*J[16]+a53*J[17])+(a54*J[18]+a55*J[19]);
  m(4,4) =                                            a55*J[20];
  m(0,4) = m(4,0);
  m(1,4) = m(4,1);
  m(2,4) = m(4,2);
  m(3,4) = m(4,3);
  return nM;
}

/////////////////////////////////////////////////////////////////////////////////
// Plane local position transformation to global system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformPlaneToGlobal
(bool useJac,const Trk::Surface* Su,const double* p,double* P) const 
{
  const Amg::Transform3D& T = Su->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};

  P[ 0] = p[0]*Ax[0]+p[1]*Ay[0]+T(0,3);                            // X
  P[ 1] = p[0]*Ax[1]+p[1]*Ay[1]+T(1,3);                            // Y
  P[ 2] = p[0]*Ax[2]+p[1]*Ay[2]+T(2,3);                            // Z

  if(!useJac) return;

  //    /dL1   |     /dL2     |   /dPhi   |  /dThe    |
  P[ 7] = Ax[0]; P[14] = Ay[0]; P[21] = 0.; P[28] = 0.;             // dX/
  P[ 8] = Ax[1]; P[15] = Ay[1]; P[22] = 0.; P[29] = 0.;             // dY/
  P[ 9] = Ax[2]; P[16] = Ay[2]; P[23] = 0.; P[30] = 0.;             // dZ/

}


/////////////////////////////////////////////////////////////////////////////////
// Disc local position transformation to global system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformDiscToGlobal 
(bool useJac,const Trk::Surface* Su,const double* p, double* P) const 
{
  const Amg::Transform3D& T = Su->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  double Sf,Cf; sincos(p[1],&Sf,&Cf);

  double d0 = Cf*Ax[0]+Sf*Ay[0]; 
  double d1 = Cf*Ax[1]+Sf*Ay[1]; 
  double d2 = Cf*Ax[2]+Sf*Ay[2];
  P[ 0]     = p[0]*d0+T(0,3)   ;                                    // X
  P[ 1]     = p[0]*d1+T(1,3)   ;                                    // Y
  P[ 2]     = p[0]*d2+T(2,3)   ;                                    // Z

  if(!useJac) return;
  
  //  /dL1  |              /dL2               |   /dPhi |  /dThe  |
  P[ 7] = d0; P[14] = p[0]*(Cf*Ay[0]-Sf*Ax[0]); P[21]=0.; P[28]=0.; // dX/
  P[ 8] = d1; P[15] = p[0]*(Cf*Ay[1]-Sf*Ax[1]); P[22]=0.; P[29]=0.; // dY/
  P[ 9] = d2; P[16] = p[0]*(Cf*Ay[2]-Sf*Ax[2]); P[23]=0.; P[30]=0.; // dZ/
}

/////////////////////////////////////////////////////////////////////////////////
// Cylinder local position transformation to global system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformCylinderToGlobal    
(bool useJac,const Trk::Surface* Su,const double* p,double* P) const 
{
  const Amg::Transform3D& T = Su->transform();
  double Ax[3] = {T(0,0),T(1,0),T(2,0)};
  double Ay[3] = {T(0,1),T(1,1),T(2,1)};
  double Az[3] = {T(0,2),T(1,2),T(2,2)};

  double  R = static_cast<const Trk::CylinderSurface*>(Su)->bounds().r();

  double fr = p[0]/R;
  double Sf,Cf; sincos(fr,&Sf,&Cf);

  P[ 0]     = R*(Cf*Ax[0]+Sf*Ay[0])+p[1]*Az[0]+T(0,3);              // X
  P[ 1]     = R*(Cf*Ax[1]+Sf*Ay[1])+p[1]*Az[1]+T(1,3);              // Y
  P[ 2]     = R*(Cf*Ax[2]+Sf*Ay[2])+p[1]*Az[2]+T(2,3);              // Z

  if(!useJac) return;
  
  //           /dL1        |    /dL2      |   /dPhi   |   /dThe   |
  P[ 7] = Cf*Ay[0]-Sf*Ax[0]; P[14] = Az[0]; P[21] = 0.; P[28] = 0.; // dX/
  P[ 8] = Cf*Ay[1]-Sf*Ax[1]; P[15] = Az[1]; P[22] = 0.; P[29] = 0.; // dY/
  P[ 9] = Cf*Ay[2]-Sf*Ax[2]; P[16] = Az[2]; P[23] = 0.; P[30] = 0.; // dZ/
}

/////////////////////////////////////////////////////////////////////////////////
// Straight line local position transformation to global system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformLineToGlobal
(bool useJac,const Trk::Surface* Su,const double* p,double* P) const
{
  const Amg::Transform3D& T = Su->transform();
  double A[3] = {T(0,2),T(1,2),T(2,2)};

  double Bx = A[1]*P[5]-A[2]*P[4];
  double By = A[2]*P[3]-A[0]*P[5];
  double Bz = A[0]*P[4]-A[1]*P[3];
  double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz); Bx*=Bn; By*=Bn; Bz*=Bn; 
  P[ 0]     = p[1]*A[0]+Bx*p[0]+T(0,3);                             // X
  P[ 1]     = p[1]*A[1]+By*p[0]+T(1,3);                             // Y
  P[ 2]     = p[1]*A[2]+Bz*p[0]+T(2,3);                             // Z
  
  if(!useJac) return; 

  double Bx2 =           -A[2]*P[25], Bx3 = A[1]*P[33]-A[2]*P[32];
  double By2 = A[2]*P[24]           , By3 = A[2]*P[31]-A[0]*P[33];
  double Bz2 = A[0]*P[25]-A[1]*P[24], Bz3 = A[0]*P[32]-A[1]*P[31];
  double B2  = Bx*Bx2+By*By2+Bz*Bz2 , B3  = Bx*Bx3+By*By3+Bz*Bz3 ;
  Bx2        =(Bx2-Bx*B2)*Bn        ; Bx3 =(Bx3-Bx*B3)*Bn        ;
  By2        =(By2-By*B2)*Bn        ; By3 =(By3-By*B3)*Bn        ;
  Bz2        =(Bz2-Bz*B2)*Bn        ; Bz3 =(Bz3-Bz*B3)*Bn        ;
  
  //  /dL1  |     /dL2    |      /dPhi      |     /dThe       |
  P[ 7] = Bx; P[14] = A[0]; P[21] = Bx2*p[0]; P[28] = Bx3*p[0];     // dX/
  P[ 8] = By; P[15] = A[1]; P[22] = By2*p[0]; P[29] = By3*p[0];     // dY/
  P[ 9] = Bz; P[16] = A[2]; P[23] = Bz2*p[0]; P[30] = Bz3*p[0];     // dZ/
}

/////////////////////////////////////////////////////////////////////////////////
// Tramsform from local to global for all track parameters
/////////////////////////////////////////////////////////////////////////////////

bool Trk::RungeKuttaUtils::transformLocalToGlobal
(bool useJac,const Trk::Surface* su,const double* p,double* P) const
{
  if(!su) return false;

  double Sf,Cf,Ce,Se; sincos(p[2],&Sf,&Cf);  sincos(p[3],&Se,&Ce);

  P[ 3] = Cf*Se;                                                         // Ax
  P[ 4] = Sf*Se;                                                         // Ay
  P[ 5] = Ce;                                                            // Az
  P[ 6] = p[4];                                                          // CM 
  if(fabs(P[6])<.000000000000001) {
    P[6]<0. ? P[6]=-.000000000000001 : P[6]= .000000000000001;
  }    

  if(useJac) {

    //   /dL1   |   /dL2    |    /dPhi    |    /dThe     |    /dCM     |
                                                           P[35] =   0.; // dX /
                                                           P[36] =   0.; // dY /
                                                           P[37] =   0.; // dZ /
    P[10]  =  0.; P[17] = 0.; P[24] =-P[4]; P[31] = Cf*Ce; P[38] =   0.; // dAx/
    P[11]  =  0.; P[18] = 0.; P[25] = P[3]; P[32] = Sf*Ce; P[39] =   0.; // dAy/
    P[12]  =  0.; P[19] = 0.; P[26] =   0.; P[33] =   -Se; P[40] =   0.; // dAz/
    P[13]  =  0.; P[20] = 0.; P[27] =   0.; P[34] =    0.; P[41] =   1.; // dCM/
    P[42]  =  0.; P[43] = 0.; P[44] =   0.;                              // d(Ax,Ay,Az)/ds
  }

  unsigned int ty = su->type();
  if(ty == Trk::Surface::Plane   ) {transformPlaneToGlobal   (useJac,su,p,P); return true;}
  if(ty == Trk::Surface::Line    ) {transformLineToGlobal    (useJac,su,p,P); return true;}
  if(ty == Trk::Surface::Cylinder) {transformCylinderToGlobal(useJac,su,p,P); return true;}
  if(ty == Trk::Surface::Perigee ) {transformLineToGlobal    (useJac,su,p,P); return true;}
  if(ty == Trk::Surface::Disc    ) {transformDiscToGlobal    (useJac,su,p,P); return true;}
  return false; 
}

/////////////////////////////////////////////////////////////////////////////////
// Global position transformation to curvilinear system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformGlobalToCurvilinear
(bool useJac,double* P,double* par,double* Jac) const 
{
  par[0] = 0.;
  par[1] = 0.;

  if(!useJac) return;

  double An    = sqrt(P[3]*P[3]+P[4]*P[4]);
  double Ax[3];
  if(An!=0.) {Ax[0] = -P[4]/An; Ax[1] = P[3]/An; Ax[2] = 0.;}
  else       {Ax[0] =       1.; Ax[1] = 0.;      Ax[2] = 0.;}

  double Ay[3] = {-Ax[1]*P[5],Ax[0]*P[5],An  };
  double S [3] = {    P[3]   , P[4]     ,P[5]};

  double    A = P[3]*S[0]+P[4]*S[1]+P[5]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;
   
  double s0 = P[ 7]*S[0]+P[ 8]*S[1]+P[ 9]*S[2];
  double s1 = P[14]*S[0]+P[15]*S[1]+P[16]*S[2]; 
  double s2 = P[21]*S[0]+P[22]*S[1]+P[23]*S[2];
  double s3 = P[28]*S[0]+P[29]*S[1]+P[30]*S[2];
  double s4 = P[35]*S[0]+P[36]*S[1]+P[37]*S[2]; 

  P[ 7]-=(s0*P[ 3]); P[ 8]-=(s0*P[ 4]); P[ 9]-=(s0*P[ 5]); 
  P[10]-=(s0*P[42]); P[11]-=(s0*P[43]); P[12]-=(s0*P[44]);
  P[14]-=(s1*P[ 3]); P[15]-=(s1*P[ 4]); P[16]-=(s1*P[ 5]);
  P[17]-=(s1*P[42]); P[18]-=(s1*P[43]); P[19]-=(s1*P[44]);
  P[21]-=(s2*P[ 3]); P[22]-=(s2*P[ 4]); P[23]-=(s2*P[ 5]);
  P[24]-=(s2*P[42]); P[25]-=(s2*P[43]); P[26]-=(s2*P[44]);
  P[28]-=(s3*P[ 3]); P[29]-=(s3*P[ 4]); P[30]-=(s3*P[ 5]);
  P[31]-=(s3*P[42]); P[32]-=(s3*P[43]); P[33]-=(s3*P[44]);
  P[35]-=(s4*P[ 3]); P[36]-=(s4*P[ 4]); P[37]-=(s4*P[ 5]);
  P[38]-=(s4*P[42]); P[39]-=(s4*P[43]); P[40]-=(s4*P[44]);

  double P3,P4,C = P[3]*P[3]+P[4]*P[4]; 
  if(C > 1.e-20) {C= 1./C ; P3 = P[3]*C; P4 =P[4]*C; C =-sqrt(C);}
  else           {C=-1.e10; P3 = 1.    ; P4 =0.    ;             }

  // Jacobian production
  //
  Jac[ 0] = Ax[0]*P[ 7]+Ax[1]*P[ 8];               // dL0/dL0
  Jac[ 1] = Ax[0]*P[14]+Ax[1]*P[15];               // dL0/dL1
  Jac[ 2] = Ax[0]*P[21]+Ax[1]*P[22];               // dL0/dPhi
  Jac[ 3] = Ax[0]*P[28]+Ax[1]*P[29];               // dL0/dThe
  Jac[ 4] = Ax[0]*P[35]+Ax[1]*P[36];               // dL0/dCM
  Jac[ 5] = Ay[0]*P[ 7]+Ay[1]*P[ 8]+Ay[2]*P[ 9];   // dL1/dL0
  Jac[ 6] = Ay[0]*P[14]+Ay[1]*P[15]+Ay[2]*P[16];   // dL1/dL1
  Jac[ 7] = Ay[0]*P[21]+Ay[1]*P[22]+Ay[2]*P[23];   // dL1/dPhi
  Jac[ 8] = Ay[0]*P[28]+Ay[1]*P[29]+Ay[2]*P[30];   // dL1/dThe
  Jac[ 9] = Ay[0]*P[35]+Ay[1]*P[36]+Ay[2]*P[37];   // dL1/dCM
  Jac[10] = P3*P[11]-P4*P[10];                     // dPhi/dL0
  Jac[11] = P3*P[18]-P4*P[17];                     // dPhi/dL1
  Jac[12] = P3*P[25]-P4*P[24];                     // dPhi/dPhi
  Jac[13] = P3*P[32]-P4*P[31];                     // dPhi/dThe
  Jac[14] = P3*P[39]-P4*P[38];                     // dPhi/dCM
  Jac[15] = C*P[12];                               // dThe/dL0
  Jac[16] = C*P[19];                               // dThe/dL1
  Jac[17] = C*P[26];                               // dThe/dPhi
  Jac[18] = C*P[33];                               // dThe/dThe
  Jac[19] = C*P[40];                               // dThe/dCM
  Jac[20] = P[41];                                 // dCM /dCM
}

/////////////////////////////////////////////////////////////////////////////////
// Curvilinear covariance transformation to global system coordinate 
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::transformCurvilinearToGlobal
(double* p, double* P) const 
{
  double Sf,Cf,Ce,Se; sincos(p[2],&Sf,&Cf);  sincos(p[3],&Se,&Ce);

  double Ax[3] = {-Sf   , Cf   , 0.};
  double Ay[3] = {-Cf*Ce,-Sf*Ce, Se};

  //   /dL1     |   /dL2       |    /dPhi     |    /dThe     |    /dCM     |
  //
  P[ 7]  = Ax[0]; P[14] = Ay[0]; P[21] =    0.; P[28] =    0.; P[35] =   0.; // dX /
  P[ 8]  = Ax[1]; P[15] = Ay[1]; P[22] =    0.; P[29] =    0.; P[36] =   0.; // dY /
  P[ 9]  = Ax[2]; P[16] = Ay[2]; P[23] =    0.; P[30] =    0.; P[37] =   0.; // dZ / 
  P[10]  =    0.; P[17] =    0.; P[24] =-Sf*Se; P[31] =-Ay[0]; P[38] =   0.; // dAx/
  P[11]  =    0.; P[18] =    0.; P[25] = Cf*Se; P[32] =-Ay[1]; P[39] =   0.; // dAy/
  P[12]  =    0.; P[19] =    0.; P[26] =    0.; P[33] =-Ay[2]; P[40] =   0.; // dAz/
  P[13]  =    0.; P[20] =    0.; P[27] =    0.; P[34] =    0.; P[41] =   1.; // dCM/
  P[42]  =    0.; P[43] =    0.; P[44] =    0.; 
  
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to local system coordinates
// for  Trk::TrackParameters
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToLocal
(const Trk::TrackParameters& Tp,double* Jac)
{
  const AmgVector(5)& Vp = Tp.parameters();
  double P[23];
  P[0] = Vp[0];  
  P[1] = Vp[1]; 
  P[2] = Vp[2]; 
  P[3] = Vp[3];  
  jacobianTransformCurvilinearToLocal(P,&Tp.associatedSurface(),Jac);
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to local system coordinates
// for Trk::PatternTrackParameters
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToLocal
      (const Trk::PatternTrackParameters& Tp,double* Jac)
{
  double P[23];
  P[0] = Tp.par()[0];
  P[1] = Tp.par()[1];
  P[2] = Tp.par()[2];
  P[3] = Tp.par()[3];
  jacobianTransformCurvilinearToLocal(P,Tp.associatedSurface(),Jac);
} 

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to local system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToLocal
(double* P, const Trk::Surface* su,double* Jac)
{
  // Common for all surfaces terms of jacobian
  //
  Jac[ 4] = 0.;   // dL0/dCM
  Jac[ 9] = 0.;   // dL1/dCM
  Jac[10] = 0.;   // dPhi/dL0
  Jac[11] = 0.;   // dPhi/dL1
  Jac[12] = 1.;   // dPhi/dPhi
  Jac[13] = 0.;   // dPhi/dThe
  Jac[14] = 0.;   // dPhi/dCM
  Jac[15] = 0.;   // dThe/dL0
  Jac[16] = 0.;   // dThe/dL1
  Jac[17] = 0.;   // dThe/dPhi
  Jac[18] = 1.;   // dThe/dThe
  Jac[19] = 0.;   // dThe/dCM
  Jac[20] = 1.;   // dCM /dCM

  const Amg::Transform3D& T = su->transform();

  double Sf,Cf,Ce,Se; sincos(P[2],&Sf,&Cf);  sincos(P[3],&Se,&Ce);
  
  P[ 4] =  Cf*Se; P[ 5] =  Sf*Se; P[ 6] = Ce    ;   // At   
  P[ 7] = -Sf   ; P[ 8] =  Cf   ; P[ 9] = 0.    ;   // Au
  P[10] = -Cf*Ce; P[11] = -Sf*Ce; P[12] = Se    ;   // Av
  P[13] = T(0,0); P[14] = T(1,0); P[15] = T(2,0);   // Ax 
  P[16] = T(0,1); P[17] = T(1,1); P[18] = T(2,1);   // Ay 
  P[19] = T(0,2); P[20] = T(1,2); P[21] = T(2,2);   // Az 

  unsigned int ty = su->type();
  

  if(ty == Trk::Surface::Plane   )  {jacobianTransformCurvilinearToPlane       (P,Jac); return;}
  if(ty == Trk::Surface::Line    )  {jacobianTransformCurvilinearToStraightLine(P,Jac); return;}
  if(ty == Trk::Surface::Cylinder)  {
    P[22] = static_cast<const Trk::CylinderSurface*>(su)->bounds().r(); 
                                     jacobianTransformCurvilinearToCylinder    (P,Jac); return;
  }
  if(ty == Trk::Surface::Perigee )  {jacobianTransformCurvilinearToStraightLine(P,Jac); return;}
  if(ty == Trk::Surface::Disc    )  {jacobianTransformCurvilinearToDisc        (P,Jac); return;}
  Jac[0] = Jac[3] = 1.;
  Jac[1] = Jac[2] = 0.;
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Plane system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToPlane
(double* P,double* Jac) const 
{
  double* At = &P[ 4];
  double* Au = &P[ 7];
  double* Av = &P[10];
  double* Ax = &P[13];
  double* Ay = &P[16];
  double* S  = &P[19]; 

  double    A = At[0]*S[0]+At[1]*S[1]+At[2]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  double s1 = Au[0]*S[0]+Au[1]*S[1];
  double s2 = Av[0]*S[0]+Av[1]*S[1]+Av[2]*S[2];

  Au[0]-=(s1*At[0]); Au[1]-=(s1*At[1]); Au[2]-=(s1*At[2]); 
  Av[0]-=(s2*At[0]); Av[1]-=(s2*At[1]); Av[2]-=(s2*At[2]); 

  Jac[ 0] = Ax[0]*Au[0]+Ax[1]*Au[1]+Ax[2]*Au[2]; // dL0/dL0
  Jac[ 1] = Ax[0]*Av[0]+Ax[1]*Av[1]+Ax[2]*Av[2]; // dL0/dL1
  Jac[ 2] = 0.                                 ; // dL0/dPhi
  Jac[ 3] = 0.                                 ; // dL0/dThe
  Jac[ 5] = Ay[0]*Au[0]+Ay[1]*Au[1]+Ay[2]*Au[2]; // dL1/dL0
  Jac[ 6] = Ay[0]*Av[0]+Ay[1]*Av[1]+Ay[2]*Av[2]; // dL1/dL1
  Jac[ 7] = 0.                                 ; // dL1/dPhi
  Jac[ 8] = 0.                                 ; // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Disc system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToDisc
(double* P,double* Jac) const 
{
  double* p  = &P[ 0];
  double* At = &P[ 4];
  double* Au = &P[ 7];
  double* Av = &P[10];
  double* Ax = &P[13];
  double* Ay = &P[16];
  double* S  = &P[19]; 
  
  // Condition trajectory on surface
  //
  double    A = At[0]*S[0]+At[1]*S[1]+At[2]*S[2];
  if(A!=0.) A=1./A;
  S[0]*=A; S[1]*=A; S[2]*=A;

  double s1 = Au[0]*S[0]+Au[1]*S[1];
  double s2 = Av[0]*S[0]+Av[1]*S[1]+Av[2]*S[2];

  Au[0]-=(s1*At[0]); Au[1]-=(s1*At[1]); Au[2]-=(s1*At[2]); 
  Av[0]-=(s2*At[0]); Av[1]-=(s2*At[1]); Av[2]-=(s2*At[2]); 
 
  // Jacobian production
  //
  double Sf,Cf; sincos(p[1],&Sf,&Cf);

  double Ri = 1./p[0]              ;
  double A0 =(Cf*Ax[0]+Sf*Ay[0])   ;
  double A1 =(Cf*Ax[1]+Sf*Ay[1])   ;
  double A2 =(Cf*Ax[2]+Sf*Ay[2])   ;
  double B0 =(Cf*Ay[0]-Sf*Ax[0])*Ri;
  double B1 =(Cf*Ay[1]-Sf*Ax[1])*Ri;
  double B2 =(Cf*Ay[2]-Sf*Ax[2])*Ri;

  Jac[ 0] = A0*Au[0]+A1*Au[1]+A2*Au[2];   // dL0/dL0
  Jac[ 1] = A0*Av[0]+A1*Av[1]+A2*Av[2];   // dL0/dL1
  Jac[ 2] = 0.                        ;   // dL0/dPhi
  Jac[ 3] = 0.                        ;   // dL0/dThe
  Jac[ 5] = B0*Au[0]+B1*Au[1]+B2*Au[2];   // dL1/dL0
  Jac[ 6] = B0*Av[0]+B1*Av[1]+B2*Av[2];   // dL1/dL1
  Jac[ 7] = 0.                        ;   // dL1/dPhi
  Jac[ 8] = 0.                        ;   // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Cylinder system coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToCylinder
(double* P,double* Jac) const 
{
  double* p  = &P[ 0];
  double* At = &P[ 4];
  double* Au = &P[ 7];
  double* Av = &P[10];
  double* Ax = &P[13];
  double* Ay = &P[16];
  double* Az = &P[19]; 
  double  R  =  P[22];
  
  double fr  = p[0]/R;
  double Sf,Cf; sincos(fr,&Sf,&Cf);

  double x   = Cf*Ax[0]+Sf*Ay[0];
  double y   = Cf*Ax[1]+Sf*Ay[1];
  double z   = Cf*Ax[2]+Sf*Ay[2];

  // Condition trajectory on surface
  //
  double C  = At[0]*Az[0]+At[1]*Az[1]+At[2]*Az[2];
  double ax = At[0]-Az[0]*C;
  double ay = At[1]-Az[1]*C;
  double az = At[2]-Az[2]*C;
  double A  =(ax*x+ay*y+az*z); if(A!=0.) A=1./A;

  double s1 = (Au[0]*x+Au[1]*y        )*A; 
  double s2 = (Av[0]*x+Av[1]*y+Av[2]*z)*A; 

  Au[0]-=(s1*At[0]); Au[1]-=(s1*At[1]); Au[2]-=(s1*At[2]); 
  Av[0]-=(s2*At[0]); Av[1]-=(s2*At[1]); Av[2]-=(s2*At[2]); 

  // Jacobian production
  //
  double A0 =(Cf*Ay[0]-Sf*Ax[0]);
  double A1 =(Cf*Ay[1]-Sf*Ax[1]);
  double A2 =(Cf*Ay[2]-Sf*Ax[2]);

  Jac[ 0] = A0*Au[0]+A1*Au[1]+A2*Au[2]         ;  // dL0/dL0
  Jac[ 1] = A0*Av[0]+A1*Av[1]+A2*Av[2]         ;  // dL0/dL1
  Jac[ 2] = 0.                                 ;  // dL0/dPhi
  Jac[ 3] = 0.                                 ;  // dL0/dThe
  Jac[ 5] = Az[0]*Au[0]+Az[1]*Au[1]+Az[2]*Au[2];  // dL1/dL0
  Jac[ 6] = Az[0]*Av[0]+Az[1]*Av[1]+Az[2]*Av[2];  // dL1/dL1 
  Jac[ 7] = 0.                                 ;  // dL1/dPhi
  Jac[ 8] = 0.                                 ;  // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Jacobian of transformations from curvilinear to Straight Line Ssystem coordinates
/////////////////////////////////////////////////////////////////////////////////

void Trk::RungeKuttaUtils::jacobianTransformCurvilinearToStraightLine
(double* P,double* Jac) const 
{
  double* p  = &P[ 0];
  double* At = &P[ 4];
  double* Au = &P[ 7];
  double* Av = &P[10];
  double* A  = &P[19]; 

  double Bx = A[1]*At[2]-A[2]*At[1];
  double By = A[2]*At[0]-A[0]*At[2];
  double Bz = A[0]*At[1]-A[1]*At[0];
  double Bn = 1./sqrt(Bx*Bx+By*By+Bz*Bz); Bx*=Bn; By*=Bn; Bz*=Bn;
  
  // Condition trajectory on surface
  //
  double d  = At[0]*A[0]+At[1]*A[1]+At[2]*A[2]; 
  double a  = (1.-d)*(1.+d); if(a!=0.) a=1./a;
  double X  = d*A[0]-At[0], Y = d*A[1]-At[1], Z = d*A[2]-At[2];

  double s1 =       (Au[0]*X+Au[1]*Y          )*a;
  double s2 =       (Av[0]*X+Av[1]*Y+Av[2]*Z  )*a;
  double s3 = p[0]*(Bx*At[1]-By*At[0]         )*a;
  double s4 = p[0]*(Bx*Av[0]+By*Av[1]+Bz*Av[2])*a;

  // Jacobian production
  //
  Jac[ 0] = Bx*Au[0]+By*Au[1]         ;             // dL0/dL0
  Jac[ 1] = Bx*Av[0]+By*Av[1]+Bz*Av[2];             // dL0/dL1
  Jac[ 2] = 0.;                                     // dL0/dPhi
  Jac[ 3] = 0.;                                     // dL0/dThe
  Jac[ 5] = A[0]*Au[0]+A[1]*Au[1]            +s1*d; // dL1/dL0
  Jac[ 6] =(A[0]*Av[0]+A[1]*Av[1]+A[2]*Av[2])+s2*d; // dL1/dL1
  Jac[ 7] = s3*d;                                   // dL1/dPhi
  Jac[ 8] = s4*d;                                   // dL1/dThe
}

/////////////////////////////////////////////////////////////////////////////////
// Fill distances map (initial map<distance,N of SU> preparation
// Output
// Step[0] - min negative step
// Step[1] - min positive step
// Step[2] - max distance 
// int     - vector SU element number with initial surface 
/////////////////////////////////////////////////////////////////////////////////

int Trk::RungeKuttaUtils::fillDistancesMap
(std::vector<std::pair<const Trk::Surface*,Trk::BoundaryCheck> >& SU,
 std::multimap<double,int>& DN,
 const double* Pinp,double W,const Trk::Surface* So,double* Step) const 
{
  int Ns = -1;
  DN.erase(DN.begin(),DN.end()); 
  
  Amg::Vector3D pos(Pinp[0],Pinp[1],Pinp[2]);
  Amg::Vector3D dir(Pinp[3],Pinp[4],Pinp[5]);

  Step[0] = -1.e+20;
  Step[1] =  1.e+20;
  Step[2] =      0.;

  int N = SU.size(); W=fabs(W); 

  for(int i=0; i!=N; ++i ) {
    
    const Trk::Surface* s = SU[i].first; if(!s) continue; if(s==So) Ns = i;
    Trk::DistanceSolution ds = s->straightLineDistanceEstimate(pos,dir,SU[i].second);
    double dist = ds.currentDistance(false);
    DN.insert(std::make_pair(dist+W,i));

    if(dist > Step[2]) Step[2] = dist;

    int  n = ds.numberOfSolutions(); if(!n) continue;
    for(int i=0; i!=n; ++i) {
      
      double st; i == 0 ? st=ds.first() : st=ds.second();

      if(s==So && fabs(st)<=.001) continue;
      
      if(st < 0.) {if(st > Step[0]) Step[0] = st;}
      else        {if(st < Step[1]) Step[1] = st;}
    }
  }
  return Ns;
}
