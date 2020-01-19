/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TGCcabling/TGCDatabaseSLBToROD.h"

namespace LVL1TGCCabling8 
{

void TGCDatabaseSLBToROD::readDB (void) {
  std::ifstream file(filename.c_str());
  std::string buf;

  unsigned int space = blockname.find(" ");
  std::string module = blockname.substr(0,space);
  std::string type = blockname.substr(space+1);

  while(getline(file,buf)){
    if(buf.substr(0,module.size())==module) break;
  }

  while(getline(file,buf)){
    if(buf.substr(1,type.size())==type) break;
  }

  while(getline(file,buf)){
    if(buf.substr(0,2)!="  ") break;
    std::istringstream line(buf);
    std::vector<int> entry;
    for(int i=0; i<4; i++){
      int temp=-1;
      line >> temp; 
      entry.push_back(temp);
    }
    database.push_back(entry);
  }
  
  file.close();
}

} // end of namespace
