// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Message.h 567323 2013-10-27 14:49:47Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_MESSAGE_H
#define XAODROOTACCESS_TOOLS_MESSAGE_H

/// Simple macro for printing error/verbose messages
///
/// This little macro is used in some places in the code to print verbose and
/// error messages with information on where they are coming from.
///
/// @param MESSAGE The message as it is normally given to the ROOT messaging
///        functions
///
#define XAOD_MESSAGE( MESSAGE )                 \
   "%s:%i " MESSAGE, __FILE__, __LINE__

#endif // XAODROOTACCESS_TOOLS_MESSAGE_H
