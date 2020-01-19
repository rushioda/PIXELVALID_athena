/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/Root/normalizedTypeinfoName.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Convert a @c type_info to a normalized string representation
 *        (matching the names used in the root dictionary).
 */


#include "AthContainers/normalizedTypeinfoName.h"
#include "AthContainers/tools/threading.h"
#include "AthContainers/tools/error.h"
#include "CxxUtils/ClassName.h"


#if __cplusplus < 201100
# include "CxxUtils/unordered_map.h"
namespace SG_STD_OR_SG = SG;
#else
# include <unordered_map>
# include <functional>
namespace SG_STD_OR_SG = std;
#endif


namespace {


typedef SG_STD_OR_SG::unordered_map<const std::type_info*, std::string>
  typemap_t;

typedef AthContainers_detail::upgrade_mutex mutex_t;
typedef AthContainers_detail::strict_shared_lock<mutex_t> lock_t;
typedef AthContainers_detail::upgrading_lock<mutex_t> upgrading_lock_t;


void initRules (CxxUtils::ClassName::Rules& rules)
{
  // STL container types that Reflex knows about.
  rules.add ("std::deque<$T, std::allocator<$T> >",
             "std::deque<$T>");
  rules.add ("std::list<$T, std::allocator<$T> >",
             "std::list<$T>");
  rules.add ("std::vector<$T, std::allocator<$T> >",
             "std::vector<$T>");
  rules.add ("std::map<$KEY, $T, std::less<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::map<$KEY, $T>");
  rules.add ("std::multimapmap<$KEY, $T, std::less<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::multimapmap<$KEY, $T>");
  rules.add ("std::set<$KEY, std::less<$KEY>, std::allocator<$KEY> >",
             "std::set<$KEY>");
  rules.add ("std::multiset<$KEY, std::less<$KEY>, std::allocator<$KEY> >",
             "std::multiset<$KEY>");
  rules.add ("std::unordered_map<$KEY, $T, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::unordered_map<$KEY, $T>");
  rules.add ("std::unordered_multimap<$KEY, $T, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<std::pair<const $KEY, $T> > >",
             "std::unordered_multimap<$KEY, $T>");
  rules.add ("std::unordered_set<$KEY, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<$KEY> >",
             "std::unordered_set<$KEY>");
  rules.add ("std::unordered_multiset<$KEY, std::hash<$KEY>, std::equal_to<$KEY>, std::allocator<$KEY> >",
             "std::unordered_multiset<$KEY>");
  rules.add ("std::queue<$T, std::deque<$T> >",
             "std::queue<$T>");
  rules.add ("std::stack<$T, std::deque<$T> >",
             "std::stack<$T>");

  // These container types are also defined in the C++11 standard,
  // but root5 reflex doesn't know about them.  List them here anyway.
  rules.add ("std::forward_list<$T, std::allocator<$T> >",
             "std::forward_list<$T>");
  rules.add ("std::priority_queue<$T, std::vector<$T>, std::less<$T> >",
             "std::priority_queue<$T>");

  // Reflex also handles the non-standard containers hash_map, etc.
  // Don't think atlas is using those anywhere, so skip that.

  // Reflex handles the basic_string template.
  // I actually think that demangling will produce `std::string' rather
  // than `std::basic_string' with default arguments; however, add it
  // for completeness.
  rules.add ("std::basic_string<$T, std::char_traits<$T>, std::allocator<$T> >",
             "std::string");

  // Atlas-specific mappings.
  rules.add ("DataVector<$T, $U>", "DataVector<$T>");
  rules.add ("DataList<$T, $U>",   "DataList<$T>");

  // Handle gcc's C++11 ABI.
  rules.add ("std::__cxx11", "std");

  // Needed for macos?
  rules.add ("std::__1", "std");
}


} // anonymous namespace


namespace SG {


/**
 * @brief Convert a @c type_info to a normalized string representation
 *        (matching the names used in the root dictionary).
 * @param info The type to convert.
 *
 * The function `AthContainer_detail::typeinfoName` may be used to convert
 * a C++ `type_info` to a string representing the name of the class; this
 * handles platform-dependent details such as performing demangling.
 *
 * However, the name you get as a result of this does not necessarily match
 * the name by which the class is known in the ROOT dictionary.
 * In particular, defaulted template arguments for STL containers and
 * @c DataVector are suppressed in the dictionary.  So, for example,
 * for a vector class @c typeinfoName may produce
 * `std::vector<int, std::allocator<T> >`, while in the dictionary
 * it is known as `std::vector<int>`.  Using @c normalizedTypeinfoName
 * instead will transform the names to match what's in the dictionary.
 * This function will also cache the typeinfo -> string conversions.
 */
std::string normalizedTypeinfoName (const std::type_info& info)
{
  static typemap_t normalizedTypemap;
  static mutex_t normalizedTypemapMutex;

  // First test to see if we already have this mapping.
  // For this, use a read lock.
  {
    lock_t lock (normalizedTypemapMutex);
    typemap_t::iterator it = normalizedTypemap.find (&info);
    if (it != normalizedTypemap.end())
      return it->second;
  }

  static CxxUtils::ClassName::Rules normalizeRules;

  // Didn't find it.  Take out an upgrading lock.
  {
    upgrading_lock_t lock (normalizedTypemapMutex);

    // Need to check again to see if the mapping's there.
    typemap_t::iterator it = normalizedTypemap.find (&info);
    if (it != normalizedTypemap.end())
      return it->second;

    // Convert lock to exclusive.
    lock.upgrade();

    if (normalizeRules.size() == 0)
      initRules (normalizeRules);

    std::string tiname = AthContainers_detail::typeinfoName (info);
    std::string normalizedName = normalizeRules.apply (tiname);
    normalizedTypemap[&info] = normalizedName;
    return normalizedName;
  }
}


} // namespace SG
