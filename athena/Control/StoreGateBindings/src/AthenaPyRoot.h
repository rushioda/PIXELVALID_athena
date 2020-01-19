// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATEBINDINGS_ATHENAPYROOT_H
#define STOREGATEBINDINGS_ATHENAPYROOT_H 1

#include "Python.h"

// PyROOT includes
#include <TPython.h>
#include <TPyException.h>
#ifndef ROOT_TPyException
# define ROOT_TPyException 1 /* there was a typo in TPyException-v20882 */
#endif

// fixes 'dereferencing type-punned pointer will break strict-aliasing rules'
#ifdef Py_True
#undef Py_True
#define Py_True ( (PyObject*)(void*)&_Py_TrueStruct )
#endif
#ifdef Py_False
#undef Py_False
#define Py_False ( (PyObject*)(void*)&_Py_ZeroStruct )
#endif

#define ObjectProxy_ASVOIDPTR(o) (TPython::ObjectProxy_AsVoidPtr(o))

namespace PyROOT {

inline
void throw_py_exception (bool display = true)
{
  if (display) {
    // fetch error
    PyObject* pytype = 0, *pyvalue = 0, *pytrace = 0;
    PyErr_Fetch (&pytype, &pyvalue, &pytrace);
    Py_XINCREF  (pytype);
    Py_XINCREF  (pyvalue);
    Py_XINCREF  (pytrace);
    // restore...
    PyErr_Restore (pytype, pyvalue, pytrace);
    // and print
    PyErr_Print();
  }
  throw PyROOT::TPyException();
}

} //> namespace PyROOT

#endif //> STOREGATEBINDINGS_ATHENAPYROOT_H
