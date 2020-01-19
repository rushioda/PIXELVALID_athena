/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <fstream>
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "RPCcablingInterface/CMAprogram.h"

#include "MuonRPC_Cabling/OddPhiCMA.h"
#include "MuonRPC_Cabling/SectorLogicSetup.h"

#ifndef LVL1_STANDALONE
#include "PathResolver/PathResolver.h"
#endif

using namespace MuonRPC_Cabling;

OddPhiCMA::OddPhiCMA(int num,int stat,int type,CMAcoverage coverage,
                       int eta,int phi,int PAD,int Ixx,
	               int pivot_station,int lowPt_station,int highPt_station,
                       int start_ch,int start_st,int stop_ch,int stop_st) :
                       CMAparameters(num,stat,type,coverage,eta,phi,PAD,Ixx,
                                    pivot_station,lowPt_station,highPt_station,
                                    start_ch,start_st,stop_ch,stop_st,Phi) 
{ 
    m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, name());
    m_debug = log.level() <= MSG::DEBUG;

    m_inversion = false;
    m_conf_type = CMAparameters::Atlas;
    
    // Set the memory for storing the cabling data
    if (!m_pivot_station) {m_pivot_rpc_read  = 1; create_pivot_map(1);}
    if (!m_lowPt_station) {m_lowPt_rpc_read  = 1; create_lowPt_map(1);}
    if (!m_highPt_station) {m_highPt_rpc_read  = 1; create_highPt_map(1);} 
}
 
OddPhiCMA::OddPhiCMA(const OddPhiCMA& cma) : 
CMAparameters(static_cast<const CMAparameters&>(cma))
{
    m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, name());
    m_debug = log.level() <= MSG::DEBUG;

    m_pivot_WORs = cma.pivot_WORs();
    m_lowPt_WORs = cma.lowPt_WORs();
    m_highPt_WORs = cma.highPt_WORs();
    m_inversion = cma.inversion();
    m_conf_type = CMAparameters::Atlas;
}


OddPhiCMA::~OddPhiCMA()
{
    m_pivot_WORs.clear();
    m_lowPt_WORs.clear();
    m_highPt_WORs.clear();
}

OddPhiCMA&
OddPhiCMA::operator=(const OddPhiCMA& cma)
{
    if(this!=&cma){
      static_cast<CMAparameters&>(*this)=static_cast<const CMAparameters&>(cma);
      m_pivot_WORs.clear();
      m_pivot_WORs = cma.pivot_WORs();
      m_lowPt_WORs.clear();
      m_lowPt_WORs = cma.lowPt_WORs();
      m_highPt_WORs.clear();
      m_highPt_WORs = cma.highPt_WORs();
      
      m_inversion = cma.inversion();
    }
    return *this;
}

