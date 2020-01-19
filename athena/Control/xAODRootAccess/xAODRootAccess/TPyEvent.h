// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TPyEvent.h 649835 2015-02-26 08:19:01Z krasznaa $
#ifndef XAODROOTACCESS_TPYEVENT_H
#define XAODROOTACCESS_TPYEVENT_H

// System include(s):
#include <string>

// ROOT include(s):
#include <Rtypes.h>

// Local include(s):
#include "xAODRootAccess/TEvent.h"

namespace xAOD {

   /// Python interface to xAOD::TEvent
   ///
   /// In order to make it possible to record objects that are created in
   /// Python, into an xAOD::TEvent object (in order to record selected objects
   /// into an output file for instance), this class extends the xAOD::TEvent
   /// object with some non-template functions. Functions that are inconvenient
   /// to use from C++, but which allow for much more flexibility in PyROOT.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 649835 $
   /// $Date: 2015-02-26 09:19:01 +0100 (Thu, 26 Feb 2015) $
   ///
   class TPyEvent : public TEvent {

   public:
      /// Default constructor
      TPyEvent();

      /// Function checking if an object is available from the store
      ::Bool_t contains( const std::string& key, const std::string& type );
      /// Function checking if an object is already in memory
      ::Bool_t transientContains( const std::string& key,
                                  const std::string& type ) const;

      /// Add an output object to the event
      TReturnCode record( void* obj, const std::string& key,
                          const std::string& type,
                          ::Int_t basketSize = 32000, ::Int_t splitLevel = 0 );

   }; // class TPyEvent

} // namespace xAOD

#endif // XAODROOTACCESS_TPYEVENT_H
