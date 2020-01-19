
#include "JetMonitoring/JetKinematicHistos.h"
#include "JetMonitoring/JetAttributeHisto.h"
#include "JetMonitoring/HistoDefinitionTool.h"
#include "JetMonitoring/HistosForJetSelection.h"
#include "JetMonitoring/LeadingJetsRelations.h"
#include "JetMonitoring/EfficiencyResponseHistos.h"
#include "JetMonitoring/HIEfficiencyResponseHistos.h"
#include "JetMonitoring/JetContainerHistoFiller.h"
#include "JetMonitoring/JetSelectorAttribute.h"
#include "JetMonitoring/HIJetUEMonitoring.h"

#include "../JetMonitoringTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( JetKinematicHistos )
DECLARE_TOOL_FACTORY( JetMonitoringTool )
DECLARE_TOOL_FACTORY( HistoDefinitionTool )
DECLARE_TOOL_FACTORY( HistosForJetSelection )
DECLARE_TOOL_FACTORY( LeadingJetsRelations )
DECLARE_TOOL_FACTORY( EfficiencyResponseHistos )
DECLARE_TOOL_FACTORY( HIEfficiencyResponseHistos )
DECLARE_TOOL_FACTORY( JetContainerHistoFiller )
DECLARE_TOOL_FACTORY( JetSelectorAttribute )
DECLARE_TOOL_FACTORY( HIJetUEMonitoring )

DECLARE_TOOL_FACTORY( JetAttributeHisto )


DECLARE_FACTORY_ENTRIES(JetMonitoring) {
  DECLARE_TOOL( JetKinematicHistos );
  DECLARE_TOOL( JetMonitoringTool );
  DECLARE_TOOL( JetAttributeHisto );
  DECLARE_TOOL( LeadingJetsRelations );
  DECLARE_TOOL( EfficiencyResponseHistos );
  DECLARE_TOOL( HIEfficiencyResponseHistos );

  DECLARE_TOOL( JetContainerHistoFiller );
  DECLARE_TOOL( JetSelectorAttribute );

  DECLARE_TOOL( HistoDefinitionTool );
  DECLARE_TOOL( HistosForJetSelection );

  DECLARE_TOOL( HIJetUEMonitoring );
 
}



