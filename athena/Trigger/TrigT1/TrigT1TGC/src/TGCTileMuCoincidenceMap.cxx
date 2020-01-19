/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCTileMuCoincidenceMap.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "PathResolver/PathResolver.h"

#include "MuonCondInterface/ITGCTriggerDbTool.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"


namespace LVL1TGCTrigger {

 extern bool        g_TILE_MU;
 extern bool        g_USE_CONDDB;

TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap(const std::string& version)
  :m_verName(version),
   m_condDbTool("TGCTriggerDbTool")
{
  // intialize map
  for (size_t side=0; side< N_Side; side++){
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	for (size_t input=0; input< N_Input_TileMuModule; input++){
	  map[input][ssc][sec][side] = TM_NA;
	}
	flagPT[0][ssc][sec][side]  =0; //pt1     
	flagPT[1][ssc][sec][side]  =0; //pt2     
	flagPT[2][ssc][sec][side]  =0; //pt3     
	flagPT[3][ssc][sec][side]  =0; //pt4     
	flagPT[4][ssc][sec][side]  =1; //pt5     
	flagPT[5][ssc][sec][side]  =1; //pt6     
	for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	  flagROI[pos][ssc][sec][side]  = 1;
	}
      }
    }
  }
   
  if (!g_TILE_MU) return;
  if (g_USE_CONDDB) return;

  //////////////////////////////
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return;
  }
  MsgStream log(msgSvc, "TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap");

  // read Inner Coincidence Map 
  if (this->readMap()) {
    log << MSG::INFO 
      << " TGC TileMu CW version of " << m_verName << " is selected " << endmsg;
  } else {
    log << MSG::INFO  
	<< " NOT use TileMu " << endmsg;
    g_TILE_MU = false;
    for (size_t side=0; side< N_Side; side++){
      for (size_t sec=0; sec< N_EndcapSector; sec++){
	for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	  flagPT[0][ssc][sec][side]  =0; //pt1     
	  flagPT[1][ssc][sec][side]  =0; //pt2     
	  flagPT[2][ssc][sec][side]  =0; //pt3     
	  flagPT[3][ssc][sec][side]  =0; //pt4     
	  flagPT[4][ssc][sec][side]  =0; //pt5     
	  flagPT[5][ssc][sec][side]  =0; //pt6     
	}
      }    
    }
  }

  // for debug  
  //dumpMap();
}


TGCTileMuCoincidenceMap::~TGCTileMuCoincidenceMap()
{
}

TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap(const TGCTileMuCoincidenceMap& right)
  :m_verName(right.m_verName)
{
  for (size_t side=0; side< N_Side; side++){
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	for (size_t input=0; input< N_Input_TileMuModule; input++){
	  map[input][ssc][sec][side] = right.map[input][ssc][sec][side];
	}
	flagPT[0][ssc][sec][side] = right.flagPT[0][ssc][sec][side];
	flagPT[1][ssc][sec][side] = right.flagPT[1][ssc][sec][side];
	flagPT[2][ssc][sec][side] = right.flagPT[2][ssc][sec][side];
	flagPT[3][ssc][sec][side] = right.flagPT[3][ssc][sec][side];
	flagPT[4][ssc][sec][side] = right.flagPT[4][ssc][sec][side];
	flagPT[5][ssc][sec][side] = right.flagPT[5][ssc][sec][side];
	for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	  flagROI[pos][ssc][sec][side] = right.flagROI[pos][ssc][sec][side];
	}
      }
    }
  }
}

TGCTileMuCoincidenceMap& TGCTileMuCoincidenceMap::operator=(const TGCTileMuCoincidenceMap& right)
{
  if (this != &right) {
    for (size_t side=0; side< N_Side; side++){
      for (size_t sec=0; sec< N_EndcapSector; sec++){
	for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	  for (size_t input=0; input< N_Input_TileMuModule; input++){
	    map[input][ssc][sec][side] = right.map[input][ssc][sec][side];
	  }
	  flagPT[0][ssc][sec][side] = right.flagPT[0][ssc][sec][side];
	  flagPT[1][ssc][sec][side] = right.flagPT[1][ssc][sec][side];
	  flagPT[2][ssc][sec][side] = right.flagPT[2][ssc][sec][side];
	  flagPT[3][ssc][sec][side] = right.flagPT[3][ssc][sec][side];
	  flagPT[4][ssc][sec][side] = right.flagPT[4][ssc][sec][side];
	  flagPT[5][ssc][sec][side] = right.flagPT[5][ssc][sec][side];
	  for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
	    flagROI[pos][ssc][sec][side] = right.flagROI[pos][ssc][sec][side];
	  }
	}
      }
    }
    m_verName = right.m_verName;
  }
  return  *this;
}

