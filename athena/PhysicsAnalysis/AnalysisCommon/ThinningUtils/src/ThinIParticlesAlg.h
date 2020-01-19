///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef THINNINGUTILS_THINIPARTICLESALG_H
#define THINNINGUTILS_THINIPARTICLESALG_H 1

/**
   @class ThinIParticlesAlg
   @brief Tool to thin (remove) individual xAOD::IParticles from a container.
          This algorithm is not doing anything itself. It is rather holding a
          private instance of the ThinIParticlesTool and it is forwarding
          its Properties to this tool and finally using this tool for every event.
          An additional option is to give a selection string,
          e.g., "Muons.pt > 10.0*GeV".

   @author Karsten Koeneke
*/


// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IThinningSvc.h"


// forward declarations
class IJobOptionsSvc;
namespace DerivationFramework {
  class IThinningTool;
}



class ThinIParticlesAlg
  : public ::AthAlgorithm
{
 public:

  /// Constructor with parameters:
  ThinIParticlesAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor:
  virtual ~ThinIParticlesAlg();

  /// Athena algorithm's initalize hook
  virtual StatusCode  initialize();

  /// Athena algorithm's execute hook
  virtual StatusCode  execute();

  /// Athena algorithm's finalize hook
  virtual StatusCode  finalize();


private:
  // The update handlers

  /// This internal method will realize if a user sets the 'IParticlesToThin' property
  void setupIParticlesToThin( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'InputContainerList' property
  void setupInputContainerList( Property& /*prop*/ );

  /// This internal method will realize if a user sets the 'Selection' property
  void setupSelection( Property& /*prop*/ );


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  /// The job options service (will be used to forward this algs properties to
  /// the private tool)
  ServiceHandle<IJobOptionsSvc> m_jos;

  /// Pointer to IThinningSvc
  ServiceHandle<IThinningSvc> m_thinningSvc;

  /// The ToolHandle to the ThinningTool
  ToolHandle<DerivationFramework::IThinningTool> m_thinTool;


  /// Name of the IParticleContainer to thin
  StringProperty m_ipartKey;

  /// This boolean is true if the user sets the 'IParticlesToThin' property
  bool m_setIPartKey;


  /// List of names of the object collections
  StringArrayProperty m_inCollKeyList;

  /// This boolean is true if the user sets the 'InputContainerList' property
  bool m_setInCollKey;


  /// The selection string that will select which xAOD::IParticles to keep from
  /// an xAOD::IParticleContainer
  StringProperty m_selection;

  /// This boolean is true if the user sets the 'Selection' property
  bool m_setSelection;



  /// Internal event counter
  unsigned long m_nEventsProcessed;

};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

/// This internal method will realize if a user sets the 'IParticlesToThin' property
inline void ThinIParticlesAlg::setupIParticlesToThin( Property& /*prop*/ ) {
  m_setIPartKey = true;
  return;
}

/// This internal method will realize if a user sets the 'InputContainerList' property
inline void ThinIParticlesAlg::setupInputContainerList( Property& /*prop*/ ) {
  m_setInCollKey = true;
  return;
}

/// This internal method will realize if a user sets the 'Selection' property
inline void ThinIParticlesAlg::setupSelection( Property& /*prop*/ ) {
  m_setSelection = true;
  return;
}


#endif //> !THINNINGUTILS_THINIPARTICLESALG_H