bool
OddPhiCMA::cable_CMA_channels(void)
{
    if(pivot_station())  //Check and connect strips with Pivot matrix channels
    {
        WORlink::iterator found = m_pivot_WORs.find(this->pivot_start_ch());
        WiredOR* wor = (*found).second;

        m_pivot_rpc_read = wor->RPCacquired();
        create_pivot_map(m_pivot_rpc_read);

        int start   = pivot_start_st();
        int stop    = pivot_stop_st();
        int max_st  = wor->give_max_phi_strips();

        int first_ch_cabled = 32;
	int last_ch_cabled = -1;
	
	std::vector <int> multiplicity(max_st);

        for(int i = 0; i < m_pivot_rpc_read; ++i)
        { 
            const RPCchamber* rpc = wor->connected_rpc(i);
            int rpc_st = rpc->phi_strips();
            int cham = rpc->number();
            int local_strip = max_st - (max_st - start);
            int final_strip = max_st - (max_st - stop );
	    
            int chs = (this->id().Ixx_index() == 0) ?
	                       pivot_channels - abs(stop - start) -1 : 0;
            if(chs >= pivot_channels)
            {
                noMoreChannels("Pivot");
                return false;
            }
            if (chs <= first_ch_cabled) first_ch_cabled = chs;
            
	    do
            {
	        if(chs == pivot_channels)
                {
                    noMoreChannels("Pivot");
		    return false;
	        }
                if(local_strip > 0 && local_strip <= rpc_st)
	        {
		    if(rpc->ijk_phiReadout() == 1)
		    {
	            m_pivot[i][0][chs] = cham*100 + local_strip - 1;
	            m_pivot[i][1][chs] = 10000 + cham*100 + local_strip - 1;
		    } else
		    {
		    m_pivot[i][1][chs] = cham*100 + local_strip - 1;
	            m_pivot[i][0][chs] = 10000 + cham*100 + local_strip - 1;
		    }
                    multiplicity[local_strip - 1 ] = 1;
	        }
	        ++chs;
            }while(--local_strip >= final_strip);
            
	    if (chs-1 >= last_ch_cabled) last_ch_cabled = chs-1;
            if(chs > m_active_pivot_chs) m_active_pivot_chs = chs;
        }
        wor->add_odd_read_mul(multiplicity);
	
	// Set first and last connectors code
	int code = m_pivot_station*100000 + 1*100000000;
	int ch = 0;

	// first_ch_cabled and last_ch_cabled are initialized with "out-of-bound" values
	// of the m_pivot array; proper values should be assigned during the loop;
	// the init values though are "misused" for certain conditions, too, therefore
	// they cannot be changed; but the following if should NEVER fire.
	if( first_ch_cabled >= pivot_channels || last_ch_cabled < 0 ) {
	  std::cout << "MuonRPC_Cabling: OddPhiCMA::cable_CMA_channels - out of bound array indices!" <<std::endl;
	  std::cout << "\t\tValues:" << first_ch_cabled << ", " << last_ch_cabled << " . Taking emergency exit!"<< std::endl;
	  throw;
	}

	// get LowPhi code
	for (ch=0; ch < m_pivot_rpc_read;++ch) 
	    if(m_pivot[ch][0][last_ch_cabled] >= 0) break;
	if(ch == m_pivot_rpc_read ) --ch;
	m_first_pivot_code  = code + m_pivot[ch][0][last_ch_cabled];
	
	// get HighPhi code
	for (ch=0; ch < m_pivot_rpc_read;++ch) 
	    if(m_pivot[ch][0][first_ch_cabled] >= 0) break;
	if(ch == m_pivot_rpc_read ) --ch;
	m_last_pivot_code  = code + m_pivot[ch][0][first_ch_cabled];
	
	/*
	//get LowPhi code
	int wor = 0;
	for(ch=31;ch >=0; --ch) 
	{
	    if(m_pivot[ch][0][0] >= 0) break;
	    
	}
        if(ch == m_pivot_rpc_read ) --ch;
	m_first_pivot_code = code + m_pivot[ch][0][0];
	
	//get HighPhi code
	for(ch=0;ch < m_pivot_rpc_read; ++ch)
	    if(m_pivot[ch][0][m_active_pivot_chs-1] >= 0) break;
        if(ch == m_pivot_rpc_read ) --ch;
	m_last_pivot_code  = code + m_pivot[ch][0][m_active_pivot_chs-1];
	*/	
    }
   
    if(lowPt_station() && lowPt_start_ch() != -1)
    {   //Check and connect strips with Low Pt matrix channels
        for (int i = lowPt_start_ch(); i <= lowPt_stop_ch(); ++i)
	{
            WORlink::iterator found = m_lowPt_WORs.find(i);
            m_lowPt_rpc_read += (*found).second->RPCacquired();
	}

        create_lowPt_map(m_lowPt_rpc_read);

        int start   = lowPt_start_st();
        int stop    = lowPt_stop_st();
        int max_st  = get_max_strip_readout(lowPt_station());
        
	int first_ch_cabled = 64;
	int last_ch_cabled = -1;
        
	int r = 0; 
        
        for (int w = lowPt_start_ch(); w <= lowPt_stop_ch(); ++w)
	{
            WORlink::iterator found = m_lowPt_WORs.find(w);
            WiredOR* wor = (*found).second;  
	    std::vector <int> multiplicity(wor->give_max_phi_strips());
            multiplicity.clear();

            for(int i = 0; i < wor->RPCacquired(); ++i)
            { 
                const RPCchamber* rpc = wor->connected_rpc(i);

                int rpc_st = rpc->phi_strips();
                int cham   = rpc->number();
                int local_strip= max_st - (max_st - start);
                int final_strip= max_st - (max_st - stop );
		
		int chs = (this->id().Ixx_index() == 0) ? 40 - max_st/2 : 0;
		
		char E = rpc->chamber_name()[2];
		//E='A';
		
		if( E=='E' && this->id().Ixx_index() == 1){
		  //std::cout<<" attenzione - Odd Phi CM in BME Ixx_index==1 -> resetting chs (now ="<<chs<<") and final_strip (now ="<<final_strip<<"), local_strip="<<local_strip<<std::endl;
		  //ok for sector 23 (side C [and side A], cm1, ijk=2 and 3) 
		 chs=8;
		 local_strip=32;
		 final_strip=1;
		 
		 //std::cout<<" attenzione ODD-                                    reset chs (now ="<<chs<<") and final_strip (now ="<<final_strip<<"), local_strip="<<local_strip<<std::endl;
		}
		if( E=='E' &&  this->id().Ixx_index() == 0){
		  //std::cout<<" attenzione - Odd Phi CM in BME Ixx_index==0 -> resetting chs (now ="<<chs<<") and final_strip (now ="<<final_strip<<"), local_strip="<<local_strip<<std::endl;
		  //ok for sector 23 (side C [and side A], cm0, ijk=2 and 3) 		  
		  chs=24;
		  local_strip=32;
		  final_strip=1;
		  //std::cout<<" attenzione ODD-                                    reset chs (now ="<<chs<<") and final_strip (now ="<<final_strip<<"), local_strip="<<local_strip<<std::endl;
		}
		
                if (chs <= first_ch_cabled) first_ch_cabled = chs;
                do
                {
	            if(chs == confirm_channels)
                    {
                        noMoreChannels("Low Pt");
		        return false;
	            }
                    if(local_strip > 0 && local_strip <= rpc_st)
	            {
		        if(rpc->ijk_phiReadout() == 1)
			{
	                m_lowPt[r][0][chs]= cham*100 + local_strip - 1;
	                m_lowPt[r][1][chs]= 10000 + cham*100 + local_strip - 1;
			} else
			{
			m_lowPt[r][1][chs]= cham*100 + local_strip - 1;
	                m_lowPt[r][0][chs]= 10000 + cham*100 + local_strip - 1;
			}
			if(max_st > wor->give_max_phi_strips())
			{
			  multiplicity[local_strip - 1] = 1;
			} else {
                          multiplicity[local_strip - 1 + (max_st - rpc_st)] = 1;
			}
	            }
	            ++chs;
                }while(--local_strip >= final_strip);

                ++r;
		if (chs-1 >= last_ch_cabled) last_ch_cabled = chs-1;
                if(chs > m_active_lowPt_chs) m_active_lowPt_chs = chs;

            }
            wor->add_odd_read_mul(multiplicity);
	    
	    // Set first and last connectors code
	    int code = m_lowPt_station*100000 + 1*100000000;
	    int ch = 0;

	    if(last_ch_cabled < 0) {
	      std::cout << "MuonRPC_Cabling: OddPhiCMA - neg. array idx - taking emergency exit!." <<std::endl;
	      throw;
            }
	
	    for(ch=0;ch < m_lowPt_rpc_read; ++ch) 
	        if(m_lowPt[ch][0][last_ch_cabled] >= 0) break;
	    if(ch == m_lowPt_rpc_read) --ch;
	    m_first_lowPt_code = code + m_lowPt[ch][0][last_ch_cabled];
	
	    for(ch=0;ch < m_lowPt_rpc_read; ++ch)
	        if(m_lowPt[ch][0][first_ch_cabled] >= 0) break;
            if(ch == m_lowPt_rpc_read) --ch;
	    m_last_lowPt_code = code + m_lowPt[ch][0][first_ch_cabled];	    
        }
    }

    if(highPt_station() && highPt_start_ch() != -1)
    {   //Check and connect strips with High Pt matrix channels
        for (int i = highPt_start_ch(); i <= highPt_stop_ch(); ++i)
	{
            WORlink::iterator found = m_highPt_WORs.find(i);
            m_highPt_rpc_read += (*found).second->RPCacquired();
	}

        create_highPt_map(m_highPt_rpc_read);

        int start   = highPt_start_st();
        int stop    = highPt_stop_st();
        int max_st  = get_max_strip_readout(highPt_station());
        
	int first_ch_cabled = 64;
	int last_ch_cabled = -1;
        
	int r = 0; 

        for (int w = highPt_start_ch(); w <= highPt_stop_ch(); ++w)
	{
            WORlink::iterator found = m_highPt_WORs.find(w);
            WiredOR* wor = (*found).second;  
	    std::vector <int> multiplicity(wor->give_max_phi_strips());
            multiplicity.clear();

            for(int i = 0; i < wor->RPCacquired(); ++i)
            { 
                const RPCchamber* rpc = wor->connected_rpc(i);

                int rpc_st = rpc->phi_strips();
                int cham   = rpc->number();
                int local_strip= max_st - (max_st - start);
                int final_strip= max_st - (max_st - stop );
                
		int chs = (this->id().Ixx_index() == 0) ? 40 - max_st/2 : 0;
		
		char L = rpc->chamber_name()[2];
		int  sEta = rpc->stationEta();	
		
		bool isBOE = false;
		if (abs(sEta)==8 && L=='L' ) isBOE=true;
		if(isBOE && this->id().Ixx_index() == 1){
		  chs=0;
		  local_strip=52;
		  final_strip=1;
		}
		
		if(isBOE && this->id().Ixx_index() == 0){
		  chs=4;
		  final_strip=13;
		  local_strip=64;
		}

                if (chs <= first_ch_cabled) first_ch_cabled = chs;
		do
                {
	            if(chs == confirm_channels)
                    {
                        noMoreChannels("High Pt");
		        return false;
	            }
                    

		    bool skipChannel=false;
		    if (isBOE && this->id().Ixx_index() == 1 && 
			( (chs>3 && chs<8) || (chs>39 && chs<44) ) ) skipChannel=true;
		    if (isBOE && this->id().Ixx_index() == 0 && 
			( (chs>19 && chs<24) || (chs>55 && chs<60) ) ) skipChannel=true;
		    if (skipChannel)
		      {++local_strip;}
		    else 
		      {
			if(local_strip > 0 && local_strip <= rpc_st)
			  {
			    if(rpc->ijk_phiReadout() == 1)
			      {
				m_highPt[r][0][chs]=cham*100 + local_strip - 1;
				m_highPt[r][1][chs]=10000 + cham*100 + local_strip - 1;
			      } else
			      {
				m_highPt[r][1][chs]=cham*100 + local_strip - 1;
				m_highPt[r][0][chs]=10000 + cham*100 + local_strip - 1;
			      }
			    if(max_st > wor->give_max_phi_strips())
			      {
				multiplicity[local_strip - 1] = 1;
			      } else {
			      multiplicity[local_strip - 1 + (max_st - rpc_st)] = 1;
			    }
			  }
		      }

	            ++chs;
                }while(--local_strip >= final_strip);

                ++r;
		if (chs-1 >= last_ch_cabled) last_ch_cabled = chs-1;
                if(chs > m_active_highPt_chs) m_active_highPt_chs = chs;
            }
            wor->add_odd_read_mul(multiplicity);
	    
	    // Set first and last connectors code
	    int code = m_highPt_station*100000 + 1*100000000;
	    int ch = 0;

	    if(last_ch_cabled < 0) {
	      std::cout << "MuonRPC_Cabling: OddPhiCMA - neg. array idx - taking emergency exit!." <<std::endl;
	      throw;
            }
	
	    for(ch=0;ch < m_highPt_rpc_read; ++ch)
	        if(m_highPt[ch][0][last_ch_cabled] >= 0) break;
            if (ch == m_highPt_rpc_read) --ch;
	    m_first_highPt_code = code + m_highPt[ch][0][last_ch_cabled];
	
	    for(ch=0;ch < m_highPt_rpc_read; ++ch)
	        if(m_highPt[ch][0][first_ch_cabled] >= 0) break;
            if (ch == m_highPt_rpc_read) --ch;
	    m_last_highPt_code = code + m_highPt[ch][0][first_ch_cabled];
	}
    }

    return true;
}

