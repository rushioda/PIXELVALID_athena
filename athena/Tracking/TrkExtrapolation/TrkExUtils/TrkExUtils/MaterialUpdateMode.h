/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEXUTILS_MATERIALUPDATEMODE_H
#define TRKEXUTILS_MATERIALUPDATEMODE_H

namespace Trk {

   /** This is a steering enum to force the material update 
       it can be:
        (1)  addNoise
        (-1) removeNoise
       Second is mainly for vertex reconstruction, but potentially dangeraous.
     */

   enum MaterialUpdateMode 
   {
          addNoise    =  1,
          removeNoise = -1 
   };
   
   
}

#endif
