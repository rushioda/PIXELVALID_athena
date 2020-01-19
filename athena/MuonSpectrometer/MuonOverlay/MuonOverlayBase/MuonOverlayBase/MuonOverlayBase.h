/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Generic overlaying code for boolean-like hits.
// Factored out from InDetOverlay.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006-2008

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#ifndef MUONOVERLAYBASE_H
#define MUONOVERLAYBASE_H

#include <string>
#include "IDC_OverlayBase/IDC_OverlayBase.h"

class ISvcLocator;
class StoreGateSvc;

class MuonOverlayBase : public IDC_OverlayBase  {
public:
  
  MuonOverlayBase(const std::string &name, ISvcLocator *pSvcLocator)
    : IDC_OverlayBase(name, pSvcLocator), m_copyObjects(0)
  {}

  template<class TypeToBeCopied> 
  void copyMuonObjects(StoreGateSvc *to, StoreGateSvc *from, std::string key);

  template<class TypeToBeCopied, class Datum> 
  void copyMuonIDCobject(StoreGateSvc* from, StoreGateSvc *to);

  template<class TypeToBeCopied, class Datum> 
  TypeToBeCopied* copyMuonDigitContainer(const TypeToBeCopied* oldObject);

protected:

  bool m_copyObjects;

};

#include "MuonOverlayBase/MuonOverlayBase.icc"

#endif /* MUONOVERLAYBASE_H */

