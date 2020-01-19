/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_MISSINGETRETRIEVER_H
#define JIVEXML_MISSINGETRETRIEVER_H

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"

class MissingET;

namespace JiveXML {

  /* @class MissingETRetriever
   * @brief Retrieves all @c MissingET @c objects
   *
   *  - @b Properties
   *    - <em>FavouriteMissingETCollection </em><tt>= "MET_RefFinal"</tt>: @copydoc m_sgKeyFavourite
   *    - <em>OtherMissingETCollections</em> @copydoc m_otherKeys
   *
   *  - @b Retrieved @b Data
   *    - @e et : magnitude of  @f$E_{T}@f$
   *    - @e etx, ety: X- and Y- component of @f$E_{T}@f$ vector
   *    .
   */
  class MissingETRetriever : virtual public IDataRetriever,
                                     public AthAlgTool {
    
  public:
    
    MissingETRetriever(const std::string& t,const std::string& n,const IInterface* p);

    ~MissingETRetriever() {}
    
    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
    const DataMap getData(const MissingET*);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return typeName; };
    

  private:
    ///The data type that is generated by this retriever
    const std::string typeName;

    ///First MissingET collection to retrieve, shown as default in Atlantis
    std::string m_sgKeyFavourite;
    ///Other MissingET collections to retrieve, all if vector is empty (default)
    std::vector<std::string> m_otherKeys;
  };
}

#endif