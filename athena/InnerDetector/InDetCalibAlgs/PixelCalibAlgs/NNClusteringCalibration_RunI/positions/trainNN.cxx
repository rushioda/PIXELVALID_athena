/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include "TChain.h"
#include <TH1F.h>
#include <TLegend.h>
#include <iostream>
#include <TPad.h>
//#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
#include <math.h>
#include "../TJetNet.h"
#include "../doNormalization.C"
#include "Riostream.h"
#include "../TNetworkToHistoTool.h"
#include "TSystem.h"

#include "../TTrainedNetwork.h"

#include <iostream>

#include "TMatrixD.h"
#include "TVectorD.h"
#include "trainNN.h"

#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

Double_t sigmoid(Double_t x)
{
  return 1./(1.+exp(-2*x));
}

using namespace std;
bool isBadCluster( int sizeX, int nParticles ){


  if(sizeX==-100){return true;}
  if(nParticles==0){return true;}     
  if(nParticles!=1 && nParticles!=2 && nParticles!=3){return true;}
  
  

  return false;


}


bool skipSingle(int nParticles,int iClus, int dilutionFactor){


  // this reduces the number of single particle clusters of a factor k
  int k = 60;
   

  if (nParticles==1)
    {
      if ((iClus %(dilutionFactor*k) != 0) &&  (iClus % (dilutionFactor*k) != 1))
	{
	  return true;
	}
    }

  return false;
  
}



bool badTrackInfo(bool useTrackEstimate, double theta){

  if(useTrackEstimate && theta==0){return true;}
  return false;

}



int main()
{
  trainNN(
      "../reduceddatasets/reduceddataset_Cone4H1TopoParticleJets_forNN.root",
      "dummy",
      10000,
      200,
      10,
      10,
      0);
  return 0;
}



