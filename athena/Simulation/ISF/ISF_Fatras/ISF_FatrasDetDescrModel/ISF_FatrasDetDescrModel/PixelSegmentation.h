/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelSegmentation.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRMODEL_PIXELSEGMENTATION_H
#define ISF_FATRASDETDESCRMODEL_PIXELSEGMENTATION_H

#include "ISF_FatrasDetDescrInterfaces/ISegmentation.h"

namespace iFatras {

  /**
     @class PixelSegmentation
     Hold segmentation information for pixel detector elements
     
     @author Noemi Calace
     
  */

  class PixelSegmentation : public ISegmentation {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

  public:

    /// Constructor:
    PixelSegmentation();

    PixelSegmentation(double lengthX, double lengthY, double pitchX = 0., double pitchY = 0.);

    /// Destructor:
    ~PixelSegmentation();

    //
    bool cellOfPosition(const Amg::Vector2D& localPos, std::pair<int, int>& entryXY) const;
    Amg::Vector2D localPositionOfCell(const InDetDD::SiCellId &cellId) const;

    double pitchX() const;
    double pitchY() const;
    double phiPitch() const;
    double phiPitch(const Amg::Vector2D &) const;
    double stripLength(const Amg::Vector2D &) const;
    double sinStereoLocal(const Amg::Vector2D &localPos) const;
 
    int NcellX() const;
    int NcellY() const;

  protected:

    double m_pitchX;
    double m_pitchY;

    int m_NcellX;
    int m_NcellY;

  };

  inline double PixelSegmentation::pitchX() const {
    return m_pitchX; 
  }

  inline double PixelSegmentation::pitchY() const {
    return m_pitchY; 
  }

  inline int PixelSegmentation::NcellX() const {
    return m_NcellX; 
  }

  inline int PixelSegmentation::NcellY() const {
    return m_NcellY; 
  }

  inline double PixelSegmentation::phiPitch() const {
    return this->pitchX();
  }
  
  inline double PixelSegmentation::phiPitch(const Amg::Vector2D &) const {
    return this->phiPitch();
  }

  inline double PixelSegmentation::stripLength(const Amg::Vector2D &) const { return 0.; }
  
  inline double PixelSegmentation::sinStereoLocal(const Amg::Vector2D &) const { return 0.; }
}


#endif // ISF_FATRASDETDESCRMODEL_PIXELSEGMENTATION_H
