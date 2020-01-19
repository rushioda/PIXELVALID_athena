// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RoiReader.h        
//
//                   
//  
//
//   $Id: RoiReader.h, v0.0   Sun 25th Jan 2016 12:20:40 CET sutt $


#ifndef  ROIREADER_H
#define  ROIREADER_H

#include <string>
#include <vector>

#include "AsgTools/AsgTool.h"

#include "TrigRoiConversion/IRoiReader.h"

/// RoiDescriptor collection
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"


class RoiReader: public virtual IRoiReader, public asg::AsgTool {

  ASG_TOOL_CLASS( RoiReader, IRoiReader )
  
public:

  RoiReader( const std::string& name );

  virtual ~RoiReader() { } 

  virtual StatusCode initialize();

  void execute( std::vector<std::string>& keys );

  void deserialiser( TrigRoiDescriptorCollection& collection, 
  		     const std::string key ) const;

};



#endif  // ROIREADER_H 










