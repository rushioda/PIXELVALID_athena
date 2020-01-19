/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage

This package contains code used to work around various problems
with Root.  It may also contain small pieces of Root code of
general utility.

This package should have no dependencies other than root/python (and the build
system).

Here's a brief list of what's here.

 - RootUtils::ClearCINTMessageCallback: Work around an issue where importing
   PyCintex will disable error messages if you're using CINT as a shell.

 - RootUtils::DataVectorConvert: Provide backwards compatibility for
   reading DataVector classes for which inheritance relations have changed.

 - RootUtils::InitHist: Force initialization of Root's getline command
   history, to prevent crashes in TTreeViewer when using python as a shell.

 - RootUtils::PyROOTConstFix: Work around PyROOT const/non-const method
   return argument conversion bug.

 - RootUtils::PyROOTTTreePatch: Work around performance problems with
   PyROOT's pythonization of TTree.

 - RootUtils::ScanForAbstract: Work around problems caused by Root's
   inability to correctly represent virtual bases in TClass.

 - RootUtils::StdHackGenerator: Work around inconsistent use of std::
   with Root's autoloading.

 - RootUtils.PyROOTFixes (.py): Work around PyROOT bugs that affect DataVector.

Dictionaries are generated for all C++ classes, so they're callable
from Python.

*/
