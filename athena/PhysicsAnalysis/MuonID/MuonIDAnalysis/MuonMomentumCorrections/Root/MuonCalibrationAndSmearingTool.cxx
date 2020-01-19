/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

namespace CP {

MuonCalibrationAndSmearingTool::MuonCalibrationAndSmearingTool( const std::string& name ) :
  asg::AsgTool( name ),

  m_smearDeltaMS( 0. ), m_smearDeltaID( 0. ), m_smearDeltaCB( 0. ),
  m_Tsmear( 0 ), m_Tdet( 0 ), m_Tdata( 0 ), m_Trel( 0 ), m_Talgo( 0 ), m_detRegion( 0 ),

  m_useNsigmaForICombine( 0. ),

  m_ptms( 0. ), m_ptid( 0. ), m_ptcb( 0. ), m_eta( 0. ), m_phi( 0. ), 
  m_weightMS( 0. ), m_weightID( 0. ),
  m_g0( 0. ), m_g1( 0. ), m_g2( 0. ), m_g3( 0. ), m_g4( 0. ), m_charge( 0. ),

  m_loadNames( false ), m_nb_regions( 0. ), m_doMacroRegions( false ) {

  declareProperty( "Year", m_year = "Data16" );
  declareProperty( "Algo", m_algo = "muons" );
  declareProperty( "SmearingType", m_type = "q_pT" );
  //  declareProperty( "Release", m_release = "Recs2016_08_07" );
  declareProperty( "Release", m_release = "Recs2016_15_07" );
  declareProperty( "ToroidOff", m_toroidOff = false );
  declareProperty( "FilesPath", m_FilesPath = "" );

  m_currentParameters = NULL; 
  m_scaleRegion = -1; 

  m_useExternalSeed = false;

}

MuonCalibrationAndSmearingTool::MuonCalibrationAndSmearingTool( const MuonCalibrationAndSmearingTool& tool ) : 
  asg::AsgTool( tool.name() + "Copy" ),
  m_useExternalSeed( tool.m_useExternalSeed ),

  m_smearDeltaMS( tool.m_smearDeltaMS ),
  m_smearDeltaID( tool.m_smearDeltaID ),
  m_smearDeltaCB( tool.m_smearDeltaCB ),
  m_Tsmear( tool.m_Tsmear ),
  m_Tdet( tool.m_Tdet ),
  m_Tdata( tool.m_Tdata ),
  m_Trel( tool.m_Trel ),
  m_Talgo( tool.m_Talgo ),
  m_detRegion( tool.m_detRegion ),

  m_useNsigmaForICombine( tool.m_useNsigmaForICombine ),

  m_ptms( tool.m_ptms ),
  m_ptid( tool.m_ptid ),
  m_ptcb( tool.m_ptcb ),
  m_eta( tool.m_eta ),
  m_phi( tool.m_phi ),
  m_scale_ID( tool.m_scale_ID ),
  m_enLoss_MS( tool.m_enLoss_MS ),
  m_scale_MS( tool.m_scale_MS ),
  m_scale_CB( tool.m_scale_CB ),

  m_scaleSyst_ID( tool.m_scaleSyst_ID ),
  m_enLossSyst_MS( tool.m_enLossSyst_MS ),
  m_scaleSyst_MS( tool.m_scaleSyst_MS ),
  m_scaleSyst_CB( tool.m_scaleSyst_CB ),

  m_scaleSystUp_ID( tool.m_scaleSystUp_ID ),
  m_enLossSystUp_MS( tool.m_enLossSystUp_MS ),
  m_scaleSystUp_MS( tool.m_scaleSystUp_MS ),

  m_scaleSystDw_ID( tool.m_scaleSystDw_ID ),
  m_enLossSystDw_MS( tool.m_enLossSystDw_MS ),
  m_scaleSystDw_MS( tool.m_scaleSystDw_MS ),

  m_p1_ID( tool.m_p1_ID ),
  m_p2_ID( tool.m_p2_ID ),
  m_p2_ID_TAN( tool.m_p2_ID_TAN ),
  m_p0_MS( tool.m_p0_MS ),
  m_p1_MS( tool.m_p1_MS ),
  m_p2_MS( tool.m_p2_MS ),

  m_E_p1_ID( tool.m_E_p1_ID ),
  m_E_p2_ID( tool.m_E_p2_ID ),
  m_E_p2_ID_TAN( tool.m_E_p2_ID_TAN ),
  m_E_p0_MS( tool.m_E_p0_MS ),
  m_E_p1_MS( tool.m_E_p1_MS ),
  m_E_p2_MS( tool.m_E_p2_MS ),
  m_S_p1_ID( tool.m_S_p1_ID ),
  m_S_p2_ID( tool.m_S_p2_ID ),
  m_S_p2_ID_TAN( tool.m_S_p2_ID_TAN ),
  m_S_p0_MS( tool.m_S_p0_MS ),
  m_S_p1_MS( tool.m_S_p1_MS ),
  m_S_p2_MS( tool.m_S_p2_MS ),

  m_SUp_p1_ID( tool.m_SUp_p1_ID ),
  m_SUp_p2_ID( tool.m_SUp_p2_ID ),
  m_SUp_p2_ID_TAN( tool.m_SUp_p2_ID_TAN ),
  m_SUp_p0_MS( tool.m_SUp_p0_MS ),
  m_SUp_p1_MS( tool.m_SUp_p1_MS ),
  m_SUp_p2_MS( tool.m_SUp_p2_MS ),
  m_SDw_p1_ID( tool.m_SDw_p1_ID ),
  m_SDw_p2_ID( tool.m_SDw_p2_ID ),
  m_SDw_p2_ID_TAN( tool.m_SDw_p2_ID_TAN ),
  m_SDw_p0_MS( tool.m_SDw_p0_MS ),
  m_SDw_p1_MS( tool.m_SDw_p1_MS ),
  m_SDw_p2_MS( tool.m_SDw_p2_MS ),

  m_MC_p1_ID( tool.m_MC_p1_ID ),
  m_MC_p2_ID( tool.m_MC_p2_ID ),
  m_MC_p2_ID_TAN( tool.m_MC_p2_ID_TAN ),
  m_MC_p0_MS( tool.m_MC_p0_MS ),
  m_MC_p1_MS( tool.m_MC_p1_MS ),
  m_MC_p2_MS( tool.m_MC_p2_MS ),

  m_weightMS( tool.m_weightMS ),
  m_weightID( tool.m_weightID ),
  m_g0( tool.m_g0 ),
  m_g1( tool.m_g1 ),
  m_g2( tool.m_g2 ),
  m_g3( tool.m_g3 ),
  m_g4( tool.m_g4 ),
  m_charge( tool.m_charge ),

  m_names( tool.m_names ),
  m_loadNames( tool.m_loadNames ),
  m_nb_regions( tool.m_nb_regions ),
  m_eta_min( tool.m_eta_min ),
  m_eta_max( tool.m_eta_max ),
  m_phi_min( tool.m_phi_min ),
  m_phi_max( tool.m_phi_max ),

  m_doMacroRegions( tool.m_doMacroRegions ),
  m_MacroRegionIdxMap( tool.m_MacroRegionIdxMap ),
  m_MacroRegionName( tool.m_MacroRegionName ),
  m_MacroRegionInnerEta( tool.m_MacroRegionInnerEta ),

  m_scaleRegion( tool.m_scaleRegion ),

