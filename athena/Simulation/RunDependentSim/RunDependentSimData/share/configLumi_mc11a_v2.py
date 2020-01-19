####################
## File ../0804/configLumi_mc11_v2.py
## Created on Fri Aug  5 00:00:33 2011
####################
#Run-dependent digi job configuration file.
#SimuJobTransforms/ConfigLumi_preOptions.py

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
   {'run':180164, 'lb':69, 'starttstamp':1303554791, 'dt':0.000, 'evts':_evts(66), 'mu':6.750, 'force_new':False},
   {'run':180164, 'lb':70, 'starttstamp':1303554852, 'dt':0.000, 'evts':_evts(14), 'mu':7.250, 'force_new':False},
   {'run':180164, 'lb':71, 'starttstamp':1303554913, 'dt':0.000, 'evts':_evts(187), 'mu':5.750, 'force_new':False},
   {'run':180164, 'lb':73, 'starttstamp':1303555035, 'dt':0.000, 'evts':_evts(203), 'mu':5.250, 'force_new':False},
   {'run':180164, 'lb':74, 'starttstamp':1303555062, 'dt':0.000, 'evts':_evts(4), 'mu':7.750, 'force_new':False},
   {'run':180164, 'lb':75, 'starttstamp':1303555090, 'dt':0.000, 'evts':_evts(151), 'mu':4.750, 'force_new':False},
   {'run':180164, 'lb':76, 'starttstamp':1303555151, 'dt':0.000, 'evts':_evts(138), 'mu':6.250, 'force_new':False},
   {'run':180164, 'lb':77, 'starttstamp':1303555212, 'dt':0.000, 'evts':_evts(25), 'mu':3.750, 'force_new':False},
   {'run':180164, 'lb':78, 'starttstamp':1303555273, 'dt':0.000, 'evts':_evts(112), 'mu':4.250, 'force_new':False},
   {'run':183003, 'lb':177, 'starttstamp':1306973316, 'dt':0.000, 'evts':_evts(575), 'mu':6.750, 'force_new':False},
   {'run':183003, 'lb':178, 'starttstamp':1306973377, 'dt':0.000, 'evts':_evts(4), 'mu':8.250, 'force_new':False},
   {'run':183003, 'lb':179, 'starttstamp':1306973438, 'dt':0.000, 'evts':_evts(673), 'mu':4.750, 'force_new':False},
   {'run':183003, 'lb':180, 'starttstamp':1306973499, 'dt':0.000, 'evts':_evts(534), 'mu':4.250, 'force_new':False},
   {'run':183003, 'lb':181, 'starttstamp':1306973560, 'dt':0.000, 'evts':_evts(877), 'mu':5.250, 'force_new':False},
   {'run':183003, 'lb':182, 'starttstamp':1306973621, 'dt':0.000, 'evts':_evts(953), 'mu':5.750, 'force_new':False},
   {'run':183003, 'lb':183, 'starttstamp':1306973682, 'dt':0.000, 'evts':_evts(832), 'mu':6.250, 'force_new':False},
   {'run':183003, 'lb':184, 'starttstamp':1306973743, 'dt':0.000, 'evts':_evts(182), 'mu':3.750, 'force_new':False},
   {'run':183003, 'lb':185, 'starttstamp':1306973779, 'dt':0.000, 'evts':_evts(35), 'mu':3.250, 'force_new':False},
   {'run':183003, 'lb':186, 'starttstamp':1306973840, 'dt':0.000, 'evts':_evts(81), 'mu':7.750, 'force_new':False},
   {'run':183003, 'lb':187, 'starttstamp':1306973901, 'dt':0.000, 'evts':_evts(256), 'mu':7.250, 'force_new':False},
   {'run':185649, 'lb':52, 'starttstamp':1310870414, 'dt':0.000, 'evts':_evts(22), 'mu':2.750, 'force_new':False},
   {'run':185649, 'lb':55, 'starttstamp':1310870574, 'dt':0.000, 'evts':_evts(329), 'mu':6.750, 'force_new':False},
   {'run':185649, 'lb':56, 'starttstamp':1310870635, 'dt':0.000, 'evts':_evts(285), 'mu':4.750, 'force_new':False},
   {'run':185649, 'lb':57, 'starttstamp':1310870696, 'dt':0.000, 'evts':_evts(182), 'mu':4.250, 'force_new':False},
   {'run':185649, 'lb':58, 'starttstamp':1310870757, 'dt':0.000, 'evts':_evts(51), 'mu':3.250, 'force_new':False},
   {'run':185649, 'lb':59, 'starttstamp':1310870818, 'dt':0.000, 'evts':_evts(337), 'mu':5.750, 'force_new':False},
   {'run':185649, 'lb':63, 'starttstamp':1310871062, 'dt':0.000, 'evts':_evts(368), 'mu':6.250, 'force_new':False},
   {'run':185649, 'lb':64, 'starttstamp':1310871123, 'dt':0.000, 'evts':_evts(89), 'mu':3.750, 'force_new':False},
   {'run':185649, 'lb':65, 'starttstamp':1310871184, 'dt':0.000, 'evts':_evts(93), 'mu':9.250, 'force_new':False},
   {'run':185649, 'lb':66, 'starttstamp':1310871245, 'dt':0.000, 'evts':_evts(26), 'mu':9.750, 'force_new':False},
   {'run':185649, 'lb':67, 'starttstamp':1310871306, 'dt':0.000, 'evts':_evts(200), 'mu':8.250, 'force_new':False},
   {'run':185649, 'lb':68, 'starttstamp':1310871349, 'dt':0.000, 'evts':_evts(180), 'mu':8.750, 'force_new':False},
   {'run':185649, 'lb':69, 'starttstamp':1310871410, 'dt':0.000, 'evts':_evts(316), 'mu':7.750, 'force_new':False},
   {'run':185649, 'lb':70, 'starttstamp':1310871450, 'dt':0.000, 'evts':_evts(303), 'mu':5.250, 'force_new':False},
   {'run':185649, 'lb':71, 'starttstamp':1310871511, 'dt':0.000, 'evts':_evts(317), 'mu':7.250, 'force_new':False},
   {'run':185761, 'lb':182, 'starttstamp':1310954001, 'dt':0.000, 'evts':_evts(20), 'mu':5.250, 'force_new':False},
   {'run':185761, 'lb':183, 'starttstamp':1310954036, 'dt':0.000, 'evts':_evts(20), 'mu':5.750, 'force_new':False},
   {'run':185761, 'lb':184, 'starttstamp':1310954095, 'dt':0.000, 'evts':_evts(20), 'mu':25.750, 'force_new':False},
   {'run':185761, 'lb':185, 'starttstamp':1310954156, 'dt':0.000, 'evts':_evts(20), 'mu':29.250, 'force_new':False},
   {'run':185761, 'lb':186, 'starttstamp':1310954217, 'dt':0.000, 'evts':_evts(20), 'mu':7.750, 'force_new':False},
   {'run':185761, 'lb':187, 'starttstamp':1310954278, 'dt':0.000, 'evts':_evts(20), 'mu':23.250, 'force_new':False},
   {'run':185761, 'lb':188, 'starttstamp':1310954339, 'dt':0.000, 'evts':_evts(20), 'mu':15.250, 'force_new':False},
   {'run':185761, 'lb':189, 'starttstamp':1310954400, 'dt':0.000, 'evts':_evts(20), 'mu':27.250, 'force_new':False},
   {'run':185761, 'lb':190, 'starttstamp':1310954461, 'dt':0.000, 'evts':_evts(20), 'mu':8.750, 'force_new':False},
   {'run':185761, 'lb':191, 'starttstamp':1310954522, 'dt':0.000, 'evts':_evts(20), 'mu':10.750, 'force_new':False},
   {'run':185761, 'lb':192, 'starttstamp':1310954583, 'dt':0.000, 'evts':_evts(20), 'mu':17.750, 'force_new':False},
   {'run':185761, 'lb':193, 'starttstamp':1310954644, 'dt':0.000, 'evts':_evts(20), 'mu':29.750, 'force_new':False},
   {'run':185761, 'lb':194, 'starttstamp':1310954655, 'dt':0.000, 'evts':_evts(20), 'mu':12.750, 'force_new':False},
   {'run':185761, 'lb':195, 'starttstamp':1310954666, 'dt':0.000, 'evts':_evts(20), 'mu':21.750, 'force_new':False},
   {'run':185761, 'lb':196, 'starttstamp':1310954727, 'dt':0.000, 'evts':_evts(20), 'mu':6.750, 'force_new':False},
   {'run':185761, 'lb':197, 'starttstamp':1310954788, 'dt':0.000, 'evts':_evts(20), 'mu':14.750, 'force_new':False},
   {'run':185761, 'lb':198, 'starttstamp':1310954849, 'dt':0.000, 'evts':_evts(20), 'mu':18.250, 'force_new':False},
   {'run':185761, 'lb':199, 'starttstamp':1310954910, 'dt':0.000, 'evts':_evts(20), 'mu':20.250, 'force_new':False},
   {'run':185761, 'lb':200, 'starttstamp':1310954971, 'dt':0.000, 'evts':_evts(20), 'mu':28.250, 'force_new':False},
   {'run':185761, 'lb':201, 'starttstamp':1310955032, 'dt':0.000, 'evts':_evts(20), 'mu':9.250, 'force_new':False},
   {'run':185761, 'lb':202, 'starttstamp':1310955093, 'dt':0.000, 'evts':_evts(20), 'mu':11.250, 'force_new':False},
   {'run':185761, 'lb':203, 'starttstamp':1310955154, 'dt':0.000, 'evts':_evts(20), 'mu':18.750, 'force_new':False},
   {'run':185761, 'lb':204, 'starttstamp':1310955215, 'dt':0.000, 'evts':_evts(20), 'mu':13.250, 'force_new':False},
   {'run':185761, 'lb':205, 'starttstamp':1310955276, 'dt':0.000, 'evts':_evts(20), 'mu':22.750, 'force_new':False},
   {'run':185761, 'lb':206, 'starttstamp':1310955337, 'dt':0.000, 'evts':_evts(20), 'mu':6.250, 'force_new':False},
   {'run':185761, 'lb':210, 'starttstamp':1310955581, 'dt':0.000, 'evts':_evts(20), 'mu':9.750, 'force_new':False},
   {'run':185761, 'lb':211, 'starttstamp':1310955642, 'dt':0.000, 'evts':_evts(20), 'mu':26.750, 'force_new':False},
   {'run':185761, 'lb':212, 'starttstamp':1310955703, 'dt':0.000, 'evts':_evts(20), 'mu':17.250, 'force_new':False},
   {'run':185761, 'lb':213, 'starttstamp':1310955764, 'dt':0.000, 'evts':_evts(20), 'mu':19.250, 'force_new':False},
   {'run':185761, 'lb':214, 'starttstamp':1310955825, 'dt':0.000, 'evts':_evts(20), 'mu':21.250, 'force_new':False},
   {'run':185761, 'lb':215, 'starttstamp':1310955886, 'dt':0.000, 'evts':_evts(20), 'mu':15.750, 'force_new':False},
   {'run':185761, 'lb':216, 'starttstamp':1310955947, 'dt':0.000, 'evts':_evts(20), 'mu':25.250, 'force_new':False},
   {'run':185761, 'lb':217, 'starttstamp':1310956008, 'dt':0.000, 'evts':_evts(20), 'mu':11.750, 'force_new':False},
   {'run':185761, 'lb':218, 'starttstamp':1310956069, 'dt':0.000, 'evts':_evts(20), 'mu':19.750, 'force_new':False},
   {'run':185761, 'lb':219, 'starttstamp':1310956130, 'dt':0.000, 'evts':_evts(20), 'mu':7.250, 'force_new':False},
   {'run':185761, 'lb':220, 'starttstamp':1310956191, 'dt':0.000, 'evts':_evts(20), 'mu':13.750, 'force_new':False},
   {'run':185761, 'lb':221, 'starttstamp':1310956252, 'dt':0.000, 'evts':_evts(20), 'mu':23.750, 'force_new':False},
   {'run':185761, 'lb':222, 'starttstamp':1310956313, 'dt':0.000, 'evts':_evts(20), 'mu':24.750, 'force_new':False},
   {'run':185761, 'lb':223, 'starttstamp':1310956374, 'dt':0.000, 'evts':_evts(20), 'mu':24.250, 'force_new':False},
   {'run':185761, 'lb':224, 'starttstamp':1310956435, 'dt':0.000, 'evts':_evts(20), 'mu':27.750, 'force_new':False},
   {'run':185761, 'lb':225, 'starttstamp':1310956496, 'dt':0.000, 'evts':_evts(20), 'mu':16.250, 'force_new':False},
   {'run':185761, 'lb':226, 'starttstamp':1310956557, 'dt':0.000, 'evts':_evts(20), 'mu':22.250, 'force_new':False},
   {'run':185761, 'lb':228, 'starttstamp':1310956679, 'dt':0.000, 'evts':_evts(20), 'mu':26.250, 'force_new':False},
   {'run':185761, 'lb':236, 'starttstamp':1310957149, 'dt':0.000, 'evts':_evts(20), 'mu':8.250, 'force_new':False},
   {'run':185761, 'lb':237, 'starttstamp':1310957181, 'dt':0.000, 'evts':_evts(20), 'mu':10.250, 'force_new':False},
   {'run':185761, 'lb':238, 'starttstamp':1310957241, 'dt':0.000, 'evts':_evts(20), 'mu':16.750, 'force_new':False},
   {'run':185761, 'lb':239, 'starttstamp':1310957294, 'dt':0.000, 'evts':_evts(20), 'mu':28.750, 'force_new':False},
   {'run':185761, 'lb':240, 'starttstamp':1310957355, 'dt':0.000, 'evts':_evts(20), 'mu':12.250, 'force_new':False},
   {'run':185761, 'lb':241, 'starttstamp':1310957415, 'dt':0.000, 'evts':_evts(20), 'mu':20.750, 'force_new':False},
   {'run':185761, 'lb':242, 'starttstamp':1310957476, 'dt':0.000, 'evts':_evts(20), 'mu':14.250, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

del JobMaker
