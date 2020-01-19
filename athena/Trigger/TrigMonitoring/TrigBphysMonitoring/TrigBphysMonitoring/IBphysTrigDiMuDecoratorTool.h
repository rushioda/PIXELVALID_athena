///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IBphysTrigDiMuDecoratorTool.h 
// Header file for class IBphysTrigDiMuDecoratorTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGBPHYSMONITORING_IBPHYSTRIGDIMUDECORATORTOOL_H
#define TRIGBPHYSMONITORING_IBPHYSTRIGDIMUDECORATORTOOL_H 1

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/IAlgTool.h"

// Forward declaration
#include "xAODTracking/VertexFwd.h"


static const InterfaceID IID_IBphysTrigDiMuDecoratorTool("IBphysTrigDiMuDecoratorTool", 1, 0);

class IBphysTrigDiMuDecoratorTool
  : virtual public ::IAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IBphysTrigDiMuDecoratorTool();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////
  static const InterfaceID& interfaceID();

    virtual StatusCode decorateVertex(const xAOD::Vertex* vtx, const xAOD::Vertex * pv=nullptr) const =0;
    
  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline const InterfaceID& IBphysTrigDiMuDecoratorTool::interfaceID() 
{ 
   return IID_IBphysTrigDiMuDecoratorTool; 
}


#endif //> !TRIGBPHYSMONITORING_IBPHYSTRIGDIMUDECORATORTOOL_H