bool
OddPhiCMA::cable_CMA_channelsP03(void)
{
    if(pivot_station())  //Check and connect strips with Pivot matrix channels
    {
        WORlink::iterator found = m_pivot_WORs.find(this->pivot_start_ch());
        WiredOR* wor = (*found).second;

        m_pivot_rpc_read = wor->RPCacquired();
        create_pivot_map(m_pivot_rpc_read);

        int start   = pivot_start_st();
        int stop    = pivot_stop_st();
        int max_st  = wor->give_max_phi_strips();

	std::vector <int> multiplicity(max_st);

        for(int i = 0; i < m_pivot_rpc_read; ++i)
        { 
            const RPCchamber* rpc = wor->connected_rpc(i);
            int rpc_st = rpc->phi_strips();
            int cham = rpc->number();
            int local_strip = max_st - (max_st - start) - (max_st - rpc_st);
            int final_strip = max_st - (max_st - stop ) - (max_st - rpc_st);
            int chs = 0;
            do
            {
	        if(chs == pivot_channels)
                {
                    noMoreChannels("Pivot");
		    return false;
	        }
                if(local_strip > 0)
	        {
		    if(rpc->ijk_phiReadout() == 1)
		    {
	            m_pivot[i][0][chs] = cham*100 + local_strip - 1;
	            m_pivot[i][1][chs] = 10000 + cham*100 + local_strip - 1;
		    } else
		    {
		    m_pivot[i][1][chs] = cham*100 + local_strip - 1;
	            m_pivot[i][0][chs] = 10000 + cham*100 + local_strip - 1;
		    }
                    multiplicity[local_strip - 1 + (max_st - rpc_st)] = 1;
	        }
	        ++chs;
            }while(--local_strip >= final_strip);

            if(chs > m_active_pivot_chs) m_active_pivot_chs = chs;
        }
        wor->add_odd_read_mul(multiplicity);
	
	// Set first and last connectors code
	int code = m_pivot_station*100000 + 1*100000000;
	int ch = 0;
	
	for(ch=0;ch < m_pivot_rpc_read; ++ch) if(m_pivot[ch][0][0] >= 0) break;
	m_first_pivot_code = code + m_pivot[ch][0][0];
        if(ch == m_pivot_rpc_read ) --ch;
	
	for(ch=0;ch < m_pivot_rpc_read; ++ch)
	    if(m_pivot[ch][0][m_active_pivot_chs-1] >= 0) break;
        if(ch == m_pivot_rpc_read ) --ch;
	m_last_pivot_code  = code + m_pivot[ch][0][m_active_pivot_chs-1];	
    }
   
    if(lowPt_station() && lowPt_start_ch() != -1)
    {   //Check and connect strips with Low Pt matrix channels
        for (int i = lowPt_start_ch(); i <= lowPt_stop_ch(); ++i)
	{
            WORlink::iterator found = m_lowPt_WORs.find(i);
            m_lowPt_rpc_read += (*found).second->RPCacquired();
	}

        create_lowPt_map(m_lowPt_rpc_read);

        int start   = lowPt_start_st();
        int stop    = lowPt_stop_st();
        int max_st  = get_max_strip_readout(lowPt_station());

        int r = 0; 
        
        for (int w = lowPt_start_ch(); w <= lowPt_stop_ch(); ++w)
	{
            WORlink::iterator found = m_lowPt_WORs.find(w);
            WiredOR* wor = (*found).second;  
	    std::vector <int> multiplicity(wor->give_max_phi_strips());
            multiplicity.clear();

            for(int i = 0; i < wor->RPCacquired(); ++i)
            { 
                const RPCchamber* rpc = wor->connected_rpc(i);

                int rpc_st = rpc->phi_strips();
                int cham   = rpc->number();
                int local_strip= max_st - (max_st - start) - (max_st - rpc_st);
                int final_strip= max_st - (max_st - stop ) - (max_st - rpc_st);
                int chs = 0;
                do
                {
	            if(chs == confirm_channels)
                    {
                        noMoreChannels("Pivot");
		        return false;
	            }
                    if(local_strip > 0)
	            {
		        if(rpc->ijk_phiReadout() == 1)
			{
	                m_lowPt[r][0][chs]= cham*100 + local_strip - 1;
	                m_lowPt[r][1][chs]= 10000 + cham*100 + local_strip - 1;
			} else
			{
			m_lowPt[r][1][chs]= cham*100 + local_strip - 1;
	                m_lowPt[r][0][chs]= 10000 + cham*100 + local_strip - 1;
			}
			if(max_st > wor->give_max_phi_strips())
			{
			  multiplicity[local_strip - 1] = 1;
			} else {
                          multiplicity[local_strip - 1 + (max_st - rpc_st)] = 1;
			}
	            }
	            ++chs;
                }while(--local_strip >= final_strip);

                ++r;
                if(chs > m_active_lowPt_chs) m_active_lowPt_chs = chs;

            }
            wor->add_odd_read_mul(multiplicity);
	    
	    // Set first and last connectors code
	    int code = m_lowPt_station*100000 + 1*100000000;
	    int ch = 0;
	
	    for(ch=0;ch < m_lowPt_rpc_read; ++ch) if(m_lowPt[ch][0][0] >= 0) break;
	    m_first_lowPt_code = code + m_lowPt[ch][0][0];
	    if(ch == m_lowPt_rpc_read) --ch;
	
	    for(ch=0;ch < m_lowPt_rpc_read; ++ch)
	        if(m_lowPt[ch][0][m_active_lowPt_chs-1] >= 0) break;
            if(ch == m_lowPt_rpc_read) --ch;
	    m_last_lowPt_code = code + m_lowPt[ch][0][m_active_lowPt_chs-1];	    
        }
    }

    if(highPt_station() && highPt_start_ch() != -1)
    {   //Check and connect strips with High Pt matrix channels
        for (int i = highPt_start_ch(); i <= highPt_stop_ch(); ++i)
	{
            WORlink::iterator found = m_highPt_WORs.find(i);
            m_highPt_rpc_read += (*found).second->RPCacquired();
	}

        create_highPt_map(m_highPt_rpc_read);

        int start   = highPt_start_st();
        int stop    = highPt_stop_st();
        int max_st  = get_max_strip_readout(highPt_station());

        int r = 0; 

        for (int w = highPt_start_ch(); w <= highPt_stop_ch(); ++w)
	{
            WORlink::iterator found = m_highPt_WORs.find(w);
            WiredOR* wor = (*found).second;  
	    std::vector <int> multiplicity(wor->give_max_phi_strips());
            multiplicity.clear();

            for(int i = 0; i < wor->RPCacquired(); ++i)
            { 
                const RPCchamber* rpc = wor->connected_rpc(i);

                int rpc_st = rpc->phi_strips();
                int cham   = rpc->number();
                int local_strip= max_st - (max_st - start) - (max_st - rpc_st);
                int final_strip= max_st - (max_st - stop ) - (max_st - rpc_st);
                int chs = 0;
                do
                {
	            if(chs == confirm_channels)
                    {
                        noMoreChannels("Pivot");
		        return false;
	            }
                    if(local_strip > 0)
	            {
		        if(rpc->ijk_phiReadout() == 1)
			{
	                m_highPt[r][0][chs]=cham*100 + local_strip - 1;
	                m_highPt[r][1][chs]=10000 + cham*100 + local_strip - 1;
			} else
			{
			m_highPt[r][1][chs]=cham*100 + local_strip - 1;
	                m_highPt[r][0][chs]=10000 + cham*100 + local_strip - 1;
			}
			if(max_st > wor->give_max_phi_strips())
			{
			  multiplicity[local_strip - 1] = 1;
			} else {
                          multiplicity[local_strip - 1 + (max_st - rpc_st)] = 1;
			}
	            }
	            ++chs;
                }while(--local_strip >= final_strip);

                ++r;
                if(chs > m_active_highPt_chs) m_active_highPt_chs = chs;
            }
            wor->add_odd_read_mul(multiplicity);
	    
	    // Set first and last connectors code
	    int code = m_highPt_station*100000 + 1*100000000;
	    int ch = 0;
	
	    for(ch=0;ch < m_highPt_rpc_read; ++ch) if(m_highPt[ch][0][0] >= 0) break;
            if (ch == m_highPt_rpc_read) --ch;
	    m_first_highPt_code = code + m_highPt[ch][0][0];
	
	    for(ch=0;ch < m_highPt_rpc_read; ++ch)
	        if(m_highPt[ch][0][m_active_highPt_chs-1] >= 0) break;
            if (ch == m_highPt_rpc_read) --ch;
	    m_last_highPt_code = code + m_highPt[ch][0][m_active_highPt_chs-1];
	}
    }

    return true;
}

