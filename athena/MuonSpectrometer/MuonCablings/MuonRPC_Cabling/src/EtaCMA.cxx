/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "RPCcablingInterface/CMAprogram.h"
#include "MuonRPC_Cabling/EtaCMA.h"
#include "MuonRPC_Cabling/SectorLogicSetup.h"

#ifndef LVL1_STANDALONE
#include "PathResolver/PathResolver.h"
#endif

using namespace MuonRPC_Cabling;

EtaCMA::EtaCMA(int num,int stat,int type,CMAcoverage coverage,
              int eta,int phi,int PAD,int Ixx,
	      int pivot_station,int lowPt_station,int highPt_station,       
              int start_ch,int start_st,int stop_ch,int stop_st) :
              CMAparameters(num,stat,type,coverage,eta,phi,PAD,Ixx,
                            pivot_station,lowPt_station,highPt_station,
                            start_ch,start_st,stop_ch,stop_st,Eta) 
{

   m_msgSvc = Athena::getMessageSvc();
   MsgStream log(m_msgSvc, name());

   m_debug = log.level() <= MSG::DEBUG;

   if(m_debug) log<<MSG::DEBUG
   <<"Building EtaCMA for eta/phi/PAD/Ixx/pivot/lpt/hpt/start_ch/start_st/stop_ch/stop_st "
   <<eta<<" "<<phi<<" "<<PAD<<" "<<Ixx<<" "<<pivot_station<<" "<<lowPt_station<<" "<<highPt_station
   <<" "<<start_ch<<" "<<start_st<<" "<<stop_ch<<" "<<stop_st << endmsg;

    // Set the memory for storing the cabling map
    m_pivot_rpc_read  = 1;
    m_lowPt_rpc_read  = 1;
    m_highPt_rpc_read = 1;
    
    m_inversion = false;
    m_conf_type = CMAparameters::Atlas;

    create_pivot_map(m_pivot_rpc_read);
    create_lowPt_map(m_lowPt_rpc_read);
    create_highPt_map( m_highPt_rpc_read);
} 

EtaCMA::EtaCMA(int num,int stat,int type,int eta, int phi,
	      int lowPt_start_co,int lowPt_stop_co,int lowPt_number_co,
              int highPt_start_co,int highPt_stop_co,int highPt_number_co) :
                  CMAparameters(num,stat,type,eta,phi,
                      lowPt_start_co,lowPt_stop_co,lowPt_number_co,
                      highPt_start_co,highPt_stop_co,highPt_number_co)
{
    // Set the memory for storing the cabling map

    m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, name());
    m_debug = log.level() <= MSG::DEBUG;

    m_pivot_rpc_read  = 1;
    m_lowPt_rpc_read  = 1;
    m_highPt_rpc_read = 1;
    
    m_inversion = false;
    m_conf_type = CMAparameters::Atlas;

    create_pivot_map(m_pivot_rpc_read);
    create_lowPt_map(m_lowPt_rpc_read);
    create_highPt_map( m_highPt_rpc_read);
}

EtaCMA::EtaCMA(const EtaCMA& cma) : 
CMAparameters(static_cast<const CMAparameters&>(cma))
{
    m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, name());
    m_debug = log.level() <= MSG::DEBUG;

    m_pivot_RPCs = cma.pivot_RPCs();
    m_lowPt_RPCs = cma.lowPt_RPCs();
    m_highPt_RPCs = cma.highPt_RPCs();
    m_inversion = cma.inversion();
    m_conf_type = CMAparameters::Atlas;
}


EtaCMA::~EtaCMA()
{
    m_pivot_RPCs.clear();
    m_lowPt_RPCs.clear();
    m_highPt_RPCs.clear();
}

EtaCMA&
EtaCMA::operator=(const EtaCMA& cma)
{
    if(this!=&cma){
      static_cast<CMAparameters&>(*this)=static_cast<const CMAparameters&>(cma);
      m_pivot_RPCs.clear();
      m_pivot_RPCs = cma.pivot_RPCs();
      m_lowPt_RPCs.clear();
      m_lowPt_RPCs = cma.lowPt_RPCs();
      m_highPt_RPCs.clear();
      m_highPt_RPCs = cma.highPt_RPCs();
      
      m_inversion = cma.inversion();
    }
    return *this;
}

