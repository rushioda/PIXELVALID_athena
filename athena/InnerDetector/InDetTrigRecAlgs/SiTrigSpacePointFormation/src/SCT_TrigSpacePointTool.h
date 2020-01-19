/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: SCT_TrigSpacePointTool.h
//
// author: Patricia Conde Muino 31/01/2005
//         Patricia.Conde.Muino@cern.ch
//
//         Jiri.Masik@cern.ch
//
// Description: Tool to create SCT SpacePoints out of a cluster collection.
//              This tool hides from the normal user all the details of the
//              geometry (separating clusters from each stereo layer, ...).
//              This tool is based on the SiSpacePointFinder algorithm.
//
// Modified :
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef SCT_TrigSpacePointTool_H
#define SCT_TrigSpacePointTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"

#include "Identifier/IdentifierHash.h"

#include "InDetTrigToolInterfaces/ITrigSCT_SpacePointTool.h"
#include "SiSpacePointTool/SiSpacePointMakerTool.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"  
#include "InDetReadoutGeometry/PixelDetectorManager.h"  
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
  	 
#include <string>

class SCT_ID;
class SpacePointCollection; 
class SpacePointOverlapCollection; 
class SpacePointContainer; 
class SvcLocator;
class StoreGateSvc;
class SCT_NeighboursTable;
class IBeamCondSvc;

namespace InDet { class SiElementPropertiesTable;}
typedef InDet::SCT_ClusterContainer SCT_ClusterContainer; 
typedef InDet::SiClusterCollection SiClusterCollection; 
typedef InDet::SiCluster SiCluster; 
typedef InDet::SCT_ClusterCollection SCT_ClusterCollection; 
typedef Trk::SpacePoint SpacePoint; 

namespace InDet {


  class SCT_TrigSpacePointTool : virtual public ITrigSCT_SpacePointTool,
				 public AthAlgTool {

  public:


    // Constructor 

   SCT_TrigSpacePointTool( const std::string& type, 
			    const std::string& name, 
			    const IInterface* parent );

    virtual ~SCT_TrigSpacePointTool();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // Convert clusters to space points

    void addSCT_SpacePoints (const SCT_ClusterCollection* clusCollection,
			     const SCT_ClusterContainer* clusterContainer,
			     SpacePointCollection* spacepointCollection,
			     SpacePointOverlapCollection* overlapColl); 

    void addSCT_SpacePoints (const SCT_ClusterCollection* clusCollection,
			     const SCT_ClusterContainer* clusterContainer,
			     SpacePointCollection* spacepointCollection); 


    void checkForSCT_Points (const SCT_ClusterCollection* clusters1,
			     const IdentifierHash& id2, double minDiff, 
			     double maxDiff,
			     SpacePointCollection* spacepointCollection, 
			     bool overlapColl); 

    void checkForSCT_Points (const SCT_ClusterCollection* clusters1, 
			     const IdentifierHash& id2, double min1, 
			     double max1,
			     double min2, double max2);


  private:

    //!< The following are ranges within which clusters must lie to
    //!< make a spacepoint. Opposite and eta neighbours
    //!< clusters must lie within range of each other. 
    //!< Phi clusters must lie in region of each wafer separately.
    
    float m_overlapLimitOpposite;  //!< overlap limit for opposite-neighbours.
    float m_overlapLimitPhi;       //!< overlap limit for phi-neighbours.
    float m_overlapLimitEtaMin;    //!< low overlap limit for eta-neighbours.
    float m_overlapLimitEtaMax;    //!< high overlap limit for eta-neighbours.
    float m_epsWidth;		   //!< safety margin for half-width.

    std::string m_spacePointsOverlapName;
    std::string m_SiSpacePointMakerToolName;


    const InDetDD::SCT_DetectorManager *m_manager; 
    const SCT_ID* m_idHelper;
    InDet::SiElementPropertiesTable* m_properties;

    const SCT_ClusterContainer* m_Sct_clcontainer;
    InDet::SiSpacePointMakerTool* m_SiSpacePointMakerTool;

    bool m_allClusters;     //!< process all clusters without limits if true.
    bool m_overlap;         //!< process all overlapping SCT pairs if true.   
    bool m_overrideBS;

    float m_xVertex;
    float m_yVertex;
    float m_zVertex;

    ServiceHandle<IBeamCondSvc> m_iBeamCondSvc;
    std::string  m_beamCondSvcName;
    SpacePointOverlapCollection* m_spacepointoverlapCollection; 

    
  };


}
#endif //SCT_TrigSpacePointTool_H
