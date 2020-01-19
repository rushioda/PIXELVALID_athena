// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTUTILS_PYROOTINSPECTOR_H
#define ROOTUTILS_PYROOTINSPECTOR_H 1

#ifdef _POSIX_C_SOURCE
# undef _POSIX_C_SOURCE
#endif
#include "Python.h"

namespace RootUtils {

class PyROOTInspector
{
 public:
  static
  PyObject* pyroot_inspect(PyObject* obj,
                           bool persistentOnly = false);

  static
  PyObject* pyroot_inspect2(PyObject *obj, 
                            PyObject *obj_name,
                            bool persistentOnly = false);

};

} // namespace RootUtils

#endif // not ROOTUTILS_PYROOTINSPECTOR_H