void trainNN(TString inputfile,
             TString outputclass,
             int nIterations,
             int dilutionFactor,
             int nodesFirstLayer,
             int nodesSecondLayer,
             int restartTrainingFrom,
             int nParticlesTraining,
             bool useTrackEstimate,
             int nPatternsPerUpdate,
             double learningRate,
             double learningRateDecrease,
             double learningRateMomentum) {

  double bweight=1;
  double cweight=1.;
  double lweight=1;

  gROOT->SetStyle("Plain");

  cout << "starting with settings: " << endl;
  cout << " nIterations: " << nIterations << endl;
  cout << " dilutionFactor: " << dilutionFactor << endl;
  cout << " nodesFirstLayer: " << nodesFirstLayer << endl;
  cout << " nodesSecondLayer: " << nodesSecondLayer << endl;

//  TFile *file = TFile::Open(inputfile);
//  TTree *simu = (TTree*)file->Get("Validation/NNinput");

  TChain *myChain = new TChain("Validation/NNinput");


if(!useTrackEstimate){
  #include "../files.txt"
}

if(useTrackEstimate){
  #include "../filesOnTrack.txt"
}
  TChain* simu=myChain;

  std::cout << " Training sample obtained... " << std::endl;

  vector<int>     *NN_sizeX;
  vector<int>     *NN_sizeY;
  vector<vector<float> > *NN_matrixOfToT;
  vector<vector<float> > *NN_vectorOfPitchesY;
  vector<int>     *NN_ClusterPixLayer;
  vector<int>     *NN_ClusterPixBarrelEC;
  vector<float>   *NN_phiBS;
  vector<float>   *NN_thetaBS;
  vector<float>   *NN_etaModule;
  vector<bool>    *NN_useTrackInfo;
  vector<int>     *NN_columnWeightedPosition;
  vector<int>     *NN_rowWeightedPosition;
  vector<vector<float> > *NN_positionX;
  vector<vector<float> > *NN_positionY;
  vector<vector<float> > *NN_position_idX;
  vector<vector<float> > *NN_position_idY;
  vector<vector<float> > *NN_theta;
  vector<vector<float> > *NN_phi;
  
  // List of branches
  TBranch        *b_NN_sizeX;   //!
  TBranch        *b_NN_sizeY;   //!
  TBranch        *b_NN_matrixOfToT;   //!
  TBranch        *b_NN_vectorOfPitchesY;   //!
  TBranch        *b_NN_ClusterPixLayer;   //!
  TBranch        *b_NN_ClusterPixBarrelEC;   //!
  TBranch        *b_NN_phiBS;   //!
  TBranch        *b_NN_thetaBS;   //!
  TBranch        *b_NN_etaModule;   //!
  TBranch        *b_NN_useTrackInfo;   //!
  TBranch        *b_NN_columnWeightedPosition;   //!
  TBranch        *b_NN_rowWeightedPosition;   //!
  TBranch        *b_NN_positionX;   //!
  TBranch        *b_NN_positionY;   //!
  TBranch        *b_NN_position_idX;   //!
  TBranch        *b_NN_position_idY;   //!
  TBranch        *b_NN_theta;   //!
  TBranch        *b_NN_phi;   //!
  


  NN_sizeX = 0;
  NN_sizeY = 0;
  NN_matrixOfToT = 0;
  NN_vectorOfPitchesY = 0;
  NN_ClusterPixLayer = 0;
  NN_ClusterPixBarrelEC = 0;
  NN_phiBS = 0;
  NN_thetaBS = 0;
  NN_etaModule = 0;
  NN_useTrackInfo = 0;
  NN_columnWeightedPosition = 0;
  NN_rowWeightedPosition = 0;
  NN_positionX = 0;
  NN_positionY = 0;
  NN_position_idX = 0;
  NN_position_idY = 0;
  NN_theta = 0;
  NN_phi = 0;
  // Set branch addresses and branch pointers
  //  if (!tree) return 0;
  //  TTree* simu = tree;
  //  fCurrent = -1;
  simu->SetMakeClass(1);

  simu->SetBranchAddress("NN_sizeX", &NN_sizeX, &b_NN_sizeX);
  simu->SetBranchAddress("NN_sizeY", &NN_sizeY, &b_NN_sizeY);
  simu->SetBranchAddress("NN_matrixOfToT", &NN_matrixOfToT, &b_NN_matrixOfToT);
  simu->SetBranchAddress("NN_vectorOfPitchesY", &NN_vectorOfPitchesY, &b_NN_vectorOfPitchesY);
  simu->SetBranchAddress("NN_ClusterPixLayer", &NN_ClusterPixLayer, &b_NN_ClusterPixLayer);
  simu->SetBranchAddress("NN_ClusterPixBarrelEC", &NN_ClusterPixBarrelEC, &b_NN_ClusterPixBarrelEC);
  simu->SetBranchAddress("NN_phiBS", &NN_phiBS, &b_NN_phiBS);
  simu->SetBranchAddress("NN_thetaBS", &NN_thetaBS, &b_NN_thetaBS);
  simu->SetBranchAddress("NN_etaModule", &NN_etaModule, &b_NN_etaModule);
  simu->SetBranchAddress("NN_useTrackInfo", &NN_useTrackInfo, &b_NN_useTrackInfo);
  simu->SetBranchAddress("NN_columnWeightedPosition", &NN_columnWeightedPosition, &b_NN_columnWeightedPosition);
  simu->SetBranchAddress("NN_rowWeightedPosition", &NN_rowWeightedPosition, &b_NN_rowWeightedPosition);
  simu->SetBranchAddress("NN_positionX", &NN_positionX, &b_NN_positionX);
  simu->SetBranchAddress("NN_positionY", &NN_positionY, &b_NN_positionY);
  simu->SetBranchAddress("NN_position_idX", &NN_position_idX, &b_NN_position_idX);
  simu->SetBranchAddress("NN_position_idY", &NN_position_idY, &b_NN_position_idY);
  simu->SetBranchAddress("NN_theta", &NN_theta, &b_NN_theta);
  simu->SetBranchAddress("NN_phi", &NN_phi, &b_NN_phi);
  
 
  cout << "Branches set..." << endl;

  TString filterTrain("Entry$%");
  filterTrain+=dilutionFactor;
  filterTrain+="==0";
  
  TString filterTest("Entry$%");
  filterTest+=dilutionFactor;
  filterTest+="==1";

  int* nneurons;
  int nlayer=3;

  simu->GetEntry(0);

  
  cout << "First entry..." << endl;
  
  Int_t           sizeX=-7; 
  Int_t           sizeY=-7; 
  
  
  // loop over the clusters loking for the first cluster properly saved 
  for( unsigned int clus =0; clus<NN_sizeX->size(); clus++ ){
    
    sizeX = (*NN_sizeX)[clus];
    sizeY = (*NN_sizeY)[clus];
    
    if(sizeX>0)break;
    
  }
  
  cout << "Size obtained" << endl;


  int numberinputs=sizeX*(sizeY+1)+4;
  if (!useTrackEstimate)
  {
    numberinputs=sizeX*(sizeY+1)+5;
  }

  int numberoutputs=2*nParticlesTraining;

  if (nodesSecondLayer!=0)
  {
    nlayer=4;
  }

  if (nodesSecondLayer!=0)
  {
    nneurons=new int[4];
  }
  else
  {
    nneurons=new int[3];
  }
  
  nneurons[0]=numberinputs;
  
  nneurons[1]=nodesFirstLayer;

  if (nodesSecondLayer!=0)
  {
    nneurons[2]=nodesSecondLayer;
    nneurons[3]=numberoutputs;//number of output nodes
  }
  else
  {
    nneurons[2]=numberoutputs;//number of output nodes
  }

  for (int i=0;i<nlayer;i++)
  {
    cout << " layer i: " << i << " number neurons: " << nneurons[i] << endl;
  }
  

  //  float eventWeight(0);
  float trainingError(0);
  float testError(0);
  
  //setting learning parameters

  cout << " now providing training events " << endl;
  
  Int_t numberTrainingEvents=0;
  Int_t numberTestingEvents=0;

  int iClus=0;
  int part_0=0;
  int part_1=0;
  int part_2=0;
  int part_3=0;

  int totalN=simu->GetEntries();
  
//  totalN=10000;

  // Loop over entries:
  for (Int_t i = 0; i < totalN; i++) {
    
    if (i % 50000 == 0 ) {
      std::cout << " Counting training / testing events in sample. Looping over event " << i << std::endl;
    }
    
    simu->GetEntry(i);
 
    for( unsigned int clus =0; clus<NN_sizeX->size(); clus++ ){

      vector<float>  *matrixOfToT=0;
      vector<float>   *vectorOfPitchesY=0;
    
      Float_t         phiBS;
      Float_t         thetaBS;
      Float_t         etaModule;
      Int_t ClusterPixLayer;
      Int_t ClusterPixBarrelEC;
      
      std::vector<float> * positionX=0;
      std::vector<float> * positionY=0;
      std::vector<float> * thetaTr=0;
      std::vector<float> * phiTr=0;

      std::vector<float> positionX_reorder;
      std::vector<float> positionY_reorder;
      
      
      sizeX = (*NN_sizeX)[clus];      
      positionX =&(*NN_positionX)[clus];
      int nParticles = positionX->size();
     
      thetaTr = &(*NN_theta)[clus];
      phiTr = &(*NN_phi)[clus];
      
      if (nParticlesTraining!=nParticles) 
	{
	  continue;
	}
      if (isBadCluster(sizeX, nParticles ) )continue;
      
      // loop over the particles;  
      for( unsigned int P = 0; P < positionX->size(); P++){
        double theta = (*thetaTr)[P];
        if (theta!=theta) continue;

	iClus++;

        if ( badTrackInfo(useTrackEstimate, theta ) )continue;
	
 
	
	if (iClus%dilutionFactor==0) numberTrainingEvents+=1;
	if (iClus%dilutionFactor==1) numberTestingEvents+=1;
	
	if (iClus%dilutionFactor==1 && nParticles==1 ) part_1++;
        if (iClus%dilutionFactor==1 && nParticles==2 ) part_2++;
        if (iClus%dilutionFactor==1 && nParticles==3 ) part_3++;
	
	
	
      }// end loop over th particles
    }// end loop over cluster
  }// end Loop over entries
  

  
  cout << " N. training events: " << numberTrainingEvents << 
      " N. testing events: " << numberTestingEvents << endl;

  // if(numberTrainingEvents!=numberTestingEvents)return;

  cout << "now start to setup the network..." << endl;
  
  TJetNet* jn = new TJetNet( numberTestingEvents, numberTrainingEvents, nlayer, nneurons );
  // return;
  cout <<  " setting learning method... " << endl;

  //  jn->SetMSTJN(4,12); Fletscher-Rieves (Scaled Conj Grad)

  
//  int nPatternsPerUpdate=1;
  
  jn->SetPatternsPerUpdate( nPatternsPerUpdate );
  jn->SetUpdatesPerEpoch( (int)std::floor((float)numberTrainingEvents/(float)nPatternsPerUpdate) );
  jn->SetUpdatingProcedure( 0 );
  jn->SetErrorMeasure( 0 );
  jn->SetActivationFunction( 1 );

  jn->SetLearningRate( learningRate );//0.8
//  jn->SetLearningRate( 1 );//0.8
//  jn->SetMomentum( 0.2 );//0.3 //is now 0.5
  jn->SetMomentum( learningRateMomentum );//0.3 //is now 0.5
  jn->SetInitialWeightsWidth( 1. );
  //  jn->SetLearningRateDecrease( 0.992 );

  jn->SetLearningRateDecrease( learningRateDecrease );//0.992
//  jn->SetLearningRateDecrease( 0.995 );//0.992
  jn->SetActivationFunction(4,nlayer-1-1);
  


  cout << " setting pattern for training events " << endl;

  int trainSampleNumber=0;
  int testSampleNumber=1;
  
  cout << " copying over training events " << endl;
  
  int counter=0;
  int counter0=0;
  int counter1=0;

 //input and output of first event
  vector<double> inputVar;
  vector<double> outputVar;
    

  iClus=0;
  for (Int_t i = 0; i < totalN; i++) {
    
    if (i % 100000 == 0 ) {
      std::cout << " Copying over training events. Looping over event " << i << std::endl;
    }

 
    simu->GetEntry(i);

    for( unsigned int clus =0; clus<NN_sizeX->size(); clus++ ){
    
      vector<float>  *matrixOfToT=0;
      vector<float>   *vectorOfPitchesY=0;
      
      Float_t         phiBS;
      Float_t         thetaBS;
      Float_t         etaModule;
      Int_t ClusterPixLayer;
      Int_t ClusterPixBarrelEC;
      
      std::vector<float> * position_idX=0;
      std::vector<float> * position_idY=0;
      std::vector<float> * thetaTr=0;
      std::vector<float> * phiTr=0;


      sizeX = (*NN_sizeX)[clus];

      sizeY = (*NN_sizeY)[clus];
      matrixOfToT=&(*NN_matrixOfToT)[clus];
      vectorOfPitchesY=&(*NN_vectorOfPitchesY)[clus];
      
      phiBS = (*NN_phiBS)[clus];
      thetaBS =(*NN_thetaBS)[clus];
      etaModule =(*NN_etaModule)[clus];
      
      ClusterPixLayer=(*NN_ClusterPixLayer)[clus];
      ClusterPixBarrelEC = (*NN_ClusterPixBarrelEC)[clus];
      
      position_idX =&(*NN_position_idX)[clus];
      position_idY =&(*NN_position_idY)[clus];

      int nParticles = position_idX->size();
      
      if (nParticlesTraining!=nParticles) 
	{
	  continue;
	}
      
      thetaTr = &(*NN_theta)[clus];
      phiTr = &(*NN_phi)[clus];
      
      
      if(isBadCluster(sizeX, nParticles ) )continue;
      
      for( unsigned int P = 0; P < position_idX->size(); P++){
	
	double	theta = (*thetaTr)[P];
	double	phi   = (*phiTr)[P]; 
        if (theta!=theta) continue;

	if (ClusterPixBarrelEC==2)
        {
          theta=-theta;
          phi=-phi;
        }

	iClus++;	 

	

      	if ( badTrackInfo(useTrackEstimate, theta ) )continue;

	
	
	if (matrixOfToT->size()!=sizeX*sizeY)
	  {
	    std::cout << " Event: " << i << " PROBLEM: size Y is: " << matrixOfToT->size() << std::endl;
	    throw;
	  }
	
	// loop over elements of matrixOfTot which is actually a vector
	for(unsigned int ME =0; ME < matrixOfToT->size(); ME++){
	  
	  if (iClus%dilutionFactor==0)	jn->SetInputTrainSet( counter0, ME, norm_ToT((*matrixOfToT)[ME]));
	  if (iClus%dilutionFactor==1)	jn->SetInputTestSet( counter1, ME, norm_ToT((*matrixOfToT)[ME]));
	  
          if ((*matrixOfToT)[ME] != (*matrixOfToT)[ME])
          {
            cout << "ME n. " << ME << " is: " << (*matrixOfToT)[ME] << endl;
            throw;
          }

	  if (counter0 == 0) std::cout << " element: " << ME <<  " ToT set to: " << norm_ToT((*matrixOfToT)[ME]) << std::endl;
	  if (iClus%dilutionFactor==1&&counter1==0) inputVar.push_back(norm_ToT((*matrixOfToT)[ME]));
	  
	}     
	
	
	// loop over vector of pitches
	for (int s=0;s<sizeY;s++)
	  {
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, sizeX*sizeY+s, norm_pitch((*vectorOfPitchesY)[s]));
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, sizeX*sizeY+s, norm_pitch((*vectorOfPitchesY)[s]));
	    
            if ((*vectorOfPitchesY)[s]!=(*vectorOfPitchesY)[s])
            {
              cout << " pitchY: " << (*vectorOfPitchesY)[s] << endl;
              throw;
            }
	    if (counter0 == 0) std::cout <<  " s: " << s << " pitch set to: " << norm_pitch((*vectorOfPitchesY)[s]) << std::endl;

	    if (iClus%dilutionFactor==1&&counter1==0) inputVar.push_back(norm_pitch((*vectorOfPitchesY)[s]));
	  }
	
	if (iClus%dilutionFactor==0)      jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY, norm_layerNumber(ClusterPixLayer));
	if (iClus%dilutionFactor==0)      jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+1, norm_layerType(ClusterPixBarrelEC));
	
        if (ClusterPixLayer!=ClusterPixLayer || ClusterPixBarrelEC!=ClusterPixBarrelEC)
        {
          cout << " ClusterPixLayer: " << ClusterPixLayer << " ClusterPixBarrelEC " << ClusterPixBarrelEC << endl;
          throw;
        }

	if (iClus%dilutionFactor==1)      jn->SetInputTestSet( counter1, (sizeX+1)*sizeY, norm_layerNumber(ClusterPixLayer));
	if (iClus%dilutionFactor==1)    jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+1, norm_layerType(ClusterPixBarrelEC));
	
	if (iClus%dilutionFactor==1&&counter1==0) {
	  inputVar.push_back(norm_layerNumber(ClusterPixLayer));
	  inputVar.push_back(norm_layerType(ClusterPixBarrelEC));
	}
	
	
	if (counter0 == 0) std::cout << " ClusterPixLayer " << norm_layerNumber(ClusterPixLayer) << " ClusterPixBarrelEC " << norm_layerType(ClusterPixBarrelEC) << std::endl;
	
	if (useTrackEstimate)
	  {
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+2, norm_phi(phi) );
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+3, norm_theta(theta) );
	
            if (phi!=phi) {
              cout << " phi: " << phi << endl;
              throw;
            }
            
            if (theta!=theta) {
              cout << " theta: " << theta << endl;
              throw;
            }
            

    
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+2, norm_phi(phi) );
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+3, norm_theta(theta) );
	    
	    if (counter0==0) std::cout << " phi " << norm_phi(phi) << " theta: " << norm_theta(theta) << std::endl;
	    
	    if (iClus%dilutionFactor==1&&counter1==0) {
	      inputVar.push_back(norm_phi(phi));
	      inputVar.push_back(norm_theta(theta));
	    }
	  }
	else
	  {
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+2, norm_phiBS(phiBS) );
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+3, norm_thetaBS(thetaBS) );
	    if (iClus%dilutionFactor==0)	  jn->SetInputTrainSet( counter0, (sizeX+1)*sizeY+4, norm_etaModule(etaModule) );
	    
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+2, norm_phiBS(phiBS) );
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+3, norm_thetaBS(thetaBS) );
	    if (iClus%dilutionFactor==1)	  jn->SetInputTestSet( counter1, (sizeX+1)*sizeY+4, norm_etaModule(etaModule) );
	    
	    
	    if (iClus%dilutionFactor==1&&counter1==0) {
	      inputVar.push_back(norm_phiBS(phiBS));
	      inputVar.push_back(norm_thetaBS(thetaBS));
	      inputVar.push_back(norm_etaModule(etaModule) );
	    }
	    
	    if (counter0==0) std::cout << 
	      " phiBS " << norm_phiBS(phiBS) << 
	      " thetaBS: " << norm_thetaBS(thetaBS) << 
	      " etaModule: " << norm_etaModule(etaModule) << std::endl;
	  }
	
	
	vector<float> xPositions = *position_idX;
	
	// 	for (int e=0;e<nParticles;e++)
	// 	  {
	// 	    xPositions.push_back((*positionsX)[e]);
	// 	  }
	
	std::sort(xPositions.begin(),xPositions.end());
	
	for (int o=0;o<xPositions.size();o++)
	  {
	    
	    if (iClus%dilutionFactor==0)   jn->SetOutputTrainSet(counter0, 2*o, norm_posX(xPositions[o]));
	    if (iClus%dilutionFactor==1)   jn->SetOutputTestSet(counter1, 2*o, norm_posX(xPositions[o])); 
	    
            if (xPositions[o]!=xPositions[o])
            {
              cout << "pos: " << xPositions[o] << endl;
              throw;
            }

	    if (counter0==0) std::cout << " output node: " << 2*o << " set to: " << norm_posX(xPositions[o]) << endl;

	    if (iClus%dilutionFactor==1&&counter1==0) { outputVar.push_back(norm_posX(xPositions[o]));}

	    double corry=-1000;
	    
	    for (int e=0;e<nParticles;e++)
	      {
		if (fabs((*position_idX)[e]-xPositions[o])<1e-10)
		  {
		    if (fabs(corry+1000)>1e-6)
		      {
			cout << " Value find more than once! " << endl;
			for (int p=0;p<xPositions.size();p++)
			  {
			    cout << " X n. : " << p << " is: " << xPositions[p] << endl;
			  }
		      }
		    corry=(*position_idY)[e];
		  }
	      }
	    
	    if (fabs(corry+1000)<1e-6) {
	      cout << " could not find original X pos. " << endl;
	      throw;
	    }
	    
	    if (iClus%dilutionFactor==0)    jn->SetOutputTrainSet(counter0, 2*o+1, norm_posY(corry));
	    if (iClus%dilutionFactor==1)    jn->SetOutputTestSet(counter1, 2*o+1, norm_posY(corry));	   
	    
            if (corry!=corry) {
              cout << " posY " << corry << endl;
              throw;
            }
            

	    if (counter0==0) std::cout << " output node: " << 2*o+1 << " set to: " << norm_posY(corry) << endl;
	    if (iClus%dilutionFactor==1&&counter1==0) {  outputVar.push_back(norm_posY(corry));}

	  }
	
	if (iClus%dilutionFactor==0)	jn->SetEventWeightTrainSet(  counter0, 1 );
	if (iClus%dilutionFactor==1)	jn->SetEventWeightTestSet(  counter1, 1 );
	

	if (iClus%dilutionFactor==0){counter0+=1;}
	if (iClus%dilutionFactor==1){counter1+=1;}
	
		

      }
    }
  }
  
  if (counter0!=numberTrainingEvents)
    {
      cout << " counter up to: " << counter0 << " while events in training sample are " << numberTrainingEvents << endl;
           return;
    }
  
  if (counter1!=numberTestingEvents)
    {
      cout << " counter up to: " << counter1 << " while events in testing sample are " << numberTestingEvents << endl;
         return;
    }

  // return;

  jn->Shuffle(true,false);
  
  if (restartTrainingFrom==0)
  {
    jn->Init();
    //    jn->DumpToFile("WeightsInitial.w");
  }
  else
  {
    TString name("Weights");
    name+=restartTrainingFrom;
    name+=".w";

    jn->ReadFromFile(name);
  }
  


  float minimumError=1e10;
  int epochesWithRisingError=0;
  int epochWithMinimum=0;

  int updatesPerEpoch=jn->GetUpdatesPerEpoch();

  //prepare output stream

  TString directory("weights");
  directory+="_nPat";
  directory+=nPatternsPerUpdate;
  directory+="_rate";
  directory+=(int)(learningRate*100);
  directory+="_rdec";
  directory+=(int)(100*learningRateDecrease);
  directory+="_mom";
  directory+=(int)(100*learningRateMomentum);
  directory+="_";
  directory+=nParticlesTraining;
  if (useTrackEstimate)
  {
    directory+="_withTracks";
  }

  TString command("mkdir ");
  command+=directory;

  gSystem->Exec(command);

  TString nameCronology=directory;
  
  nameCronology+="/trainingCronology.txt";

  ofstream cronology(nameCronology,ios_base::out);//|ios_base::app);
  
  cronology << "-------------SETTINGS----------------" << endl;
  cronology << "Epochs: " << jn->GetEpochs() << std::endl;
  cronology << "Updates Per Epoch: " << jn->GetUpdatesPerEpoch() << std::endl;
  cronology << "Updating Procedure: " << jn->GetUpdatingProcedure() << std::endl;
  cronology << "Error Measure: " << jn->GetErrorMeasure() << std::endl;
  cronology << "Patterns Per Update: " << jn->GetPatternsPerUpdate() << std::endl;
  cronology << "Learning Rate: " << jn->GetLearningRate() << std::endl;
  cronology << "Momentum: " << jn->GetMomentum() << std::endl;
  cronology << "Initial Weights Width: " << jn->GetInitialWeightsWidth() << std::endl;
  cronology << "Learning Rate Decrease: " << jn->GetLearningRateDecrease() << std::endl;
  cronology << "Activation Function: " << jn->GetActivationFunction() << std::endl;
  cronology << "-------------LAYOUT------------------" << endl;
  cronology << "Input variables: " << jn->GetInputDim() << endl;
  cronology << "Output variables: " << jn->GetOutputDim() << endl;
  cronology << "Hidden layers: " << jn->GetHiddenLayerDim() << endl;
  cronology << "Layout : ";
  for (Int_t s=0;s<jn->GetHiddenLayerDim()+2;++s)
  {
    cronology << jn->GetHiddenLayerSize(s);
    if (s<jn->GetHiddenLayerDim()+1) cronology << "-";
  }
  cronology << endl;
  cronology << "--------------HISTORY-----------------" << endl;
  cronology << "History of iterations: " << endl;
  cronology.close();

  //prepare training histo
  TH1F* histoTraining=new TH1F("training","training",(int)std::floor((float)nIterations/10.+0.5),1,std::floor((float)nIterations/10.+1.5));
  TH1F* histoTesting=new TH1F("testing","testing",(int)std::floor((float)nIterations/10.+0.5),1,std::floor((float)nIterations/10.+1.5));

  double maximumTrain=0;
  double minimumTrain=1e10;

  for(int epoch=restartTrainingFrom+1;epoch<=nIterations;++epoch)
  {
    if (epoch!=restartTrainingFrom+1)
    {
      trainingError = jn->Train();
    }

    if (epoch%10==0 || epoch==restartTrainingFrom+1)
    {

      cronology.open(nameCronology,ios_base::app);

      testError = jn->Test();

      if (trainingError>maximumTrain) maximumTrain=trainingError;
      if (testError>maximumTrain) maximumTrain=testError;
      if (trainingError<minimumTrain) minimumTrain=trainingError;
      if (testError<minimumTrain) minimumTrain=testError;

      
      histoTraining->Fill(epoch/10.,trainingError);
      histoTesting->Fill(epoch/10.,testError);

      if (testError<minimumError)
      {
        minimumError=testError;
        epochesWithRisingError=0;
        epochWithMinimum=epoch;
      }
      else
      {
        epochesWithRisingError+=10;

      }
      
      
      if (epochesWithRisingError>300)
      {
	if (trainingError<minimumError)
	{
	  cout << " End of training. Minimum already on epoch: " << epochWithMinimum << endl;
          cronology << " End of training. Minimum already on epoch: " << epochWithMinimum << endl;
	  break;
	} 
      }
      
      cronology << "Epoch: [" << epoch <<
          "] Error: " << trainingError << 
          " Test: " << testError << endl;

      cout << "Epoch: [" << epoch <<
	"] Error: " << trainingError << 
	" Test: " << testError << endl;

      cronology.close();
      
      TString name=directory;
      name+="/Weights";
      name+=epoch;
      name+=".root";

      TFile* file=new TFile(name,"recreate");
      TTrainedNetwork* trainedNetwork=jn->createTrainedNetwork();

      
      jn->Evaluate( 0  ); //evaluate the first test pattern
      for (int z=0;z<nParticlesTraining;z++)
      {
        std::cout << "output "<<z<<" x: " << jn->GetOutput(2*z) << " y: " << jn->GetOutput(2*z+1) << endl;
      }
      
      std::vector<double> myTestOutput=trainedNetwork->calculateOutputValues(inputVar);
      for (int z=0;z<nParticlesTraining;z++)
      {
        std::cout << "output TTNet "<<z<<" x: " << myTestOutput[2*z] << 
            " y: " << myTestOutput[2*z+1] << endl;
      }
      for (int z=0;z<nParticlesTraining;z++)
      {
        std::cout << "should be " << z << "x: " << outputVar[2*z] << " y: " << outputVar[2*z+1] << endl;
      }

      trainedNetwork->Write();
      file->Write();
      file->Close();
      delete file;

      /*
      TFile* file2=new TFile(name);
      trainedNetwork=(TTrainedNetwork*)file2->Get("TTrainedNetwork");
      cout <<" hid lay 1 size: " << trainedNetwork->getnHiddenLayerSize()[0] << endl;
      file2->Close();
      delete file2;
      */

      //      jn->DumpToFile(name);
    }
  }
      
  jn->writeNetworkInfo(1);
  jn->writeNetworkInfo(2);
  //  jn->writeNetworkInfo(3);
  //  jn->writeNetworkInfo(4);
  //  jn->writeNetworkInfo(5);


  //  cout << " Now try to understand how to get the weights..." << endl;

  ////////////WWWWWAAAAASSSSSS HERE
  Int_t nInput=jn->GetInputDim();
  
  cout << " create Trained Network object..." << endl;
  
  TTrainedNetwork* trainedNetwork=jn->createTrainedNetwork();

