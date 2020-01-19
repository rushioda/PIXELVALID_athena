/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PADDATA_H
#define PADDATA_H

#include <list>
#include "MuonCablingTools/BaseObject.h"

#include "TrigT1RPClogic/PADpatterns.h"
#include "TrigT1RPClogic/CMAdata.h"


class PADdata : public BaseObject
{
    public:
    typedef std::list < PADpatterns > PATTERNSlist;
    typedef std::list < PADpatterns* > PatternsList;

    private:
    unsigned long int m_debug;
    PATTERNSlist m_pad_patterns;
    const IRPCcablingSvc* m_cabling;
    
    void create_pad_patterns(CMApatterns*);
    PADpatterns* find(const int,const int);
    

    public:
    PADdata(CMAdata*, const IRPCcablingSvc* cablingSvc,unsigned long int );
    PADdata(const PADdata&);
    ~PADdata();

    PADdata operator=(const PADdata&);


    unsigned long int debug(void) const {return m_debug;}
    const IRPCcablingSvc* cabling(void) const {return m_cabling;};
    
    const PATTERNSlist& pad_patterns(void) const {return m_pad_patterns;}
    PatternsList give_patterns(void);

    void PrintElement(std::ostream&,std::string,bool) const;
    void Print(std::ostream&,bool) const;
};

template <class X> X& operator<<(X& stream,const PADdata& data)
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[5000000];
    for (int i=0;i<5000000;++i) buffer[i] = '\0';
    __osstream display(buffer,5000000);
#endif

    data.Print(display,false);
    stream << display.str();
    return stream;
}


#endif
