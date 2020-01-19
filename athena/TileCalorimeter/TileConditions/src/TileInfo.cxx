/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 Filename      : TileInfo.cxx 
 Author        : F. Merritt 
 Created       : May, 2002
 Last Modified : April 2006 - F Spano' (add covariance matrix for coherent 
 noise)
 Version 00-01-22
 
 :DESCRIPTION: see header file.
****************************************************************************/
//Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"

#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TilePulseShapes.h"
#include "TileConditions/TileOptFilterWeights.h"

#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileFragHash.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IClassIDSvc.h"
#include <boost/preprocessor/stringize.hpp>

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>


//
//_____________________________________________________________________________
TileInfo::TileInfo(ISvcLocator *svcLocator)
  : m_svcLocator(svcLocator)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_tileTBID(0)
  , m_cabling(0)
  , m_channel_context(0)
  , m_drawer_context(0)
  , m_emscaleA(0)
  , m_emscaleC()
  , m_emscaleMBTS()
  , m_adcmax(1023)                
  , m_nSamples(0)
  , m_iTrigSample(0)
  , m_tileNoise(0)
  , m_tileCoherNoise(0)
  , m_tileZeroSuppress(0)
  , m_thresholdRawChannel(0)
  , m_thresholdDigits(0)
  , m_ttL1Calib(0)
  , m_ttL1NoiseSigma(0)
  , m_ttL1Thresh(0)
  , m_ttL1Ped(0)
  , m_ttL1Max(0)
  , m_MuRcvCalib(0)
  , m_MuRcvNoiseSigma(0)
  , m_MuRcvThresh(0)
  , m_MuRcvPed(0)
  , m_MuRcvMax(0)
  , m_muL1Calib(0)
  , m_muL1NoiseSigma(0)
  , m_muL1Thresh(0)
  , m_muL1Ped(0)
  , m_muL1Max(0)
  , m_mbtsL1Calib(0)
  , m_mbtsL1NoiseSigma(0)
  , m_mbtsL1Thresh(0)
  , m_mbtsL1Ped(0)
  , m_mbtsL1Max(0)
  , m_ttL1CosmicsGain(0)
  , m_ttL1CosmicsThresh(0)
  , m_digitsNBinsHi(0)
  , m_digitsNBinsLo(0)
  , m_digitsTime0BinHi(0)
  , m_digitsTime0BinLo(0)
  , m_digitsBinsPerXHi(0)
  , m_digitsBinsPerXLo(0)
  , m_TTL1NBins(0)
  , m_TTL1Time0Bin(0)
  , m_TTL1BinsPerX(0)
  , m_MuRcvNBins(0)
  , m_MuRcvTime0Bin(0)
  , m_MuRcvBinsPerX(0)
  , m_MuL1NBins(0)
  , m_MuL1Time0Bin(0)
  , m_MuL1BinsPerX(0)
  , m_pulseShapes(0)
  , m_OptFilterWeights(0)
  , m_OptFilterCorrelation(0)
  , m_tileCablingSvc("TileCablingSvc","TileInfo")
  , m_tileIdTrans("TileCondIdTransforms")
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_tileToolNoiseSample("TileCondToolNoiseSample")
  , m_tileToolTiming("TileCondToolTiming")
  , m_nPhElec(0)
  , m_nPhElecVec()
{
  //=== initialize TestBeam defaults
  for (int i=0; i<32; ++i)
    m_emscaleTB[i] = 1.0;
  
  for (int i=0; i<32; ++i)
    m_mev2adcTB[i] = 1.0;

  for (int i=0; i<10; ++i)
    m_emscaleE[i] = -1.0;
  for (int i=10; i<16; ++i)
    m_emscaleE[i] = 75.0;

  // scale factors from ADC HF noise to Opt filter noise calculated by Luca Fiorini
  m_noiseScaleFactor[0] = 1.0;  // no scale at all
  m_noiseScaleFactor[1] = 1.14; // opt filter without iterations
  m_noiseScaleFactor[2] = 1.28; // opt filter with iterations
  m_noiseScaleFactor[3] = 1.27; // fit method

  m_noiseScaleIndex = 1; // by default noise for opt filter without iterations
}


