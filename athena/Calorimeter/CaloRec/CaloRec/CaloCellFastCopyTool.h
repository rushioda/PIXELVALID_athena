// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * CaloCellFastCopyTool.h
 *
 *  Created on: Oct 7, 2013
 *
 */

#ifndef CALOREC_CALOCELLFASTCOPYTOOL_H_
#define CALOREC_CALOCELLFASTCOPYTOOL_H_

/**
 @class CaloCellFastCopyTool
 @brief Concrete tool for coping Calo cells

  CaloCellFastCopyTool to be used by CaloCellMaker algorithms.
  The tool copies Calo cells from an existing
  CaloCellContainer to the one processed by the CaloCellMaker algorithm.
  Properties of the tool:
    InputName - source cell container (default: "AllCalo")
    IncludeSamplings - name of CaloSampling to be copied into new CaloCellContainer
    AvoidDuplicates - indicates if this tool should avoid duplicates in
                      new CaloCellContainer.
    IsFindCellFast - indicates if source CaloCellContainer find CaloCell fast
                     (i.e. source CaloCellContainer is complete and ordered or
                      has already initialized internal lookUpTable).
                     In this case this tool can be very fast.

 */


// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdentifierHash.h"

// Calo includes
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <vector>

// Forward declaration
class CaloCell_ID;


class CaloCellFastCopyTool: public AthAlgTool,
                            virtual public ICaloCellMakerTool,
                            virtual public ICaloConstCellMakerTool
{
  public:

    /// AthAlgTool constructor
    CaloCellFastCopyTool(const std::string& name, const std::string& type,
        const IInterface* parent);

    //    virtual ~TileGapScintilatorsCopyTool();

    virtual StatusCode initialize();
    virtual StatusCode process(CaloCellContainer* theCellContainer);
    virtual StatusCode process(CaloConstCellContainer* theCellContainer);


  private:

    typedef StatusCode (CaloCellFastCopyTool::*COPY_CELLS)(const CaloCellContainer* srcCont
                                                           , CaloCellContainer* destCont);
    typedef StatusCode (CaloCellFastCopyTool::*COPY_CONST_CELLS)(const CaloCellContainer* srcCont
                                                                 , CaloConstCellContainer* destCont);


  StatusCode viewNotAvoidingDuplicatesFindCellIsFast(const CaloCellContainer* srcCont,
                                                     CaloConstCellContainer* destCont);
  StatusCode viewAvoidingDuplicatesFindCellIsFast(const CaloCellContainer* srcCont,
                                                  CaloConstCellContainer* destCont);
  StatusCode viewNotAvoidingDuplicatesFindCellIsNotFast(const CaloCellContainer* srcCont,
                                                        CaloConstCellContainer* destCont);
  StatusCode viewAvoidingDuplicatesFindCellIsNotFast(const CaloCellContainer* srcCont,
                                                     CaloConstCellContainer* destCont);


  template <class CONTAINER>
  StatusCode cloneNotAvoidingDuplicatesFindCellIsFast(const CaloCellContainer* srcCont,
                                                      CONTAINER* destCont);
  template <class CONTAINER>
  StatusCode cloneAvoidingDuplicatesFindCellIsFast(const CaloCellContainer* srcCont,
                                                   CONTAINER* destCont);

  template <class CONTAINER>
  StatusCode cloneNotAvoidingDuplicatesFindCellIsNotFast(const CaloCellContainer* srcCont,
                                                         CONTAINER* destCont);
  template <class CONTAINER>
  StatusCode cloneAvoidingDuplicatesFindCellIsNotFast(const CaloCellContainer* srcCont,
                                                      CONTAINER* destCont);

  StatusCode dispatchCopy(const CaloCellContainer* srcCont,
                          CaloCellContainer* destCont);
  StatusCode dispatchCopyConst(const CaloCellContainer* srcCont,
                               CaloConstCellContainer* destCont);

    COPY_CELLS m_copyCells;
    COPY_CONST_CELLS m_copyConstCells;

    std::string m_srcCellContainerName;  // name of input CaloCellContainer
    bool m_avoidDuplicates;
    bool m_isFindCellFast;
    IdentifierHash m_hashMax;
    const CaloCell_ID* m_caloID; //!< Pointer to CaloID helper

    std::vector<std::string> m_acceptedSampleNames;
    std::vector<CaloCell_ID::SUBCALO> m_acceptedCalos;

    // Calo cell hashes
    std::vector<IdentifierHash> m_acceptedCellHashes;
    std::vector<bool> m_cellsToBeCopied;

};


#endif /* CALOREC_CALOCELLFASTCOPYTOOL_H_ */
