/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES__IASGFORWARDELECTRONISEMSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGFORWARDELECTRONISEMSELECTOR__

/**
   @class IAsgForwardElectronIsEMSelector
   @brief Interface to tool to select electrons

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   11-MAR-2014, convert to ASGTool
*/

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgEGammaIsEMSelector.h"

// Forward declarations
namespace Root{
  class TAccept;
}
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"

class IAsgForwardElectronIsEMSelector : virtual public IAsgEGammaIsEMSelector
{

  ASG_TOOL_INTERFACE(IAsgForwardElectronIsEMSelector)
  
  public:

  /**Virtual Destructor*/
  virtual ~IAsgForwardElectronIsEMSelector() {};

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** Accept with generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const = 0;
  
  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma* part) const = 0;

  /** Accept with Egamma objects */
  virtual const Root::TAccept& accept( const xAOD::Egamma& part) const = 0;

  /** Accept with Photon objects */
  virtual const Root::TAccept& accept( const xAOD::Photon* part ) const = 0;

  /** Accept with Photon objects */
  virtual const Root::TAccept& accept( const xAOD::Photon& part ) const = 0;

  /** Accept with Electron objects */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;

  /** Accept with Electron objects */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const = 0;


  ///////////////Expert methods //////////////////////////////////////

  
  //The main execute method
  virtual StatusCode execute(const xAOD::Egamma* eg) const =0;

  //////////////////////////////////////////////////////////////
  virtual unsigned int IsemValue() const = 0;
  
  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const =0;



}; // End: class definition


#endif

