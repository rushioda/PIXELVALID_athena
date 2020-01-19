# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import VnHypo

from AthenaCommon.SystemOfUnits import TeV,GeV
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class VnMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="VnMonitoring", harmonic=1):
        super(VnMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        self.Histograms += [ defineHistogram('TotalEt',
                                             type='TH1F',
                                             title="Total ET; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=0.5*TeV)]
        self.Histograms += [ defineHistogram('TotalEtPassing',
                                             type='TH1F',
                                             title="Total ET for passing events; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=0.5*TeV)]


        self.Histograms += [ defineHistogram('q',
                                             type='TH1F',
                                             title="q%d of checked events" % harmonic,
                                             xbins = 50, xmin=0.0, xmax=0.25)]

        self.Histograms += [ defineHistogram('qPassing',
                                             type='TH1F',
                                             title="q%d of passing events" % harmonic,
                                             xbins = 50, xmin=0.0, xmax=0.25)]
        

        self.Histograms += [ defineHistogram('TotalEt, decision',
                                             type='TH2F',
                                             title="(all row 0) (passing row 1); FCal Et",
                                             xbins = 100, xmin=0.0, xmax=0.5*TeV,
                                             ybins=2, ymin=-0.5, ymax=1.5) ]

class VnBootstrap(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="VnBootstrap", harmonic=1):
        super(VnBootstrap, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])

        self.Histograms += [ defineHistogram('TotalEt, q',
                                             type='TH2F',
                                             title="q%d vs Fcal of all events" % harmonic,
                                             xbins=100, xmin=0*GeV, xmax=0.5*TeV,
                                             ybins=100, ymin=0, ymax=0.3) ]            

class QZeroMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="QZeroMonitoring", harmonic=1, qmax=0.3):
        super(QZeroMonitoring, self).__init__(name)        
        self.defineTarget(['Online', 'Validation'])

#        self.Histograms += [ defineHistogram('TotalEt, qnx', type="TH2F", title="TotalEt[TeV];q_{%d x 0};q_{x0}" % harmonic, xbins=100, xmin=0, xmax=4.5*TeV, ybins=30, ymin=-qmax, ymax=qmax) ]
#        self.Histograms += [ defineHistogram('TotalEt, qny', type="TH2F", title="TotalEt[TeV];q_{%d y 0};q_{y0}" % harmonic, xbins=100, xmin=0, xmax=4.5*TeV, ybins=30, ymin=-qmax, ymax=qmax) ]
        self.Histograms += [ defineHistogram('TotalEt, qnx', type="TProfile", title="FCal Event q0x;TotalEt[TeV];q_{%d x 0}" % harmonic, xbins=100, xmin=0, xmax=0.5*TeV) ]
        self.Histograms += [ defineHistogram('TotalEt, qny', type="TProfile", title="FCal Event q0y;TotalEt[TeV];q_{%d y 0}" % harmonic, xbins=100, xmin=0, xmax=0.5*TeV) ]


_EtCuts_v2=[ x*1e-3 for x in reversed(range(300, 6050, 50)) ] # equidistant bins
_EtCuts_v3=[ x*1e-3 for x in reversed(range(400, 6050, 50)) ]

_EtCuts_v2_assym = [ x*1e-3 for x in reversed(range(100, 303, 2))]

########################################################################################
# V2

q2xshift=0.0036
q2yshift=-0.00034

class V2Hypo(VnHypo):
    __slots__ = []
    def __init__(self, name):
        super( VnHypo, self ).__init__( name )
        self.FlowHarmonic = 2
        self.CentralityBins=_EtCuts_v2
        self.QxShifts = [q2xshift]*len(_EtCuts_v2)
        self.QyShifts = [q2yshift]*len(_EtCuts_v2)
        self.AthenaMonTools += [VnMonitoring(name="VnMonitoring", harmonic=self.FlowHarmonic)]
        
V2_th16 = V2Hypo("V2_th16")
V2_th16.AthenaMonTools += [QZeroMonitoring(name="QZeroMonitoring", harmonic=2, qmax=2), VnBootstrap(name="V2Bootstrap", harmonic=2)]
V2_th16.CentralityBins=[-0.5]
V2_th16.QThresholds=[1e3]
V2_th16.QxShifts=[q2xshift]
V2_th16.QyShifts=[q2yshift]


def generateVnThresholds(a,b,c, etbins):
    return [ a - b*et +c*pow(et, 2)  for et in etbins]
    
def generateAssymVnThresholds(a,b, etbins):
    ret = [ a + 1.e6*b*et for et in etbins]
    ret[0] = 1e9
    return ret
# thresholds mappint
#      %v2     %v3
#15    2.5     2.5
#14    5       5 
#13    10      10
#10    1       1 
#5     0.5     20

V2_th15 = V2Hypo("V2_th15")            
V2_th15.QThresholds = generateVnThresholds(0.144638, 0.0239043, -0.000368144, _EtCuts_v2)

V2_th14 = V2Hypo("V2_th14")            
V2_th14.QThresholds = generateVnThresholds(0.134306, 0.0213092, -0.00069103, _EtCuts_v2)

V2_th13 = V2Hypo("V2_th13")
V2_th13.QThresholds = generateVnThresholds(0.12048, 0.0172576, -0.0011478, _EtCuts_v2)


V2_th10 = V2Hypo("V2_th10")
V2_th10.QThresholds = generateVnThresholds(0.158364, 0.0281745, 0.000147842, _EtCuts_v2)

V2_th5 = V2Hypo("V2_th5")
V2_th5.QThresholds =  generateVnThresholds(0.165639, 0.0295911, 0.00035603, _EtCuts_v2)


V2_th = {16: V2_th16, 15: V2_th15, 14: V2_th14, 13: V2_th13, 10: V2_th10, 5: V2_th5 }
def V2(names, threshold):
    return V3_th[threshold]


V2_th1_veto = V2Hypo("V2_th1_veto")
V2_th1_veto.UpperLimit=True
V2_th1_veto.QThresholds = generateVnThresholds(0.0330088, -0.00256366, -0.00233638, _EtCuts_v2)




class V2HypoAssym(VnHypo):
    __slots__ = []
    def __init__(self, name, side):
        super( VnHypo, self ).__init__( name )
        self.FlowHarmonic = 2
        self.CentralityBins = _EtCuts_v2_assym 
        if side == "A":
            self.MinEta=3.2 
            self.QxShifts = [0.00289428]*len(_EtCuts_v2_assym)
            self.QyShifts = [0.00369803]*len(_EtCuts_v2_assym)

        elif side == "C":
            self.MaxEta=-3.2
            self.QxShifts = [-0.0015]*len(_EtCuts_v2_assym)
            self.QyShifts = [0.0039]*len(_EtCuts_v2_assym)
        else:
            raise 'The V2 hypothesis algorithm is insufficiently configured, no FCal side specified '

        self.AthenaMonTools += [VnMonitoring(name="VnMonitoring", harmonic=self.FlowHarmonic)]
    

V2A_th1 = V2HypoAssym("V2A_th1", "A")
V2A_th1.QThresholds = generateVnThresholds(0,0,0, _EtCuts_v2_assym)

V2A_th2 = V2HypoAssym("V2A_th2", "A")
V2A_th2.QThresholds = generateVnThresholds(0,0,0, _EtCuts_v2_assym)

V2A_th3 = V2HypoAssym("V2A_th3", "A")
V2A_th3.QThresholds = generateVnThresholds(0,0,0, _EtCuts_v2_assym)

V2A_thresholds = {1: V2A_th1, 2: V2A_th2, 3: V2A_th3}


V2A_th0p = V2HypoAssym("V2A_th0p", "A")
V2A_th0p.QThresholds = generateAssymVnThresholds( 10, 0, _EtCuts_v2_assym)
V2A_th0p.AthenaMonTools += [ VnBootstrap(name="VnBootstrap", harmonic=2), QZeroMonitoring(name="QZeroMonitoring", harmonic=2, qmax=2) ]

V2A_th005p = V2HypoAssym("V2A_th005p", "A")
V2A_th005p.QThresholds = generateAssymVnThresholds( 0.3043, -7.20e-07  , _EtCuts_v2_assym)

V2A_th01p = V2HypoAssym("V2A_th01p", "A")
V2A_th01p.QThresholds = generateAssymVnThresholds(  0.280792, -6.50e-07, _EtCuts_v2_assym)

V2A_th025p = V2HypoAssym("V2A_th025p", "A")
V2A_th025p.QThresholds = generateAssymVnThresholds( 0.22820, -4.09e-07, _EtCuts_v2_assym)

V2A_th05p = V2HypoAssym("V2A_th05p", "A")
V2A_th05p.QThresholds = generateAssymVnThresholds(  0.194324, -2.90e-07, _EtCuts_v2_assym)

V2A_th10p = V2HypoAssym("V2A_th10p", "A")
V2A_th10p.QThresholds = generateAssymVnThresholds(  0.174735, -2.93e-07, _EtCuts_v2_assym)

V2A_th15p = V2HypoAssym("V2A_th15p", "A")
V2A_th15p.QThresholds = generateAssymVnThresholds(  0.151147, -2.12e-07, _EtCuts_v2_assym)

V2A_th20p = V2HypoAssym("V2A_th20p", "A")
V2A_th20p.QThresholds = generateAssymVnThresholds(  0.140162, -2.03e-07, _EtCuts_v2_assym)


V2A_thresholds = {"1": V2A_th1, "2": V2A_th2, "3": V2A_th3, 
                  "0p": V2A_th0p,
                  "005p": V2A_th005p, "01p": V2A_th01p,
                  "025p": V2A_th025p, "05p": V2A_th05p,
                  "10p": V2A_th10p,   "15p": V2A_th15p,
                  "20p": V2A_th20p}



V2C_th1 = V2HypoAssym("V2C_th1", "C")
V2C_th1.QThresholds = generateVnThresholds(0,0,0, _EtCuts_v2_assym)

V2C_th2 = V2HypoAssym("V2C_th2", "C")
V2C_th2.QThresholds = generateVnThresholds(0,0,0, _EtCuts_v2_assym)

V2C_th3 = V2HypoAssym("V2C_th3", "C")
V2C_th3.QThresholds = generateVnThresholds(0,0,0, _EtCuts_v2_assym)
        
V2C_th0p = V2HypoAssym("V2C_th0p", "C")
V2C_th0p.QThresholds = generateAssymVnThresholds( 10, 0, _EtCuts_v2_assym)
V2C_th0p.AthenaMonTools += [ VnBootstrap(name="VnBootstrap", harmonic=2), QZeroMonitoring(name="QZeroMonitoring", harmonic=2, qmax=2) ]

V2C_th005p = V2HypoAssym("V2C_th005p", "C")
V2C_th005p.QThresholds = generateAssymVnThresholds( 0.3043, -7.20e-07  , _EtCuts_v2_assym)
                                                                       
V2C_th01p = V2HypoAssym("V2C_th01p", "C")                              
V2C_th01p.QThresholds = generateAssymVnThresholds(  0.280792, -6.50e-07, _EtCuts_v2_assym)
                                                                       
V2C_th025p = V2HypoAssym("V2C_th025p", "C")                            
V2C_th025p.QThresholds = generateAssymVnThresholds( 0.22820, -4.09e-07, _EtCuts_v2_assym)
                                                                       
V2C_th05p = V2HypoAssym("V2C_th05p", "C")                              
V2C_th05p.QThresholds = generateAssymVnThresholds(  0.194324, -2.90e-07, _EtCuts_v2_assym)
                                                                       
V2C_th10p = V2HypoAssym("V2C_th10p", "C")                              
V2C_th10p.QThresholds = generateAssymVnThresholds(  0.174735, -2.93e-07, _EtCuts_v2_assym)
                                                                       
V2C_th15p = V2HypoAssym("V2C_th15p", "C")                              
V2C_th15p.QThresholds = generateAssymVnThresholds(  0.151147, -2.12e-07,  _EtCuts_v2_assym)
                                                                       
V2C_th20p = V2HypoAssym("V2C_th20p", "C")                              
V2C_th20p.QThresholds = generateAssymVnThresholds(  0.140162, -2.03e-07, _EtCuts_v2_assym)

V2C_thresholds = {"1": V2C_th1, "2": V2C_th2, "3": V2C_th3, 
                  "0p": V2C_th0p,
                  "005p": V2C_th005p, "01p": V2C_th01p,
                  "025p": V2C_th025p, "05p": V2C_th05p,
                  "10p": V2C_th10p,   "15p": V2C_th15p,
                  "20p": V2C_th20p}

def V2Assym(threshold, side):
    if side == "A":
        return V2A_thresholds[threshold]
    if side == "C":
        return V2C_thresholds[threshold]
    raise "Side of the detector has to be either A or C"

########################################################################################
# V3
q3xshift=-0.0017
q3yshift=0.0013



class V3Hypo(VnHypo):
    __slots__ = []
    def __init__(self, name):
        super( VnHypo, self ).__init__( name )
        self.FlowHarmonic = 3
        self.CentralityBins = _EtCuts_v3
        self.QxShifts = [q3xshift]*len(_EtCuts_v3)
        self.QyShifts =  [q3yshift]*len(_EtCuts_v3)
        self.AthenaMonTools += [VnMonitoring(name="VnMonitoring", harmonic=self.FlowHarmonic)]
        
V3_th16 =V3Hypo("V3_th16")
V3_th16.CentralityBins=[-0.5]
V3_th16.QThresholds=[1e3]
V3_th16.QxShifts=[q3xshift]
V3_th16.QyShifts=[q3yshift]

V3_th16.AthenaMonTools += [QZeroMonitoring(name="QZeroMonitoring", harmonic=3, qmax=2), VnBootstrap(name="V3Bootstrap", harmonic=3)]

V3_th15 = V3Hypo("V3_th15")
V3_th15.QThresholds = generateVnThresholds(0.0680338, 0.0128576, 0.000901204, _EtCuts_v3)

V3_th14 = V3Hypo("V3_th14")
V3_th14.QThresholds = generateVnThresholds(0.0632529, 0.0135102, 0.00102213, _EtCuts_v3)

V3_th13 = V3Hypo("V3_th13")
V3_th13.QThresholds = generateVnThresholds(0.0549295, 0.0111602, 0.0006531, _EtCuts_v3)

V3_th10 = V3Hypo("V3_th10")
V3_th10.QThresholds = generateVnThresholds(0.0777345, 0.0161675, 0.00145098, _EtCuts_v3)


V3_th5 = V3Hypo("V3_th5")
V3_th5.QThresholds = generateVnThresholds(0.0456977, 0.00745393, -2.59157e-05,  _EtCuts_v3)

V3_th = {16: V3_th16, 15: V3_th15, 14: V3_th14, 13: V3_th13, 10: V3_th10, 5: V3_th5 }
def V3(names, threshold):
    return V3_th[threshold]


V3_th1_veto = V3Hypo("V3_th1_veto")
V3_th1_veto.UpperLimit=True
V3_th1_veto.QThresholds = generateVnThresholds(0.0118349, 0.00241723, 0.000276767, _EtCuts_v3)
