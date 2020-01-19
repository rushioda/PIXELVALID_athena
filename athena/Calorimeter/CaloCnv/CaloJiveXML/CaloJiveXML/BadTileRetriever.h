/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_BADTILERETRIEVER_H
#define JIVEXML_BADTILERETRIEVER_H

#include <string>
#include <vector>
#include <cstddef>
#include <map>

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArCabling/LArCablingService.h"

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

class IToolSvc;

class CaloIdManager;
class Identifier;
class CaloCellContainer;

namespace JiveXML{
  
  /**
   * @class BadTileRetriever
   * @brief Retrieves all @c Tile Calo Cell @c objects 
   *
   *  - @b Properties
   *    - StoreGateKeyTile: default is 'AllCalo'. Don't change.
   *	- CallThreshold: default is 50 MeV
   *	- RetrieveTile: activate retriever, default is true
   *	- CellEnergyPrec: output precision, default is 3 digits
   *	- DoBadTile: write Tile bad cell, default is false 
   *
   *   
   *  - @b Retrieved @b Data
   *    - location in phi and eta
   *    - identifier and energy of each cell 
   */
  class BadTileRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      BadTileRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
      const DataMap getBadTileData(const CaloCellContainer* cellContainer);

      /// Return the name of the data type
      virtual std::string dataTypeName() const { return m_typeName; };
	
      ///Default AthAlgTool methods
      StatusCode initialize();

    private:
      ///The data type that is generated by this retriever
      const std::string m_typeName;

      void calcTILELayerSub(Identifier&);
      const CaloIdManager* m_calo_id_man;
      const CaloCell_ID*   m_calocell_id;
    
      std::string m_sgKey; 
      double m_cellThreshold;
      int m_cellEnergyPrec;
      bool m_tile;
      bool m_doBadTile;

      DataVect m_sub;
  };
}
#endif