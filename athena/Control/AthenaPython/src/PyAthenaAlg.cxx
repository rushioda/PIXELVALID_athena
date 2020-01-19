///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyAthenaAlg.cxx 
// Implementation file for class PyAthena::Alg
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Python includes
#include "Python.h"

// PyROOT includes
#include "TPython.h"

// AthenaPython includes
#include "AthenaPython/PyAthenaUtils.h"
#include "AthenaPython/PyAthenaAlg.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaPython/IPyComponentMgr.h"

namespace PyAthena {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
Alg::Alg( const std::string& name, ISvcLocator* svcLocator ) :
  AlgBase_t( name, svcLocator ),
  m_self   ( 0 )
{}

// Destructor
///////////////
Alg::~Alg()
{ 
  ATH_MSG_DEBUG("Calling destructor");
  Py_XDECREF( m_self );
}

// Framework's Hooks
////////////////////////////
StatusCode 
Alg::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysInitialize" );
}

StatusCode 
Alg::reinitialize()
{
  ATH_MSG_INFO("Re-Initializing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysReinitialize" );
}

StatusCode 
Alg::beginRun()
{ 
  return PyAthena::callPyMethod( m_self, "sysBeginRun" );
}

StatusCode 
Alg::endRun()
{
  return PyAthena::callPyMethod( m_self, "sysEndRun" );
}

StatusCode 
Alg::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysFinalize" );
}

StatusCode 
Alg::execute()
{  
//   ATH_MSG_DEBUG("Executing " << name() << "...");
  return PyAthena::callPyMethod( m_self, "sysExecute" );
}

void 
Alg::resetExecuted()
{  
  PyAthena::callPyMethod( m_self, "resetExecuted" ).ignore();
}

StatusCode
Alg::sysInitialize()
{
  ServiceHandle<IPyComponentMgr> pyMgr
    ( "PyAthena::PyComponentMgr/PyComponentMgr", name() );
  if ( !pyMgr.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Could not retrieve service [" << pyMgr.typeAndName()
                  << "] !!");
    return StatusCode::FAILURE;
  }

  // first retrieve our python object cousin...
  m_self = pyMgr->pyObject( this );

  if ( m_self == Py_None ) {
    ATH_MSG_ERROR("Wrapped PyObject is NONE !");
    return StatusCode::FAILURE;
  }

  // re-route to usual sysInit...
  return AlgBase_t::sysInitialize();
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

const char*
Alg::typeName() const
{ 
  static const std::string tname = System::typeinfoName(typeid(*this));
  return tname.c_str();
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

bool
Alg::setPyAttr( PyObject* o )
{
  // now we tell the PyObject which C++ object it is the cousin of.
  PyObject* pyobj = TPython::ObjectProxy_FromVoidPtr
    ( (void*)this, this->typeName() );
  if ( !pyobj ) {
    PyErr_Clear();
    // try PyAthena::Alg
    pyobj = TPython::ObjectProxy_FromVoidPtr ((void*)this, "PyAthena::Alg");
    ATH_MSG_INFO
      ("could not dyncast component [" << name() << "] to a python "
       << "object of type [" << this->typeName() << "] (probably a missing "
       << "dictionary)" << endmsg
       << "fallback to [PyAthena::Alg]...");
  }
  if ( !pyobj ) {
    PyErr_Clear();
    ATH_MSG_WARNING("Could not dyncast component ["
                    << name() << "] to a pyobject of type [" 
                    << this->typeName() << "]");
  } else {
    if ( -1 == PyObject_SetAttrString(o, "_cppHandle", pyobj) ) {
      PyErr_Clear();
      ATH_MSG_WARNING
        ("Could not attach C++ handle [" << name() << "] to its python "
         << "cousin !");
      if ( -1 == PyObject_SetAttrString(o, "_cppHandle", Py_None) ) {
        PyErr_Clear();
        ATH_MSG_WARNING
          ("could not attach a dummy C++ handle [" << name() << "] to its "
           "python cousin !");
      }
    } else {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

} //> end namespace PyAthena
