####################
## File configLumi_run189751.py: autogenerated configuration file from command
##RunDepTaskMaker.py --externalDict={15.25: 158.0, 15.75: 88.0, 6.75: 95.0, 8.25: 228.0, 9.25: 285.0, 8.75: 246.0, 9.75: 274.0, 10.75: 348.0, 11.75: 370.0, 12.75: 321.0, 6.25: 59.0, 13.75: 285.0, 16.75: 12.0, 14.75: 214.0, 16.25: 45.0, 5.75: 8.0, 7.25: 173.0, 10.25: 353.0, 11.25: 337.0, 12.25: 338.0, 13.25: 307.0, 14.25: 239.0, 7.75: 217.0} --lumimethod=EXTERNAL --nMC=1 --trigger=L1_MBTS_2 --outfile=configLumi_run189751.py devval/conf/1027/run189751.xml
## Created on Tue Dec  6 20:02:05 2011
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
   {'run':189751, 'lb':116, 'starttstamp':1316868299, 'dt':0.000, 'evts':_evts(158), 'mu':15.250, 'force_new':False},
   {'run':189751, 'lb':117, 'starttstamp':1316868360, 'dt':0.000, 'evts':_evts(88), 'mu':15.750, 'force_new':False},
   {'run':189751, 'lb':118, 'starttstamp':1316868414, 'dt':0.000, 'evts':_evts(353), 'mu':10.250, 'force_new':False},
   {'run':189751, 'lb':119, 'starttstamp':1316868474, 'dt':0.000, 'evts':_evts(214), 'mu':14.750, 'force_new':False},
   {'run':189751, 'lb':120, 'starttstamp':1316868535, 'dt':0.000, 'evts':_evts(246), 'mu':8.750, 'force_new':False},
   {'run':189751, 'lb':121, 'starttstamp':1316868596, 'dt':0.000, 'evts':_evts(274), 'mu':9.750, 'force_new':False},
   {'run':189751, 'lb':122, 'starttstamp':1316868657, 'dt':0.000, 'evts':_evts(348), 'mu':10.750, 'force_new':False},
   {'run':189751, 'lb':123, 'starttstamp':1316868718, 'dt':0.000, 'evts':_evts(370), 'mu':11.750, 'force_new':False},
   {'run':189751, 'lb':124, 'starttstamp':1316868779, 'dt':0.000, 'evts':_evts(321), 'mu':12.750, 'force_new':False},
   {'run':189751, 'lb':125, 'starttstamp':1316868840, 'dt':0.000, 'evts':_evts(95), 'mu':6.750, 'force_new':False},
   {'run':189751, 'lb':126, 'starttstamp':1316868851, 'dt':0.000, 'evts':_evts(285), 'mu':13.750, 'force_new':False},
   {'run':189751, 'lb':127, 'starttstamp':1316868912, 'dt':0.000, 'evts':_evts(45), 'mu':16.250, 'force_new':False},
   {'run':189751, 'lb':128, 'starttstamp':1316868973, 'dt':0.000, 'evts':_evts(285), 'mu':9.250, 'force_new':False},
   {'run':189751, 'lb':129, 'starttstamp':1316869034, 'dt':0.000, 'evts':_evts(8), 'mu':5.750, 'force_new':False},
   {'run':189751, 'lb':130, 'starttstamp':1316869095, 'dt':0.000, 'evts':_evts(59), 'mu':6.250, 'force_new':False},
   {'run':189751, 'lb':131, 'starttstamp':1316869156, 'dt':0.000, 'evts':_evts(228), 'mu':8.250, 'force_new':False},
   {'run':189751, 'lb':132, 'starttstamp':1316869217, 'dt':0.000, 'evts':_evts(217), 'mu':7.750, 'force_new':False},
   {'run':189751, 'lb':133, 'starttstamp':1316869278, 'dt':0.000, 'evts':_evts(337), 'mu':11.250, 'force_new':False},
   {'run':189751, 'lb':134, 'starttstamp':1316869339, 'dt':0.000, 'evts':_evts(12), 'mu':16.750, 'force_new':False},
   {'run':189751, 'lb':135, 'starttstamp':1316869400, 'dt':0.000, 'evts':_evts(338), 'mu':12.250, 'force_new':False},
   {'run':189751, 'lb':136, 'starttstamp':1316869461, 'dt':0.000, 'evts':_evts(307), 'mu':13.250, 'force_new':False},
   {'run':189751, 'lb':137, 'starttstamp':1316869522, 'dt':0.000, 'evts':_evts(239), 'mu':14.250, 'force_new':False},
   {'run':189751, 'lb':138, 'starttstamp':1316869583, 'dt':0.000, 'evts':_evts(173), 'mu':7.250, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker
