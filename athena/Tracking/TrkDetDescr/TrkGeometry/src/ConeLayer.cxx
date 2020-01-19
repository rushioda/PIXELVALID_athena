/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeLayer.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/ConeLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkSurfaces/ConeBounds.h"
#include "TrkParameters/TrackParameters.h"
// CLHEP
#include "GeoPrimitives/GeoPrimitives.h"


Trk::ConeLayer::ConeLayer(Amg::Transform3D* transform,
                          Trk::ConeBounds* cbounds,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  ConeSurface(transform, cbounds),
  Layer(laymatprop, thickness, olap, laytyp)
{}

Trk::ConeLayer::ConeLayer(Trk::ConeSurface* cyl,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  ConeSurface(*cyl),
  Layer(laymatprop, thickness, olap, laytyp)
{}
        
Trk::ConeLayer::ConeLayer(Amg::Transform3D* transform,
			              Trk::ConeBounds* cbounds,
			              Trk::SurfaceArray* surfaceArray,
			              double thickness,
			              Trk::OverlapDescriptor* olap,
			              int laytyp) :
  ConeSurface(transform, cbounds),
  Layer(surfaceArray, thickness, olap, laytyp)
{}
                
Trk::ConeLayer::ConeLayer(Amg::Transform3D* transform,
                          Trk::ConeBounds* cbounds,
                          Trk::SurfaceArray* surfaceArray,
                          const Trk::LayerMaterialProperties& laymatprop,
                          double thickness,
                          Trk::OverlapDescriptor* olap,
                          int laytyp) :
  ConeSurface(transform, cbounds),
  Layer(surfaceArray, laymatprop, thickness, olap, laytyp)
{}

Trk::ConeLayer::ConeLayer(const Trk::ConeLayer& clay):
  ConeSurface(clay),
  Layer(clay)
{}

Trk::ConeLayer::ConeLayer(const Trk::ConeLayer& clay, const Amg::Transform3D& transf):
  ConeSurface(clay,transf),
  Layer(clay)
{}

Trk::ConeLayer& Trk::ConeLayer::operator=(const ConeLayer& clay)
{
  if (this!=&clay){
    // call the assignments of the base classes
    Trk::ConeSurface::operator=(clay);
    Trk::Layer::operator=(clay);
  }
  return(*this);
}

    
const Trk::ConeSurface& Trk::ConeLayer::surfaceRepresentation() const
{
  return (*this);
}

double Trk::ConeLayer::preUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                   Trk::PropDirection dir) const
{    
  if (!Trk::Layer::m_layerMaterialProperties.getPtr())
    return 0.;
  // calculate the direction to the normal 
  const Amg::Vector3D& parmPos = parm.position();
  Amg::Vector3D pastStep(parmPos + dir*parm.momentum().normalized());
  if (pastStep.perp() > parm.position().perp())
    return Trk::Layer::m_layerMaterialProperties->alongPreFactor();
  return   Trk::Layer::m_layerMaterialProperties->oppositePreFactor();
}

double Trk::ConeLayer::postUpdateMaterialFactor(const Trk::TrackParameters& parm,
                                                    Trk::PropDirection dir) const 
{
  if (!Trk::Layer::m_layerMaterialProperties.getPtr())
    return 0;
  const Amg::Vector3D& parmPos = parm.position();
  Amg::Vector3D pastStep(parmPos + dir*parm.momentum().normalized());
  if (pastStep.perp() > parm.position().perp())
    return Trk::Layer::m_layerMaterialProperties->alongPostFactor();
  return   Trk::Layer::m_layerMaterialProperties->oppositePostFactor();
}

void Trk::ConeLayer::moveLayer(Amg::Transform3D& shift) const {
  Amg::Transform3D transf = shift * (*m_transform);
  delete m_transform;
  m_transform = new Amg::Transform3D(transf);
  delete m_center;
  m_center = new Amg::Vector3D(m_transform->translation());
  delete m_normal;
  m_normal = new Amg::Vector3D(m_transform->rotation().col(2));
}
