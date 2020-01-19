/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBALGOSEQUENCER
#define TBREC_TBALGOSEQUENCER
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include <map>

class StoreGateSvc;

class TBAlgoSequencer : public AthAlgorithm
{

  typedef std::string             KeyType;
  typedef std::vector<Algorithm*> AlgoStore;
  typedef AlgoStore::iterator     AlgoIterator;

 public:

  TBAlgoSequencer(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBAlgoSequencer();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ////////////////
  // Properties //
  ////////////////

  std::vector<std::string> m_subAlgoNames;
  AlgoStore  m_subAlgos;
  std::vector<std::string> m_algoNameStore;
  bool m_timingOn;

  ////////////////
  // Statistics //
  ////////////////

  unsigned int m_eventPrintFreq;
  unsigned int m_eventCounter;
  unsigned int m_rejectCounter;
  unsigned int m_rejectNoEvent;
  std::map<std::string,unsigned int> m_rejectPattern;
  std::map<std::string,unsigned int> m_acceptPattern;
};
#endif
