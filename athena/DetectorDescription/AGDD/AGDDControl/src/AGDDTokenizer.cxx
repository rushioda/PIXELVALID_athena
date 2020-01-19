/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/AGDDTokenizer.h"
#include <iostream>

AGDDTokenizer::AGDDTokenizer(const std::string & sep, const std::string & input) {
  std::string::size_type i=0, j=0;
  while( (j=input.find(sep,i))!=std::string::npos) {
    push_back(input.substr(i,j-i));
    i = j+sep.size();
  }
  push_back(input.substr(i));
}
