// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonSelectionTool.h 299883 2014-03-28 17:34:16Z krasznaa $
#ifndef MUONSELECTORTOOLS_MUONSELECTIONTOOL_H
#define MUONSELECTORTOOLS_MUONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "TFile.h"
#include "TH2D.h"
#include "TSystem.h" // Replace with PathResolver   

// Local include(s):
#include "MuonSelectorTools/IMuonSelectionTool.h"

namespace CP {

   /// Implementation of the muon selector tool
   ///
   /// Example implementation of how an object selector tool should
   /// look according to the TF3 recommendations.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299883 $
   /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
   ///
   class MuonSelectionTool : public virtual IAsgSelectionTool,
                             public virtual IMuonSelectionTool,
                             public asg::AsgTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS2( MuonSelectionTool, IAsgSelectionTool,
                       CP::IMuonSelectionTool )

   public:
      /// Constructor for standalone usage
      MuonSelectionTool( const std::string& name = "MuonSelection");

     virtual ~MuonSelectionTool();

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @}

      /// @name Function(s) implementing the IAsgSelectionTool interface
      /// @{

      /// Get an object describing the "selection steps" of the tool
      virtual const Root::TAccept& getTAccept() const;

      /// Get the decision using a generic IParticle pointer
      virtual const Root::TAccept& accept( const xAOD::IParticle* p ) const;

      /// @}

      /// @name Function(s) implementing the IMuonSelectorTool interface
      /// @{

      /// Get the decision for a specific Muon object
      virtual const Root::TAccept& accept( const xAOD::Muon& mu ) const;

      /// set the passes ID cuts variable of the muon 
      void setPassesIDCuts(xAOD::Muon&) const;
	  
      /// set the passes high pT cuts variable of the muon 
      virtual void setPassesHighPtCuts( xAOD::Muon& mu ) const;

      /// set the passes quality variable of the muon 
      void setQuality( xAOD::Muon& mu ) const;

      /// Returns true if the muon passes the standard MCP ID cuts. To set the value on the muon, instead call setPassesIDCuts(xAOD::Muon&) const
      bool passedIDCuts(const xAOD::Muon&) const;
 
      /// Returns true if the muon passes a standardized loose preselection.
      bool passedMuonCuts(const xAOD::Muon&) const;

      /// Returns true if the track particle passes the standard MCP ID cuts.
      bool passedIDCuts(const xAOD::TrackParticle&) const;
     
      /// Returns true if the muon passes the standard MCP High Pt cuts. To set the value on the muon, instead call setPassesHighPtCuts(xAOD::Muon&) const
      bool passedHighPtCuts(const xAOD::Muon&) const;

      /// Returns true if a CB muon fails some loose quaility requirements designed to remove pathological tracks
      bool isBadMuon(const xAOD::Muon&) const;

      /// Returns the quality of the muon. To set the value on the muon, instead call setQuality(xAOD::Muon&) const
      xAOD::Muon::Quality     getQuality( const xAOD::Muon& mu ) const;

     /// Returns true if the muon passed additional calo-tag quality cuts
     bool passedCaloTagQuality (const xAOD::Muon& mu) const;

     /// Returns true if the muon passed the tight working point cuts    
     bool passTight(const xAOD::Muon& mu, float rho, float oneOverPSig) const;
      /// @}


   private:

     MuonSelectionTool & operator=(const MuonSelectionTool &right);
     MuonSelectionTool( const MuonSelectionTool& toCopy );
     const std::string m_name;
      /// Maximum pseudorapidity for the selected muons
     double m_maxEta;
     /// xAOD::Muon::Quality m_quality;
     int  m_quality;
     bool m_isSimulation;
     
     /// Object used to store the last decision
     mutable Root::TAccept m_accept;
     
     bool m_toroidOff;
     bool m_developMode;
     bool m_TrtCutOff;
     bool m_SctCutOff;
     bool m_PixCutOff;
     bool m_SiHolesCutOff;
     bool m_TurnOffMomCorr;

     /// Checks for each histogram  
     StatusCode getHist( TFile* file, const char* histName, TH2D*& hist );
     // 
     std::string m_tightWP_rootFile;
     TH2D* m_tightWP_lowPt_rhoCuts;
     TH2D* m_tightWP_lowPt_qOverPCuts;
     TH2D* m_tightWP_mediumPt_rhoCuts;
     TH2D* m_tightWP_highPt_rhoCuts;

     // subfolder to load from the calibration db
     std::string m_calibration_version;
     // possible override for the calibration version
     std::string m_custom_dir;

   }; // class MuonSelectionTool

} // namespace CP

#endif // CPTOOLTESTS_MUONSELECTIONTOOL_H
