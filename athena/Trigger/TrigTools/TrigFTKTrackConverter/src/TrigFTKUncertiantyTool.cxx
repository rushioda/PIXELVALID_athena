/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigFTKUncertiantyTool tool
// -------------------------------
// ATLAS Collaboration
//
// Store and return the parametrized uncertianties for the FTK tracks   
//
// 17.07.2013 Package created
//
// Author: John Alison, UChicago
// e-mail: John.Alison@cern.ch
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include "GaudiKernel/MsgStream.h"

#include "TrigFTKTrackConverter/TrigFTKUncertiantyTool.h"

using std::map; using std::string;
using std::cout; using std::endl; 


TrigFTKUncertiantyTool::TrigFTKUncertiantyTool(const std::string& t, 
					       const std::string& n,
					       const IInterface*  p ): 
  AthAlgTool(t,n,p),
  fNoIBL(false)
{
  declareInterface< ITrigFTKUncertiantyTool >( this );
  declareProperty( "NoIBL",  fNoIBL);
}

StatusCode TrigFTKUncertiantyTool::initialize() {

  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  //
  //   Load Constants
  //
  if(fNoIBL)
    LoadConstants_NoIBL();
  else
    LoadConstants();


  athenaLog << MSG::INFO << "TrigFTKUncertiantyTool initialized "<< endreq;
  return sc;
}

