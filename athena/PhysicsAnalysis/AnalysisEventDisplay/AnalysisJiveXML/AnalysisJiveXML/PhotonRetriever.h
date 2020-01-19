/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_PHOTONRETRIEVER_H
#define JIVEXML_PHOTONRETRIEVER_H

#include <string>
#include <vector>
#include <map>

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"

class PhotonContainer;

namespace JiveXML{
  
  /**
   * @class PhotonRetriever
   * @brief Retrieves all @c Photon @c objects (PhotonAODCollection)
   *
   *  - @b Properties
   *    - StoreGateKey: First collection to be retrieved, displayed
   *      in Atlantis without switching. All other collections are 
   *      also retrieved.
   *
   *  - @b Retrieved @b Data
   *    - Usual four-vector: phi, eta, et
   */
  class PhotonRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      PhotonRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
      const DataMap getData(const PhotonContainer*);
    
      /// Return the name of the data type
      virtual std::string dataTypeName() const { return typeName; };

    private:
      ///The data type that is generated by this retriever
      const std::string typeName;

      std::string m_sgKey;
  };
}
#endif
