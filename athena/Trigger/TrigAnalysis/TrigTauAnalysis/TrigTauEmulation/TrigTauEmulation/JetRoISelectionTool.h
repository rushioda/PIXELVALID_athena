/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef JETROISELECTIONTOOL_H
#define JETROISELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "TrigTauEmulation/IJetRoISelectionTool.h"
#include "TrigTauEmulation/Level1SelectionTool.h"

class JetRoISelectionTool : public virtual IJetRoISelectionTool, public Level1SelectionTool
{

  ASG_TOOL_CLASS(JetRoISelectionTool, IJetRoISelectionTool)

  public:

    // Default Constructor 
    JetRoISelectionTool(const std::string& name);

    // Copy Constructor 
    JetRoISelectionTool(const JetRoISelectionTool& other);
    JetRoISelectionTool& operator=(const JetRoISelectionTool& other) = delete;

    // Destructor
    virtual ~JetRoISelectionTool() {};

    // Tool initialization
    virtual StatusCode initialize();

    // Get the decision for a specific JetRoI
    virtual const Root::TAccept& accept(const xAOD::JetRoI& l1jet) const;

  private:

    mutable Root::TAccept m_accept;

    double m_roi_pt;
    double m_roi_eta;

};
#endif
