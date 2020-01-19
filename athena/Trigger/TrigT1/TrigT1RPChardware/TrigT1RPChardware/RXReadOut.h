/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RXReadOut_H
#define RXReadOut_H

#include<iostream>
#include "TrigT1RPChardware/PadReadOut.h"
#include "TrigT1RPChardware/RXReadOutStructure.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class RXReadOut : public BaseObject {

public:
RXReadOut(ubit16 sectorID, PadReadOut **PRvector);
~RXReadOut();
void reset();
void makeFragment();
void makeHeader();
void makeFooter();
void makeBody();
ubit16 readHeader(); 
ubit16 readFooter();
ubit16 readBody();
void   topRXBody(); 
ubit16 readRXWord();
RXReadOutStructure getHeader();
RXReadOutStructure getFooter();
ubit16 numberOfFragmentWords (){return numberOfWordsInFragment;};
void bytestream(std::ostream &stream);
//void decodeBytestream();

private:
//
static const ubit16 numberOfPads=8;
//
// control flags for Body scanninng ...
//
ubit16 addressOfWordScanned;
ubit16 numberOfPDFragments;
ubit16 numberOfWordsInFragment;
ubit16 newPDIndex;
ubit16 newPDRO;
ubit16 numberOfWordsRead;
ubit16 numberOfWordsInPDRO;
ubit16 endOfPDFragments;
PadReadOut *currentPDRO;
//
//
RXReadOutStructure RROS;
PadReadOut **m_PROlist;
ubit16 m_sectorID;
ubit16 m_Header;
PadReadOut *m_PADFragment[8];
ubit16 m_Footer;
};
#endif
