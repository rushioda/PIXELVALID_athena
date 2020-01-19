/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DetachedTrackingVolume.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"

Trk::DetachedTrackingVolume::DetachedTrackingVolume() :
  m_trkVolume(),
  m_name("undefined"),
  m_layerRepresentation(0),
  m_multilayerRepresentation(0),
  m_baseTransform(0),
  m_constituents(0)
{}

Trk::DetachedTrackingVolume::DetachedTrackingVolume(std::string name,
                                    const Trk::TrackingVolume* volume
                                    ) :
  m_trkVolume(volume),
  m_name(name),
  m_layerRepresentation(0),
  m_multilayerRepresentation(0),
  m_baseTransform(0),
  m_constituents(0)
{}   

Trk::DetachedTrackingVolume::DetachedTrackingVolume(std::string name,
						    const Trk::TrackingVolume* volume,
                                                    const Trk::Layer* lay,
                                                    const std::vector<const Trk::Layer*>* multilay  
                                    ) :
  m_trkVolume(volume),
  m_name(name),
  m_layerRepresentation(lay),
  m_multilayerRepresentation(multilay),
  m_baseTransform(0),
  m_constituents(0)
{}   
  
Trk::DetachedTrackingVolume::~DetachedTrackingVolume()
{
  delete m_trkVolume;
  if (m_layerRepresentation) delete m_layerRepresentation;
  if (m_multilayerRepresentation) {
    for (unsigned int i=0;i<m_multilayerRepresentation->size();i++) delete (*m_multilayerRepresentation)[i];
    delete m_multilayerRepresentation; 
  }
  delete m_baseTransform;
}

void Trk::DetachedTrackingVolume::move( Amg::Transform3D& shift) const 
{
  m_trkVolume->moveTV( shift );
  if (m_layerRepresentation) m_layerRepresentation->moveLayer(shift);
  if (m_multilayerRepresentation) for (unsigned int i=0;i<m_multilayerRepresentation->size();i++) (*m_multilayerRepresentation)[i]->moveLayer(shift);
}

const Trk::DetachedTrackingVolume* Trk::DetachedTrackingVolume::clone( std::string name, Amg::Transform3D& shift) const
{
    const Trk::TrackingVolume* newTV = new TrackingVolume(*(this->trackingVolume()),shift );
    const Trk::DetachedTrackingVolume* newStat = 0;
    // layer representation ?
    const Trk::PlaneLayer* newLay=0;
    if (this->layerRepresentation()) {
        std::vector<const Trk::Layer*>* newMulti = 0;
        const Trk::PlaneLayer* pl = dynamic_cast<const Trk::PlaneLayer*> (this->layerRepresentation());
        if (pl) {
            newLay = new Trk::PlaneLayer(*pl);
            newLay->moveLayer( shift );
            if (this->multilayerRepresentation()) {
                newMulti =  new std::vector<const Trk::Layer*>;
                for (unsigned int i=0;i<this->multilayerRepresentation()->size();i++) {
                    const Trk::PlaneLayer* mpl = dynamic_cast<const Trk::PlaneLayer*> ((*(this->multilayerRepresentation()))[i]);
                    if (mpl) {
                        const Trk::PlaneLayer* newPl = new Trk::PlaneLayer(*mpl);
                        newPl->moveLayer( shift );
                        newMulti->push_back(newPl);
                    }
                    else
                        std::cout << "WARNING   Trk::DetachedTrackingVolume::clone()   dynamic cast to 'const Trk::PlaneLayer* mpl' failed!" << std::endl;
                }
            }
            newStat = new Trk::DetachedTrackingVolume( name, newTV, newLay, newMulti );
        } else {
            std::cout << "WARNING   Trk::DetachedTrackingVolume::clone()   dynamic cast to 'const Trk::PlaneLayer* pl' failed!" << std::endl;
            newStat = new Trk::DetachedTrackingVolume( name, newTV );
        }
    } else {
        newStat = new Trk::DetachedTrackingVolume( name, newTV );
    }
    //
    // enclose layers
    if (newTV->confinedVolumes()) {
        std::vector<const Trk::TrackingVolume*> vols = newTV->confinedVolumes()->arrayObjects();
        for ( unsigned int ivol=0; ivol<vols.size(); ivol++) {
            const Trk::LayerArray* layAr = vols[ivol]->confinedLayers();
            const std::vector<const Trk::Layer*>* alays = vols[ivol]->confinedArbitraryLayers();
            if (layAr) {
                const std::vector<const Trk::Layer*> lays = layAr->arrayObjects();
                for ( unsigned int il=0; il<lays.size(); il++)
                    lays[il]->encloseDetachedTrackingVolume(*newStat);
            }
            if (alays) {
                for ( unsigned int il=0; il<alays->size(); il++)
                    (*alays)[il]->encloseDetachedTrackingVolume(*newStat);
            }
        }
    }
    if (newTV->confinedLayers()) {
        const std::vector<const Trk::Layer*> lays = newTV->confinedLayers()->arrayObjects();
        for ( unsigned int il=0; il<lays.size(); il++) 
            lays[il]->encloseDetachedTrackingVolume(*newStat);
    }
    if (newTV->confinedArbitraryLayers()) {
        const std::vector<const Trk::Layer*>* alays = newTV->confinedArbitraryLayers();
        for ( unsigned int il=0; il<alays->size(); il++) 
            (*alays)[il]->encloseDetachedTrackingVolume(*newStat);
    }
    //
    newStat->saveConstituents(this->constituents());
    return newStat;
}

