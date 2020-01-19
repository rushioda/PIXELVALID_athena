/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/CMAdata.h"
//#include "RPCcablingInterface/CablingRPCBase.h"

#ifdef LVL1_STANDALONE
// ==> #include "RPCcabling/CablingRPC.h"
#endif


using namespace std;

CMAdata::CMAdata(unsigned long int debug) : 
    BaseObject(Data,"CMApatterns"),m_debug(debug)
{
    m_eta_cma_patterns.clear();
    m_phi_cma_patterns.clear();
}

#ifdef LVL1_STANDALONE
CMAdata::CMAdata(const RPCdata* rpc_data) : BaseObject(Data,"CMApatterns"),
    m_debug(0) 
#else
CMAdata::CMAdata(const RPCdata* rpc_data,const IRPCcablingSvc* cabling,
                 unsigned long int debug) : 
    BaseObject(Data,"CMApatterns"),m_debug(debug)
#endif
{
    m_eta_cma_patterns.clear();
    m_phi_cma_patterns.clear();

#ifdef LVL1_STANDALONE
    const CablingRPCBase* cabling = RPCcabling::CablingRPC::instance();
#endif
    //    cout << "Comincio:" << endl;
    //    RPCcablingSvc*  cabling;
    //    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    //    cout << "Istanziato il service locator" << endl;
    //    StatusCode sc = svcLoc->service( "RPCcablingSvc", cabling );
    //    if(sc.isFailure()) cout << "caZZo!" << endl; 
    //#endif

    if(!cabling) 
    {
        DISP << "Creation of CMA data is not possible: no cabling service!" 
             << endl;
	DISP_ERROR;
        return;
    }

    RPCdata::digitList eta = rpc_data->eta_digits_list();
    RPCdata::digitList::const_iterator digi = eta.begin();

    while(digi != eta.end())
    {
	const int sector       = (*digi)->decoding().logic_sector();
        const ViewType type    = (*digi)->decoding().view();
	const int station      = (*digi)->decoding().lvl1_station();
        const int cabling_code = (*digi)->decoding().cabling_code();

	const CMAparameters::CMAlist list =
                          cabling->give_CMAs(sector,type,station,cabling_code);
        CMAparameters::CMAlist::const_iterator cma = list.begin();
        while(cma != list.end())
	{
            create_patterns(*cma,*digi);
	    ++cma;
	}
	++digi;
    }     

    RPCdata::digitList phi = rpc_data->phi_digits_list();
    digi = phi.begin();
    while(digi != phi.end())
    {
	const int sector       = (*digi)->decoding().logic_sector();
        const ViewType type    = (*digi)->decoding().view();
	const int station      = (*digi)->decoding().lvl1_station();
        const int cabling_code = (*digi)->decoding().cabling_code();

	const CMAparameters::CMAlist list =
                          cabling->give_CMAs(sector,type,station,cabling_code);
        CMAparameters::CMAlist::const_iterator cma = list.begin();
        while(cma != list.end())
	{
            create_patterns(*cma,*digi);
	    ++cma;
	}
	++digi;
    }
}



CMAdata::CMAdata(const CMAdata& cma_patterns) : 
    BaseObject(Data,cma_patterns.name())
{
    m_eta_cma_patterns = cma_patterns.eta_cma_patterns();
    m_phi_cma_patterns = cma_patterns.phi_cma_patterns();    
    m_debug     = cma_patterns.debug();
}

CMAdata::~CMAdata()
{
    m_eta_cma_patterns.clear();
    m_phi_cma_patterns.clear();
}

CMAdata
CMAdata::operator=(const CMAdata& cma_patterns)
{
    static_cast<BaseObject&>(*this) = 
                                  static_cast<const BaseObject&>(cma_patterns);
    m_eta_cma_patterns.clear();
    m_phi_cma_patterns.clear(); 
    m_eta_cma_patterns = cma_patterns.eta_cma_patterns();
    m_phi_cma_patterns = cma_patterns.phi_cma_patterns();
    m_debug     = cma_patterns.debug();
    return *this;
}

