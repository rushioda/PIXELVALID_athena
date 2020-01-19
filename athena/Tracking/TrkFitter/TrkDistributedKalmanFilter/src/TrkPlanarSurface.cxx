/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// TrkPlanarSurface.cpp
//   Implementation of class TrkPlanarSurface
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dmitry Emeliyanov, RAL
// D.Emeliyanov@rl.ac.uk
///////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "TrkSurfaces/Surface.h"
#include"TrkDistributedKalmanFilter/TrkPlanarSurface.h"

namespace Trk
{

  TrkPlanarSurface::TrkPlanarSurface(double c[3],double n[3],double m[3][3], double rl, const Surface* pS):
    m_radLength(rl), 
    m_input{},
    m_trkSurface(pS)
  {
    int i,j;
    m_Params[3]=0.0;m_Rc=0.0;
    for(i=0;i<3;i++){
			m_Center[i]=c[i];
			m_Normal[i]=n[i];
			m_Params[i]=n[i];
			m_Params[3]-=n[i]*c[i];
			m_Rc+=c[i]*c[i];
    }
    for(i=0;i<3;i++){
	    for(j=0;j<3;j++)
	      m_L[i][j]=m[i][j];
    }
    m_Rc=sqrt(m_Rc);
    m_calculateInverseTransform();
    m_breakPoint=false;
  }

  const Surface* TrkPlanarSurface::m_getTrkSurface()
  {
    return m_trkSurface;
  }

  void TrkPlanarSurface::m_calculateInverseTransform()
  {
    m_M[0][0]= m_L[1][1]*m_L[2][2]-m_L[1][2]*m_L[2][1];
    m_M[1][0]=-m_L[1][0]*m_L[2][2]+m_L[1][2]*m_L[2][0];
    m_M[2][0]= m_L[1][0]*m_L[2][1]-m_L[1][1]*m_L[2][0];
    m_M[0][1]=-m_L[0][1]*m_L[2][2]+m_L[0][2]*m_L[2][1];
    m_M[1][1]= m_L[0][0]*m_L[2][2]-m_L[0][2]*m_L[2][0];
    m_M[2][1]=-m_L[0][0]*m_L[2][1]+m_L[0][1]*m_L[2][0];
    m_M[0][2]= m_L[0][1]*m_L[1][2]-m_L[0][2]*m_L[1][1];
    m_M[1][2]=-m_L[0][0]*m_L[1][2]+m_L[0][2]*m_L[1][0];
    m_M[2][2]= m_L[0][0]*m_L[1][1]-m_L[0][1]*m_L[1][0];
  }
  TrkPlanarSurface::~TrkPlanarSurface()
  {
    
  }
  const double* TrkPlanarSurface::m_getCenter()
  {
    return &m_Center[0];
  }
  double TrkPlanarSurface::m_radiusVector()
  {
    return m_Rc;
  }
  const double* TrkPlanarSurface::m_getNormal()
  {
    return &m_Normal[0];
  }
  const double* TrkPlanarSurface::m_getParameters()
  {
    return &m_Params[0];
  }
  double TrkPlanarSurface::m_getPar(int i)
  {
    return m_Params[i];
  }
  double TrkPlanarSurface::m_getRotMatrix(int i, int j)
  {
    return m_M[i][j];
  }
  double TrkPlanarSurface::m_getInvRotMatrix(int i, int j)
  {
    return m_L[i][j];
  }
  double TrkPlanarSurface::m_calculateCombinedMatrix(int i,int j,TrkPlanarSurface* pS)
  {
    double rc=0;
    for(int m=0;m<3;m++) rc+=pS->m_M[i][m]*m_L[m][j];
    return rc;
  }
  void TrkPlanarSurface::m_rotateVectorToLocal(double* X, double* Y)
  {
    int i;
    for(i=0;i<3;i++)
      Y[i]=m_M[i][0]*X[0]+m_M[i][1]*X[1]+m_M[i][2]*X[2];
  }
  void TrkPlanarSurface::m_rotateVectorToGlobal(double* X, double* Y)
  {
    int i;
    for(i=0;i<3;i++)
      Y[i]=m_L[i][0]*X[0]+m_L[i][1]*X[1]+m_L[i][2]*X[2];
  }
  void TrkPlanarSurface::m_transformPointToLocal(double* X, double* Y)
  {
    int i;
    for(i=0;i<3;i++)
      Y[i]=m_M[i][0]*(X[0]-m_Center[0])+
	m_M[i][1]*(X[1]-m_Center[1])+
	m_M[i][2]*(X[2]-m_Center[2]);
  }
  void TrkPlanarSurface::m_transformPointToGlobal(double* X, double* Y)
  {
    int i;
    for(i=0;i<3;i++)
      Y[i]=m_Center[i]+m_L[i][0]*X[0]+m_L[i][1]*X[1]+m_L[i][2]*X[2];
  }
  double TrkPlanarSurface::m_getRadLength()
  {
    //return 0.022;
    return m_radLength;
  }

  void TrkPlanarSurface::m_setBreakPoint(double u)
  {
    m_breakPoint=true;
    m_input=u;
  }

  bool TrkPlanarSurface::m_isBreakPoint()
  {
    return m_breakPoint;
  }

  double TrkPlanarSurface::m_getInput()
  {
    return m_input;
  }

  void TrkPlanarSurface::m_report()
  {
    printf("SURFACE: CENTER x=%f y=%f z=%f R=%f\n",m_Center[0],m_Center[1],m_Center[2],m_Rc);
    printf("         NORMAL nx=%f ny=%f nz=%f\n",m_Normal[0],m_Normal[1],m_Normal[2]);
    printf("EQUATION: (%f)x+(%f)y+(%f)z+(%f)=0\n",m_Params[0],m_Params[1],m_Params[2],m_Params[3]);
    for(int i=0;i<3;i++)
      {
	for(int j=0;j<3;j++) printf("%f ",m_M[i][j]);
	printf("\n");
      }
  }
}

