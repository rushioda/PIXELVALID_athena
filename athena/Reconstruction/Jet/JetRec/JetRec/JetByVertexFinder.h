/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetByVertexFinder.h

#ifndef JetByVertexFinder_H
#define JetByVertexFinder_H

// David Adams
// March2014

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetFinder.h"
#include "fastjet/PseudoJet.hh"
#include "xAODJet/JetContainer.h"
#include "AsgTools/ToolHandle.h"

namespace fastjet {
  class ClusterSequence;
}

/// Tool to find jets by vertex.
/// A JetFinder is called separately with the input pseudojets
/// associated with each vertex.
/// The full set of ghosts is used for each jet.
/// Jets from different vertices may overlap and may share ghosts.
///
/// Tool parameters:
///   JetFinder: Tool called to find jets.
///   Vertex: >=0 selects that vertex (0 for the primary primary vertex)
///           <0 to add jets from all vertices
/// Output jet attributes:
///   OriginIndex (int): Index for the vertex corresponding to this jet.

class JetByVertexFinder
: virtual public asg::AsgTool,
  virtual public IJetFinder {
ASG_TOOL_CLASS(JetByVertexFinder, IJetFinder)

public:

  // Ctor.
  JetByVertexFinder(std::string name);

  // Initialization.
  StatusCode initialize();

  // Find jets and put them in a container.
  int find(const PseudoJetVector& inps, xAOD::JetContainer& jets,
           xAOD::JetInput::Type contype,
           const NameList& ghostlabs) const;

  // Dump to log.
  void print() const;

private:  //data

  // Job options.
  ToolHandle<IJetFinder> m_finder;  // Tool to find jets.
  int m_ivtx;                       // Vertex index or <0 for all

};

#endif