  m_Parameters( tool.m_Parameters ),
  m_currentParameters( NULL ) {

  declareProperty( "Year", m_year = "Data16" );
  declareProperty( "Algo", m_algo = "muons" );
  declareProperty( "SmearingType", m_type = "q_pT" );
  declareProperty( "Release", m_release = "Recs2016_15_07" );
  //declareProperty( "Release", m_release = "Recs2016_08_07" );
  //declareProperty( "Release", m_release = "PreRecs2016_05_23" );
  declareProperty( "ToroidOff", m_toroidOff = false );
  declareProperty( "FilesPath", m_FilesPath = "" );

}

StatusCode MuonCalibrationAndSmearingTool::initialize() {

  //::: Greet the user:
  ATH_MSG_INFO( "Initialising..." );

  if( SetData( m_year ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( SetAlgorithm( m_algo ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( SetRelease( m_release ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( SetType( m_type ) == StatusCode::FAILURE ) return StatusCode::FAILURE;

  ATH_MSG_DEBUG( "Checking Initialization - Year: " << m_year );
  ATH_MSG_DEBUG( "Checking Initialization - Algo: " << m_algo );
  ATH_MSG_DEBUG( "Checking Initialization - Type: " << m_type );
  ATH_MSG_DEBUG( "Checking Initialization - Release: " << m_release );
  
  std::string regionsPath;
  int regionMode = 0; // simple Eta Bins
  //::: Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
  if ( m_FilesPath == "" ) {
    if ( m_Trel <= MCAST::Release::Rel17 ) {
      regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/Regions.dat" );
    }
    else if ( m_Trel == MCAST::Release::Rel17_2_Repro ) {
      regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsRepro.dat" );
    }
    else if ( m_Trel == MCAST::Release::Rel17_2_Sum13 ) {
      regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsPhi18.dat" );
      regionMode = 2;//MS: 48Bins L/S split plus 11_15 and Feet in the BARREL. ID has values from 16 eta bins
      m_useNsigmaForICombine = 0;
    }
    else if ( m_Trel >= MCAST::Release::PreRec ) {
      regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsPhi18.dat" );
      regionMode = 1; 
      m_useNsigmaForICombine = 0;
    }

    //    else if ( m_Trel >= MCAST::Release::Recs2016_08_07 ) {
    //regionsPath = PathResolverFindCalibFile( "MuonMomentumCorrections/RegionsPhi18.dat" );
    //regionMode = 1;
    //m_useNsigmaForICombine = 0;
    //}

    else {
      ATH_MSG_ERROR( "Unknown release" );
      return StatusCode::FAILURE;
    }
  }
  else {
    regionsPath = m_FilesPath + "RegionsPhi18.dat";
    regionMode = 1; 
    m_useNsigmaForICombine = 0;
  }
  ATH_MSG_DEBUG( "Checking Initialization - Regions file: " << regionsPath );

  if( Regions( regionsPath, regionMode ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  
  if( FillValues() == StatusCode::FAILURE ) return StatusCode::FAILURE;

  if( !applySystematicVariation( SystematicSet() ) ) {
    ATH_MSG_ERROR( "Unable to run with no systematic" );
    return StatusCode::FAILURE;
  }
  SystematicRegistry& registry = SystematicRegistry::getInstance();
  if( registry.registerSystematics( *this ) != SystematicCode::Ok ) return StatusCode::FAILURE;

  //::: Return gracefully:
  return StatusCode::SUCCESS;
}

CorrectionCode MuonCalibrationAndSmearingTool::applyCorrection( xAOD::Muon& mu ) {

  ATH_MSG_VERBOSE( "Muon Type = " << mu.muonType() << " ( 0: Combined, 1: StandAlone, 2: SegmentTagged, 3: CaloTagged )" );
  ATH_MSG_VERBOSE( "Muon Author = " << mu.author() );

  //::: Set pt ID:
  ATH_MSG_VERBOSE( "Retrieving ElementLink to ID TrackParticle..." );
  ATH_MSG_VERBOSE( "Setting Pt  [ID]: if no track available, set to 0..." );
  ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"inDetTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "inDetTrackParticleLink" ) );
  ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() == NULL ) = " << ( mu.inDetTrackParticleLink() == NULL ) );
  ATH_MSG_VERBOSE( "mu.inDetTrackParticleLink() = " << mu.inDetTrackParticleLink() );
  ATH_MSG_VERBOSE( "( mu.inDetTrackParticleLink() ).isValid() = " << ( mu.inDetTrackParticleLink() ).isValid() );
  if( ( mu.inDetTrackParticleLink() ).isValid() ) {
    const ElementLink< xAOD::TrackParticleContainer >& id_track = mu.inDetTrackParticleLink();
    m_ptid = ( !id_track ) ? 0. : ( *id_track )->pt() / 1000.;
  }
  else m_ptid = 0.;

  //::: Set pt MS:
  ATH_MSG_VERBOSE( "Retrieving ElementLink to MS TrackParticle..." );
  ATH_MSG_VERBOSE( "Setting Pt  [MS]: if no track available, set to 0..." );
  ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"extrapolatedMuonSpectrometerTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "extrapolatedMuonSpectrometerTrackParticleLink" ) );
  //ElementLink< xAOD::TrackParticleContainer > track = mu.auxdata< ElementLink< xAOD::TrackParticleContainer > >( "extrapolatedMuonSpectrometerTrackParticleLink" );
  //ATH_MSG_VERBOSE( "track = " << track );
  //ATH_MSG_VERBOSE( "( *track )->pt() = " << ( *track )->pt() );
  ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() == NULL ) );
  ATH_MSG_VERBOSE( "mu.extrapolatedMuonSpectrometerTrackParticleLink() = " << mu.extrapolatedMuonSpectrometerTrackParticleLink() );
  ATH_MSG_VERBOSE( "( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() = " << ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() );
  if( ( mu.extrapolatedMuonSpectrometerTrackParticleLink() ).isValid() ) {
    const ElementLink< xAOD::TrackParticleContainer >& ms_track = mu.extrapolatedMuonSpectrometerTrackParticleLink();
    m_ptms = ( !ms_track ) ? 0. : ( *ms_track )->pt() / 1000.;
  }
  else m_ptms = 0.;

  //::: Set pt CB:
  ATH_MSG_VERBOSE( "Retrieving ElementLink to CB TrackParticle..." );
  ATH_MSG_VERBOSE( "Setting Pt  [CB]: if no track available, set to 0..." );
  ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"primaryTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "primaryTrackParticleLink" ) );
  ATH_MSG_VERBOSE( "( mu.primaryTrackParticleLink() == NULL ) = " << ( mu.primaryTrackParticleLink() == NULL ) );
  ATH_MSG_VERBOSE( "mu.primaryTrackParticleLink() = " << mu.primaryTrackParticleLink() );
  ATH_MSG_VERBOSE( "( mu.primaryTrackParticleLink() ).isValid() = " << ( mu.primaryTrackParticleLink() ).isValid() );
  if( ( mu.primaryTrackParticleLink() ).isValid() ) {
    const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.primaryTrackParticleLink();
    m_ptcb = ( !cb_track ) ? 0. : ( *cb_track )->pt() / 1000.;
  }
  else m_ptcb = 0.;

  //::: Set remaining muon information:
  //::: Check if muon has been already corrected! If so, take original pt...
  //ATH_MSG_VERBOSE( "Setting Pt  [CB]..." );
  //if( mu.isAvailable< float >( "OriginalMuonPt" ) && mu.auxdata< float >( "OriginalMuonPt" ) > 0 ) {
  //  m_ptcb = mu.auxdata< float >( "OriginalMuonPt" ) / 1000.;
  //  ATH_MSG_DEBUG( "Found \"OriginalMuonPt\" decoration, with value: " << m_ptcb * 1000. );
  //}
  //else {
  //  m_ptcb = mu.pt() / 1000.;
  //  mu.auxdata< float >( "OriginalMuonPt" ) = mu.pt();
  //  ATH_MSG_DEBUG( "Setting \"OriginalMuonPt\" decoration to value: " << mu.auxdata< float >( "OriginalMuonPt" ) );
  //}
  ATH_MSG_VERBOSE( "Setting Eta [CB]..." );
  m_eta = mu.eta();
  ATH_MSG_VERBOSE( "Setting Phi [CB]..." );
  m_phi = mu.phi();

  //::: Retrieve the event information:
  const xAOD::EventInfo* evtInfo = 0;
  ATH_MSG_VERBOSE( "Retrieving EventInfo from the EventStore..." );
  if( evtStore()->retrieve( evtInfo, "EventInfo" ).isFailure() ) {
    ATH_MSG_ERROR( "No EventInfo object could be retrieved" );
    ATH_MSG_ERROR( "Random number generation not configured correctly, impossible to determine if dealing with data or MC" );
    return CorrectionCode::Error;
  }
  ATH_MSG_VERBOSE( "Checking Simulation flag: " << evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) );

  if( !evtInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) {
    mu.auxdata< float >( "InnerDetectorPt" ) = m_ptid * 1000.;
    //::: Toroid-off specifics
    if( m_toroidOff ) {
      mu.auxdata< float >( "MuonSpectrometerPt" ) = 0.;
      mu.setP4( m_ptid * 1000., m_eta, m_phi );
    }
    else {
      mu.auxdata< float >( "MuonSpectrometerPt" ) = m_ptms * 1000.;
    }
    //::: SAF specifics
    if( mu.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon ) {
      ATH_MSG_VERBOSE( "Retrieving ElementLink to CB TrackParticle..." );
      ATH_MSG_VERBOSE( "Setting Pt  [CB]: if no track available, set to 0..." );
      ATH_MSG_VERBOSE( "mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( \"combinedTrackParticleLink\" ) = " << mu.isAvailable< ElementLink< xAOD::TrackParticleContainer > >( "combinedTrackParticleLink" ) );
      ATH_MSG_VERBOSE( "( mu.combinedTrackParticleLink() == NULL ) = " << ( mu.combinedTrackParticleLink() == NULL ) );
      ATH_MSG_VERBOSE( "mu.combinedTrackParticleLink() = " << mu.combinedTrackParticleLink() );
      ATH_MSG_VERBOSE( "( mu.combinedTrackParticleLink() ).isValid() = " << ( mu.combinedTrackParticleLink() ).isValid() );
      float temp_cb_pt = 0.;
      if( ( mu.combinedTrackParticleLink() ).isValid() ) {
        const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.combinedTrackParticleLink();
        temp_cb_pt = ( !cb_track ) ? 0. : ( *cb_track )->pt();
      }
      mu.setP4( temp_cb_pt, m_eta, m_phi );
    }
    return CorrectionCode::Ok;
  }

  //const ElementLink< xAOD::TrackParticleContainer >& cb_track = mu.combinedTrackParticleLink();
  //m_charge = ( *cb_track )->charge();
  m_charge = mu.charge();
  ATH_MSG_DEBUG( "Checking Input Muon Info -  Pt_ID: " << m_ptid * 1000. );
  ATH_MSG_DEBUG( "Checking Input Muon Info -  Pt_MS: " << m_ptms * 1000. );
  ATH_MSG_DEBUG( "Checking Input Muon Info -  Pt_CB: " << m_ptcb * 1000. );
  ATH_MSG_VERBOSE( "Checking Input Muon Info -    Eta: " << m_eta );
  ATH_MSG_VERBOSE( "Checking Input Muon Info -    Phi: " << m_phi );
  ATH_MSG_VERBOSE( "Checking Input Muon Info - Sector: " << mu.auxdata< unsigned char >( "primarySector" ) );
  ATH_MSG_VERBOSE( "Checking Input Muon Info - Charge: " << ( ( m_charge > 0 ) ? "+" : "-" ) );
  ATH_MSG_VERBOSE( "Checking Input Muon Info -  Pt_CB - Pt_ID: " << ( m_ptcb - m_ptid ) * 1000. );

  if( !m_useExternalSeed ) {
    //::: Get Event Number:
    const unsigned long long eventNumber = evtInfo ? evtInfo->eventNumber() : 0;
    //::: Construct a seed for the random number generator:
    const UInt_t seed = 1 + abs( mu.phi() ) * 1E6 + std::abs( mu.eta() ) * 1E3 + eventNumber;
    m_random3.SetSeed( seed );
    //m_random3.SetSeed(0);
  }

  m_smearDeltaMS = 0.;
  m_smearDeltaID = 0.;
  m_smearDeltaCB = 0.;
  m_detRegion = -1;

  //::: Getting detector region
  if( int( m_phi ) == DEFAULT_INIT_VAL && m_Trel == MCAST::Release::Rel17_2_Sum13 ) ATH_MSG_ERROR( "You need phi of the muon for after Rel17.2Sum13!" );
 
  if( int( m_phi ) == DEFAULT_INIT_VAL && m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
    //Lower release have just Eta bins, so usually the user doesn't input
    //Phi and 0 is a good default for these regions
    m_detRegion = GetRegion( m_eta, 0 );
  }
  else {
    m_detRegion = GetRegion( m_eta, m_phi );
  }
  if( m_detRegion == -1 ) {
    ATH_MSG_DEBUG( "Can't find correct detector region! Set smearing to 0." );
    mu.auxdata< float >( "InnerDetectorPt" ) = m_ptid * 1000.;
    mu.auxdata< float >( "MuonSpectrometerPt" ) = m_ptms * 1000.;
    return CorrectionCode::OutOfValidityRange;
  }
  
  //::: Getting scale region
  m_scaleRegion = GetScaleRegion( mu );
  m_g0 = m_random3.Gaus( 0, 1 );
  m_g1 = m_random3.Gaus( 0, 1 );
  m_g2 = m_random3.Gaus( 0, 1 );
  m_g3 = m_random3.Gaus( 0, 1 );
  m_g4 = m_random3.Gaus( 0, 1 );
  ATH_MSG_VERBOSE( "Checking Random Values - g_0: " << m_g0 ); 
  ATH_MSG_VERBOSE( "Checking Random Values - g_1: " << m_g1 ); 
  ATH_MSG_VERBOSE( "Checking Random Values - g_2: " << m_g2 ); 
  ATH_MSG_VERBOSE( "Checking Random Values - g_3: " << m_g3 ); 
  ATH_MSG_VERBOSE( "Checking Random Values - g_4: " << m_g4 ); 

  //::: Getting smearing values 
  //::: MS
  if( m_currentParameters->SmearTypeMS == MCAST::SystVariation::Default ) {
    m_smearDeltaMS = GetSmearing( MCAST::DetectorType::MS, mu );
  }
  else if( m_currentParameters->SmearTypeMS == MCAST::SystVariation::Up ) {
    m_smearDeltaMS = GetSystVariation( MCAST::DetectorType::MS, 1. );
  }
  else if( m_currentParameters->SmearTypeMS == MCAST::SystVariation::Down ) {
    m_smearDeltaMS = GetSystVariation( MCAST::DetectorType::MS, -1. );
  }
  else {
    ATH_MSG_ERROR( "Invalid value for m_currentParameters->SmearTypeMS" );
  }
  //::: ID
  if( m_currentParameters->SmearTypeID == MCAST::SystVariation::Default ) {
    m_smearDeltaID = GetSmearing( MCAST::DetectorType::ID, mu );
  }
  else if( m_currentParameters->SmearTypeID == MCAST::SystVariation::Up ) {
    m_smearDeltaID = GetSystVariation( MCAST::DetectorType::ID, 1. );
  }
  else if( m_currentParameters->SmearTypeID == MCAST::SystVariation::Down ) {
    m_smearDeltaID = GetSystVariation( MCAST::DetectorType::ID, -1. );
  }
  else {
    ATH_MSG_ERROR( "Invalid value for m_currentParameters->SmearTypeID" );
  }
  CalcCBWeights( mu );
  ATH_MSG_VERBOSE( "Checking Smearing - smearDeltaID: " << m_smearDeltaID );
  ATH_MSG_VERBOSE( "Checking Smearing - smearDeltaMS: " << m_smearDeltaMS );
  ATH_MSG_VERBOSE( "Checking Weights - weightID: " << m_weightID << " - fabs( weightID - 1 ): " << fabs( m_weightID - 1 ) );
  ATH_MSG_VERBOSE( "Checking Weights - weightMS: " << m_weightMS << " - fabs( weightMS - 1 ): " << fabs( m_weightMS - 1 ) );
  m_smearDeltaCB = m_smearDeltaID * m_weightID + m_smearDeltaMS * m_weightMS;

  //::: Calibrate the pt of the muon:
  double res_idPt = 1000. * CalculatePt( MCAST::DetectorType::ID, m_smearDeltaID, m_smearDeltaMS, m_currentParameters->Scale );
  double res_msPt = 1000. * CalculatePt( MCAST::DetectorType::MS, m_smearDeltaID, m_smearDeltaMS, m_currentParameters->Scale );
  double res_cbPt = 1000. * CalculatePt( MCAST::DetectorType::CB, m_smearDeltaID, m_smearDeltaMS, m_currentParameters->Scale );
  //::: Override combined momentum for special cases
  if( fabs( m_weightID - 1 ) < EPSILON ) res_cbPt = res_idPt; 
  if( fabs( m_weightMS - 1 ) < EPSILON ) res_cbPt = res_msPt; 
  //::: Using ToroidOff flag
  mu.auxdata< float >( "InnerDetectorPt" ) = res_idPt;
  if( m_toroidOff ) {
    mu.auxdata< float >( "MuonSpectrometerPt" ) = 0.;
    mu.setP4( res_idPt, m_eta, m_phi );
  }
  else {
    mu.auxdata< float >( "MuonSpectrometerPt" ) = res_msPt;
    mu.setP4( res_cbPt, m_eta, m_phi );
  }
  ATH_MSG_DEBUG( "Checking Output Muon Info - Pt_ID: " << res_idPt );
  ATH_MSG_DEBUG( "Checking Output Muon Info - Pt_MS: " << res_msPt );
  ATH_MSG_DEBUG( "Checking Output Muon Info - Pt_CB: " << res_cbPt );
  ATH_MSG_VERBOSE( "Checking Output Muon Info - Pt_CB - Pt_ID: " << res_cbPt - res_idPt );

  //::: Return gracefully:
  return CorrectionCode::Ok;

}

CorrectionCode MuonCalibrationAndSmearingTool::correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) {

  //::: A sanity check:
  if( output ) ATH_MSG_WARNING( "Non-null pointer received. " "There's a possible memory leak!" );

  //::: Create a new object:
  ATH_MSG_VERBOSE( "Going to create new xAOD::Muon..." );
  output = new xAOD::Muon();
  ATH_MSG_VERBOSE( "Calling makePrivateStore..." );
  output->makePrivateStore( input );

  //::: Use the other function to modify this object:
  ATH_MSG_VERBOSE( "Calling applyCorrection..." );
  return applyCorrection( *output );
}

bool MuonCalibrationAndSmearingTool::isAffectedBySystematic( const SystematicVariation& systematic ) const {

  SystematicSet sys = affectingSystematics();
  return sys.find( systematic ) != sys.end();

}

SystematicSet MuonCalibrationAndSmearingTool::affectingSystematics() const {

  SystematicSet result;

  //::: ID systematics
  result.insert( SystematicVariation( "MUONS_ID", 1 ) );
  result.insert( SystematicVariation( "MUONS_ID", -1 ) );

  //::: MS systematics
  result.insert( SystematicVariation( "MUONS_MS", 1 ) );
  result.insert( SystematicVariation( "MUONS_MS", -1 ) );

  //::: Scale systematics
  result.insert( SystematicVariation( "MUONS_SCALE", 1 ) );
  result.insert( SystematicVariation( "MUONS_SCALE", -1 ) );

  return result;

}

SystematicSet MuonCalibrationAndSmearingTool::recommendedSystematics() const {

  return affectingSystematics();

}

SystematicCode MuonCalibrationAndSmearingTool::applySystematicVariation( const SystematicSet& systConfig ) {

  //::: First check if we already know this systematic configuration
  boost::unordered_map< SystematicSet, ParameterSet >::iterator parIter = m_Parameters.find( systConfig );
  if( parIter != m_Parameters.end() ) {
    m_currentParameters = &parIter->second;
    return SystematicCode::Ok;
  }

  //::: Then check if it is actually supported
  static CP::SystematicSet affSysts = affectingSystematics();
  SystematicSet checkSysConf;
  if( !SystematicSet::filterForAffectingSystematics( systConfig, affSysts, checkSysConf ) ) {
    ATH_MSG_ERROR( "Passing unsupported systematic to the tool!" );
    return SystematicCode::Unsupported;
  }

  ParameterSet param;
  param.SmearTypeID = MCAST::SystVariation::Default; 
  param.SmearTypeMS = MCAST::SystVariation::Default;
  param.Scale = MCAST::SystVariation::Default;

  //::: ID systematics
  SystematicVariation syst = systConfig.getSystematicByBaseName( "MUONS_ID" );

  if( syst == SystematicVariation( "MUONS_ID", 1 ) ) {
    param.SmearTypeMS = MCAST::SystVariation::Default;
    param.SmearTypeID = MCAST::SystVariation::Up;
    param.Scale = MCAST::SystVariation::Default;
  }
  else if( syst == SystematicVariation( "MUONS_ID", -1 ) ) {
    //param.SmearTypeMS = m_smearDeltaMS;
    //param.SmearTypeID = GetSystVariation( MCAST::DetectorType::ID, -1. );
    param.SmearTypeMS = MCAST::SystVariation::Default;
    param.SmearTypeID = MCAST::SystVariation::Down;
    param.Scale = MCAST::SystVariation::Default;
  }
  else if( !syst.empty() ) return SystematicCode::Unsupported;

  //::: MS systematics
  syst = systConfig.getSystematicByBaseName( "MUONS_MS" );

  if( syst == SystematicVariation( "MUONS_MS", 1 ) ) {
    //param.SmearTypeMS = GetSystVariation( MCAST::DetectorType::MS, 1. );
    //param.SmearTypeID = m_smearDeltaID;
    param.SmearTypeMS = MCAST::SystVariation::Up;
    param.SmearTypeID = MCAST::SystVariation::Default;
    param.Scale = MCAST::SystVariation::Default;
  }
  else if( syst == SystematicVariation( "MUONS_MS", -1 ) ) {
    //param.SmearTypeID = m_smearDeltaID;
    //param.SmearTypeMS = GetSystVariation( MCAST::DetectorType::MS, -1. );
    param.SmearTypeMS = MCAST::SystVariation::Down;
    param.SmearTypeID = MCAST::SystVariation::Default;
    param.Scale = MCAST::SystVariation::Default;
  }
  else if( !syst.empty() ) return SystematicCode::Unsupported;

  //::: Scale systematics
  syst = systConfig.getSystematicByBaseName( "MUONS_SCALE" );

  if( syst == SystematicVariation( "MUONS_SCALE", 1 ) ) {
    param.SmearTypeMS = MCAST::SystVariation::Default;
    param.SmearTypeID = MCAST::SystVariation::Default;
    param.Scale = MCAST::SystVariation::Down;
  }
  else if( syst == SystematicVariation( "MUONS_SCALE", -1 ) ) {
    param.SmearTypeMS = MCAST::SystVariation::Default;
    param.SmearTypeID = MCAST::SystVariation::Default;
    param.Scale = MCAST::SystVariation::Up;
  }
  else if( !syst.empty() ) return SystematicCode::Unsupported;

  //:::
  ATH_MSG_DEBUG( "Systematic variation's parameters, SmearTypeID: " << param.SmearTypeID ); 
  ATH_MSG_DEBUG( "Systematic variation's parameters, SmearTypeMS: " << param.SmearTypeMS ); 
  ATH_MSG_DEBUG( "Systematic variation's parameters, Scale: " << param.Scale ); 
  //::: store this calibration for future use, and make it current
  m_currentParameters = &m_Parameters.insert( std::make_pair( systConfig, param ) ).first->second;
  return SystematicCode::Ok;

}

StatusCode MuonCalibrationAndSmearingTool::SetData( std::string data ) {

  if( data == "Data10" ) {
    m_Tdata = MCAST::DataType::Data10;
  }
  else if( data == "Data11" ) {
    m_Tdata = MCAST::DataType::Data11;
  }
  else if( data == "Data12" ) {
    m_Tdata = MCAST::DataType::Data12;
  }
  else if( data == "Data15" ) {
    m_Tdata = MCAST::DataType::Data15;
  }
  else if( data == "Data16" ) {
    m_Tdata = MCAST::DataType::Data16;
  }

  else {
    ATH_MSG_ERROR( "Unrecognized value for SetData" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

StatusCode MuonCalibrationAndSmearingTool::SetAlgorithm( std::string algo ) {

  if( algo == "muid" ) {
    m_Talgo = MCAST::AlgoType::Muid;
  }
  else if( algo == "staco" ) {
    m_Talgo = MCAST::AlgoType::Staco;
  }
  else if( algo == "muons" ) {
    m_Talgo = MCAST::AlgoType::Muons;
  }
  else {
    ATH_MSG_ERROR( "Unrecognized value for SetAlgorithm" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

StatusCode MuonCalibrationAndSmearingTool::SetType( std::string type ) {

  if( type == "pT" ) {
    m_Tsmear = MCAST::SmearingType::Pt;
    ATH_MSG_DEBUG( "Using " << type << " Smearing " );
  }
  else if( type == "q_pT" ) {
    m_Tsmear = MCAST::SmearingType::QoverPt;
    ATH_MSG_DEBUG( "Using " << type << " Smearing " );
  }
  else {
    ATH_MSG_ERROR( "Unrecognized value for SetType" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

StatusCode MuonCalibrationAndSmearingTool::SetRelease( std::string rel ) {

  if( rel == "Rel16.6" ) {
    m_Trel = MCAST::Release::Rel16_6;
  }
  else if( rel == "Rel17" ) {
    m_Trel = MCAST::Release::Rel17;
  }
  else if( rel == "Rel17.2" ) {
    m_Trel = MCAST::Release::Rel17_2;
  }
  else if( rel == "Rel17.2Repro" ) {
    m_Trel = MCAST::Release::Rel17_2_Repro;
  }
  else if( rel == "Rel17.2Sum13" ) {
    m_Trel = MCAST::Release::Rel17_2_Sum13;
  }
  else if( rel == "PreRecs" ) {
    m_Trel = MCAST::Release::PreRec;
  }
  else if( rel == "PreRecs2015_06_22" ) {
    m_Trel = MCAST::Release::PreRec_2015_06_22;
  }
  else if( rel == "PreRecs2015_08_06" ) {
    m_Trel = MCAST::Release::PreRec_2015_08_06;
  }
  else if( rel == "Recs2015_11_15" ) {
    m_Trel = MCAST::Release::Rec_2015_11_15;
  }
  else if( rel == "Recs2016_01_13" ) {
    m_Trel = MCAST::Release::Rec_2016_01_13;
  }
  else if( rel == "Recs2016_01_19" ) {
    m_Trel = MCAST::Release::Rec_2016_01_19;
  }
  else if( rel == "PreRecs2016_05_23" ) {
    m_Trel = MCAST::Release::PreRec_2016_05_23;
  }
  else if (rel == "Recs2016_08_07") {
    m_Trel = MCAST::Release::Recs2016_08_07;
  }
  else if (rel == "Recs2016_15_07") {
    m_Trel = MCAST::Release::Recs2016_08_07;
  }
  
  else {
    m_Trel = MCAST::Release::Recs2016_08_07;
    //ATH_MSG_ERROR( "Unrecognized value for SetRelease" );
    //return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

//double MuonCalibrationAndSmearingTool::ScaleApply( const double pt, const double S1, const double S2, double S, const double S_enLoss ) const {
double MuonCalibrationAndSmearingTool::ScaleApply( const double pt, double S, const double S_enLoss ) const {


  //::: Sanity checks:
  if( m_detRegion < 0 || m_detRegion >= m_nb_regions ) return pt;

  //::: default momentum scale (derived form template fits)
  return pt * S + S_enLoss;
  //bool apply_to_data = false;
  //return apply_to_data ? pt / S : ( pt * S + S_enLoss );

}


double MuonCalibrationAndSmearingTool::CalculatePt( const int DetType, const double inSmearID, const double inSmearMS, const double scaleVar ) const {

  double scaleID = 0., enLossCorrMS = 0., scaleMS = 0., scaleCB = 0.;//initialize all to 0
  //::: These are alternative scale corrections (KPKM,KC,K,C) they are != 0. if Tscale != SCALE_DEFAULT.
  //double s1_ID = 0., s2_ID = 0., s1_MS = 0., s2_MS = 0., s1_CB = 0., s2_CB = 0.;//Description of these is in ApplyScale

  if( fabs( scaleVar ) != 1. && scaleVar != 0. ) ATH_MSG_ERROR( "Unpredicted scale variation of Delta "<<scaleVar<<" sigmas!" );

  if( m_scale_ID[m_detRegion] != -1 ) {
    if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
      scaleID = scaleVar > 0. ? m_scaleSystUp_ID[m_detRegion] : m_scaleSystDw_ID[m_detRegion];
    }
    else {
      scaleID = m_scaleSyst_ID[m_detRegion];
    }
    scaleID = m_scale_ID[m_detRegion] + scaleVar*scaleID;
  }
  else {
    scaleID = ( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) ? 0. : 1.;
  }
  if( m_scale_MS[m_detRegion] != -1 ) {
    if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
      scaleMS = scaleVar > 0. ? m_scaleSystUp_MS[m_detRegion] : m_scaleSystDw_MS[m_detRegion];
      enLossCorrMS = scaleVar > 0. ? m_enLossSystUp_MS[m_detRegion] : m_enLossSystDw_MS[m_detRegion];
    }
    else {
      scaleMS = m_scaleSyst_MS[m_detRegion];
    }
    scaleMS =  m_scale_MS[m_detRegion] + scaleVar*scaleMS;
    if( true ) enLossCorrMS = m_enLoss_MS[m_detRegion] + scaleVar*enLossCorrMS;
  }
  else {
    scaleMS = ( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) ? 0. : 1.;
  }
  if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) { //NB: MCAST::Release::Rel17_2_Sum13 has scale parametrized around 0, not 1!!
    scaleID += 1.;
    scaleMS += 1.;
  }
  if( m_scale_CB[m_detRegion] != -1 ) {
    scaleCB = m_scale_CB[m_detRegion] + scaleVar * m_scaleSyst_CB[m_detRegion];
  }
  else {
    if( m_ptms ) scaleCB = ( scaleID * m_weightID + ( scaleMS + enLossCorrMS / m_ptms ) * m_weightMS );
    else         scaleCB = scaleID; // was scaleID * m_weightID
  }
  
  double tmpDelta = 1.;
  double outPtID = m_ptid, outPtMS = m_ptms, outPtCB = m_ptcb;
  if( DetType == MCAST::DetectorType::ID ) {
    ATH_MSG_VERBOSE( "Checking s1_ID = " << scaleID );
    ATH_MSG_VERBOSE( "Double-Checking outPtID = " << outPtID << " at first..." );
    if( outPtID == 0. ) return outPtID;
    //Load the ID scale and smearing that you will use
    ATH_MSG_VERBOSE( "Double-Checking int( inSmearID ) = " << int( inSmearID ) );
    ATH_MSG_VERBOSE( "Double-Checking DEFAULT_INIT_VAL = " << DEFAULT_INIT_VAL );
    tmpDelta = ( int( inSmearID ) == DEFAULT_INIT_VAL ) ? 1. + m_smearDeltaID : 1. + inSmearID;
    ATH_MSG_VERBOSE( "Double-Checking inSmearID = " << inSmearID );
    ATH_MSG_VERBOSE( "Double-Checking m_smearDeltaID = " << m_smearDeltaID );
    ATH_MSG_VERBOSE( "Double-Checking tmpDelta = " << tmpDelta );
    if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
      if( m_Tsmear == MCAST::SmearingType::Pt )  outPtID = outPtID * tmpDelta;
      if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtID = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtID / tmpDelta;
    }
    //outPtID = ScaleApply( fabs( outPtID ), s1_ID, s2_ID, scaleID );
    outPtID = ScaleApply( fabs( outPtID ), scaleID );
    if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
      if( m_Tsmear == MCAST::SmearingType::Pt )  outPtID = outPtID * tmpDelta;
      if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtID = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtID / tmpDelta;
    }
    return outPtID;
  }
  if( DetType == MCAST::DetectorType::MS ) {
    ATH_MSG_VERBOSE( "Checking s0_MS = " << enLossCorrMS );
    ATH_MSG_VERBOSE( "Checking s1_MS = " << scaleMS );
    ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at first..." );
    if( outPtMS == 0. ) return outPtMS;
    //Load the MS scale and smearing that you will use
    ATH_MSG_VERBOSE( "Double-Checking int( inSmearMS ) = " << int( inSmearMS ) );
    ATH_MSG_VERBOSE( "Double-Checking DEFAULT_INIT_VAL = " << DEFAULT_INIT_VAL );
    tmpDelta = ( int( inSmearMS ) == DEFAULT_INIT_VAL ) ? 1. + m_smearDeltaMS : 1. + inSmearMS;
    ATH_MSG_VERBOSE( "Double-Checking inSmearMS = " << inSmearMS );
    ATH_MSG_VERBOSE( "Double-Checking m_smearDeltaMS = " << m_smearDeltaMS );
    ATH_MSG_VERBOSE( "Double-Checking tmpDelta = " << tmpDelta );
    //In these releases the smearing was applied to the pt before the scale
    if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
      if( m_Tsmear == MCAST::SmearingType::Pt )  outPtMS = outPtMS * tmpDelta;
      if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtMS = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtMS / tmpDelta;
    }
    ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at second..." );
    //outPtMS = ScaleApply( fabs( outPtMS ), s1_MS, s2_MS, scaleMS, enLossCorrMS );
    outPtMS = ScaleApply( fabs( outPtMS ), scaleMS, enLossCorrMS );
    ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at third..." );
    if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
      if( m_Tsmear == MCAST::SmearingType::Pt )  outPtMS = outPtMS * tmpDelta;
      if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtMS = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtMS / tmpDelta;
    }
    ATH_MSG_VERBOSE( "Double-Checking outPtMS = " << outPtMS << " at fourth..." );
    return outPtMS;
  }
  if( DetType == MCAST::DetectorType::CB ) {
    if( int( inSmearID ) != DEFAULT_INIT_VAL && int( inSmearMS ) != DEFAULT_INIT_VAL ) {
      tmpDelta = 1. + inSmearID * m_weightID + inSmearMS * m_weightMS;
    }
    else if( int( inSmearID ) == DEFAULT_INIT_VAL && int( inSmearMS ) == DEFAULT_INIT_VAL ) {
      tmpDelta = 1. + m_smearDeltaCB;
    }
    else {
    }
    //In these releases the smearing was applied to the pt before the scale
    if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
      if( m_Tsmear == MCAST::SmearingType::Pt )  outPtCB = outPtCB * tmpDelta;
      if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtCB = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtCB / tmpDelta;
    }
    //outPtCB = ScaleApply( fabs( outPtCB ), s1_CB, s2_CB, scaleCB );
    outPtCB = ScaleApply( fabs( outPtCB ), scaleCB );
    if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
      if( m_Tsmear == MCAST::SmearingType::Pt )  outPtCB = outPtCB * tmpDelta;
      if( m_Tsmear == MCAST::SmearingType::QoverPt ) outPtCB = ( tmpDelta == 0 ) ? MCAST_MAX_PT : outPtCB / tmpDelta;
    }
    return outPtCB;
  }
  return 0.;

}

StatusCode MuonCalibrationAndSmearingTool::FillValues() {

  Clean();
  double tmpval;
  std::string tmpname;
  std::ifstream InValues;

  //::: Retrieving scale corrections!
  std::string scale_val;
  //::: Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
  if ( m_FilesPath == "" ) {
    if( m_Trel >= MCAST::Release::PreRec ) {
      scale_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Scale_" + m_algo + "_" + m_year + "_" + m_release + ".dat" );
    }
    else if( m_year == "Data11" && m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
      scale_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Data11_Scale_values_" + m_algo + "_" + m_release + ".dat" );
    }
    else {
      scale_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Scale_values_" + m_algo + "_" + m_release + ".dat" );
    }
  }
  else {
    scale_val = m_FilesPath + "Scale_" + m_algo + "_" + m_year + "_" + m_release + ".dat";
  }
  //if ( m_Trel >= MCAST::Release::PreRec ) scale_val = "Scales_Test.dat";
  ATH_MSG_DEBUG( "Checking Files - Scales: " << scale_val );

  InValues.open( scale_val.c_str() );
  int i = 0;
  if( !InValues.good() ) {
    ATH_MSG_ERROR( "File " << scale_val <<" not found!!" );
    return StatusCode::FAILURE;
  }
  else {
    while( InValues.good() && i < m_nb_regions ) {
      tmpval = 0;
      if( i == 0 ) {
        getline( InValues, tmpname );
      }
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        //These have a different order and Up/Dw split sys
        InValues>>tmpval;
        m_scale_ID.push_back( tmpval );
        InValues>>tmpval;
        m_enLoss_MS.push_back( tmpval );
        InValues>>tmpval;
        m_scale_MS.push_back( tmpval );
        InValues>>tmpval;
        m_scaleSystUp_ID.push_back( tmpval );
        InValues>>tmpval;
        m_enLossSystUp_MS.push_back( tmpval );
        InValues>>tmpval;
        m_scaleSystUp_MS.push_back( tmpval );
        InValues>>tmpval;
        m_scaleSystDw_ID.push_back( tmpval );
        InValues>>tmpval;
        m_enLossSystDw_MS.push_back( tmpval );
        InValues>>tmpval;
        m_scaleSystDw_MS.push_back( tmpval );
      }
      else {
        m_enLoss_MS.push_back( 0. );
        m_enLossSyst_MS.push_back( 0. );
        InValues>>tmpval;
        m_scale_ID.push_back( tmpval );
        InValues>>tmpval;
        m_scaleSyst_ID.push_back( tmpval );
        InValues>>tmpval;
        m_scale_MS.push_back( tmpval );
        InValues>>tmpval;
        m_scaleSyst_MS.push_back( tmpval );
      }
      //CB is always on the cfg file although rarely used
      InValues>>tmpval;
      m_scale_CB.push_back( tmpval );
      InValues>>tmpval;
      m_scaleSyst_CB.push_back( tmpval );
      i++;
    }
  }
  InValues.close();
  InValues.clear();
 
  //::: Retrieving data values
  std::string data_val;
  //::: Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
  if ( m_FilesPath == "" ) {
    if( m_Trel >= MCAST::Release::PreRec ) {
      data_val = PathResolverFindCalibFile( "MuonMomentumCorrections/Smearing_" + m_algo + "_" + m_year + "_" + m_release + ".dat" );
    }
    else {
      data_val = PathResolverFindCalibFile( "MuonMomentumCorrections/" + m_year + "_values_" + m_algo + "_" + m_release + ".dat" ); 
    }
  }
  else {
    data_val = m_FilesPath + "Smearing_" + m_algo + "_" + m_year + "_" + m_release + ".dat";
  }
  //if ( m_Trel >= MCAST::Release::Test ) data_val = "Smearings_Test.dat";
  ATH_MSG_DEBUG( "Checking Files - Data: " << data_val );

  InValues.open( data_val.c_str() );
  i = 0;
  if( !InValues.good() ) {
    ATH_MSG_ERROR( "File " << data_val << " not found!!" );
    return StatusCode::FAILURE;
  }
  else {
    while( InValues.good() && i < m_nb_regions ) {
      tmpval = 0;
      if( i == 0 ) {
        getline( InValues,tmpname );
      }
      InValues>>tmpval;
      m_p1_ID.push_back( tmpval );
      InValues>>tmpval;
      m_p2_ID.push_back( tmpval );
      InValues>>tmpval;
      m_p2_ID_TAN.push_back( tmpval );
      InValues>>tmpval;
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        m_p0_MS.push_back( tmpval );
        InValues>>tmpval;
      }
      else {
        m_p0_MS.push_back( 0 );
      }
      m_p1_MS.push_back( tmpval );
      InValues>>tmpval;
      m_p2_MS.push_back( tmpval );
      //Syst and stat uncertianties: they are split and symmetrized before Rel17.2Sum13, after
      // that, the stat is added in ^2 and they are split in Up/Dw
      if( m_Trel >= MCAST::Release::Rel17_2_Sum13 ) {
        InValues>>tmpval;
        m_SUp_p1_ID.push_back( tmpval );
        InValues>>tmpval;
        m_SUp_p2_ID.push_back( tmpval );
        InValues>>tmpval;
        m_SUp_p2_ID_TAN.push_back( tmpval );
        InValues>>tmpval;
        m_SUp_p0_MS.push_back( tmpval );
        InValues>>tmpval;
        m_SUp_p1_MS.push_back( tmpval );
        InValues>>tmpval;
        m_SUp_p2_MS.push_back( tmpval );
        InValues>>tmpval;
        m_SDw_p1_ID.push_back( tmpval );
        InValues>>tmpval;
        m_SDw_p2_ID.push_back( tmpval );
        InValues>>tmpval;
        m_SDw_p2_ID_TAN.push_back( tmpval );
        InValues>>tmpval;
        m_SDw_p0_MS.push_back( tmpval );
        InValues>>tmpval;
        m_SDw_p1_MS.push_back( tmpval );
        InValues>>tmpval;
        m_SDw_p2_MS.push_back( tmpval );
      }
      else {
        m_E_p0_MS.push_back( 0 );
        m_S_p0_MS.push_back( 0 );
        //Stat
        InValues>>tmpval;
        m_E_p1_ID.push_back( tmpval );
        InValues>>tmpval;
        m_E_p2_ID.push_back( tmpval );
        InValues>>tmpval;
        m_E_p2_ID_TAN.push_back( tmpval );
        InValues>>tmpval;
        m_E_p1_MS.push_back( tmpval );
        InValues>>tmpval;
        m_E_p2_MS.push_back( tmpval );
        /*systematic*/
        InValues>>tmpval;
        m_S_p1_ID.push_back( tmpval );
        InValues>>tmpval;
        m_S_p2_ID.push_back( tmpval );
        InValues>>tmpval;
        m_S_p2_ID_TAN.push_back( tmpval );
        InValues>>tmpval;
        m_S_p1_MS.push_back( tmpval );
        InValues>>tmpval;
        m_S_p2_MS.push_back( tmpval );
      }
    }
  }
  InValues.close();
  InValues.clear();

  //::: Retrieving MC values
  std::string mc_val;
  //::: Check if FilesPath defined: if so override other configurations (advanced user setting, for debugging within MCP)
  if ( m_FilesPath == "" ) {
    if ( m_Trel >= MCAST::Release::PreRec ) {
      mc_val = PathResolverFindCalibFile( "MuonMomentumCorrections/MC_values_" + m_algo + "_" + m_year + "_" + m_release + ".dat" );
    }
    else if ( m_Trel >= MCAST::Release::PreRec ) {
      mc_val = PathResolverFindCalibFile( "MuonMomentumCorrections/MC_values_" + m_algo + "_Rel17.2Sum13.dat" );
    }
    else {
      mc_val = PathResolverFindCalibFile( "MuonMomentumCorrections/MC_values_" + m_algo + "_" + m_release + ".dat" );
    }
  }
  else {
    mc_val = m_FilesPath + "MC_values_" + m_algo + "_Rel17.2Sum13.dat";
  }
  ATH_MSG_DEBUG( "Checking Files - MC: " << mc_val );
  
  InValues.open( mc_val.c_str() );
  i = 0;
  if( !InValues.good() ) {
    ATH_MSG_ERROR( "File " << mc_val << " not found!!" );
    return StatusCode::FAILURE;
  }
  else {
    while( InValues.good() && i<m_nb_regions ) {
      tmpval = 0;
      if( i == 0 ) {
        getline( InValues,tmpname );
      }
      InValues>>tmpval;
      m_MC_p1_ID.push_back( tmpval );
      InValues>>tmpval;
      m_MC_p2_ID.push_back( tmpval );
      InValues>>tmpval;
      m_MC_p2_ID_TAN.push_back( tmpval );
      InValues>>tmpval;
      m_MC_p0_MS.push_back( tmpval );
      InValues>>tmpval;
      m_MC_p1_MS.push_back( tmpval );
      InValues>>tmpval;
      m_MC_p2_MS.push_back( tmpval );
    }
  }
  InValues.close();
  InValues.clear();
  
  return StatusCode::SUCCESS;

}

int MuonCalibrationAndSmearingTool::GetScaleRegion( xAOD::Muon& mu ) {

  if( m_scaleBins.empty() ) {
    return -1;
  }
  double _min = m_scaleBins[0];
  double _max = m_scaleBins[m_scaleBins.size()-1];
  if( mu.eta()<_min ) {
    return 0;
  }
  if( mu.eta()>=_max ) {
    return m_scaleBins.size() - 1;
  }

  std::vector<double>::iterator lb = lower_bound( m_scaleBins.begin(),m_scaleBins.end(),mu.eta()+1e-9 ); // [first,last)
  return( std::min( static_cast<int>( distance( m_scaleBins.begin(),lb ) ) , static_cast<int>( m_scaleBins.size()-1 ) ) );

}

double MuonCalibrationAndSmearingTool::GetSmearing( int DetType, xAOD::Muon& mu ) {

  bool useTan2 = true;
  if ( m_Trel >= MCAST::Release::PreRec && m_Trel < MCAST::Release::Rec_2015_11_15 ) useTan2 = false; 
  if ( m_detRegion < 0 || m_detRegion >= m_nb_regions ) return 0; //++++++ HOW TO IMPROVE THIS CHECK?!
  double smear = 0.;
  if ( DetType == MCAST::DetectorType::MS ) {
    if( m_ptms == 0 ) {
      return 0.;
    }
    else {
      smear = m_p0_MS[m_detRegion]*m_g0/m_ptms + m_p1_MS[m_detRegion]*m_g1 + m_p2_MS[m_detRegion]*m_g2*m_ptms;
      return smear;
    }
  }
  else if( DetType == MCAST::DetectorType::ID ) {
    if( m_Trel >= MCAST::Release::Rec_2015_11_15 ) {
      float additional_weight = 1.;
      ATH_MSG_VERBOSE( "mu.eta() = " << mu.eta() << ",  abs( mu.eta() ) = " << abs( mu.eta() ) << ",  fabs( mu.eta() ) = " << fabs( mu.eta() ) );  
      ATH_MSG_VERBOSE( "Case 0: useTan2 && abs( mu.eta() ) > 2 = " << useTan2 << " && " << ( abs( mu.eta() ) > 2 ) );  
      if ( useTan2 && fabs( mu.eta() ) > 2 ) {
        ATH_MSG_VERBOSE( "Case 1: Using p1ID = " << m_p1_ID[m_detRegion] << " and p2ID = " << m_p2_ID[m_detRegion] );
        additional_weight = sinh( mu.eta() ) * sinh( mu.eta() );
      } 
      else ATH_MSG_VERBOSE( "Case 2: Using p1ID = " << m_p1_ID[m_detRegion] << " and p2ID = " << m_p2_ID[m_detRegion] );
      smear = m_p1_ID[m_detRegion]*m_g3 + m_p2_ID[m_detRegion]*m_g4*m_ptid*additional_weight;
      return smear;
    }
    else {
      if ( useTan2 && m_p2_ID_TAN[m_detRegion] != 0. ) {
        ATH_MSG_VERBOSE( "Case 3: Using p1ID = " << m_p1_ID[m_detRegion] << " and p2ID = " << m_p2_ID[m_detRegion] );
        smear = m_p1_ID[m_detRegion]*m_g3 + m_p2_ID_TAN[m_detRegion]*m_g4*m_ptid*sinh( mu.eta() )*sinh( mu.eta() );
        return smear;
      }
      else {
        ATH_MSG_VERBOSE( "Case 4: Using p1ID = " << m_p1_ID[m_detRegion] << " and p2ID = " << m_p2_ID[m_detRegion] );
        smear = m_p1_ID[m_detRegion]*m_g3 + m_p2_ID[m_detRegion]*m_g4*m_ptid;
        return smear;
      }
    }
  }
  else {
    ATH_MSG_ERROR( " DetType not defined "<<DetType );
  }
  return 0.;

}

void MuonCalibrationAndSmearingTool::CalcCBWeights( xAOD::Muon& mu ) {

  //Calculate the weights of ID and MS used for CB. Two ways are implemented:
  // - weight by ^2 of exp reso
  // - from equation pT_CB = R(pT_ID) + (1-R)(pT_MS)
  //(weights should be correctly normalized)
  m_weightID = 0.5;
  m_weightMS = 0.5;
  if ( m_detRegion < 0 || m_detRegion >= m_nb_regions ) return;
  if ( m_ptcb == 0 ) {
    ATH_MSG_WARNING( "Combined Pt = 0" );
    return;
  }
  const bool doNotAddSmearing = true;
  double SigmaMS = ExpectedResolution( MCAST::DetectorType::MS, mu, doNotAddSmearing );
  double SigmaID = ExpectedResolution( MCAST::DetectorType::ID, mu, doNotAddSmearing );
  double Nsigma = m_useNsigmaForICombine;
  if( fabs( m_ptcb - m_ptms ) > Nsigma * SigmaMS * m_ptcb || fabs( m_ptcb - m_ptid ) > Nsigma * SigmaID * m_ptcb ) {
    double R=1, Rplus;
    if( fabs( m_ptcb-m_ptms ) == fabs( m_ptcb - m_ptid ) ) return; //This case returns weightID = weightMS = 0.5
    if( fabs( m_ptcb-m_ptms ) != 0 && fabs( m_ptcb-m_ptms ) > fabs( m_ptcb-m_ptid ) ) {
      R = ( m_ptid - m_ptcb )/( m_ptcb - m_ptms ); /* R~wMS/wID */
      Rplus = 1 + R;
      if ( Rplus != 0 && R > 0 ) {
        m_weightID = 1/Rplus;
        m_weightMS = R/Rplus;
        return;
      }
    }
    if ( fabs( m_ptcb-m_ptid ) != 0 && fabs( m_ptcb-m_ptms ) < fabs( m_ptcb-m_ptid ) ) {
      R = ( m_ptms - m_ptcb )/( m_ptcb - m_ptid ); /* R~wID/wMS */
      Rplus = 1 + R;
      if ( Rplus != 0 && R > 0 ) {
        m_weightID = R/Rplus;
        m_weightMS = 1/Rplus;
        return;
      }
    }
  }

  double wMS = m_ptms/m_ptcb/pow(SigmaMS,2);
  double wID = m_ptid/m_ptcb/pow(SigmaID,2);
  m_weightID =  wID/(wMS + wID);
  m_weightMS =  wMS/(wMS + wID);
  return;

}

void MuonCalibrationAndSmearingTool::CleanScales() {

  m_scaleBins.clear();

}

double MuonCalibrationAndSmearingTool::ExpectedResolution( const std::string &DetType, xAOD::Muon& mu, const bool mc ) const {

  if( DetType == "MS" ) {
    return ExpectedResolution( MCAST::DetectorType::MS, mu, mc );
  }
  else if( DetType == "ID" ) {
    return ExpectedResolution( MCAST::DetectorType::ID, mu, mc );
  }
  else if( DetType == "CB" ) {
    return ExpectedResolution( MCAST::DetectorType::CB, mu, mc );
  }
  else {
    ATH_MSG_ERROR( "wrong DetType in input " << DetType );
    return 0.;
  }

}

double MuonCalibrationAndSmearingTool::ExpectedResolution( const int DetType,xAOD::Muon& mu, const bool mc ) const {

  bool useTan2 = true;
  /** do the average with the EXPECTED resolutions **/
  if ( m_detRegion<0 || m_detRegion>=m_nb_regions ) return 0;
  double expRes = 0.;
  if ( DetType == MCAST::DetectorType::MS ) {
    if (m_ptms == 0) return 1e12;
    double p0 = mc ? m_MC_p0_MS[m_detRegion] : ( m_MC_p0_MS[m_detRegion] + m_p0_MS[m_detRegion] );
    double p1 = mc ? m_MC_p1_MS[m_detRegion] : ( m_MC_p1_MS[m_detRegion] + m_p1_MS[m_detRegion] );
    double p2 = mc ? m_MC_p2_MS[m_detRegion] : ( m_MC_p2_MS[m_detRegion] + m_p2_MS[m_detRegion] );
    expRes =  sqrt( pow( p0/m_ptms, 2 ) + pow( p1, 2 ) + pow( p2*m_ptms ,2 ) );
    return expRes; //+++++No SYS!!!
  }
  else if ( DetType == MCAST::DetectorType::ID ) {
    if ( m_ptid == 0 ) ATH_MSG_DEBUG( "ptid == 0" );
    double p1 = mc ? m_MC_p1_ID[m_detRegion] : ( m_MC_p1_ID[m_detRegion] + m_p1_ID[m_detRegion] );
    double p2 = mc ? m_MC_p2_ID[m_detRegion] : ( m_MC_p2_ID[m_detRegion] + m_p2_ID[m_detRegion] );
    if ( m_MC_p2_ID_TAN[m_detRegion] != 0 && useTan2 ) {
      p2 = mc ? m_MC_p2_ID_TAN[m_detRegion] : ( m_MC_p2_ID_TAN[m_detRegion] + m_p2_ID_TAN[m_detRegion] );
      p2 = p2*sinh( mu.eta() )*sinh( mu.eta() );
    }
    expRes = sqrt( pow( p1, 2 ) + pow( p2*m_ptid ,2 ) );
    return expRes; //+++++No SYS!!!
  }
  else if ( DetType == MCAST::DetectorType::CB ) {
    // Due to complicated maths, the expected combined resolution
    // is given by this equation (note: all sigmas are fractional uncertainties):
    // sigma_CB = sqrt(2) * sigma_ID * sigma_MS * pTMS * pTID / {pTCB * sqrt({sigma_ID*pTID}^2 + {sigma_MS*pTMS}^2)}
    // Do a little recursive calling to make things easier to read
    // Turn these into *absolute* uncertainties to make life easier
    double sigmaID = ExpectedResolution( MCAST::DetectorType::ID, mu, mc ) * m_ptid;
    double sigmaMS = ExpectedResolution( MCAST::DetectorType::MS, mu, mc ) * m_ptms;
    double denominator = ( m_ptcb ) * sqrt( sigmaID*sigmaID + sigmaMS*sigmaMS );
    return denominator ? sqrt( 2. ) * sigmaID * sigmaMS / denominator : 0.;
  }
  else {
    ATH_MSG_ERROR( "wrong DetType in input "<<DetType );
    return 0.;
  }

}

void MuonCalibrationAndSmearingTool::Clean() {

  m_scale_ID.clear();
  m_enLoss_MS.clear();
  m_scale_MS.clear();
  m_scale_CB.clear();
  m_scaleSyst_ID.clear();
  m_enLossSyst_MS.clear();
  m_scaleSyst_MS.clear();
  m_scaleSyst_CB.clear();
  m_scaleSystUp_ID.clear();
  m_enLossSystUp_MS.clear();
  m_scaleSystUp_MS.clear();
  m_scaleSystDw_ID.clear();
  m_enLossSystDw_MS.clear();
  m_scaleSystDw_MS.clear();
  CleanScales();
  m_p1_ID.clear();
  m_p2_ID.clear();
  m_p2_ID_TAN.clear();
  m_p0_MS.clear();
  m_p1_MS.clear();
  m_p2_MS.clear();
  m_E_p1_ID.clear();
  m_E_p2_ID.clear();
  m_E_p2_ID_TAN.clear();
  m_E_p0_MS.clear();
  m_E_p1_MS.clear();
  m_E_p2_MS.clear();
  m_S_p1_ID.clear();
  m_S_p2_ID.clear();
  m_S_p2_ID_TAN.clear();
  m_S_p0_MS.clear();
  m_S_p1_MS.clear();
  m_S_p2_MS.clear();
  m_SUp_p1_ID.clear();
  m_SUp_p2_ID.clear();
  m_SUp_p2_ID_TAN.clear();
  m_SUp_p0_MS.clear();
  m_SUp_p1_MS.clear();
  m_SUp_p2_MS.clear();
  m_SDw_p1_ID.clear();
  m_SDw_p2_ID.clear();
  m_SDw_p2_ID_TAN.clear();
  m_SDw_p0_MS.clear();
  m_SDw_p1_MS.clear();
  m_SDw_p2_MS.clear();
  m_MC_p1_ID.clear();
  m_MC_p2_ID.clear();
  m_MC_p2_ID_TAN.clear();
  m_MC_p0_MS.clear();
  m_MC_p1_MS.clear();
  m_MC_p2_MS.clear();

}

void MuonCalibrationAndSmearingTool::PrintRegions() const {

  if( m_doMacroRegions ) {
    for( std::map<int, int>::const_iterator it = m_MacroRegionIdxMap.begin(); it!=m_MacroRegionIdxMap.end(); ++it ) {
      int bReg = it->first, mReg = it->second;
      ATH_MSG_INFO( "Base region n "<<bReg );
      ATH_MSG_INFO( "phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg] );
      ATH_MSG_INFO( "eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg] );
      ATH_MSG_INFO( "included in Macro region N "<<mReg<<" with innerEta="<<m_MacroRegionInnerEta[mReg] );
    }
  }
  else {
    for( int bReg=0; bReg<m_nb_regions; ++bReg ) {
      ATH_MSG_INFO( "Base region n "<<bReg<<": phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg] );
      ATH_MSG_INFO( "eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg] );
    }
  }

}

unsigned int MuonCalibrationAndSmearingTool::GetNRegions() const {

  if( m_doMacroRegions ) return m_MacroRegionName.size();
  else return m_nb_regions;

}

StatusCode MuonCalibrationAndSmearingTool::Regions( std::string inRegionFile, int doMacroRegionsFlag ) {

  m_eta_min.clear();
  m_eta_max.clear();
  m_phi_min.clear();
  m_phi_max.clear();
  m_loadNames = false;
  m_doMacroRegions = false;
  std::string tmpname;
  double tmpval;
  int i = 0;
  std::ifstream inFile( inRegionFile.c_str() );
  if( !inFile.good() ) {
    ATH_MSG_ERROR( "File not good" );
    return StatusCode::FAILURE;
  }
  else {
    while( !inFile.eof() ) {
      tmpval=0;
      tmpname="";
      if( i==0 ) {
        getline( inFile,tmpname );
        //Check if I am loading the name of the regions in the file
        if( tmpname.find( "name" ) != std::string::npos && !m_loadNames ) m_loadNames = true;
      }
      else {
        inFile>>tmpval;
        if( inFile.eof() ) break; //this will break immediatly as soon as the new line is empty
        m_eta_min.push_back( tmpval );
        inFile>>tmpval;
        m_eta_max.push_back( tmpval );
        inFile>>tmpval;
        m_phi_min.push_back( tmpval );
        inFile>>tmpval;
        m_phi_max.push_back( tmpval );
        if( m_loadNames ) { //Fill the name vector only if you have them!
          inFile>>tmpname;
          m_names.push_back( tmpname );
        }
      }
      ++i;
    }
  }
  inFile.close();
  m_nb_regions = ( int ) m_eta_min.size();
  if( doMacroRegionsFlag ) { //In case I want macroRegions, I need to define the criteria here
    m_doMacroRegions = true;
    switch( doMacroRegionsFlag ) {
    case 1:
      //Collects all the Large and Small sectors if they belong to the same eta bin
      CollectMacroRegionsSL();
      break;
    case 2:
      //Large,Small sectors split plus Feet(12+14) and 11+15 sector split in Barrel
      CollectMacroRegionsSL_SplitBAR();
      break;
    case 3:
      //Collects all the Large and Small, Up and Down sectors if they belong to the same eta bin
      CollectMacroRegionsSL_UpDn();
      break;
    case 4:
      //Collects all the Large and Small, Up and Down sectors if they belong to the same eta bin
      CollectSectors();
      break;
    default:
      ATH_MSG_ERROR( "doMacroRegionFlag=" << doMacroRegionsFlag << " not defined!" );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;

}

int MuonCalibrationAndSmearingTool::GetRegion( const double eta, const double phi ) const {

  ATH_MSG_VERBOSE( "eta,phi = " << eta << "," << phi );
  int ret_k =-1;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( eta>=m_eta_min[k] && eta<m_eta_max[k] ) {
      if( m_phi_min[k] <= m_phi_max[k] ) { //Standard case
        if( phi>=m_phi_min[k] && phi<m_phi_max[k] ) {
          ret_k = k;
          break;
        }
      }
      else {  //Circular closure of Phi, min and max are excanged in the .dat file
        if( phi>=m_phi_min[k] || phi<m_phi_max[k] ) {
          ret_k = k;
          break;
        }
      }
    }
  }
  if( ret_k == -1 ) {
    ATH_MSG_DEBUG( "Region corresponding to Eta=" << eta << ", Phi=" << phi << " NOT FOUND!" );
    return -1;
  }
  if( m_doMacroRegions ) {
    ATH_MSG_VERBOSE( "INDEX = " << m_MacroRegionIdxMap.find( ret_k )->second );
    return m_MacroRegionIdxMap.find( ret_k )->second;
  }
  return ret_k;

}

float MuonCalibrationAndSmearingTool::GetRegionInnerEta( const int r_i ) const { //Return Eta closer to the origin

  if( m_doMacroRegions ) {
    if( r_i>=0 && r_i < ( int )m_MacroRegionName.size() ) return m_MacroRegionInnerEta[r_i];
  }
  else {
    if( r_i >= 0 && r_i < m_nb_regions ) {
      if( m_eta_min[r_i] >= 0. ) return m_eta_min[r_i];
      else return m_eta_max[r_i];
    }
  }
  //ATH_MSG_WARNING( "Region Inner Eta corresponding to Region index=" << r_i << " NOT FOUND!" );
  return -999.;

}

std::string MuonCalibrationAndSmearingTool::GetRegionName( const int r_i ) const {

  if( m_loadNames ) {
    if( m_doMacroRegions ) {
      if( r_i>=0 && r_i < ( int )m_MacroRegionName.size() ) return m_MacroRegionName[r_i];
    }
    else {
      if( r_i>=0 && r_i < m_nb_regions ) return m_names[r_i];
    }
    ATH_MSG_WARNING( "Region Name corresponding to Region index=" << r_i << " NOT FOUND!" );
  }
  ATH_MSG_WARNING( "Region Names are not set!" );
  return "NAN";

}

std::string MuonCalibrationAndSmearingTool::GetRegionName( const double eta, const double phi ) const {

  if( m_loadNames ) return GetRegionName( GetRegion( eta, phi ) );
  ATH_MSG_WARNING( "Region Names are not set!" );
  return "NAN";

}

//Collects all the Large and Small sectors if they belong to the same eta bin
void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL() {

  double etaMin = -999., etaMax = -999.;
  int macroRegionIdx = 0;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
      //Build a new macroRegion
      etaMin = m_eta_min[k];
      etaMax = m_eta_max[k];
      //One of the following is true
      std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
      m_MacroRegionName.push_back( macroRegName+"Large" );
      m_MacroRegionName.push_back( macroRegName+"Small" );
      //insert twice the innerEta, for Large and for Small sectors
      for( int i=0; i<2; ++i ) {
        if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
        else         m_MacroRegionInnerEta.push_back( etaMax );
      }
      macroRegionIdx+=2;
    }
    if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos )
      m_MacroRegionIdxMap[k] = macroRegionIdx-2;//Large sectors
    if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos )
      m_MacroRegionIdxMap[k] = macroRegionIdx-1;//Small sectors
  }
  return;

}

//Collects all the Large and Small sectors if they belong to the same eta bin
void MuonCalibrationAndSmearingTool::CollectSectors() {

  double etaMin = -999., etaMax = -999.;
  int macroRegionIdx = 0;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
      //Build a new macroRegion
      etaMin = m_eta_min[k];
      etaMax = m_eta_max[k];
      //One of the following is true
      std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
      m_MacroRegionName.push_back( macroRegName+"LargeDn" );
      m_MacroRegionName.push_back( macroRegName+"SmallDn" );
      m_MacroRegionName.push_back( macroRegName+"LargeUp" );
      m_MacroRegionName.push_back( macroRegName+"SmallUp" );
      //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
      for( int i=0; i<4; ++i ) {
        if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
        else         m_MacroRegionInnerEta.push_back( etaMax );
      }
      macroRegionIdx+=4;
    }
    if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos ) { //Large sectors
      if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "13" ) != std::string::npos ||
          m_names[k].find( "15" ) != std::string::npos ) {
        m_MacroRegionIdxMap[k] = macroRegionIdx-4;//Down Large sectors (within 10 to 16)
      }
      else {
        m_MacroRegionIdxMap[k] = macroRegionIdx-2; //Up, large sectors
      }
    }
    if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos ) { //Small sectors
      if( m_names[k].find( "10" ) != std::string::npos || m_names[k].find( "12" ) != std::string::npos ||
          m_names[k].find( "14" ) != std::string::npos || m_names[k].find( "16" ) != std::string::npos ) {
        m_MacroRegionIdxMap[k] = macroRegionIdx-3; //Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
      }
      else {
        m_MacroRegionIdxMap[k] = macroRegionIdx-1; ; //Up, Small sectors
      }
    }
  }
  return;

}