//
//_____________________________________________________________________________
TileInfo::~TileInfo() 
{
  if (m_channel_context) delete m_channel_context;
  if (m_drawer_context) delete m_drawer_context;

  int sizepart=m_decoCovaria.size();
  for (int i=0; i<sizepart; ++i){
    int sizemodu=(m_decoCovaria[i]).size();
    for (int j=0; j<sizemodu; ++j){
      int sizegain=(m_decoCovaria[i][j]).size();
      for (int k=0; k<sizegain; ++k){
        if (m_decoCovaria[i][j][k]) delete (TMatrixD*)(m_decoCovaria[i][j][k]);
      }
      m_decoCovaria[i][j].clear();
    }
    m_decoCovaria[i].clear();
  }
  m_decoCovaria.clear();

}


//
//_____________________________________________________________________________
StatusCode
TileInfo::initialize()
{
  MsgStream log(msgSvc(),"TileInfo");
  MSG::Level logLevel = log.level();
  bool debug = (logLevel <= MSG::DEBUG);

  if(debug) log<<MSG::DEBUG<<"In TileInfo::initialize..."<<endmsg;

  // Declare our CLID.
  IClassIDSvc* clidsvc = 0;
  CHECK( m_svcLocator->service("ClassIDSvc", clidsvc, true) );
  clidsvc->setTypePackageForID (classID(),
                                "TileInfo",
                                Athena::PackageInfo (BOOST_PP_STRINGIZE(PACKAGE_VERSION_UQ)));

    //=== get TileCablingSvc
  StatusCode sc = m_tileCablingSvc.retrieve();
  if(sc.isFailure()){
    log << MSG::ERROR
        << "Unable to retrieve " << m_tileCablingSvc << endmsg;
    return StatusCode::FAILURE;
  }
  //=== cache pointers to cabling helpers
  m_cabling  = m_tileCablingSvc->cablingService();
  if(!m_cabling){
    log << MSG::ERROR
        << "Pointer to TileCablingService is zero: " << m_cabling << endmsg;
    return StatusCode::FAILURE;
    
  }
  m_tileID   = m_cabling->getTileID();
  m_tileTBID = m_cabling->getTileTBID();
  m_tileHWID = m_cabling->getTileHWID();

  //=== get TileIdTrans Tool
  sc = m_tileIdTrans.retrieve();
  if(sc.isFailure()){
    log << MSG::ERROR
        << "Unable to retrieve " << m_tileIdTrans << endmsg;
    return StatusCode::FAILURE;
  }

  //=== get TileCondToolEmscale
  sc = m_tileToolEmscale.retrieve();
  if(sc.isFailure()){
    log << MSG::ERROR
        << "Unable to retrieve " << m_tileToolEmscale << endmsg;
    return StatusCode::FAILURE;
  }
  
  //=== get TileCondToolNoiseSample
  sc = m_tileToolNoiseSample.retrieve();
  if(sc.isFailure()){
    log << MSG::ERROR
        << "Unable to retrieve " << m_tileToolNoiseSample << endmsg;
    return StatusCode::FAILURE;
  }

  //=== get TileCondToolTiming
  sc = m_tileToolTiming.retrieve();
  if(sc.isFailure()){
    log << MSG::ERROR
        << "Unable to retrieve " << m_tileToolTiming << endmsg;
    return StatusCode::FAILURE;
  }


  //=== initialize channel context
  m_channel_context = new IdContext(m_tileHWID->channel_context());
  //=== initialize drawer context
  m_drawer_context = new IdContext(m_tileHWID->drawer_context());

  //=== Initialize TilePulseShapes
  if (m_pulseShapes)
    m_pulseShapes->load(log);
  
  //=== Read OptFilter Weights &&/|| Correlation in TileInfoLoader.cxx
  if (m_OptFilterWeights)
    m_OptFilterWeights->loadWeights(log);
  if (m_OptFilterCorrelation)
    m_OptFilterCorrelation->loadCorrelation(log);

  if(debug) log << MSG::DEBUG << " TileInfo initialization completed. " << endmsg;  
  return StatusCode::SUCCESS;
}

