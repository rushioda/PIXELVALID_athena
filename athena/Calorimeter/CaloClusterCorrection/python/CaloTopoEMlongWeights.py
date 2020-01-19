# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloTopoEMlongWeights.py,v 1.5 2009-04-20 16:59:16 ssnyder Exp $
#
# File: CaloClusterCorrection/python/CaloTopoEMlongWeights.py
# Created: Mar 2007, sss, from earlier job options.
# Purpose: Steering module + corrections for longitudinal weights correction
# for TopoEM clusters.
#

from CaloClusterCorrection        import CaloClusterCorrectionConf
from CaloClusterCorrection.common import *

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.common import topoem_valid_keys as keys
cls = CaloClusterCorrectionConf.CaloTopoEMLongWeights
CaloTopoEMlongWeights_versions = [

    # v1 --- from the original DC3 job options file.
    ['v1', cls, ['CaloTopoEMlongWeights.CaloTopoEMlongWeights_v1_parms',
                 'calotopoemcorr_pool', CALOCORR_COOL], keys],

    ]


#
# Create a new tool instance.
#  NAME is the base name for this tool.  If defaulted, a name will
#   be constructed from the name of the correction, the version, and the key.
#  If SUFFIX is not None, it will be added onto the end of the tool name.
#  VERSION is a string specifying which of several versions of the correction
#   to choose.  If defaulted, the latest version of the correction is chosen.
#  KEY is a string to specify the type of cluster to which the correction
#   applies.  The convention is to use `ele55' for 5x5 electron clusters,
#   `gam35' for 3x5 photon clusters, and so on.
#  SOURCE tells from where we should read the calibration constants.
#   See common.py for a description of the possibilities.
#   None means to use the default.
#  CONFCLASS gives the Configurable class to use to create the tool.
#   It may (and usually should) be defaulted.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def make_CaloTopoEMlongWeights (name = None,
                                suffix = None,
                                version = None,
                                key = CALOCORR_DEFAULT_KEY,
                                source = None,
                                confclass = None,
                                **kw):
    # Make the tool.
    return makecorr (versions  = CaloTopoEMlongWeights_versions,
                     name      = name,
                     basename  = 'topolw',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)


