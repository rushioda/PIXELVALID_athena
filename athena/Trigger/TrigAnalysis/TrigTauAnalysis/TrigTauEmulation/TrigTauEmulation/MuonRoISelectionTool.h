/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef MUONROISELECTIONTOOL_H
#define MUONROISELECTIONTOOL_H

// Framework include
#include "AsgTools/AsgTool.h"
/* #include "PATCore/IAsgSelectionTool.h" */

// Local includes
#include "TrigTauEmulation/IMuonRoISelectionTool.h"
#include "TrigTauEmulation/Level1SelectionTool.h"

class MuonRoISelectionTool : public virtual IMuonRoISelectionTool, public Level1SelectionTool
{
  ASG_TOOL_CLASS(MuonRoISelectionTool, IMuonRoISelectionTool)

  public:

    MuonRoISelectionTool(const std::string& name);
    MuonRoISelectionTool(const MuonRoISelectionTool& other);
    MuonRoISelectionTool& operator=(const MuonRoISelectionTool& other) = delete;
    virtual ~MuonRoISelectionTool() {};

    // Tool initialization
    virtual StatusCode initialize();

    // Get the decision for a specific MuonRoI
    virtual const Root::TAccept& accept(const xAOD::MuonRoI& l1muon) const;

  private:

    mutable Root::TAccept m_accept;

    double m_roi_pt;
    double m_roi_eta;

};
#endif