//
//_____________________________________________________________________________
IMessageSvc* 
TileInfo::msgSvc() const 
{
  IMessageSvc *msgSvc = 0;
  StatusCode sc = m_svcLocator->service("MessageSvc",msgSvc,true);
  return sc.isFailure() ? 0 : msgSvc;
}



//
//_____________________________________________________________________________
double
TileInfo::HitCalib(const Identifier& pmt_id) const {

  double calib;

  if (m_tileTBID->is_tiletb(pmt_id)) { // MBTS or E4'
    calib = m_emscaleMBTS[m_tileTBID->eta(pmt_id)]; // 0=inner cell, 1=outer cell, 2=E4'
  } else if (m_tileID->is_tile_gap(pmt_id)) { // ITC of gap/crack scin
    int sample = m_tileID->sample(pmt_id);
    if (TileID::SAMP_E == sample) { // gap/crack scin
      int tower=m_tileID->tower(pmt_id);
      if (tower>15) tower /= 4; // fix for upgrade geometry with 0.025 granularity
      calib = m_emscaleE[tower];
    } else if (TileID::SAMP_C == sample) { // C10 cell
      calib = m_emscaleC[m_tileID->module(pmt_id)];
    } else {
      calib = m_emscaleA;
    }
  } else {
    calib = m_emscaleA;
  }
  
  return calib;
}

//
//_____________________________________________________________________________
double
TileInfo::EmsCalib(const HWIdentifier& channel_id, double amplitude) const 
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(channel_id, drawerIdx, channel, adc);
  return m_tileToolEmscale->doCalibEms(drawerIdx, channel, amplitude);
}

//
//_____________________________________________________________________________
double
TileInfo::CesiumCalib(const HWIdentifier& channel_id, double amplitude, 
		      bool applyLasCorr) const 
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(channel_id, drawerIdx, channel, adc); 
  return m_tileToolEmscale->doCalibCes(drawerIdx,channel,amplitude,applyLasCorr);
}

//
//_____________________________________________________________________________
double
TileInfo::LaserCalib(const HWIdentifier& channel_id, double amplitude) const 
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(channel_id, drawerIdx, channel, adc); 
  return m_tileToolEmscale->doCalibLas(drawerIdx,channel,amplitude);
}

//
//_____________________________________________________________________________
double
TileInfo::CisCalib(const HWIdentifier& adc_id, double amplitude,
		   const TileFragHash::TYPE /*filterType*/) const
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(adc_id, drawerIdx, channel, adc); 
  return m_tileToolEmscale->doCalibCis(drawerIdx,channel,adc,amplitude);
}

//
//_____________________________________________________________________________
double
TileInfo::ChannelCalib(const HWIdentifier& adc_id, double amplitude,
		       const TileRawChannelUnit::UNIT rawDataUnitIn,
		       const TileRawChannelUnit::UNIT rawDataUnitOut,
		       const TileFragHash::TYPE /*filterType*/) const
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(adc_id, drawerIdx, channel, adc); 
  return m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, amplitude, rawDataUnitIn, rawDataUnitOut);
}

//
//_____________________________________________________________________________
double
TileInfo::DigitsPedSigma(const HWIdentifier& adc_id) const 
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(adc_id, drawerIdx, channel, adc); 
  return m_tileToolNoiseSample->getHfn(drawerIdx,channel,adc);
}

//
//_____________________________________________________________________________
double
TileInfo::DigitsPedLevel(const HWIdentifier& adc_id) const 
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(adc_id, drawerIdx, channel, adc); 
  return m_tileToolNoiseSample->getPed(drawerIdx,channel,adc);
}

