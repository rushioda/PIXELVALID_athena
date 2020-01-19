# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

cutNames = ['No MET obj', 'Rejected', 'MET', 'SumET', 'Both', 'Forward', 'Significance' ]
labelsDescription = ''
for a in cutNames:
    labelsDescription += a+':'

### histograms
hCuts      = defineHistogram('Hypo_MET_cuts',  type='TH1F', title="Missing E_{T} Hypo cuts", xbins=6, xmin=-1.5, xmax=5.5, labels=labelsDescription) 

hEx_log    = defineHistogram('Hypo_MEx_log',   type='TH1F', title="Missing E_{x};sgn(ME_{x}) log_{10}(ME_{x}/GeV)", xbins=41, xmin=-5.075, xmax=5.075)
hEy_log    = defineHistogram('Hypo_MEy_log',   type='TH1F', title="Missing E_{y};sgn(ME_{y}) log_{10}(ME_{y}/GeV)", xbins=41, xmin=-5.075, xmax=5.075)
hEz_log    = defineHistogram('Hypo_MEz_log',   type='TH1F', title="Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)", xbins=41, xmin=-5.075, xmax=5.075)
hMET_log   = defineHistogram('Hypo_MET_log',   type='TH1F', title="|Missing E_{T}|;log_{10}(ME_{T}/GeV)",           xbins=35, xmin=-1.875, xmax=5.375)
hSumEt_log = defineHistogram('Hypo_SumEt_log', type='TH1F', title="Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",             xbins=35, xmin=-1.875, xmax=5.125)

hEx_lin    = defineHistogram('Hypo_MEx_lin',   type='TH1F', title="Missing E_{x};ME_{x} (GeV)",    xbins=199, xmin=-298.5, xmax=298.5)
hEy_lin    = defineHistogram('Hypo_MEy_lin',   type='TH1F', title="Missing E_{y};ME_{y} (GeV)",    xbins=199, xmin=-298.5, xmax=298.5)
hEz_lin    = defineHistogram('Hypo_MEz_lin',   type='TH1F', title="Missing E_{z};ME_{z} (GeV)",    xbins=199, xmin=-298.5, xmax=298.5)
hMET_lin   = defineHistogram('Hypo_MET_lin',   type='TH1F', title="|Missing E_{T}|;ME_{T} (GeV)",  xbins=105, xmin=-13.5,  xmax=301.5)
hSumEt_lin = defineHistogram('Hypo_SumEt_lin', type='TH1F', title="Sum |E_{T}|;SumE_{T} (GeV)",    xbins=155, xmin=-27.,   xmax=2000.)

hMETPhi    = defineHistogram('Hypo_MET_phi',   type='TH1F', title="MET #phi;#phi (rad)",           xbins=32, xmin=-3.1416, xmax=3.1416) 
hXS        = defineHistogram('Hypo_XS',        type='TH1F', title="EF Significance; (XS/GeV^{1/2})",         xbins=40,  xmin=-0.025,   xmax=20.025)
hXS2       = defineHistogram('Hypo_XS2',        type='TH1F', title="EF Significance 2; (XS2/GeV^{1/2})",         xbins=40,  xmin=-0.025,   xmax=20.025)



class TrigEFMissingETHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETHypoValidationMonitoring"):
        super(TrigEFMissingETHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms  = [ hCuts ]
        self.Histograms += [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
        self.Histograms += [ hEx_lin, hEy_lin, hEz_lin, hMET_lin, hSumEt_lin, hMETPhi, hXS, hXS2 ]


class TrigEFMissingETHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETHypoOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFMissingETHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms  = [ hCuts ]


class TrigEFMissingETHypoFullOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETHypoFullOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFMissingETHypoFullOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
        self.Histograms += [ hEx_lin, hEy_lin, hEz_lin, hMET_lin, hSumEt_lin, hMETPhi, hXS, hXS2 ]


class TrigEFMissingETHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETHypoCosmicMonitoring"):
        """ defines histograms for cosmic """
        super(TrigEFMissingETHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms  = [ hCuts ]
