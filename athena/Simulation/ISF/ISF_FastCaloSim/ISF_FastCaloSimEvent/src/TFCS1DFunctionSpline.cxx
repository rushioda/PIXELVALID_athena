/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCS1DFunctionSpline.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunctionInt32Histogram.h"
#include <algorithm>
#include <iostream>
#include "TMath.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TFile.h"

//=============================================
//======= TFCS1DFunctionSpline =========
//=============================================

double TFCS1DFunctionSpline::Initialize(TH1* hist,double maxdevgoal,double maxeffsiggoal,int maxnp)
{
  double max_penalty_best=-1;
  TSpline3 sp_best;
  for(int np=3;np<=maxnp;++np) {
    std::cout<<"========== Spline #="<<np<<" =============="<<std::endl;
    double max_penalty;
    if(max_penalty_best>0) {
      max_penalty=InitializeFromSpline(hist,sp_best,maxdevgoal,maxeffsiggoal);
      if(max_penalty_best<0 || max_penalty<max_penalty_best) {
        max_penalty_best=max_penalty;
        sp_best=m_spline;
      }
    }

    max_penalty=InitializeEqualDistance(hist,maxdevgoal,maxeffsiggoal,np);
    if(max_penalty_best<0 || max_penalty<max_penalty_best) {
      max_penalty_best=max_penalty;
      sp_best=m_spline;
    }

    max_penalty=InitializeEqualProbability(hist,maxdevgoal,maxeffsiggoal,np);
    if(max_penalty_best<0 || max_penalty<max_penalty_best) {
      max_penalty_best=max_penalty;
      sp_best=m_spline;
    }

    std::cout<<"========== Spline #="<<np<<" max_penalty_best="<<max_penalty_best<<" =============="<<std::endl;
    std::cout<<"==== Best spline init | ";
    for(int i=0;i<sp_best.GetNp();++i) {
      double p,x;
      sp_best.GetKnot(i,p,x);
      std::cout<<i<<" : p="<<p<<" x="<<x<<" ; ";
    }
    std::cout<<" ====="<<std::endl;

    if(max_penalty_best<2) break;
  }
  m_spline=sp_best;

  return max_penalty_best;
}

double TFCS1DFunctionSpline::InitializeFromSpline(TH1* hist,const TSpline3& sp,double maxdevgoal,double maxeffsiggoal)
{
  TFCS1DFunctionInt32Histogram hist_fct(hist);

  double maxeffsig;
  double p_maxdev;
  double p_maxeffsig;
  double maxdev=get_maxdev(hist,sp,maxeffsig,p_maxdev,p_maxeffsig);
  double p_improve;
  if(maxdev/maxdevgoal > maxeffsig/maxeffsiggoal) p_improve=p_maxdev;
   else p_improve=p_maxeffsig;

  int nsplinepoints=sp.GetNp();
  std::vector<double> nprop(nsplinepoints+1);
  int ind=0;
  std::cout<<"Spline init p_improve="<<p_improve<<" | ";
  for(int i=0;i<nsplinepoints;++i) {
    double p,x;
    sp.GetKnot(i,p,x);
    if(i==0 && p_improve<p) {
      nprop[ind]=(0+p)/2;
      std::cout<<ind<<" : pi="<<nprop[ind]<<" ; ";
      ++ind;
    }

    nprop[ind]=p;
    std::cout<<ind<<" : p="<<nprop[ind]<<" ; ";
    ++ind;

    if(i==nsplinepoints-1 && p_improve>p) {
      nprop[ind]=(1+p)/2;
      std::cout<<ind<<" : pi="<<nprop[ind]<<" ; ";
      ++ind;
    }
    if(i<nsplinepoints-1) {
      double pn,xn;
      sp.GetKnot(i+1,pn,xn);
      if(p_improve>p && p_improve<pn) {
        nprop[ind]=(p+pn)/2;
        std::cout<<ind<<" : pi="<<nprop[ind]<<" ; ";
        ++ind;
      }
    } 
  }
  std::cout<<std::endl;
  nsplinepoints=ind;
  nprop.resize(nsplinepoints);

  double max_penalty=optimize(m_spline,nprop,hist,hist_fct,maxdevgoal,maxeffsiggoal);
  maxdev=get_maxdev(hist,m_spline,maxeffsig,p_maxdev,p_maxeffsig);
  std::cout<<"Spline init spline #="<<nsplinepoints<<" : maxdev="<<maxdev<<" p_maxdev="<<p_maxdev<<" maxeffsig="<<maxeffsig<<" p_maxeffsig="<<p_maxeffsig<<" max_penalty="<<max_penalty<<std::endl;
  std::cout<<"  ";
  for(int i=0;i<m_spline.GetNp();++i) {
    double p,x;
    m_spline.GetKnot(i,p,x);
    std::cout<<i<<" : p="<<p<<" x="<<x<<" ; ";
  }
  std::cout<<std::endl;
  return max_penalty;
}

