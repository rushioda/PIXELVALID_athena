///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IAsgSelectionTool.h 
// Header file for class IAsgSelectionTool
// Author: Karsten Koeneke <karsten.koeneke@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PATCORE_IASGSELECTIONTOOL_H
#define PATCORE_IASGSELECTIONTOOL_H 1

// STL includes


// FrameWork includes
#include "AsgTools/IAsgTool.h"

// Include the return object
#include "PATCore/TAccept.h"

// Forward declaration
namespace xAOD{
  class IParticle;
}


//static const InterfaceID IID_IAsgSelectionTool("IAsgSelectionTool", 1, 0);

class IAsgSelectionTool
  : virtual public asg::IAsgTool
{ 
  /// Declare the interface ID for this pure-virtual interface class to the Athena framework
  ASG_TOOL_INTERFACE(IAsgSelectionTool)


  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method to get the plain TAccept.
      This is needed so that one can already get the TAccept 
      and query what cuts are defined before the first object 
      is passed to the tool. */
  virtual const Root::TAccept& getTAccept( ) const = 0;


  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::IParticle* /*part*/ ) const = 0;


}; 

#endif //> !PATCORE_IASGSELECTIONTOOL_H
