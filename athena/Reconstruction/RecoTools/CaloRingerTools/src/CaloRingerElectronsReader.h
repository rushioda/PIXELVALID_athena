/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingerElectronsReader.h 668867 2015-05-20 20:23:22Z wsfreund $
#ifndef CALORINGERTOOLS_CALORINGERELECTRONSREADER_H
#define CALORINGERTOOLS_CALORINGERELECTRONSREADER_H

// STL includes:
#include <string>

// Base includes:
#include "CaloRingerInputReader.h"
#include "CaloRingerTools/ICaloRingerElectronsReader.h"
#include "CaloRingerReaderUtils.h"

//CxxUtils for override final  
#include "CxxUtils/final.h"
#include "CxxUtils/override.h"

// xAOD includes:
#include "xAODEgamma/ElectronContainer.h"

// Asg selectors include:
#include "RingerSelectorTools/IAsgElectronRingerSelector.h"

namespace Ringer {

class CaloRingerElectronsReader : public CaloRingerInputReader, 
                                  public ICaloRingerElectronsReader
{

  public:

    /// @name CaloRingerElectronsReader ctors and dtors:
    /// @{
    /** 
     * @brief Default constructor
     **/
    CaloRingerElectronsReader(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    /** 
     * @brief Destructor
     **/
    ~CaloRingerElectronsReader();
    /// @}
    
    /// Tool main methods:
    /// @{
    /** 
     * @brief initialize method 
     **/
    virtual StatusCode initialize() ATH_OVERRIDE;
    /** 
     * @brief read electrons and populates @name decoMap with them and their
     * respective CaloRings.
     **/
    virtual StatusCode execute() ATH_OVERRIDE;
    /** 
     * @brief finalize method
     **/
    virtual StatusCode finalize() ATH_OVERRIDE;
    /// @}


  private:

    /// @name CaloRingerElectronsReader private methods:
    /// @{
    /** Retrieve Electron Asg Selector */
    StatusCode retrieveSelectors();
    /// @}

    /// Tool CaloRingerElectronsReader props (python configurables):
    /// @{
    /** 
     * @brief Electron selectors.
     **/
    ToolHandleArray<IAsgElectronRingerSelector> m_ringerSelectors;
    /// @}

    /// Tool CaloRingerElectronsReader props (non configurables):
    /// @{
    /// The electron container
    xAOD::ElectronContainer* m_container;
    const xAOD::ElectronContainer* m_constContainer;

    /// The CaloRings Builder functor:
    BuildCaloRingsFctor<xAOD::Electron> *m_clRingsBuilderElectronFctor;

    /// Whether selectors are available
    bool m_selectorsAvailable;
    /// @}

};

} // namespace Ringer

#endif
