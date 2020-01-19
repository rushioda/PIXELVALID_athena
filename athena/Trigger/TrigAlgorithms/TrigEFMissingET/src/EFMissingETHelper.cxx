/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *********************************************************************
//
// NAME:        EFMissingETHelper.cxx
// PACKAGE:     Trigger/TrigAlgorithms/TrigEFMissingET
//
// AUTHOR:      Diego Casadei
// CREATED:     May 6, 2008
//
// Description: Transient-only helper class (companion to TrigMissingET)
// *********************************************************************


#include "TrigEFMissingET/EFMissingETHelper.h"

#include <iostream>
#include <cstdio>
#include <cmath>


TrigEFMissingEtComponent::TrigEFMissingEtComponent(){
  std::snprintf(m_name,10,"--empty--");
  m_skip=false;
  m_status=0;
  m_calib0=0; 
  m_calib1=1; 
  Reset();
}

//---

void TrigEFMissingEtComponent::Reset(){
  m_status=0;
  m_usedChannels=0;
  m_sumOfSigns=0; 
  m_ex = m_ey = m_ez = m_sumEt = m_sumBadEt = m_sumE = 0;
  m_minTime = m_maxTime = 0;
  m_maxE = m_minE = 0;
  m_maxQlty = 0;
}

//---

std::string TrigEFMissingEtComponent::getFormattedHeader(){
  std:: string s="__name____skip_status__usedCh._sumOfSigns__calib1_calib0/MeV";
  s+="___Ex/MeV_____Ey/MeV_____Ez/MeV___SumE/MeV__SumEt/MeV";
  s+="__tMin/ns__tMax/ns__minE/MeV__maxE/MeV__maxQlty__sumBadEt/MeV";
  return s;
}

//---

std::string TrigEFMissingEtComponent::getFormattedValues(){
  char buff[1024];
  const char* c="N";
  if (m_skip) c="Y";
  std::snprintf(buff,1024,
      "%s   %s 0x%04x %8d %11d %7.2f %8.2f %10.2f %10.2f %10.2f %10.2f %10.2f %8.2f %8.2f %9.1f %9.1f %8.1f %10.2f", 
      m_name, c,
      (unsigned short) m_status, m_usedChannels, m_sumOfSigns, m_calib1, m_calib0,
      m_ex, m_ey, m_ez, m_sumE, m_sumEt,
      m_minTime, m_maxTime, m_minE, m_maxE, m_maxQlty, m_sumBadEt);
  return std::string(buff);
}


/////////////////////////////////////////////////////////////////////


TrigEFMissingEtHelper::TrigEFMissingEtHelper(unsigned char len){
  m_status=0;
  m_ex=m_ey=m_ez=m_sumEt=m_sumE=0;

  len>REASONABLE_MAX ? m_elements=REASONABLE_MAX : m_elements=len;
  m_vecOfComponents.resize(m_elements);

  char names[42][10] = { // from CaloSampling::CaloSample, apart from muons
    "PreSamplB", "EMB1     ", "EMB2     ", "EMB3     ",   // LAr barrel
    "PreSamplE", "EME1     ", "EME2     ", "EME3     ",   // LAr EM endcap 
    "HEC0     ", "HEC1     ", "HEC2     ", "HEC3     ",   // Hadronic end cap cal.
    "TileBar0 ", "TileBar1 ", "TileBar2 ",                // Tile barrel
    "TileGap1 ", "TileGap2 ", "TileGap3 ",                // Tile gap (ITC & scint)
    "TileExt0 ", "TileExt1 ", "TileExt2 ",                // Tile extended barrel
    "FCalEM   ", "FCalHad1 ", "FCalHad2 ",                // Forward cal endcap
    "TCLCW    ",										  // Topo. clusters Had
    "TCLCWB1  ", "TCLCWB2  ",						      // pos. and neg. eta barrel 
    "TCLCWE1  ", "TCLCWE2  ",							  // pos. and neg. eta endcap 
    "TCEM     ",                                          // Topo. clusters EM
    "TCEMB1   ", "TCEMB2   ",							  // pos. and neg. eta barrel 
    "TCEME1   ", "TCEME2   ",							  // pos. and neg. eta endcap 
    "JET      ",                                          // Jet 
    "JETB1   ", "JETB2   ",							      //
    "JETE1   ", "JETE2   ",						     	  // 
    "TCPUC    ",                                          // Topo. cluster Fit 
    "TCPUCUnc ",                                          // Topo. cluster Fit -- uncorrected
    "Muons    "                                           // Muons 
  };
  // calibration: constant term, MeV
  float c0[42] = { 0.0,   0.0,   0.0,   0.0,    // LAr barrel
		   0.0,   0.0,   0.0,   0.0,    // LAr EM endcap
		   0.0,   0.0,   0.0,   0.0,    // Hadronic end cap cal.
		   0.0,   0.0,   0.0,           // Tile barrel
		   0.0,   0.0,   0.0,           // Tile gap (ITC & scint)
		   0.0,   0.0,   0.0,           // Tile extended barrel
		   0.0,                         // FCal EM
		   0.0,   0.0,                  // FCal Had
		   0.0,                         // Topo. clusters EM
		   0.0,   0.0,                  // 
		   0.0,   0.0,                  // 
		   0.0,                         // Topo. clusters Had
		   0.0,   0.0,                  // 
		   0.0,   0.0,                  // 		   
		   0.0,                         // Jet
		   0.0,   0.0,                  // 
		   0.0,   0.0,                  // 		   
		   0.0,                         // Topo. cluster Fit 
		   0.0,                         // Topo. cluster Fit -- uncorrected
		   0.0                          // Muons
  };
  // calibration: linear term (slope)
  float c1[42] = { 1.0,  1.0,  1.0,  1.0,    // LAr barrel
		   1.0,  1.0,  1.0,  1.0,    // LAr EM endcap
		   1.0,  1.0,  1.0,  1.0,    // Hadronic end cap cal.
		   1.0,  1.0,  1.0,           // Tile barrel
		   1.0,  1.0,  1.0,           // Tile gap (ITC & scint)
		   1.0,  1.0,  1.0,           // Tile extended barrel
		   1.0,                         // FCal EM
		   1.0,  1.0,                  // FCal Had
		   1.0,                          // Topo. clusters EM
		   1.0, 1.0,                     // 
		   1.0, 1.0,                     // 
		   1.0,                          // Topo. clusters Had
		   1.0, 1.0,                     // 
		   1.0, 1.0,                     // 
		   1.0,                          // Jet
		   1.0, 1.0,                     // 
		   1.0, 1.0,                     // 
		   1.0,                          // Topo. cluster Fit 
		   1.0,                          // Topo. cluster Fit -- uncorrected
		   1.0                           // Muons
  };

  for (unsigned char i=0; i<m_elements; ++i){
    if(m_elements==42){ // finest granularity
      std::snprintf(m_vecOfComponents[i].m_name,10, "%s", names[i]);
      m_vecOfComponents[i].m_calib0 = c0[i];
      m_vecOfComponents[i].m_calib1 = c1[i];
    } else {
      std::snprintf(m_vecOfComponents[i].m_name,10, "comp%03d  ", i);
      m_vecOfComponents[i].m_calib0 = 0;
      m_vecOfComponents[i].m_calib1 = 1;
    }
  }

  Reset();
}

