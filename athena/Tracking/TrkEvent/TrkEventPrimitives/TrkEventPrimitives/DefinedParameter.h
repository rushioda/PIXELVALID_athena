/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DefinedParameter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_DEFINEDPARAMETER_H
#define TRKEVENTPRIMITIVES_DEFINEDPARAMETER_H

//Trk
#include "TrkEventPrimitives/ParamDefs.h"
// STL
#include <utility>

/** standard namespace for Tracking*/
namespace Trk {

  /** @typedef  DefinedParameter
    
    Typedef to of a std::pair<double, ParamDefs>
    to identify a passed-through double as a specific type of parameter
    
    @author Andreas.Salzburger@cern.ch 
  */
  typedef std::pair<double, ParamDefs> DefinedParameter;

}

#endif // TRKEVENTPRIMITIVES_DEFINEDPARAMETER_H
