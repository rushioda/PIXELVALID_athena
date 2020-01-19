/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_FILESCHEDULINGTOOL_H
#define ATHENAMPTOOLS_FILESCHEDULINGTOOL_H 1

#include "AthenaMPToolBase.h"
#include "AthenaInterprocess/SharedQueue.h"

class FileSchedulingTool : public AthenaMPToolBase
{
 public:
  FileSchedulingTool(const std::string& type
		     , const std::string& name
		     , const IInterface* parent);

  virtual ~FileSchedulingTool();
  
  StatusCode initialize();
  StatusCode finalize();

  // _________IAthenaMPTool_________   
  int makePool(int maxevt, int nprocs, const std::string& topdir);
  StatusCode exec();

  void subProcessLogs(std::vector<std::string>&);

  // _____ Actual working horses ________
  std::unique_ptr<AthenaInterprocess::ScheduledWork> bootstrap_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> exec_func();
  std::unique_ptr<AthenaInterprocess::ScheduledWork> fin_func();

 private:
  FileSchedulingTool();
  FileSchedulingTool(const FileSchedulingTool&);
  FileSchedulingTool& operator= (const FileSchedulingTool&);

  int   m_nevts;           // Number of events to be processed
  int   m_rankId;          // Each worker has its own unique RankID from the range (0,...,m_nprocs-1) 
  bool  m_debug;

  AthenaInterprocess::SharedQueue*  m_sharedRankQueue;          
  AthenaInterprocess::SharedQueue*  m_sharedFinQueue;   // Shared queue for scheduling worker finalizations (fighting memory spikes)

  bool                           m_isPileup;
};

#endif