# Long weights correction, from 12.0.3?
# I think this was derived for 6-3-3 clusters.
# Currently applied for all emtopo clusters.
LW_electrons_1203 = [
                # a        w0       w3     scale       b     eoffset
[0.0125, 599.319, 1.17312, 2.07943,  1.0183,   2.20837,  394.95],
[0.0375, 444.48,  1.11663, 1.59474,  1.01412,  2.27522,  365.82],
[0.0625, 381.738, 1.20179, 1.72241,  1.01278,  1.9858,   280.98], 
[0.0875, 402.124, 1.08454, 1.80548,  1.01176,  1.83568,  276.09 ],
[0.1125, 447.942, 1.06615, 1.65346,  1.01195,  2.27652,  363.44 ],
[0.1375, 419.724, 1.13551, 1.72067,  1.01162,  2.07489,  274.29 ],
[0.1625, 374.143, 1.12147, 1.58993,  1.01305,  2.12248,  318.34 ],
[0.1875, 456.015, 1.15388, 1.76808,  1.00954,  2.41817,  389.18 ],
[0.2125, 364,     1.10648, 1.66861,  1.012,    1.62777,  216.43 ],
[0.2375, 403.421, 1.11308, 1.50937,  1.01295,  1.98073,  230.98 ],
[0.2625, 434.427, 1.11666, 1.60026,  1.01219,  2.1859,   263.1 ],
[0.2875, 434.978, 1.14267, 1.59589,  1.01144,  1.93362,  139.25 ],
[0.3125, 413.973, 1.08645, 1.64434,  1.01151,  2.01791,  236.78 ],
[0.3375, 399.977, 1.09005, 1.47713,  1.01246,  2.03792,  269.63 ],
[0.3625, 399.92,  1.07103, 1.55917,  1.01217,  1.80685,  244.49 ],
[0.3875, 430.406, 1.11439, 1.35351,  1.01196,  2.04952,  272.76 ],
[0.4125, 400.171, 1.06804, 1.48427,  1.01193,  1.57706,  188.28 ],
[0.4375, 437.234, 1.10279, 1.19538,  1.01293,  2.17073,  275.37 ],
[0.4625, 423.683, 1.08896, 1.22227,  1.01212,  1.96789,  304.14 ],
[0.4875, 476.43,  1.06552, 1.14105,  1.01232,  1.90715,  217.49 ],
[0.5125, 526.839, 1.07628, 1.10934,  1.01088,  2.10788,  259.2 ],
[0.5375, 332.272, 1.1238,  0.939561, 1.01587,  1.91171,  370.48 ],
[0.5625, 456.152, 1.08914, 1.10261,  1.01279,  1.94407,  238.46 ],
[0.5875, 361.61,  1.06705, 0.836423, 1.01758,  1.57934,  251.75 ],
[0.6125, 446.054, 1.06735, 1.02989,  1.01349,  1.83711,  323.48 ],
[0.6375, 397.847, 1.15574, 0.740889, 1.01574,  1.6922,   367.33 ],
[0.6625, 354.525, 1.15779, 0.870557, 1.01616,  1.31804,  443.53 ],
[0.6875, 392.755, 1.15788, 0.866387, 1.01611,  1.54988,  489.39 ],
[0.7125, 452.46,  1.13546, 0.826545, 1.01649,  1.69759,  541.08 ],
[0.7375, 368.11,  1.14235, 0.815892, 1.01941,  1.65431,  683.13 ],
[0.7625, 377.673, 1.15269, 0.67647,  1.02246,  2.30317,  919.39 ],
[0.7875, 677.221, 1.2073,  0.79992,  1.02379,  1.87806,  675.85 ],
[0.8125, 901.202, 1.24757, 1.89879,  1.01256,  1.9237,   812.32 ],
[0.8375, 459.288, 1.1721,  0.839089, 1.02541,  2.19541,  955.2 ],
[0.8625, 555.002, 1.17363, 0.600198, 1.02547,  2.70074,  1062.44 ],
[0.8875, 691.744, 1.18339, 0.74731,  1.02149,  2.14857,  738 ],
[0.9125, 784.015, 1.19613, 0.901383, 1.01938,  2.08977,  630.12 ],
[0.9375, 789.763, 1.14235, 0.256485, 1.02608,  2.08469,  661.4 ],
[0.9625, 958.56,  1.17155, 0.678529, 1.01993,  2.43451,  889.22 ],
[0.9875, 1033.73, 1.17396, 0.754517, 1.01882,  2.53029,  951.82 ],
[1.0125, 1203.53, 1.18982, 0.780035, 1.01798,  2.49232,  904.08 ],
[1.0375, 1265.45, 1.16914, 0.732369, 1.01983,  2.72483,  1085.28 ],
[1.0625, 1346.39, 1.16114, 0.260496, 1.02306,  2.43365,  725.48 ],
[1.0875, 1330.77, 1.17826, 0.525022, 1.01964,  2.36538,  824.4 ],
[1.1125, 1459.37, 1.17368, 0.819427, 1.01596,  2.40191,  810.5 ],
[1.1375, 1387.89, 1.12309, 0.524443, 1.02357,  2.45161,  847.03 ],
[1.1625, 1030.08, 1.13822, 0.277952, 1.0338,   2.55929,  948.77 ],
[1.1875, 1288.23, 1.11093, 0.353221, 1.0296,   2.81427,  1350.33 ],
[1.2125, 885.202, 1.09018, 0.675857, 1.03104,  2.09254,  873.9 ],
[1.2375, 1292.86, 1.08401, 0.632695, 1.02769,  2.20168,  788.67 ],
[1.2625, 1134.08, 1.06011, 0.362552, 1.03763,  2.55408,  1149.04 ],
[1.2875, 799.555, 1.05346, 0.0,      1.04725,  1.71917,  881.82 ],
[1.3125, 922.869, 1.06753, 0.0,      1.04455,  2.22089,  1256.28 ],
[1.3375, 1042.18, 1.06581, 0.0,      1.04486,  2.02894,  1057.06 ],
[1.3625, 944.393, 1.06258, 0.0,      1.05237,  1.89901,  883.41 ],
[1.3875, 1016.91, 1.03975, 0.0,      1.06878,  1.82453,  1111.41 ],
[1.4125, 345.83,  0.992074, 0.0,      1.13857,  0.0,      1068.74 ],
[1.4375, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.4625, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.4875, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.5125, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.5375, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.5625, 698.318, 1.36675, 1.16422,  0.990969, 0.0,      1521.05 ],
[1.5875, 929.652, 1.29071, 0.76693,  0.998384, 2.12666,  2206.5 ],
[1.6125, 714.731, 1.27793, 0.884885, 1.00157,  0.924293, 1657.98 ],
[1.6375, 563.122, 1.34128, 1.37882,  0.991714, 0.0,      2380.54 ],
[1.6625, 370.982, 1.54397, 1.9242,   0.98297,  0.0,      3528.86 ],
[1.6875, 1264.24, 1.73707, 2.80141,  0.959282, 1.32881,  3214 ],
[1.7125, 592.505, 1.22341, 1.58582,  0.996701, 0.0,      1250.85 ],
[1.7375, 336.251, 1.12602, 1.19976,  1.00375,  0.0,      1685.97 ],
[1.7625, 509.354, 1.10354, 1.25343,  1.0017,   0.0,      931.07 ],
[1.7875, 636.328, 1.11084, 1.26781,  0.99926,  0.755914, 807 ],
[1.8125, 509.622, 0.847071, 0.454241, 1.02131,  0.0,      1445.68 ],
[1.8375, 1035.83, 2.48932, 0.969606, 1.00973,  3.27519,  2683.98 ],
[1.8625, 827.374, 0.0,     0.873598, 1.01166,  2.00865,  1263.26 ],
[1.8875, 668.757, 0.0,     0.931706, 1.01391,  2.53886,  1638.94 ],
[1.9125, 1179.4,  0.0,     0.951636, 1.00897,  2.34415,  1138.62 ],
[1.9375, 1203.64, 0.0,     0.807721, 1.01027,  3.12539,  1974.27 ],
[1.9625, 421.994, 0.0,     0.993962, 1.0167,   0.560264, 1326.37 ],
[1.9875, 1155.57, 0.0,     0.873548, 1.01071,  1.98085,  1022.2 ],
[2.0125, 1032.61, 0.0,     0.643531, 1.01211,  2.11497,  1278.98 ],
[2.0375, 1183.31, 0.0,     0.902407, 1.00959,  2.4291,   1466.19 ],
[2.0625, 1111.38, 0.0,     0.516395, 1.01282,  2.92521,  1996.19 ],
[2.0875, 1532.24, 0.0,     0.496276, 1.01085,  2.90047,  1824.19 ],
[2.1125, 1999.93, 0.0,     0.871707, 1.0016,   2.4451,   1186.51 ],
[2.1375, 2039.61, 0.0,     0.8238,   1.00318,  2.65682,  1391.23 ],
[2.1625, 2780.56, 0.0,     0.783811, 0.996993, 2.67928,  1570.47 ],
[2.1875, 2842.25, 0.0,     0.863019, 0.994301, 2.64224,  1538.72 ],
[2.2125, 660.824, 0.0,     0.801581, 1.01458,  1.42814,  1407.73 ],
[2.2375, 592.227, 0.0,     0.939205, 1.01452,  1.40176,  1381.59 ],
[2.2625, 635.467, 0.0,     0.898907, 1.01367,  1.70352,  1461.65 ],
[2.2875, 1431.08, 0.0,     0.859078, 1.00629,  2.56876,  1202.09 ],
[2.3125, 2202.46, 0.0,     1.04788,  0.999408, 2.81303,  1776.18 ],
[2.3375, 668.042, 0.0,     0.962298, 1.0166,   1.18084,  1161.83 ],
[2.3625, 1288.78, 0.0,     0.849863, 1.01313,  2.71299,  1504.43 ],
[2.3875, 794.649, 0.0,     0.868348, 1.01336,  1.48679,  993 ],
[2.4125, 416.671, 0.0,     0.708318, 1.02012,  0.0,      1383.27 ],
[2.4375, 654.734, 0.0,     0.109434, 1.01978,  2.3393,   1755.68 ],
[2.4625, 519.175, 0.0,     0.185451, 1.02737,  2.86861,  1692.84 ],
[2.4875, 252.352, 0.0,     0.288374, 1.04028,  0.0,      1654.09],
]

