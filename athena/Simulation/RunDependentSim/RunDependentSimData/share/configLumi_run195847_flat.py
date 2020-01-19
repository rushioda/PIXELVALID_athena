####################
## File configLumi_run195847_flat.py: autogenerated configuration file from command
##/cvmfs/atlas.cern.ch/repo/sw/software/i686-slc5-gcc43-opt/17.0.6/AtlasSimulation/17.0.6/InstallArea/share/bin/RunDepTaskMaker.py --externalDict={38.0: 148.0, 39.0: 148.0, 40.0: 147.0, 41.0: 147.0, 42.0: 147.0, 43.0: 147.0, 44.0: 147.0, 45.0: 147.0, 46.0: 147.0, 47.0: 147.0, 48.0: 147.0, 49.0: 147.0, 50.0: 147.0, 51.0: 147.0, 52.0: 147.0, 53.0: 147.0, 54.0: 147.0, 55.0: 147.0, 56.0: 147.0, 57.0: 147.0, 58.0: 147.0, 59.0: 147.0, 60.0: 147.0, 61.0: 147.0, 62.0: 147.0, 63.0: 147.0, 64.0: 147.0, 65.0: 147.0, 66.0: 147.0, 67.0: 147.0, 68.0: 147.0, 69.0: 147.0, 70.0: 147.0, 71.0: 147.0} --lumimethod=EXTERNAL --nMC=1 --trigger=L1_MBTS_2 --outfile=configLumi_run195847_flat.py run195847.xml
## Created on Sat Jul 14 03:30:09 2012
####################
#Run-dependent digi job configuration file.
#RunDependentSimData/OverrideRunLBLumiDigitConfig.py

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
   {'run':195847, 'lb':1, 'starttstamp':1328039085, 'dt':0.000, 'evts':_evts(147), 'mu':64.000, 'force_new':False},
   {'run':195847, 'lb':2, 'starttstamp':1328039152, 'dt':0.000, 'evts':_evts(147), 'mu':65.000, 'force_new':False},
   {'run':195847, 'lb':3, 'starttstamp':1328039213, 'dt':0.000, 'evts':_evts(147), 'mu':66.000, 'force_new':False},
   {'run':195847, 'lb':4, 'starttstamp':1328039274, 'dt':0.000, 'evts':_evts(147), 'mu':67.000, 'force_new':False},
   {'run':195847, 'lb':5, 'starttstamp':1328039335, 'dt':0.000, 'evts':_evts(147), 'mu':68.000, 'force_new':False},
   {'run':195847, 'lb':6, 'starttstamp':1328039396, 'dt':0.000, 'evts':_evts(147), 'mu':69.000, 'force_new':False},
   {'run':195847, 'lb':7, 'starttstamp':1328039457, 'dt':0.000, 'evts':_evts(147), 'mu':70.000, 'force_new':False},
   {'run':195847, 'lb':8, 'starttstamp':1328039518, 'dt':0.000, 'evts':_evts(147), 'mu':71.000, 'force_new':False},
   {'run':195847, 'lb':9, 'starttstamp':1328039579, 'dt':0.000, 'evts':_evts(148), 'mu':38.000, 'force_new':False},
   {'run':195847, 'lb':10, 'starttstamp':1328039640, 'dt':0.000, 'evts':_evts(148), 'mu':39.000, 'force_new':False},
   {'run':195847, 'lb':11, 'starttstamp':1328039701, 'dt':0.000, 'evts':_evts(147), 'mu':40.000, 'force_new':False},
   {'run':195847, 'lb':12, 'starttstamp':1328039762, 'dt':0.000, 'evts':_evts(147), 'mu':41.000, 'force_new':False},
   {'run':195847, 'lb':13, 'starttstamp':1328039823, 'dt':0.000, 'evts':_evts(147), 'mu':42.000, 'force_new':False},
   {'run':195847, 'lb':14, 'starttstamp':1328039884, 'dt':0.000, 'evts':_evts(147), 'mu':43.000, 'force_new':False},
   {'run':195847, 'lb':15, 'starttstamp':1328039945, 'dt':0.000, 'evts':_evts(147), 'mu':44.000, 'force_new':False},
   {'run':195847, 'lb':16, 'starttstamp':1328040006, 'dt':0.000, 'evts':_evts(147), 'mu':45.000, 'force_new':False},
   {'run':195847, 'lb':17, 'starttstamp':1328040067, 'dt':0.000, 'evts':_evts(147), 'mu':46.000, 'force_new':False},
   {'run':195847, 'lb':18, 'starttstamp':1328040128, 'dt':0.000, 'evts':_evts(147), 'mu':47.000, 'force_new':False},
   {'run':195847, 'lb':19, 'starttstamp':1328040189, 'dt':0.000, 'evts':_evts(147), 'mu':48.000, 'force_new':False},
   {'run':195847, 'lb':20, 'starttstamp':1328040250, 'dt':0.000, 'evts':_evts(147), 'mu':49.000, 'force_new':False},
   {'run':195847, 'lb':21, 'starttstamp':1328040311, 'dt':0.000, 'evts':_evts(147), 'mu':50.000, 'force_new':False},
   {'run':195847, 'lb':22, 'starttstamp':1328040372, 'dt':0.000, 'evts':_evts(147), 'mu':51.000, 'force_new':False},
   {'run':195847, 'lb':23, 'starttstamp':1328040433, 'dt':0.000, 'evts':_evts(147), 'mu':52.000, 'force_new':False},
   {'run':195847, 'lb':24, 'starttstamp':1328040494, 'dt':0.000, 'evts':_evts(147), 'mu':53.000, 'force_new':False},
   {'run':195847, 'lb':25, 'starttstamp':1328040555, 'dt':0.000, 'evts':_evts(147), 'mu':54.000, 'force_new':False},
   {'run':195847, 'lb':26, 'starttstamp':1328040616, 'dt':0.000, 'evts':_evts(147), 'mu':55.000, 'force_new':False},
   {'run':195847, 'lb':27, 'starttstamp':1328040677, 'dt':0.000, 'evts':_evts(147), 'mu':56.000, 'force_new':False},
   {'run':195847, 'lb':28, 'starttstamp':1328040738, 'dt':0.000, 'evts':_evts(147), 'mu':57.000, 'force_new':False},
   {'run':195847, 'lb':29, 'starttstamp':1328040799, 'dt':0.000, 'evts':_evts(147), 'mu':58.000, 'force_new':False},
   {'run':195847, 'lb':30, 'starttstamp':1328040860, 'dt':0.000, 'evts':_evts(147), 'mu':59.000, 'force_new':False},
   {'run':195847, 'lb':31, 'starttstamp':1328040921, 'dt':0.000, 'evts':_evts(147), 'mu':60.000, 'force_new':False},
   {'run':195847, 'lb':32, 'starttstamp':1328040982, 'dt':0.000, 'evts':_evts(147), 'mu':61.000, 'force_new':False},
   {'run':195847, 'lb':33, 'starttstamp':1328041043, 'dt':0.000, 'evts':_evts(147), 'mu':62.000, 'force_new':False},
   {'run':195847, 'lb':34, 'starttstamp':1328041104, 'dt':0.000, 'evts':_evts(147), 'mu':63.000, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker