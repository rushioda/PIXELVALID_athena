/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileRawChannelVerify.cxx
//  Author   : Zhifang
//  Created  : May, 2002
//
//  DESCRIPTION:
//     Implement the TileRawChannelVerify class
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// Atlas includes
// access all RawChannels inside container
#include "EventContainers/SelectAllObject.h" 
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileRecUtils/TileRawChannelVerify.h"

// C++ STL includes
#include <vector>
#include <algorithm>

// C includes
#include <cmath>

using namespace std;


/**
@class CompRawChannel
@brief Small class holding a single method to compare two different TileRawChannel  
 
*/
class CompRawChannel: public binary_function<const TileRawChannel*, const TileRawChannel*, bool> {
  public:
    bool operator()(const TileRawChannel* p1, const TileRawChannel* p2) {
      return p1->amplitude() < p2->amplitude();
    }
};

//==========================================================================
// TileRawChannelVerify's implementations
//==========================================================================

// Constructor
TileRawChannelVerify::TileRawChannelVerify(string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileHWID(0)
{
  declareProperty("TileRawChannelContainer1", m_rawChannelContainer1 = "TileRawChannelContainer1");
  declareProperty("TileRawChannelContainer2", m_rawChannelContainer2 = "TileRawChannelContainer2");
  declareProperty("Precision", m_precision = 0);
  declareProperty("DumpRawChannels", m_dumpRawChannels = false);
  declareProperty("SortFlag", m_sortFlag = false);
}

TileRawChannelVerify::~TileRawChannelVerify() {
}

// Alg standard interfacw function
StatusCode TileRawChannelVerify::initialize() {

  // retrieve TileHWID helper from det store
  CHECK( detStore()->retrieve(m_tileHWID) );

  ATH_MSG_INFO( "TileRawChannelVerify initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelVerify::execute() {

  // step1: read two cell containers from TES
  const TileRawChannelContainer* pRawChannels1;
  const TileRawChannelContainer* pRawChannels2;
  CHECK( evtStore()->retrieve(pRawChannels1, m_rawChannelContainer1) );
  CHECK( evtStore()->retrieve(pRawChannels2, m_rawChannelContainer2) );

  SelectAllObject<TileRawChannelContainer> selAll1(pRawChannels1);
  SelectAllObject<TileRawChannelContainer>::const_iterator rawItr1 = selAll1.begin();
  SelectAllObject<TileRawChannelContainer>::const_iterator end1 = selAll1.end();

  SelectAllObject<TileRawChannelContainer> selAll2(pRawChannels2);
  SelectAllObject<TileRawChannelContainer>::const_iterator rawItr2 = selAll2.begin();
  SelectAllObject<TileRawChannelContainer>::const_iterator end2 = selAll2.end();

  // step2: first compare the number of cells in the two containers 
  int nSize1 = 0;
  for (; rawItr1 != end1; ++rawItr1) ++nSize1;

  int nSize2 = 0;
  for (; rawItr2 != end2; ++rawItr2) ++nSize2;

  ATH_MSG_INFO( "The number of cells in " << m_rawChannelContainer1 << " is " << nSize1 );
  ATH_MSG_INFO( "The number of cells in " << m_rawChannelContainer2 << " is " << nSize2 );

  if (nSize1 != nSize2) {
    ATH_MSG_ERROR( "The number of rawChannels is not equal in the two containers" );
    return (StatusCode::SUCCESS);
  }

  // step3: to sort the cells in the containers by amplitude
  vector<const TileRawChannel*> v1;
  vector<const TileRawChannel*> v2;
  const TileRawChannel* p1;
  const TileRawChannel* p2;
  if (m_sortFlag) {
    rawItr1 = selAll1.begin();
    end1 = selAll1.end();
    for (; rawItr1 != end1; ++rawItr1)
      v1.push_back((*rawItr1));

    sort(v1.begin(), v1.end(), CompRawChannel());

    rawItr2 = selAll2.begin();
    end2 = selAll2.end();
    for (; rawItr2 != end2; ++rawItr2)
      v2.push_back((*rawItr2));

    sort(v2.begin(), v2.end(), CompRawChannel());
  }

  rawItr1 = selAll1.begin();
  end1 = selAll1.end();

  rawItr2 = selAll2.begin();
  end2 = selAll2.end();

  // step4: then compare every cell-pair in the containers
  bool bErrorFlag = false;
  bool bHeaderFlag = true;
  for (int i = 0; i < nSize1; ++i) {
    if (m_sortFlag) {
      p1 = v1[i];
      p2 = v2[i];
    } else {
      p1 = (*rawItr1);
      ++rawItr1;
      p2 = (*rawItr2);
      ++rawItr2;
    }
    HWIdentifier id1 = p1->adc_HWID();
    HWIdentifier id2 = p2->adc_HWID();
    double amp1 = p1->amplitude();
    double amp2 = p2->amplitude();
    double diff = fabs(amp1 - amp2);
    if (id1 != id2 || diff > m_precision) bErrorFlag = true;
    if (msgLvl(MSG::VERBOSE) && (m_dumpRawChannels || bErrorFlag)) {
      if (bHeaderFlag) {
        msg(MSG::VERBOSE) << "             ===" << m_rawChannelContainer1 << "===      ===" << m_rawChannelContainer2 << "===" << endmsg;
        msg(MSG::VERBOSE) << "  Index      e1            id1        |        e2           id2" << endmsg;
        msg(MSG::VERBOSE) << "--------------------------------------------------------------------------------" << endmsg;
        bHeaderFlag = false;
      }
      msg(MSG::VERBOSE) << setw(5) << i
                        << "   " << setw(12)  << amp1
                        << "   [" << m_tileHWID->to_string(id1) << "]"
                        << "  |  " << setw(12) << amp2
                        << "   [" << m_tileHWID->to_string(id2) << "]";
      if (diff > m_precision) {
        msg(MSG::VERBOSE) << " A* ";
      }
      if (id1 != id2) {
        msg(MSG::VERBOSE) << " I* ";
      }
      msg(MSG::VERBOSE) << endmsg;
    } else if (bErrorFlag) {
      break;
    }
  }
  if (!bHeaderFlag) {
    msg(MSG::VERBOSE) << "--------------------------------------------------------------------------------" << endmsg;
  }
  if (!bErrorFlag) {
    ATH_MSG_INFO( "The two cellContainers (" << m_rawChannelContainer1
                 << " and " << m_rawChannelContainer2 << ") are the same!!!" );
  } else {
    ATH_MSG_INFO( "The two cellContainers (" << m_rawChannelContainer1
                 << " and " << m_rawChannelContainer2 << ") are not the same!!!" );
  }

  // Execution completed.
  ATH_MSG_INFO( "TileRawChannelVerify execution completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileRawChannelVerify::finalize() {

  ATH_MSG_INFO( "TileRawChannelVerify finalized successfully" );

  return StatusCode::SUCCESS;
}

