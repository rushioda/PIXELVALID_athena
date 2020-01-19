/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  MetSort.h
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#ifndef TCS__MetSort
#define TCS__MetSort

#include "L1TopoInterfaces/SortingAlg.h"
#include "L1TopoEvent/TOBArray.h"

#include <iostream>
#include <vector>

namespace TCS {
   
   class MetSort : public SortingAlg {
   public:
      
      // constructor
      MetSort(const std::string & name);

      // destructor
      virtual ~MetSort();

      virtual StatusCode initialize();

      virtual TCS::StatusCode sortBitCorrect(const InputTOBArray & input, TOBArray & output);    
      virtual TCS::StatusCode sort(const InputTOBArray & input, TOBArray & output);



   };

} // end of namespace TCS

#endif /* defined(__TopoCore__SortingAlg__) */
