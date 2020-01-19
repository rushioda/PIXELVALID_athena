/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileDigitsFilter.cxx
//  Author   : Alexander Solodkov
//  Created  : June 2008
//
//  DESCRIPTION
// 
//  Copy TileDigits and TileRawChannel from input container to output container
//  keeping only channels with (max-min) sample above threshold
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecAlgs/TileDigitsFilter.h"

//C++ STL includes
#include <vector>
#include <set>

//
// Constructor
//
TileDigitsFilter::TileDigitsFilter(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(0)
{
  declareProperty("InputDigitsContainer", m_inputContainer = "TileDigitsCnt");
  declareProperty("OutputDigitsContainer", m_outputContainer = "TileDigitsFlt");
  declareProperty("InputRawChannelContainer", m_inRchContainer = "TileRawChannelCnt");
  declareProperty("OutputRawChannelContainer", m_outRchContainer = "TileRawChannelFlt");
  declareProperty("LowGainThereshold", m_threshold[0] = 0); // keep all LG except zeros
  declareProperty("HighGainThereshold", m_threshold[1] = 10); // keep signals above ~128(106) MeV in A,BC(D) samplings
}

TileDigitsFilter::~TileDigitsFilter() {
}

//
// Alg standard initialize function
//
StatusCode TileDigitsFilter::initialize() {

  CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_MSG_INFO( "Input digits container: '" << m_inputContainer
                << "'  output container: '" << m_outputContainer << "'" );

  ATH_MSG_INFO( "Input raw channel container: '" << m_inRchContainer
                << "'  output container: '" << m_outRchContainer << "'" );

  ATH_MSG_INFO( "Threshold low gain: " << m_threshold[0]
                << " counts,  high gain: " << m_threshold[1] << " counts" );

  ATH_MSG_INFO( "initialization completed" );

  return StatusCode::SUCCESS;
}
/*==========================================================================*/
//
// Begin Execution Phase.
//
StatusCode TileDigitsFilter::execute() {

  int digCounter = 0;
  int rchCounter = 0;

  ATH_MSG_DEBUG( "in execute()" );

  // Create new container for filtered digits
  TileDigitsContainer* outputCont = new TileDigitsContainer(true, SG::VIEW_ELEMENTS);
  if (!outputCont) {
    ATH_MSG_FATAL( "Could not create a new TileDigitsContainer instance as requested!" );
    return StatusCode::FAILURE;
  }

  // Initialize iterators to some dummy values
  TileDigitsContainer::const_iterator collItr, lastColl;
  collItr = lastColl = outputCont->end();

  // Get digit container from TES
  if (m_inputContainer.size() > 0) {
    const TileDigitsContainer* inputCont;
    if (evtStore()->retrieve(inputCont, m_inputContainer).isFailure()) {
      ATH_MSG_WARNING( "can't retrieve TileDigitsContainer with name '"
                      << m_inputContainer << "' from TDS" );
    } else {
      collItr = inputCont->begin();
      lastColl = inputCont->end();
    }
  }

  TileRawChannelContainer::const_iterator firstRchColl, lastRchColl, collRchItr;
  TileFragHash::TYPE type = TileFragHash::Digitizer;
  TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts;
  bool emptyRch = true;

  // Get rawChannel container from TES
  if (m_inRchContainer.size() > 0) {
    const TileRawChannelContainer* inRchCont;
    if (evtStore()->retrieve(inRchCont, m_inRchContainer).isFailure()) {
      ATH_MSG_WARNING( "can't retrieve TileRawChannelContainer with name '"
                      << m_inRchContainer << "' from TDS" );
    } else {
      collRchItr = firstRchColl = inRchCont->begin();
      lastRchColl = inRchCont->end();
      emptyRch = (firstRchColl == lastRchColl);
      type = inRchCont->get_type();
      unit = inRchCont->get_unit();
    }
  }

  // Create new container for filtered raw channels
  TileRawChannelContainer* outRchCont = new TileRawChannelContainer(true, type, unit, SG::VIEW_ELEMENTS);

  // Iterate over all collections (drawers)
  for (; collItr != lastColl; ++collItr) {

    std::set<HWIdentifier> ids;

    // Iterate over all digits in this collection
    TileDigitsCollection::const_iterator digitItr = (*collItr)->begin();
    TileDigitsCollection::const_iterator lastDigit = (*collItr)->end();

    for (; digitItr != lastDigit; ++digitItr) {
      std::vector<float> digits = (*digitItr)->samples();
      std::vector<float>::const_iterator idig = digits.begin();
      std::vector<float>::const_iterator iend = digits.end();
      float smin = 99999., smax = -99999.;
      for (; idig != iend; ++idig) {
        float samp = (*idig);
        smin = std::min(samp, smin);
        smax = std::max(samp, smax);
      }
      HWIdentifier adcId = (*digitItr)->adc_HWID();
      if (smax - smin > m_threshold[m_tileHWID->adc(adcId)]) {
        outputCont->push_back(*digitItr);
        ids.insert(adcId);
        ++digCounter;
        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << (m_tileHWID->adc(adcId) ? " HG " : " LG ")
                            << "id=" << m_tileHWID->to_string(adcId)
                            << " samples=";

          for (idig = digits.begin(); idig != iend; ++idig)
            msg(MSG::VERBOSE) << " " << (*idig);

          msg(MSG::VERBOSE) << endmsg;
        }
      }

//      else if (msgLvl(MSG::VERBOSE)){
//        msg(MSG::VERBOSE) << "Filtered Out "
//                          << (m_tileHWID->adc(adcId) ? " HG " : " LG ")
//                          << "id=" << m_tileHWID->to_string(adcId)
//                          << " samples=";
//        for(idig=digits.begin(); idig!=iend; ++idig)
//          msg(MSG::VERBOSE) << " " << (*idig);
//
//        msg(MSG::VERBOSE) << endmsg;
//      }

    }

    if (ids.size() == 0 || emptyRch)
      continue;

    while (collRchItr != lastRchColl
        && (*collRchItr)->identify() < (*collItr)->identify()) {
      ++collRchItr;
    }
    if (collRchItr == lastRchColl) {
      ATH_MSG_WARNING( MSG::hex
          << "TileDigitsCollection ID 0x" << (*collItr)->identify()
          << " but corresponding TileRawChannelCollection is missing"
          << MSG::dec );

    } else if ((*collItr)->identify() != (*collRchItr)->identify()) {
      ATH_MSG_WARNING(  MSG::hex
          << "TileDigitsCollection ID 0x" << (*collItr)->identify()
          << " and TileRawChannelCollection ID 0x" << (*collRchItr)->identify()
          << " do not match " << MSG::dec );

    } else {
      // Iterate over all rawChannels in this collection
      TileRawChannelCollection::const_iterator rchItr = (*collRchItr)->begin();
      TileRawChannelCollection::const_iterator lastRch = (*collRchItr)->end();

      for (; rchItr != lastRch; ++rchItr) {
        HWIdentifier adcId = (*rchItr)->adc_HWID();
        if (ids.find(adcId) != ids.end()) {
          outRchCont->push_back(*rchItr);
          ++rchCounter;
          ATH_MSG_VERBOSE( (m_tileHWID->adc(adcId) ? " HG " : " LG ")
                            << "id=" << m_tileHWID->to_string(adcId)
                            << " amp=" << (*rchItr)->amplitude() );
        }
      }
    }
  }

  ATH_MSG_DEBUG( digCounter << " digits and "
                << rchCounter << " raw channels stored in output containers");

  if (m_outputContainer.size() > 0) {
    // register new container in the TES
    CHECK( evtStore()->record(outputCont, m_outputContainer, false));
    ATH_MSG_DEBUG( "TileDigitsContainer registered successfully (" << m_outputContainer << ")");

  } else if (outputCont) {
    ATH_MSG_DEBUG( "Deleting filtered digits container");
    delete outputCont;
  }

  if (m_outRchContainer.size() > 0) {
    // register new container in the TES
    CHECK( evtStore()->record(outRchCont, m_outRchContainer, false));
    ATH_MSG_DEBUG( "TileRawChannelContainer registered successfully (" << m_outRchContainer << ")");
  } else if (outRchCont) {
    ATH_MSG_DEBUG( "Deleting filtered raw channel container");
    delete outRchCont;
  }

  // Execution completed.
  ATH_MSG_DEBUG( "execution completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileDigitsFilter::finalize() {

  ATH_MSG_INFO( "in finalize()" );

  return StatusCode::SUCCESS;
}