//---

void TrigEFMissingEtHelper::Reset(){
  m_ex=m_ey=m_ez=m_sumEt=m_sumE=0;
  m_status=0;
  for (unsigned char i=0; i<m_elements; ++i){
    m_vecOfComponents[i].Reset();
    if (m_vecOfComponents[i].m_skip) 
      m_vecOfComponents[i].m_status=1;
    else
      m_vecOfComponents[i].m_status=0;
  }
}

//---

  TrigEFMissingEtComponent* TrigEFMissingEtHelper::GetComponent(unsigned char c){
    if (c<m_elements)
      return &(m_vecOfComponents[c]);
    else
      return 0;
  }

//---

std::string TrigEFMissingEtHelper::getFormattedValues(){
  char buff[256];
  std::snprintf(buff,256,"status = 0x%08x,\telements = %d", (unsigned)m_status, (int)m_elements);
  std::string s(buff);
  if (m_elements==0) return s; // basic info only

  s += ":\n\t\t";
  s += m_vecOfComponents[0].getFormattedHeader();
  for (unsigned char i=0; i<m_elements; ++i){
    s += "\n\t\t";
    s += m_vecOfComponents[i].getFormattedValues();
  }
  return s;
}

//---

MsgStream& operator<<(MsgStream& out, TrigEFMissingEtHelper& obj){
  std::string s;
  std::string start("REGTEST: (EF) ");
  char buff[380];

  std::snprintf(buff,sizeof(buff),"Ex    = %10.2f MeV", obj.m_ex);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"Ey    = %10.2f MeV", obj.m_ey);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"Ez    = %10.2f MeV", obj.m_ez);
  s = start;
  s += buff;
  out << s << endmsg;

  float et = std::sqrt(obj.m_ex*obj.m_ex+obj.m_ey*obj.m_ey);
  std::snprintf(buff,sizeof(buff),"Et    = %10.2f MeV", et);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"SumEt = %10.2f MeV", obj.m_sumEt);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"SumE  = %10.2f MeV", obj.m_sumE);
  s = start;
  s += buff;
  out << s << endmsg;

  std::snprintf(buff,sizeof(buff),"Event status = 0x%08x;   auxiliary components = %4d", 
	       (unsigned)obj.m_status, (int)obj.m_elements);
  s = start;
  s += buff;
  out << s << endmsg;

  if (obj.m_elements==0) return out; // basic info only

  s = start;
  s += obj.m_vecOfComponents[0].getFormattedHeader();
  out << s << endmsg;

  for (unsigned char i=0; i<obj.m_elements; ++i){
    s = start;
    s += obj.m_vecOfComponents[i].getFormattedValues();
    out << s << endmsg;
  }

  return out;
}
