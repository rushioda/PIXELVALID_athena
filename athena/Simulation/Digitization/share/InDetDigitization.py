## @file  InDetDigitization.py
#  @brief include file to configure indet digitization
from AthenaCommon.DetFlags import DetFlags
##################################################################
#BCM
##################################################################
if DetFlags.digitize.BCM_on():
    pass
##################################################################
#Pixels
##################################################################
if DetFlags.digitize.pixel_on():
    pass
##################################################################
# SCT
##################################################################
if DetFlags.digitize.SCT_on():
    pass
##################################################################
# TRT
##################################################################
if DetFlags.digitize.TRT_on():
    from Digitization.DigiConfigCheckers import checkTRT_DigitizationConfiguration
    checkTRT_DigitizationConfiguration()
##################################################################
