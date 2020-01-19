///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METMuonAssociator.h 
// Header file for class METTruthAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METTRUTHASSOCIATOR_H
#define METRECONSTRUCTION_METTRUTHASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"

namespace met{
  class METTruthAssociator final
    : public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METTruthAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METTruthAssociator(const std::string& name);
    ~METTruthAssociator();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
    protected: 

    StatusCode fillAssocMap(xAOD::MissingETAssociationMap* metMap,
			    const xAOD::IParticleContainer* hardObjs) const final;
    StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const;
    //
    StatusCode associateJets(xAOD::MissingETAssociationMap* metMap) const;
    //
    StatusCode extractTruthParticles(const xAOD::IParticle* obj,
				     std::vector<const xAOD::IParticle*>& truthlist) const;
    StatusCode extractTruthFromElectron(const xAOD::IParticle* obj,
					std::vector<const xAOD::IParticle*>& truthlist) const;
    StatusCode extractTruthFromPhoton(const xAOD::IParticle* obj,
				      std::vector<const xAOD::IParticle*>& truthlist) const;
    StatusCode extractTruthFromMuon(const xAOD::IParticle* obj,
				    std::vector<const xAOD::IParticle*>& truthlist) const;
    StatusCode extractTruthFromTau(const xAOD::IParticle* obj,
				     std::vector<const xAOD::IParticle*>& truthlist) const;
    //
    StatusCode computeSoftTerms(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const;
    //
    StatusCode extractPFO(const xAOD::IParticle*,
			  std::vector<const xAOD::IParticle*>&,
			  const met::METAssociator::ConstitHolder&,
			  std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t>&) const final
    {return StatusCode::FAILURE;} // should not be called
    StatusCode extractTracks(const xAOD::IParticle*,
			     std::vector<const xAOD::IParticle*>&,
			     const met::METAssociator::ConstitHolder&) const final
    {return StatusCode::FAILURE;} // should not be called
    StatusCode extractTopoClusters(const xAOD::IParticle*,
				   std::vector<const xAOD::IParticle*>&,
				   const met::METAssociator::ConstitHolder&) const final
    {return StatusCode::FAILURE;} // should not be called

    private:

    std::string m_recoJetKey;
    std::string m_recoElKey;
    std::string m_recoMuKey;
    std::string m_recoGamKey;
    std::string m_recoTauKey;
    std::string m_truthEventKey;
 
    /// Default constructor: 
    METTruthAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METMUONASSOCIATOR_H