bool
OddPhiCMA::connect(SectorLogicSetup& setup)
{
    bool oldPhiSchema = (setup.layout()=="P03" || setup.layout()=="H8")?
                        true : false;
    
    if(pivot_station())  //Check and connect Pivot chambers
    {
        for(int i=pivot_start_ch();i<=pivot_stop_ch();++i)
        {   
            WiredOR* wor = setup.find_wor(m_pivot_station,i);
            if(wor)
	    {
	        wor->add_cma(this);
	        m_pivot_WORs.insert(WORlink::value_type(i,wor));
	    }
            else 
            {
                this->no_connection_error("WOR",i);
                return false;
            }
        }
    }
    if(lowPt_station())  //Check and connect Low Pt plane chambers
    {
        std::list < const EtaCMA* > CMAs = 
                                  setup.find_eta_CMAs_in_PAD(id().PAD_index());
        if(CMAs.size() == 0)
	{
	    error("   have no Eta matrix into PAD!");
            return false;
	}
	
        //get last CMA fake
	const EtaCMA* cmaFake = CMAs.back(); 
	
	// remove fake CMAs
	std::list < const EtaCMA* >::iterator cm = CMAs.begin();
	while (cm != CMAs.end())
	{
	    if(!(*cm)->pivot_station() || (*cm)->lowPt_start_ch()==-1) cm = CMAs.erase(cm);
	    ++cm;
	}

        int start_ch = (CMAs.size())? CMAs.front()->lowPt_start_ch() :
	                                   cmaFake->lowPt_start_ch();
        int stop_ch  = (CMAs.size())? CMAs.back()->lowPt_stop_ch() :
	                                  cmaFake->lowPt_stop_ch();

        if(start_ch != -1 && stop_ch != -1)
	{
            RPCchamber* start = setup.find_chamber(lowPt_station(),start_ch);
            RPCchamber* stop  = setup.find_chamber(lowPt_station(),stop_ch);

            if(start->readoutWORs().size() == 0 ) 
	       {no_wor_readout(start->number(),lowPt_station());return false;}
	    if(stop->readoutWORs().size() == 0 )
	        {no_wor_readout(stop->number(),lowPt_station());return false;}

            m_lowPt_start_ch = start->readoutWORs().front()->number();
            m_lowPt_stop_ch  = stop->readoutWORs().front()->number();
            int max = 0;

            for(int i=lowPt_start_ch();i<=lowPt_stop_ch();++i)
            {   
                WiredOR* wor = setup.find_wor(lowPt_station(),i);
                if(wor)
	        {
	            wor->add_cma(this);
	            m_lowPt_WORs.insert(WORlink::value_type(i,wor));
                    if(wor->give_max_phi_strips() > max) 
		                              max = wor->give_max_phi_strips();
	        }
                else 
                {
                    this->no_connection_error("WOR",i);
                    return false;
                }
            }
            if(!oldPhiSchema) get_confirm_strip_boundaries(lowPt_station(),max);
	    else get_confirm_strip_boundariesP03(lowPt_station(),max);
	}
    }
    if(highPt_station())  //Check and connect High Pt plane chambers
    {
        std::list < const EtaCMA* > CMAs = 
                                  setup.find_eta_CMAs_in_PAD(id().PAD_index());
        if(CMAs.size() == 0)
	{	    
            error("   have no Eta matrix into PAD!");
            return false;
	}

	//get last CMA fake
	const EtaCMA* cmaFake = CMAs.back(); 

	// remove fake CMAs
	std::list < const EtaCMA* >::iterator cm = CMAs.begin();
	while (cm != CMAs.end())
	{
	    if(!(*cm)->pivot_station() || (*cm)->highPt_start_ch()==-1) cm = CMAs.erase(cm);
	    ++cm;
	}

	int start_ch = (CMAs.size())? CMAs.front()->highPt_start_ch() :
	                                   cmaFake->highPt_start_ch();
        int stop_ch  = (CMAs.size())? CMAs.back()->highPt_stop_ch() :
	                                  cmaFake->highPt_stop_ch();

        if(start_ch != -1 && stop_ch != -1)
	{
            RPCchamber* start = setup.find_chamber(highPt_station(),start_ch);
            RPCchamber* stop  = setup.find_chamber(highPt_station(),stop_ch);

            if(start->readoutWORs().size() == 0 ) 
	       {no_wor_readout(start->number(),highPt_station());return false;}
	    if(stop->readoutWORs().size() == 0 )
	        {no_wor_readout(stop->number(),highPt_station());return false;}

            m_highPt_start_ch = start->readoutWORs().front()->number();
            m_highPt_stop_ch  = stop->readoutWORs().front()->number();
            int max = 0;

            for(int i=highPt_start_ch();i<=highPt_stop_ch();++i)
            {   
                WiredOR* wor = setup.find_wor(highPt_station(),i);
                if(wor)
	        {
	            wor->add_cma(this);
	            m_highPt_WORs.insert(WORlink::value_type(i,wor));
                    if(wor->give_max_phi_strips() > max) 
		                              max = wor->give_max_phi_strips();
	        }
                else 
                {
                    this->no_connection_error("WOR",i);
                    return false;
                }
            }
            if(!oldPhiSchema) get_confirm_strip_boundaries(highPt_station(),max);
	    else get_confirm_strip_boundariesP03(highPt_station(),max);
	}
    }
    return true;
}

