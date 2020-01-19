/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_STRIPRESPONSE_H
#define MM_DIGITIZATION_STRIPRESPONSE_H
/** @class StripsResponse

// ------------
// Authors:
//   Iakovidis George        <george.iakovidis@cern.ch>
//   Karakostas Konstantinos <konstantinos.karakostas@cern.ch>
//   Leontsinis Stefanos     <stefanos.leontsinis@cern.ch>
//   Nektarios Chr. Benekos  <nbenekos@cern.ch>    
//////////////////////////////////////////////////////////////////////////////
  
Comments to be added here...

*/

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Service.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/StatusCode.h"

/// ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TF1.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <TCanvas.h>
#include <TMath.h>

/// Projects
#include "MM_Digitization/MmDigitToolInput.h"
#include "MM_Digitization/MmStripToolOutput.h"
//#include "MM_Digitization/MmElectronicsToolInput.h"
#include "MM_Digitization/MM_IonizationCluster.h"
#include "MM_Digitization/GarfieldGas.h" 

/// STD'S
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <sstream>
#include <sys/stat.h>

using std::vector;
using std::cout;
using std::endl;

/// ROOT Classed
class TF1;
class TRandom;
class TRandom3;

class ElectronicsResponse;
class MmDigitToolInput;
class MmStripToolOutput;
//class MmElectronicsToolInput;
//class MmDigitToolOutput;

class GarfieldGas; 

class StripsResponse {
  
private:

  /** qThreshold=2e, we accept a good strip if the charge is >=2e */
  float qThreshold; 
  /** // 0.350/10 diffusSigma=transverse diffusion (350 μm per 1cm ) for 93:7 @ 600 V/cm, according to garfield  */
  float diffusSigma; 
  float LogitundinalDiffusSigma;  
  float pitch; 
  /** //pitch=0.500 properties of the micromegas ToDo: to be reviewed */
  float stripwidth; 
  /** crosstalk of neighbor strips, it's 15%  */
  float crossTalk1;//0.10; // 
  /** // crosstalk of second neighbor strips, it's 6% */
  float crossTalk2;//0.03; 
  /** // (degrees) Magnetic Field 0.5 T */
  float Lorentz_Angle; 
  // Avalanche gain
  float gain;

  /// ToDo: random number from custom functions
  TF1 *polya, *conv_gaus;
  TF1 *LongitudinalDiffusionFunction, *TransverseDiffusionFunction;

  GarfieldGas* gas; 

  StripsResponse & operator=(const StripsResponse &right);
  StripsResponse(const StripsResponse&);

  std::vector<MM_IonizationCluster> IonizationClusters;

public :

  float driftGap;  
  /** //0.050 drift velocity in [mm/ns], driftGap=5 mm +0.128 mm (the amplification gap) */
  float driftVelocity; 
 
  StripsResponse();
   
  virtual ~StripsResponse();
  MmStripToolOutput GetResponceFrom(const MmDigitToolInput & digiInput);
  //  MmElectronicsToolInput GetResponceFrom(const MmDigitToolInput & digiInput);
  //  MmDigitToolOutput GetResponseFrom(const MmDigitToolInput & digiInput);
    
  void initializationFrom ();
  void writeHistos();
  void initHistos ();
  void clearValues ();
  void initFunctions ();
  void whichStrips(const float & hitx, const int & stripOffest, const float & thetaD, const int & stripMaxID, const MmDigitToolInput & digiInput);

  void loadGasFile      (const std::string fileName); // 27/05/2015 T.Saito
  
  inline void set_qThreshold (float val) { qThreshold = val; };
  inline void set_diffusSigma (float val) { diffusSigma = val; };
  inline void set_LogitundinalDiffusSigma (float val) { LogitundinalDiffusSigma = val; };
  inline void set_driftVelocity (float val) { driftVelocity = val; };
  inline void set_crossTalk1 (float val) { crossTalk1 = val; };
  inline void set_crossTalk2 (float val) { crossTalk2 = val; };
  inline void set_driftGap      (float val) {driftGap = val;};
  inline void set_stripWidth      (float val) {stripwidth = val;};
   
  float get_stripWidth    () const { return stripwidth   ;};
  float get_qThreshold    () const { return qThreshold   ;};
  float get_driftGap      () const { return driftGap     ;};
  float get_driftVelocity () const { return driftVelocity;};

/*
  Coverity defects
  float get_tMinFirstPeak () const { return tMinFirstPeak;};
  float get_tMinIntegration () const { return tMinIntegration;};
  float get_tminIntegrationAboveThreshold () const { return tminIntegrationAboveThreshold;};
*/  
  
  vector <float> get_tStripElectronicsAbThr() const { return tStripElectronicsAbThr;};
  vector <float> get_qStripElectronics() const { return qStripElectronics;};
  vector <float> get_finaltStripNoSlewing() const { return finaltStripNoSlewing;};
  vector < vector <float> > get_finalqStrip() const { return finalqStrip;};
  vector < vector <float> > get_finaltStrip() const { return finaltStrip;};
  vector <int>   get_nStripElectronics() const { return nStripElectronics;};
  vector <int>   get_finalNumberofStrip() const { return finalNumberofStrip;};

  vector <int>   finalNumberofStrip;
  vector <int>   nStripElectronics;
  vector < vector <float> > finalqStrip;
  vector < vector <float> > finaltStrip;
  vector <float> finaltStripNoSlewing;
  vector <float> tStripElectronicsAbThr;
  vector <float> qStripElectronics;
 
  int    nstrip;
  float  temp_polya;
  float  polya_theta;
  float  numberOfElectrons;
/*
  Coverity defects
  float  tMinFirstPeak;
  float  tMinIntegration;
  float  tminIntegrationAboveThreshold;
*/ 

// whichStrips()
  int dimClusters; //dimClusters=total number of collisions
  int MaxPrimaryIons;
  vector <int> stripNumber;
  double pt,xx, xxDiffussion, yy, yyDiffussion ;
  int primaryion; 
  vector <int> firstq;
  float lmean;
  vector <float> qstrip; 
  vector <float> cntTimes;
  vector <float> tStrip;
  vector <float> qStrip;
  vector <float> time;  //Drift velocity [mm/ns]
  vector <int> numberofStrip;
  
  vector <float> clusterelectrons;
  vector <float> l;
  float totalelectrons;
  float ll;
   
  TRandom * gRandom_loc;
  TRandom3 *randomNum;
};
#endif