void
CMAdata::create_patterns(const CMAparameters* cma,const RPCdigit* digit)
{
    CMApatterns* patterns;	

    const int sector    = digit->decoding().logic_sector();
    const ViewType type = digit->decoding().view();

    if( (patterns = find(sector,cma)) ) patterns->load_digit(digit);
    else 
    {
        patterns = new CMApatterns(sector,cma,m_debug);
        patterns->load_digit(digit);
	if(type == Eta)       m_eta_cma_patterns.push_back(*patterns);
        else if (type == Phi) m_phi_cma_patterns.push_back(*patterns);
	delete patterns;
    }
}

    
CMApatterns*
CMAdata::find(const int sector,const CMAparameters* cma)
{
    const ViewType view = cma->id().type();

    if(view == Eta)
    {
	PATTERNSlist::iterator it = m_eta_cma_patterns.begin();
	while (it != m_eta_cma_patterns.end())
	{
	    if((*it).cma_parameters() == *cma &&
               (*it).sector()         == sector  ) return &(*it);
	    ++it;
	}
    }
    else if (view == Phi)
    {
	PATTERNSlist::iterator it = m_phi_cma_patterns.begin();
	while (it != m_phi_cma_patterns.end())
	{
	    if((*it).cma_parameters() == *cma &&
               (*it).sector()         == sector  ) return &(*it);
	    ++it;
	}
    }

    return 0;
}


CMAdata::PatternsList
CMAdata::give_patterns()
{
    PatternsList patterns;

    PATTERNSlist::iterator eta = m_eta_cma_patterns.begin();
    PATTERNSlist::iterator phi = m_phi_cma_patterns.begin();

    while(eta != m_eta_cma_patterns.end())
    {   
        patterns.push_back(&(*eta));
	++eta;
    }
    while(phi != m_phi_cma_patterns.end())
    {
        patterns.push_back(&(*phi));
	++phi;
    }
    return patterns;
}


void CMAdata::PrintElement(ostream& stream,std::string element,bool detail) 
    const
{
    bool all  = (element == name() || element == "")? true : false;
    bool nEta = eta_cma_patterns().size();
    bool nPhi = phi_cma_patterns().size();
    bool printed = false;

    if(nEta && (element == (*m_eta_cma_patterns.begin()).name() || all))
    {
        stream << name() << " contains " << eta_cma_patterns().size()
	       << " eta patterns:" << endl;  
        printed = true;
	PATTERNSlist::const_iterator it = m_eta_cma_patterns.begin();
	while(it != m_eta_cma_patterns.end())
        {
            it->Print(stream,detail);
	    ++it;
	}
    }

    if (nPhi && (element == (*m_phi_cma_patterns.begin()).name() || all))
    {
        stream << name() << " contains " << phi_cma_patterns().size()
	       << " phi patterns:" << endl;  
        printed = true;
        PATTERNSlist::const_iterator it = m_phi_cma_patterns.begin();
	while(it != m_phi_cma_patterns.end())
        {
            it->Print(stream,detail);
	    ++it;
	}
    }

    if(!printed)
    {
        if (element == "") element = "CMAs";
        stream << name() << " contains no " << element << "!" << endl;
    }
}


void CMAdata::Print(ostream& stream,bool detail) const
{
    stream << name() << " contains " 
           << eta_cma_patterns().size()
	   << " eta patterns and " << phi_cma_patterns().size()
	   << " phi patterns" << endl;
    
    PATTERNSlist::const_iterator eta = eta_cma_patterns().begin();
    PATTERNSlist::const_iterator phi = phi_cma_patterns().begin();

    while(eta != eta_cma_patterns().end())
    {
        (*eta).Print(stream,detail);
	++eta;
    }
    while(phi != phi_cma_patterns().end())
    {
        (*phi).Print(stream,detail);
	++phi;
    }
}
