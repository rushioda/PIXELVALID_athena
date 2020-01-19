/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ASG include(s)
#include "PathResolver/PathResolver.h"

// xAOD include(s)
#include "xAODTracking/TrackParticle.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauxAODHelpers.h"

// local include(s)
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/HelperFunctions.h"

#include <fstream>

//root includes
#include <TFile.h>
#include <TTree.h>

using namespace tauRecTools;

//==============================================================================
// class TauTrackClassifier
//==============================================================================

//______________________________________________________________________________
TauTrackClassifier::TauTrackClassifier(const std::string& sName)
  : TauRecToolBase(sName)
  , m_mu(0.)
{
  declareProperty("Classifiers", m_vClassifier );
  declareProperty("ClassifierNames", m_vClassifierNames );
  declareProperty("TauTrackContainerName", m_tauTrackConName="TauTracks");
}

//______________________________________________________________________________
TauTrackClassifier::~TauTrackClassifier()
{
}

//______________________________________________________________________________
StatusCode TauTrackClassifier::initialize()
{
  ATH_MSG_DEBUG("intialize classifiers");

  #ifdef ROOTCORE
  for (auto cClassifierName : m_vClassifierNames){
    tauRecTools::TrackMVABDT* mva_tool = dynamic_cast<tauRecTools::TrackMVABDT*> (asg::ToolStore::get(cClassifierName));
    ToolHandle< tauRecTools::TrackMVABDT > handle(mva_tool);    
    m_vClassifier.push_back(handle);    
    ATH_CHECK(m_vClassifier.back()->initialize());//retrieve() does not seem to work
  }
  #else
  for (auto cClassifier : m_vClassifier){
    ATH_MSG_INFO("TauTrackClassifier tool : " << cClassifier );
    ATH_CHECK(cClassifier.retrieve());
  }
  #endif

 
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TauTrackClassifier::eventInitialize()
{
  // online MVA track classification uses mu-dependent cuts
  if ( inTrigger() ) {    
    m_mu = 0.;
    ATH_CHECK( tauEventData()->getObject("AvgInteractions", m_mu) );
  }
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauTrackClassifier::execute(xAOD::TauJet& xTau)
{
  xAOD::TauTrackContainer* tauTrackCon = 0;

  // trigger applies delta(z0) cut w.r.t. leading core track in addition to BDT cut
  const xAOD::TauTrack * lead_track = 0; 

  if (inTrigger()) {
    ATH_CHECK(tauEventData()->getObject( "TauTrackContainer", tauTrackCon ));

    float lead_track_pt = 0.;
    // this should select the leading 'classifiedCharged' track from TauTrackFinder, if any
    for (const auto track : xTau.tracks(xAOD::TauJetParameters::passTrkSelector))
      if(track->track()->p4().DeltaR(xTau.p4())<0.2)
	if (track->pt() > lead_track_pt) {
	  lead_track_pt = track->pt();
	  lead_track = track;
	}
    // if no track found, drop quality cut
    if(!lead_track)	
      for(const auto track : xTau.allTracks())
	if(track->track()->p4().DeltaR(xTau.p4())<0.2)
	  if(track->pt() > lead_track_pt) {
	    lead_track_pt = track->pt();
	    lead_track = track;
	  }
  }
  else {
    ATH_CHECK(evtStore()->retrieve(tauTrackCon, m_tauTrackConName));
  }

  std::vector<xAOD::TauTrack*> vTracks = xAOD::TauHelpers::allTauTracksNonConst(&xTau, tauTrackCon);
  for (xAOD::TauTrack* xTrack : vTracks)
  {
    // reset all track flags and set status to unclassified
    xTrack->setFlag(xAOD::TauJetParameters::classifiedCharged, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedConversion, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedIsolation, false);
    xTrack->setFlag(xAOD::TauJetParameters::classifiedFake, false);
    xTrack->setFlag(xAOD::TauJetParameters::unclassified, true);
    if(inTrigger()) xTrack->setFlag(xAOD::TauJetParameters::failTrackFilter, false);

    // execute the bdt track classifier 
    for (auto cClassifier : m_vClassifier) {
      if(!inTrigger()) 
	ATH_CHECK(cClassifier->classifyTrack(*xTrack, xTau));
      else
 	ATH_CHECK(cClassifier->classifyTriggerTrack(*xTrack, xTau, lead_track, m_mu));
    }
  }

  // if no core track passes nominal BDT cut, but one core track passes looser BDT cut, promote it as classifiedCharged
  // this recovers some efficiency for true 1p that would end up as 0p
  if(inTrigger()) {
    if(xTau.nTracks()==0 && xTau.nTracks(xAOD::TauJetParameters::failTrackFilter)==1)
      for (xAOD::TauTrack* xTrack : vTracks)
	if( xTrack->flag(xAOD::TauJetParameters::failTrackFilter) ) {
	  xTrack->setFlag(xAOD::TauJetParameters::classifiedCharged, true); 
	  // don't reset failTrackFilter bit, it should not affect track sorting
	}
  }

  std::vector< ElementLink< xAOD::TauTrackContainer > > &tauTrackLinks(xTau.allTauTrackLinksNonConst());
  std::sort(tauTrackLinks.begin(), tauTrackLinks.end(), sortTracks);
  float charge=0.0;
  for( const xAOD::TauTrack* trk : xTau.tracks(xAOD::TauJetParameters::classifiedCharged) ){
    charge += trk->track()->charge();
  }
  xTau.setCharge(charge);
  xTau.setDetail(xAOD::TauJetParameters::nChargedTracks, (int) xTau.nTracks());
  xTau.setDetail(xAOD::TauJetParameters::nIsolatedTracks, (int) xTau.nTracks(xAOD::TauJetParameters::classifiedIsolation));

  //set modifiedIsolationTrack
  for (xAOD::TauTrack* xTrack : vTracks) {
    if( not xTrack->flag(xAOD::TauJetParameters::classifiedCharged) and 
	xTrack->flag(xAOD::TauJetParameters::passTrkSelector) ) xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, true);
    else xTrack->setFlag(xAOD::TauJetParameters::modifiedIsolationTrack, false);
  }
  xTau.setDetail(xAOD::TauJetParameters::nModifiedIsolationTracks, (int) xTau.nTracks(xAOD::TauJetParameters::modifiedIsolationTrack));


  return StatusCode::SUCCESS;
}

//==============================================================================
// class TrackMVABDT
//==============================================================================

//______________________________________________________________________________
TrackMVABDT::TrackMVABDT(const std::string& sName)
  : TauRecToolBase(sName)
  , m_sInputWeightsPath("")
  , m_fThreshold(0.)
  , m_iSignalType(xAOD::TauJetParameters::classifiedCharged)
  , m_iBackgroundType(xAOD::TauJetParameters::classifiedFake)
  , m_iExpectedFlag(xAOD::TauJetParameters::unclassified)
  , m_deltaZ0(0.)
  , m_rReader(0)
    //  , m_mParsedVarsBDT({})
  , m_mAvailableVars({})
{
  declareProperty( "InputWeightsPath", m_sInputWeightsPath );
  declareProperty( "Threshold", m_fThreshold );
  declareProperty( "BackgroundType" , m_iBackgroundType );
  declareProperty( "SignalType", m_iSignalType );
  declareProperty( "ExpectedFlag", m_iExpectedFlag );
  // for trigger
  declareProperty( "DeltaZ0", m_deltaZ0 );
  //  m_mParsedVarsBDT.clear();
}

//______________________________________________________________________________
TrackMVABDT::~TrackMVABDT()
{
}

//______________________________________________________________________________
StatusCode TrackMVABDT::finalize()
{
  delete m_rReader;
  for( std::pair<TString, float*> p : m_mAvailableVars ) delete p.second;
  m_mAvailableVars.clear();
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::initialize()
{
  if (!inTrigger()) {

    m_mAvailableVars= {
      {"TracksAuxDyn.tauPt", new float(0)}
      , {"TracksAuxDyn.jetSeedPt", new float(0)}
      , {"TracksAuxDyn.tauEta", new float(0)}
      , {"TracksAuxDyn.trackEta", new float(0)}
      , {"TracksAuxDyn.z0sinThetaTJVA", new float(0)}
      , {"TracksAuxDyn.rConv", new float(0)}
      , {"TracksAuxDyn.rConvII", new float(0)}
      , {"TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII", new float(0)}
      , {"TracksAuxDyn.DRJetSeedAxis", new float(0)}
      , {"TracksAuxDyn.dRJetSeedAxis", new float(0)}
      , {"TracksAux.d0", new float(0)}
      , {"TracksAux.qOverP", new float(0)}
      , {"TracksAux.theta", new float(0)}
      , {"TracksAux.eProbabilityHT", new float(0)}
      , {"TracksAux.numberOfInnermostPixelLayerHits", new float(0)}
      , {"TracksAux.numberOfPixelHits", new float(0)}
      , {"TracksAux.numberOfPixelDeadSensors", new float(0)}
      , {"TracksAux.numberOfPixelSharedHits", new float(0)}
      , {"TracksAux.numberOfSCTHits", new float(0)}
      , {"TracksAux.numberOfSCTDeadSensors", new float(0)}
      , {"TracksAux.numberOfSCTSharedHits", new float(0)}
      , {"TracksAux.numberOfTRTHighThresholdHits", new float(0)}
      , {"TracksAux.numberOfTRTHits", new float(0)}
      , {"TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors", new float(0)}
      , {"TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors+TracksAux.numberOfSCTHits+TracksAux.numberOfSCTDeadSensors", new float(0)}
      
      , {"TauTracksAuxDyn.tauPt", new float(0)}
      , {"TauTracksAuxDyn.jetSeedPt", new float(0)}
      , {"TauTracksAuxDyn.tauEta", new float(0)}
      , {"TauTracksAuxDyn.trackEta", new float(0)}
      , {"TauTracksAuxDyn.z0sinThetaTJVA", new float(0)}
      , {"TauTracksAuxDyn.rConv", new float(0)}
      , {"TauTracksAuxDyn.rConvII", new float(0)}
      , {"TauTracksAuxDyn.dRJetSeedAxis", new float(0)}
      , {"TauTracksAuxDyn.d0", new float(0)}
      , {"TauTracksAuxDyn.qOverP", new float(0)}
      , {"TauTracksAuxDyn.theta", new float(0)}
      , {"TauTracksAuxDyn.eProbabilityHT", new float(0)}
      , {"TauTracksAuxDyn.numberOfInnermostPixelLayerHits", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelHits", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelDeadSensors", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelSharedHits", new float(0)}
      , {"TauTracksAuxDyn.numberOfSCTHits", new float(0)}
      , {"TauTracksAuxDyn.numberOfSCTDeadSensors", new float(0)}
      , {"TauTracksAuxDyn.numberOfSCTSharedHits", new float(0)}
      , {"TauTracksAuxDyn.numberOfTRTHighThresholdHits", new float(0)}
      , {"TauTracksAuxDyn.numberOfTRTHits", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors", new float(0)}
            
      , {"1/(TauTracksAuxDyn.trackPt)", new float(0)}
      , {"fabs(TauTracksAuxDyn.qOverP)", new float(0)}
      , {"TauTracksAuxDyn.numberOfContribPixelLayers", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors+TauTracksAuxDyn.numberOfSCTHoles", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelHoles", new float(0)}
      , {"TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHoles", new float(0)}
      , {"TauTracksAuxDyn.numberOfSCTHoles", new float(0)}
      , {"TauTracksAux.pt", new float(0)}
    };
  } 
  else {

    m_mAvailableVars = {
      {"tau_eta", new float(0)}, 
      {"track_delta_d0", new float(0)},
      {"track_dR_leadTrack", new float(0)},
      {"track_dR_tau", new float(0)},
      {"log(tau_pt/track_pt)", new float(0)},
      {"track_nIBLHitsAndExp", new float(0)},
      {"track_isgood", new float(0)},
      {"track_nSCTHits", new float(0)},
      {"track_nPiHits", new float(0)},
    };

  }
    
  ATH_CHECK(addWeightsFile());
  
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::classifyTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau)
{
  ATH_CHECK( setVars(xTrack, xTau) );

  if (xTrack.flag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag)==false)
    return StatusCode::SUCCESS;
  
  double dValue = m_rReader->GetClassification();
  
  xTrack.setFlag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag, false);
  if (m_fThreshold < dValue)
    xTrack.setFlag((xAOD::TauJetParameters::TauTrackFlag) m_iSignalType, true);
  else 
    xTrack.setFlag((xAOD::TauJetParameters::TauTrackFlag) m_iBackgroundType, true);

  xTrack.addBdtScore(dValue);

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::classifyTriggerTrack(xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track, double mu)
{
  if (xTrack.flag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag)==false)
    return StatusCode::SUCCESS;
  xTrack.setFlag((xAOD::TauJetParameters::TauTrackFlag) m_iExpectedFlag, false);

  // the track classification logic for trigger is different than for offline
  // the BDT score is only used for core tracks
  // if no core track passes the nominal BDT cut, try looser mu-depdent cut
  // all tracks should pass a mu-dependent tight delta(z0) cut
  // the mu-dependent cuts provide a reasonably flat efficiency vs mu for tau tracks

  double BDTscore = -1.;

  float dz0_cut_tight = std::max( m_deltaZ0*(1.-mu/120.), 0.2) ;

  // if candidate has no core track, still process isolation tracks (which don't use the BDT)
  float delta_z0 = lead_track ? abs(lead_track->track()->z0() - xTrack.track()->z0()) : -1.;
  
  if(delta_z0 > dz0_cut_tight) {
    xTrack.addBdtScore(BDTscore);
    return StatusCode::SUCCESS;
  }
     
  if( xTrack.track()->p4().DeltaR(xTau.p4()) < 0.2 ) {  
 
    ATH_CHECK( setTriggerVars(xTrack, xTau, lead_track) );

    BDTscore = m_rReader->GetClassification();

    if(BDTscore > m_fThreshold)
      xTrack.setFlag(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged, true);
    
    else {
      // real tau tracks don't have BDTscore < 0.2
      float BDTcut_loose = std::max( m_fThreshold*(1.-mu/100.), 0.2);
      
      // use existing failTrackFilter bit to flag tracks which only pass looser BDT cut
      // this should be safe as we don't run TauTrackFilter online
      if (BDTscore > BDTcut_loose)
	xTrack.setFlag(xAOD::TauJetParameters::TauTrackFlag::failTrackFilter, true);
    }
  }
  else if( xTrack.track()->p4().DeltaR(xTau.p4()) < 0.4) { 
    if( xTrack.flag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelector) )
      xTrack.setFlag(xAOD::TauJetParameters::TauTrackFlag::classifiedIsolation, true);
  }
 
  xTrack.addBdtScore(BDTscore);

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::addWeightsFile()
{

  std::string sInputWeightsPath = find_file(m_sInputWeightsPath);
  ATH_MSG_DEBUG("InputWeightsPath: " << sInputWeightsPath);
  


  // if (m_mParsedVarsBDT.empty())
  // {
  //   ATH_CHECK(parseVariableContent());
      
  //   for (auto i : m_mParsedVarsBDT)
  //     ATH_MSG_DEBUG(i.first<<" "<<i.second);
    
  //   for (size_t i = 0; i < m_mParsedVarsBDT.size(); i++){
  //     std::string sVarName = m_mParsedVarsBDT[i];
  //     if (m_mAvailableVars.find(sVarName) == m_mAvailableVars.end())
  //     {
  //       ATH_MSG_ERROR("Variable "<<sVarName<<" not in map of available variables");
  //       return StatusCode::FAILURE;
  //     }
  //     reader->AddVariable( sVarName, &(m_mAvailableVars[sVarName]));
  //   }
  // }

  m_rReader = tauRecTools::configureMVABDT( m_mAvailableVars, sInputWeightsPath.c_str() );
  if(m_rReader==0) {
    ATH_MSG_FATAL("Couldn't configure MVA");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::parseVariableContent()
{
  // // example     <Variable VarIndex="0" Expression="TracksAuxDyn.tauPt" Label="TracksAuxDyn.tauPt" Title="tauPt" Unit="" Internal="TracksAuxDyn.tauPt" Type="F" Min="1.50000762e+04" Max="5.32858625e+05"/>
  // std::string sLine;
  // std::ifstream sFileStream (m_sInputWeightsPath);
  // if (sFileStream.is_open())
  // {
  //   while ( getline (sFileStream,sLine) )
  //   {
  //     if (sLine.find("/Variables") != std::string::npos)
  //       break;
  //     size_t iPosVarindex = sLine.find("VarIndex=");
	
  //     if ( iPosVarindex == std::string::npos )
  //       continue;
	
  //     iPosVarindex += 10;
	
  //     size_t iPosVarindexEnd = sLine.find("\"",iPosVarindex);
  //     size_t iPosExpression = sLine.find("Expression=")+12;
  //     size_t iPosExpressionEnd = sLine.find("\"",iPosExpression);
	
  //     int iVarIndex = std::stoi(sLine.substr(iPosVarindex, iPosVarindexEnd-iPosVarindex));
  //     std::string sExpression = sLine.substr(iPosExpression, iPosExpressionEnd-iPosExpression);
	
  //     m_mParsedVarsBDT.insert(std::pair<int, std::string >(iVarIndex,sExpression));
  //   }
  //   sFileStream.close();
  //   return StatusCode::SUCCESS;
  // }

  // ATH_MSG_ERROR("Unable to open file "<<m_sInputWeightsPath);
  // return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TrackMVABDT::setVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau)
{
  const xAOD::TrackParticle* xTrackParticle = xTrack.track();

  uint8_t iTracksNumberOfInnermostPixelLayerHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNumberOfInnermostPixelLayerHits, xAOD::numberOfInnermostPixelLayerHits), StatusCode::FAILURE );
  uint8_t iTracksNPixelHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNPixelHits, xAOD::numberOfPixelHits), StatusCode::FAILURE );
  uint8_t iTracksNPixelSharedHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNPixelSharedHits, xAOD::numberOfPixelSharedHits), StatusCode::FAILURE );
  uint8_t iTracksNPixelDeadSensors = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNPixelDeadSensors, xAOD::numberOfPixelDeadSensors), StatusCode::FAILURE );
  uint8_t iTracksNSCTHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNSCTHits, xAOD::numberOfSCTHits), StatusCode::FAILURE );
  uint8_t iTracksNSCTSharedHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNSCTSharedHits, xAOD::numberOfSCTSharedHits), StatusCode::FAILURE );
  uint8_t iTracksNSCTDeadSensors = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iTracksNSCTDeadSensors, xAOD::numberOfSCTDeadSensors), StatusCode::FAILURE );
  uint8_t iTracksNTRTHighThresholdHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue( iTracksNTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits), StatusCode::FAILURE );
  uint8_t iTracksNTRTHits = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue( iTracksNTRTHits, xAOD::numberOfTRTHits), StatusCode::FAILURE );
  uint8_t iNumberOfContribPixelLayers = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iNumberOfContribPixelLayers, xAOD::numberOfContribPixelLayers), StatusCode::FAILURE );
  uint8_t iNumberOfPixelHoles = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iNumberOfPixelHoles, xAOD::numberOfPixelHoles), StatusCode::FAILURE );
  uint8_t iNumberOfSCTHoles = 0; TRT_CHECK_BOOL( xTrackParticle->summaryValue(iNumberOfSCTHoles, xAOD::numberOfSCTHoles), StatusCode::FAILURE );
  
  float fTracksNumberOfInnermostPixelLayerHits = (float)iTracksNumberOfInnermostPixelLayerHits;
  float fTracksNPixelHits = (float)iTracksNPixelHits;
  float fTracksNPixelDeadSensors = (float)iTracksNPixelDeadSensors;
  float fTracksNPixelSharedHits = (float)iTracksNPixelSharedHits;
  float fTracksNSCTHits = (float)iTracksNSCTHits;
  float fTracksNSCTDeadSensors = (float)iTracksNSCTDeadSensors;
  float fTracksNSCTSharedHits = (float)iTracksNSCTSharedHits;
  float fTracksNTRTHighThresholdHits = (float)iTracksNTRTHighThresholdHits;
  float fTracksNTRTHits = (float)iTracksNTRTHits;
  
  float fTracksNPixHits = fTracksNPixelHits + fTracksNPixelDeadSensors;
  float fTracksNSiHits = fTracksNPixelHits + fTracksNPixelDeadSensors + fTracksNSCTHits + fTracksNSCTDeadSensors;
  
  float fTracksEProbabilityHT; TRT_CHECK_BOOL( xTrackParticle->summaryValue( fTracksEProbabilityHT, xAOD::eProbabilityHT), StatusCode::FAILURE );
  
  float fNumberOfContribPixelLayers = float(iNumberOfContribPixelLayers);
  float fNumberOfPixelHoles = float(iNumberOfPixelHoles);
  float fNumberOfSCTHoles = float(iNumberOfSCTHoles);
  
  setVar("TracksAuxDyn.jetSeedPt") = xTau.ptJetSeed();
  setVar("TracksAuxDyn.tauPt") = xTau.ptIntermediateAxis();
  setVar("TracksAuxDyn.tauEta") = xTau.etaIntermediateAxis();
  setVar("TracksAuxDyn.z0sinThetaTJVA") = xTrack.z0sinThetaTJVA(xTau);
  setVar("TracksAuxDyn.rConv") = xTrack.rConv(xTau);
  setVar("TracksAuxDyn.rConvII") = xTrack.rConvII(xTau);
  setVar("TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII") = xTrack.rConv(xTau)/xTrack.rConvII(xTau);
  setVar("TracksAuxDyn.DRJetSeedAxis") = xTrack.dRJetSeedAxis(xTau);
  setVar("TracksAuxDyn.dRJetSeedAxis") = xTrack.dRJetSeedAxis(xTau);
  setVar("TracksAuxDyn.trackEta") = xTrackParticle->eta();
  setVar("TracksAux.d0") = xTrackParticle->d0();
  setVar("TracksAux.qOverP") = xTrackParticle->qOverP();
  setVar("TracksAux.theta") = xTrackParticle->theta();
  setVar("TracksAux.eProbabilityHT") = fTracksEProbabilityHT;
  setVar("TracksAux.numberOfInnermostPixelLayerHits") = fTracksNumberOfInnermostPixelLayerHits;
  setVar("TracksAux.numberOfPixelHits") = fTracksNPixelHits;
  setVar("TracksAux.numberOfPixelDeadSensors") = fTracksNPixelDeadSensors;
  setVar("TracksAux.numberOfPixelSharedHits") = fTracksNPixelSharedHits;
  setVar("TracksAux.numberOfSCTHits") = fTracksNSCTHits;
  setVar("TracksAux.numberOfSCTDeadSensors") = fTracksNSCTDeadSensors;
  setVar("TracksAux.numberOfSCTSharedHits") = fTracksNSCTSharedHits;
  setVar("TracksAux.numberOfTRTHighThresholdHits") = fTracksNTRTHighThresholdHits;
  setVar("TracksAux.numberOfTRTHits") = fTracksNTRTHits;
  setVar("TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors") = fTracksNPixHits;
  setVar("TracksAux.numberOfPixelHits+TracksAux.numberOfPixelDeadSensors+TracksAux.numberOfSCTHits+TracksAux.numberOfSCTDeadSensors") = fTracksNSiHits;
  
  setVar("TauTracksAuxDyn.jetSeedPt") = xTau.ptJetSeed();
  setVar("TauTracksAuxDyn.tauPt") = xTau.ptIntermediateAxis();
  setVar("TauTracksAuxDyn.tauEta") = xTau.etaIntermediateAxis();
  setVar("TauTracksAuxDyn.z0sinThetaTJVA") = xTrack.z0sinThetaTJVA(xTau);
  setVar("TauTracksAuxDyn.rConv") = xTrack.rConv(xTau);
  setVar("TauTracksAuxDyn.rConvII") = xTrack.rConvII(xTau);
  setVar("TauTracksAuxDyn.rConv/TauTracksAuxDyn.rConvII") = xTrack.rConv(xTau)/xTrack.rConvII(xTau);
  setVar("TauTracksAuxDyn.dRJetSeedAxis") = xTrack.dRJetSeedAxis(xTau);
  setVar("TauTracksAuxDyn.trackEta") = xTrackParticle->eta();
  setVar("TauTracksAuxDyn.d0") = xTrackParticle->d0();
  setVar("TauTracksAuxDyn.qOverP") = xTrackParticle->qOverP();
  setVar("TauTracksAuxDyn.theta") = xTrackParticle->theta();
  setVar("TauTracksAuxDyn.eProbabilityHT") = fTracksEProbabilityHT;
  setVar("TauTracksAuxDyn.numberOfInnermostPixelLayerHits") = fTracksNumberOfInnermostPixelLayerHits;
  setVar("TauTracksAuxDyn.numberOfPixelHits") = fTracksNPixelHits;
  setVar("TauTracksAuxDyn.numberOfPixelDeadSensors") = fTracksNPixelDeadSensors;
  setVar("TauTracksAuxDyn.numberOfPixelSharedHits") = fTracksNPixelSharedHits;
  setVar("TauTracksAuxDyn.numberOfSCTHits") = fTracksNSCTHits;
  setVar("TauTracksAuxDyn.numberOfSCTDeadSensors") = fTracksNSCTDeadSensors;
  setVar("TauTracksAuxDyn.numberOfSCTSharedHits") = fTracksNSCTSharedHits;
  setVar("TauTracksAuxDyn.numberOfTRTHighThresholdHits") = fTracksNTRTHighThresholdHits;
  setVar("TauTracksAuxDyn.numberOfTRTHits") = fTracksNTRTHits;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors") = fTracksNPixHits;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors") = fTracksNSiHits;
  
  setVar("1/(TauTracksAuxDyn.trackPt)") = 1./xTrackParticle->pt();
  setVar("fabs(TauTracksAuxDyn.qOverP)") = std::abs(xTrackParticle->qOverP());
  setVar("TauTracksAuxDyn.numberOfContribPixelLayers") = fNumberOfContribPixelLayers;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles") = fTracksNPixHits+fNumberOfPixelHoles;
  setVar("TauTracksAuxDyn.numberOfPixelHits+TauTracksAuxDyn.numberOfPixelDeadSensors+TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHits+TauTracksAuxDyn.numberOfSCTDeadSensors+TauTracksAuxDyn.numberOfSCTHoles") = fTracksNSiHits+fNumberOfPixelHoles+fNumberOfSCTHoles;
  setVar("TauTracksAuxDyn.numberOfPixelHoles") = fNumberOfPixelHoles;
  setVar("TauTracksAuxDyn.numberOfPixelHoles+TauTracksAuxDyn.numberOfSCTHoles") = fNumberOfPixelHoles+fNumberOfSCTHoles;
  setVar("TauTracksAuxDyn.numberOfSCTHoles") = fNumberOfSCTHoles;
  setVar("TauTracksAux.pt") = xTrackParticle->pt();

  return StatusCode::SUCCESS;
} 

