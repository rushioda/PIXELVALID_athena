///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HistAlg.h 
// Header file for class HistAlg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef HISTOGRAMUTILS_HISTALG_H
#define HISTOGRAMUTILS_HISTALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

class IAthHistogramTool;


class HistAlg
  : public ::AthHistogramAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  HistAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~HistAlg(); 

  // Assignment operator: 
  //HistAlg &operator=(const HistAlg &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  ToolHandleArray<IAthHistogramTool> m_histToolList;

  /// Default constructor: 
  HistAlg();

  /// Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !HISTOGRAMUTILS_HISTALG_H