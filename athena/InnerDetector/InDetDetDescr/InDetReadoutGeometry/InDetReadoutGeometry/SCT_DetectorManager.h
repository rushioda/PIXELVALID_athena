/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_DetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SCT_DETECTORMANAGER_H
#define INDETREADOUTGEOMETRY_SCT_DETECTORMANAGER_H

#include "GeoPrimitives/GeoPrimitives.h"

#include "GeoModelKernel/GeoVPhysVol.h"

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include "InDetIdentifier/SCT_ID.h"
  
class StoreGateSvc;
class Identifier; 
class IdentifierHash;
class GeoAlignableTransform;
class GeoVFullPhysVol;
class GeoVPhysVol;

namespace InDetDD {

  class SiDetectorElement;
  class ExtendedAlignableTransform;
  class SCT_ModuleSideDesign;

  /** @class SCT_DetectorManager
  
      Dedicated detector manager extending the functionality of the SiDetectorManager
      with dedicated SCT information, access.
      
      @author: Grant Gorfine
      - modified and maintained by Nick Styles & Andreas Salzburger
      */
      
  class SCT_DetectorManager : public SiDetectorManager  {

    public:
    
      // Constructor
      SCT_DetectorManager( StoreGateSvc* detStore );
     
      // Destructor
      virtual ~SCT_DetectorManager();
     
      /** Access Raw Geometry */
      virtual unsigned int getNumTreeTops()           const;                  
      virtual PVConstLink  getTreeTop(unsigned int i) const;
      /** Add tree top */
      void addTreeTop(PVLink); 
    
    
      //
      // Access Readout Elements
      //
    
      /** access to individual elements via Identifier */
      virtual SiDetectorElement * getDetectorElement(const Identifier &id) const;

      /** access to individual elements via IdentifierHash */
      virtual SiDetectorElement * getDetectorElement(const IdentifierHash &idHash) const;
      
      /** access to individual elements via module numbering schema */
      SiDetectorElement * getDetectorElement(int barrel_endcap, int layer_wheel, int phi_module, int eta_module, int side) const;
    
      /** access to whole collectiom via iterators */
      virtual const SiDetectorElementCollection * getDetectorElementCollection() const;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementBegin() const;
      virtual SiDetectorElementCollection::const_iterator getDetectorElementEnd() const;
    
      /** Add elememts during construction */
      virtual void addDetectorElement(SiDetectorElement * element);
    
      /** Add alignable transforms. No access to these, they will be changed by manager: */
      virtual void addAlignableTransform (int level,
    				                      const Identifier &id, 
    				                      GeoAlignableTransform *xf,
    				                      const GeoVFullPhysVol * child);
      
      /**  As above but does a dynamic_cast to GeoVFullPhysVol */
      virtual void addAlignableTransform (int level,
    				                      const Identifier &id, 
    				                      GeoAlignableTransform *xf,
    				                      const GeoVPhysVol * child); 
    
      // DEPRECATED
      virtual void addAlignableTransform (int, const Identifier &, GeoAlignableTransform *) {}; // For backward compatibility
    
      /** Initialize the neighbours. This can only be done when all elements are built. */
      virtual void initNeighbours();
        
      /** Methods to query which manager we have */
      virtual bool isPixel() const {return false;}
    
      /** Check identifier is for this detector */
      virtual bool identifierBelongs(const Identifier & id) const;
    
      /** Access to module design, casts to SCT_ModuleSideDesign */
      const SCT_ModuleSideDesign * getSCT_Design(int i) const;

      /** Process new global DB folders for L1 and L2 **/
      bool processGlobalAlignment(const std::string &, int level, FrameType frame) const;
    
    private:  
      /** implements the main alignment update for delta transforms in different frames,
          it translates into the LocalDelta or GlobalDelta function of SiDetectorManager
      */
      virtual bool setAlignableTransformDelta(int level, 
    					  const Identifier & id, 
    					  const Amg::Transform3D & delta,
    					  FrameType frame) const;
      
      /** Prevent copy and assignment */
      const SCT_DetectorManager & operator=(const SCT_DetectorManager &right);
      SCT_DetectorManager(const SCT_DetectorManager &right); 
    
      virtual const SCT_ID  * getIdHelper() const;
       
      // Private member data
      std::vector<PVLink>                                           m_volume;  
      SiDetectorElementCollection                                   m_elementCollection;
      typedef std::map<Identifier, ExtendedAlignableTransform *>    AlignableTransformMap;
      std::vector< AlignableTransformMap >                          m_higherAlignableTransforms;
      std::vector< ExtendedAlignableTransform *>                    m_alignableTransforms; 
      std::vector< ExtendedAlignableTransform *>                    m_moduleAlignableTransforms; 
      const SCT_ID*                                                 m_idHelper;
      
      /** This variable switches the how the local alignment corrections are applied
          If true they will be calcualted on top  of all of other corrections but in the default reference frame
          If false they will be calcualted  on top  of all of other corrections but in the globally aligned reference frame    
      */
      bool                                                          m_isLogical;      
      
      
    };

} // namespace InDetDD

#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(InDetDD::SCT_DetectorManager, 72488296, 1) 
#endif

#endif // INDETREADOUTGEOMETRY_SCT_DETECTORMANAGER_H
