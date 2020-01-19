// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUOVERLAPPINGELECTRONLLHDECORATOR_H
#define TAUANALYSISTOOLS_TAUOVERLAPPINGELECTRONLLHDECORATOR_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"

// Local include(s):
#include "TauAnalysisTools/ITauOverlappingElectronLLHDecorator.h"

// ROOT include(s):
#include "TH2D.h"

namespace TauAnalysisTools
{

class SelectionCutEleOLR;

class TauOverlappingElectronLLHDecorator
  : public virtual ITauOverlappingElectronLLHDecorator
  , public asg::AsgMetadataTool
{

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauOverlappingElectronLLHDecorator,
                  TauAnalysisTools::ITauOverlappingElectronLLHDecorator )

  friend class SelectionCutEleOLR;

public:
  /// Create a constructor for standalone usage
  TauOverlappingElectronLLHDecorator( const std::string& name );

  ~TauOverlappingElectronLLHDecorator( );

  /// Function initialising the tool
  virtual StatusCode initialize();

  virtual StatusCode initializeEvent() __attribute__ ((deprecated("This function is deprecated. Please remove it from your code.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauOverlappingElectronLLHDecorator.rst")));

  virtual StatusCode decorate(const xAOD::TauJet& xTau);

private:
  AsgElectronLikelihoodTool* m_tEMLHTool;
  const xAOD::ElectronContainer* m_xElectronContainer;
  std::string m_sElectronContainerName;
  bool m_bElectonsAvailable;
  std::string m_sEleOLRFilePath;
  TH2D* m_hCutValues;

private:
  float getCutVal(float fEta, float fPt);
  StatusCode retrieveElectrons();
  virtual StatusCode beginEvent();

private:
  bool m_bEleOLRMatchAvailable;
  bool m_bEleOLRMatchAvailableChecked;
  bool m_bNewEvent;

  std::string m_sElectronPhotonSelectorToolsConfigFile;

}; // class TauOverlappingElectronLLHDecorator

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUOVERLAPPINGELECTRONLLHDECORATOR_H
