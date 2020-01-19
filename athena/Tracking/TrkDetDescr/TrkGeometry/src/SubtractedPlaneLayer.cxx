/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedPlaneLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/SubtractedPlaneLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkParameters/TrackParameters.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"

Trk::SubtractedPlaneLayer::SubtractedPlaneLayer(const SubtractedPlaneSurface* subtrPlaneSurf,
                            const Trk::LayerMaterialProperties& laymatprop,
                            double thickness,
                            Trk::OverlapDescriptor* olap,
                            int laytyp) :
  SubtractedPlaneSurface(*subtrPlaneSurf),
  Layer(laymatprop, thickness, olap, laytyp) 
{}
  
Trk::SubtractedPlaneLayer::SubtractedPlaneLayer(const Trk::SubtractedPlaneLayer& play):
  SubtractedPlaneSurface(play),
  Layer(play)
{}

Trk::SubtractedPlaneLayer::SubtractedPlaneLayer(const Trk::SubtractedPlaneLayer& play, const Amg::Transform3D& transf):
  SubtractedPlaneSurface(play,transf),
  Layer(play)
{}

Trk::SubtractedPlaneLayer& Trk::SubtractedPlaneLayer::operator=(const SubtractedPlaneLayer& play)
{
  if (this!=&play){
   // call the assignments of the base classes
   Trk::SubtractedPlaneSurface::operator=(play);
   Trk::Layer::operator=(play);
  }
  return(*this);
}
        
const Trk::SubtractedPlaneSurface& Trk::SubtractedPlaneLayer::surfaceRepresentation() const
{
  return (*this);
}

double Trk::SubtractedPlaneLayer::preUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                          Trk::PropDirection dir) const
{
    if (!Trk::Layer::m_layerMaterialProperties.getPtr())
      return 0.;
    if (Trk::SubtractedPlaneSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPreFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePreFactor();  
}

double Trk::SubtractedPlaneLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                 Trk::PropDirection dir) const 
{
    if (!Trk::Layer::m_layerMaterialProperties.getPtr())
      return 0.;
    if (Trk::SubtractedPlaneSurface::normal().dot(dir*parm.momentum().normalized()) > 0. )
      return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
    return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}

void Trk::SubtractedPlaneLayer::moveLayer(Amg::Transform3D& shift) const {
       Amg::Transform3D transf = shift * (*m_transform);
       delete m_transform;
       m_transform = new Amg::Transform3D(transf);
       delete m_center;
       m_center = new Amg::Vector3D(m_transform->translation());
       delete m_normal;
       m_normal = new Amg::Vector3D(m_transform->rotation().col(2));
}
