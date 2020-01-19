#include "xAODJiveXML/xAODCaloClusterRetriever.h"
#include "xAODJiveXML/xAODElectronRetriever.h"
#include "xAODJiveXML/xAODMissingETRetriever.h"
#include "xAODJiveXML/xAODMuonRetriever.h"
#include "xAODJiveXML/xAODPhotonRetriever.h"
#include "xAODJiveXML/xAODJetRetriever.h"
#include "xAODJiveXML/xAODTauRetriever.h"
#include "xAODJiveXML/xAODTrackParticleRetriever.h"
#include "xAODJiveXML/xAODVertexRetriever.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(xAODCaloClusterRetriever)
DECLARE_TOOL_FACTORY(xAODElectronRetriever)
DECLARE_TOOL_FACTORY(xAODMissingETRetriever)
DECLARE_TOOL_FACTORY(xAODMuonRetriever)
DECLARE_TOOL_FACTORY(xAODPhotonRetriever)
DECLARE_TOOL_FACTORY(xAODJetRetriever)
DECLARE_TOOL_FACTORY(xAODTauRetriever)
DECLARE_TOOL_FACTORY(xAODTrackParticleRetriever)
DECLARE_TOOL_FACTORY(xAODVertexRetriever)

DECLARE_FACTORY_ENTRIES(xAODJiveXML) {
  DECLARE_ALGTOOL(xAODCaloClusterRetriever)
  DECLARE_ALGTOOL(xAODElectronRetriever)
  DECLARE_ALGTOOL(xAODMissingETRetriever)
  DECLARE_ALGTOOL(xAODMuonRetriever)
  DECLARE_ALGTOOL(xAODPhotonRetriever)
  DECLARE_ALGTOOL(xAODJetRetriever)
  DECLARE_ALGTOOL(xAODTauRetriever)
  DECLARE_ALGTOOL(xAODTrackParticleRetriever)
  DECLARE_ALGTOOL(xAODVertexRetriever)
}