double TFCS1DFunctionSpline::InitializeEqualDistance(TH1* hist,double maxdevgoal,double maxeffsiggoal,int nsplinepoints)
{
  TFCS1DFunctionInt32Histogram hist_fct(hist);

  double xmin=0;
  double xmax=0;
  for(int i=1;i<=hist->GetNbinsX();i++) {
    xmin=hist->GetXaxis()->GetBinLowEdge(i);
    if(hist->GetBinContent(i)>0) break;
  }
  for(int i=hist->GetNbinsX();i>=1;i--) {
    xmax=hist->GetXaxis()->GetBinUpEdge(i);
    if(hist->GetBinContent(i)>0) break;
  }
  //std::cout<<"xmin="<<xmin<<" xmax="<<xmax<<std::endl;
  
  double dx=(xmax-xmin)/(nsplinepoints-1);

  std::vector<double> nprop(nsplinepoints);
  std::vector<double> nx(nsplinepoints);
  nprop[0]=0;
  nx[0]=hist_fct.rnd_to_fct(nprop[0]);
  //std::cout<<0<<" p="<<nprop[0]<<" x="<<nx[0]<<std::endl;
  for(int i=1;i<nsplinepoints;++i) {
    nx[i]=xmin+i*dx;
    double p_min=0;
    double p_max=1;
    double p_test;
    double tx;
    do {
      p_test=0.5*(p_min+p_max);
      tx=hist_fct.rnd_to_fct(p_test);
      if(nx[i]<tx) p_max=p_test;
       else p_min=p_test;
      if((p_max-p_min)<0.0000001) break;
    } while (TMath::Abs(tx-nx[i])>dx/10);
    //std::cout<<i<<" p="<<p_test<<" x="<<tx<<std::endl;
    nprop[i]=p_test;
  }

  double max_penalty=optimize(m_spline,nprop,hist,hist_fct,maxdevgoal,maxeffsiggoal);
  double maxeffsig;
  double p_maxdev;
  double p_maxeffsig;
  double maxdev=get_maxdev(hist,m_spline,maxeffsig,p_maxdev,p_maxeffsig);
  std::cout<<"Spline init equ. dist. #="<<nsplinepoints<<" : maxdev="<<maxdev<<" p_maxdev="<<p_maxdev<<" maxeffsig="<<maxeffsig<<" p_maxeffsig="<<p_maxeffsig<<" max_penalty="<<max_penalty<<std::endl;
  std::cout<<"  ";
  for(int i=0;i<m_spline.GetNp();++i) {
    double p,x;
    m_spline.GetKnot(i,p,x);
    std::cout<<i<<" : p="<<p<<" x="<<x<<" ; ";
  }
  std::cout<<std::endl;
  return max_penalty;
}

double TFCS1DFunctionSpline::InitializeEqualProbability(TH1* hist,double maxdevgoal,double maxeffsiggoal,int nsplinepoints)
{
  TFCS1DFunctionInt32Histogram hist_fct(hist);

  double dprop=1.0/(nsplinepoints-1);
  std::vector<double> nprop(nsplinepoints);
  for(int i=0;i<nsplinepoints;++i) {
    nprop[i]=i*dprop;
  }

  double max_penalty=optimize(m_spline,nprop,hist,hist_fct,maxdevgoal,maxeffsiggoal);
  double maxeffsig;
  double p_maxdev;
  double p_maxeffsig;
  double maxdev=get_maxdev(hist,m_spline,maxeffsig,p_maxdev,p_maxeffsig);
  std::cout<<"Spline init equ. prob. #="<<nsplinepoints<<" : maxdev="<<maxdev<<" p_maxdev="<<p_maxdev<<" maxeffsig="<<maxeffsig<<" p_maxeffsig="<<p_maxeffsig<<" max_penalty="<<max_penalty<<std::endl;
  std::cout<<"  ";
  for(int i=0;i<m_spline.GetNp();++i) {
    double p,x;
    m_spline.GetKnot(i,p,x);
    std::cout<<i<<" : p="<<p<<" x="<<x<<" ; ";
  }
  std::cout<<std::endl;
  return max_penalty;
}