bool
EtaCMA::cable_CMA_channels(HalfType side)
{
    if(pivot_station())  //Check and connect strips with Pivot matrix channels
    {
        for(int i=pivot_start_ch();i<=pivot_stop_ch();++i)
        {
            int strip_number = (m_active_pivot_chs)? 1 : pivot_start_st();
            RPClink::iterator found = m_pivot_RPCs.find(i);
            RPCchamber* rpc = (*found).second;
            int final_strip = rpc->eta_strips();
            if ( i == pivot_stop_ch() ) final_strip = pivot_stop_st();
            do
            {
	        if(m_active_pivot_chs == pivot_channels) 
                {
                    noMoreChannels("Pivot");
                    return false;
	        }
		
		int st_effective = strip_number - 1; 
		if(!i && side == Negative) st_effective = 
		                           abs(st_effective-final_strip) -1;
		
		if(rpc->ijk_etaReadout() == 1)
		{
                m_pivot[0][0][m_active_pivot_chs]=i*100 + st_effective;
                m_pivot[0][1][m_active_pivot_chs]=10000 + i*100 + st_effective;
		}else
		{
		m_pivot[0][1][m_active_pivot_chs]=i*100 + st_effective;
                m_pivot[0][0][m_active_pivot_chs]=10000 + i*100 + st_effective;
		}
                rpc->add_eta_channel(st_effective);
	        ++m_active_pivot_chs;
            }while(++strip_number <= final_strip);
        }
	// Set first and last connectors code
	int code = m_pivot_station*100000 + 2*100000000;
	m_first_pivot_code = code + m_pivot[0][0][0];
	m_last_pivot_code  = code + m_pivot[0][0][m_active_pivot_chs-1];
    }
    if(lowPt_station() && lowPt_number_co() != -1)
    {
	for(int i=lowPt_start_ch();i<=lowPt_stop_ch();++i)
	{
            int strip_number = (m_active_lowPt_chs)? 1 : lowPt_start_st();
            RPClink::iterator found = m_lowPt_RPCs.find(i);
            RPCchamber* rpc = (*found).second;
            int final_strip = rpc->eta_strips();
            if ( i == lowPt_stop_ch() ) final_strip = lowPt_stop_st();
            do
            {   
		if(m_active_lowPt_chs == confirm_channels) 
                {
                    noMoreChannels("Low Pt");
                    return false;
	        }
		
		int st_effective = strip_number - 1; 
		if(!i && side == Negative) st_effective = 
		                           abs(st_effective-final_strip) -1;
		
		if(rpc->ijk_etaReadout() == 1)
		{
                m_lowPt[0][0][m_active_lowPt_chs]=i*100 + st_effective;
                m_lowPt[0][1][m_active_lowPt_chs]=10000 + i*100 + st_effective;
		}else
		{
		m_lowPt[0][1][m_active_lowPt_chs]=i*100 + st_effective;
                m_lowPt[0][0][m_active_lowPt_chs]=10000 + i*100 + st_effective;
		}
                rpc->add_eta_channel(st_effective);
	        ++m_active_lowPt_chs;
            }while(++strip_number <= final_strip);
	}
	// Set first and last connectors code	
	int code = m_lowPt_station*100000 + 2*100000000;
	m_first_lowPt_code = code + m_lowPt[0][0][0];
	m_last_lowPt_code  = code + m_lowPt[0][0][m_active_lowPt_chs-1];
    }
    if(highPt_station() && highPt_number_co() != -1)
    {
	for(int i=highPt_start_ch();i<=highPt_stop_ch();++i)
	{
            int strip_number = (m_active_highPt_chs)? 1 : highPt_start_st();
            RPClink::iterator found = m_highPt_RPCs.find(i);
            RPCchamber* rpc = (*found).second;
            int final_strip = rpc->eta_strips();
            if ( i == highPt_stop_ch() ) final_strip = highPt_stop_st();
            do
            {
	        if(m_active_highPt_chs == confirm_channels) 
                {
                    noMoreChannels("High Pt");
                    return false;
	        }
		
		int st_effective = strip_number - 1; 
		if(!i && side == Negative) st_effective = 
		                           abs(st_effective-final_strip) -1;
		
		if(rpc->ijk_etaReadout() == 1)
		{
                m_highPt[0][0][m_active_highPt_chs]=i*100 + st_effective;
                m_highPt[0][1][m_active_highPt_chs]=10000+i*100 + st_effective;
		} else
		{
		m_highPt[0][1][m_active_highPt_chs]=i*100 + st_effective;
                m_highPt[0][0][m_active_highPt_chs]=10000+i*100 + st_effective;
		}
                rpc->add_eta_channel(st_effective);
	        ++m_active_highPt_chs;
            }while(++strip_number <= final_strip);
	}
	// Set first and last connectors code
	int code = m_highPt_station*100000 + 2*100000000;
	m_first_highPt_code = code + m_highPt[0][0][0];
	m_last_highPt_code  = code + m_highPt[0][0][m_active_highPt_chs-1];	
    }
    return true;
}


