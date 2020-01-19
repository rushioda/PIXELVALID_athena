///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ICallGraphBuilderSvc.h 
// Header file for class ICallGraphBuilderSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONKERNEL_ICALLGRAPHBUILDERSVC_H 
#define PERFMONKERNEL_ICALLGRAPHBUILDERSVC_H 

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IInterface.h"

// Forward declaration

class ICallGraphBuilderSvc : virtual public IInterface
{ 
 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~ICallGraphBuilderSvc();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// open a new node in the call graph tree
  virtual void openNode( const std::string& nodeName ) = 0;

  /// close an existing node in the call graph tree
  virtual void closeNode( const std::string& nodeName ) = 0;

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& ICallGraphBuilderSvc::interfaceID() 
{ 
  static const InterfaceID 
    IID_ICallGraphBuilderSvc("ICallGraphBuilderSvc", 1, 0);
  return IID_ICallGraphBuilderSvc; 
}

#endif //> PERFMONKERNEL_ICALLGRAPHBUILDERSVC_H