double TFCS1DFunctionSpline::optimize(TSpline3& sp_best,std::vector<double>& nprop,const TH1* hist,TFCS1DFunctionInt32Histogram& hist_fct,double maxdevgoal,double maxeffsiggoal)
{
  int nsplinepoints=(int)nprop.size();
  //double xmin=hist->GetXaxis()->GetXmin();
  //double xmax=hist->GetXaxis()->GetXmax();
  std::vector<double> nx(nsplinepoints);
  std::vector<double> nprop_try=nprop;
  double max_penalty=-1;
  double p_gotobest=0.5/nsplinepoints;
  int ntry=200/p_gotobest;
  int itrytot=0;
  for(double dproploop=0.4/nsplinepoints;dproploop>0.02/nsplinepoints;dproploop/=2) {
    double dprop=dproploop;
    int n_nogain=0;
    for(int itry=0;itry<ntry;++itry) {
      itrytot++;
      for(int i=0;i<nsplinepoints;++i) {
        nx[i]=hist_fct.rnd_to_fct(nprop_try[i]);
      }
      TSpline3 sp("1Dspline", nprop_try.data(), nx.data(), nsplinepoints, "b2e2", 0, 0);
      double maxeffsig;
      double p_maxdev;
      double p_maxeffsig;
      double maxdev=get_maxdev(hist,sp,maxeffsig,p_maxdev,p_maxeffsig);
      double penalty=maxdev/maxdevgoal+maxeffsig/maxeffsiggoal;
      if(max_penalty<0 || penalty<max_penalty) {
        max_penalty=penalty;
        nprop=nprop_try;
        sp_best=sp;
        /*
        std::cout<<"#="<<nsplinepoints<<" try="<<itrytot-1<<" | ";
        for(int i=0;i<nsplinepoints;++i) {
          std::cout<<i<<":p="<<nprop_try[i]<<" x="<<nx[i]<<" ; ";
        }
        std::cout<<"new maxdev="<<maxdev<<" maxeffsig="<<maxeffsig<<" max_penalty="<<max_penalty<<std::endl;
        */
        n_nogain=0;
      } else {
        if(gRandom->Rndm()<p_gotobest) nprop_try=nprop;
        ++n_nogain;
        //allow ~20 times retrying from the best found spline before aborting
        if(n_nogain>20/p_gotobest) break;
      }
      int ip=1+gRandom->Rndm()*(nsplinepoints-1);
      if(ip>nsplinepoints-1) ip=nsplinepoints-1;
      double d=dprop;
      if(gRandom->Rndm()>0.5) d=-dprop;
      double nprop_new=nprop_try[ip]+d;
      if(ip>0) if(nprop_try[ip-1]+dprop/2 > nprop_new) {
        nprop_new=nprop_try[ip];
        dprop/=2;
      }
      if(ip<nsplinepoints-1) if(nprop_new > nprop_try[ip+1]-dprop/2) {
        nprop_new=nprop_try[ip];
        dprop/=2;
      }
      if(nprop_new<0) {
        nprop_new=0;
        dprop/=2;
      }
      if(nprop_new>1) {
        nprop_new=1;
        dprop/=2;
      }
      nprop_try[ip]=nprop_new;
    }
    nprop_try=nprop;
  }
  return max_penalty;
}

double TFCS1DFunctionSpline::get_maxdev(const TH1* hist,const TSpline3& sp,double& maxeffsig,double& p_maxdev,double& p_maxeffsig,int ntoy)
{
  double maxdev=0;
  maxeffsig=0;

  TH1* hist_clone=(TH1*)hist->Clone("hist_clone");
  hist_clone->SetDirectory(0);
  hist_clone->Reset();
  double interr=0;
  double integral=hist->IntegralAndError(1,hist->GetNbinsX(),interr);
  double effN=integral/interr;
  effN*=effN;
  //std::cout<<"integral="<<integral<<" +- "<<interr<<" relerr="<<interr/integral<<std::endl;
  //std::cout<<"effN="<<effN<<" +- "<<TMath::Sqrt(effN)<<" relerr="<<1/TMath::Sqrt(effN)<<std::endl;
  double toyweight=1.0/ntoy;
  for(int itoy=0;itoy<ntoy;++itoy) {
    double prop=itoy*toyweight;
    hist_clone->Fill(sp.Eval(prop),toyweight);
  }
  
  double int1=0;
  double int2=0;
  for(int i=0;i<=hist->GetNbinsX()+1;i++) {
    int1+=hist->GetBinContent(i)/integral;
    int2+=hist_clone->GetBinContent(i);
    double val=TMath::Abs(int1-int2);
    if(val>maxdev) {
      maxdev=val;
      p_maxdev=int1;
    }  

    //now view the normalized integral as selection efficiency from a total sample of sizze effN
    double int1err=TMath::Sqrt(int1*(1-int1)/effN);
    double valsig=0;
    if(int1err>0) valsig=val/int1err;
    if(valsig>maxeffsig) {
      maxeffsig=valsig;
      p_maxeffsig=int1;
    }

    //std::cout<<i<<": diff="<<int1-int2<<" sig(diff)="<<valsig<<" int1="<<int1<<" +- "<<int1err<<" int2="<<int2<<" maxdev="<<maxdev<<" maxeffsig="<<maxeffsig<<std::endl;
  }

  delete hist_clone;

  return maxdev; 
}

