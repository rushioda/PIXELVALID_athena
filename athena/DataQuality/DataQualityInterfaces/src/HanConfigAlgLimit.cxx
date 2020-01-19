/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanConfigAlgLimit.cxx,v 1.5 2007-12-14 16:04:39 mgwilson Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanConfigAlgLimit.h"
#include "DataQualityInterfaces/HanUtils.h"

#include <iomanip>
#include <sstream>

#include <TObjString.h>
#include <TSeqCollection.h>

//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanConfigAlgLimit)
///\endcond


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanConfigAlgLimit::
HanConfigAlgLimit()
  : name("")
  , greenValue(0.0)
  , redValue(0.0)
{
}


HanConfigAlgLimit::
~HanConfigAlgLimit()
{
}


void
HanConfigAlgLimit::
SetName( std::string name_ )
{
  name.SetString( name_.c_str() );
}


const char*
HanConfigAlgLimit::
GetName() const
{
  return name.GetName();
}


void
HanConfigAlgLimit::
SetGreen( float value_ )
{
  greenValue = value_;
}


float
HanConfigAlgLimit::
GetGreen() const
{
  return greenValue;
}


void
HanConfigAlgLimit::
SetRed( float value_ )
{
  redValue = value_;
}


float
HanConfigAlgLimit::
GetRed() const
{
  return redValue;
}


void
HanConfigAlgLimit::
PrintIOStream( std::ostream& o ) const
{
  o << GetName() << " = { green = " << GetGreen() << ", red = " << GetRed() << " }\n";
}


TSeqCollection *
HanConfigAlgLimit::
GetList()
{
	TSeqCollection *ret = newTObjArray(this->GetName(), 0, 2);
	
  std::ostringstream greenValStr;
  greenValStr << std::setprecision(4) << this->greenValue;
	ret->Add( newTObjArray("Green",new TObjString( greenValStr.str().c_str() ), 1) );
	
  std::ostringstream redValStr;
  redValStr << std::setprecision(4) << this->redValue;
	ret->Add( newTObjArray("Red",new TObjString( redValStr.str().c_str() ), 1) );
	
	return ret;
}

} // namespace dqi

// *********************************************************************
// Private Methods
// *********************************************************************


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgLimit& l )
{
  l.PrintIOStream(o);
  return o;
}


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgLimit* l )
{
  l->PrintIOStream(o);
  return o;
}

