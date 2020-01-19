/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  JMS Calibration
 *
 *  Author: Jonathan Bossio (jbossios@cern.ch)
 *
 */

#include <TKey.h>

#include "JetCalibTools/CalibrationMethods/JMSCorrection.h"
#include "PathResolver/PathResolver.h"


JMSCorrection::JMSCorrection()
  : asg::AsgTool( "JMSCorrection::JMSCorrection" ), JetCalibrationToolBase::JetCalibrationToolBase(),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false)
   
{ }

JMSCorrection::JMSCorrection(const std::string& name)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(NULL), m_jetAlgo(""), m_calibAreaTag(""), m_dev(false)
   
{ }

JMSCorrection::JMSCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev)
  : asg::AsgTool( name ), JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config), m_jetAlgo(jetAlgo), m_calibAreaTag(calibAreaTag), m_dev(dev)
{ }

JMSCorrection::~JMSCorrection() {

}

StatusCode JMSCorrection::initializeTool(const std::string&) {

  ATH_MSG_INFO("Initializing the JMS Calibration tool");

  if ( !m_config ) { ATH_MSG_FATAL("Config file not specified. Aborting."); return StatusCode::FAILURE; }

  m_jetStartScale = m_config->GetValue("JMSStartingScale","JetGSCScaleMomentum");
  m_jetOutScale = m_config->GetValue("JMSOutScale","JetJMSScaleMomentum");
  // Starting pT value to calibrate
  m_pTMinCorr = m_config->GetValue("MinpT",180); 

  m_pTfixed   = m_config->GetValue("pTfixed",false); //small-R:true, large-R:false
  m_warning_counter_mTACorr = 0; // if >0 the track assisted mass correction is not applied

  if ( m_jetAlgo.EqualTo("") ) { ATH_MSG_FATAL("No jet algorithm specified. Aborting."); return StatusCode::FAILURE; }

  //find the ROOT file containing response histograms, path comes from the config file.
  TString JMSFile = m_config->GetValue("MassCalibrationFile","empty");
  if ( JMSFile.EqualTo("empty") ) { 
    ATH_MSG_FATAL("NO JMSFactorsFile specified. Aborting.");
    return StatusCode::FAILURE;
  }
  if(m_dev){
    JMSFile.Insert(0,"JetCalibTools/");
    JMSFile.Insert(28,m_calibAreaTag);
  }
  else{JMSFile.Insert(14,m_calibAreaTag);}
  TString fileName = PathResolverFindCalibFile(JMSFile.Data());
  TFile *inputFile = TFile::Open(fileName);
  if (!inputFile){
    ATH_MSG_FATAL("Cannot open JMS factors file" << fileName);
    return StatusCode::FAILURE;
  }

  //ATH_MSG_INFO("  for " << m_jetAlgo << " jets\n\n");

  setMassEtaBins( JetCalibUtils::VectorizeD( m_config->GetValue("MassEtaBins","") ) );

  //Get a TList of TKeys pointing to the histograms contained in the ROOT file
  inputFile->cd();
  TList *keys = inputFile->GetListOfKeys();
  //fill the names of the TKeys into a vector of TStrings
  TIter ikeys(keys);
  while ( TKey *iterobj = (TKey*)ikeys() ) {
    TString histoName = iterobj->GetName();
    if ( histoName.Contains(m_jetAlgo) ) m_respFactorsMass.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile,histoName.Data()) );
  }

  //Make sure we put something in the vector of TH2Fs
  if ( m_respFactorsMass.size() < 3 ) {
    ATH_MSG_FATAL("Vector of mass correction histograms may be empty. Please check your mass calibration file: " << JMSFile);
    return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO("JMS Tool has been initialized with binning and eta fit factors from: " << fileName);

  // Shoulb be applied the mass combination?
  m_combination = m_config->GetValue("Combination",false); // true: turn on combination of calo mass with track-assisted mass
  m_useCorrelatedWeights = m_config->GetValue("UseCorrelatedWeights",false); // true: turn on combination of calo mass with track-assisted mass

  // Track-Assisted Jet Mass correction
  m_trackAssistedJetMassCorr = m_config->GetValue("TrackAssistedJetMassCorr",false);
  TString JMS_TrackAssisted_File;
  TString file_trkAssisted_Name;
  TFile *inputFile_trkAssisted;
  if(m_trackAssistedJetMassCorr || m_combination){
    ATH_MSG_INFO("Track Assisted Jet Mass will be calibrated");
    JMS_TrackAssisted_File = m_config->GetValue("TrackAssistedMassCalibrationFile","empty");
    if ( JMS_TrackAssisted_File.EqualTo("empty") ) { 
      ATH_MSG_FATAL("NO Track Assisted Mass Factors File specified. Aborting.");
      return StatusCode::FAILURE;
    }
    if(m_dev){
      JMS_TrackAssisted_File.Insert(0,"JetCalibTools/");
      JMS_TrackAssisted_File.Insert(28,m_calibAreaTag);
    }
    else{JMS_TrackAssisted_File.Insert(14,m_calibAreaTag);}
    file_trkAssisted_Name = PathResolverFindCalibFile(JMS_TrackAssisted_File.Data());
    inputFile_trkAssisted = TFile::Open(file_trkAssisted_Name);
    if (!inputFile_trkAssisted){
      ATH_MSG_FATAL("Cannot open Track Assisted Mass factors file" << fileName);
      return StatusCode::FAILURE;
    }

    //setMassEtaBins( JetCalibUtils::VectorizeD( m_config->GetValue("MassEtaBins","") ) );

    //Get a TList of TKeys pointing to the histograms contained in the ROOT file
    inputFile_trkAssisted->cd();
    TList *keys_trkAssisted = inputFile_trkAssisted->GetListOfKeys();
    //fill the names of the TKeys into a vector of TStrings
    TIter ikeys_trkAssisted(keys_trkAssisted);
    while ( TKey *iterobj = (TKey*)ikeys_trkAssisted() ) {
      TString histoName = iterobj->GetName();
      if ( histoName.Contains(m_jetAlgo) ) m_respFactorsTrackAssistedMass.push_back( (TH2F*)JetCalibUtils::GetHisto2(inputFile_trkAssisted,histoName.Data()) );
    }

    //Make sure we put something in the vector of TH2Fs
    if ( m_respFactorsTrackAssistedMass.size() < 3 ) {
      ATH_MSG_FATAL("Vector of track assisted mass correction histograms may be empty. Please check your track assisted mass calibration file: " << JMSFile);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_INFO("JMS Tool has been initialized with binning and eta fit factors from: " << file_trkAssisted_Name);
  }

  // Combination
  TString Combination_File;
  TString file_combination_Name;
  TFile *inputFile_combination;
  if(m_combination){
    ATH_MSG_INFO("Mass Combination: ON");
    Combination_File = m_config->GetValue("CombinationFile","empty");
    if ( Combination_File.EqualTo("empty") ) { 
      ATH_MSG_FATAL("NO Combination File specified. Aborting.");
      return StatusCode::FAILURE;
    }
    if(m_dev){
      Combination_File.Insert(0,"JetCalibTools/");
      Combination_File.Insert(28,m_calibAreaTag);
    }
    else{Combination_File.Insert(14,m_calibAreaTag);}
    file_combination_Name = PathResolverFindCalibFile(Combination_File.Data());
    inputFile_combination = TFile::Open(file_combination_Name);
    if (!inputFile_combination){
      ATH_MSG_FATAL("Cannot open Mass Combination file" << fileName);
      return StatusCode::FAILURE;
    }

    setMassCombinationEtaBins( JetCalibUtils::VectorizeD( m_config->GetValue("MassCombinationEtaBins","") ) );

    // Identify which object is being tagged (QCD, Top, WZ, Hbb)
    TString combObj = "";
    if(m_jetOutScale.Contains("QCD")) combObj = "_QCD_";
    else if(m_jetOutScale.Contains("Top")){ combObj = "_Top_";}
    else if(m_jetOutScale.Contains("WZ")){ combObj = "_WZ_";}
    else if(m_jetOutScale.Contains("Hbb")){ combObj = "_WZ_";} // Temporary due to missing Hbb weights
    if(combObj==""){
      ATH_MSG_FATAL("Wrong JMS Outgoing Scale");
      return StatusCode::FAILURE;
    }

    //Get a TList of TKeys pointing to the histograms contained in the ROOT file
    inputFile_combination->cd();
    TList *keys_combination = inputFile_combination->GetListOfKeys();
    //fill the names of the TKeys into a vector of TStrings
    TIter ikeys_combination(keys_combination);
    while ( TKey *iterobj = (TKey*)ikeys_combination() ) {
      TString histoName = iterobj->GetName();
      if ( histoName.Contains("CaloMass") && histoName.Contains(combObj.Data()) ) m_caloResolutionMassCombination.push_back( (TH2D*)JetCalibUtils::GetHisto2(inputFile_combination,histoName.Data()) );
      if ( histoName.Contains("TAMass") && histoName.Contains(combObj.Data()) )  m_taResolutionMassCombination.push_back( (TH2D*)JetCalibUtils::GetHisto2(inputFile_combination,histoName.Data()) );
      if ( histoName.Contains("Correlation") && histoName.Contains(combObj.Data()) )  m_correlationMapMassCombination.push_back( (TH2D*)JetCalibUtils::GetHisto2(inputFile_combination,histoName.Data()) );
    }

    //Make sure we put something in the vector of TH2Ds
    if ( m_caloResolutionMassCombination.size() < 1 ) {
      ATH_MSG_FATAL("Vector of mass combination histograms with calo factors may be empty. Please check your mass combination file: " << JMSFile);
      return StatusCode::FAILURE;
    }
    else if ( m_taResolutionMassCombination.size() < 1 ) {
      ATH_MSG_FATAL("Vector of mass combination histograms with trk-assisted factors may be empty. Please check your mass combination file: " << JMSFile);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_INFO("JMS Tool has been initialized with mass combination weights from: " << file_combination_Name);
  }

  return StatusCode::SUCCESS;

}

float JMSCorrection::getMassCorr(double pT_uncorr, double m_uncorr, int etabin) const {

  // Asymptotic values
  double pTMax = m_respFactorsMass[etabin]->GetXaxis()->GetBinLowEdge(m_respFactorsMass[etabin]->GetNbinsX()+1);
  double pTMin = m_respFactorsMass[etabin]->GetXaxis()->GetBinLowEdge(1);
  double massMax = m_respFactorsMass[etabin]->GetYaxis()->GetBinLowEdge(m_respFactorsMass[etabin]->GetNbinsY()+1);
  if ( pT_uncorr > pTMax ) pT_uncorr = pTMax-1e-6 ; //so it fits the up-most pt-bin
  if ( pT_uncorr < m_pTMinCorr ) return 1; // no correction
  if ( pT_uncorr < pTMin ) pT_uncorr = pTMin+1e-6; //so it fits the low-most pt-bin
  if ( m_uncorr > massMax ) m_uncorr = massMax-1e-6; //so it fits the up-most m-bin

  float mass_corr = m_respFactorsMass[etabin]->Interpolate( pT_uncorr, m_uncorr );

  return mass_corr;
}

float JMSCorrection::getTrackAssistedMassCorr(double pT_uncorr, double m_uncorr, int etabin) const {

  // Asymptotic values
  double pTMax = m_respFactorsTrackAssistedMass[etabin]->GetXaxis()->GetBinLowEdge(m_respFactorsTrackAssistedMass[etabin]->GetNbinsX()+1);
  double pTMin = m_respFactorsTrackAssistedMass[etabin]->GetXaxis()->GetBinLowEdge(1);
  double massMax = m_respFactorsTrackAssistedMass[etabin]->GetYaxis()->GetBinLowEdge(m_respFactorsTrackAssistedMass[etabin]->GetNbinsY()+1);
  if ( pT_uncorr > pTMax ) pT_uncorr = pTMax-1e-6 ; //so it fits the up-most pt-bin
  if ( pT_uncorr < m_pTMinCorr ) return 1; // no correction
  if ( pT_uncorr < pTMin ) pT_uncorr = pTMin+1e-6; //so it fits the low-most pt-bin
  if ( m_uncorr > massMax ) m_uncorr = massMax-1e-6; //so it fits the up-most m-bin

  float mass_corr = m_respFactorsTrackAssistedMass[etabin]->Interpolate( pT_uncorr, m_uncorr );

  return mass_corr;
}

float JMSCorrection::getRelCalo(double pT_uncorr, double m_over_pt_uncorr, int etabin) const {

  // Asymptotic values
  double pTMax = m_caloResolutionMassCombination[etabin]->GetXaxis()->GetBinLowEdge(m_caloResolutionMassCombination[etabin]->GetNbinsX()+1);
  double pTMin = m_caloResolutionMassCombination[etabin]->GetXaxis()->GetBinLowEdge(1);
  double mass_over_pTMax = m_caloResolutionMassCombination[etabin]->GetYaxis()->GetBinLowEdge(m_caloResolutionMassCombination[etabin]->GetNbinsY()+1);
  double mass_over_pTMin = m_caloResolutionMassCombination[etabin]->GetYaxis()->GetBinLowEdge(1);
  if ( pT_uncorr > pTMax ) pT_uncorr = pTMax-1e-6 ; //so it fits the up-most pt-bin
  if ( pT_uncorr < pTMin ) pT_uncorr = pTMin+1e-6; //so it fits the low-most pt-bin
  if ( m_over_pt_uncorr > mass_over_pTMax ) m_over_pt_uncorr = mass_over_pTMax-1e-6; //so it fits the up-most m_over_pt-bin
  if ( m_over_pt_uncorr < mass_over_pTMin ) m_over_pt_uncorr = mass_over_pTMin+1e-6; //so it fits the low-most m_over_pt-bin

  float rel = m_caloResolutionMassCombination[etabin]->Interpolate( pT_uncorr, m_over_pt_uncorr );

  return rel;
}

float JMSCorrection::getRelTA(double pT_uncorr, double m_over_pt_uncorr, int etabin) const {

  // Asymptotic values
  double pTMax = m_taResolutionMassCombination[etabin]->GetXaxis()->GetBinLowEdge(m_taResolutionMassCombination[etabin]->GetNbinsX()+1);
  double pTMin = m_taResolutionMassCombination[etabin]->GetXaxis()->GetBinLowEdge(1);
  double mass_over_pTMax = m_taResolutionMassCombination[etabin]->GetYaxis()->GetBinLowEdge(m_taResolutionMassCombination[etabin]->GetNbinsY()+1);
  double mass_over_pTMin = m_taResolutionMassCombination[etabin]->GetYaxis()->GetBinLowEdge(1);
  if ( pT_uncorr > pTMax ) pT_uncorr = pTMax-1e-6 ; //so it fits the up-most pt-bin
  if ( pT_uncorr < pTMin ) pT_uncorr = pTMin+1e-6; //so it fits the low-most pt-bin
  if ( m_over_pt_uncorr > mass_over_pTMax ) m_over_pt_uncorr = mass_over_pTMax-1e-6; //so it fits the up-most m_over_pt-bin
  if ( m_over_pt_uncorr < mass_over_pTMin ) m_over_pt_uncorr = mass_over_pTMin+1e-6; //so it fits the low-most m_over_pt-bin

  float rel = m_taResolutionMassCombination[etabin]->Interpolate( pT_uncorr, m_over_pt_uncorr );

  return rel;
}

float JMSCorrection::getRho(double pT_uncorr, double m_over_pt_uncorr, int etabin) const {

  // Asymptotic values
  double pTMax = m_correlationMapMassCombination[etabin]->GetXaxis()->GetBinLowEdge(m_correlationMapMassCombination[etabin]->GetNbinsX()+1);
  double pTMin = m_correlationMapMassCombination[etabin]->GetXaxis()->GetBinLowEdge(1);
  double mass_over_pTMax = m_correlationMapMassCombination[etabin]->GetYaxis()->GetBinLowEdge(m_correlationMapMassCombination[etabin]->GetNbinsY()+1);
  double mass_over_pTMin = m_correlationMapMassCombination[etabin]->GetYaxis()->GetBinLowEdge(1);
  if ( pT_uncorr > pTMax ) pT_uncorr = pTMax-1e-6 ; //so it fits the up-most pt-bin
  if ( pT_uncorr < pTMin ) pT_uncorr = pTMin+1e-6; //so it fits the low-most pt-bin
  if ( m_over_pt_uncorr > mass_over_pTMax ) m_over_pt_uncorr = mass_over_pTMax-1e-6; //so it fits the up-most m_over_pt-bin
  if ( m_over_pt_uncorr < mass_over_pTMin ) m_over_pt_uncorr = mass_over_pTMin+1e-6; //so it fits the low-most m_over_pt-bin

  float rho = m_correlationMapMassCombination[etabin]->Interpolate( pT_uncorr, m_over_pt_uncorr );

  return rho;
}


StatusCode JMSCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const {

  //Apply the JMS calibration scale factor

  //Takes the uncorrected jet eta (in case the origin and/or 4vector jet area corrections were applied)
  float detectorEta = jet.getAttribute<float>("DetectorEta");
  double absdetectorEta = fabs(detectorEta);

  xAOD::JetFourMom_t jetStartP4;
  ATH_CHECK( setStartP4(jet) );
  jetStartP4 = jet.jetP4();

  xAOD::JetFourMom_t calibP4 = jet.jetP4();

  // For combination
  float m_calo;   // saving calibrated calo mass
  double pT_calo; // saving pT corrected by calo mass calib
  float m_ta;     // saving calibrated trk-assisted mass

  float mass_corr = jetStartP4.mass();
  double pT_corr = jetStartP4.pt();

  // Determine mass eta bin to use
  int etabin=-99;
  if (m_massEtaBins.size()==0 || m_respFactorsMass.size() != m_massEtaBins.size()-1){
    ATH_MSG_FATAL("Please check that the mass correction eta binning is properly set in your config file");
    return StatusCode::FAILURE;
  }
  xAOD::JetConstituentVector constituents = jet.getConstituents();
  int nconstituents = constituents.size();
  if ( absdetectorEta < m_massEtaBins.back() && nconstituents>1 ) { //Fiducial Cuts
    for (uint i=0; i<m_massEtaBins.size()-1; ++i) {
        if(absdetectorEta >= m_massEtaBins[i] && absdetectorEta < m_massEtaBins[i+1]) etabin = i;
    }
    if (etabin< 0){
      ATH_MSG_FATAL("There was a problem determining the eta bin to use for the mass correction");
      return StatusCode::FAILURE;
    }
    mass_corr = jetStartP4.mass()/getMassCorr( jetStartP4.pt()/m_GeV, jetStartP4.mass()/m_GeV, etabin );
    if(!m_pTfixed) pT_corr = sqrt(jetStartP4.e()*jetStartP4.e()-mass_corr*mass_corr)/cosh( jetStartP4.eta() );
  }

  m_calo = mass_corr;
  pT_calo = pT_corr;
  
  if(!m_combination){
    TLorentzVector TLVjet;
    TLVjet.SetPtEtaPhiM( pT_corr, jetStartP4.eta(), jetStartP4.phi(), mass_corr );
    calibP4.SetPxPyPzE( TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E() );
  
    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentum",calibP4);
    jet.setJetP4( calibP4 );
  }

  // Track Assisted Mass Correction
  if(m_trackAssistedJetMassCorr || m_combination ){

    double E_corr = jetStartP4.e();

    // Determine mass eta bin to use
    etabin=-99;
    if (m_massEtaBins.size()==0 || m_respFactorsTrackAssistedMass.size() != m_massEtaBins.size()-1){
      ATH_MSG_FATAL("Please check that the mass correction eta binning is properly set in your config file");
      if(m_combination) return StatusCode::FAILURE;
    }
    xAOD::JetConstituentVector constituents = jet.getConstituents();
    int nconstituents = constituents.size();
    float trackSumMass;
    std::string TrackSumMassStr = "TrackSumMass";
    if(m_jetAlgo=="AntiKt4EMTopo" || m_jetAlgo=="AntiKt4LCTopo") TrackSumMassStr = "DFCommonJets_TrackSumMass";
    std::string TrackSumPtStr = "TrackSumPt";
    if(m_jetAlgo=="AntiKt4EMTopo" || m_jetAlgo=="AntiKt4LCTopo") TrackSumPtStr = "DFCommonJets_TrackSumPt";
    if( !jet.getAttribute<float>(TrackSumMassStr,trackSumMass) ) {
      if(!m_combination){
        //ATH_MSG_WARNING("Failed to retrieve TrackSumMass! Track Assisted Mass Correction will NOT be applied\n\n");
        if(m_warning_counter_mTACorr==0) ATH_MSG_WARNING("Failed to retrieve TrackSumMass! Track Assisted Mass Correction will NOT be applied");
        m_warning_counter_mTACorr++;
        return StatusCode::SUCCESS;
      } else{
        ATH_MSG_FATAL("Failed to retrieve TrackSumMass! Mass Combination can NOT be performed. Aborting.");
	return StatusCode::FAILURE;
      }
    }
    float trackSumPt;
    if( !jet.getAttribute<float>(TrackSumPtStr,trackSumPt) ) {
      if(!m_combination){
        //ATH_MSG_WARNING("Failed to retrieve TrackSumPt! Track Assisted Mass Correction will NOT be applied\n\n");
        if(m_warning_counter_mTACorr==0) ATH_MSG_WARNING("Failed to retrieve TrackSumPt! Track Assisted Mass Correction will NOT be applied");
        m_warning_counter_mTACorr++;
        return StatusCode::SUCCESS;
      } else{
        ATH_MSG_FATAL("Failed to retrieve TrackSumPt! Mass Combination can NOT be performed. Aborting.");
	return StatusCode::FAILURE;
      }
    }
    pT_corr = jetStartP4.pt();
    float mTA;
    if(trackSumPt==0) mTA = 0;
    else{mTA = (jetStartP4.pt()/trackSumPt)*trackSumMass;}
    if(mTA<0) mTA = 0;
    mass_corr = mTA;
    if ( absdetectorEta < m_massEtaBins.back() && nconstituents>1 ) { //Fiducial Cuts
      for (uint i=0; i<m_massEtaBins.size()-1; ++i) {
        if(absdetectorEta >= m_massEtaBins[i] && absdetectorEta < m_massEtaBins[i+1]) etabin = i;
      }
      if (etabin< 0){
        ATH_MSG_FATAL("There was a problem determining the eta bin to use for the track assisted mass correction");
	if(m_combination) return StatusCode::FAILURE;
      }
      
      mass_corr = mTA/getTrackAssistedMassCorr( jetStartP4.pt()/m_GeV, mTA/m_GeV, etabin );
      if(!m_pTfixed) pT_corr = sqrt(jetStartP4.e()*jetStartP4.e()-mass_corr*mass_corr)/cosh( jetStartP4.eta() );
      else{E_corr  = sqrt(jetStartP4.P()*jetStartP4.P()+mass_corr*mass_corr);}
    }
    else{
      mTA       = 0;
      mass_corr = 0;
      if(!m_pTfixed) pT_corr = jetStartP4.e()/cosh( jetStartP4.eta() );
      else{E_corr = jetStartP4.P();}
    }

    // For combination
    m_ta = mass_corr;

    if(!m_combination){
      //Transfer calibrated track assisted mass property to the Jet object
      jet.setAttribute<float>("JetTrackAssistedMassUnCalibrated",mTA);
      jet.setAttribute<float>("JetTrackAssistedMassCalibrated",mass_corr);
      if(!m_pTfixed) jet.setAttribute<float>("JetpTCorrByCalibratedTAMass",pT_corr);
      else{jet.setAttribute<float>("JetECorrByCalibratedTAMass",E_corr);}
    }

    if(m_combination){
    
      //Transfer calibrated track assisted mass property to the Jet object
      xAOD::JetFourMom_t calibP4_ta = jet.jetP4();
      TLorentzVector TLVjet_ta;
      TLVjet_ta.SetPtEtaPhiM( pT_corr, jetStartP4.eta(), jetStartP4.phi(), mass_corr );
      calibP4_ta.SetPxPyPzE( TLVjet_ta.Px(), TLVjet_ta.Py(), TLVjet_ta.Pz(), TLVjet_ta.E() );
      jet.setAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA",calibP4_ta);

      float  m_comb  = m_calo;  // combined mass
      double pT_comb = pT_calo;

      // if one of the mass is null, use the other one
      if( (m_comb==0) || (mass_corr==0) ) { 
        m_comb = mass_corr+m_comb;
      }
      else {
        // Determine mass combination eta bin to use
        int etabin=-99;
        if (m_massCombinationEtaBins.size()==0 || m_caloResolutionMassCombination.size() != m_massCombinationEtaBins.size()-1){
	  ATH_MSG_FATAL("Please check that the mass combination eta binning is properly set in your config file");
	  return StatusCode::FAILURE;
	}
        if (m_massCombinationEtaBins.size()==0 || m_taResolutionMassCombination.size() != m_massCombinationEtaBins.size()-1){
	  ATH_MSG_FATAL("Please check that the mass combination eta binning is properly set in your config file");
	  return StatusCode::FAILURE;
	}

        if ( absdetectorEta < m_massCombinationEtaBins.back() ) {
          for (uint i=0; i<m_massCombinationEtaBins.size()-1; ++i) {
            if(absdetectorEta >= m_massCombinationEtaBins[i] && absdetectorEta < m_massCombinationEtaBins[i+1]) etabin = i;
          }
          if (etabin< 0){
	    ATH_MSG_FATAL("There was a problem determining the eta bin to use for the mass combination");
	    return StatusCode::FAILURE;
	  }
	  const double relCalo = getRelCalo( pT_calo/m_GeV, m_calo/pT_calo, etabin );
	  const double relTA   = getRelTA( pT_calo/m_GeV, m_ta/pT_calo, etabin );
	  const double rho     = ( m_useCorrelatedWeights ? getRho( pT_calo/m_GeV, m_calo/pT_calo, etabin ) : 0);
          // Watch for division by zero
          if(m_useCorrelatedWeights && (relCalo*relCalo + relTA*relTA - 2 * rho* relCalo * relTA == 0)){
            ATH_MSG_ERROR("Encountered division by zero when calculating mass combination weight using correlated weights");
            return StatusCode::FAILURE;
          }
	  const double Weight = ( relTA*relTA - rho *relCalo*relTA ) / ( relCalo*relCalo + relTA*relTA - 2 * rho* relCalo * relTA );
  	  m_comb =  ( m_calo * Weight ) + ( m_ta * ( 1 - Weight) );
	  // Protection
	  if(m_comb>jetStartP4.e()) m_comb = m_calo;
	  else if(!m_pTfixed) pT_comb = sqrt(jetStartP4.e()*jetStartP4.e()-m_comb*m_comb)/cosh( jetStartP4.eta() );
        }
      }


      TLorentzVector TLVjet;
      TLVjet.SetPtEtaPhiM( pT_comb, jetStartP4.eta(), jetStartP4.phi(), m_comb );
      calibP4.SetPxPyPzE( TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E() );
  
      //Transfer calibrated jet properties to the Jet object
      jet.setAttribute<xAOD::JetFourMom_t>(m_jetOutScale.Data(),calibP4);
      jet.setJetP4( calibP4 );

      //Transfer calibrated calo mass property to the Jet object
      xAOD::JetFourMom_t calibP4_calo = jet.jetP4();
      calibP4_calo.SetCoordinates( pT_calo, jetStartP4.eta(), jetStartP4.phi(), m_calo );
      jet.setAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",calibP4_calo);

    }
  }

  return StatusCode::SUCCESS;

}


