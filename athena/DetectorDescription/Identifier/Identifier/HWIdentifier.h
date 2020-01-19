/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//<doc><file>	$Id: HWIdentifier.h,v 1.3 2004-02-24 13:52:15 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IDENTIFIER_HWIDENTIFIER_H
# define IDENTIFIER_HWIDENTIFIER_H

#include "Identifier/Identifier.h"

class HWIdentifier : public Identifier {

public:

    /// Default constructor
    HWIdentifier ();

    /// Constructor from value_type
    explicit HWIdentifier(value_type value);
 
    /// Constructor from Identifier
    explicit HWIdentifier(const Identifier& old);
 
    /// Constructor from Identifier32 value_type (unsigned int)
    explicit HWIdentifier(Identifier32::value_type value);
    explicit HWIdentifier(int value);
};


inline HWIdentifier::HWIdentifier()
    : Identifier::Identifier()
{}

inline HWIdentifier::HWIdentifier(value_type value)
    : Identifier::Identifier(value)
{}

inline HWIdentifier::HWIdentifier(const Identifier& old)
    : Identifier::Identifier(old)
{}

inline HWIdentifier::HWIdentifier(Identifier32::value_type value)
    : Identifier::Identifier(value)
{}

inline HWIdentifier::HWIdentifier(int value)
    : Identifier::Identifier(value)
{}

#endif // IDENTIFIER_HWIDENTIFIER_H
