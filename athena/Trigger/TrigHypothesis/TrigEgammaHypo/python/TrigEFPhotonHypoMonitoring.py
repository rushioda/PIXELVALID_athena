# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigEFPhotonHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFPhotonHypoValidationMonitoring"):
        super(TrigEFPhotonHypoValidationMonitoring, self).__init__(name)
        self.defineTarget( "Validation" )

        cuts=['Input','has egammaContainer','isEM','isEMTrig', 'has caloCluster', 'Cluster Et', 'Isolation', 
              'Photon','OldSet2','OldSet3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="EFPhoton Hypo cut counter; Cut",
                                             xbins=10, xmin=0.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ]    

        #----------------------------------------------------------
        #AT Jan2010
        #----BitDef for Trigger-coded 28-Bit isEMTrig 
        cuts=['ClusterEtaRange','ClusterHadronicLeakage','empty2','empty3','ClusterMiddleEnergy','ClusterMiddleEratio37 ',
              'ClusterMiddleEratio33','ClusterMiddleWidth','ClusterStripsEratio','ClusterStripsDeltaEmax2',
              'ClusterStripsDeltaE ','ClusterStripsWtot','ClusterStripsFracm','ClusterStripsWeta1c',
              'ClusterIsolation','ClusterStripsDEmaxs1','TrackBlayer','TrackPixel','TrackSi','TrackA0','TrackMatchEta',
              'TrackMatchPhi','TrackMatchEoverP','empty23','TrackTRThits','TrackTRTratio','TrackTRTratio90', 'TrackIsolation']        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
 

        #
        #----BitDefPhoton for Offline 32-Bit isEM (valid for LowLumi, at HighLumi must use BitDefPhoton)
        #
        cuts=['ClusterEtaRange ','ConversionMatch','ClusterHadronicLeakage ','ClusterMiddleEnergy ',
              'ClusterMiddleEratio37 ','ClusterMiddleEratio33 ','ClusterMiddleWidth ','f3',
              'ClusterStripsEratio ','ClusterStripsDeltaEmax2 ','ClusterStripsDeltaE ',
              'ClusterStripsWtot ','ClusterStripsFracm ','ClusterStripsWeta1c ','empty14',
              'ClusterStripsDEmaxs1 ','TrackBlayer ','TrackPixel ','TrackSi ',
              'TrackA0 ','TrackMatchEta ','TrackMatchPhi ','TrackMatchEoverP ','empty23',
              'TrackTRThits ','TrackTRTratio ','TrackTRTratio90 ','TrackA0Tight','TrackMatchEtaTight',
              'Isolation ','ClusterIsolation ','TrackIsolation ']
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #isEMTrig Monitoring Histograms - Before/After Offline isEM cuts are applied        
        self.Histograms += [ defineHistogram('IsEMAfterCut', type='TH1I', title="EFPhoton Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMBeforeCut', type='TH1I', title="EFPhoton Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMRequiredBitsBeforeCut', type='TH1I', title="Required Bits for isEM flag Before Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMRequiredBitsAfterCut', type='TH1I', title="Required Bits for isEM flag After Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]       
        #----------------------------------------------------------


        cuts=["loose","medium","tight"]
        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

	#AT Aug2011: deactivate histogram egIsEM - outdated
        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaHypo isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]
       
        self.Histograms += [ defineHistogram('Ph_ClusterEt', 	type='TH1F', title="EFPhoton Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('Ph_F1', 	type='TH1F', title="EFPhoton Hypo fraction of energy found in 1st em sampling;Fraction",
                                             xbins=50, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('Ph_E237', 	type='TH1F', title="EFPhoton Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_E277', 	type='TH1F', title="EFPhoton Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]
                                             
        self.Histograms += [ defineHistogram('Ph_Reta', 	type='TH1F', title="EFPhoton Hypo Reta = E237/E277; Reta",
                                             xbins=130, xmin=-5.0, xmax=1.5 ) ]
                                             
        self.Histograms += [ defineHistogram('Ph_Rphi', 	type='TH1F', title="EFPhoton Hypo Rphi = E233/E277; Rphi",
                                             xbins=130, xmin=-5.0, xmax=1.5 ) ]
                                             
        self.Histograms += [ defineHistogram('Ph_Rhad', 	type='TH1F', title="EFPhoton Hypo Rhad = Ethad/et; Rhad",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        self.Histograms += [ defineHistogram('Ph_Rhad1', 	type='TH1F', title="EFPhoton Hypo Rhad1 = Ethad1/et; Rhad1",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        
        self.Histograms += [ defineHistogram('Ph_F3core',	 type='TH1F', title="EFPhoton Hypo F3; E3(3x3)/(E0+E1+E2+E3)",xbins=96, xmin=-0.1, xmax=1.1) ]       
        
        self.Histograms += [ defineHistogram('Ph_Eta',	 type='TH1F', title="EFPhoton Hypo Eta; #eta ",xbins=100, xmin=-2.5, xmax=2.5) ]       

        self.Histograms += [ defineHistogram('Ph_Phi',	 type='TH1F', title="EFPhoton Hypo Phi; #eta ",xbins=160, xmin=-3.2, xmax=3.2) ]       
        
        self.Histograms += [ defineHistogram('Ph_ClusterEt37', 	type='TH1F', title="EFPhoton Hypo Cluster(3x7) E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE0', 	type='TH1F', title="EFPhoton Hypo Cluster E0; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE1', 	type='TH1F', title="EFPhoton Hypo Cluster E1; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE2', 	type='TH1F', title="EFPhoton Hypo Cluster E2; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE3', 	type='TH1F', title="EFPhoton Hypo Cluster E3; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_Eaccordion', 	type='TH1F', title="EFPhoton Hypo Cluster (E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_E0Eaccordion', 	type='TH1F', title="EFPhton Hypo Cluster E0/(E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('Ph_Eratio', 	type='TH1F', title="EFPhoton Hypo Eratio = (emax1 - emax2) / (emax1 + emax2) ; Eratio ",
                                             xbins=32, xmin=-0.1, xmax=1.5 ) ]

        self.Histograms += [ defineHistogram('Ph_EtHad1', 	type='TH1F', title="EFPhoton Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_WEta1', 	type='TH1F', title="EFPhoton Hypo corrected width in 3 strips in the 1st samp. ; WEta1",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('Ph_WEta2', 	type='TH1F', title="EFPhoton  Hypo corrected width in 3 strips in the 2st samp. ; WEta2",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('Ph_Emax2', 	type='TH1F', title="EFPhoton Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_Emins1', 	type='TH1F', title="EFPhoton Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_Fracs1', 	type='TH1F', title="EFPhoton Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('Ph_wtots1', 	type='TH1F', title="EFPhoton Hypo total width in em sampling 1 in 20 strips; WTots1",
                                             xbins=20, xmin=0, xmax=20 ) ]

        self.Histograms += [ defineHistogram('Ph_EtCone20', 	type='TH1F', title="EFPhoton Hypo Et in a ring of DR<0.20 above noise (excluding electron ET); ET [MeV]",
                                             xbins=60, xmin=-10000, xmax=50000 ) ]


class TrigEFPhotonHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFPhotonHypoOnlineMonitoring"):
        super(TrigEFPhotonHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        cuts=['Input','has egammaContainer','isEM','isEMTrig', 'has caloCluster', 'Cluster Et', 'Isolation', 
              'Photon','OldSet2','OldSet3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
            
        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="EFPhoton Hypo cut counter; Cut",
                                             xbins=10, xmin=0.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ]    

        #----------------------------------------------------------
        #AT Jan2010
        #----BitDef for Trigger-coded 28-Bit isEMTrig 
        cuts=['ClusterEtaRange','ClusterHadronicLeakage','empty2','empty3','ClusterMiddleEnergy','ClusterMiddleEratio37 ',
              'ClusterMiddleEratio33','ClusterMiddleWidth','ClusterStripsEratio','ClusterStripsDeltaEmax2',
              'ClusterStripsDeltaE ','ClusterStripsWtot','ClusterStripsFracm','ClusterStripsWeta1c',
              'ClusterIsolation','ClusterStripsDEmaxs1','TrackBlayer','TrackPixel','TrackSi','TrackA0','TrackMatchEta',
              'TrackMatchPhi','TrackMatchEoverP','empty23','TrackTRThits','TrackTRTratio','TrackTRTratio90', 'TrackIsolation']        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #
        #----BitDefPhoton for Offline 32-Bit isEM (valid for LowLumi, at HighLumi must use BitDefPhoton)
        #
        cuts=['ClusterEtaRange ','ConversionMatch','ClusterHadronicLeakage ','ClusterMiddleEnergy ',
              'ClusterMiddleEratio37 ','ClusterMiddleEratio33 ','ClusterMiddleWidth ','f3',
              'ClusterStripsEratio ','ClusterStripsDeltaEmax2 ','ClusterStripsDeltaE ',
              'ClusterStripsWtot ','ClusterStripsFracm ','ClusterStripsWeta1c ','empty14',
              'ClusterStripsDEmaxs1 ','TrackBlayer ','TrackPixel ','TrackSi ',
              'TrackA0 ','TrackMatchEta ','TrackMatchPhi ','TrackMatchEoverP ','empty23',
              'TrackTRThits ','TrackTRTratio ','TrackTRTratio90 ','TrackA0Tight','TrackMatchEtaTight',
              'Isolation ','ClusterIsolation ','TrackIsolation ']
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #isEMTrig Monitoring Histograms - Before/After Offline isEM cuts are applied        
        self.Histograms += [ defineHistogram('IsEMAfterCut', type='TH1I', title="EFPhoton Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMBeforeCut', type='TH1I', title="EFPhoton Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMRequiredBitsBeforeCut', type='TH1I', title="Required Bits for isEM flag Before Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMRequiredBitsAfterCut', type='TH1I', title="Required Bits for isEM flag After Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]              
        #----------------------------------------------------------

        cuts=["loose","medium","tight"]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

	#AT Aug2011: deactivate histogram egIsEM - outdated
        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaHypo isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]

        self.Histograms += [ defineHistogram('Ph_ClusterEt', 	type='TH1F', title="EFPhoton Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_Eta',	 type='TH1F', title="EFPhoton Hypo Eta; #eta ",xbins=100, xmin=-2.5, xmax=2.5) ]       

        self.Histograms += [ defineHistogram('Ph_Phi',	 type='TH1F', title="EFPhoton Hypo Phi; #eta ",xbins=160, xmin=-3.2, xmax=3.2) ]       
        


class TrigEFPhotonHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFPhotonHypoCosmicMonitoring"):
        super(TrigEFPhotonHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        cuts=['Input','has egammaContainer','isEM','isEMTrig', 'has caloCluster', 'Cluster Et', 'Isolation', 
              'Photon','OldSet2','OldSet3']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'
            
       
        self.Histograms  = [ defineHistogram('CutCounter', type='TH1F', title="EFPhoton Hypo cut counter; Cut",
                                             xbins=10, xmin=0.5, xmax=10.5, opt="kCumulative", labels=labelsDescription) ] 


        #----------------------------------------------------------
        #AT Jan2010
        #----BitDef for Trigger-coded 28-Bit isEMTrig 
        cuts=['ClusterEtaRange','ClusterHadronicLeakage','empty2','empty3','ClusterMiddleEnergy','ClusterMiddleEratio37 ',
              'ClusterMiddleEratio33','ClusterMiddleWidth','ClusterStripsEratio','ClusterStripsDeltaEmax2',
              'ClusterStripsDeltaE ','ClusterStripsWtot','ClusterStripsFracm','ClusterStripsWeta1c',
              'ClusterIsolation','ClusterStripsDEmaxs1','TrackBlayer','TrackPixel','TrackSi','TrackA0','TrackMatchEta',
              'TrackMatchPhi','TrackMatchEoverP','empty23','TrackTRThits','TrackTRTratio','TrackTRTratio90', 'TrackIsolation']        
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #
        #----BitDefPhoton for Offline 32-Bit isEM (valid for LowLumi, at HighLumi must use BitDefPhoton)
        #
        cuts=['ClusterEtaRange ','ConversionMatch','ClusterHadronicLeakage ','ClusterMiddleEnergy ',
              'ClusterMiddleEratio37 ','ClusterMiddleEratio33 ','ClusterMiddleWidth ','f3',
              'ClusterStripsEratio ','ClusterStripsDeltaEmax2 ','ClusterStripsDeltaE ',
              'ClusterStripsWtot ','ClusterStripsFracm ','ClusterStripsWeta1c ','empty14',
              'ClusterStripsDEmaxs1 ','TrackBlayer ','TrackPixel ','TrackSi ',
              'TrackA0 ','TrackMatchEta ','TrackMatchPhi ','TrackMatchEoverP ','empty23',
              'TrackTRThits ','TrackTRTratio ','TrackTRTratio90 ','TrackA0Tight','TrackMatchEtaTight',
              'Isolation ','ClusterIsolation ','TrackIsolation ']
        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        #isEMTrig Monitoring Histograms - Before/After Offline isEM cuts are applied        
        self.Histograms += [ defineHistogram('IsEMAfterCut', type='TH1I', title="EFPhoton Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMBeforeCut', type='TH1I', title="EFPhoton Hypo passed cuts from isEM flag; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMRequiredBitsBeforeCut', type='TH1I', title="Required Bits for isEM flag Before Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('IsEMRequiredBitsAfterCut', type='TH1I', title="Required Bits for isEM flag After Cut; cut",
                                             xbins=32, xmin=-0.5, xmax=31.5, opt="kVec", labels=labelsDescription) ]             
        #----------------------------------------------------------
        

        cuts=["loose","medium","tight"]

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

	#AT Aug2011: deactivate histogram egIsEM - outdated
        #self.Histograms += [ defineHistogram('egIsEM', 	type='TH1I', title="EFEgammaHypo isEM; Cut",
        #                                     xbins=3, xmin=0.5, xmax=3.5, labels=labelsDescription)]
       
        self.Histograms += [ defineHistogram('Ph_ClusterEt', 	type='TH1F', title="EFPhoton Hypo Cluster E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
     
        self.Histograms += [ defineHistogram('Ph_F1', 	type='TH1F', title="EFPhoton Hypo fraction of energy found in 1st em sampling;Fraction",
                                             xbins=50, xmin=-0.1, xmax=1.1 ) ]

        self.Histograms += [ defineHistogram('Ph_E237', 	type='TH1F', title="EFPhoton Hypo uncor energy in 3x7 cells in em sampling 2; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_E277', 	type='TH1F', title="EFPhoton Hypo uncor energy in 7x7 cells in em sampling 2;E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]
                                             
        self.Histograms += [ defineHistogram('Ph_Reta', 	type='TH1F', title="EFPhoton Hypo Reta = E237/E277; Reta",
                                             xbins=130, xmin=-5.0, xmax=1.5 ) ]
                                             
        self.Histograms += [ defineHistogram('Ph_Rhad', 	type='TH1F', title="EFPhoton Hypo Rhad = Ethad/et; Rhad",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        self.Histograms += [ defineHistogram('Ph_Rhad1', 	type='TH1F', title="EFPhoton Hypo Rhad1 = Ethad1/et; Rhad1",
                                             xbins=130, xmin=-0.25, xmax=0.25 ) ]
        
        
        self.Histograms += [ defineHistogram('Ph_F3core',	 type='TH1F', title="EFPhoton Hypo F3; E3(3x3)/(E0+E1+E2+E3)",xbins=96, xmin=-0.1, xmax=1.1) ]       
        
        self.Histograms += [ defineHistogram('Ph_Eta',	 type='TH1F', title="EFPhoton Hypo Eta; #eta ",xbins=100, xmin=-2.5, xmax=2.5) ]       

        self.Histograms += [ defineHistogram('Ph_Phi',	 type='TH1F', title="EFPhoton Hypo Phi; #eta ",xbins=160, xmin=-3.2, xmax=3.2) ]       
        
        self.Histograms += [ defineHistogram('Ph_ClusterEt37', 	type='TH1F', title="EFPhoton Hypo Cluster(3x7) E_{T}; E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE0', 	type='TH1F', title="EFPhoton Hypo Cluster E0; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE1', 	type='TH1F', title="EFPhoton Hypo Cluster E1; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE2', 	type='TH1F', title="EFPhoton Hypo Cluster E2; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_EnergyBE3', 	type='TH1F', title="EFPhoton Hypo Cluster E3; E_{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_Eaccordion', 	type='TH1F', title="EFPhoton Hypo Cluster (E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        
        self.Histograms += [ defineHistogram('Ph_E0Eaccordion', 	type='TH1F', title="EFPhton Hypo Cluster E0/(E1+E2+E3); E_{T}^{em} [MeV]",
                                            xbins=50, xmin=-2000, xmax=100000) ]
        self.Histograms += [ defineHistogram('Ph_Eratio', 	type='TH1F', title="EFPhoton Hypo Eratio = (emax1 - emax2) / (emax1 + emax2) ; Eratio ",
                                             xbins=32, xmin=-0.1, xmax=1.5 ) ]

        self.Histograms += [ defineHistogram('Ph_EtHad1', 	type='TH1F', title="EFPhoton Hypo E leakage into 1st sampling of had calo; E [MeV]",
                                             xbins=50, xmin=-15000, xmax=150000 ) ]

        self.Histograms += [ defineHistogram('Ph_WEta1', 	type='TH1F', title="EFPhoton Hypo corrected width in 3 strips in the 1st samp. ; WEta1",
                                             xbins=20, xmin=0, xmax=1 ) ]

        self.Histograms += [ defineHistogram('Ph_WEta2', 	type='TH1F', title="EFPhoton  Hypo corrected width in 3 strips in the 2st samp. ; WEta2",
                                             xbins=20, xmin=0, xmax=0.05 ) ]

        self.Histograms += [ defineHistogram('Ph_Emax2', 	type='TH1F', title="EFPhoton Hypo 2nd maximum in strips; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_Emins1', 	type='TH1F', title="EFPhoton Hypo energy of strip with minimum between max 1 & 2; E [MeV]",
                                             xbins=50, xmin=-1000, xmax=5000 ) ]

        self.Histograms += [ defineHistogram('Ph_Fracs1', 	type='TH1F', title="EFPhoton Hypo energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",
                                             xbins=50, xmin=-5000, xmax=10000 ) ]

        self.Histograms += [ defineHistogram('Ph_wtots1', 	type='TH1F', title="EFPhoton Hypo total width in em sampling 1 in 20 strips; WTots1",
                                             xbins=20, xmin=0, xmax=20 ) ]

        self.Histograms += [ defineHistogram('Ph_EtCone20', 	type='TH1F', title="EFPhoton Hypo Et in a ring of DR<0.20 above noise (excluding electron ET); ET [MeV]",
                                             xbins=60, xmin=-10000, xmax=50000 ) ]