StatusCode TrigFTKUncertiantyTool::finalize() {
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

//
// Covariance Matrix if there is a BLayer Hit
//
double TrigFTKUncertiantyTool::getParamCovMtx(const FTKTrack *trk, int id0, int id1)
{
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  athenaLog << MSG::VERBOSE << "In getParamCovMtx: id0: " << id0 << " id1: " << id1 << endreq; 

  if(!trk){
    athenaLog << MSG::ERROR << "FTK Track is Null" << endreq; 
    return -99;
  }

  //
  // Use diagonal Maxtrix for now
  //
  if (id0!=id1) {
    return 0.;
  }

  bool hasBL = (trk->getBitmask()&(1<<0));
  if(outputLevel <= MSG::DEBUG)
    athenaLog << MSG::DEBUG << "TrigFTKUncertiantyTool:: has BL " << hasBL << endreq; 
  
  double trkIpt (trk->getHalfInvPt());  // NOTE: this is 1./(2xpt)
  double trkEta (trk->getEta()  );

  if(outputLevel <= MSG::VERBOSE){
    athenaLog << MSG::VERBOSE << "TrigFTKUncertiantyTool:: trkIpt " << trkIpt << endreq; 
    athenaLog << MSG::VERBOSE << "TrigFTKUncertiantyTool:: trkEta " << trkEta << endreq; 
  }

  double sigmaTP = -1.0;

  //
  //  Uncertianties are stored using eta and 1/pt
  //
  int lookUpParam = id0;

  //   if theta is asked for, fetch eta and then convert later
  if(id0 == FTKTrackParam::theta){
    lookUpParam = FTKTrackParam::eta;
  }

  //   if q/P or pt are asked for, fetch Ipt and then convert later
  if(id0 == FTKTrackParam::qOp || id0 == FTKTrackParam::pt){
    lookUpParam = FTKTrackParam::Ipt;
  }


  //
  // square root model
  //
  if(allConsts[hasBL].mode(lookUpParam,trkEta) == TrigFTKUncertiantyTool::sqroot){
    sigmaTP = sqrt(allConsts[hasBL].par0(lookUpParam,trkEta)+allConsts[hasBL].par1(lookUpParam,trkEta)*trkIpt*trkIpt);
    if(outputLevel <= MSG::DEBUG){
      athenaLog << MSG::DEBUG << "TrigFTKUncertiantyTool:: sigmaTP ("   
		<< sigmaTP <<") = sqrt("<<allConsts[hasBL].par0(lookUpParam,trkEta) 
		<< "+" << allConsts[hasBL].par1(lookUpParam,trkEta) << "*" << trkIpt << "**2)" <<  endreq; 
      athenaLog << MSG::DEBUG << "TrigFTKUncertiantyTool:: (sqrt)cov "   << sigmaTP*sigmaTP << endreq; 
    }
    
  //
  // linear model
  //
  }else{
    sigmaTP = allConsts[hasBL].par0(lookUpParam,trkEta) + allConsts[hasBL].par1(lookUpParam,trkEta)*fabs(trkIpt);
    if(outputLevel <= MSG::DEBUG){
      athenaLog << MSG::DEBUG << "TrigFTKUncertiantyTool:: sigmaTP ("   
		<< sigmaTP <<") = "<<allConsts[hasBL].par0(lookUpParam,trkEta) 
		<< "+" << allConsts[hasBL].par1(lookUpParam,trkEta) << "*" << fabs(trkIpt) <<  endreq; 
      athenaLog << MSG::DEBUG << "TrigFTKUncertiantyTool:: (linear)cov "   << sigmaTP*sigmaTP << endreq; 
    }
  }

  if(sigmaTP < 0)
    athenaLog << MSG::ERROR << " sigma TP" << sigmaTP << endreq;

  //
  // Convert eta to theta
  //
  if(id0 == FTKTrackParam::theta){
    double sigmaTheta = getSigmaTheta(trkEta, sigmaTP);
    sigmaTP    = sigmaTheta;
  }

  //
  // Convert 1/2pt  to qoverp
  //
  if(id0 == FTKTrackParam::qOp){
    double sigmaEta     = allConsts[hasBL].par0(FTKTrackParam::eta,trkEta)+allConsts[hasBL].par1(FTKTrackParam::eta,trkEta)*fabs(trkIpt);
    double sigmaQoverP  = getSigmaQoverP(trkIpt, sigmaTP, trkEta, sigmaEta );
    sigmaTP             = sigmaQoverP;
  }

  //
  // Convert 1/2pt  to pt
  //
  if(id0 == FTKTrackParam::pt){
    double sigmaPt      = getSigmaPt(trkIpt, sigmaTP);
    sigmaTP             = sigmaPt;
  }

  
  return sigmaTP*sigmaTP;

}



//
// Get sigma(Q/P) from sigma(1/2pT) and sigma(eta) 
//    (NOTE: invpt = 1/2pT)
double TrigFTKUncertiantyTool::getSigmaQoverP(double invpt, double sigmaTwoInvPt, double eta,  double sigmaEta)
{
  //
  // (Might not be necessary, but lets to it right.)
  //  
  //   We have paramaterized the uncertianty of 1/2xPt and eta
  //    (NOTE: invpt = 1/2pT)
  //
  //
  // But we want sigma q/p := "f"
  //   define sigmaTwoInvPt := "g"
  //
  //   f = 2g/cosh(eta)
  //  
  // (After calculus) 
  //
  //  sigma(f) = 2/cosh(eta) x sqrt( sigma(g)^2 + (gxtanh(eta))^2 )
  //
  
  double secondTerm = invpt*tanh(eta)*sigmaEta;
  double coshEta    = cosh(eta); 
  double sigmaQoverP = coshEta ? 2./cosh(eta) * sqrt( sigmaTwoInvPt*sigmaTwoInvPt + secondTerm*secondTerm) : 10 ;
  return sigmaQoverP;
}

//
// Get sigma(Pt) from sigma(1/2pT) 
//    (NOTE: invpt = 1/2pT)
double TrigFTKUncertiantyTool::getSigmaPt(double invpt, double sigmaTwoInvPt)
{
  //
  // (Might not be necessary, but lets to it right.)
  //  
  //   We have paramaterized the uncertianty of 1/2xPt
  //    (NOTE: invpt = 1/2pT)
  //
  //  if,    f := 1./(2xpt), then pt = 1./(2xf)
  //  sigma(pT) = 1./(2xf^2) sigma(f) 
  
  double sigmaPt = 1./(2*invpt * invpt) * sigmaTwoInvPt;
  return sigmaPt;
}



double TrigFTKUncertiantyTool::getSigmaTheta(double eta, double sigmaEta)
{
  //
  //  sigma(theta) = d(theta)/d(eta) x sigma(eta)
  //
  //  and,   d(theta)/d(eta) = 1./cosh(eta)
  //
  double coshEta    = cosh(eta); 
  double sigmaTheta = coshEta ? sigmaEta/coshEta : 10;
  return sigmaTheta;
}


//
//   Initialize the IBL constants
//
void TrigFTKUncertiantyTool::LoadConstants()
{
  //
  // has BLayer constants
  //
  TPConsts&  nomConsts =  allConsts[1];

  //
  // no BLayer constants
  //
  TPConsts&  nBLConsts =  allConsts[0];
  
  //
  //  d0
  //
  nomConsts.set(FTKTrackParam::d0,  0,  sqroot,     6.72e-04,   2.47e+04 );
  nomConsts.set(FTKTrackParam::d0,  1,  sqroot,     6.73e-04,   2.58e+04 );
  nomConsts.set(FTKTrackParam::d0,  2,  sqroot,     8.48e-04,   3.71e+04 );
  nomConsts.set(FTKTrackParam::d0,  3,  sqroot,     1.08e-03,   7.59e+04 );
  nomConsts.set(FTKTrackParam::d0,  4,  sqroot,     1.61e-03,   1.45e+05 );
 
  nBLConsts.set(FTKTrackParam::d0,  0,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  1,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  2,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  3,  sqroot,  3.14e-03,     2.11e5);
  nBLConsts.set(FTKTrackParam::d0,  4,  sqroot,  3.14e-03,     2.11e5);

  //
  //  z0 
  //
  nomConsts.set(FTKTrackParam::z0,  0,  sqroot,      9.66e-03, 7.72e+04);
  nomConsts.set(FTKTrackParam::z0,  1,  sqroot,      7.39e-03, 7.35e+04);
  nomConsts.set(FTKTrackParam::z0,  2,  sqroot,      9.94e-03, 1.73e+05);
  nomConsts.set(FTKTrackParam::z0,  3,  sqroot,      2.61e-02, 5.31e+05);
  nomConsts.set(FTKTrackParam::z0,  4,  sqroot,      7.59e-02, 3.06e+06);

  nBLConsts.set(FTKTrackParam::z0,  0,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  1,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  2,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  3,  linear,  0.19,      558);
  nBLConsts.set(FTKTrackParam::z0,  4,  linear,  0.19,      558);

  //
  //  phi
  //
  nomConsts.set(FTKTrackParam::phi,  0,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  1,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  2,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  3,  linear,  0.00054,  4.3);
  nomConsts.set(FTKTrackParam::phi,  4,  linear,  0.00054,  4.3);

  nBLConsts.set(FTKTrackParam::phi,  0,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  1,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  2,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  3,  linear,  0.000614,   8.15);
  nBLConsts.set(FTKTrackParam::phi,  4,  linear,  0.000614,   8.15);

  //
  //  eta
  //
  nomConsts.set(FTKTrackParam::eta,  0,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  1,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  2,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  3,  linear,  0.00081,  4.3);
  nomConsts.set(FTKTrackParam::eta,  4,  linear,  0.00081,  4.3);

  nBLConsts.set(FTKTrackParam::eta,  0,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  1,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  2,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  3,  linear,  0.00095,  6.7);
  nBLConsts.set(FTKTrackParam::eta,  4,  linear,  0.00095,  6.7);

  //
  //  InvPt
  //
  nomConsts.set(FTKTrackParam::Ipt,  0,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  1,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  2,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  3,  linear,  2.7e-6,  0.023);
  nomConsts.set(FTKTrackParam::Ipt,  4,  linear,  2.7e-6,  0.023);

  nBLConsts.set(FTKTrackParam::Ipt,  0,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  1,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  2,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  3,  linear,  3.2e-6,  0.025);
  nBLConsts.set(FTKTrackParam::Ipt,  4,  linear,  3.2e-6,  0.025);
  return;
}


//
//   Initialize the IBL constants
//
void TrigFTKUncertiantyTool::LoadConstants_NoIBL()
{
  //
  // has BLayer constants
  //
  TPConsts&  nomConsts =  allConsts[1];

  //
  // has noBLayer constants
  //
  TPConsts&  nBLConsts =  allConsts[0];
  
  //
  //  d0
  //
  nomConsts.set(FTKTrackParam::d0,  0,  linear,  2.66e-02,   1.67e+02);
  nomConsts.set(FTKTrackParam::d0,  1,  linear,  2.63e-02,   2.00e+02);
  nomConsts.set(FTKTrackParam::d0,  2,  linear,  3.34e-02,   2.26e+02);
  nomConsts.set(FTKTrackParam::d0,  3,  linear,  3.84e-02,   3.14e+02);
  nomConsts.set(FTKTrackParam::d0,  4,  linear,  4.61e-02,   4.63e+02);
 
  nBLConsts.set(FTKTrackParam::d0,  0,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  1,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  2,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  3,  sqroot,  0.0093,     5.2e5);
  nBLConsts.set(FTKTrackParam::d0,  4,  sqroot,  0.0093,     5.2e5);

  //
  //  z0 
  //
  nomConsts.set(FTKTrackParam::z0,  0,  linear,  1.52e-01,  2.68e+02);
  nomConsts.set(FTKTrackParam::z0,  1,  linear,  1.39e-01,  3.60e+02);
  nomConsts.set(FTKTrackParam::z0,  2,  linear,  2.01e-01,  5.25e+02);
  nomConsts.set(FTKTrackParam::z0,  3,  linear,  3.10e-01,  8.64e+02);
  nomConsts.set(FTKTrackParam::z0,  4,  linear,  5.58e-01,  1.44e+03);

  nBLConsts.set(FTKTrackParam::z0,  0,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  1,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  2,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  3,  linear,  0.35,      480);
  nBLConsts.set(FTKTrackParam::z0,  4,  linear,  0.35,      480);

  //
  //  phi
  //
  nomConsts.set(FTKTrackParam::phi,  0,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  1,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  2,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  3,  linear,  0.00052,  5.9);
  nomConsts.set(FTKTrackParam::phi,  4,  linear,  0.00052,  5.9);

  nBLConsts.set(FTKTrackParam::phi,  0,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  1,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  2,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  3,  linear,  0.0011,   9.0);
  nBLConsts.set(FTKTrackParam::phi,  4,  linear,  0.0011,   9.0);

  //
  //  eta
  //
  nomConsts.set(FTKTrackParam::eta,  0,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  1,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  2,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  3,  linear,  0.0012,  5.6);
  nomConsts.set(FTKTrackParam::eta,  4,  linear,  0.0012,  5.6);

  nBLConsts.set(FTKTrackParam::eta,  0,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  1,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  2,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  3,  linear,  0.0017,  7.5);
  nBLConsts.set(FTKTrackParam::eta,  4,  linear,  0.0017,  7.5);

  //
  //  InvPt
  //
  nomConsts.set(FTKTrackParam::Ipt,  0,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  1,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  2,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  3,  linear,  3.0e-6,  0.024);
  nomConsts.set(FTKTrackParam::Ipt,  4,  linear,  3.0e-6,  0.024);

  nBLConsts.set(FTKTrackParam::Ipt,  0,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  1,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  2,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  3,  linear,  3.3e-6,  0.028);
  nBLConsts.set(FTKTrackParam::Ipt,  4,  linear,  3.3e-6,  0.028);
  return;
}
