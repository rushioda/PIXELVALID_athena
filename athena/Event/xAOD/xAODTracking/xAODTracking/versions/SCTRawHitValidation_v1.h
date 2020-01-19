// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_SCTRAWHITVALIDATION_V1_H
#define XAODTRACKING_VERSIONS_SCTRAWHITVALIDATION_V1_H

// System include(s):
#include <vector>

// Core include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /** Class describing an SCT Raw Data Object (RDO).
   *
   *  @author Kilian Rosbach <kilian.rosbach@cern.ch>
   */
  class SCTRawHitValidation_v1 : public SG::AuxElement {
  public:
    SCTRawHitValidation_v1();

    /// set identifier of this RDO
    void setIdentifier(uint64_t new_identifier);

    /// return identifier of this RDO
    uint64_t identifier() const;

    /// set data word of this RDO
    void setWord(uint32_t new_word);

    /** access data word of this RDO and properties encoded therein
     *  @{
     */
    /// return the data word
    uint32_t getWord() const;

    /// return number of strips in this RDO (typically just 1)
    int getGroupSize() const;

    /// return ID of (first) strip
    int getStrip() const;

    /** Decode time bin information for the 3 consecutive bunch crossings.
     *
     *  This information is stored in 3 bits where the most significant bit
     *  corresponds to BC-1. So the 3 bit pattern is: (BC-1), BC, (BC+1).
     */
    int getTimeBin() const;

    /// return bitmask representing hit errors
    int getErrors() const;

    /// check if the time bin corresponding to the present BC is on
    bool OnTime() const;

    /// check if there is an error in the first hit's data
    bool FirstHitError() const;

    /// check if there is an error in the second hit's data
    bool SecondHitError() const;
    /// @}
  }; // end of the SCTRawHitValidation_v1 class definition

} // end of the xAOD namespace

#endif // XAODTRACKING_VERSIONS_SCTRAWHITVALIDATION_V1_H