void
OddPhiCMA::get_confirm_strip_boundaries(int stat,int max)
{
    //OddPhiCMA::WORlink::const_iterator it = m_pivot_WORs.begin();
    //int max_pivot_strips = (*it).second->give_max_phi_strips();
    if(stat == lowPt_station())
    {
        //if(pivot_start_st() == max_pivot_strips)
	if(this->id().Ixx_index() == 0)
        {
	    m_lowPt_start_st = max;
	    m_lowPt_stop_st = max - confirm_channels + 1+ (40 - max/2);
            if (m_lowPt_stop_st <= 0) m_lowPt_stop_st = 1;
        }
        //else if (pivot_stop_st() == 1)
        if(this->id().Ixx_index() == 1)
	{
	    m_lowPt_stop_st = 1;
	    m_lowPt_start_st = confirm_channels - (40 - max/2);
	    if (m_lowPt_start_st >= max ) m_lowPt_start_st = max;
            //m_lowPt_start_st = (confirm_channels<max)? confirm_channels : max;
        }	
	 
    }
    else if (stat == highPt_station())
    {
        //if(pivot_start_st() == max_pivot_strips)
        if(this->id().Ixx_index() == 0)
	{
	    m_highPt_start_st = max;
	    m_highPt_stop_st = max - confirm_channels + 1+ (40 - max/2);
            if (m_highPt_stop_st <= 0) m_highPt_stop_st = 1;
        }
        //else if (pivot_stop_st() == 1)
        if(this->id().Ixx_index() == 1)
	{
	    m_highPt_stop_st = 1;
	    m_highPt_start_st = confirm_channels - (40 - max/2);
	    if (m_highPt_start_st >= max ) m_highPt_start_st = max;
            //m_highPt_start_st = (confirm_channels<max)? confirm_channels : max;
        }	
	
    }
    
}

