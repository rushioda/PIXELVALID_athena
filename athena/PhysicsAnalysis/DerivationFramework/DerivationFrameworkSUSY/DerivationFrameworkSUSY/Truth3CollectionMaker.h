/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Truth3CollectionMaker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_TRUTH3COLLECTIONMAKER_H
#define DERIVATIONFRAMEWORK_TRUTH3COLLECTIONMAKER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace ExpressionParsing {
  class ExpressionParser;
}
class IMCTruthClassifier;

namespace DerivationFramework {

  class Truth3CollectionMaker : public AthAlgTool, public IAugmentationTool {
    public: 
      Truth3CollectionMaker(const std::string& t, const std::string& n, const IInterface* p);
      ~Truth3CollectionMaker();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      ExpressionParsing::ExpressionParser *m_vertParser;
      ExpressionParsing::ExpressionParser *m_partParser;
      mutable unsigned int m_ntotpart, m_npasspart;
      std::string m_particlesKey;
      //std::string m_verticesKey;
      std::string m_collectionName; 
      std::string m_partString;
      ToolHandle<IMCTruthClassifier> m_classifier;
      bool m_runClassifier;
  }; 
}

#endif // DERIVATIONFRAMEWORK_TRUTH3COLLECTIONMAKER_H