bool TGCTileMuCoincidenceMap::readMap() 
{
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "TGCTileMuCoincidenceMap::TGCTileMuCoincidenceMap");

  if (g_USE_CONDDB)  {
    m_verName = m_condDbTool->getVersion(ITGCTriggerDbTool::CW_TILE);
  }

  // select right database according to a set of thresholds
  std::string dbname="";
  dbname = "TileMuCoincidenceMap." + m_verName + "._12.db";

  //----- 
  
  if (g_USE_CONDDB) {
  
  std::string data = m_condDbTool->getData(ITGCTriggerDbTool::CW_TILE, dbname);
  std::istringstream stream(data);

  char delimiter = '\n';
  std::string field;
  std::string tag;

  while (std::getline(stream, field, delimiter)) {
    int sideId = -1;
    int sectorId = -1;
    int sscId    = -1;
    int use[N_PT_THRESH] = {0, 0, 0, 0, 0, 0};
    int roi[N_ROI_IN_SSC] = {1, 1, 1, 1, 1, 1, 1, 1};
    std::istringstream header(field); 
    header >> tag;
    if(tag=="#"){ // read header part.     
      header >> sideId >> sectorId >> sscId 
	     >> use[0] >> use[1] >> use[2] 
	     >> use[3] >> use[4] >> use[5] 
	     >> roi[0] >> roi[1] >> roi[2] >> roi[3]
	     >> roi[4] >> roi[5] >> roi[6] >> roi[7];
    }
    // check Id
    if( sideId<0   || sideId>=N_Side ||
        sectorId<0 || sectorId>=N_EndcapSector ||
	sscId<0    || sscId>=N_Endcap_SSC ) {
      log << MSG::WARNING 
	  << " illegal parameter in database header : " << header.str()
	  << " in file " << dbname
	  << endmsg;
      return false;
    }
    for (size_t pt=0; pt<N_PT_THRESH; pt++){
      flagPT[pt][sscId][sectorId][sideId]   = use[pt];
    }
    for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
      flagROI[pos][sscId][sectorId][sideId] = roi[pos];
    }

    // get trigger word
    std::getline(stream, field, delimiter);
    std::istringstream cont(field);
    unsigned int word;
    for(size_t pos=0; pos<N_Input_TileMuModule; pos++){
      cont >> word;
      map[pos][sscId][sectorId][sideId] = word;
    }
  }

  } else { // will delete...
  
  std::string fullName;
  fullName = PathResolver::find_file( dbname.c_str(), "DATAPATH" );
  bool isFound =( fullName.length() > 0 );
  if( !isFound) {
    log << MSG::WARNING 
	<< " Could not found " << dbname << endmsg;
    return false ;  
  } 

  std::ifstream file(fullName.c_str(),std::ios::in);    

  enum{BufferSize=1024};
  char buf[BufferSize];
  std::string tag;

  while(file.getline(buf,BufferSize)){
    int sideId = -1;
    int sectorId = -1;
    int sscId    = -1;
    int use[N_PT_THRESH] = {0, 0, 0, 0, 0, 0};
    int roi[N_ROI_IN_SSC] = {1, 1, 1, 1, 1, 1, 1, 1};
    std::istringstream header(buf); 
    header >> tag;
    if(tag=="#"){ // read header part.     
      header >> sideId >> sectorId >> sscId 
	     >> use[0] >> use[1] >> use[2] 
	     >> use[3] >> use[4] >> use[5] 
	     >> roi[0] >> roi[1] >> roi[2] >> roi[3]
	     >> roi[4] >> roi[5] >> roi[6] >> roi[7];
    }
    // check Id
    if( sideId<0   || sideId>=N_Side ||
        sectorId<0 || sectorId>=N_EndcapSector ||
	sscId<0    || sscId>=N_Endcap_SSC ) {
      log << MSG::WARNING 
	  << " illegal parameter in database header : " << header.str()
	  << " in file " << dbname
	  << endmsg;
      file.close();
      return false;
    }
    for (size_t pt=0; pt<N_PT_THRESH; pt++){
      flagPT[pt][sscId][sectorId][sideId]   = use[pt];
    }
    for (size_t pos=0; pos< N_ROI_IN_SSC; pos++){
      flagROI[pos][sscId][sectorId][sideId] = roi[pos];
    }

    // get trigger word
    file.getline(buf,BufferSize);
    std::istringstream cont(buf);
    unsigned int word;
    for(size_t pos=0; pos<N_Input_TileMuModule; pos++){
      cont >> word;
      map[pos][sscId][sectorId][sideId] = word;
    }
  }
  file.close();	  
  }
  return true;
}


void TGCTileMuCoincidenceMap::dumpMap() const
{
  // select right database according to a set of thresholds
  std::string fullName="TileMuCoincidenceMap."+m_verName+"._12.out";

  std::ofstream file(fullName.c_str());    

  for (size_t side=0; side< N_Side; side++){
    for (size_t sec=0; sec< N_EndcapSector; sec++){
      for (size_t ssc=0; ssc< N_Endcap_SSC; ssc++){
	file << "# " << side << " " << sec << " " << ssc << " "
	     << flagPT[0][ssc][sec][side] << " "
	     << flagPT[1][ssc][sec][side]  << " "
	     << flagPT[2][ssc][sec][side]  << " "
	     << flagPT[3][ssc][sec][side]  << " "
	     << flagPT[4][ssc][sec][side]  << " "
	     << flagPT[5][ssc][sec][side]  << " "
	     << flagROI[0][ssc][sec][side]  << " "
	     << flagROI[1][ssc][sec][side]  << " "
	     << flagROI[2][ssc][sec][side]  << " "
	     << flagROI[3][ssc][sec][side]  << " "
	     << flagROI[4][ssc][sec][side]  << " "
	     << flagROI[5][ssc][sec][side]  << " "
	     << flagROI[6][ssc][sec][side]  << " "
	     << flagROI[7][ssc][sec][side]  << " "
	     << std::endl;
	file << map[0][ssc][sec][side]  << " "
	     << map[1][ssc][sec][side]  << " "
	     << map[2][ssc][sec][side]  << " "
	     << map[3][ssc][sec][side]  << " "
	     << std::endl;
      }
    }
  }
  file.close();	  
}

} //end of namespace bracket
