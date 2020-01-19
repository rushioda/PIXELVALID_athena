// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_TAUSELECTIONTOOL_H
#define TAUANALYSISTOOLS_TAUSELECTIONTOOL_H

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
#include "PATCore/IAsgSelectionTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/Enums.h"
#include "TauAnalysisTools/HelperFunctions.h"

// EDM include(s):
#include "xAODEgamma/ElectronContainer.h"

// ROOT include(s):
#include "TH1F.h"

namespace TauAnalysisTools
{

/// forward declarations
class SelectionCut;
class SelectionCutPt;
class SelectionCutAbsEta;
class SelectionCutAbsCharge;
class SelectionCutNTracks;
class SelectionCutBDTJetScore;
class SelectionCutJetIDWP;
class SelectionCutBDTEleScore;
class SelectionCutEleBDTWP;
class SelectionCutEleOLR;
class SelectionCutMuonVeto;

enum SelectionCuts
{
  NoCut           = 0,   	// 00000000000
  CutPt           = 1,   	// 00000000001
  CutAbsEta       = 1<<1,	// 00000000010
  CutPhi          = 1<<2,	// 00000000100
  CutNTrack       = 1<<3,	// 00000001000
  CutAbsCharge    = 1<<4,	// 00000010000
  CutJetBDTScore  = 1<<5,	// 00000100000
  CutJetIDWP      = 1<<6,	// 00001000000
  CutEleBDTScore  = 1<<7,	// 00010000000
  CutEleBDTWP     = 1<<8,	// 00100000000
  CutMuonVeto     = 1<<9,	// 01000000000
  CutEleOLR       = 1<<10	// 10000000000
};

class TauSelectionTool : public virtual IAsgSelectionTool,
  public virtual ITauSelectionTool,
  public asg::AsgMetadataTool
{
  /// need to define cut classes to be friends to access protected variables,
  /// needed for access of cut thresholds
  friend class SelectionCut;
  friend class SelectionCutPt;
  friend class SelectionCutAbsEta;
  friend class SelectionCutAbsCharge;
  friend class SelectionCutNTracks;
  friend class SelectionCutBDTJetScore;
  friend class SelectionCutJetIDWP;
  friend class SelectionCutBDTEleScore;
  friend class SelectionCutEleBDTWP;
  friend class SelectionCutEleOLR;
  friend class SelectionCutMuonVeto;

  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( TauSelectionTool,
                   IAsgSelectionTool,
                   TauAnalysisTools::ITauSelectionTool )

  // declaration of classes as friends to access private member variables
  friend class TauEfficiencyCorrectionsTool;

public:
  /// Constructor for standalone usage
  TauSelectionTool( const std::string& name );

  virtual ~TauSelectionTool();

  /// Function initialising the tool
  virtual StatusCode initialize();

  /// Function initialising the tool
  virtual StatusCode initializeEvent() __attribute__ ((deprecated("This function is deprecated. Please remove it from your code.\nFor further information please refer to the README:\nhttps://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-TauSelectionTool.rst")));

  /// Get an object describing the "selection steps" of the tool
  virtual const Root::TAccept& getTAccept() const;

  /// Get the decision using a generic IParticle pointer
  virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;

  /// Get the decision for a specific TauJet object
  virtual const Root::TAccept& accept( const xAOD::TauJet& tau ) const;

  /// Set output file for control histograms
  void setOutFile( TFile* fOutFile );

  /// Write control histograms to output file
  void writeControlHistograms();

private:

  // Execute at each event
  virtual StatusCode beginEvent();

  template<typename T, typename U>
  void FillRegionVector(std::vector<T>& vRegion, U tMin, U tMax);
  template<typename T, typename U>
  void FillValueVector(std::vector<T>& vRegion, U tVal);
  template<typename T>
  void PrintConfigRegion(std::string sCutName, std::vector<T>& vRegion);
  template<typename T>
  void PrintConfigValue(std::string sCutName, std::vector<T>& vRegion);
  template<typename T>
  void PrintConfigValue(std::string sCutName, T& sVal);

  // bitmask of tau selection cuts
  int m_iSelectionCuts;
  // vector of transverse momentum cut regions
  std::vector<float> m_vPtRegion;
  // vector of absolute eta cut regions
  std::vector<float> m_vAbsEtaRegion;
  // vector of absolute charge requirements
  std::vector<int> m_vAbsCharges;
  // vector of number of track requirements
  std::vector<size_t> m_vNTracks;
  // vector of JetBDT cut regions
  std::vector<float> m_vJetBDTRegion;
  // JetID working point
  std::string m_sJetIDWP;
  int m_iJetIDWP;
  // vector of EleBDT cut regions
  std::vector<float> m_vEleBDTRegion;
  // EleBDT working point
  std::string m_sEleBDTWP;
  int m_iEleBDTWP;
  // do electron overlap removal
  bool m_bEleOLR;
  // do muon veto
  bool m_bMuonVeto;

  float m_dPtMin;
  float m_dPtMax;
  float m_dAbsEtaMin;
  float m_dAbsEtaMax;
  float m_iAbsCharge;
  float m_iNTrack;
  float m_dJetBDTMin;
  float m_dJetBDTMax;
  float m_dEleBDTMin;
  float m_dEleBDTMax;

protected:
  TFile* m_fOutFile;//!
  std::shared_ptr<TH1F> m_hCutFlow;//!

private:
  std::string m_sConfigPath;
  std::string m_sEleOLRFilePath;
  std::string m_sCuts;
  std::string m_sElectronContainerName;

  const xAOD::ElectronContainer* m_xElectronContainer;

  std::map<SelectionCuts, TauAnalysisTools::SelectionCut*> m_cMap;

  void setupCutFlowHistogram();
  int convertStrToJetIDWP(std::string sJetIDWP);
  int convertStrToEleBDTWP(std::string sEleBDTWP);
  std::string convertJetIDWPToStr(int iJetIDWP);
  std::string convertEleBDTWPToStr(int iEleBDTWP);

protected:
  bool m_bCreateControlPlots;

  /// Object used to store the last decision
  mutable Root::TAccept m_aAccept;



}; // class TauSelectionTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_TAUSELECTIONTOOL_H
