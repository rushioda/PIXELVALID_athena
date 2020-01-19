# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


""" TrigTauRec """

__author__  = 'S.Xella, O.Igonkina, F.Friedrich'
__version__ =""
__doc__     ="Configuration of TrigTauRec"

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from TrigTauRec.TrigTauRecConf import TrigTauRecMerged
from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.AppMgr import ToolSvc

class TrigTauRecMerged_Tau (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_Tau"):
            super( TrigTauRecMerged_Tau , self ).__init__( name )
            self._mytools = []

            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
            
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            #what should run in trigger???
            #same as tauRec (but without Pi0 cluster finder)
            tools.append(taualgs.getJetSeedBuilder())
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            tools.append(taualgs.getTauAxis())
            tools.append(taualgs.getTauTrackFinder())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            tools.append(taualgs.getCellVariables(cellConeSize=0.4))
            
            #tools.append(taualgs.getElectronVetoVars())  #needed?
            #tools.append(taualgs.getEnergyCalibrationEM())
            ## tools.append(taualgs.getTauEflowTrackMatchCells())
            ## tools.append(taualgs.getTauEflowAddCaloInfo())  
            ## tools.append(taualgs.getTauEflowVariables())
            
            tools.append(taualgs.getTauVertexVariables())
            tools.append(taualgs.getTauCommonCalcVars())
            tools.append(taualgs.getTauSubstructure())
            #tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
	    
            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass
	    
	    
            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()
		    

class TrigTauRecMerged_Tau2012 (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_Tau2012"):
            super( TrigTauRecMerged_Tau2012 , self ).__init__( name )
            self._mytools = []

            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
            
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

            # Don't try to retrieve for now, should be handled globally
            #if not hasattr(ToolSvc, "LumiBlockMuTool"):
            #        from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuTool
            #        ToolSvc += CfgMgr.LumiBlockMuTool("LumiBlockMuTool")

           
            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTau2012_")

            #what should run in trigger???
            #same as tauRec (but without Pi0 cluster finder)
            tools.append(taualgs.getJetSeedBuilder())
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            tools.append(taualgs.getTauAxis())
            tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=2))
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))

            tools.append(taualgs.getCellVariables(cellConeSize=0.2))  #cellConeSize 0.2!!

            #tools.append(taualgs.getElectronVetoVars())  #needed?
            tools.append(taualgs.getTauVertexVariables())
            tools.append(taualgs.getTauCommonCalcVars())
            tools.append(taualgs.getTauSubstructure())
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            
            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            #necessary to write out deltaZ0 between tracks and lead trk
            #self.theTauPVTool = taualgs.getTauPVTrackTool()
            #self.useTauPVTool = True;

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()


class TrigTauRecMerged_TauPreselection (TrigTauRecMerged) :

        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_TauPreselection"):
            super( TrigTauRecMerged_TauPreselection , self ).__init__( name )
            self._mytools = []
            
            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
                
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTauPreselection_")
            
            # Collection name
            self.OutputCollection = "TrigTauRecPreselection"
            
            # Only include tools needed for pre-selection
            
            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Associate RoI vertex or Beamspot to tau - don't use TJVA - no vertices with fast-tracking
            # tools.append(taualgs.getTauVertexFinder(doUseTJVA=False))
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
            tools.append(taualgs.getTauAxis())
            # Count tracks with deltaZ0 cut of 2mm -> Need to remove quality criteria for fast-tracks here
            # Insert bypass later?
            # Count tracks with deltaZ0 cut of 2mm for 2016 and 1mm for 2017 (see ATR-15845)
            if TriggerFlags.run2Config == '2016':
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=2, noSelector=False))
            else:
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=1, noSelector=False))
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # tools.append(taualgs.getElectronVetoVars())
            # Variables combining tracking and calorimeter information
            tools.append(taualgs.getTauCommonCalcVars())
            # Cluster-based sub-structure, with dRMax also
            tools.append(taualgs.getTauSubstructure())
            # tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            tools.append(taualgs.getPileUpCorrection())


            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools
                
            #necessary to write out deltaZ0 between tracks and lead trk
            #self.theTauPVTool = taualgs.getTauPVTrackTool()
            #self.useTauPVTool = True;

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()


class TrigTauRecMerged_TauFTK (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_TauFTK"):
            super( TrigTauRecMerged_TauFTK , self ).__init__( name )
            self._mytools = []
            
            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
                
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]
 
            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTauFTK_")
            
            
            # Collection name
            self.OutputCollection = "TrigTauRecPreselection"
            
            # Only include tools needed for pre-selection
            
            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            #use FTK vertices
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=True))
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
            tools.append(taualgs.getTauAxis())
            # Count tracks with deltaZ0 cut of 2mm -> Need to remove quality criteria for fast-tracks here
            # Insert bypass later?
            tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=2, noSelector = False))
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # tools.append(taualgs.getElectronVetoVars())
            # Variables combining tracking and calorimeter information
            tools.append(taualgs.getTauCommonCalcVars())
            # Cluster-based sub-structure, with dRMax also
            tools.append(taualgs.getTauSubstructure())
            # tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            tools.append(taualgs.getPileUpCorrection())


            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools
                
            #necessary to write out deltaZ0 between tracks and lead trk
            #self.theTauPVTool = taualgs.getTauPVTrackTool()
            #self.useTauPVTool = True;

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()            

