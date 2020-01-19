/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * CaloCellFastCopyTool.cxx
 *
 *  Created on: Oct 7, 2013
 *
 */

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloRec/CaloCellFastCopyTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloUtils/CaloSamplingHelper.h"

#include <algorithm>
#include <string>


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellFastCopyTool::CaloCellFastCopyTool(const std::string& type,
    const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent)
  , m_copyCells(nullptr)
  , m_copyConstCells(nullptr)
  , m_avoidDuplicates(false)
  , m_isFindCellFast(false)
  , m_caloID(0)
{
  declareInterface<ICaloCellMakerTool>(this);
  declareInterface<ICaloConstCellMakerTool>(this);
  declareProperty("InputName", m_srcCellContainerName = "AllCalo");
  declareProperty("IncludeSamplings", m_acceptedSampleNames);
  declareProperty("AvoidDuplicates", m_avoidDuplicates);
  declareProperty("IsFindCellFast", m_isFindCellFast);
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellFastCopyTool::initialize() {

  ATH_MSG_INFO( "In initialize ");

  CHECK( detStore()->retrieve(m_caloID));

  std::vector<bool> acceptedSamplings(m_caloID->Unknown, false);
  std::vector<CaloSampling::CaloSample> acceptedSamples;

  std::vector<std::string>::const_iterator fName = m_acceptedSampleNames.begin();
  for (; fName != m_acceptedSampleNames.end(); ++fName) {
    CaloSampling::CaloSample sampInd = CaloSamplingHelper::getSamplingId(*fName);
    if (sampInd != CaloSampling::Unknown) {
      acceptedSamplings[sampInd] = true;
      acceptedSamples.push_back(sampInd);
    }
  }

  CaloSamplingHelper::getCalos(acceptedSamples, m_acceptedCalos);

  if (msgLvl(MSG::DEBUG)) {

    msg(MSG::DEBUG) << "Accepted calo samplings: ";
    std::vector<CaloSampling::CaloSample>::iterator itSampling = acceptedSamples.begin();
    for (; itSampling < acceptedSamples.end(); ++itSampling) {
      msg(MSG::DEBUG) << (int) (*itSampling) << "; ";
    }
    msg(MSG::DEBUG) << endmsg;

    msg(MSG::DEBUG) << "Accepted sub calos: ";
    std::vector<CaloCell_ID::SUBCALO>::iterator itCalo = m_acceptedCalos.begin();
    for (; itCalo < m_acceptedCalos.end(); ++itCalo) {
      msg(MSG::DEBUG) << (int) (*itCalo) << "; ";
    }
    msg(MSG::DEBUG) << endmsg;
  }

  m_hashMax = m_caloID->calo_cell_hash_max();
  ATH_MSG_DEBUG( "CaloCell hash max: " << m_hashMax);

  if (m_isFindCellFast) {
    for (IdentifierHash cellHash = 0; cellHash < m_hashMax; cellHash += 1) {
      if (acceptedSamplings[m_caloID->calo_sample(cellHash)]) {
        m_acceptedCellHashes.push_back(cellHash);
      }
    }
  } else {
    m_cellsToBeCopied.resize(m_hashMax, false);
    for (IdentifierHash cellHash = 0; cellHash < m_hashMax; cellHash += 1) {
      if (acceptedSamplings[m_caloID->calo_sample(cellHash)]) {
        m_cellsToBeCopied[cellHash] = true;
      }
    }
  }

  ATH_MSG_DEBUG( "Number of accepted hashes: " << m_acceptedCellHashes.size() );


  m_copyCells = &CaloCellFastCopyTool::dispatchCopy;
  m_copyConstCells = &CaloCellFastCopyTool::dispatchCopyConst;

  return StatusCode::SUCCESS;
}


StatusCode CaloCellFastCopyTool::process(CaloCellContainer* theCont)
{
  // Retrieve source cell container
  const CaloCellContainer* srcCont;
  CHECK( evtStore()->retrieve(srcCont, m_srcCellContainerName));

  CHECK( (this->*m_copyCells)(srcCont, theCont) );

  std::vector<CaloCell_ID::SUBCALO>::const_iterator it = m_acceptedCalos.begin();
  for (; it != m_acceptedCalos.end(); ++it) {
    if (srcCont->hasCalo(*it)) theCont->setHasCalo(*it);
  }

  return StatusCode::SUCCESS;
}


StatusCode CaloCellFastCopyTool::process(CaloConstCellContainer* theCont)
{
  // Retrieve source cell container
  const CaloCellContainer* srcCont;
  CHECK( evtStore()->retrieve(srcCont, m_srcCellContainerName));

  CHECK( (this->*m_copyConstCells)(srcCont, theCont) );

  std::vector<CaloCell_ID::SUBCALO>::const_iterator it = m_acceptedCalos.begin();
  for (; it != m_acceptedCalos.end(); ++it) {
    if (srcCont->hasCalo(*it)) theCont->setHasCalo(*it);
  }

  return StatusCode::SUCCESS;
}


StatusCode CaloCellFastCopyTool::viewNotAvoidingDuplicatesFindCellIsFast
  (const CaloCellContainer* srcCont,
   CaloConstCellContainer* destCont)
{
  const CaloCell* cell;
  std::vector<IdentifierHash>::const_iterator it = m_acceptedCellHashes.begin();
  for (; it != m_acceptedCellHashes.end(); ++it) {
    cell = srcCont->findCell(*it);
    if (cell) destCont->push_back_fast(cell);
  }

  return StatusCode::SUCCESS;
}


StatusCode CaloCellFastCopyTool::viewAvoidingDuplicatesFindCellIsFast
  (const CaloCellContainer* srcCont,
   CaloConstCellContainer* destCont)
{
  std::vector<bool> notInContainer(m_hashMax, true);
  CaloCellContainer::const_iterator itCell = destCont->begin();
  for (; itCell != destCont->end(); ++itCell) {
    notInContainer[(*itCell)->caloDDE()->calo_hash()] = false;
  }

  const CaloCell* cell;
  std::vector<IdentifierHash>::const_iterator it = m_acceptedCellHashes.begin();
  for (; it != m_acceptedCellHashes.end(); ++it) {
    if (notInContainer[*it]) {
      cell = srcCont->findCell(*it);
      if (cell) destCont->push_back_fast(cell);
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode CaloCellFastCopyTool::viewNotAvoidingDuplicatesFindCellIsNotFast
  (const CaloCellContainer* srcCont,
   CaloConstCellContainer* destCont)
{
  IdentifierHash cellHash;
  const CaloCell* cell;
  CaloCellContainer::const_iterator itSrcCell = srcCont->begin();
  for (; itSrcCell != srcCont->end(); ++itSrcCell) {
    cell = (*itSrcCell);
    cellHash = cell->caloDDE()->calo_hash();
    if (m_cellsToBeCopied[cellHash])
      destCont->push_back_fast(cell);
  }
  return StatusCode::SUCCESS;
}


StatusCode CaloCellFastCopyTool::viewAvoidingDuplicatesFindCellIsNotFast
  (const CaloCellContainer* srcCont,
   CaloConstCellContainer* destCont)
{
  std::vector<bool> notInContainer(m_hashMax, true);
  CaloCellContainer::const_iterator itCell = destCont->begin();
  for (; itCell != destCont->end(); ++itCell) {
    notInContainer[(*itCell)->caloDDE()->calo_hash()] = false;
  }

  IdentifierHash cellHash;
  const CaloCell* cell;
  CaloCellContainer::const_iterator itSrcCell = srcCont->begin();
  for (; itSrcCell != srcCont->end(); ++itSrcCell) {
    cell = (*itSrcCell);
    cellHash = cell->caloDDE()->calo_hash();
    if (m_cellsToBeCopied[cellHash] && notInContainer[cellHash])
      destCont->push_back_fast(cell);
  }

  return StatusCode::SUCCESS;
}


template <class CONTAINER>
StatusCode CaloCellFastCopyTool::cloneNotAvoidingDuplicatesFindCellIsFast
  (const CaloCellContainer* srcCont,
   CONTAINER* destCont)
{
  const CaloCell* cell;
  std::vector<IdentifierHash>::const_iterator it = m_acceptedCellHashes.begin();
  for (; it != m_acceptedCellHashes.end(); ++it) {
    cell = srcCont->findCell(*it);
    if (cell) destCont->push_back_fast(cell->clone());
  }

  return StatusCode::SUCCESS;
}


template <class CONTAINER>
StatusCode CaloCellFastCopyTool::cloneAvoidingDuplicatesFindCellIsFast
  (const CaloCellContainer* srcCont,
   CONTAINER* destCont)
{

  std::vector<bool> notInContainer(m_hashMax, true);
  CaloCellContainer::const_iterator itCell = destCont->begin();
  for (; itCell != destCont->end(); ++itCell) {
    notInContainer[(*itCell)->caloDDE()->calo_hash()] = false;
  }

  const CaloCell* cell;
  std::vector<IdentifierHash>::const_iterator it = m_acceptedCellHashes.begin();
  for (; it != m_acceptedCellHashes.end(); ++it) {
    if (notInContainer[*it]) {
      cell = srcCont->findCell(*it);
      if (cell) destCont->push_back_fast(cell->clone());
    }
  }

  return StatusCode::SUCCESS;
}


template <class CONTAINER>
StatusCode CaloCellFastCopyTool::cloneNotAvoidingDuplicatesFindCellIsNotFast
  (const CaloCellContainer* srcCont,
   CONTAINER* destCont)
{
  IdentifierHash cellHash;
  const CaloCell* cell;
  CaloCellContainer::const_iterator itSrcCell = srcCont->begin();
  for (; itSrcCell != srcCont->end(); ++itSrcCell) {
    cell = (*itSrcCell);
    cellHash = cell->caloDDE()->calo_hash();
    if (m_cellsToBeCopied[cellHash])
      destCont->push_back_fast(cell->clone());
  }

  return StatusCode::SUCCESS;
}


template <class CONTAINER>
StatusCode CaloCellFastCopyTool::cloneAvoidingDuplicatesFindCellIsNotFast
  (const CaloCellContainer* srcCont,
   CONTAINER* destCont)
{
  std::vector<bool> notInContainer(m_hashMax, true);
  CaloCellContainer::const_iterator itCell = destCont->begin();
  for (; itCell != destCont->end(); ++itCell) {
    notInContainer[(*itCell)->caloDDE()->calo_hash()] = false;
  }

  IdentifierHash cellHash;
  const CaloCell* cell;
  CaloCellContainer::const_iterator itSrcCell = srcCont->begin();
  for (; itSrcCell != srcCont->end(); ++itSrcCell) {
    cell = (*itSrcCell);
    cellHash = cell->caloDDE()->calo_hash();
    if (m_cellsToBeCopied[cellHash] && notInContainer[cellHash])
      destCont->push_back_fast(cell->clone());
  }

  return StatusCode::SUCCESS;
}



StatusCode CaloCellFastCopyTool::dispatchCopy(const CaloCellContainer* srcCont,
                                              CaloCellContainer* destCont)
{
  if (m_isFindCellFast) {
    if (destCont->ownPolicy() == SG::OWN_ELEMENTS) {
      m_copyCells = (m_avoidDuplicates) ?
        (&CaloCellFastCopyTool::cloneAvoidingDuplicatesFindCellIsFast<CaloCellContainer>) :
        (&CaloCellFastCopyTool::cloneNotAvoidingDuplicatesFindCellIsFast<CaloCellContainer>);
    }
    else {
      REPORT_ERROR(StatusCode::FAILURE) << "Can't copy to a non-const view container.";
      return StatusCode::FAILURE;
    }
  }
  else {
    if (destCont->ownPolicy() == SG::OWN_ELEMENTS) {
      m_copyCells = (m_avoidDuplicates) ?
        (&CaloCellFastCopyTool::cloneAvoidingDuplicatesFindCellIsNotFast<CaloCellContainer>) :
        (&CaloCellFastCopyTool::cloneNotAvoidingDuplicatesFindCellIsNotFast<CaloCellContainer>);
    }
    else {
      REPORT_ERROR(StatusCode::FAILURE) << "Can't copy to a non-const view container.";
      return StatusCode::FAILURE;
    }
  }

  return (this->*m_copyCells)(srcCont, destCont);
}


StatusCode
CaloCellFastCopyTool::dispatchCopyConst (const CaloCellContainer* srcCont,
                                         CaloConstCellContainer* destCont)
{
  if (m_isFindCellFast) {
    if (destCont->ownPolicy() == SG::OWN_ELEMENTS) {
      m_copyConstCells = (m_avoidDuplicates) ?
        (&CaloCellFastCopyTool::cloneAvoidingDuplicatesFindCellIsFast<CaloConstCellContainer>) :
        (&CaloCellFastCopyTool::cloneNotAvoidingDuplicatesFindCellIsFast<CaloConstCellContainer>);
    } else {
      m_copyConstCells = (m_avoidDuplicates) ? 
        (&CaloCellFastCopyTool::viewAvoidingDuplicatesFindCellIsFast) :
        (&CaloCellFastCopyTool::viewNotAvoidingDuplicatesFindCellIsFast);
    }
  } else {
    if (destCont->ownPolicy() == SG::OWN_ELEMENTS) {
      m_copyConstCells = (m_avoidDuplicates) ?
        (&CaloCellFastCopyTool::cloneAvoidingDuplicatesFindCellIsNotFast<CaloConstCellContainer>) :
        (&CaloCellFastCopyTool::cloneNotAvoidingDuplicatesFindCellIsNotFast<CaloConstCellContainer>);
    } else {
      m_copyConstCells = (m_avoidDuplicates) ?
        (&CaloCellFastCopyTool::viewAvoidingDuplicatesFindCellIsNotFast) :
        (&CaloCellFastCopyTool::viewNotAvoidingDuplicatesFindCellIsNotFast);
    }
  }

  return (this->*m_copyConstCells)(srcCont, destCont);
}
