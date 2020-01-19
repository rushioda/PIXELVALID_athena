//        Copyright Iowa State University 2016.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/UnitTest.h>

#include <boost/regex.hpp>

//
// method implementations
//

namespace asg
{
  namespace detail
  {
    bool matchesRegex (const std::string& regex, const std::string& str)
    {
      return regex_search (str, boost::regex (regex));
    }
  }
}
