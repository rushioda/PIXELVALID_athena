/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_ITILECELLNOISETOOL_H
#define TILECONDITIONS_ITILECELLNOISETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloGain.h"

//NGO This interface should eventually go into a separate package that CaloNoiseTool 
//NGO can depend upon

class ITileCellNoiseTool : virtual public IAlgTool {
 public:

  virtual ~ITileCellNoiseTool() {}

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ITileCellNoiseTool("ITileCellNoiseTool", 1 , 0);
    return IID_ITileCellNoiseTool; 
  }
  
  virtual float getCellNoise(const Identifier& cell_id, CaloGain::CaloGain caloGain) const = 0;
  
};

#endif
