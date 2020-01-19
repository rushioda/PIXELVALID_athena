///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyComponentMgr.cxx 
// Implementation file for class PyComponentMgr
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// Python includes
#include "Python.h"

#include "SGTools/CLASS_DEF.h"
CLASS_DEF (PyObject, 72785480, 1)
#include "SGTools/BaseInfo.h"
SG_BASES1(PyObject, SG::NoBase);

// AthenaPython includes
#include "PyComponentMgr.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/SvcFactory.h"
#include "AthenaPython/IPyComponent.h"

// PyROOT includes
#include "TPyException.h"
#include "TPython.h"

using namespace PyAthena;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
PyComponentMgr::PyComponentMgr( const std::string& name, 
				ISvcLocator* pSvcLocator ) : 
  AthService  ( name,     pSvcLocator ),
  m_dict      ( 0 ),
  m_components(   )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty, "descr" );

}

// Destructor
///////////////
PyComponentMgr::~PyComponentMgr()
{ 
  ATH_MSG_DEBUG("Calling destructor");

  // we own the repository of instances' description
  Py_XDECREF( m_dict );

  // as well as the one of corresponding instances
  for ( PyComponents_t::iterator 
	  i    = m_components.begin(), 
	  iEnd = m_components.end();
	i != iEnd;
	++i ) {
    ATH_MSG_VERBOSE("__del__(" << i->first << ")...");
    Py_XDECREF( i->second );
  }

}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode 
PyComponentMgr::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  const std::string pyModuleName = "AthenaPython.Configurables";

  // import the module holding the dictionary of component instances
  ATH_MSG_DEBUG("Importing module [" << pyModuleName << "]...");
  PyObject* module = PyImport_ImportModule( const_cast<char*>(pyModuleName.c_str()) );
  if ( !module || !PyModule_Check( module ) ) {
    ATH_MSG_ERROR("Could not import [" << pyModuleName << "] !!");
    Py_XDECREF (module);
    throw PyROOT::TPyException();
  }

  const std::string pyClassName = "PyComponents";
  PyObject* pyClass = 0;
  pyClass = PyDict_GetItemString( PyModule_GetDict( module ),
				  const_cast<char*>( pyClassName.c_str() ) );
  
  // borrowed ref. so ->increment
  Py_XINCREF( pyClass );

  if ( !pyClass ) {
    ATH_MSG_ERROR("Could not retrieve class [" << pyClassName
                  << "] from module [" << pyModuleName << "] !!");
    Py_XDECREF(pyClass);
    Py_DECREF (module);
    return StatusCode::FAILURE;
  }

  m_dict = PyObject_GetAttrString( pyClass,
				   const_cast<char*>( "instances" ) );
  if ( !m_dict || !PyDict_Check( m_dict ) ) {
    ATH_MSG_ERROR("Could not retrieve attribute [instances] from class ["
                  << pyClassName << "] !!");
    Py_DECREF (pyClass);
    Py_DECREF (module);
    return StatusCode::FAILURE;
  }
  Py_DECREF(pyClass);

  // install the fancy attribute getter for PyComponents to automatically
  // retrieve an initialized component.
  // see bug #46668
  {
    PyObject* fancy_attr = 0;
    fancy_attr = PyDict_GetItemString(PyModule_GetDict(module),
				      (char*)"_install_fancy_attrs");
    // borrowed ref => increment
    Py_XINCREF(fancy_attr);
    if (!fancy_attr) {
      PyErr_Clear();
      ATH_MSG_INFO
        ("could not retrieve function [_install_fancy_attrs] from module ["
         << pyModuleName << "]");
    } else {
      PyObject* ret = PyObject_CallFunction(fancy_attr, NULL);
      Py_XDECREF(ret);
    }
    Py_XDECREF(fancy_attr);
  }
  Py_DECREF (module);

  // make sure the PyROOT fixes are installed
  const std::string pyRootFixes = "RootUtils.PyROOTFixes";
  ATH_MSG_DEBUG("Importing module [" << pyRootFixes << "]...");
  PyObject *rootFixes = 0;
  rootFixes = PyImport_ImportModule(const_cast<char*>(pyRootFixes.c_str()));
  if ( !rootFixes || !PyModule_Check( rootFixes ) ) {
    PyErr_Clear();
    ATH_MSG_WARNING("Could not import [" << pyRootFixes << "] !!"
                    << endmsg
                    << "Some problem may appear wit some C++->python binded classes (YMMV)");
  }
  Py_XDECREF(rootFixes);
  return StatusCode::SUCCESS;
}

StatusCode 
PyComponentMgr::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
PyComponentMgr::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IPyComponentMgr::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IPyComponentMgr*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

PyObject*
PyComponentMgr::pyObject( IPyComponent* cppComp )
{
  const std::string& name = cppComp->name();

  // Check if we already have instantiated that component
  PyComponents_t::iterator comp = m_components.find( name );
  if ( comp != m_components.end() && comp->second ) {
    Py_INCREF (comp->second);
    return comp->second;
  }
  m_components[name] = static_cast<PyObject*>( NULL );

  // Check we have a valid dict.
  if ( !m_dict || !PyDict_Check(m_dict) ) {
    ATH_MSG_ERROR("Invalid repository of Python components !!");
    Py_INCREF( Py_None );
    return Py_None;
  }
  
  PyObject* o = PyDict_GetItemString( m_dict, 
				      const_cast<char*>(name.c_str()) );
  // borrowed ref -> incr
  Py_XINCREF( o );

  // Check we have a valid dict.
  if ( !o ) {
    ATH_MSG_ERROR("No such python component [" << name
                  << "] or invalid item !!");
    Py_XDECREF( o );
    throw PyROOT::TPyException();
  }
  m_components[name] = o;

  /// Hum... remove ? or not ?
  /// leaving the objects on the python side may allow easier retrieval from
  /// py-components...
//   // remove the object from the python dict
//   if ( PyDict_DelItemString( m_dict, const_cast<char*>(name.c_str()) ) ) {
//     ATH_MSG_ERROR("Could not remove [" << name << "] from PyComponents.instances !!");
//     throw PyROOT::TPyException();
//   }

  // now we tell the PyObject which C++ object it is the cousin of.
  if ( !cppComp->setPyAttr(o) ) {
    ATH_MSG_WARNING("Could not connect the C++ object ["
                    << cppComp->typeName() << "/" << cppComp->name()
                    << "] with its python cousin !");
  }

  Py_INCREF(o);
  return o;
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
