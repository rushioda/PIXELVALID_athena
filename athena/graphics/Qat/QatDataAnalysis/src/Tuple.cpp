//---------------------------------------------------------------------------//
//                                                                           //
// This file is part of the Pittsburgh Visualization System (PVS)            //
//                                                                           //
// Copyright (C) 2004 Joe Boudreau, University of Pittsburgh                 //
//                                                                           //
//  This program is free software; you can redistribute it and/or modify     //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation; either version 2 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  This program is distributed in the hope that it will be useful,          //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with this program; if not, write to the Free Software              //
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307     //
// --------------------------------------------------------------------------//


#include "QatDataAnalysis/Tuple.h"
#include <iomanip>
#include <stdexcept>

std::ostream & Tuple::print (std::ostream & o) const {
 
  
  for (unsigned int i=0; i<_attributeList->size();i++) {
    o << std::setw(10);

    {
      int value=0;
      if (this->read(value,i)) {
	o << value << " "; continue;
      }
    }
      

    {
      double value=0;
      if (this->read(value,i)) {
	o << value << " "; continue;
      }
    }

    {
      float value=0;
      if (this->read(value,i)) {
	o << value << " "; continue;
      }
    }

    {
      unsigned int value=0;
      if (this->read(value,i)) {
	o << value << " "; continue;
      }
    }
      

    o << (*_attributeList)[i].typeName() << "(?)" ;
  }
  o << std::endl;
  return o;
}



const Genfun::Argument & Tuple::asDoublePrec() const {
  
  // No space has ever been allocated.  Allocate it!
  if (!_doubleRep) {
    _doubleRep = new Genfun::Argument(_attributeList->size());
    // just to be sure:
    _doubleRepStale=true;
  }

  if (_doubleRepStale) {
    Genfun::Argument &a = *_doubleRep;
    
    unsigned int nAttr=_attributeList->size();
    for (unsigned int i=0;i<nAttr;i++) {
      switch ((*_attributeList)[i].type()) {
      case Attribute::DOUBLE:
	{
	  fastread(a[i],i);
	  break;
	}
      case Attribute::FLOAT:
	{
	  float value;
	  fastread(value,i);
	  a[i]=value;
	  break;
	}
      case Attribute::INT:
	{
	  int value;
	  fastread(value,i);
	  a[i]=value;
	  break;
	}
      case Attribute::UINT:
	{
	  unsigned int value;
	  fastread(value,i);
	  a[i]=value;
	  break;
	}
      default:
	{
	  a[i]=0;
	}
      }
    }
    _doubleRepStale=false;
  }
  return *_doubleRep;
}

