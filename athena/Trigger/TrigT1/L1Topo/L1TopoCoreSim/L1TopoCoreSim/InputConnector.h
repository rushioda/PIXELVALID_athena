/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  Connector.h
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 11/20/12.

#ifndef L1TopoCoreSimulation_InputConnector
#define L1TopoCoreSimulation_InputConnector

#include "L1TopoCoreSim/Connector.h"

#include <iostream>
#include <vector>
#include <string>

#include "L1TopoCommon/StatusCode.h"
#include "L1TopoCommon/Types.h"
#include "L1TopoInterfaces/Decision.h"


namespace TCS {

   class InputTOBArray;

   // special connector for topo input
   class InputConnector : public Connector {
   public:
      
      InputConnector(const std::string & name);

      virtual ~InputConnector();

      inputTOBType_t inputTOBType() const { return m_inputTOBType; }

      void attachOutputData(const InputTOBArray* data);

      const InputTOBArray* outputData() const { return m_outputData; }

      StatusCode clearOutput();

   private:

      inputTOBType_t m_inputTOBType;  // holds the type of inputData

      // attached input data
      InputTOBArray const * m_outputData { nullptr };

   };
   
std::ostream & operator<<(std::ostream &, const TCS::InputConnector&);

}

#endif
