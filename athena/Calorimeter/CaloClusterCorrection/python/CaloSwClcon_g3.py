# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloSwClcon_g3.py,v 1.3 2009-04-21 21:16:21 ssnyder Exp $
#
# File: CaloClusterCorrection/python/CaloSwClcon_g3.py
# Created: Nov 2006, sss
# Purpose: Cluster containment corrections, original G3 version.
#
# This is the original G3-based cluster containment correction,
# translated from the original fortran version.
#


import string


    # 3x3 cluster size corrections
qclcon_g3_correction_33 = \
    [ 1.078375, 1.073588, 1.068029, 1.067124, 1.065353, 1.063478,
      1.063487, 1.064091, 1.064620, 1.065535, 1.065928, 1.064441,
      1.064246, 1.065485, 1.065754, 1.066126, 1.067769, 1.065284,
      1.064236, 1.065026, 1.064942, 1.065327, 1.065729, 1.066053,
      1.066340, 1.068023, 1.068735, 1.067680, 1.070011, 1.069546,
      1.071910, 1.075589, 1.079728, 1.080829, 1.081037, 1.083144,
      1.084341, 1.083764, 1.083305, 1.083606, 1.084588, 1.085374,
      1.086412, 1.086967, 1.086926, 1.087401, 1.090684, 1.090928,
      1.092272, 1.093483, 1.094661, 1.096832, 1.096227, 1.094092,
      1.098986, 1.118014, 1.095985, 1.074587, 1.098217, 1.099300,
      1.053789, 1.061959, 1.070160, 1.074437, 1.084294, 1.079147,
      1.083527, 1.084443, 1.088541, 1.085724, 1.088645, 1.087872,
      1.087518, 1.093915, 1.101152, 1.098736, 1.103793, 1.102747,
      1.107745, 1.109256, 1.112341, 1.113750, 1.117217, 1.116764,
      1.120677, 1.123925, 1.126915, 1.130532, 1.130191, 1.130600,
      1.133374, 1.136113, 1.141839, 1.142262, 1.146060, 1.141805,
      1.144689, 1.146580, 1.151204, 1.187682
      ]

    # 3x5 cluster size corrections
qclcon_g3_correction_35 = \
    [ 1.058707, 1.054308, 1.048840, 1.047414, 1.045694, 1.043893,
      1.043898, 1.044700, 1.045242, 1.045696, 1.045675, 1.045520,
      1.044316, 1.045420, 1.045847, 1.046269, 1.046963, 1.045477,
      1.045048, 1.045402, 1.045360, 1.045964, 1.046266, 1.045580,
      1.046944, 1.046865, 1.048155, 1.047536, 1.048320, 1.049386,
      1.050464, 1.052843, 1.056883, 1.057436, 1.057149, 1.058935,
      1.060314, 1.059422, 1.059474, 1.059201, 1.059732, 1.061403,
      1.061918, 1.061247, 1.062741, 1.061693, 1.064595, 1.064434,
      1.065684, 1.066000, 1.068271, 1.068857, 1.068434, 1.065671,
      1.071404, 1.096477, 1.079768, 1.061586, 1.084594, 1.083435,
      1.036563, 1.042124, 1.051633, 1.053368, 1.061088, 1.056587,
      1.059516, 1.060670, 1.063166, 1.061037, 1.064451, 1.062119,
      1.061860, 1.067356, 1.074288, 1.071918, 1.076366, 1.073404,
      1.079255, 1.079430, 1.082620, 1.083466, 1.084529, 1.085417,
      1.089347, 1.091720, 1.094488, 1.097046, 1.097670, 1.096592,
      1.099629, 1.102022, 1.106214, 1.106825, 1.108838, 1.104877,
      1.108785, 1.108351, 1.113116, 1.155775
      ]

    # 3x7 cluster size corrections
qclcon_g3_correction_37 = \
    [ 1.053498, 1.049514, 1.043753, 1.042321, 1.040980, 1.038947,
      1.039176, 1.039773, 1.040388, 1.040720, 1.040950, 1.040566,
      1.039400, 1.040319, 1.040733, 1.041271, 1.042318, 1.040246,
      1.039718, 1.040266, 1.040023, 1.040895, 1.041332, 1.040172,
      1.041528, 1.041379, 1.042315, 1.041904, 1.043389, 1.043543,
      1.044631, 1.046862, 1.051299, 1.051138, 1.050803, 1.052100,
      1.053434, 1.052402, 1.052445, 1.051936, 1.053088, 1.054241,
      1.054323, 1.054055, 1.055183, 1.054367, 1.057329, 1.056301,
      1.057498, 1.056996, 1.058906, 1.060370, 1.059576, 1.054732,
      1.062164, 1.084753, 1.071525, 1.051846, 1.074818, 1.072311,
      1.026452, 1.033437, 1.044789, 1.046867, 1.054820, 1.049857,
      1.052079, 1.053832, 1.056246, 1.054355, 1.057156, 1.055337,
      1.054791, 1.060255, 1.066472, 1.063960, 1.068481, 1.065575,
      1.070179, 1.070292, 1.073186, 1.074448, 1.075529, 1.074810,
      1.079697, 1.082692, 1.085428, 1.086750, 1.087144, 1.085802,
      1.088702, 1.091019, 1.095583, 1.095858, 1.097643, 1.093953,
      1.096176, 1.097043, 1.101017, 1.145469
      ]

    # 5x5 cluster size corrections
