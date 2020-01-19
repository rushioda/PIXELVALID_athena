/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MetTOBArray.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.

#include "L1TopoEvent/MetTOBArray.h"

TCS::MetTOBArray::MetTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<MetTOB>(reserve)
{}

void
TCS::MetTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
