/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRecAlgs/MBTSTimeDiffEventInfoAlg.h"

#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/errorcheck.h"
#include "TileIdentifier/TileTBID.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/MBTSCollisionTime.h"

using xAOD::EventInfo;

/* 
 We have actually about five almost-identical copies of the code below in our repository.
 I am adding now the sixth because I need it fast. But we should try to clean that up.
 My suggestion: Adopt the strategy used by LAr, put a object in SG containing MBTS times
 (and counts, possibly energies) usable by all clients.
 */

MBTSTimeDiffEventInfoAlg::MBTSTimeDiffEventInfoAlg(const std::string &name,
    ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_timeDiffThreshold(10.0)
  , m_minhitsperside(2)
  , m_mbts_threshold(40.0 / 222.0)
  , m_tileTBID(0)
  , m_mbtsContainerName("MBTSContainer")
  , m_mask(TileCell::MASK_BADCH | TileCell::MASK_OVER | TileCell::MASK_TIME)
  , m_pattern(TileCell::MASK_TIME)
{
  declareProperty("MBTS_Threshold", m_mbts_threshold);  // Value in pC
  declareProperty("MinHitsPerSide", m_minhitsperside);
  declareProperty("TimeDiffThreshold", m_timeDiffThreshold);
  declareProperty("MBTSContainer", m_mbtsContainerName);
}

StatusCode MBTSTimeDiffEventInfoAlg::initialize() {
  if (m_minhitsperside == 0)
    m_minhitsperside = 1; //Avoid div-by-zero later
  ATH_MSG_INFO("Initializing. MBTS_Threshold=" << m_mbts_threshold
               << ", MinHitsPerSide=" << m_minhitsperside
               << ",TimeDiffThreshold=" << m_timeDiffThreshold );

  CHECK( detStore()->retrieve(m_tileTBID));

  return StatusCode::SUCCESS;
}

StatusCode MBTSTimeDiffEventInfoAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode MBTSTimeDiffEventInfoAlg::execute() {

  const EventInfo * eventInfo_c = 0;
  CHECK( evtStore()->retrieve(eventInfo_c));

  EventInfo* eventInfo = const_cast<EventInfo*>(eventInfo_c);

  const TileCellContainer *tileCellCnt = 0;
  float eneA = 0.F;
  float eneC = 0.F;
  float timeA = 0.F;
  float timeC = 0.F;
  unsigned countA = 0;
  unsigned countC = 0;

  CHECK( evtStore()->retrieve(tileCellCnt, m_mbtsContainerName) );

  // ATH_MSG_DEBUG( "Mask:" << (int)mask << " Pattern:" << (int)pattern );

  TileCellContainer::const_iterator itr = tileCellCnt->begin();
  TileCellContainer::const_iterator itr_end = tileCellCnt->end();
  for (; itr != itr_end; ++itr) {
    if ((*itr)->energy() < m_mbts_threshold) continue;
    //Got cell above threshold

    const uint8_t qbit1 = (*itr)->qbit1();
    const Identifier& id = (*itr)->ID();

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "Working on MBTS cell side=" << m_tileTBID->type(id)
          << " module_id=" << m_tileTBID->module(id)
          << " channel_id=" << m_tileTBID->channel(id)
          << " Energy=" << (*itr)->energy()
          << " Time=" << (*itr)->time()
          << " Qbit1=" << (int) qbit1
          << " Q1Mask(BC|OF|AP|TM)=[" << ((qbit1 & TileCell::MASK_BADCH) != 0)
          << "|" << ((qbit1 & TileCell::MASK_OVER) != 0) << "|"
          << ((qbit1 & TileCell::MASK_AMPL) != 0) << "|"
          << ((qbit1 & TileCell::MASK_TIME) != 0) << "] "
          //	         << " Q2Mask(BC|OF|AP|TM)=[" << (qbit2 & TileCell::MASK_BADCH)==1 << "|" <<  (qbit2 &  TileCell::MASK_OVER)==1 << "|"
          //	         << (qbit2 &  TileCell::MASK_AMPL) << "|" <<  (qbit2 &  TileCell::MASK_TIME) << "]"
          << endmsg;
    }

    //Accept only cells with time computed, not known to be bad and no overflow:
    if ((qbit1 & m_mask) != m_pattern) {
      ATH_MSG_DEBUG( "Rejected based on quality bits" );
      continue;
    }

    // MBTS Id type is  "side"  +/- 1
    const int type_id = m_tileTBID->type(id);
    if (type_id == 1) { //A-side
      eneA += (*itr)->energy();
      timeA += (*itr)->time();
      countA++;
    } else if (type_id == -1) { //C-side
      eneC += (*itr)->energy();
      timeC += (*itr)->time();
      countC++;
    } else {
      ATH_MSG_WARNING( "Unexpected 'side' from MBTS identifier " );
      continue;
    }
  } //end loop over MBTS cells

  if (countA > 0) timeA /= countA;
  if (countC > 0) timeC /= countC;

  if (countA > m_minhitsperside && countC > m_minhitsperside) {
    ATH_MSG_DEBUG( "Got MBTS cells above threshold on both sides" );
    const float timediff = fabs(timeA - timeC);
    ATH_MSG_DEBUG( "Time diff " << timediff << "(" << m_timeDiffThreshold << ")");

    if (timediff > m_timeDiffThreshold) {
      ATH_MSG_DEBUG( "Event identified as background, set bit 'MBTSTimeDiffHalo' in EventInfo Background word" );

      if (eventInfo->setEventFlagBit(EventInfo::Background,
          EventInfo::MBTSTimeDiffHalo) == false)

        ATH_MSG_WARNING( "Failed to set EventInfo Background word!" );
    } //end if above theshold
    else {
      ATH_MSG_DEBUG( "Event identified as collision, set bit 'MBTSTimeDiffCol' in EventInfo Background word" );

      if (eventInfo->setEventFlagBit(EventInfo::Background,
          EventInfo::MBTSTimeDiffCol) == false)
        ATH_MSG_WARNING( "Failed to set EventInfo Background word!" );
    } //end if below threshold
  } // end if have counts on both sides
  else
    ATH_MSG_DEBUG( "Not enough hits above threshold to distinguish halo from collision event");

  MBTSCollisionTime * mbtsTime = new MBTSCollisionTime(countA, countC, eneA,
      eneC, timeA, timeC);
  if (evtStore()->record(mbtsTime, "MBTSCollisionTime").isFailure()) {
    ATH_MSG_WARNING( "Cannot record MBTSCollisionTime " );
  } else {
    ATH_MSG_DEBUG( "MBTSCollisionTime recorded in event store" );
    ATH_MSG_DEBUG( " countA=" << countA
                  << " countC=" << countC
                  << " eneA=" << eneA
                  << " eneC=" << eneC
                  << " timeA=" << timeA
                  << " timeC=" << timeC );
  }

  return StatusCode::SUCCESS;
}
