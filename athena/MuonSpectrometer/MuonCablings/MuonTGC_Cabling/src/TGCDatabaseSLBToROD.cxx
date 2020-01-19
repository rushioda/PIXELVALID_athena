/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGC_Cabling/TGCDatabaseSLBToROD.h"

#include <fstream>
#include <sstream>

namespace MuonTGC_Cabling 
{

TGCDatabaseSLBToROD::TGCDatabaseSLBToROD(std::string filename, std::string blockname) 
   : TGCDatabase(TGCDatabase::SLBToROD, filename, blockname)
{
  // read out ascii file and fill database
  if(database.size()==0) readDB();
}

TGCDatabaseSLBToROD::TGCDatabaseSLBToROD(const TGCDatabaseSLBToROD& right)
  : TGCDatabase(right)
{
}

void TGCDatabaseSLBToROD::readDB(void) {
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
    for(int i=0; i<6; i++){
      int temp=-1;
      line >> temp; 
      entry.push_back(temp);
    }
    database.push_back(entry);
  }
  
  file.close();
}

TGCDatabaseSLBToROD::~TGCDatabaseSLBToROD(void)
{
}

bool TGCDatabaseSLBToROD::update(const std::vector<int>& input)
{
  int ip = find(input);
  if(ip<0) return false;

  if(input.size()<6) { 
    // SSW block
    database[ip].at(1) = input.at(1);
  } else {
    // SLB block
    database[ip].at(4) = input.at(4);
    database[ip].at(5) = input.at(5);
  }

  return true;
}
 
int  TGCDatabaseSLBToROD::find(const std::vector<int>& channel) const
{
  int index=-1;

  const unsigned int size = database.size();

  if(channel.size()<4) { 
    // SSW block
    for(unsigned int i=0; i<size; i++){
      if(database[i].at(0) == channel.at(0)){
	index = i;
	break;
      }
    }
  } else {
    // SLB block
    for(unsigned int i=0; i<size; i++){
      if(database[i].at(3) == channel.at(3) && 
	 database[i].at(2) == channel.at(2) &&
	 database[i].at(1) == channel.at(1) &&
	 database[i].at(0) == channel.at(0)){
	index = i;
	break;
      }
    }
  }

  return index;
}

} // end of namespace