//
//_____________________________________________________________________________
double
TileInfo::TimeCalib(const HWIdentifier& adc_id, double time) const 
{
  unsigned int drawerIdx(0), channel(0), adc(0);
  m_tileIdTrans->getIndices(adc_id, drawerIdx, channel, adc); 
  time -= m_tileToolTiming->getSignalPhase(drawerIdx, channel, adc);
  return time;
}

/////////////////////////////////////////////////////////////////////////////
// Return the sigma of the cell noise for different low/high gain combinations
// in units of MeV

// NGO what we really want here is the noise per cell for a given reco method
// NGO can we compute this somehow from the sample noise??
//
// NGO we need also some function that returns pileup noise per channel, need 
// NGO new DB channels, can store together with cell noise (might depend on reco method)
//
//_____________________________________________________________________________
double 
TileInfo::CellNoiseSigma(const Identifier& cell_id, CaloGain::CaloGain oldgain,
			 const TileFragHash::TYPE filterType) const
{
  CaloGain::CaloGain gain(oldgain);
  int nchan = NchannelsPerCell(cell_id);
  
  // positive values are LAr gains
  // convert them to appropriate Tile gains
  if (gain > CaloGain::INVALIDGAIN) {

    switch (nchan) {
    case 2:
      switch (oldgain) {
        case CaloGain::LARHIGHGAIN:   gain = CaloGain::TILEHIGHHIGH; break;
        case CaloGain::LARMEDIUMGAIN: gain = CaloGain::TILEHIGHLOW;  break;
        case CaloGain::LARLOWGAIN:    gain = CaloGain::TILELOWLOW;   break;
        default:                      gain = CaloGain::INVALIDGAIN;
      }
      break;
    case 1:
      switch (oldgain) {
        case CaloGain::LARHIGHGAIN:   gain = CaloGain::TILEONEHIGH; break;
        case CaloGain::LARLOWGAIN:    gain = CaloGain::TILEONELOW;  break;
      default:                        gain = CaloGain::INVALIDGAIN; // medium gain is invalid as well
      }
      break;
    default:
      gain = CaloGain::INVALIDGAIN;
    }
  }

  double noise(-9999.);

  if (gain != CaloGain::INVALIDGAIN) {
    
    unsigned int gain1 = gain & 3; // gain for first PMT
    unsigned int gain2 = (gain >> 2) & 3; // gain for second PMT 
  
    double noise1(0.), noise2(0.), noise12(0.), noise21(0.);

    if (gain1 < 2) { // valid gain
      Identifier pmt_id = m_tileID->pmt_id(cell_id,0); // first pmt offline ID
      HWIdentifier hwid = m_tileHWID->adc_id(m_cabling->s2h_channel_id(pmt_id),gain1); // ADC HWID ch1,gain1
      // get noise for ch1, gain1 in MeV
      noise1 = ChannelCalib(hwid,DigitsPedSigma(hwid),TileRawChannelUnit::ADCcounts,
                            TileRawChannelUnit::MegaElectronVolts,filterType);

      if (CaloGain::LARMEDIUMGAIN == oldgain) { // swap gains to take max noise later
        HWIdentifier hwid = m_tileHWID->adc_id(m_cabling->s2h_channel_id(pmt_id),1-gain1); // ADC HWID other gain
        // get noise for ch1, other gain in MeV
        noise12 = ChannelCalib(hwid,DigitsPedSigma(hwid),TileRawChannelUnit::ADCcounts,
                               TileRawChannelUnit::MegaElectronVolts,filterType);
      }
    }

    if (gain2 < 2 && nchan > 1) { // valid gain and second PMT exists
      Identifier pmt_id = m_tileID->pmt_id(cell_id,1); // second pmt offline ID
      HWIdentifier hwid = m_tileHWID->adc_id(m_cabling->s2h_channel_id(pmt_id),gain2); // ADC HWID ch2,gain2
      // get noise for ch2, gain2 in MeV
      noise2 = ChannelCalib(hwid,DigitsPedSigma(hwid),TileRawChannelUnit::ADCcounts,
                            TileRawChannelUnit::MegaElectronVolts,filterType);

      if (CaloGain::LARMEDIUMGAIN == oldgain) { // swap gains to take max noise later
        HWIdentifier hwid = m_tileHWID->adc_id(m_cabling->s2h_channel_id(pmt_id),1-gain2); // ADC HWID other gain
        // get noise for ch2, other gain in MeV
        noise21 = ChannelCalib(hwid,DigitsPedSigma(hwid),TileRawChannelUnit::ADCcounts,
                               TileRawChannelUnit::MegaElectronVolts,filterType);
      }
    }

    // calculating noise of 2 channels
    noise = hypot(noise1,noise2);
    if (CaloGain::LARMEDIUMGAIN == oldgain) { // choosing maximum among 2 (high,low) combinations
      noise = std::max(noise,hypot(noise12,noise21));
    }
  }
  
 // conversion from ADC sigma noise to OF sigma noise
  return noise*m_noiseScaleFactor[m_noiseScaleIndex];
}

