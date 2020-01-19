/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IL1TOPOHISTSVC_H
#define IL1TOPOHISTSVC_H

#include <iostream>

class TH1;

class IL1TopoHistSvc {
public:

   virtual void registerHist(TH1 * h) = 0;

   virtual TH1 * findHist(const std::string & histName) = 0;

   virtual void setBaseDir(const std::string & baseDir) = 0;

   virtual void save() = 0;

};

#endif