double TFCS1DFunctionSpline::rnd_to_fct(double rnd) const
{
  return m_spline.Eval(rnd);
}

void TFCS1DFunctionSpline::unit_test(TH1* hist)
{
  int nbinsx;
  TH1* histfine=0;
  if(hist==nullptr) {
    nbinsx=50;
    double xmin=1;
    double xpeak=1.5;
    double sigma=0.6;
    double xmax=5;
    hist=new TH1D("test1D","test1D",nbinsx,xmin,xmax);
    histfine=new TH1D("test1Dfine","test1Dfine",10*nbinsx,xmin,xmax);
    hist->Sumw2();
    for(int i=1;i<=100000;++i) {
      double x=gRandom->Gaus(xpeak,sigma);
      if(x>=xmin && x<xmax) {
        //hist->Fill(TMath::Sqrt(x));
        hist->Fill(x);
        if(histfine) histfine->Fill(x,10);
      }  
    }
  }
  if(!histfine) histfine=hist;
  TFCS1DFunctionSpline rtof(histfine,0.01,2,15);
  nbinsx=hist->GetNbinsX();
  
  float value[2];
  float rnd[2];
  for(rnd[0]=0;rnd[0]<0.9999;rnd[0]+=0.25) {
      rtof.rnd_to_fct(value,rnd);
      std::cout<<"rnd0="<<rnd[0]<<" -> x="<<value[0]<<std::endl;
  }

  TH1* hist_val=(TH1*)histfine->Clone("hist_val");
  hist_val->SetTitle("toy simulation");
  hist_val->Reset();
  hist_val->SetLineColor(2);
  TH1* hist_diff=(TH1*)hist->Clone("hist_diff");
  hist_diff->SetTitle("difference");
  hist_diff->Reset();
  int nrnd=5000000;
  double weight=histfine->Integral()/nrnd;
  double weightdiff=hist->Integral()/nrnd;
  hist_val->Sumw2();
  hist_diff->Sumw2();
  for(int i=0;i<nrnd;++i) {
    rnd[0]=gRandom->Rndm();
    rtof.rnd_to_fct(value,rnd);
    hist_val->Fill(value[0],weight);
    hist_diff->Fill(value[0],weightdiff);
  } 
  hist_diff->Add(hist,-1);

  TH1F* hist_pull=new TH1F("pull","pull",200,-10,10);
  for(int ix=1;ix<=nbinsx;++ix) {
    float val=hist_diff->GetBinContent(ix);
    float err=hist_diff->GetBinError(ix);
    if(err>0) hist_pull->Fill(val/err);
    //std::cout<<"val="<<val<<" err="<<err<<std::endl;
  }
  
//Screen output in athena won't make sense and would require linking of additional libraries
#if defined(__FastCaloSimStandAlone__)
  new TCanvas("input","Input");
  histfine->SetLineColor(kGray);
  histfine->Draw("hist");
  hist->Draw("same");
  hist_val->Draw("sameshist");

  new TCanvas("spline","spline");
  TFCS1DFunctionInt32Histogram hist_fct(hist);
  int ngr=101;
  TGraph* gr=new TGraph();
  for(int i=0;i<ngr;++i) {
    double r=i*1.0/(ngr-1);
    gr->SetPoint(i,r,hist_fct.rnd_to_fct(r));
  }
  gr->SetMarkerStyle(7);
  gr->Draw("AP");
  TSpline3* sp=new TSpline3(rtof.spline());
  sp->SetLineColor(2);
  sp->SetMarkerColor(2);
  sp->SetMarkerStyle(2);
  sp->Draw("LPsame");
  
  new TCanvas("difference","difference");
  hist_diff->Draw();

  new TCanvas("pull","Pull");
  hist_pull->Draw();  
#endif
}
