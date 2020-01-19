/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_TAUJETRETRIEVER_H
#define JIVEXML_TAUJETRETRIEVER_H

#include <string>
#include <vector>
#include <map>

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "tauEvent/TauJetContainer.h"

namespace JiveXML{
  
  /**
   * @class TauJetRetriever
   * @brief Retrieves all @c TauJet @c objects 
   *
   *  - @b Properties
   *    - StoreGateKey: First collection to be retrieved, displayed
   *      in Atlantis without switching. All other collections are 
   *      also retrieved.
   *
   *  - @b Retrieved @b Data
   *    - Usual four-vectors: phi, eta, et etc.
   *    - some special tau: isolFrac, stripwidth etc. Note: this
   *      was once optimised for rel.13 ! In future: isTau
   *    - Associations via ElementLink: key/index scheme
   */
  class TauJetRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      TauJetRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
      const DataMap getData(const Analysis::TauJetContainer*);
    
      /// Return the name of the data type
      virtual std::string dataTypeName() const { return typeName; };

    private:
      ///The data type that is generated by this retriever
      const std::string typeName;

      std::string m_sgKey;
      bool m_doWriteHLT;
      std::string m_tauCutLevelSelect;
      bool m_fastSimSGFlag;
  };
}
#endif