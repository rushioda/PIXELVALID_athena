/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaxCellDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_MAXCELLDECORATOR_H
#define DERIVATIONFRAMEWORK_MAXCELLDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CaloIdentifier/CaloIdManager.h"

class LArCablingService;

namespace DerivationFramework {

  class MaxCellDecorator : public AthAlgTool, public IAugmentationTool {
  public:
    MaxCellDecorator(const std::string& t, const std::string& n, const IInterface* p);
    ~MaxCellDecorator();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;
    
  private:
    std::string m_SGKey_photons;
    std::string m_SGKey_electrons;
    
    ToolHandle<LArCablingService>  m_larCablingSvc;

    void decorateObject(const xAOD::Egamma*& egamma) const;
  };
}

#endif // DERIVATIONFRAMEWORK_MAXCELLDECORATOR_H
