///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaTool.h 
// Header file for class PyAthena::Tool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAPYTHON_PYATHENATOOL_H 
#define ATHENAPYTHON_PYATHENATOOL_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaPython/IPyComponent.h"

// Forward declaration
// Python
struct _object;
typedef _object PyObject;

namespace PyAthena {

typedef ::AthAlgTool ToolBase_t;
class Tool : virtual public ::IPyComponent,
	             public   ToolBase_t
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  Tool( const std::string& type,
	const std::string& name, 
	const IInterface* parent );

  /// Destructor: 
  virtual ~Tool(); 

  // Athena tool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  sysInitialize();
  virtual StatusCode  finalize();

  static const InterfaceID& interfaceID();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** return the @c std::type_info name of the underlying py-component
   *  This is used by concrete implementations to connect a python
   *  component to its C++ counter-part
   */
  const char* typeName() const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** @brief return associated python object. BORROWED reference.
   */ 
  virtual PyObject* self() { return m_self; }

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** attach the C++ component to its python cousin
   */
  virtual bool setPyAttr( PyObject* pyobj );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  Tool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Pointer to self (from the python world)
  PyObject* m_self;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PyAthena

#endif //> ATHENAPYTHON_PYATHENATOOL_H