/*
  cout << " now getting value with trained Network ";

  


  double inputexample[9]={norm_nVTX(1),
			  norm_nTracksAtVtx(2),
			  norm_nSingleTracks(0),
			  norm_energyFraction(0.6),
			  norm_mass(2500),
			  norm_significance3d(4 ),
			  norm_IP3D(3),
			  norm_cat_pT(3),
			  norm_cat_eta(1)};

  for (Int_t i=0;i<nInput;++i)
  {
    jn->SetInputs(i,inputexample[i]);
  }

  cronology.open("weights/trainingCronology.txt",ios_base::app);

  jn->Evaluate();

  cronology << "----------------CONSISTENCY CHECK-----------" << endl;
  cout << "Result 0:" << jn->GetOutput(0);
  cronology << "Result 0:" << jn->GetOutput(0);
  cout << " Result 1:" << jn->GetOutput(1);
  cronology << "Result 0:" << jn->GetOutput(1);
  cout << " Result 2:" << jn->GetOutput(2) << endl;
  cronology << " Result 2:" << jn->GetOutput(2) << endl;

  cout << " Reading back old network " << endl;
  jn->readBackTrainedNetwork(trainedNetwork);

  cout <<" resetting input " << endl;
  for (Int_t i=0;i<nInput;++i)
  {
    jn->SetInputs(i,inputexample[i]);
  }

  jn->Evaluate();
 
  cout << "After reading back - Result 0:" << jn->GetOutput(0);
  cronology << "After reading back - Result 0:" << jn->GetOutput(0);
  // <<     " my: " << result[0] << endl;
  cout << " After reading back - Result 1:" << jn->GetOutput(1);
  cronology << "After reading back - Result 1:" << jn->GetOutput(1);
  //<<     " my: " << result[1] << endl;
  cout << " After reading back - Result 2:" << jn->GetOutput(2) << endl;
  cronology << "After reading back - Result 2:" << jn->GetOutput(2);
  // << " my: " << result[2] << endl;
  */

  cout << " Now getting histograms from trainingResult" << endl;
  cronology << " Now getting histograms from trainingResult" << endl;

  TNetworkToHistoTool myHistoTool;

  cout << " From network to histo..." << endl;
  std::vector<TH1*> myHistos=myHistoTool.fromTrainedNetworkToHisto(trainedNetwork);

  cout << " From histo to network back..." << endl;
  TTrainedNetwork* trainedNetwork2=myHistoTool.fromHistoToTrainedNetwork(myHistos);
  
  cout << " reading back " << endl;
  jn->readBackTrainedNetwork(trainedNetwork2);
   
