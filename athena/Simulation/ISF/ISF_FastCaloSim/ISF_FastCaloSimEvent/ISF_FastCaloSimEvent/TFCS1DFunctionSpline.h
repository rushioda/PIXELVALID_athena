/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS1DFunctionSpline_h
#define ISF_FASTCALOSIMEVENT_TFCS1DFunctionSpline_h

#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include <TSpline.h>
#include <vector>

class TH1;
class TFCS1DFunctionInt32Histogram;

class TFCS1DFunctionSpline:public TFCS1DFunction
{
  public:
    TFCS1DFunctionSpline(TH1* hist=nullptr,double maxdevgoal=0.01,double maxeffsiggoal=3,int maxnp=20) {if(hist) Initialize(hist,maxdevgoal,maxeffsiggoal,maxnp);};
    ~TFCS1DFunctionSpline() {};

    static double get_maxdev(const TH1* hist,const TSpline3& sp,double& maxeffsig,double& p_maxdev,double& p_maxeffsig,int ntoy=10000);

    virtual double Initialize(TH1* hist,double maxdevgoal=0.01,double maxeffsiggoal=3,int maxnp=20);

    virtual double InitializeFromSpline(TH1* hist,const TSpline3& sp,double maxdevgoal=0.01,double maxeffsiggoal=3);
    virtual double InitializeEqualDistance(TH1* hist,double maxdevgoal=0.01,double maxeffsiggoal=3,int nsplinepoints=5);
    virtual double InitializeEqualProbability(TH1* hist,double maxdevgoal=0.01,double maxeffsiggoal=3,int nsplinepoints=5);

    using TFCS1DFunction::rnd_to_fct;
    
    ///Function gets random number rnd in the range [0,1) as argument 
    ///and returns function value according to a histogram distribution
    virtual double rnd_to_fct(double rnd) const;

    const TSpline3& spline() const {return m_spline;};
    TSpline3& spline() {return m_spline;};

    static void unit_test(TH1* hist=nullptr);
  protected:
    double optimize(TSpline3& sp_best,std::vector<double>& nprop,const TH1* hist,TFCS1DFunctionInt32Histogram& hist_fct,double maxdevgoal=0.01,double maxeffsiggoal=3);

    TSpline3 m_spline;    

  private:

  ClassDef(TFCS1DFunctionSpline,1)  //TFCS1DFunctionSpline
};

#if defined(__ROOTCLING__) && defined(__FastCaloSimStandAlone__)
#pragma link C++ class TFCS1DFunctionSpline+;
#endif

#endif
