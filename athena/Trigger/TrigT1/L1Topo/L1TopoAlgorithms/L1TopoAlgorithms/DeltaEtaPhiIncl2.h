/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DeltaEtaPhiIncl2.h
//  TopoCore
//  Created by Joerg Stelzer on 11/16/12.

#ifndef __TopoCore__DeltaEtaPhiIncl2__
#define __TopoCore__DeltaEtaPhiIncl2__

#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"

namespace TCS {
   
   class DeltaEtaPhiIncl2 : public DecisionAlg {
   public:
      DeltaEtaPhiIncl2(const std::string & name);
      virtual ~DeltaEtaPhiIncl2();

      virtual StatusCode initialize();

      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison );
      

   private:

      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_NumberLeading2 = { 0 };
      parType_t      p_DeltaPhiMin[3] = {0, 0, 0};
      parType_t      p_DeltaPhiMax[3] = {0, 0, 0};
      parType_t      p_DeltaEtaMin[3] = {0, 0, 0};
      parType_t      p_DeltaEtaMax[3] = {0, 0, 0};
      parType_t      p_MinET1[3] = { 0,0,0 };
      parType_t      p_MinET2[3] = { 0,0,0 };

 
   };
   
}

#endif
