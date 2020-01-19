/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef defaultsHandler_H
#define defaultsHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class defaultsHandler:public XMLHandler {
public:
	defaultsHandler(std::string);
	void ElementHandle();

};

#endif