/*
  cout <<" resetting input " << endl;
  for (Int_t i=0;i<nInput;++i)
  {
    jn->SetInputs(i,inputexample[i]);
  }

  jn->Evaluate();

  cout << "After reading back - Result 0:" << jn->GetOutput(0);
  cronology << "After reading back - Result 0:" << jn->GetOutput(0);
  // <<     " my: " << result[0] << endl;
  cout << " After reading back - Result 1:" << jn->GetOutput(1);
  cronology << "After reading back - Result 1:" << jn->GetOutput(1);
  //<<     " my: " << result[1] << endl;
  cout << " After reading back - Result 2:" << jn->GetOutput(2) << endl;
  cronology << "After reading back - Result 2:" << jn->GetOutput(2);
  // << " my: " << result[2] << endl;


  cout << " Directly from the trainedNetwork read back from HISTOS...!" << endl;

  std::vector<Double_t> inputData;
  for (Int_t u=0;u<nInput;++u)
  {
    inputData.push_back(inputexample[u]);
  }

  std::vector<Double_t> outputData=trainedNetwork2->calculateOutputValues(inputData);

  cout << "After reading back - Result 0:" << outputData[0] << endl;
  cout << " After reading back - Result 1:" << outputData[1] << endl;
  cout << " After reading back - Result 2:" << outputData[2] << endl;
*/   

  


  if (epochWithMinimum!=0)
  {
    cronology << "Minimum stored from Epoch: " << epochWithMinimum << endl;
  }  else
  {
    cronology << "Minimum not reached" << endl;
  }

  cronology.close();

  if (epochWithMinimum!=0)
  {
    
      TString name=directory;
      name+="/Weights";
      name+=epochWithMinimum;
      name+=".root";
      std::cout << " reading back from minimum " << endl;


      TFile *_file0 = new TFile(name);
      TTrainedNetwork* trainedNetwork=(TTrainedNetwork*)_file0->Get("TTrainedNetwork");

      cout << " Reading back network with minimum" << endl;
      jn->readBackTrainedNetwork(trainedNetwork);

      TString nameFile=directory;
      nameFile+="/weightMinimum.root";

      TFile* file=new TFile(nameFile,"recreate");
      trainedNetwork->Write();
      file->Write();
      file->Close();
      delete file;

      cout << " -------------------- " << endl;
      cout << " Writing OUTPUT histos " << endl;
      TString histoFName=directory;
      histoFName+="/histoWeights.root";

      TFile* fileHistos=new TFile(histoFName,"recreate");
      TNetworkToHistoTool histoTool;
      std::vector<TH1*> myHistos=histoTool.fromTrainedNetworkToHisto(trainedNetwork);
      std::vector<TH1*>::const_iterator histoBegin=myHistos.begin();
      std::vector<TH1*>::const_iterator histoEnd=myHistos.end();
      for (std::vector<TH1*>::const_iterator histoIter=histoBegin;
           histoIter!=histoEnd;++histoIter)
      {
        (*histoIter)->Write();
      }
      fileHistos->Write();
      fileHistos->Close();
      delete fileHistos;

      //        " filename: " << name << endl;
      
    //    jn->ReadFromFile(name);

  } 
  else
  {
    cout << " using network at last iteration (minimum not reached..." << endl;
  }
  
  //here you should create the class... Still open how to deal with this...
  //  char* myname=const_cast<char*>(static_cast<const char*>(outputclass));
  //  ierr=mlpsavecf_(myname);
 
  TString histoTName=directory;
  histoTName+="/trainingInfo.root";

  TFile* histoFile=new TFile(histoTName,"recreate");
  histoTraining->Write();
  histoTesting->Write();
  histoFile->Write();
  histoFile->Close();
  delete histoFile;

  TCanvas* trainingCanvas=new TCanvas("trainingCanvas","trainingCanvas");
  histoTraining->SetLineColor(2);
  histoTesting->SetLineColor(4);

  histoTraining->GetYaxis()->SetRangeUser(minimumTrain,maximumTrain);
  histoTraining->Draw("l");
  histoTesting->Draw("lsame");
  TString canvasName=directory;
  canvasName+="/trainingCurve.eps";
  trainingCanvas->SaveAs(canvasName);

 
  TCanvas* mlpa_canvas = new TCanvas("jetnet_canvas","Network analysis");
  mlpa_canvas->Divide(2,4);

