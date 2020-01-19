//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <AsgTools/MessagePrinterErrorCollect.h>

#include <regex>

//
// method implementations
//

#ifdef ROOTCORE

namespace asg
{
  void MessagePrinterErrorCollect ::
  print (MSG::Level reglvl, const std::string& /*name*/,
         const std::string& text)
  {
    if (reglvl >= MSG::ERROR)
    {
      m_errorMessages.push_back (text);
    }
  }



  bool MessagePrinterErrorCollect ::
  empty () const noexcept
  {
    return m_errorMessages.empty();
  }



  bool MessagePrinterErrorCollect ::
  matchesRegex (const std::string& pattern) const
  {
    std::regex regex (pattern);
    for (auto& msg : m_errorMessages)
    {
      if (std::regex_search (msg, regex))
        return true;
    }
    return false;
  }



  std::string MessagePrinterErrorCollect ::
  asString (const std::string& prefix) const
  {
    std::string result;
    for (auto& msg : m_errorMessages)
    {
      if (!result.empty())
        result += "\n";
      result += prefix + msg;
    }
    return result;
  }
}

#endif
