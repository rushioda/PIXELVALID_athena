/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_T1_TRT_H
#define TRIG_T1_TRT_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include <string>
#include <vector>

namespace TrigConf{
  class ILVL1ConfigSvc;
}
class ITRT_StrawNeighbourSvc;

namespace LVL1 {
  class TrigT1TRT : public AthAlgorithm {
  public:
    TrigT1TRT(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
  private:
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
    std::string m_TRTL1ContainerName;
    
    const TRT_RDO_Container* m_TRTRDO;
 
    const TRT_ID* m_pTRTHelper;

    const InDetDD::TRT_DetectorManager *mgr;
    
    ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc;
   
    int m_TTCMultiplicity;
    int m_BoardMultiplicity;
    int m_ToTCut;
    int m_cablestarts[3]; // threshold cable starts

    int BarrelChipToBoard(int chip);
    int EndcapChipToBoard(int chip);
    int BarrelStrawNumber(int strawNumber, int strawlayerNumber, int LayerNumber);
    int EndcapStrawNumber(int strawNumber,int strawLayerNumber,int LayerNumber,int phi_stack,int side);
    int BarrelStrawLayerNumber(int strawLayerNumber, int LayerNumber);

    unsigned char mat_chip_barrel[64][1642];
    unsigned char mat_chip_endcap[64][3840];

    int numberOfStraws[75];

  };
}

#endif