/*
  
//  TCanvas* mlpa_canvas_5=gDirectory->Get("mlpa_canvas_5");
//  mlpa_canvas_5->SetLogy(kTrue);
  gPad->SetLogy();

  // Use the NN to plot the results for each sample
  // This will give approx. the same result as DrawNetwork.
  // All entries are used, while DrawNetwork focuses on 
  // the test sample. Also the xaxis range is manually set.
  TH1F *bg2 = new TH1F("bg2h", "NN output", 50, -.5, 1.5);
  TH1F *bg = new TH1F("bgh", "NN output", 50, -.5, 1.5);
  TH1F *sig = new TH1F("sigh", "NN output", 50, -.5, 1.5);

  //sig = 1 part; bg = 2 part; bg2 = 3 part

  TH1F *bg2test = new TH1F("bg2htest", "NN output", 50, -.5, 1.5);
  TH1F *bgtest = new TH1F("bghtest", "NN output", 50, -.5, 1.5);
  TH1F *sigtest = new TH1F("sightest", "NN output", 50, -.5, 1.5);

  int weight=1;
      
  for (Int_t i = 0; i < totalN; i++) {
    
    if (i % 100000 == 0 ) {
      std::cout << " First plot. Looping over event " << i << std::endl;
    }
    
    if (i%dilutionFactor!=0&&i%dilutionFactor!=1) continue;
    
    simu->GetEntry(i);

    for (int u=0;u<sizeX;u++)
    {
      for (int s=0;s<sizeY;s++)
      {
        jn->SetInputs(  s+u*sizeY, norm_ToT((*matrixOfToT)[u][s]));
      }
    }
    for (int s=0;s<sizeY;s++)
    {
      jn->SetInputs( sizeX*sizeY+s, norm_pitch((*vectorOfPitchesY)[s]));
    }

    jn->SetInputs( (sizeX+1)*sizeY, norm_phi(phi) );
    jn->SetInputs( (sizeX+1)*sizeY+1, norm_theta(theta) );

    jn->Evaluate();

    float p1=jn->GetOutput(0);
    float p2=jn->GetOutput(1);
    float p3=jn->GetOutput(2);

    if (nParticles==1)
    {
      if (i%dilutionFactor==0)
      {
        sig->Fill(p1/(p1+p2+p3),weight);
      }
      else if (i%dilutionFactor==1)
      {
        sigtest->Fill(p1/(p1+p2+p3),weight);
      }
    }
    if (nParticles==2)
    {
      if (i%dilutionFactor==0)
      {
        bg->Fill(p1/(p1+p2+p3),weight);
      }
      else if (i%dilutionFactor==1)
      {
        bgtest->Fill(p1/(p1+p2+p3),weight);
      }
    }
    if (nParticles>=3)
    {
      if (i%dilutionFactor==0)
      {
        bg2->Fill(p1/(p1+p2+p3),weight);
      }
      else  if (i%dilutionFactor==1)
      {
        bg2test->Fill(p1/(p1+p2+p3),weight);
      }
    }
  }

  //now you need the maximum
  float maximum=1;
  for (Int_t a=0;a<bg->GetNbinsX();a++)
  {
    if (bg->GetBinContent(a)>maximum)
    {
      maximum=1.2*bg->GetBinContent(a);
    }
  }


  bg2->SetLineColor(kYellow);
  bg2->SetFillStyle(3008);   bg2->SetFillColor(kYellow);
  bg->SetLineColor(kBlue);
  bg->SetFillStyle(3008);   bg->SetFillColor(kBlue);
  sig->SetLineColor(kRed);
  sig->SetFillStyle(3003); sig->SetFillColor(kRed);
  bg2->SetStats(0);
  bg->SetStats(0);
  sig->SetStats(0);


  bg2test->SetLineColor(kYellow);
  bg2test->SetFillStyle(3008);   bg2test->SetFillColor(kYellow);
  bgtest->SetLineColor(kBlue);
  bgtest->SetFillStyle(3008);   bgtest->SetFillColor(kBlue);
  sigtest->SetLineColor(kRed);
  sigtest->SetFillStyle(3003); sigtest->SetFillColor(kRed);
  bg2test->SetStats(0);
  bgtest->SetStats(0);
  sigtest->SetStats(0);

 mlpa_canvas->cd(1);
 gPad->SetLogy();

 bg->GetYaxis()->SetRangeUser(1,maximum);
 bgtest->GetYaxis()->SetRangeUser(1,maximum);

 mlpa_canvas->cd(1);
 bg->Draw();
 bg2->Draw("same");
 sig->Draw("same");

 TLegend *legend = new TLegend(.75, .80, .95, .95);
 legend->AddEntry(bg2, "particles >=3");
 legend->AddEntry(bg, "particles = 2");
 legend->AddEntry(sig, "particles = 1");
 legend->Draw();
 
 mlpa_canvas->cd(2);
 gPad->SetLogy();

 bgtest->Draw();
 bg2test->Draw("same");
 sigtest->Draw("same");

 TLegend *legendtest = new TLegend(.75, .80, .95, .95);
 legendtest->AddEntry(bg2test, "particles >=3");
 legendtest->AddEntry(bgtest, "particles = 2");
 legendtest->AddEntry(sigtest, "particles = 1");
 legendtest->Draw();

 mlpa_canvas->cd(5);
 gPad->SetLogy();
 bg->DrawNormalized();
 bg2->DrawNormalized("same");
 sig->DrawNormalized("same");
 legend->Draw();
 
 mlpa_canvas->cd(6);
 gPad->SetLogy();
 bgtest->DrawNormalized();
 bg2test->DrawNormalized("same");
 sigtest->DrawNormalized("same");
 legendtest->Draw();


 
 mlpa_canvas->cd(3);
 gPad->SetLogy();
 
 // Use the NN to plot the results for each sample
 // This will give approx. the same result as DrawNetwork.
 // All entries are used, while DrawNetwork focuses on 
 // the test sample. Also the xaxis range is manually set.
 TH1F *c_bg2 = new TH1F("c_bg2h", "NN output", 50, -.5, 1.5);
 TH1F *c_bg = new TH1F("c_bgh", "NN output", 50, -.5, 1.5);
 TH1F *c_sig = new TH1F("c_sigh", "NN output", 50, -.5, 1.5);

 TH1F *c_bg2test = new TH1F("c_bg2htest", "NN output", 50, -.5, 1.5);
 TH1F *c_bgtest = new TH1F("c_bghtest", "NN output", 50, -.5, 1.5);
 TH1F *c_sigtest = new TH1F("c_sightest", "NN output", 50, -.5, 1.5);

 for (Int_t i = 0; i < totalN; i++) {
   
   if (i % 100000 == 0 ) {
     std::cout << " Second plot. Looping over event " << i << std::endl;
   }
   
   if (i%dilutionFactor!=0&&i%dilutionFactor!=1) continue;
   
   simu->GetEntry(i);

    for (int u=0;u<sizeX;u++)
    {
      for (int s=0;s<sizeY;s++)
      {
        jn->SetInputs(  s+u*sizeY, norm_ToT((*matrixOfToT)[u][s]));
      }
    }
    for (int s=0;s<sizeY;s++)
    {
      jn->SetInputs( sizeX*sizeY+s, norm_pitch((*vectorOfPitchesY)[s]));
    }

    jn->SetInputs( (sizeX+1)*sizeY, norm_phi(phi) );
    jn->SetInputs( (sizeX+1)*sizeY+1, norm_theta(theta) );

    jn->Evaluate();

    float p1=jn->GetOutput(0);
    float p2=jn->GetOutput(1);
    float p3=jn->GetOutput(2);

    float discr=(p1+p2)/(p1+p2+p3);

    if (nParticles==1)
    {
      if (i%dilutionFactor==0)
      {
        c_sig->Fill(discr,weight);
      }
      else if (i%dilutionFactor==1)
      {
        c_sigtest->Fill(discr,weight);
      }
    }
    if (nParticles==2)
    {
      if (i%dilutionFactor==0)
      {
        c_bg->Fill(discr,weight);
      }
      else if (i%dilutionFactor==1)
      {
        c_bgtest->Fill(discr,weight);
      }
    }
    if (nParticles>=3)
    {
      if (i%dilutionFactor==0)
      {
        c_bg2->Fill(discr,weight);
      }
      else  if (i%dilutionFactor==1)
      {
        c_bg2test->Fill(discr,weight);
      }
    }
   }

  //now you need the maximum
 maximum=1;
  for (Int_t a=0;a<c_bg->GetNbinsX();a++)
  {
    if (c_bg->GetBinContent(a)>maximum)
    {
      maximum=1.2*c_bg->GetBinContent(a);
    }
  }

   c_bg2->SetLineColor(kYellow);
   c_bg2->SetFillStyle(3008);   c_bg2->SetFillColor(kYellow);
   c_bg->SetLineColor(kBlue);
   c_bg->SetFillStyle(3008);   c_bg->SetFillColor(kBlue);
   c_sig->SetLineColor(kRed);
   c_sig->SetFillStyle(3003); c_sig->SetFillColor(kRed);
   c_bg2->SetStats(0);
   c_bg->SetStats(0);
   c_sig->SetStats(0);
 
   c_bg2test->SetLineColor(kYellow);
   c_bg2test->SetFillStyle(3008);   c_bg2test->SetFillColor(kYellow);
   c_bgtest->SetLineColor(kBlue);
   c_bgtest->SetFillStyle(3008);   c_bgtest->SetFillColor(kBlue);
   c_sigtest->SetLineColor(kRed);
   c_sigtest->SetFillStyle(3003); c_sigtest->SetFillColor(kRed);
   c_bg2test->SetStats(0);
   c_bgtest->SetStats(0);
   c_sigtest->SetStats(0);

   mlpa_canvas->cd(3);
   gPad->SetLogy();


   c_bg->GetYaxis()->SetRangeUser(1,maximum);
   c_bgtest->GetYaxis()->SetRangeUser(1,maximum);
   
   c_bg->Draw();
   c_bg2->Draw("same");
   c_sig->Draw("same");

   TLegend *legend2 = new TLegend(.75, .80, .95, .95);
   legend2->AddEntry(c_bg2, "particles >=3");
   legend2->AddEntry(c_bg, "particles = 2");
   legend2->AddEntry(c_sig, "particles = 1");
   legend2->Draw();

   mlpa_canvas->cd(4);
   gPad->SetLogy();
   
   c_bgtest->Draw();
   c_bg2test->Draw("same");
   c_sigtest->Draw("same");

   TLegend *legend2test = new TLegend(.75, .80, .95, .95);
   legend2test->AddEntry(c_bg2test, "particles >=3");
   legend2test->AddEntry(c_bgtest, "particles = 2");
   legend2test->AddEntry(c_sigtest, "particles = 1");
   legend2test->Draw();

   mlpa_canvas->cd(7);
   gPad->SetLogy();
   c_bg->DrawNormalized();
   c_bg2->DrawNormalized("same");
   c_sig->DrawNormalized("same");
   legend2->Draw();
 
   mlpa_canvas->cd(8);
   gPad->SetLogy();
   c_bgtest->DrawNormalized();
   c_bg2test->DrawNormalized("same");
   c_sigtest->DrawNormalized("same");
   legend2test->Draw();


   mlpa_canvas->cd(0);


  mlpa_canvas->SaveAs("weights/result.eps");

*/
}

