/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/MuonAlignmentPar.h"

MuonAlignmentPar::MuonAlignmentPar() :
  m_Jff(0), m_Jzz(0), m_Job(0), m_isNew(false)
{ }

MuonAlignmentPar::~MuonAlignmentPar() 
{ }



void MuonAlignmentPar::setAmdbId(std::string type, int jff, int jzz, int job) 
{
  m_Type = type;
  m_Jff  = jff;
  m_Jzz  = jzz;
  m_Job  = job;  

  return;
}

void MuonAlignmentPar::getAmdbId(std::string& type, int& jff, int& jzz, int& job) 
{
  type = m_Type;
  jff  = m_Jff;
  jzz  = m_Jzz;
  job  = m_Job;  

  return;
}