void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL_UpDn() {

  double etaMin = -999., etaMax = -999.;
  int macroRegionIdx = 0;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) {
      //Build a new macroRegion
      etaMin = m_eta_min[k];
      etaMax = m_eta_max[k];
      //One of the following is true
      std::string macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BL" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
      macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
      m_MacroRegionName.push_back( macroRegName+"LargeDn" );
      m_MacroRegionName.push_back( macroRegName+"SmallDn" );
      m_MacroRegionName.push_back( macroRegName+"LargeUp" );
      m_MacroRegionName.push_back( macroRegName+"SmallUp" );
      //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
      for( int i=0; i<4; ++i ) {
        if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
        else         m_MacroRegionInnerEta.push_back( etaMax );
      }
      macroRegionIdx+=4;
    }
    if( m_names[k].find( "EL" ) != std::string::npos || m_names[k].find( "BL" ) != std::string::npos ) { //Large sectors
      if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "13" ) != std::string::npos ||
          m_names[k].find( "15" ) != std::string::npos ) {
        m_MacroRegionIdxMap[k] = macroRegionIdx-4;//Down Large sectors (within 10 to 16)
      }
      else {
        m_MacroRegionIdxMap[k] = macroRegionIdx-2; //Up, large sectors
      }
    }
    if( m_names[k].find( "ES" ) != std::string::npos || m_names[k].find( "BS" ) != std::string::npos ) { //Small sectors
      if( m_names[k].find( "10" ) != std::string::npos || m_names[k].find( "12" ) != std::string::npos ||
          m_names[k].find( "14" ) != std::string::npos || m_names[k].find( "16" ) != std::string::npos ) {
        m_MacroRegionIdxMap[k] = macroRegionIdx-3; //Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
      }
      else {
        m_MacroRegionIdxMap[k] = macroRegionIdx-1; ; //Up, Small sectors
      }
    }
  }
  return;

}

