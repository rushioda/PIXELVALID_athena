/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CuboidVolumeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_BOXVOLUMESBOUNDS_H
#define TRKVOLUMES_BOXVOLUMESBOUNDS_H
    
// Trk
#include "TrkVolumes/VolumeBounds.h"
#include "TrkDetDescrUtils/SixObjectsAccessor.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

   class RectangleBounds;
   class Volume;
   class Surface;
   
  /** 
   @class CuboidVolumeBounds
    
   Bounds for a cubical Volume, the decomposeToSurfaces method creates a
   vector of 6 surfaces:
      
    BoundarySurfaceFace [index]:
        
      - negativeFaceXY [0] : Rectangular Trk::PlaneSurface, parallel to \f$ xy \f$ plane at negative \f$ z \f$
      - positiveFaceXY [1] : Rectangular Trk::PlaneSurface, parallel to \f$ xy \f$ plane at positive \f$ z \f$
      - negativeFaceXY [2] : Rectangular Trk::PlaneSurface, attached to \f$ yz \f$ plane at negative \f$ x \f$
      - positiveFaceXY [3] : Rectangular Trk::PlaneSurface, attached to \f$ yz \f$ plane at negative \f$ x \f$
      - negativeFaceXY [4] : Rectangular Trk::PlaneSurface, parallel to \f$ zx \f$ plane at negative \f$ y \f$
      - positiveFaceXY [5] : Rectangular Trk::PlaneSurface, parallel to \f$ zx \f$ plane at positive \f$ y \f$
    
    @image html CuboidVolumeBounds_decomp.gif  
    
    @author Andreas.Salzburger@cern.ch 
    */
    
 class CuboidVolumeBounds : public VolumeBounds {
  
  public:
    /**Default Constructor*/
    CuboidVolumeBounds(); 
     
    /**Constructor - the box boundaries */
    CuboidVolumeBounds(double hlenghtx, double hlenghty, double hlengthz);
    
    /**Copy Constructor */
    CuboidVolumeBounds(const CuboidVolumeBounds& bobo);
    
    /**Destructor */
    virtual ~CuboidVolumeBounds();
    
    /**Assignment operator*/
    CuboidVolumeBounds& operator=(const CuboidVolumeBounds& bobo);
    
    /**Virtual constructor */
    CuboidVolumeBounds* clone() const override;
    
    /**This method checks if position in the 3D volume frame is inside the cylinder*/     
    bool inside(const Amg::Vector3D& , double tol=0.) const override;
         
    /** Method to decompose the Bounds into boundarySurfaces */
    const std::vector<const Trk::Surface*>* decomposeToSurfaces(const Amg::Transform3D& transform) const override;
    
    /** Provide accessor for BoundarySurfaces */
    const ObjectAccessor& boundarySurfaceAccessor(const Amg::Vector3D& gp,
                                                  const Amg::Vector3D& dir,
                                                  bool forceInside=false) const override;
                                                
    /**This method returns the halflength in local x*/
    double halflengthX() const;
    
    /**This method returns the halflength in local y*/
    double halflengthY() const;    
    
    /**This method returns the halflength in local z*/
    double halflengthZ() const;        
    
    /** Output Method for MsgStream*/
    MsgStream& dump(MsgStream& sl) const override;
    
    /** Output Method for std::ostream */
    std::ostream& dump(std::ostream& sl) const override;

  private:
    /** This method returns the associated RecantleBounds of the face PlaneSurface parallel to local xy plane */
    RectangleBounds* faceXYRectangleBounds() const;
    
    /** This method returns the associated RecantleBounds of the face PlaneSurface parallel to local yz plane */
    RectangleBounds* faceYZRectangleBounds() const;
    
    /** This method returns the associated RecantleBounds of the face PlaneSurface parallel to local zx plane */
    RectangleBounds* faceZXRectangleBounds() const;
 
#ifdef TRKDETDESCR_USEFLOATPRECISON
#define double float
#endif    
    double m_halfX;
    double m_halfY;
    double m_halfZ;
#ifdef TRKDETDESCR_USEFLOATPRECISON
#undef double
#endif
        
    /** There's only one single object Acessor for the moment
       has to be implemented if Cuboids are used more widely */
    SixObjectsAccessor m_objectAccessor;
    
 };

 inline CuboidVolumeBounds* CuboidVolumeBounds::clone() const
 { return new CuboidVolumeBounds(*this); }

 inline bool CuboidVolumeBounds::inside(const Amg::Vector3D &pos, double tol) const
 { 
   return (fabs(pos.x())<=m_halfX+tol && fabs(pos.y())<=m_halfY+tol && fabs(pos.z())<=m_halfZ+tol);
 }

 inline double CuboidVolumeBounds::halflengthX() const { return m_halfX; }

 inline double CuboidVolumeBounds::halflengthY() const { return m_halfY; }

 inline double CuboidVolumeBounds::halflengthZ() const { return m_halfZ; }

 inline const ObjectAccessor& CuboidVolumeBounds::boundarySurfaceAccessor(const Amg::Vector3D&,
                                                                          const Amg::Vector3D&,
                                                                          bool) const
  { return(m_objectAccessor); }
                        

}

#endif // TRKVOLUMES_BOXVOLUMESBOUNDS_H
