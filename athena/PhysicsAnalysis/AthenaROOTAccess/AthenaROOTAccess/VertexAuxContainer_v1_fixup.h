// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/VertexAuxContainer_v1_fixup.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2015
 * @brief Run fixup for VertexAuxContainer_v1.
 */


#ifndef ATHENAROOTACCESS_VERTEXAUXCONTAINER_V1_FIXUP_H
#define ATHENAROOTACCESS_VERTEXAUXCONTAINER_V1_FIXUP_H


#include "AthenaROOTAccess/IARAFixup.h"


namespace AthenaROOTAccess {


class VertexAuxContainer_v1_fixup
  : public IARAFixup
{
public:
  /// Run fixup on an object that has just been read in, after associating
  /// aux data.
  virtual void fixupAfterAux (void* data, long long entry) override;
};


} // namespace AthenaROOTAccess


#endif // not ATHENAROOTACCESS_VERTEXAUXCONTAINER_V1_FIXUP_H