LW_photons_1203 = [
[0.0125,   277.703,   1.31079,   1.95691,   1.02632,   1.04994, 0.0],
[0.0375,   313.162,   1.28381,   1.72891,   1.01526,   1.76187 , 0.0], 
[0.0625,   436.178,   1.31827,   1.8653,    1.01031,   2.2319, 0.0],  
[0.0875,   402.691,   1.33751,   1.83954,   1.01008,   2.03004 , 0.0], 
[0.1125,   342.554,   1.33031,   1.87718,   1.01084,   1.83837 , 0.0], 
[0.1375,   401.297,   1.27632,   1.84164,   1.01062,   2.11849 , 0.0], 
[0.1625,   354.232,   1.30259,   1.88197,   1.01091,   1.91854 , 0.0], 
[0.1875,   312.072,   1.33403,   1.81374,   1.01091,   1.61565 , 0.0], 
[0.2125,   341.935,   1.24662,   1.57076,   1.01258,   1.8212 , 0.0], 
[0.2375,   300.554,   1.26075,   1.67528,   1.01378,   1.69839 , 0.0], 
[0.2625,   400.177,   1.26234,   1.61348,   1.01059,   2.02065 , 0.0], 
[0.2875,   352.525,   1.31667,   1.72339,   1.01203,   1.96891 , 0.0], 
[0.3125,   386.131,   1.25889,   1.57806,   1.01089,   1.96838 , 0.0], 
[0.3375,   379.326,   1.24223,   1.76505,   1.00954,   1.87697 , 0.0], 
[0.3625,   398.364,   1.25401,   1.58991,   1.01072,   2.00313 , 0.0], 
[0.3875,   348.646,   1.27611,   1.69013,   1.01148,   1.73234 , 0.0], 
[0.4125,   381.67,    1.23714,   1.44192,   1.0113,    1.93096 , 0.0], 
[0.4375,   421.683,   1.23078,   1.617,     1.00968,   2.06123 , 0.0], 
[0.4625,   347.332,   1.24783,   1.22817,   1.01299,   1.77656 , 0.0], 
[0.4875,   400.24,    1.23807,   1.31338,   1.01115,   2.028  , 0.0],
[0.5125,   421.904,   1.294,     1.24191,   1.01104,   2.11648  , 0.0],
[0.5375,   371.556,   1.25309,   1.11316,   1.0126,    1.86631 , 0.0], 
[0.5625,   451.796,   1.23465,   1.1065,    1.01083,   2.12478 , 0.0], 
[0.5875,   435.596,   1.27043,   1.13499,   1.01061,   2.0674  , 0.0],
[0.6125,   423.724,   1.24338,   1.12225,   1.01161,   2.09577  , 0.0],
[0.6375,   483.226,   1.2329,    0.98585,   1.01197,   2.18856 , 0.0], 
[0.6625,   447.862,   1.30018,   1.00941,   1.01188,   2.03338 , 0.0], 
[0.6875,   451.364,   1.28217,   0.986807,   1.01211,   1.99437 , 0.0], 
[0.7125,   370.201,   1.29467,   0.973838,   1.01484,   1.75575 , 0.0], 
[0.7375,   309.444,   1.28253,   0.943686,   1.01627,   1.25478 , 0.0], 
[0.7625,   461.904,   1.30269,   0.91994,   1.01418,   1.83589  , 0.0],
[0.7875,   387.338,   1.34461,   0.971945,   1.02354,   1.23654  , 0.0],
[0.8125,   621.047,   1.41351,   1.67731,   1.01476,   1.8035  , 0.0],
[0.8375,   488.765,   1.32148,   1.28649,   1.01757,   1.96092  , 0.0],
[0.8625,   399.375,   1.32053,   1.38914,   1.01826,   1.65651  , 0.0],
[0.8875,   459.325,   1.29882,   1.21417,   1.01849,   1.80071  , 0.0],
[0.9125,   415.262,   1.29976,   1.10948,   1.02009,   1.62494  , 0.0],
[0.9375,   417.374,   1.33092,   1.18441,   1.01848,   1.59926  , 0.0],
[0.9625,   527.568,   1.26494,   0.897512,   1.02101,   1.88054  , 0.0],
[0.9875,   476.009,   1.29628,   0.951901,   1.02129,   1.73691  , 0.0],
[1.0125,   576.25,    1.29615,   0.929837,   1.01922,   1.98054  , 0.0],
[1.0375,   567.959,   1.27027,   0.771906,   1.02387,   2.12415  , 0.0],
[1.0625,   768.605,   1.26927,   0.714732,   1.02191,   2.29573  , 0.0],
[1.0875,   807.146,   1.25307,   0.610936,   1.02428,   2.36825  , 0.0],
[1.1125,   833.055,   1.27789,   0.860764,   1.02074,   2.40892  , 0.0],
[1.1375,   930.35,    1.24974,   0.81241,    1.02272,   2.49448  , 0.0],
[1.1625,   964.67,    1.23555,   0.473478,   1.02809,   2.57598  , 0.0],
[1.1875,   1158.23,   1.23535,   0.640035,   1.02228,   2.49175  , 0.0],
[1.2125,   1158.86,   1.17722,   0.617294,   1.02537,   2.66247  , 0.0],
[1.2375,   930.061,   1.17797,   0.35678,    1.03282,   2.53132  , 0.0],
[1.2625,   967.458,   1.19419,   0.681982,   1.02549,   2.21107  , 0.0],
[1.2875,   909.459,   1.18986,   0.424091,   1.03224,   2.28817 , 0.0], 
[1.3125,   812.566,   1.1688,    0.490109,   1.03571,   2.26724  , 0.0],
[1.3375,   611.449,   1.15067,   0.0,   1.04631,   1.74944  , 0.0],
[1.3625,   360.294,   1.13011,   0.0,   1.05618,   0.512014  , 0.0],
[1.3875,   423.128,   1.08216,   0.586511,   1.07919,   0.484852  , 0.0],
[1.4125,   283.678,   1.06028,   9.70986,   1.13405,   0.0  , 0.0],
[1.4375, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.4625, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.4875, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.5125, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.5375, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0],
[1.5625,   395.587,   1.39037,   1.2288,    0.996822,   0.0  , 0.0],
[1.5875,   323.342,   1.35037,   1.22883,   1.00468,   0.0  , 0.0],
[1.6125,   311.28,    1.31772,   1.07354,   1.00666,   0.0  , 0.0],
[1.6375,   446.357,   1.46131,   1.2426,    0.991885,   0.0  , 0.0],
[1.6625,   669.428,   1.68528,   2.05962,   0.969506,   0.0 , 0.0], 
[1.6875,   771.535,   1.79629,   1.68542,   0.962951,   0.0  , 0.0],
[1.7125,   362.986,   1.29034,   1.64246,   0.999155,   0.0  , 0.0],
[1.7375,   525.137,   1.18076,   1.15722,   1.00414,   1.80837  , 0.0],
[1.7625,   405.292,   1.16313,   1.1428,   1.00477,   1.20022 , 0.0], 
[1.7875,   956.475,   1.1887,   1.3461,   0.993717,   2.44407 , 0.0], 
[1.8125,   381.862,   1.04966,   0.925989,   1.01354,   0.0 , 0.0], 
[1.8375,   643.322,   3.22859,   0.89704,   1.01505,   2.00075  , 0.0],
[1.8625,   649.001,   5.18822,   1.05813,   1.01154,   1.96032  , 0.0],
[1.8875,   710.284,   0.0,   0.95535,   1.0133,   2.2612  , 0.0],
[1.9125,   877.7,     0.0,   0.911789,   1.01222,   2.35831  , 0.0],
[1.9375,   810.643,   0.0,   0.958578,   1.01289,   2.31328  , 0.0],
[1.9625,   655.504,   0.0,   0.933073,   1.015,   2.07907  , 0.0],
[1.9875,   802.918,   0.0,   0.937505,   1.0123,   2.11967  , 0.0],
[2.0125,   422.366,   0.0,   1.02381,   1.01596,   0.742854 , 0.0], 
[2.0375,   1188.99,   0.0,   0.790139,   1.01051,   2.58502 , 0.0], 
[2.0625,   859.337,   0.0,   0.678162,   1.01291,   2.13872 , 0.0], 
[2.0875,   970.541,   0.0,   0.905774,   1.00957,   2.27769 , 0.0], 
[2.1125,   1129.1,    0.0,   0.773069,   1.01005,   2.27555  , 0.0],
[2.1375,   951.611,   0.0,   0.969216,   1.01286,   2.21886 , 0.0], 
[2.1625,   1201.47,   0.0,   1.00229,   1.01116,   2.45104  , 0.0],
[2.1875,   1672.35,   0.0,   0.790886,   1.00581,   2.56663  , 0.0],
[2.2125,   1786.81,   0.0,   0.908304,   1.00704,   2.76622  , 0.0],
[2.2375,   2123.9,    0.0,   1.02943,   1.00051,   2.73235  , 0.0],
[2.2625,   812.468,   0.0,   0.889174,   1.01232,   1.79835 , 0.0], 
[2.2875,   1598.04,   0.0,   0.966816,   1.00243,   2.50984 , 0.0], 
[2.3125,   1492.13,   0.0,   0.915533,   1.00616,   2.41499 , 0.0], 
[2.3375,   1767.69,   0.0,   1.00476,   1.0046,   2.46872  , 0.0],
[2.3625,   768.353,   0.0,   0.862243,   1.01741,   1.63739  , 0.0],
[2.3875,   778.534,   0.0,   0.99564,   1.01397,   1.69124 , 0.0], 
[2.4125,   744.15,    0.0,   0.730395,   1.01621,   1.35324  , 0.0],
[2.4375,   665.676,   0.0,   0.860559,   1.01494,   1.63404  , 0.0],
[2.4625,   535.749,   0.0,   0.633483,   1.02024,   1.73377  , 0.0],
[2.4875,   782.266,   0.0,   0.802359,   1.02856,   2.674  , 0.0],
]



