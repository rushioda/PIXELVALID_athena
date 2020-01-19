/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// sTgcDigit.h

#ifndef sTgcDigitUH
#define sTgcDigitUH

// Nektarios Chr. Benekos
// March 2013
//
// sTGC digitization.

#include <iosfwd>
#include <inttypes.h>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

class sTgcDigit : public MuonDigit {

 public:  // functions

  // Default constructor.
  sTgcDigit();

  // Full constructor --- From Identifier.
  sTgcDigit(const Identifier& id, float time);
  
  // Full constructor --- From Identifier and bctag
  sTgcDigit(const Identifier& id, uint16_t bctag, float time);

  // Full constructor --- From Identifier, bctag and charge
  sTgcDigit(const Identifier& id, uint16_t bctag, float time, float charge);

  // Is this a valid digit?
  bool is_valid( const sTgcIdHelper *sTgcHelper) const;

  // get BC Tag
  uint16_t bcTag() const;
  enum {BC_UNDEFINED=0, BC_PREVIOUS, BC_CURRENT, BC_NEXT};

  // get the charge
  float charge() const;
  int charge_6bit() const;
  int charge_10bit() const;

  // get the time 
  float time() const;

  // Set bcTag 
  void set_bcTag(uint16_t newbcTag);
  
  // Set the charge
  void set_charge(float newCharge);
  

 private:  // data

  uint16_t  m_bcTag;
  float m_charge;
  float m_time;
};

#endif
