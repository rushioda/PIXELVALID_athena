/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef LARCOOLCONDITIONS_LARMPHYSOVERMCALSC_H
#define LARCOOLCONDITIONS_LARMPHYSOVERMCALSC_H

#include "LArElecCalib/ILArNoise.h" 
#include "LArCOOLConditions/LArSingleFloatBlob.h"
#include "LArCOOLConditions/LArCondSuperCellBase.h"


class CondAttrListCollection;

class LArNoiseSC: public ILArNoise,
                          public LArCondSuperCellBase,
		          public LArSingleFloatBlob {

public:
  LArNoiseSC(); 
  LArNoiseSC(const CondAttrListCollection* attrList);

  virtual ~LArNoiseSC();

  bool good() const { return m_isInitialized && m_nChannels>0; }
  
  // retrieving LArNoise using online ID  
  virtual const float& noise(const HWIdentifier& chid, int gain) const;

  // retrieving LArNoise using offline ID  
  virtual const float& noise(const Identifier& id, int gain) const; 
  
private:
  //static const float errorcode;

};

CLASS_DEF( LArNoiseSC ,234451881 , 1 )

#endif 