void
OddPhiCMA::get_confirm_strip_boundariesP03(int stat,int max)
{
    OddPhiCMA::WORlink::const_iterator it = m_pivot_WORs.begin();
    int max_pivot_strips = (*it).second->give_max_phi_strips();
    if(stat == lowPt_station())
    {
        if(pivot_start_st() == max_pivot_strips)
        {
	    m_lowPt_start_st = max;
	    m_lowPt_stop_st = max - confirm_channels + 1;
            if (m_lowPt_stop_st <= 0) m_lowPt_stop_st = 1;
        }
        else if (pivot_stop_st() == 1)
        {
	    m_lowPt_stop_st = 1;
            m_lowPt_start_st = (confirm_channels<max)? confirm_channels : max;
        }
    }
    else if (stat == highPt_station())
    {
        if(pivot_start_st() == max_pivot_strips)
        {
	    m_highPt_start_st = max;
	    m_highPt_stop_st = max - confirm_channels + 1;
            if (m_highPt_stop_st <= 0) m_highPt_stop_st = 1;
        }
        else if (pivot_stop_st() == 1)
        {
	    m_highPt_stop_st = 1;
            m_highPt_start_st = (confirm_channels<max)? confirm_channels : max;
        }
    }
    
}

int
OddPhiCMA::get_max_strip_readout(int stat)
{
    int max = 0;
    if (stat == pivot_station())
      {
        OddPhiCMA::WORlink::const_iterator it  = m_pivot_WORs.begin();
        while(it != m_pivot_WORs.end())
	{
	    max = (max > (*it).second->give_max_phi_strips())? max :
	                                   (*it).second->give_max_phi_strips();
	    ++it;
	}
      }
    else if(stat == lowPt_station())
    {
        OddPhiCMA::WORlink::const_iterator it  = m_lowPt_WORs.begin();
        while(it != m_lowPt_WORs.end())
	{
	    max = (max > (*it).second->give_max_phi_strips())? max :
	                                   (*it).second->give_max_phi_strips();
	    ++it;
	}
    }
    else if (stat == highPt_station())
    {
        OddPhiCMA::WORlink::const_iterator it  = m_highPt_WORs.begin();
        while(it != m_highPt_WORs.end())
	{
	    max = (max > (*it).second->give_max_phi_strips())? max :
	                                   (*it).second->give_max_phi_strips();
	    ++it;
	}
    }
    return max;
}

