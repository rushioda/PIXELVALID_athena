/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanConfigParMap.cxx,v 1.1 2008-09-10 11:40:09 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanConfigParMap.h"

#include <iomanip>
#include <sstream>

#include <TObjString.h>


//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanConfigParMap)
///\endcond


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanConfigParMap::
HanConfigParMap()
  : name("")
  , value("")
{
}

HanConfigParMap::
HanConfigParMap(const HanConfigParMap& other)
  : TObject()
  , name(other.name)
  , value(other.value)
{
}


HanConfigParMap::
~HanConfigParMap()
{
}


void
HanConfigParMap::
SetName( const std::string name_ )
{
  name.SetString( name_.c_str() );
}


const char*
HanConfigParMap::
GetName() const
{
  return name.GetName();
}


void
HanConfigParMap::
SetValue( const std::string value_ )
{
  value.SetString( value_.c_str() );
}


const char*
HanConfigParMap::
GetValue() const
{
  return value.GetName();
}


void
HanConfigParMap::
PrintIOStream( std::ostream& o ) const
{
  o << GetName() << " = " << GetValue() << "\n";
}

TList *
HanConfigParMap::
GetList()
{
	TList *ret = new TList();
	
	ret->SetName( this->GetName() );
	ret->Add( new TObjString(value.GetName()) );
	
	return ret;
}

} // namespace dqi


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigParMap& p )
{
  p.PrintIOStream(o);
  return o;
}


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigParMap* p )
{
  p->PrintIOStream(o);
  return o;
}