bool
EtaCMA::connect(SectorLogicSetup& setup)
{
    if(pivot_station())  //Check and connect Pivot chambers
    {
        for(int i=pivot_start_ch();i<=pivot_stop_ch();++i)
        {   
            RPCchamber* rpc = setup.find_chamber(m_pivot_station,i);
            if(rpc)
	    {
	        rpc->add_cma(this);
	        m_pivot_RPCs.insert(RPClink::value_type(i,rpc));
	    }
            else 
            {
                this->no_connection_error("RPC",i);
                return false;
            }
        }
    }

    if(lowPt_station() && lowPt_number_co() != -1)
    {   // Check and connect Low Pt chambers
        if(got_confirm_cabling(setup,lowPt_station()))
	{
	    ++m_lowPt_start_st;   // RPC strips starts from 1!
            for(int i = m_lowPt_start_ch; i <= m_lowPt_stop_ch;++i)
            {   
                RPCchamber* rpc = setup.find_chamber(lowPt_station(),i);
                if(rpc)
	        {
	            rpc->add_cma(this);
	            m_lowPt_RPCs.insert(RPClink::value_type(i,rpc));
	        }
                else 
                {
                    this->no_connection_error("RPC",i);
                    return false;
                }
                if(i == m_lowPt_stop_ch) m_lowPt_stop_st += 
                                                     rpc->strips_in_Eta_Conn();
            }
	}
        else
	{
            no_confirm_error(lowPt_station());
            return false;
	}
    }
   
    if(highPt_station() && highPt_number_co() != -1)
    {   // Check and connect High Pt chambers
        if(got_confirm_cabling(setup,highPt_station()))
	{
	    ++m_highPt_start_st;   // RPC strips starts from 1!
            for(int i = m_highPt_start_ch;i <= m_highPt_stop_ch;++i)
            {   
                RPCchamber* rpc = setup.find_chamber(highPt_station(),i);
                if(rpc)
	        {
	            rpc->add_cma(this);
	            m_highPt_RPCs.insert(RPClink::value_type(i,rpc));
	        }
                else 
                {
                    this->no_connection_error("RPC",i);
                    return false;
                }
                if(i == m_highPt_stop_ch) m_highPt_stop_st += 
                                                     rpc->strips_in_Eta_Conn();
            }
        }
	else
	{
            no_confirm_error(highPt_station());
            return false;
	}
    }
    return true;
}

bool
EtaCMA::got_confirm_cabling(SectorLogicSetup& setup,int stat)
{
  int l;
  if(stat == lowPt_station())
  {
      bool result = setup.local_conn_add
          (Eta,stat,lowPt_start_co(),l,m_lowPt_start_ch,m_lowPt_start_st) &&
             setup.local_conn_add
	  (Eta,stat,lowPt_stop_co(),l,m_lowPt_stop_ch,m_lowPt_stop_st);
      if ((!m_lowPt_start_ch || !m_lowPt_stop_ch) && setup.side() == NoHalf)
      {
           DISP << "Sector Type must belong to a specific side when"
	        << " RPC chamber is in between eta 0" << std::endl;
	   DISP_ERROR;
	   return false;
      }
      return result;
  }
  else if (stat == highPt_station())
  {
      bool result = setup.local_conn_add
          (Eta,stat,highPt_start_co(),l,m_highPt_start_ch,m_highPt_start_st) &&
             setup.local_conn_add
	  (Eta,stat,highPt_stop_co(),l,m_highPt_stop_ch,m_highPt_stop_st);
      if ( (!m_highPt_start_ch || !m_highPt_stop_ch)&& setup.side() == NoHalf)
      {
           DISP << "Sector Type must belong to a specific side when"
	        << " RPC chamber is in between eta 0" << std::endl;
	   DISP_ERROR;
	   return false;
      }
      return result;
  
  
  }
  else 
  {
      DISP << "Station n. " << stat 
           << " don't give input to CMA confirm planes!" << std::endl;
      DISP_ERROR;
      return false;
  }
}

