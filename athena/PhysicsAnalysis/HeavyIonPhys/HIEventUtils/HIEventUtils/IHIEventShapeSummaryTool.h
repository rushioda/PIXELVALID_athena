/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HI_IHIEVENTSHAPESUMMARYTOOL_H
#define HI_IHIEVENTSHAPESUMMARYTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

class IHIEventShapeSummaryTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IHIEventShapeSummaryTool)
  
  public:

  virtual ~IHIEventShapeSummaryTool() {};
  virtual StatusCode summarize(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShapeContainer* out) const = 0;
  virtual StatusCode initialize() = 0;
  virtual std::string dumpList() const  {return std::string("");};
  
};
#endif