qclcon_g3_correction_55 = \
    [ 1.037459, 1.032735, 1.027135, 1.026086, 1.024178, 1.023081,
      1.023061, 1.023073, 1.023593, 1.024318, 1.024313, 1.024106,
      1.023116, 1.024557, 1.024093, 1.024755, 1.026209, 1.024318,
      1.024097, 1.023648, 1.023718, 1.024990, 1.024454, 1.023596,
      1.024630, 1.024752, 1.026288, 1.025682, 1.027240, 1.027728,
      1.028461, 1.031026, 1.033114, 1.033694, 1.033398, 1.033332,
      1.034540, 1.033850, 1.033782, 1.033815, 1.033400, 1.035734,
      1.035844, 1.035578, 1.036123, 1.036736, 1.038962, 1.038979,
      1.038690, 1.039699, 1.041597, 1.042964, 1.041769, 1.041427,
      1.050201, 1.043962, 1.045128, 1.043883, 1.056332, 1.007678,
      1.013919, 1.020643, 1.027454, 1.030064, 1.036959, 1.033641,
      1.032976, 1.034597, 1.036010, 1.035479, 1.035308, 1.036325,
      1.032615, 1.039775, 1.041586, 1.042964, 1.043622, 1.043790,
      1.045085, 1.047912, 1.048896, 1.051080, 1.049787, 1.052125,
      1.052460, 1.057436, 1.057053, 1.059391, 1.057641, 1.059529,
      1.059108, 1.063259, 1.065385, 1.068215, 1.065840, 1.064185,
      1.064107, 1.066780, 1.077536, 1.129594
      ]

    # 5x7 cluster size corrections
qclcon_g3_correction_57 = \
    [ 1.031022, 1.026096, 1.020796, 1.019615, 1.018107, 1.016474,
      1.016954, 1.016979, 1.017321, 1.017850, 1.018149, 1.017924,
      1.016578, 1.017901, 1.017421, 1.018473, 1.019586, 1.017595,
      1.017349, 1.017165, 1.017054, 1.018254, 1.017739, 1.016800,
      1.017879, 1.018447, 1.019433, 1.018882, 1.019886, 1.020187,
      1.021509, 1.023099, 1.025302, 1.025021, 1.024774, 1.024840,
      1.025978, 1.025095, 1.025120, 1.024655, 1.024853, 1.026348,
      1.026358, 1.025984, 1.026484, 1.027019, 1.028524, 1.028515,
      1.028193, 1.028845, 1.030006, 1.031865, 1.030999, 1.029917,
      1.038042, 1.033155, 1.035087, 1.032095, 1.042205, 0.996135,
      1.002880, 1.011719, 1.019929, 1.022660, 1.028572, 1.025368,
      1.023223, 1.025199, 1.026696, 1.026263, 1.025933, 1.026655,
      1.022736, 1.029787, 1.031418, 1.032962, 1.033105, 1.033244,
      1.034608, 1.036483, 1.037240, 1.039945, 1.038712, 1.039870,
      1.040197, 1.044991, 1.043983, 1.046236, 1.045636, 1.045494,
      1.044860, 1.049053, 1.051161, 1.053638, 1.051546, 1.049570,
      1.049918, 1.053061, 1.062447, 1.114986
      ]

    # 7x7 cluster size corrections
qclcon_g3_correction_77 = \
    [ 1.022106, 1.018038, 1.013550, 1.012581, 1.010664, 1.009822,
      1.009181, 1.010324, 1.010339, 1.010975, 1.011172, 1.010981,
      1.009761, 1.010842, 1.010622, 1.011645, 1.012573, 1.010854,
      1.010490, 1.010109, 1.009834, 1.011441, 1.010540, 1.009929,
      1.010871, 1.010753, 1.012097, 1.011656, 1.012627, 1.013266,
      1.013738, 1.015718, 1.016643, 1.017116, 1.016803, 1.016540,
      1.016508, 1.016217, 1.016081, 1.016105, 1.015432, 1.017361,
      1.017296, 1.017277, 1.017086, 1.018083, 1.019130, 1.019360,
      1.019189, 1.019911, 1.020923, 1.023181, 1.021834, 1.022776,
      1.025981, 1.024581, 1.025969, 1.024075, 1.033404, 0.984940,
      1.000732, 1.003847, 1.012588, 1.013506, 1.021033, 1.016447,
      1.015109, 1.015921, 1.017602, 1.015794, 1.017237, 1.015622,
      1.012959, 1.018511, 1.020624, 1.020630, 1.022221, 1.020960,
      1.022920, 1.023543, 1.025552, 1.026582, 1.026415, 1.026043,
      1.027273, 1.030103, 1.030627, 1.031805, 1.032091, 1.030606,
      1.030750, 1.033480, 1.036700, 1.036593, 1.036908, 1.032752,
      1.033819, 1.038726, 1.054542, 1.105910
      ]

class CaloSwClcon_g3_parms:
    etamin = 0
    etamax = 2.5

    correction = {
        'ele33' : qclcon_g3_correction_33,
        'gam33' : qclcon_g3_correction_33,
        'ele35' : qclcon_g3_correction_35,
        'gam35' : qclcon_g3_correction_35,
        'ele37' : qclcon_g3_correction_37,
        'gam37' : qclcon_g3_correction_37,
        'ele55' : qclcon_g3_correction_55,
        'gam55' : qclcon_g3_correction_55,
        'ele57' : qclcon_g3_correction_57,
        'gam57' : qclcon_g3_correction_57,
        'ele77' : qclcon_g3_correction_77,
        'gam77' : qclcon_g3_correction_77,
        }
                  