class TrigTauRecMerged_TauCaloOnly (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_TauCaloOnly"):
            super( TrigTauRecMerged_TauCaloOnly , self ).__init__( name )
            self._mytools = []
            
            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
                
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTauCaloOnly_")
            
            # Collection name - not stored
            self.OutputCollection = "TrigTauRecCaloOnly"
            
            # Only include tools needed for calo pre-selection
            
            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
            tools.append(taualgs.getTauAxis())
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy', caloOnly=True))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))

            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()

class TrigTauRecMerged_TauCaloOnlyMVA (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_TauCaloOnlyMVA"):
            super( TrigTauRecMerged_TauCaloOnlyMVA , self ).__init__( name )
            self._mytools = []

            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring
            validation = TrigTauRecValidationMonitoring()
            online     = TrigTauRecOnlineMonitoring()

            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTauCaloOnlyMVA_")

            # Collection name - not stored
            self.OutputCollection = "TrigTauRecCaloOnly"

            # Only include tools needed for calo pre-selection

            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
            tools.append(taualgs.getTauAxis())
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy', caloOnly=True))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()


class TrigTauRecMerged_TauPrecision (TrigTauRecMerged) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecMerged_TauPrecision"):
            super( TrigTauRecMerged_TauPrecision , self ).__init__( name )
            self._mytools = []

            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
            
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTau_")

            # Collection name
            self.OutputCollection = "TrigTauRecMerged"

            # Include full set of tools

            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Associate RoI vertex or Beamspot to tau - don't use TJVA
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)       
            tools.append(taualgs.getTauAxis())
            # Count tracks with deltaZ0 cut of 2mm for 2016 and 1mm for 2017 (see ATR-15845)
            if TriggerFlags.run2Config == '2016':
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=2, prefix='TrigTau2016_'))
            else:
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=1))
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # tools.append(taualgs.getElectronVetoVars())
            # Lifetime variables
            tools.append(taualgs.getTauVertexVariables())
            # Variables combining tracking and calorimeter information
            tools.append(taualgs.getTauCommonCalcVars())
            # Cluster-based sub-structure, with dRMax also
            tools.append(taualgs.getTauSubstructure())
            # tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            tools.append(taualgs.getPileUpCorrection())
            
            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()



class TrigTauRecMerged_TauPrecisionMVA (TrigTauRecMerged) :
        __slots__ = [ '_mytools']

        def __init__(self, name = "TrigTauRecMerged_TauPrecisionMVA", doMVATES=False, doTrackBDT=False, doRNN=False):
        
            super( TrigTauRecMerged_TauPrecisionMVA , self ).__init__( name )

            self._mytools = []

            # monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
            from TrigTauRec.TrigTauRecMonitoring import TrigTauRecValidationMonitoring, TrigTauRecOnlineMonitoring 
            validation = TrigTauRecValidationMonitoring()        
            online     = TrigTauRecOnlineMonitoring()
            
            from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
            time = TrigTimeHistToolConfig("Time")
            self.AthenaMonTools = [ time, validation, online ]

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            # using same prefix as in TauPrecision sequence should be safe if tools with different configurations have different names
            # e.g. TauTrackFinder in 2016 using dz0=2mm instead of 1mm in 2017
            taualgs.setPrefix("TrigTau_")

            # Collection name
            self.OutputCollection = "TrigTauRecMerged"

            # Include full set of tools

            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Associate RoI vertex or Beamspot to tau - don't use TJVA
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)       
            tools.append(taualgs.getTauAxis())
            
            # for now, use 'doMVATES=False' to detect tracktwoEF, instead of introducing new flag
            if not doMVATES:
                # Count tracks with deltaZ0 cut of 1mm for tracktwoEF           
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=1))
            else:
                # tightened to 0.75 mm for tracktwoMVA (until the track BDT can be used)
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=0.75, prefix='TrigTauTightDZ_'))            

            if doTrackBDT:                
                # BDT track classification
                tools.append(taualgs.getTauTrackClassifier())

            # Calibrate to calo TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))

            if doMVATES:
                # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
                tools.append(taualgs.getMvaTESVariableDecorator())
                tools.append(taualgs.getMvaTESEvaluator())

            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # tools.append(taualgs.getElectronVetoVars())
            # Lifetime variables
            tools.append(taualgs.getTauVertexVariables())
            # Variables combining tracking and calorimeter information
            tools.append(taualgs.getTauCommonCalcVars())
            # Cluster-based sub-structure, with dRMax also
            tools.append(taualgs.getTauSubstructure())
            # tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            tools.append(taualgs.getPileUpCorrection())

            if doRNN:
                # RNN tau ID
                tools.append(taualgs.getTauJetRNNEvaluator(NetworkFile0P="rnnid_config_0p_v3.json",
                                                           NetworkFile1P="rnnid_config_1p_v3.json",
                                                           NetworkFile3P="rnnid_config_mp_v3.json",
                                                           MaxTracks=10, 
                                                           MaxClusters=6,
                                                           MaxClusterDR=1.0))
                # flattened RNN score and WP
                tools.append(taualgs.getTauWPDecoratorJetRNN())


            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()


            
#end
