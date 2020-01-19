/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AbstractVolume.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_VOLUME_H
#define TRKVOLUMES_VOLUME_H

//Trk
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "CLIDSvc/CLASS_DEF.h"

class MsgStream;

namespace Trk {

  class VolumeBounds;
  class ObjectAccessor;  

  /** @class Volume
    
    Base class for all volumes inside the tracking realm, it defines 
    the interface for inherited Volume classes 
    regarding the geometrical information.
      
    @author Andreas.Salzburger@cern.ch
    */
    class Volume {
      public:
        /** Default constructor */
        Volume();
        
        /** Expizit constructor with arguments */
        Volume(Amg::Transform3D* htrans, VolumeBounds* volBounds);
        
        /** Copy Constructor */
        Volume(const Volume& vol);
        
        /** Copy Constructor */
        Volume(const Volume& vol, const Amg::Transform3D& shift);
        
        /** Destructor */
        virtual ~Volume();
        
        /** Assignment operator */
        Volume& operator=(const Volume& vol);
        
        /** Pseudo-constructor */
        virtual Volume* clone() const;
        
        /** Return methods for geometry transform */
        const Amg::Transform3D& transform() const;           

        /** returns the center of the volume */      
        const Amg::Vector3D&    center() const;
        
        /** returns the volumeBounds() */
        const VolumeBounds&     volumeBounds() const;
        
        /** Inside() method for checks */
        bool inside(const Amg::Vector3D& gp, double tol=0.) const ;
        
        /** Provide accessor for BoundarySurfaces */
        const ObjectAccessor& boundarySurfaceAccessor(const Amg::Vector3D& gp,
                                                      const Amg::Vector3D& mom,
                                                      bool forceInside=false) const;
      protected:         
        mutable Amg::Transform3D*                  m_transform;         //!< HepGeom::Transform3D
        mutable Amg::Vector3D*                     m_center;            //!< center position of the surface
        SharedObject<const VolumeBounds>           m_volumeBounds;      //!< the volumeBounds
    };  
  
    inline const Amg::Transform3D& Volume::transform() const
    {  if (m_transform) return(*m_transform); 
       return Trk::s_idTransform;
    }  
  
    inline const Amg::Vector3D& Volume::center() const
    {
     if (m_center) return (*m_center);
     if (!m_center && m_transform){
        m_center = new Amg::Vector3D(m_transform->translation());
        return(*m_center);
      }
     return Trk::s_origin;
    }

    inline const VolumeBounds& Volume::volumeBounds() const
    {  return(m_volumeBounds.getRef()); }
    
    
/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const Volume& vol);
std::ostream& operator << ( std::ostream& sl, const Volume& vol); 

                       
} // end of namespace Trk

CLASS_DEF( Trk::Volume , 125765716 , 1 )


#endif // TRKVOLUMES_VOLUME_H