//______________________________________________________________________________
StatusCode TrackMVABDT::setTriggerVars(const xAOD::TauTrack& xTrack, const xAOD::TauJet& xTau, const xAOD::TauTrack* lead_track)
{
  const xAOD::TrackParticle* xTrackParticle = xTrack.track();
  
  // fill the number of hits variables
  uint8_t n_pix_hits = 0;
  uint8_t n_pix_dead = 0;
  uint8_t n_sct_hits = 0;
  uint8_t n_sct_dead = 0;
  uint8_t n_ibl_hits = 0;
  uint8_t n_ibl_exp  = 0;
  
  TRT_CHECK_BOOL(xTrackParticle->summaryValue(n_pix_hits, xAOD::numberOfPixelHits), StatusCode::FAILURE);
  TRT_CHECK_BOOL(xTrackParticle->summaryValue(n_pix_dead, xAOD::numberOfPixelDeadSensors), StatusCode::FAILURE);
  TRT_CHECK_BOOL(xTrackParticle->summaryValue(n_sct_hits, xAOD::numberOfSCTHits), StatusCode::FAILURE);
  TRT_CHECK_BOOL(xTrackParticle->summaryValue(n_sct_dead, xAOD::numberOfSCTDeadSensors), StatusCode::FAILURE);
  TRT_CHECK_BOOL(xTrackParticle->summaryValue(n_ibl_hits, xAOD::numberOfInnermostPixelLayerHits), StatusCode::FAILURE);
  TRT_CHECK_BOOL(xTrackParticle->summaryValue(n_ibl_exp,  xAOD::expectInnermostPixelLayerHit), StatusCode::FAILURE);
  
  float delta_d0 = 999.;
  float track_dR_leadTrack = 999.;
  
  if (lead_track) {
    delta_d0 = lead_track->track()->d0() - xTrackParticle->d0();
    track_dR_leadTrack = xTrackParticle->p4().DeltaR(lead_track->track()->p4());
  }
  
  // set the variables
  
  setVar("tau_eta") = xTau.eta();
  
  setVar("track_delta_d0") = delta_d0;
  
  setVar("track_dR_leadTrack") = track_dR_leadTrack;
  
  setVar("track_dR_tau") = xTrackParticle->p4().DeltaR(xTau.p4());
  
  setVar("log(tau_pt/track_pt)") = xTrack.pt()!=0. ? std::log(xTau.pt()/xTrack.pt()) : 10.;
  
  setVar("track_nIBLHitsAndExp") = n_ibl_exp ? (float)n_ibl_hits : 1.;
  
  setVar("track_isgood") = xTrack.flag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelector) ? 1. : 0.;
  
  setVar("track_nSCTHits") = (float)(n_sct_hits + n_sct_dead);
  
  setVar("track_nPiHits") = (float)(n_pix_hits + n_pix_dead);
 
  // for (auto eEntry : m_mAvailableVars)
  //   std::cout << eEntry.first<<": "<<eEntry.second<<"\n";

  return StatusCode::SUCCESS;
}