bool
EtaCMA::setup(SectorLogicSetup& setup)
{
    m_msgSvc = Athena::getMessageSvc();    
    MsgStream log(m_msgSvc, name());

    m_debug = log.level() <= MSG::DEBUG;
    // Connect the CMA with RPC chambers
    if( !this->connect(setup) ) return false;

    // Check boundary of CMA channels
    EtaCMA* prev = setup.previousCMA(*this);
    if(prev && pivot_station())
    {
        if (this->pivot_start_ch() == prev->pivot_stop_ch())
	{
	    if(!(this->pivot_start_st() == prev->pivot_stop_st() + 1))
	    {
                this->two_obj_error_message("strips mismatch",prev);
                return false;
	    }
	}
        else if (!(this->pivot_start_ch() == prev->pivot_stop_ch() + 1))
	{
            this->two_obj_error_message("chambers mismatch",prev);
	    return false;
	}
        else
	{
	    if(!prev->end_at_RPC_Z_boundary()|| !begin_at_RPC_Z_boundary())
	    {
               this->two_obj_error_message("boundary mismatch",prev);
	       return false;
	    }
        }
    }

    // Build the cabling map
    if( !this->cable_CMA_channels(setup.side()) ) return false;
    
    // invert the strip cabling if needed
    if( !this->doInversion(setup) ) return false;
    
    // only 1 repository allowed so far
    std::string LVL1_configuration_repository;
    LVL1_configuration_repository = "ATLAS.data";
    
    // Read the program file if exist
    SectorLogicSetup::SECTORlist sectors = setup.sectors();
    SectorLogicSetup::SECTORlist::const_iterator it = sectors.begin();

    char s_tag[4];
    sprintf(s_tag,"s%02d",*it);
    
    char t_tag[3];
    sprintf(t_tag,"t%1d",id().PAD_index());
    
    char c_tag[4] = {'_','c','2','\0'};
    //GRAZ>    if(id().eta_index()==1) c_tag[2] = '3';
    if(id().Ixx_index()==1) c_tag[2]='3';


    std::ifstream CMAprogLow;
    std::istringstream CMAprogLow_COOL;
    char name[200];

    //LB retrieve pointer to the map of the trigger roads 
    const std::map<std::string, std::string>* p_trigroads=setup.GetPtoTrigRoads();
    
    // Read trigger configurations from files


    if(p_trigroads==0){ 
      while (!CMAprogLow.is_open() && it != sectors.end())
      {
        __osstream namestr;
#ifdef LVL1_STANDALONE
        namestr << "./" << LVL1_configuration_repository 
	        << "/" << s_tag 
		<< "/" << s_tag << "_" << t_tag << "_pl"
		<< "/" << s_tag << "_" << t_tag << "_pl" << c_tag
		<< "/" << s_tag << "_" << t_tag << "_pl" << c_tag << ".txt" 
		<< std::ends;
#else
        std::string dir;
        dir = setup.online_database();
        namestr<<dir<< "/" << s_tag << "_" << t_tag << "_pl" << c_tag << ".txt"
               << std::ends;  // M.C. search for local files 

#endif
        
	namestr.str().copy(name,namestr.str().length(),0);
	name[namestr.str().length()] = 0;
        if(m_debug) log<<MSG::DEBUG
	<<"filename for the trigger roads "<<name<<endmsg;            

	CMAprogLow.open(name);
        ++it;
	// namestr.freeze(0);
        namestr.clear();
      }
    }
    // Trigger configuration loaded from COOL
    else {
      while (CMAprogLow_COOL.str().empty() && it != sectors.end())
      {
        __osstream namestr;
        namestr<< s_tag << "_" << t_tag << "_pl" << c_tag << ".txt"
               << std::ends;  
        namestr.str().copy(name,namestr.str().length(),0);
        name[namestr.str().length()] = 0;            
        std::map<std::string, std::string>::const_iterator itc;
        itc=p_trigroads->find(name);
        if(itc != p_trigroads->end()){
          if(m_debug){
            log<<MSG::DEBUG
	    << "EtaCMA low: key " << name << "found in the Trigger Road Map --> OK" << endmsg;
            log<<MSG::DEBUG
	    << "EtaCMA low: key " <<  itc->second.c_str()<<endmsg;
	  }
          CMAprogLow_COOL.str(itc->second.c_str());
          if(m_debug) log<<MSG::DEBUG
		      << "Etacma:CMAPROGLOW " << CMAprogLow_COOL.str()<<endmsg;
        }
        ++it;
        namestr.clear();
      }
    }     
    if (CMAprogLow.is_open()) 
    {
      CMAprogram* program = new CMAprogram(CMAprogLow,true);
        if(program->check()) {
	    m_lowPt_program = program;
            if(setup.cosmic()) {
	        //m_lowPt_program->copy_over(0,2);
	        m_lowPt_program->open_threshold(0);
	        m_lowPt_program->open_threshold(1);
	    }
	    for(unsigned int i=0;i<3;++i)
	    {
	        if(!m_lowPt_program->hasProgrammed(i))
		{
                    if(m_debug) log<<MSG::DEBUG
		    << s_tag << ": " << id() << ": low-pt: has threshold "
		    << i << " not programmed."<<endmsg;
		}
            }
	} else delete program;
	CMAprogLow.close();
        if(m_debug) log<<MSG::DEBUG
	<<"EtaCMA::setup low_pt program has been read ---- "<<endmsg;
    } 
    else if(!CMAprogLow_COOL.str().empty()){
      CMAprogram* program = new CMAprogram(CMAprogLow_COOL,true);
        if(program->check()) {
	    m_lowPt_program = program;
            if(setup.cosmic()) {
	        //m_lowPt_program->copy_over(0,2);
	        m_lowPt_program->open_threshold(0);
	        m_lowPt_program->open_threshold(1);
	    }
	    for(unsigned int i=0;i<3;++i)
	    {
	        if(!m_lowPt_program->hasProgrammed(i))
		{
                    if(m_debug) log<<MSG::DEBUG
		    << s_tag << ": " << id() << ": low-pt: has threshold "
		    << i << " not programmed."<<endmsg;
		}
            }
	} else delete program;
	CMAprogLow_COOL.str("");
    }
    else 
    {

        if(m_debug) log<<MSG::DEBUG
	<< name << " not found! Putting a dummy configuration"<<endmsg;
	m_lowPt_program = new CMAprogram();
	m_lowPt_program->open_threshold(0);
    }


    it = sectors.begin();
    std::ifstream CMAprogHigh;
    std::istringstream CMAprogHigh_COOL;

    if(p_trigroads==0){
      while (!CMAprogHigh.is_open() && it != sectors.end())
      {
        __osstream namestr;
#ifdef LVL1_STANDALONE
        namestr << "./" 
	        << "/" << s_tag 
		<< "/" << s_tag << "_" << t_tag << "_ph"
		<< "/" << s_tag << "_" << t_tag << "_ph" << c_tag
		<< "/" << s_tag << "_" << t_tag << "_ph" << c_tag << ".txt"
		<< std::ends;
#else
        std::string dir;
        dir = setup.online_database();
        namestr<<dir<< "/" << s_tag << "_" << t_tag << "_ph" << c_tag << ".txt"
               << std::ends;
#endif
        
	namestr.str().copy(name,namestr.str().length(),0);
	name[namestr.str().length()] = 0;
        if(m_debug) log<<MSG::DEBUG
		    <<"filename for the trigger roads "<<name<<endmsg;

        CMAprogHigh.open(name);
        ++it;
	// namestr.freeze(0);
        namestr.clear();
      }   
    }
    // Trigger configuration loaded from COOL
    else {
      while (CMAprogHigh_COOL.str().empty() && it != sectors.end())
      {
        __osstream namestr;
        namestr<< s_tag << "_" << t_tag << "_ph" << c_tag << ".txt"
               << std::ends;  
        namestr.str().copy(name,namestr.str().length(),0);
        name[namestr.str().length()] = 0;            
        std::map<std::string, std::string>::const_iterator itc;
        itc=p_trigroads->find(name);
        if(itc != p_trigroads->end()){
          if(m_debug) { 
            log<<MSG::DEBUG
	    << "EtaCMA high: key " << name << "found in the Trigger Road Map --> OK"<<endmsg;
	    log<<MSG::DEBUG
	    << "EtaCMA high: key " <<  itc->second.c_str()<<endmsg;
	  }
          CMAprogHigh_COOL.str(itc->second.c_str());
          if(m_debug) log<<MSG::DEBUG
	  << "EtaCMA:CMAPROGHIGH " << CMAprogHigh_COOL.str()<<endmsg;
        }
        ++it;
        namestr.clear();
      }
    }     

    if (CMAprogHigh.is_open()) 
    {

      CMAprogram* program = new CMAprogram(CMAprogHigh,true);
        if(program->check()) {
	    m_highPt_program = program;
	    if(setup.cosmic()) {
	        //m_highPt_program->copy_over(0,2);
	        m_highPt_program->open_threshold(0);
	        m_highPt_program->open_threshold(1);
	    }
	    for(unsigned int i=0;i<3;++i)
	    {
	        if(!m_highPt_program->hasProgrammed(i))
		{

                    if(m_debug) log<<MSG::DEBUG
		    << s_tag << ": " << id() << ": high-pt: has threshold "
		    << i << " not programmed."<<endmsg;
                }
	    }
	}
	else delete program;
	CMAprogHigh.close();
        if(m_debug) log<<MSG::DEBUG
		       <<"EtaCMA::setup high_pt program has been read ---- "<<endmsg;
    }
    else if(!CMAprogHigh_COOL.str().empty()){
      CMAprogram* program = new CMAprogram(CMAprogHigh_COOL,true);
        if(program->check()) {
	    m_highPt_program = program;
            if(setup.cosmic()) {
	        //m_highPt_program->copy_over(0,2);
	        m_highPt_program->open_threshold(0);
	        m_highPt_program->open_threshold(1);
	    }
	    for(unsigned int i=0;i<3;++i)
	    {
	        if(!m_highPt_program->hasProgrammed(i))
		{
                    if(m_debug) log<<MSG::DEBUG
		    << s_tag << ": " << id() << ": high-pt: has threshold "
		    << i << " not programmed."<<endmsg;
		}
            }
	} else delete program;
	CMAprogHigh_COOL.str("");
    }
    else 
    {
        if(m_debug) log<<MSG::DEBUG
		    << name << " not found! Putting a dummy configuration"<<endmsg;
	m_highPt_program = new CMAprogram();
	m_highPt_program->open_threshold(0);
    }
    
    return true;
}

