/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOD3PDMAKER_CALOCELLDETAILSFILLERTOOL_H
#define CALOD3PDMAKER_CALOCELLDETAILSFILLERTOOL_H
/**
 * @file CaloD3PDMaker/src/CaloCellDetailsFillerTool.h
 * @author Hong Ma <hma@bnl.gov>
 * @date March 8, 2010
 * @brief Block filler tool for CaloCell, implementing similar features of CBNTAA_CaloCell
 */

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ToolHandle.h" 

class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class TileID;
class ICaloBadChanTool;
class ICaloNoiseTool;

namespace D3PD {


/**
 * @brief Block filler tool for EM samplings from a CaloCluster.
 */
class CaloCellDetailsFillerTool
  : public BlockFillerTool<CaloCell>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CaloCellDetailsFillerTool (const std::string& type,
			     const std::string& name,
			     const IInterface* parent);


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const CaloCell& p);

private: 

    unsigned int CaloCell_GetDetectorInfo(const CaloCell& cell) const;
    
    bool m_saveCellGain; 
    bool m_saveCellQuality; 
    bool m_saveDetInfo;
    bool m_saveTimeInfo;
    bool m_saveCellStatus; 
    bool m_saveId;
    bool m_savePosition;
    bool m_saveSigma;

    ToolHandle<ICaloBadChanTool> m_pb_tool;
    ToolHandle<ICaloNoiseTool> m_noise_tool;


    // variables to be in ntuple.  

    float * m_xCells;
    float * m_yCells;
    float * m_zCells;
    int  * m_detCells;
    float * m_timeCells;
    int  * m_badCell;
    int * m_fitQCells;
    int * m_gainCells;
    unsigned int * m_offId;
    float * m_sigma;

    bool m_useNoiseTool;
    

};

}
#endif





