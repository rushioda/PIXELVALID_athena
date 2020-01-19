// -*- C++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_TRACKVERTEXASSOCIATIONTOOL_H
#define JETRECTOOLS_TRACKVERTEXASSOCIATIONTOOL_H

#include "AsgTools/AsgTool.h"

#include "JetInterface/IJetExecuteTool.h"
#include "JetEDM/TrackVertexAssociation.h"

#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "AsgTools/ToolHandle.h"

#include <string>
///////////////////////////////////////////////
/// \class TrackVertexAssociationTool
/// \brief creates a jet::TrackVertexAssociation object
///
/// Creates a jet::TrackVertexAssociation map.
/// The tool expects a ITrackVertexAssociationTool which it uses to create the map
/// from a  TrackParticleContainer and a VertexContainer. The map is recorded in the evtStore.
///
/// Properties :
///  - "TrackParticleContainer" : name of TrackParticleContainer
///  - "VertexContainer" : name of VertexContainer
///  - "TrackVertexAssociation" : name of the output jet::TrackVertexAssociation
///  - "TrackVertexAssoTool" : A ITrackVertexAssociationTool instance
///
///  If the TrackVertexAssoTool is left blank, the old custom method is used to build the code
/// using cuts defined by these properties 
///  - "MaxTransverseDistance",
///  - "MaxLongitudinalDistance"
///  - "MaxZ0SinTheta"
/// This old part of the code may disappear in the future.
////////////////////////////////////////////////////////////////////

class TrackVertexAssociationTool :  virtual public asg::AsgTool,
                                    virtual public IJetExecuteTool
{
  ASG_TOOL_CLASS(TrackVertexAssociationTool,IJetExecuteTool)
  
  public:
  TrackVertexAssociationTool(const std::string & t);
  
  
  StatusCode initialize();

  // Retrieve necessary inputs, call building function, store result in evtStore
  int execute() const;
  
private:

  // Constuct a TrackVertexAssociation using a ITrackVertexAssociationTool object
  jet::TrackVertexAssociation* buildTrackVertexAssociation_withTool(const xAOD::TrackParticleContainer*, const xAOD::VertexContainer*) const;
  // Constuct a TrackVertexAssociation using the older custom method
  jet::TrackVertexAssociation* buildTrackVertexAssociation_custom(const xAOD::TrackParticleContainer*, const xAOD::VertexContainer*) const;


  ToolHandle<CP::ITrackVertexAssociationTool> m_tvaTool;

  std::string m_trackContainer;
  std::string m_vertexContainer;
  std::string m_tvaStoreName;

  // old Configurable parameters

  float m_transDistMax;
  float m_longDistMax;
  float m_maxZ0SinTheta;
  
};


#endif
