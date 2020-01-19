/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef posXYZHandler_H
#define posXYZHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class posXYZHandler:public XMLHandler {
public:
	posXYZHandler(std::string);
	void ElementHandle();

};

#endif