# Topo 420 electron, from Luis
CaloTopoLongWeights_ele420 = [
            # a  w0        w3        escale    b   eoffset 
[ 0.012500,   0, 1.0289,   1.3755,   1.0162,   0,  721.31  ],
[ 0.037500,   0, 1.0290,   0.9479,   1.0164,   0,  613.38  ],
[ 0.062500,   0, 1.1030,   1.2146,   1.0130,   0,  549.77  ],
[ 0.087500,   0, 0.9897,   0.9759,   1.0145,   0,  622.12  ],
[ 0.112500,   0, 1.0371,   0.9803,   1.0131,   0,  500.09  ],
[ 0.137500,   0, 0.9568,   0.8569,   1.0149,   0,  557.70  ],
[ 0.162500,   0, 1.0100,   1.1873,   1.0120,   0,  557.96  ],
[ 0.187500,   0, 0.9742,   1.0604,   1.0119,   0,  565.41  ],
[ 0.212500,   0, 1.0237,   0.9759,   1.0130,   0,  495.28  ],
[ 0.237500,   0, 1.0052,   0.9552,   1.0130,   0,  543.80  ],
[ 0.262500,   0, 0.9882,   0.7355,   1.0171,   0,  385.91  ],
[ 0.287500,   0, 1.0042,   0.9462,   1.0137,   0,  466.22  ],
[ 0.312500,   0, 1.0019,   0.6938,   1.0145,   0,  468.28  ],
[ 0.337500,   0, 1.0023,   0.9128,   1.0138,   0,  450.76  ],
[ 0.362500,   0, 0.9815,   0.5947,   1.0154,   0,  407.33  ],
[ 0.387500,   0, 0.9623,   0.8825,   1.0122,   0,  641.72  ],
[ 0.412500,   0, 1.0669,   0.8253,   1.0129,   0,  513.04  ],
[ 0.437500,   0, 1.0154,   1.0919,   1.0114,   0,  553.89  ],
[ 0.462500,   0, 1.0132,   0.8734,   1.0136,   0,  495.23  ],
[ 0.487500,   0, 1.0027,   0.6180,   1.0141,   0,  594.83  ],
[ 0.512500,   0, 0.9728,   0.5311,   1.0145,   0,  659.82  ],
[ 0.537500,   0, 1.0311,   0.7711,   1.0123,   0,  527.31  ],
[ 0.562500,   0, 0.9837,   0.7479,   1.0164,   0,  590.74  ],
[ 0.587500,   0, 1.0140,   0.8643,   1.0137,   0,  575.91  ],
[ 0.612500,   0, 1.0415,   0.9387,   1.0114,   0,  665.08  ],
[ 0.637500,   0, 1.0689,   0.9278,   1.0108,   0,  833.57  ],
[ 0.662500,   0, 1.0987,   0.8851,   1.0126,   0,  742.76  ],
[ 0.687500,   0, 1.0655,   0.7609,   1.0163,   0,  790.89  ],
[ 0.712500,   0, 1.0623,   0.7202,   1.0170,   0,  883.48  ],
[ 0.737500,   0, 1.0613,   0.7855,   1.0177,   0,  968.08  ],
[ 0.762500,   0, 1.0823,   0.9367,   1.0162,   0, 1106.70  ],
[ 0.787500,   0, 1.1517,   0.9401,   1.0227,   0,  946.30  ],
[ 0.812500,   0, 1.1295,   1.0357,   1.0239,   0, 1079.24  ],
[ 0.837500,   0, 1.1391,   0.6242,   1.0247,   0, 1066.95  ],
[ 0.862500,   0, 1.0958,   0.7771,   1.0212,   0, 1320.00  ],
[ 0.887500,   0, 1.1355,   0.7098,   1.0259,   0,  949.16  ],
[ 0.912500,   0, 1.0808,   0.6064,   1.0248,   0, 1233.31  ],
[ 0.937500,   0, 1.1096,   0.4948,   1.0280,   0, 1083.04  ],
[ 0.962500,   0, 1.0923,   0.3748,   1.0321,   0, 1093.83  ],
[ 0.987500,   0, 1.0797,   0.2801,   1.0327,   0, 1208.55  ],
[ 1.012500,   0, 1.0963,   0.7713,   1.0252,   0, 1373.18  ],
[ 1.037500,   0, 1.0974,   0.6490,   1.0311,   0, 1145.97  ],
[ 1.062500,   0, 1.0846,   0.2674,   1.0377,   0, 1109.77  ],
[ 1.087500,   0, 1.0686,   0.5051,   1.0349,   0, 1295.45  ],
[ 1.112500,   0, 1.1056,   0.6787,   1.0308,   0, 1275.41  ],
[ 1.137500,   0, 1.0573,   0.2611,   1.0400,   0, 1251.30  ],
[ 1.162500,   0, 1.0559,   0.4828,   1.0408,   0, 1183.87  ],
[ 1.187500,   0, 1.0374,   0.5761,   1.0359,   0, 1465.89  ],
[ 1.212500,   0, 1.0288,   0.5753,   1.0396,   0, 1353.51  ],
[ 1.237500,   0, 1.0284,   0.1835,   1.0462,   0, 1234.66  ],
[ 1.262500,   0, 1.0162,   0.2356,   1.0464,   0, 1445.89  ],
[ 1.287500,   0, 1.0123,   0.3199,   1.0475,   0, 1402.44  ],
[ 1.312500,   0, 1.0150,   0.3135,   1.0487,   0, 1524.51  ],
[ 1.337500,   0, 1.0180,   0.1334,   1.0563,   0, 1343.27  ],
[ 1.362500,   0, 1.0280,  -0.0103,   1.0549,   0, 1546.38  ],
[ 1.387500,   0, 1.0178,  -0.7423,   1.0786,   0, 1321.07  ],
[ 1.412500,   0, 1.0481,  -0.1166,   1.1189,   0,  939.59  ],
[ 1.437500,   0, 1.1033,   0.0299,   1.1726,   0, 1906.83  ],
[ 1.462500,   0, 1.6595,   0.2518,   1.4593,  -0, 2337.84  ],
[ 1.487500,   0, 0.9378,   0.9615,   2.4438,   0, -653.42  ],
[ 1.512500,   0, 1.2997,   0.6775,   1.5307,   0, 2310.51  ],
[ 1.537500,   0, 1.9511,   4.2237,   0.8710,   0, 7902.93  ],
[ 1.562500,   0, 1.2683,   1.3559,   0.9954,   0, 2604.23  ],
[ 1.587500,   0, 1.2161,   0.7973,   1.0049,   0, 2344.97  ],
[ 1.612500,   0, 1.1472,   1.0761,   1.0072,   0, 2606.22  ],
[ 1.637500,   0, 1.2180,   0.9913,   0.9971,   0, 3163.02  ],
[ 1.662500,   0, 1.4106,   1.4890,   0.9884,   0, 3356.72  ],
[ 1.687500,   0, 1.6469,   1.1393,   0.9676,   0, 2826.44  ],
[ 1.712500,   0, 1.1645,   0.9850,   0.9963,   0, 2351.64  ],
[ 1.737500,   0, 1.0847,   0.8324,   1.0042,   0, 1743.96  ],
[ 1.762500,   0, 1.0667,   0.9782,   0.9986,   0, 1771.40  ],
[ 1.787500,   0, 0.9711,   0.4376,   1.0100,   0, 1723.09  ],
[ 1.812500,   0, 0.9118,   0.4205,   1.0182,   0, 2051.05  ],
[ 1.837500,   0, 0.0000,   0.5961,   1.0158,   0, 2469.58  ],
[ 1.862500,   0, 0.0000,   0.7129,   1.0196,   0, 1578.90  ],
[ 1.887500,   0, 0.0000,   0.9220,   1.0140,   0, 1761.75  ],
[ 1.912500,   0, 0.0000,   0.5266,   1.0236,   0, 1318.12  ],
[ 1.937500,   0, 0.0000,   0.7336,   1.0139,   0, 1796.85  ],
[ 1.962500,   0, 0.0000,   0.7445,   1.0159,   0, 1859.24  ],
[ 1.987500,   0, 0.0000,   0.5836,   1.0189,   0, 1850.80  ],
[ 2.012500,   0, 0.0000,   0.4112,   1.0218,   0, 1652.48  ],
[ 2.037500,   0, 0.0000,  -0.0311,   1.0260,   0, 1751.69  ],
[ 2.062500,   0, 0.0000,   0.6097,   1.0196,   0, 1909.58  ],
[ 2.087500,   0, 0.0000,   0.7743,   1.0181,   0, 1718.31  ],
[ 2.112500,   0, 0.0000,   0.6605,   1.0219,   0, 1737.25  ],
[ 2.137500,   0, 0.0000,   0.6056,   1.0213,   0, 1719.24  ],
[ 2.162500,   0, 0.0000,   0.5229,   1.0208,   0, 1735.36  ],
[ 2.187500,   0, 0.0000,   0.4101,   1.0227,   0, 1603.74  ],
[ 2.212500,   0, 0.0000,   0.5345,   1.0201,   0, 1630.47  ],
[ 2.237500,   0, 0.0000,   0.8132,   1.0156,   0, 1605.08  ],
[ 2.262500,   0, 0.0000,   0.4881,   1.0175,   0, 1590.04  ],
[ 2.287500,   0, 0.0000,   0.7652,   1.0145,   0, 1765.23  ],
[ 2.312500,   0, 0.0000,   0.6523,   1.0187,   0, 1747.43  ],
[ 2.337500,   0, 0.0000,   0.7908,   1.0205,   0, 1605.88  ],
[ 2.362500,   0, 0.0000,   0.6193,   1.0243,   0, 1407.76  ],
[ 2.387500,   0, 0.0000,   0.6952,   1.0180,   0, 1540.78  ],
[ 2.412500,   0, 0.0000,   0.5202,   1.0228,   0, 1379.55  ],
[ 2.437500,   0, 0.0000,   0.5024,   1.0140,   0, 1849.27  ],
[ 2.462500,   0, 0.0000,   0.4072,   1.0215,   0, 1573.34  ],
[ 2.487500,   0, 0.0000,  -0.4742,   1.0393,   0, 1962.13  ]
]

class CaloTopoEMlongWeights_v1_parms:
    _default_keys = ['ele633', 'gam633']
    
    region = CALOCORR_COMBINED2
    eta_start_crack = 1.425
    eta_end_crack   = 1.55
    etamax = 2.5
    use_raw_eta = False
    degree = 3
    correction = {'ele633' : LW_electrons_1203,
                  'ele420' : CaloTopoLongWeights_ele420,
                  'ele430' : CaloTopoLongWeights_ele420,
                  'gam633' : LW_photons_1203}