bool
EtaCMA::doInversion(SectorLogicSetup& setup)
{
    SectorLogicSetup::SECTORlist Sectors = setup.sectors();
    SectorLogicSetup::SECTORlist::const_iterator it = Sectors.begin();
    int sector = *it;
    
    RPClink::const_iterator rpc;
    //std::cout << "SETTORE TIPO:" << setup.sector_type() << std::endl << *this << std::endl;
    if(lowPt_station() && lowPt_number_co() != -1)
    {  
        rpc = m_lowPt_RPCs.begin();
	if( (*rpc).second->inversion(sector) )
	{
	    //std::cout << "inverto il low: " << (*rpc).second->extendedName(sector) << std::endl;
	    m_inversion = true;
	    correct(Inversion,LowPt,0,first_lowPt_channel(),last_lowPt_channel(),0);
            correct(Inversion,LowPt,1,first_lowPt_channel(),last_lowPt_channel(),0);
	    int tmp = m_first_lowPt_code;
	    m_first_lowPt_code = m_last_lowPt_code;
	    m_last_lowPt_code = tmp;
	}
    
    }
    
    if(pivot_station())
    {
        rpc = m_pivot_RPCs.begin();
	if( (*rpc).second->inversion(sector) )
	{
	    //std::cout << "inverto il pivot: " << (*rpc).second->extendedName(sector) <<std::endl;
	    m_inversion = true;
	    correct(Inversion,Pivot,0,first_pivot_channel(),last_pivot_channel(),0);
            correct(Inversion,Pivot,1,first_pivot_channel(),last_pivot_channel(),0);
	    int tmp = m_first_pivot_code;
	    m_first_pivot_code = m_last_pivot_code;
	    m_last_pivot_code = tmp;
	}
    }
    
    if(highPt_station() && highPt_number_co() != -1)
    {
        rpc = m_highPt_RPCs.begin();
	if( (*rpc).second->inversion(sector) )
	{
	    //std::cout << "inverto l'high: " <<
	    //(*rpc).second->extendedName(sector) << std::endl;
	    m_inversion = true;
	    correct(Inversion,HighPt,0,first_highPt_channel(),last_highPt_channel(),0);
            correct(Inversion,HighPt,1,first_highPt_channel(),last_highPt_channel(),0);
	    int tmp = m_first_highPt_code;
	    m_first_highPt_code = m_last_highPt_code;
	    m_last_highPt_code = tmp; 
	}    
    }
    
    if(m_inversion) m_id->inversion();
    
    
    return true;
}

bool
EtaCMA::end_at_RPC_Z_boundary(void) const
{
    RPClink::const_iterator found = m_pivot_RPCs.find(pivot_stop_ch());
    if((*found).second->eta_strips() == pivot_stop_st()) return true;
    return false;
}

bool
EtaCMA::begin_at_RPC_Z_boundary(void) const
{
    if(pivot_start_st() == 1) return true;
    return false;
}
