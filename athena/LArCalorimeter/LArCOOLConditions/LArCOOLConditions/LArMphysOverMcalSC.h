/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARMPHYSOVERMCALSC_H
#define LARCOOLCONDITIONS_LARMPHYSOVERMCALSC_H

#include "LArElecCalib/ILArMphysOverMcal.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArMphysOverMcalSC: public ILArMphysOverMcal,
                          public LArCondSuperCellBase,
		          public LArSingleFloatBlob {

public:
  LArMphysOverMcalSC(); 
  LArMphysOverMcalSC(const CondAttrListCollection* attrList);

  virtual ~LArMphysOverMcalSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArMphysOverMcal using online ID  
  virtual const float& MphysOverMcal(const HWIdentifier& chid, int gain) const;

  // retrieving LArMphysOverMcal using offline ID  
  virtual const float& MphysOverMcal(const Identifier& id, int gain) const; 
  
private:
  //  static const float s_errorcode;

};

CLASS_DEF( LArMphysOverMcalSC ,125003343 , 1 )

#endif 
