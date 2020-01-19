/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "L1TopoCommon/StatusCode.h"

#include <vector>
#include <map>
#include <string>

namespace TXC {
   class L1TopoMenu;
}

namespace TCS {

   class Connector;
   class DecisionConnector;
   class SortingConnector;
   class InputConnector;
   class ParameterSpace;
//    class DecisionAlg;
//    class SortingAlg;

   class TopoSteeringStructure {
   public:

      TopoSteeringStructure();

      ~TopoSteeringStructure();

      StatusCode setupFromMenu(const TXC::L1TopoMenu& menu, bool debug = false);

      // accessors
      bool isConfigured() const { return m_isConfigured; }

      const std::vector<TCS::Connector*> & connectors() const { return m_connectors; }

      const std::map<std::string, TCS::DecisionConnector*> & outputConnectors() const { return m_outputLookup; }

      Connector* connector(const std::string & connectorName) const;

      DecisionConnector* outputConnector(const std::string & output) const;

      // resets the connectors (status, intermediate TOBs, and decision of algs)
      StatusCode reset();

      // print
      void print(std::ostream & o) const;

      void printParameters(std::ostream & o) const;

   private:

      SortingConnector* sortingConnector(const std::string & output) const;

      StatusCode addDecisionConnector(DecisionConnector * conn);

      StatusCode addSortingConnector(SortingConnector * conn);

      StatusCode linkConnectors();

      StatusCode instantiateAlgorithms(bool debug);

   private:

      bool m_isConfigured  { false };     // set to true after configuration has run

      std::vector<TCS::Connector*> m_connectors; // list of connectors
      
      std::map<std::string, TCS::DecisionConnector*> m_outputLookup; // output connectors (subset of m_connectors) by connector name

      std::map<std::string, TCS::SortingConnector*> m_sortedLookup; // sorting connectors (subset of m_connectors) by connector name

      std::map<std::string, TCS::InputConnector*> m_inputLookup; // input connectors (subset of m_connectors) by connector name

      std::vector<TCS::ParameterSpace*> m_parameters;
      
   };

}
