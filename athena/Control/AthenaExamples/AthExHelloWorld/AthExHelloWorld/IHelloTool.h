/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_IHELLOTOOL_H
#define ATHEXHELLOWORLD_IHELLOTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

class IHelloTool : virtual public IAlgTool {
public:
   virtual StatusCode saySomething() = 0;
   static const InterfaceID& interfaceID() {
     static const InterfaceID _IHelloToolID( "IHelloTool", 1, 0 );
     return _IHelloToolID;
   }
};
#endif // !ATHEXHELLOWORLD_IHELLOTOOL_H
