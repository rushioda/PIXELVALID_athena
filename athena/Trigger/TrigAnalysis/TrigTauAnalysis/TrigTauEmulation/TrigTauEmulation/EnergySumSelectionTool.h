/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef ENERGYSUMSELECTIONTOOL_H
#define ENERGYSUMSELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "TrigTauEmulation/IEnergySumSelectionTool.h"
#include "TrigTauEmulation/Level1SelectionTool.h"

class EnergySumSelectionTool : public virtual IEnergySumSelectionTool, public Level1SelectionTool
{
  ASG_TOOL_CLASS(EnergySumSelectionTool, IEnergySumSelectionTool)
  
  public:

    EnergySumSelectionTool(const std::string& name);
    EnergySumSelectionTool(const EnergySumSelectionTool& other);
    EnergySumSelectionTool& operator=(const EnergySumSelectionTool& other) = delete;
    virtual ~EnergySumSelectionTool() {};

    // Tool initialization
    virtual StatusCode initialize();

    // Get the decision for a specific EnergySumRoI
    virtual const Root::TAccept& accept(const xAOD::EnergySumRoI& l1xe) const;

  private:

    mutable Root::TAccept m_accept;
    double m_MET_cut;

  protected:

    double calculate_MET(const xAOD::EnergySumRoI& l1xe) const;

};
#endif
