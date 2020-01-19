/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iostream>
#include<iomanip>
#include<cstring>
#include "TrigT1RPChardware/SectorLogicReadOut.h"
#include "MuonCablingTools/BaseObject.h"

using namespace std;

//----------------------------------------------------------------------------//
SectorLogicReadOut::SectorLogicReadOut() 
                             : BaseObject(Hardware,"SectorLogicReadOut") {
initialize();
}//end-of-SectorLogicReadOut
//----------------------------------------------------------------------------//
SectorLogicReadOut::SectorLogicReadOut(const SectorLogicReadOut &SLROOrig)
                   : BaseObject(Hardware,"SectorLogicReadOut") {
//
// copy constructor
//
m_Header = SLROOrig.m_Header;
m_Footer = SLROOrig.m_Footer;
m_Body        = 0;
m_BodyCounter = 0;
m_BodyLast    = 0;
m_BodyCurr    = 0;
numberOfWordsInFrag = SLROOrig.numberOfWordsInFrag;
numberOfWordsInBody = SLROOrig.numberOfWordsInBody;
numberOfWordsInCounters = SLROOrig.numberOfWordsInCounters;
numberOfWordsInSLHits = SLROOrig.numberOfWordsInSLHits;
counter32ok = SLROOrig.counter32ok;
hitok = SLROOrig.hitok;
for( ubit16 i=0; i<numberOfDataCounters; i++) {
  counter16[i] = SLROOrig.counter16[i];
}
for( ubit16 i=0; i<numberOfDecodedCounters; i++) {
  counter32[i] = SLROOrig.counter32[i];
}
//
// Copy Dynamic structure
//
SLROData *q = 0;
SLROData *r = 0;
SLROData *p = SLROOrig.m_Body;
ubit16 cnter=0;
while(p) {
 q = new SLROData;
 q->hit = p->hit;
 q->next = 0;
 if(!cnter) {
  m_Body = q;
  if(cnter==(nGates*nLinks)) m_BodyCounter = q;
 } 
  else r->next=q;
 cnter++;
 r = q;
 p=p->next;
}//end-of-while
m_BodyLast=r;
}//end-of-SectorLogicReadOut
//----------------------------------------------------------------------------//
SectorLogicReadOut::~SectorLogicReadOut() {
//
// delete the SLROData dynamic structure used to describe Body part
// of the event fragment.
//
deleteSLBody();
}
//----------------------------------------------------------------------------//
void SectorLogicReadOut::deleteSLBody() {
  SLROData *p, *q;
  p=m_Body;
  while(p) {
    q=p;
    p=p->next;
    delete q;
  }//end-of-while
  p=0;
  q=0;
  m_Body           = 0;
  m_BodyLast       = 0;
  m_BodyCurr       = 0;
  m_BodyCounter    = 0;
  numberOfWordsInBody= 0;
}//end-of-deleteSLBody
//----------------------------------------------------------------------------//
void SectorLogicReadOut::initialize() {
  //
  // initialize data members
  //
  m_Header=0;
  m_Footer=0;
  //
  // initialize pointers
  //
  m_Body         =0;
  m_BodyLast     =0;
  m_BodyCurr     =0;
  m_BodyCounter  =0;
  //
  // initialize check flags
  //
  numberOfWordsInFrag      =0;
  numberOfWordsInBody      =0;
  numberOfWordsInCounters  =numberOfDataCounters;
  numberOfWordsInSLHits    =0;
  counter32ok=false;
  hitok      =false;
  for(ubit16 i=0; i<numberOfWordsInCounters; i++) {counter16[i]=0;}
  for(ubit16 i=0; i<numberOfDecodedCounters; i++) {counter32[i]=0;}
}//end-of-initialize
//----------------------------------------------------------------------------//
void SectorLogicReadOut::reset() {
//
// reset the data structure to host a new fragment;
// first delete the dinamyc structure...
deleteSLBody();
// then initialize the data members.
initialize();
}//end-of-SectorLogicReadOut::reset()
//----------------------------------------------------------------------------//
void SectorLogicReadOut::writeRecord(ubit16 thisRecord, bool last) {

  if(numberOfWordsInFrag==0) {
    m_Header = thisRecord;
  } else if(numberOfWordsInFrag && !last) {
    makeNewHit(thisRecord);
  } else {
    m_Footer = thisRecord;
  }
  numberOfWordsInFrag++;  
}//end-of-void SectorLogicReadOut
//----------------------------------------------------------------------------//  
void SectorLogicReadOut::makeNewHit(ubit16 newHit) {  
  SLROData *p;
  p = new SLROData;
  p->hit=newHit;
  p->next=0;
  if(!m_Body) {
    m_Body = p;
  } else {
    m_BodyLast->next = p;
  }//end-of-if
  m_BodyLast=p;
  if(!numberOfWordsInBody) topSLBody();
  numberOfWordsInBody++;
  if(numberOfWordsInBody > numberOfWordsInCounters)
    numberOfWordsInSLHits = (numberOfWordsInBody-numberOfWordsInCounters);
}//end-of-SectorLogicReadOut::makeNewHit
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::readSLHitCurrent(){
  if(m_BodyCurr) {
    ubit16 hit = m_BodyCurr->hit;
    m_BodyCurr=m_BodyCurr->next;
    return hit;
  } else {
    return 0xefac;
  }
}//end-of-SectorLogicReadOut::readSLHitCurrent
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::readSLCounterCurrent(){
  if(!m_BodyCounter) {
    SLROData *temp=m_Body;
    for(ubit16 i=0; i<numberOfHitWords(); i++) {
      if(temp->next) temp = temp->next;
    }
    if(temp) m_BodyCounter=temp;
    m_BodyCurr = m_BodyCounter;
  }
// 
  if(m_BodyCurr) {
    ubit16 hit = m_BodyCurr->hit;
    m_BodyCurr=m_BodyCurr->next;
    return hit;
  } else {
    return 0xcafe;
  }
}//end-of-SectorLogicReadOut::readSLCounterCurrent
//----------------------------------------------------------------------------//
void SectorLogicReadOut::doCounter32(){
  topSLBodyCounters();
  for(ubit16 i=0; i<numberOfWordsInCounters; i++) {
   counter16[i]=readSLCounterCurrent()&0x1fff;
  }
//
  ubit16 j=0;
  for(ubit16 i=0;i<numberOfDecodedCounters;i++){
    counter32[i]=(counter16[j]|(counter16[j+1]<<13)|(counter16[j+2] <<26));
    j+=3;
  }
}//end-of-SectorLogicReadOut::doCounter32()
//----------------------------------------------------------------------------//
RODword SectorLogicReadOut::getCounter32(ubit16 index){
  if(!counter32ok) {
   doCounter32();
   counter32ok=true;
  }
  if(index<numberOfDecodedCounters) {
    return counter32[index];
  } else {
   DISP<<" getCounter32: index= "<<index<<" is larger than "<<numberOfDecodedCounters<<std::endl;
   DISP<<" getCounter32: ... return 0 value"<<std::endl;
   DISP_ERROR;
   return 0;
  }
}
//----------------------------------------------------------------------------//
float SectorLogicReadOut::padTriggerRate(ubit16 padAddress){
  if(!counter32ok) doCounter32();
  //
  // units are kHz
  //
  static const float convertToTriggerRate=160314.74/4.0;// units are kHz
  if(padAddress < ((numberOfDecodedCounters-3)/2)) {
   return convertToTriggerRate
          * (float (counter32[padAddress*2+1])/float (counter32[padAddress*2+0]));
  } else {
    DISP<<" padTrigger: input padAddress= "<<padAddress<<" is not possible; "
        <<" return -1 value "<<std::endl;
    DISP_ERROR;
    return -1.0;
  }
}
//----------------------------------------------------------------------------//
void SectorLogicReadOut::doHit(){
  topSLBody();
  for(ubit16 j=0; j<nGates; j++) {
    for(ubit16 i=0; i<nLinks; i++) {
     hit[i][j]=readSLHitCurrent();
    }//end-of-for(ubit16 i
  }//end-of-for(ubit16 j
}//end-of-SectorLogicReadOut::doHit()
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::cmadd(ubit16 indexLink, ubit16 indexGate) {
  if(!hitok) doHit();
  if(indexLink<nLinks && indexGate<nGates) {
   return (hit[indexLink][indexGate]   ) & 0x3;
  } else {
    DISP<<" cmid: indexLink or indexGate is not possible "
        <<" return 0 value "<<std::endl;
    DISP_ERROR;
    return 0;
  }
}//end-of-SectorLogicReadOut::cmid(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::ptid(ubit16 indexLink, ubit16 indexGate) {
  if(!hitok) doHit();
  if(indexLink<nLinks && indexGate<nGates) {
   return (hit[indexLink][indexGate]>>2) & 0x7;
  } else {
    DISP<<" ptid: indexLink or indexGate is not possible "
        <<" return 0 value "<<std::endl;
    DISP_ERROR;
    return 0;
  }
}//end-of-SectorLogicReadOut::ptid(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::opl(ubit16 indexLink, ubit16 indexGate) {
  if(!hitok) doHit();
  if(indexLink<nLinks && indexGate<nGates) {
   return (hit[indexLink][indexGate]>>5) & 0x1;
  } else {
    DISP<<" opl : indexLink or indexGate is not possible "
        <<" return 0 value "<<std::endl;
    DISP_ERROR;
    return 0;
  }
}//end-of-SectorLogicReadOut::opl(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::ovphi(ubit16 indexLink, ubit16 indexGate) {
  if(!hitok) doHit();
  if(indexLink<nLinks && indexGate<nGates) {
   return (hit[indexLink][indexGate]>>6) & 0x1;
  } else {
    DISP<<" ovphi: indexLink or indexGate is not possible "
        <<" return 0 value "<<std::endl;
    DISP_ERROR;
    return 0;
  }
}//end-of-SectorLogicReadOut::ovphi(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::oveta(ubit16 indexLink, ubit16 indexGate) {
  if(!hitok) doHit();
  if(indexLink<nLinks && indexGate<nGates) {
   return (hit[indexLink][indexGate]>>7) & 0x1;
  } else {
    DISP<<" oveta: indexLink or indexGate is not possible "
        <<" return 0 value "<<std::endl;
    DISP_ERROR;
    return 0;
  }
}//end-of-SectorLogicReadOut::oveta(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::res(ubit16 indexLink, ubit16 indexGate) {
  if(!hitok) doHit();
  if(indexLink<nLinks && indexGate<nGates) {
   return (hit[indexLink][indexGate]>>8) & 0x1;
  } else {
    DISP<<" res : indexLink or indexGate is not possible "
        <<" return 0 value "<<std::endl;
    DISP_ERROR;
    return 0;
  }
}//end-of-SectorLogicReadOut::res(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
ubit16 SectorLogicReadOut::bcid(ubit16 indexLink, ubit16 indexGate) {
  if(!hitok) doHit();
  if(indexLink<nLinks && indexGate<nGates) {
   return (hit[indexLink][indexGate]>>9) & 0x7;
  } else {
    DISP<<" bcid: indexLink or indexGate is not possible "
        <<" return 0 value "<<std::endl;
    DISP_ERROR;
    return 0;
  }
}//end-of-SectorLogicReadOut::bcid(ubit16 indexLink, ubit16 indexGate)
//----------------------------------------------------------------------------//
void SectorLogicReadOut::display(std::ostream &stream) {
  stream<<" **** Sector Logic ReadOut Fragment ****"<<std::endl;
  stream<<" SectorLogic: number of Hits    :"<<numberOfWordsInSLHits<<std::endl;
  stream<<" SectorLogic: number of Counters:"<<numberOfWordsInCounters<<std::endl;
  stream<<" SectorLogic: Header "<<std::hex<<m_Header<<std::dec<<std::endl;
  for(ubit16 i=0; i<numberOfWordsInSLHits; i++) {
    stream<<" SectorLogic: hit       "<<(i+1)<<"  ==> "
          <<std::hex<<readSLHitCurrent()<<std::dec<<std::endl;
  }
  stream<<" SectorLogic: Footer "<<std::hex<<m_Footer<<std::dec<<std::endl;
  for(ubit16 i=0; i<numberOfWordsInCounters; i++) {
    stream<<" SectorLogic: counter   "<<(i+1)<<"  ==> "
          <<std::hex<<readSLCounterCurrent()<<std::dec<<std::endl;
  }
  for(int i=0; i<numberOfDecodedCounters; i++) {
    stream<<" SectorLogic: Counter32 "<<(i+1)<<"  ==> "
          <<" = "<<std::hex<<getCounter32(i)<<std::dec<<std::endl;
  }
  stream<<" SectorLogic: Pad 0 trigger rate: "<<padTriggerRate(0)<<" kHz"<<std::endl;
  stream<<" SectorLogic: Pad 1 trigger rate: "<<padTriggerRate(1)<<" kHz"<<std::endl;
}//end-of-void SectorLogicReadOut::display
//----------------------------------------------------------------------------//
bool SectorLogicReadOut::checkFragment() {
  return (numberOfWordsInBody==(nGates*nLinks+numberOfDataCounters));
}//end-of-void SectorLogicReadOut::checkFragment()
