/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_TANGENTTOCIRCLES_H
#define DCMATH_TANGENTTOCIRCLES_H

#include <cmath>
#include <vector>

#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/DriftCircle.h"

namespace TrkDriftCircleMath {

  class TangentToCircles{
  public:
    typedef std::vector<Line> LineVec;
  public:
    TangentToCircles() : m_debug(false) { m_lines.reserve(4);}

    LineVec& tangentLines( const DriftCircle& dc1, const DriftCircle& dc2 );

    void debug(bool debug) { m_debug = debug; }
  private:
    bool m_debug;
    LineVec m_lines;
  };

}

#endif