//Collects all the Large and Small sectors if they belong to the same eta bin and splits
// the barrel 12,14 smalls and 11+15 larges that may need different calibrations
void MuonCalibrationAndSmearingTool::CollectMacroRegionsSL_SplitBAR() {

  double etaMin = -999., etaMax = -999.;
  int macroRegionIdx = 0;
  for( int k=0; k < m_nb_regions; ++k ) {
    if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ) { //Build a new macroRegion
      etaMin = m_eta_min[k];
      etaMax = m_eta_max[k];
      std::string macroRegName ="";
      if( m_names[k].find( "EL" )!= std::string::npos || m_names[k].find( "ES" ) != std::string::npos ) {
        //We are in the End-Cap sectors so we split only Large and Small sectors
        macroRegName = m_names[k].substr( 0, m_names[k].find( "EL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "ES" ) );
        //insert 4 time the innerEta, for Large and Small sectors
        m_MacroRegionName.push_back( macroRegName+"Large" );
        m_MacroRegionName.push_back( macroRegName+"Small" );
        for( int i=0; i<2; ++i ) {
          if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
          else               m_MacroRegionInnerEta.push_back( etaMax );
        }
        macroRegionIdx+=2;
      }
      else {  //We are in the Barrels sectors so we split Large, Small, 12+14, 11+15 sectors
        macroRegName = m_names[k].substr( 0, m_names[k].find( "BL" ) );
        macroRegName = macroRegName.substr( 0, m_names[k].find( "BS" ) );
        //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
        m_MacroRegionName.push_back( macroRegName+"Large" );
        m_MacroRegionName.push_back( macroRegName+"Small" );
        m_MacroRegionName.push_back( macroRegName+"Large11_15" );
        m_MacroRegionName.push_back( macroRegName+"SmallFeet" );
        for( int i=0; i<4; ++i ) {
          if( etaMin >= 0. ) m_MacroRegionInnerEta.push_back( etaMin );
          else               m_MacroRegionInnerEta.push_back( etaMax );
        }
        macroRegionIdx+=4;
      }
    }
    if( m_names[k].find( "EL" ) != std::string::npos ) { //End-Cap Large sectors
      m_MacroRegionIdxMap[k] = macroRegionIdx-2;
    }
    else if( m_names[k].find( "ES" ) != std::string::npos ) { //End-Cap Small sectors
      m_MacroRegionIdxMap[k] = macroRegionIdx-1; //
    }
    else if ( m_names[k].find( "BL" ) != std::string::npos ) { //Barrel Large sectors
      if( m_names[k].find( "11" ) != std::string::npos || m_names[k].find( "15" ) != std::string::npos ) {
        m_MacroRegionIdxMap[k] = macroRegionIdx-2;//Barrel Large sectors with different alignment (11,15)
      }
      else {
        m_MacroRegionIdxMap[k] = macroRegionIdx-4; //Standard Barrel Large sectors
      }
    }
    else if( m_names[k].find( "BS" ) != std::string::npos ) { //Barrel Small sectors
      if( m_names[k].find( "12" ) != std::string::npos || m_names[k].find( "14" ) != std::string::npos ) {
        m_MacroRegionIdxMap[k] = macroRegionIdx-1; //Feet Small sectors (12+14, Down).
      }
      else {
        m_MacroRegionIdxMap[k] = macroRegionIdx-3; //All standard Barrel Small sectors
      }
    }
  }
  return;

}

double MuonCalibrationAndSmearingTool::GetSystVariation( int DetType, double var ) {

  if( var != 1. && var != -1 ) {
    ATH_MSG_WARNING( "Systematic variation is not +/- 1 sigma, returning 0." );
    return 0;
  }
  if( m_detRegion < 0 || m_detRegion >= m_nb_regions ) return 0;

  double p0_MS_var = 0., p1_MS_var = 0., p2_MS_var = 0., p1_ID_var = 0., p2_ID_var = 0., p2_ID_TAN_var = 0.;   
  double newSmear = 0.;
  if( DetType == MCAST::DetectorType::MS ) {
    if( m_ptms == 0 ) {  
      return 0;
    } else {
      if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
        p0_MS_var = pow( m_E_p0_MS[ m_detRegion ] * m_E_p0_MS[ m_detRegion ] + m_S_p0_MS[ m_detRegion ] * m_S_p0_MS[ m_detRegion ], 0.5 );
        p1_MS_var = pow( m_E_p1_MS[ m_detRegion] * m_E_p1_MS[ m_detRegion ] + m_S_p1_MS[ m_detRegion ] * m_S_p1_MS[ m_detRegion ], 0.5 );
        p2_MS_var = pow( m_E_p2_MS[ m_detRegion] * m_E_p2_MS[ m_detRegion ] + m_S_p2_MS[ m_detRegion ] * m_S_p2_MS[ m_detRegion ], 0.5 );
      } else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
        p0_MS_var = var > 0. ? m_SUp_p0_MS[ m_detRegion ] : m_SDw_p0_MS[ m_detRegion ];
        p1_MS_var = var > 0. ? m_SUp_p1_MS[ m_detRegion ] : m_SDw_p1_MS[ m_detRegion ];
        p2_MS_var = var > 0. ? m_SUp_p2_MS[ m_detRegion ] : m_SDw_p2_MS[ m_detRegion ];
      }

      p0_MS_var = m_p0_MS[ m_detRegion ] + var * p0_MS_var;
      p1_MS_var = m_p1_MS[ m_detRegion ] + var * p1_MS_var;
      p2_MS_var = m_p2_MS[ m_detRegion ] + var * p2_MS_var;

      if( p0_MS_var < 0. ) p0_MS_var = 0.; //Truncation of unphysical variations
      if( p1_MS_var < 0. ) p1_MS_var = 0.;
      if( p2_MS_var < 0. ) p2_MS_var = 0.;
      newSmear = ( p0_MS_var * m_g0 / m_ptms + p1_MS_var * m_g1 + p2_MS_var * m_g2 * m_ptms );
      return newSmear;
    }
  } else if( DetType == MCAST::DetectorType::ID ) {
    if( m_Trel < MCAST::Release::Rel17_2_Sum13 ) {
      p1_ID_var     = pow( m_E_p1_ID[ m_detRegion ] * m_E_p1_ID[ m_detRegion ] + m_S_p1_ID[ m_detRegion ] * m_S_p1_ID[ m_detRegion ], 0.5 );
      p2_ID_var     = pow( m_E_p2_ID[ m_detRegion ] * m_E_p2_ID[ m_detRegion ] + m_S_p2_ID[ m_detRegion ] * m_S_p2_ID[ m_detRegion ], 0.5 );
      p2_ID_TAN_var = pow( m_E_p2_ID_TAN[ m_detRegion ] * m_E_p2_ID_TAN[ m_detRegion ] + m_S_p2_ID_TAN[ m_detRegion ] * m_S_p2_ID_TAN[ m_detRegion ], 0.5 );
    } else {//Syst are not more symmetric in the latest release, the stat/sys add in quadrature is already done
      p1_ID_var     = var > 0. ? m_SUp_p1_ID[ m_detRegion ] : m_SDw_p1_ID[ m_detRegion ];
      p2_ID_var     = var > 0. ? m_SUp_p2_ID[ m_detRegion ] : m_SDw_p2_ID[ m_detRegion ];
      p2_ID_TAN_var = var > 0. ? m_SUp_p2_ID_TAN[ m_detRegion ] : m_SDw_p2_ID_TAN[ m_detRegion ];
    }
    p1_ID_var     = m_p1_ID[ m_detRegion ] + var * p1_ID_var;
    p2_ID_var     = m_p2_ID[ m_detRegion ] + var * p2_ID_var;
    p2_ID_TAN_var = m_p2_ID_TAN[ m_detRegion ] + var * p2_ID_TAN_var;

    if( p1_ID_var < 0. ) p1_ID_var = 0.; //Truncation of unphysical variations
    if( p2_ID_var < 0. ) p2_ID_var = 0.;
    if( p2_ID_TAN_var < 0. ) p2_ID_TAN_var = 0.;

    bool useTan2 = true;
    if( useTan2 && m_p2_ID_TAN[ m_detRegion ] != 0 ) {         
      newSmear = ( p1_ID_var * m_g3 + p2_ID_TAN_var * m_g4 * m_ptid * sinh( m_eta ) * sinh( m_eta ) );
    } else {
      newSmear = ( p1_ID_var * m_g3 + p2_ID_var * m_g4 * m_ptid );
    }
    return newSmear;
  }
  return 0;
}

} // namespace CP
