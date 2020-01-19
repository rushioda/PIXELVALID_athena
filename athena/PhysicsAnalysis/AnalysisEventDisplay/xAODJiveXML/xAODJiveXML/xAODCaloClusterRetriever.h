/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_XAODCALOCLUSTERRetriever_H
#define JIVEXML_XAODCALOCLUSTERRetriever_H

#include <string>
#include <vector>
#include <map>

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

class CaloClusterContainer;

namespace JiveXML{
  
  /**
   * @class xAODCaloClusterRetriever
   * @brief Retrieves all @c Calo Cluster @c objects 
   *
   *  - @b Properties
   *    - FavouriteJetCollection
   *    - OtherJetCollections
   *    - DoWriteHLT
   *
   *  - @b Retrieved @b Data
   *    - Usual four-vector: phi, eta, et
   *    - No cells in AOD Clusters. numCells is placeholder
   *      for compatibility with 'full' clusters in AtlantisJava
   */
  class xAODCaloClusterRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      xAODCaloClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
      const DataMap getData(const xAOD::CaloClusterContainer*);

      /// Return the name of the data type
      virtual std::string dataTypeName() const { return typeName; };

    private:
      ///The data type that is generated by this retriever
      const std::string typeName;

      std::string m_sgKeyFavourite;
      std::vector<std::string> m_otherKeys;
      bool m_doWriteHLT;
  };
}
#endif
