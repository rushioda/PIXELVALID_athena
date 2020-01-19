/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarCluster.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASEVENT_PLANARCLUSTER_H
#define ISF_FATRASEVENT_PLANARCLUSTER_H

//Base class 
#include "TrkPrepRawData/PrepRawData.h"

#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"
#include "Identifier/Identifier.h"
#include "InDetPrepRawData/SiWidth.h"

class MsgStream;

namespace iFatras {

  /**
     @class PlanarCluster
     Class to make cluster using PlanarDetElement as detector element

     @author Noemi Calace

   */

  class PlanarCluster : public Trk::PrepRawData {
    
    /////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////
  public:

    /** Public, Copy, operator=, constructor*/
    PlanarCluster();

    PlanarCluster( const PlanarCluster &);

    PlanarCluster  &operator=(const PlanarCluster &);
    PlanarCluster  &operator=(      PlanarCluster &&);
    
    PlanarCluster( const Identifier& RDOId,
		   const Amg::Vector2D& locpos, 
		   const std::vector<Identifier>& rdoList,
		   const InDet::SiWidth& width,
		   const iFatras::PlanarDetElement* detEl,
		   const Amg::MatrixX* locErrMat );
    
    PlanarCluster( const Identifier& RDOId,
		   const Amg::Vector2D& locpos, 
		   std::vector<Identifier>&& rdoList,
		   const InDet::SiWidth& width,
		   const iFatras::PlanarDetElement* detEl,
		   std::unique_ptr<const Amg::MatrixX> locErrMat );
    
    // Deconstructor:
    virtual ~PlanarCluster();
    
    /** return width class reference */
    const InDet::SiWidth&  width()  const;
    
    /** return global position as a pointer. */
    const Amg::Vector3D* globalPositionPtr() const;
    
    /** return global position reference */
    const Amg::Vector3D& globalPosition() const;
    
    /** return the detector element corresponding to this PRD */
    const iFatras::PlanarDetElement* detectorElement() const;

    /** dump information about the PlanarCluster*/
    MsgStream&    dump( MsgStream&    stream) const;
    /** dump information about the PlanarCluster*/
    std::ostream& dump( std::ostream& stream) const;
    
  private:
    InDet::SiWidth m_width; //col, row, and width in mm
    mutable const Amg::Vector3D* m_globalPosition;
    const iFatras::PlanarDetElement* m_detEl;
    
  };

  MsgStream&    operator << (MsgStream& stream,    const PlanarCluster& prd);
  std::ostream& operator << (std::ostream& stream, const PlanarCluster& prd);
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
  // return width:
  inline const InDet::SiWidth& PlanarCluster::width() const {
    return m_width;
  }
  
  // return globalPosition:
  inline const Amg::Vector3D* PlanarCluster::globalPositionPtr() const {
    if (m_globalPosition==0) m_globalPosition = m_detEl->surface(identify()).localToGlobal(localPosition());
    return m_globalPosition;
  }
  
  // return globalPosition:
  inline const Amg::Vector3D& PlanarCluster::globalPosition() const {
    return *globalPositionPtr();
  }
  
  inline const iFatras::PlanarDetElement* PlanarCluster::detectorElement() const {
    return m_detEl;
  }
  
}

#endif // ISF_FATRASEVENT_PLANARCLUSTER_H