bool
OddPhiCMA::setup(SectorLogicSetup& setup)
{
    m_msgSvc = Athena::getMessageSvc();
    MsgStream log(m_msgSvc, name());
    m_debug = log.level() <= MSG::DEBUG;
    bool oldPhiSchema = (setup.layout()=="P03" || setup.layout()=="H8")?
                        true : false;
    
    OddPhiCMA* prev = setup.previousCMA(*this);    
    if(prev && pivot_station() )
    {
        if (this->pivot_start_ch() == prev->pivot_stop_ch())
	{
	    if(!(this->pivot_start_st() == prev->pivot_stop_st() - 1))
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
	    
        }
    }
    else
    {
	
    }

    if( !this->connect(setup) ) return false;
    if( !oldPhiSchema && !this->cable_CMA_channels() ) return false;
    if(  oldPhiSchema && !this->cable_CMA_channelsP03() ) return false;

    // invert the strip cabling if needed
    //if( !this->doInversion(setup) ) return false;

    
    // only 1 repository allowed so far
    std::string LVL1_configuration_repository;
    LVL1_configuration_repository = "ATLAS.121108";
    

    // Read the program file if exist
    SectorLogicSetup::SECTORlist sectors = setup.sectors();
    SectorLogicSetup::SECTORlist::const_iterator it = sectors.begin();
    
    char s_tag[4];
    sprintf(s_tag,"s%02d",*it);
    
    char t_tag[3];
    sprintf(t_tag,"t%1d",id().PAD_index());
    
    char c_tag[4] = {'_','c','0','\0'};
    if(id().phi_index()==1) c_tag[2] = '1';

    std::ifstream CMAprogLow;
    std::istringstream CMAprogLow_COOL;
    char name[200];
    for(int i=0;i<200;++i) name[i] ='\0';

    //LB retrieve the pointer to the map of the trigger roads 
    const std::map<std::string, std::string>* p_trigroads=setup.GetPtoTrigRoads();

    // Read trigger configurations from files
    if(p_trigroads==0){   
      while (!CMAprogLow.is_open() && it != sectors.end())
      {
        __osstream namestr;
	for(int i=0;i<200;++i) name[i] ='\0';
	
	if((*it)%2 != 0)  // load only the Phi program of the Odd sectors
	{
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
            namestr<<dir<<"/"<< s_tag << "_" << t_tag << "_pl" << c_tag << ".txt"
                   << std::ends;// M.C. search for local files
                            
#endif

	    namestr.str().copy(name,namestr.str().length(),0);
	    name[namestr.str().length()] = 0;

	    CMAprogLow.open(name);
	    if (!CMAprogLow.is_open()) CMAprogLow.clear();
            namestr.clear();
	
	}
	++it;
      }
    }
    // Trigger configuration loaded from COOL
    else {
      while (CMAprogLow_COOL.str().empty() && it != sectors.end())
      {
        __osstream namestr;
	for(int i=0;i<200;++i) name[i] ='\0';
	
	if((*it)%2 != 0)  // load only the Phi program of the Odd sectors
	{
          namestr<< s_tag << "_" << t_tag << "_pl" << c_tag << ".txt"
               << std::ends;  
          namestr.str().copy(name,namestr.str().length(),0);
          name[namestr.str().length()] = 0;            
          std::map<std::string, std::string>::const_iterator itc;
          itc=p_trigroads->find(name);
          if(itc != p_trigroads->end()){
            if(m_debug) {
              log<<MSG::DEBUG
  	         << "OddPhiCMA low: key " << name << "found in the Trigger Road Map --> OK" <<endmsg;
              log<<MSG::DEBUG
		 << "OddPhiCMA low: key " <<  itc->second.c_str()<<endmsg;
	    }
            CMAprogLow_COOL.str(itc->second.c_str());
            if(m_debug) log<<MSG::DEBUG
			   << "CMAPROGLOW " << CMAprogLow_COOL.str()<<endmsg;
          }
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
      } 
      else delete program;
      CMAprogLow.close();
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
      } 
      else delete program;
      CMAprogLow_COOL.str("");
    }
    else if (name[0] != '\0')
      {
        if(m_debug) log<<MSG::DEBUG
		       << name << " not found! Putting a dummy configuration"<<endmsg;
	m_lowPt_program = new CMAprogram();
	m_lowPt_program->open_threshold(0);
    }
    
    
    std::ifstream CMAprogHigh;
    std::istringstream CMAprogHigh_COOL;

    it = sectors.begin();
    if(p_trigroads==0){
      while (!CMAprogHigh.is_open() && it != sectors.end())
      {
        __osstream namestr;
	for(int i=0;i<200;++i) name[i] ='\0';
	
	if((*it)%2 != 0)  // load only the Phi program of the Odd sectors
	{
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
            namestr<<dir<<"/"<< s_tag << "_" << t_tag << "_ph" << c_tag << ".txt"
                   << std::ends;
            
#endif

	    namestr.str().copy(name,namestr.str().length(),0);
	    name[namestr.str().length()] = 0;

            CMAprogHigh.open(name);
	    if (!CMAprogHigh.is_open()) CMAprogHigh.clear();
            namestr.clear();
	}
	++it;
      }
    }   
    // Trigger configuration loaded from COOL
    else {
      while (CMAprogHigh_COOL.str().empty() && it != sectors.end())
      {
        __osstream namestr;
	for(int i=0;i<200;++i) name[i] ='\0';
	
	if((*it)%2 != 0)  // load only the Phi program of the Odd sectors
	{
          namestr<< s_tag << "_" << t_tag << "_ph" << c_tag << ".txt"
               << std::ends;  
          namestr.str().copy(name,namestr.str().length(),0);
          name[namestr.str().length()] = 0;            
          std::map<std::string, std::string>::const_iterator itc;
          itc=p_trigroads->find(name);
          if(itc != p_trigroads->end()){
            if(m_debug) {
              log<<MSG::DEBUG
		 << "OddPhiCMA high: key " << name << "found in the Trigger Road Map --> OK" <<endmsg;
              log<<MSG::DEBUG
	         << "OddPhiCMA high: key " <<  itc->second.c_str()<<endmsg;
	    }
            CMAprogHigh_COOL.str(itc->second.c_str());
            if(m_debug) log<<MSG::DEBUG
			   << "CMAPROGHIGH " << CMAprogHigh_COOL.str()<<endmsg;
          }
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
	} 
        else delete program;
	CMAprogHigh_COOL.str("");
    }
    else if (name[0] != '\0')
    {
      if(m_debug) log<<MSG::DEBUG
		  << name << " not found! Putting a dummy configuration"<<endmsg;
	m_highPt_program = new CMAprogram();
	m_highPt_program->open_threshold(0);
    }

    return true;
}

bool
OddPhiCMA::doInversion(SectorLogicSetup& setup)
{
    SectorLogicSetup::SECTORlist Sectors = setup.sectors();
    SectorLogicSetup::SECTORlist::const_iterator it = Sectors.begin();
    int sector = *it;
    
    WORlink::const_iterator wor;
    if(lowPt_station() && lowPt_number_co() != -1)
    {  
        wor = m_lowPt_WORs.begin();
	WiredOR::RPClink Linked = (*wor).second->RPCread();
	WiredOR::RPClink::const_iterator link = Linked.begin(); 
	if( (*link).second->inversion(sector) )
	{
	    m_inversion = true;
	}
    
    }
    
    if(pivot_station())
    {
        wor = m_pivot_WORs.begin();
	WiredOR::RPClink Linked = (*wor).second->RPCread();
	WiredOR::RPClink::const_iterator link = Linked.begin(); 	
	if( (*link).second->inversion(sector) )
	{
	    m_inversion = true;
	}
    }
    
    if(highPt_station() && highPt_number_co() != -1)
    {
        wor = m_highPt_WORs.begin();
	WiredOR::RPClink Linked = (*wor).second->RPCread();
	WiredOR::RPClink::const_iterator link = Linked.begin(); 	
	if( (*link).second->inversion(sector) )
	{
	    m_inversion = true;
	}    
    }
    
    if(m_inversion) m_id->inversion();
    
    
    return true;
}
