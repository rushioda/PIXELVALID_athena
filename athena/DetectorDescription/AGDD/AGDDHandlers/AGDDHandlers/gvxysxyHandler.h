/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef gvxysxyHandler_H
#define gvxysxyHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class gvxysxyHandler:public XMLHandler {
public:
	gvxysxyHandler(std::string);
	void ElementHandle();

};

#endif
