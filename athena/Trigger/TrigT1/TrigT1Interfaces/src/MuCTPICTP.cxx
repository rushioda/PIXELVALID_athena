/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/MuCTPICTP.h"

namespace LVL1 {

  MuCTPICTP::MuCTPICTP( unsigned int word )
    : m_MuCTPICTPWord( word ) {

  }

  MuCTPICTP::~MuCTPICTP() {

  }

  unsigned int MuCTPICTP::muCTPIWord() const {
    return m_MuCTPICTPWord;
  }

} // namespace LVL1
