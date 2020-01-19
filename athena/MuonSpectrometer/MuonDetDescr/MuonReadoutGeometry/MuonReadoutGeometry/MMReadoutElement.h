/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MM Readout Element properties
 -----------------------------------------
***************************************************************************/

#ifndef MUONGEOMODEL_MMREADOUTELEMENT_H
# define MUONGEOMODEL_MMREADOUTELEMENT_H

#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonChannelDesign.h"
//#include "MuonIdHelpers/MmIdHelper.h"


class StoreGateSvc;

namespace Trk{
  class RectangleBounds;
  class PlaneSurface;
}


namespace MuonGM {
  /**
     An MMReadoutElement corresponds to a single STGC module; therefore 
     typicaly a barrel muon station contains:
  */

  class MuonDetectorManager;
 
  class MMReadoutElement: public MuonClusterReadoutElement {
  public:
    /** constructor */
    MMReadoutElement(GeoVFullPhysVol* pv, std::string stName,
		     int zi, int fi, int mL, bool is_mirrored,
		     MuonDetectorManager* mgr);
    
    /** destructor */
    ~MMReadoutElement();                  

    /** function to be used to check whether a given Identifier is contained in the readout element */
    bool containsId(Identifier id) const;

    /** distance to readout. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    double distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const;

    /** strip number corresponding to local position. 
	If the local position is outside the active volume, the function first shift the position back into the active volume */
    int stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const;

    /** strip position 
	If the strip number is outside the range of valid strips, the function will return false */
    bool stripPosition( const Identifier& id, Amg::Vector2D& pos ) const;

    /** number of layers in phi/eta projection */
    int numberOfLayers( bool ) const;

    /** number of strips per layer */
    int numberOfStrips( const Identifier& layerId )   const;
    int numberOfStrips( int , bool measuresPhi ) const;

    /** space point position for a given pair of phi and eta identifiers 
	The LocalPosition is expressed in the reference frame of the phi surface.
	If one of the identifiers is outside the valid range, the function will return false */
    bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const;

    /** Global space point position for a given pair of phi and eta identifiers 
	If one of the identifiers is outside the valid range, the function will return false */
    bool spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const;

    /** space point position for a pair of phi and eta local positions and a layer identifier 
	The LocalPosition is expressed in the reference frame of the phi projection.
    */
    void spacePointPosition( const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos ) const;


    /** simHit local (SD) To Global position - to be used by MuonGeoAdaprors only 
     */
    Amg::Vector3D localToGlobalCoords(Amg::Vector3D locPos, Identifier id) const;
    

    /** @brief function to fill tracking cache */
    void         fillCache() const;
    void         refreshCache() const {clearCache(); fillCache();}

    /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
    int surfaceHash( const Identifier& id ) const;

    /** @brief returns the hash to be used to look up the surface and transform in the MuonClusterReadoutElement tracking cache */
    int surfaceHash( int gasGap, int measPhi) const;

    /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
    int layerHash( const Identifier& id ) const;
    /** @brief returns the hash to be used to look up the normal and center in the MuonClusterReadoutElement tracking cache */
    int layerHash( int gasGap) const;
  
    /** returns the hash function to be used to look up the surface boundary for a given identifier */
    int  boundaryHash(const Identifier& id) const;  

    /** @brief returns whether the current identifier corresponds to a phi measurement */
    bool measuresPhi(const Identifier& id) const;

    /** @brief initialize the design classes for this readout element */
    void initDesign(double largeY, double smallY, double lengthX, double pitch, double thickness);
    
    /** returns the MuonChannelDesign class for the given identifier */
    const MuonChannelDesign* getDesign( const Identifier& id ) const;

    /** set methods only to be used by MuonGeoModel */
    void setIdentifier(Identifier id);
    
    /** set methods only to be used by MuonGeoModel */
    void setChamberLayer(int ml) {m_ml=ml;}
  private:

    //MuonChannelDesign m_phiDesign;
    std::vector<MuonChannelDesign> m_etaDesign;

    std::vector<int> m_nStrips;
    int m_nlayers;
    
    int m_ml; 
    Identifier m_parentId;
 
    // surface dimensions
    double m_halfX;
    double m_minHalfY;
    double m_maxHalfY;

    // transforms (RE->layer)
    Amg::Transform3D m_Xlg[4];
  };

  inline int MMReadoutElement::surfaceHash( const Identifier& id ) const {
    return surfaceHash(manager()->mmIdHelper()->gasGap(id),0);
  }


  inline int MMReadoutElement::surfaceHash( int gasGap, int /*measPhi*/ ) const {
    return gasGap-1;      // measPhi not used
  }

  inline int MMReadoutElement::layerHash( const Identifier& id ) const {
    return layerHash(manager()->mmIdHelper()->gasGap(id));
  }

  inline int MMReadoutElement::layerHash( int gasGap ) const {
    return gasGap-1;
  }

  inline int MMReadoutElement::boundaryHash( const Identifier& /*id*/ ) const {
    return 0;
  }

  inline bool MMReadoutElement::measuresPhi( const Identifier& /*id*/ ) const {
    return false;
  }


  inline const MuonChannelDesign* MMReadoutElement::getDesign( const Identifier& id ) const {
    //return measuresPhi(id) ? &m_phiDesign : &m_etaDesign;
    return &(m_etaDesign[layerHash(id)]);
  }
  inline double MMReadoutElement::distanceToReadout( const Amg::Vector2D& pos, const Identifier& id ) const {
    const MuonChannelDesign* design = getDesign(id);
    if( !design ) return -1.;
    return design->distanceToReadout(pos);
  }

  inline int MMReadoutElement::stripNumber( const Amg::Vector2D& pos, const Identifier& id ) const {
    const MuonChannelDesign* design = getDesign(id);
    if( !design ) return -1;
    return design->channelNumber(pos);
  }

  inline bool MMReadoutElement::stripPosition( const Identifier& id, Amg::Vector2D& pos ) const {
    const MuonChannelDesign* design = getDesign(id);
    if( !design ) return false;
    return design->channelPosition(manager()->mmIdHelper()->channel(id),pos);
  }

  inline int MMReadoutElement::numberOfLayers( bool ) const { return m_nlayers; }

  inline int MMReadoutElement::numberOfStrips( const Identifier& layerId )   const {
    return m_nStrips[layerHash(layerId)];
  }

  inline int MMReadoutElement::numberOfStrips( int lay, bool /*measPhi*/ ) const {
    if (lay>-1 && lay<(int)m_nStrips.size()) return m_nStrips[lay]; 
    else return -1;
  }

  inline bool MMReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector2D& pos ) const {
    Amg::Vector2D phiPos;
    Amg::Vector2D etaPos;
    if( !stripPosition(phiId,phiPos) || !stripPosition(etaId,etaPos) ) return false;
    spacePointPosition(phiPos,etaPos,pos);
    return true;
  }

  inline bool MMReadoutElement::spacePointPosition( const Identifier& phiId, const Identifier& etaId, Amg::Vector3D& pos ) const {
    Amg::Vector2D lpos;
    spacePointPosition(phiId,etaId,lpos);
    surface(phiId).localToGlobal(lpos,pos,pos);
    return true;
  }

  inline void MMReadoutElement::spacePointPosition( const Amg::Vector2D& phiPos, const Amg::Vector2D& etaPos, Amg::Vector2D& pos ) const {
    pos[0] = phiPos.x();
    pos[1] = etaPos.x();
  }


} // namespace MuonGM

#endif // MUONGEOMODEL_STGCREADOUTELEMENT_H
