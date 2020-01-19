/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "GenGauss3D.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include <math.h> 
#include "TMath.h" 

ClassImp(GenGauss3D) 

 GenGauss3D::GenGauss3D(const char *name, const char *title, 
                        RooArgList& _xvec,
                        RooArgList& _muvec,
                        RooAbsReal& _vxx,
                        RooAbsReal& _vxy,
                        RooAbsReal& _vxz,
                        RooAbsReal& _vyy,
                        RooAbsReal& _vyz,
                        RooAbsReal& _vzz,
			TMatrixDSym& dummyMatrix) :
   RooMultiVarGaussian(name,title,_xvec,_muvec,dummyMatrix), 
   vxx("vxx","vxx",this,_vxx),
   vxy("vxy","vxy",this,_vxy),
   vxz("vxz","vxz",this,_vxz),
   vyy("vyy","vyy",this,_vyy),
   vyz("vyz","vyz",this,_vyz),
   vzz("vzz","vzz",this,_vzz)
   //nUpdates(0),
   //nChanged(0),
   //oldVxx(-1),
   //oldVxy(-1),
   //oldVyy(-1),
   //oldVzz(-1)
 { 
 } 


 GenGauss3D::GenGauss3D(const GenGauss3D& other, const char* name) :  
   RooMultiVarGaussian(other,name), 
   vxx("vxx",this,other.vxx),
   vxy("vxy",this,other.vxy),
   vxz("vxz",this,other.vxz),
   vyy("vyy",this,other.vyy),
   vyz("vyz",this,other.vyz),
   vzz("vzz",this,other.vzz)
   //nUpdates(0),
   //nChanged(0),
   //oldVxx(-1),
   //oldVxy(-1),
   //oldVyy(-1),
   //oldVzz(-1)
 { 
 } 


 void GenGauss3D::updateCov() {
   //nUpdates++;
   //if (vxx==oldVxx &&
   //    vyy==oldVyy &&
   //    vxy==oldVxy &&
   //    vzz==oldVzz)
   //  return;
   //oldVxx = vxx;
   //oldVyy = vyy;
   //oldVxy = vxy;
   //oldVzz = vzz;

   //nChanged++;
   //if (nUpdates % 10000 == 0) {
   //  cout << nUpdates << " calls to updateCov" << endl;
   //  cout << nChanged << " calls required recalculation of of covariance matrix" << endl << endl;
   //}

   _cov(0,0) = vxx;
   _cov(0,1) = vxy;
   _cov(0,2) = vxz;
   _cov(1,0) = vxy;
   _cov(1,1) = vyy;
   _cov(1,2) = vyz;
   _cov(2,0) = vxz;
   _cov(2,1) = vyz;
   _cov(2,2) = vzz;
   _det = _cov.Determinant();

   int n = _cov.GetNrows();
   for (int i=0;i<n;i++) {
     for  (int j=0;j<n;j++) {
       _covI(i,j)=_cov(i,j);
     }
   }
   _covI.Invert();
 }


 Double_t GenGauss3D::evaluate() const 
 { 
   // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE 

   // Since we need to update _cov here, must get rid of const
   GenGauss3D* self =  const_cast<GenGauss3D*>(this);
   self->updateCov();

   return RooMultiVarGaussian::evaluate();
 } 



 Int_t GenGauss3D::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const  
 { 
   // LIST HERE OVER WHICH VARIABLES ANALYTICAL INTEGRATION IS SUPPORTED, 
   // ASSIGN A NUMERIC CODE FOR EACH SUPPORTED (SET OF) PARAMETERS 
   // THE EXAMPLE BELOW ASSIGNS CODE 1 TO INTEGRATION OVER VARIABLE X
   // YOU CAN ALSO IMPLEMENT MORE THAN ONE ANALYTICAL INTEGRAL BY REPEATING THE matchArgs 
   // EXPRESSION MULTIPLE TIMES

   // if (matchArgs(allVars,analVars,x)) return 1 ; 

   // Since we need to update _cov here, must get rid of const
   GenGauss3D* self =  const_cast<GenGauss3D*>(this);
   self->updateCov();

   return RooMultiVarGaussian::getAnalyticalIntegral(allVars,analVars,rangeName) ; 
 } 



 Double_t GenGauss3D::analyticalIntegral(Int_t code, const char* rangeName) const  
 { 
   // RETURN ANALYTICAL INTEGRAL DEFINED BY RETURN CODE ASSIGNED BY getAnalyticalIntegral
   // THE MEMBER FUNCTION x.min(rangeName) AND x.max(rangeName) WILL RETURN THE INTEGRATION
   // BOUNDARIES FOR EACH OBSERVABLE x

   // assert(code==1) ; 
   // return (x.max(rangeName)-x.min(rangeName)) ; 

    // Since we need to update _cov here, must get rid of const
   GenGauss3D* self =  const_cast<GenGauss3D*>(this);
   self->updateCov();

   // Prevent caching of partial integrals - these may depend on the
   // particular covariance matrix!
   _anaIntCache.clear();

   return RooMultiVarGaussian::analyticalIntegral(code,rangeName) ; 
} 


 void GenGauss3D::generateEvent(Int_t code) {
   _genCache.clear() ;
   updateCov();
   _genCache.clear() ;
   return RooMultiVarGaussian::generateEvent(code);
 }