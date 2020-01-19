/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_LAYERMATERIALPROVIDER_H
#define TRKDETDESCRTOOLS_LAYERMATERIALPROVIDER_H

// Trk
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk {

    class TrackingGeometry;
    class TrackingVolume;    
    class Layer;
    class Surface;
    class LayerMaterialMap;


    /** @class LayerMaterialProvider

      AlgTool that retrieves the LayerMaterial from COOL and 
      loads it onto the geometry

      @author Andreas.Salzburger@cern.ch
     */

    class LayerMaterialProvider :  public AthAlgTool, virtual public IGeometryProcessor {
     
      public:
        /** Constructor */
        LayerMaterialProvider(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~LayerMaterialProvider();

        /** Processor Action to work on TrackingGeometry& tgeo */
        virtual StatusCode process(const TrackingGeometry& tgeo);
       
        /** Processor Action to work on TrackingVolumes - the level is for the hierachy tree*/
        virtual StatusCode process(const TrackingVolume& tvol, size_t level = 0);   
       
        /** Processor Action to work on Layers */
        virtual StatusCode process(const Layer& lay, size_t level = 0);
       
        /** Processor Action to work on Surfaces */
        virtual StatusCode process(const Surface& surf, size_t level = 0);

      private:
          
        StatusCode loadMaterialMap();               //!< reatrieve the Material map from the detector store

        //!< boolean switch for assignLayerMaterial
        mutable const LayerMaterialMap*             m_layerMaterialMap;
        std::string                                 m_layerMaterialMapName;
                        
        
    };

} // end of namespace

#endif // TRKDETDESCRTOOLS_LAYERMATERIALPROVIDER_H