void Trk::DetachedTrackingVolume::compactify(size_t& cSurfaces, size_t& tSurfaces) const
{
 
    // deal with the Tracking Volume representation
    if ( m_trkVolume ) m_trkVolume->compactify(cSurfaces,tSurfaces);

    // deal with the layer representation
    if ( layerRepresentation() ){ 
        ++tSurfaces;
        if ( layerRepresentation()->surfaceRepresentation().owner() == Trk::noOwn ){
             layerRepresentation()->surfaceRepresentation().setOwner(Trk::TGOwn);
             ++cSurfaces;
        }
    }
    // deal with the multi-layer representation
    if ( multilayerRepresentation() ){
        tSurfaces += m_multilayerRepresentation->size();
        for ( auto& mLayerIter :  (*m_multilayerRepresentation) ){
            if ( (*mLayerIter).surfaceRepresentation().owner() == Trk::noOwn ){
                (*mLayerIter).surfaceRepresentation().setOwner(Trk::TGOwn);
                ++cSurfaces;
            }
        }
    }
    //<< !@ TODO include volumes
    
}


void Trk::DetachedTrackingVolume::sign(GeometrySignature signat, GeometryType geotype) const 
{ m_trkVolume->sign(signat, geotype); }

Trk::GeometrySignature Trk::DetachedTrackingVolume::geometrySignature() const 
{ return m_trkVolume->geometrySignature(); }

Trk::GeometryType Trk::DetachedTrackingVolume::geometryType() const 
{ return m_trkVolume->geometryType(); }

void Trk::DetachedTrackingVolume::setBaseTransform( Amg::Transform3D* transf ) const 
{
  if ( transf ) m_baseTransform = transf ;
  else {
    delete m_baseTransform;
    m_baseTransform = new Amg::Transform3D(this->trackingVolume()->transform());
  }
}

void Trk::DetachedTrackingVolume::realign( Amg::Transform3D* transf ) const 
{
  if ( transf ) {
    Amg::Transform3D shift = (*transf)*this->trackingVolume()->transform().inverse();
    this->move(shift);
  } else if (m_baseTransform) {
    Amg::Transform3D shift = *m_baseTransform*this->trackingVolume()->transform().inverse();
    this->move(shift);
  }
}
