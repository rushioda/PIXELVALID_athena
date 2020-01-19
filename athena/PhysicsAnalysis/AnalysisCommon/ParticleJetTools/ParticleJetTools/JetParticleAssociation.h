/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLEASSOCIATION_H
#define PARTICLEJETTOOLS_JETPARTICLEASSOCIATION_H

#include "AsgTools/AsgTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticle.h"

#include <vector>
#include <string>


class JetParticleAssociation : public asg::AsgTool {
    ASG_TOOL_INTERFACE(JetParticleAssociation)

    public:

        JetParticleAssociation(const std::string& name);

        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

        // obvs to be provided by the deriving class
        virtual const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >*
            match(const xAOD::JetContainer&) const = 0;

    private:
        std::string m_outputCollectionName;
        std::string m_jetCollectionName;
        SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > > *dec;
};

#endif
