/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARRAMPSUPERCELL_H
#define LARCOOLCONDITIONS_LARRAMPSUPERCELL_H

#include "LArElecCalib/ILArRamp.h" 
#include "LArCOOLConditions/LArCondSuperCellBase.h"
#include "LArCOOLConditions/LArRampBlob.h"

class CondAttrListCollection;

class LArRampSC: 
  public ILArRamp, public LArCondSuperCellBase, public LArRampBlob
{  
 private:
  LArRampSC();

 public:  
  LArRampSC(const CondAttrListCollection* attrList);
  virtual ~LArRampSC( );

  bool good() const { return m_isInitialized && m_nChannels>0; }

  virtual  RampRef_t ADC2DAC(const HWIdentifier&  CellID, int gain) const ;  
  virtual  RampRef_t ADC2DAC(const Identifier&  CellID, int gain) const;
  
};

CLASS_DEF( LArRampSC , 43028438, 1 )


#endif 
