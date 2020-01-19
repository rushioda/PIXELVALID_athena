/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_JETROIRETRIEVER_H
#define JIVEXML_JETROIRETRIEVER_H

#include <string>

#include "JiveXML/IDataRetriever.h"

#include "AthenaBaseComps/AthAlgTool.h"


namespace JiveXML{

  /**
   * @class JetROIRetriever
   * @brief Retrieves ROI objects, default mode uses HLT/Offline object
   *        LVL1_ROI::jets_type
   *
   *  - @b Properties
   *    - <em>readJEM</em>: 'false' by default. If 'true' reads low-level ROI data
   *                        from L1Calo hardware (for ACR when reading SFI)
   *
   *  - @b Retrieved @b Data
   *    - <em>eta,phi</em>: coordinates of ROI
   *    - <em>energy</em>: energy in ROI
   *    - <em>roiWord, thrPattern</em>: trigger bitpatterns, undecoded
   *    .
   */
 
  class JetROIRetriever : virtual public IDataRetriever, public AthAlgTool {

  public:

    /// Standard Constructor
    JetROIRetriever(const std::string& type, const std::string& name, const IInterface* parent);

    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return m_typeName; };

  private:

   ///The data type that is generated by this retriever
    const std::string m_typeName;

    bool m_readJEM;
    bool m_maskLowerThresholds;
    std::string m_sgKey;
  };

}
#endif