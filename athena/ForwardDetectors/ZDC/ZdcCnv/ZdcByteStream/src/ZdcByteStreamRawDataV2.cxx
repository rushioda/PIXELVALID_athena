/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcByteStreamRawDataV2.cxx
 *
 *  Updated: October 2015
 *  Author: Peter Steinberg (peter.steinberg@bnl.gov)
 *
 */

#include <utility>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"

#include "ZdcByteStream/ZdcDefs.h"
#include "ZdcByteStream/ZdcByteStreamRawDataV2.h"
#include "ZdcByteStream/ZdcToString.h"

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

//==================================================================================================
ZdcByteStreamRawDataV2::ZdcByteStreamRawDataV2(const std::string& name, ISvcLocator* pSvcLocator) :
	AthAlgorithm(name, pSvcLocator)
{
	declareProperty("ZdcTriggerTowerContainerLocation", m_ZdcTriggerTowerContainerLocation = ZdcDefs::ZdcTriggerTowerContainerLocation);
	declareProperty("ForceSlicesLUT", m_forceSlicesLut = 0);
	declareProperty("ForceSlicesFADC", m_forceSlicesFadc = 0);
}
//==================================================================================================


//==================================================================================================
ZdcByteStreamRawDataV2::~ZdcByteStreamRawDataV2()
{
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamRawDataV2::initialize()
{
	msg(MSG::INFO) << "Initializing " << name() << " - package version " << PACKAGE_VERSION
			<< endmsg;

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamRawDataV2::execute()
{
	if (!msgLvl(MSG::INFO)) return StatusCode::SUCCESS;
	msg(MSG::INFO);

	//ZdcDigitsCollection* ttCollection = 0;
	const DataHandle<xAOD::TriggerTowerContainer> ttCollection;

	msg(MSG::DEBUG) << "Looking for ZDC trigger tower container at " << m_ZdcTriggerTowerContainerLocation << endmsg;

	StatusCode sc = evtStore()->retrieve(ttCollection, m_ZdcTriggerTowerContainerLocation);
	if (sc.isFailure() || !ttCollection || ttCollection->empty())
	{
		msg() << "No Zdc Digits found" << endmsg;
		return StatusCode::SUCCESS;
	}

	msg(MSG::DEBUG) << ZdcToString(*ttCollection) << endmsg;

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
StatusCode ZdcByteStreamRawDataV2::finalize()
{

	return StatusCode::SUCCESS;
}
//==================================================================================================


//==================================================================================================
//FIXME There is no such thing like ZdcDigitsMap
void ZdcByteStreamRawDataV2::printZdcTriggerTowers() const
{
  //msg() << "Number of ZdcDigits = " << m_ZdcDigitsMap.size() << endmsg;
// 	ZdcDigitsMap::const_iterator mapIter = m_ZdcDigitsMap.begin();
// 	ZdcDigitsMap::const_iterator mapEnd = m_ZdcDigitsMap.end();
}
//==================================================================================================


//==================================================================================================
void ZdcByteStreamRawDataV2::printVec(const std::vector<int>& vec) const
{
	std::vector<int>::const_iterator pos;
	for (pos = vec.begin(); pos != vec.end(); ++pos)
	{
		if (pos != vec.begin()) msg() << ",";
		msg() << *pos;
	}
	msg() << "/";
}
//==================================================================================================

/* REMOVE

 // Set up trigger tower map
 void ZdcByteStreamRawDataV2::setupZdcDigitsMap(const ZdcDigitsCollection* const ttCollection)
 {
 m_ZdcDigitsMap.clear();
 ZdcDigitsCollection::const_iterator pos  = ttCollection->begin();
 ZdcDigitsCollection::const_iterator pose = ttCollection->end();
 for (; pos != pose; ++pos) {
 const ZdcDigits* const tt = *pos;
 tt->get_ID();

 //const unsigned int key = m_towerKey->ttKey(tt->phi(), tt->eta());
 //m_ttMap.insert(std::make_pair(key, tt));
 }
 }
 */
