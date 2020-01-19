/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTESTS_ISIMTESTTOOL_H
#define G4ATLASTESTS_ISIMTESTTOOL_H

#include "GaudiKernel/IAlgTool.h"
class ISimTestTool : virtual public IAlgTool{
public:
  virtual StatusCode processEvent() = 0;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISimTestTool, 1, 0);

};

#endif //G4ATLASTESTS_ISIMTESTTOOL_H
