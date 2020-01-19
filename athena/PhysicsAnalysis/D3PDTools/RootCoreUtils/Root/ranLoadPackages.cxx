//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/ranLoadPackages.h>

#include <RootCoreUtils/Assert.h>
#include <TROOT.h>

//
// method implementations
//

namespace RCU
{
  bool ranLoadPackages ()
  {
    return gROOT->Get ("load_packages_run") != nullptr;
  }
}
