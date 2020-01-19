/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CurvilinearUVT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_CURVILINEARUVT_H
#define TRKEVENTPRIMITIVES_CURVILINEARUVT_H

// Amg 
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

class MsgStream;

//STD
#include <iostream>

namespace Trk {
  
   /** @class CurvilinearUVT

      simple class that constructs the curvilinear vectors curvU and curvV
      from a given momentum direction (must be unit vector).
     
     The curvilinear frame @f$ (\vec u, \vec v, \vec t) at each point @f$ m @f$ is constructed by:
     
     - @f$ \vec t = \vec{curvT} @f$ ... momentum direction at @f$ m @f$ 
     - @f$ \vec u = \vec{curvU} = \frac{\vec t \times \vec z}{||\vec t \times \vec z||} @f$ ... with @f$ \vec z @f$ the global z-axis  
     - @f$ \vec v = \vec{curvV} = \vec t \times \fec u @f$ ... constructed so that frame is right-handed  
     
      The picture shows the curvilinear frame for a straight line measurement

      @image html CurvilinearFrameStraw.png

      @author Andreas.Salzburger@cern.ch
     */
   class CurvilinearUVT {
	
     public:
     /** POOL method needed  */
       CurvilinearUVT(){}
       
     /** Create from Amg::Vector3D */
       CurvilinearUVT(const Amg::Vector3D& dir);
       
     /** Destructor */  
       ~CurvilinearUVT();
       
     /** Access methods */
       const Amg::Vector3D& curvU() const;
       const Amg::Vector3D& curvV() const;
       const Amg::Vector3D& curvT() const;      
     
     private:      
      Amg::Vector3D         m_curvU;  //!< curvilinear U axis 
      Amg::Vector3D         m_curvV;  //!< curvilinear V axis
      Amg::Vector3D         m_curvT;  //!< curvilinear T axis
	
    };

   inline  CurvilinearUVT::CurvilinearUVT(const Amg::Vector3D& dir) :
     m_curvT(dir)
    {         
      // make it safe for tracks progressing along the z axis
      if ( fabs( dir.normalized().dot(Amg::Vector3D::UnitZ()) ) < 0.99 ){
          Amg::Vector3D curvZcrossT = -(m_curvT.cross(Amg::Vector3D::UnitZ()));
          m_curvU = curvZcrossT.normalized();
          m_curvV = m_curvT.cross(m_curvU);
      } else {
          Amg::Vector3D curvZcrossT = -(m_curvT.cross(Amg::Vector3D::UnitX()));
          m_curvU = curvZcrossT.normalized();
          m_curvV = m_curvT.cross(m_curvU);
        }
    }

   inline const Amg::Vector3D& CurvilinearUVT::curvU() const {
     return(m_curvU);
   }

   inline const Amg::Vector3D& CurvilinearUVT::curvV() const {
     return(m_curvV);
   }

   inline const Amg::Vector3D& CurvilinearUVT::curvT() const {
     return(m_curvT);
   }  
   
/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const CurvilinearUVT& uvt);
std::ostream& operator << ( std::ostream& sl, const CurvilinearUVT& uvt); 

}

#endif // TRKEVENTPRIMITIVES_CURVILINEARUV_H

