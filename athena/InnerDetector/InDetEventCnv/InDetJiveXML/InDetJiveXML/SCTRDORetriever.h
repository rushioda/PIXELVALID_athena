/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_SCTRDORETRIEVER_H
#define JIVEXML_SCTRDORETRIEVER_H

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetJiveXML/IInDetGeoModelTool.h"

namespace JiveXML {

  /**
   * @class SCTRDORetriever
   * @brief Retrieve all @c SCT_RDORawData information (SCT raw hits)
   *
   *  - @b Properties
   *    - <em>SCTRDOContainer</em><tt>= 'SCT_RDOs'</tt> @copydoc m_SCTRDOContainerName
   *
   *  - @b Retrieved @b Data
   *    - <em>id</em>: the identifier of the hit
   *    - <em>x0,y0,z0</em>: start coordinates of the hit strip
   *    - <em>x1,y1,z1</em>: end coordinates of the hit strip
   *    - <em>phiModule,etaModule</em>: @f$\eta@f$ and @f$\phi@f$ of module in detector coordinates
   *    - <em>timeBin</em>: time bin of the hit in the 3 consecutive bunch crossing
   *    - <em>firstHitError</em>: error flag for first hits data
   *    - <em>secondHitError</em>: error flag for second hits data
   *    .
   */
  class SCTRDORetriever : virtual public IDataRetriever, public AthAlgTool {

  public:

    /// Standard Constructor
    SCTRDORetriever(const std::string& type,const std::string& name,const IInterface* parent);

    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);    

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return typeName; };
    
    /// Only retrieve geo tool in initialize
    virtual StatusCode initialize() { return geo.retrieve(); };
    
  private:
    
    ///The data type that is generated by this retriever
    const std::string typeName;
    
    /// A tool handle to the geo model tool
    const ToolHandle<IInDetGeoModelTool> geo;

    /// The StoreGate key for the SCTRDO container
    std::string m_SCTRDOContainerName;
  };
}
#endif