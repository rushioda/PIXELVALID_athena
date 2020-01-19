/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_CSCCLUSTERRETRIEVER_H
#define JIVEXML_CSCCLUSTERRETRIEVER_H

#include "JiveXML/IDataRetriever.h"

#include "AthenaBaseComps/AthAlgTool.h"


class CscIdHelper;

namespace JiveXML{

  class CSCClusterRetriever : virtual public IDataRetriever, public AthAlgTool {

  public:
  
    /// Standard Constructor
    CSCClusterRetriever(const std::string& type, const std::string& name, const IInterface* parent);
  
    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return m_typeName; };

    ///Default AthAlgTool methods
    StatusCode initialize();

  private:

    ///The data type that is generated by this retriever
    const std::string m_typeName;

    ///The storegate key for the CSC collection
    std::string m_sgKey;

    /// CSC identifier helper
    const CscIdHelper* m_cscHelper;

  };

}
#endif