//_____________________________________________________________________________
const TMatrixD*
TileInfo::DecoCovariance(int ros, int drawer, int hilo) const {

  int part(0);
  switch (ros) {
    case TileHWID::EXTBAR_POS: part=0; break; // EBA
    case TileHWID::BARREL_POS: part=1; break; // LBA
    case TileHWID::BARREL_NEG: part=2; break; // LBC
    case TileHWID::EXTBAR_NEG: part=3; break; // EBC
  }

  int gain = (hilo) ? 0 : 1;

  return m_decoCovaria[part][drawer][gain];
}


//////////////////////////////////////////////////////////////////////////////////
// Return shape vector of a TTL1 signal, to multiply by (summed) TTL1 Amplitude
// to construct a TTL1 object. phase is a phase shift, default=0.
//
//_____________________________________________________________________________
void TileInfo::ttl1Shape(const int nsamp, const int itrig, const double phase,std::vector<double> &ttl1shape) const {

    //ttl1shape.resize(nsamp, 0.); // assume that resize already done in calling function
    for (int i=0; i<nsamp; ++i) {
      int j = m_TTL1Time0Bin + (i-itrig)*m_TTL1BinsPerX+(int)(phase*(m_TTL1BinsPerX*(1./25.0)));
      if(j<0) j = 0;
      if(j>=m_TTL1NBins) j = m_TTL1NBins-1;
      ttl1shape[i] = m_TTL1FullShape[j];
    }
#ifndef NDEBUG
    MsgStream log(msgSvc(),"TileInfo");
    if (log.level() <= MSG::DEBUG){
      log << MSG::DEBUG << " Shaping profile at beam crossings:   nsamp = " << nsamp << ", itrig = " << itrig << endmsg;
      int jc = 0;
      for (int i=0; i<nsamp; i++) {
	if(jc==0) log << MSG::DEBUG << "      bin = " << i << "   Shape factor =";
	log << MSG::DEBUG << std::setw(8) << std::setprecision(4) << ttl1shape[i] << "  ";
	if(++jc==5) {
	  log << MSG::DEBUG << endmsg;
	  jc=0;
	}
      }
      log << MSG::DEBUG << endmsg; 
    }
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the shape vector of the Muon Receiver Board input signal, to multiply by the channel amplitude
// to construct a Muon Receiver Board object. The phase is a phase shift (default=0).
//
//_____________________________________________________________________________

void TileInfo::muRcvShape(const int nsamp, const int itrig, const double phase,std::vector<double> &murcvshape) const {

    for (int i=0; i<nsamp; ++i) {
      int j = m_MuRcvTime0Bin + (i-itrig)*m_MuRcvBinsPerX+(int)(phase*(m_MuRcvBinsPerX*(1./25.0)));
      if(j<0) j = 0;
      if(j>=m_MuRcvNBins) j = m_MuRcvNBins-1;
      murcvshape[i] = m_MuRcvFullShape[j];
    }

}
