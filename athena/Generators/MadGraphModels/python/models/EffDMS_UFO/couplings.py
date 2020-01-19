# This file was automatically created by FeynRules $Revision: 1058$
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Thu 26 Jul 2012 07:58:21


from object_library import all_couplings, Coupling

from function_library import complexconjugate, re, im, csc, sec, acsc, asec



GC_1 = Coupling(name = 'GC_1',
                value = '-(ee*complex(0,1))/3.',
                order = {'QED':1})

GC_2 = Coupling(name = 'GC_2',
                value = '(2*ee*complex(0,1))/3.',
                order = {'QED':1})

GC_3 = Coupling(name = 'GC_3',
                value = '-(ee*complex(0,1))',
                order = {'QED':1})

GC_4 = Coupling(name = 'GC_4',
                value = 'ee*complex(0,1)',
                order = {'QED':1})

GC_5 = Coupling(name = 'GC_5',
                value = '2*ee**2*complex(0,1)',
                order = {'QED':2})

GC_6 = Coupling(name = 'GC_6',
                value = '-ee**2/(2.*cw)',
                order = {'QED':2})

GC_7 = Coupling(name = 'GC_7',
                value = '(ee**2*complex(0,1))/(2.*cw)',
                order = {'QED':2})

GC_8 = Coupling(name = 'GC_8',
                value = 'ee**2/(2.*cw)',
                order = {'QED':2})

GC_9 = Coupling(name = 'GC_9',
                value = '-G',
                order = {'QCD':1})

GC_10 = Coupling(name = 'GC_10',
                 value = 'complex(0,1)*G',
                 order = {'QCD':1})

GC_11 = Coupling(name = 'GC_11',
                 value = 'complex(0,1)*G**2',
                 order = {'QCD':2})

GC_12 = Coupling(name = 'GC_12',
                 value = 'cw*complex(0,1)*gw',
                 order = {'QED':1})

GC_13 = Coupling(name = 'GC_13',
                 value = '-(complex(0,1)*gw**2)',
                 order = {'QED':2})

GC_14 = Coupling(name = 'GC_14',
                 value = 'cw**2*complex(0,1)*gw**2',
                 order = {'QED':2})

GC_15 = Coupling(name = 'GC_15',
                 value = '-2*complex(0,1)*lam',
                 order = {'QED':2})

GC_16 = Coupling(name = 'GC_16',
                 value = '-4*complex(0,1)*lam',
                 order = {'QED':2})

GC_17 = Coupling(name = 'GC_17',
                 value = '-6*complex(0,1)*lam',
                 order = {'QED':2})

GC_18 = Coupling(name = 'GC_18',
                 value = '-(gC3/Mstar**2)',
                 order = {'C3':1,'QXT':2})

GC_19 = Coupling(name = 'GC_19',
                 value = 'gC4/Mstar**2',
                 order = {'C4':1,'QXT':2})

GC_20 = Coupling(name = 'GC_20',
                 value = '-(ee*MW)',
                 order = {'QED':1})

GC_21 = Coupling(name = 'GC_21',
                 value = 'ee*MW',
                 order = {'QED':1})

GC_22 = Coupling(name = 'GC_22',
                 value = '(complex(0,1)*G**2*gC5)/(4.*cmath.pi*Mstar**2)',
                 order = {'C5':1,'QCD':2,'QXT':2})

GC_23 = Coupling(name = 'GC_23',
                 value = '(G**3*gC5)/(4.*cmath.pi*Mstar**2)',
                 order = {'C5':1,'QCD':3,'QXT':2})

GC_24 = Coupling(name = 'GC_24',
                 value = '-(complex(0,1)*G**4*gC5)/(4.*cmath.pi*Mstar**2)',
                 order = {'C5':1,'QCD':4,'QXT':2})

GC_25 = Coupling(name = 'GC_25',
                 value = '(G**2*gC6)/(4.*cmath.pi*Mstar**2)',
                 order = {'C6':1,'QCD':2,'QXT':2})

GC_26 = Coupling(name = 'GC_26',
                 value = '(complex(0,1)*G**3*gC6)/(2.*cmath.pi*Mstar**2)',
                 order = {'C6':1,'QCD':3,'QXT':2})

GC_27 = Coupling(name = 'GC_27',
                 value = '(ee**2*complex(0,1))/(2.*sw**2)',
                 order = {'QED':2})

GC_28 = Coupling(name = 'GC_28',
                 value = '-(ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_29 = Coupling(name = 'GC_29',
                 value = '(ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_30 = Coupling(name = 'GC_30',
                 value = 'ee/(2.*sw)',
                 order = {'QED':1})

GC_31 = Coupling(name = 'GC_31',
                 value = '(ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_32 = Coupling(name = 'GC_32',
                 value = '(CKM11*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_33 = Coupling(name = 'GC_33',
                 value = '(CKM12*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_34 = Coupling(name = 'GC_34',
                 value = '(CKM13*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_35 = Coupling(name = 'GC_35',
                 value = '(CKM21*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_36 = Coupling(name = 'GC_36',
                 value = '(CKM22*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_37 = Coupling(name = 'GC_37',
                 value = '(CKM23*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_38 = Coupling(name = 'GC_38',
                 value = '(CKM31*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_39 = Coupling(name = 'GC_39',
                 value = '(CKM32*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_40 = Coupling(name = 'GC_40',
                 value = '(CKM33*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_41 = Coupling(name = 'GC_41',
                 value = '-(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_42 = Coupling(name = 'GC_42',
                 value = '(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_43 = Coupling(name = 'GC_43',
                 value = '-((cw*ee*complex(0,1))/sw)',
                 order = {'QED':1})

GC_44 = Coupling(name = 'GC_44',
                 value = '(cw*ee*complex(0,1))/sw',
                 order = {'QED':1})

GC_45 = Coupling(name = 'GC_45',
                 value = '-ee**2/(2.*sw)',
                 order = {'QED':2})

GC_46 = Coupling(name = 'GC_46',
                 value = '-(ee**2*complex(0,1))/(2.*sw)',
                 order = {'QED':2})

GC_47 = Coupling(name = 'GC_47',
                 value = 'ee**2/(2.*sw)',
                 order = {'QED':2})

GC_48 = Coupling(name = 'GC_48',
                 value = '-(ee*MW)/(2.*sw)',
                 order = {'QED':1})

GC_49 = Coupling(name = 'GC_49',
                 value = '-(ee*complex(0,1)*MW)/(2.*sw)',
                 order = {'QED':1})

GC_50 = Coupling(name = 'GC_50',
                 value = '(ee*MW)/(2.*sw)',
                 order = {'QED':1})

GC_51 = Coupling(name = 'GC_51',
                 value = '-(ee*MZ)/(2.*sw)',
                 order = {'QED':1})

GC_52 = Coupling(name = 'GC_52',
                 value = '(ee*MZ)/(2.*sw)',
                 order = {'QED':1})

GC_53 = Coupling(name = 'GC_53',
                 value = '-(ee*complex(0,1)*MZ)/(2.*cw*sw)',
                 order = {'QED':1})

GC_54 = Coupling(name = 'GC_54',
                 value = '-(ee*complex(0,1)*sw)/(6.*cw)',
                 order = {'QED':1})

GC_55 = Coupling(name = 'GC_55',
                 value = '(ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_56 = Coupling(name = 'GC_56',
                 value = 'complex(0,1)*gw*sw',
                 order = {'QED':1})

GC_57 = Coupling(name = 'GC_57',
                 value = '-2*cw*complex(0,1)*gw**2*sw',
                 order = {'QED':2})

GC_58 = Coupling(name = 'GC_58',
                 value = 'complex(0,1)*gw**2*sw**2',
                 order = {'QED':2})

GC_59 = Coupling(name = 'GC_59',
                 value = '-(cw*ee*complex(0,1))/(2.*sw) + (ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_60 = Coupling(name = 'GC_60',
                 value = '(cw*ee*complex(0,1))/(2.*sw) + (ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_61 = Coupling(name = 'GC_61',
                 value = '(cw*ee)/(2.*sw) + (ee*sw)/(2.*cw)',
                 order = {'QED':1})

GC_62 = Coupling(name = 'GC_62',
                 value = '(cw*ee**2*complex(0,1))/sw - (ee**2*complex(0,1)*sw)/cw',
                 order = {'QED':2})

GC_63 = Coupling(name = 'GC_63',
                 value = '(cw*ee*MW)/(2.*sw) - (ee*MW*sw)/(2.*cw)',
                 order = {'QED':1})

GC_64 = Coupling(name = 'GC_64',
                 value = '-(cw*ee*MW)/(2.*sw) + (ee*MW*sw)/(2.*cw)',
                 order = {'QED':1})

GC_65 = Coupling(name = 'GC_65',
                 value = '-(ee**2*complex(0,1)) + (cw**2*ee**2*complex(0,1))/(2.*sw**2) + (ee**2*complex(0,1)*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_66 = Coupling(name = 'GC_66',
                 value = 'ee**2*complex(0,1) + (cw**2*ee**2*complex(0,1))/(2.*sw**2) + (ee**2*complex(0,1)*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_67 = Coupling(name = 'GC_67',
                 value = '-(ee**2*v)/(2.*cw)',
                 order = {'QED':1})

GC_68 = Coupling(name = 'GC_68',
                 value = '(ee**2*v)/(2.*cw)',
                 order = {'QED':1})

GC_69 = Coupling(name = 'GC_69',
                 value = '-2*complex(0,1)*lam*v',
                 order = {'QED':1})

GC_70 = Coupling(name = 'GC_70',
                 value = '-6*complex(0,1)*lam*v',
                 order = {'QED':1})

GC_71 = Coupling(name = 'GC_71',
                 value = '(ee**2*complex(0,1)*v)/(2.*sw**2)',
                 order = {'QED':1})

GC_72 = Coupling(name = 'GC_72',
                 value = '-(ee**2*v)/(2.*sw)',
                 order = {'QED':1})

GC_73 = Coupling(name = 'GC_73',
                 value = '(ee**2*v)/(2.*sw)',
                 order = {'QED':1})

GC_74 = Coupling(name = 'GC_74',
                 value = 'ee**2*complex(0,1)*v + (cw**2*ee**2*complex(0,1)*v)/(2.*sw**2) + (ee**2*complex(0,1)*sw**2*v)/(2.*cw**2)',
                 order = {'QED':1})

GC_75 = Coupling(name = 'GC_75',
                 value = '-((complex(0,1)*yb)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_76 = Coupling(name = 'GC_76',
                 value = 'yb/cmath.sqrt(2)',
                 order = {'QED':1})

GC_77 = Coupling(name = 'GC_77',
                 value = '-(CKM13*yb)',
                 order = {'QED':1})

GC_78 = Coupling(name = 'GC_78',
                 value = '-(CKM23*yb)',
                 order = {'QED':1})

GC_79 = Coupling(name = 'GC_79',
                 value = '-(CKM33*yb)',
                 order = {'QED':1})

GC_80 = Coupling(name = 'GC_80',
                 value = '-(yc/cmath.sqrt(2))',
                 order = {'QED':1})

GC_81 = Coupling(name = 'GC_81',
                 value = '-((complex(0,1)*yc)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_82 = Coupling(name = 'GC_82',
                 value = 'CKM21*yc',
                 order = {'QED':1})

GC_83 = Coupling(name = 'GC_83',
                 value = 'CKM22*yc',
                 order = {'QED':1})

GC_84 = Coupling(name = 'GC_84',
                 value = 'CKM23*yc',
                 order = {'QED':1})

GC_85 = Coupling(name = 'GC_85',
                 value = '-((complex(0,1)*ydo)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_86 = Coupling(name = 'GC_86',
                 value = 'ydo/cmath.sqrt(2)',
                 order = {'QED':1})

GC_87 = Coupling(name = 'GC_87',
                 value = '-(CKM11*ydo)',
                 order = {'QED':1})

GC_88 = Coupling(name = 'GC_88',
                 value = '-(CKM21*ydo)',
                 order = {'QED':1})

GC_89 = Coupling(name = 'GC_89',
                 value = '-(CKM31*ydo)',
                 order = {'QED':1})

GC_90 = Coupling(name = 'GC_90',
                 value = '-ye',
                 order = {'QED':1})

GC_91 = Coupling(name = 'GC_91',
                 value = 'ye',
                 order = {'QED':1})

GC_92 = Coupling(name = 'GC_92',
                 value = '-((complex(0,1)*ye)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_93 = Coupling(name = 'GC_93',
                 value = 'ye/cmath.sqrt(2)',
                 order = {'QED':1})

GC_94 = Coupling(name = 'GC_94',
                 value = '-ym',
                 order = {'QED':1})

GC_95 = Coupling(name = 'GC_95',
                 value = 'ym',
                 order = {'QED':1})

GC_96 = Coupling(name = 'GC_96',
                 value = '-((complex(0,1)*ym)/cmath.sqrt(2))',
                 order = {'QED':1})

GC_97 = Coupling(name = 'GC_97',
                 value = 'ym/cmath.sqrt(2)',
                 order = {'QED':1})

GC_98 = Coupling(name = 'GC_98',
                 value = '(complex(0,1)*gC1*ymb)/Mstar**2',
                 order = {'C1':1,'QXT':2})

GC_99 = Coupling(name = 'GC_99',
                 value = '(gC2*ymb)/Mstar**2',
                 order = {'C2':1,'QXT':2})

GC_100 = Coupling(name = 'GC_100',
                  value = '(complex(0,1)*gC1*ymc)/Mstar**2',
                  order = {'C1':1,'QXT':2})

GC_101 = Coupling(name = 'GC_101',
                  value = '(gC2*ymc)/Mstar**2',
                  order = {'C2':1,'QXT':2})

GC_102 = Coupling(name = 'GC_102',
                  value = '(complex(0,1)*gC1*ymdo)/Mstar**2',
                  order = {'C1':1,'QXT':2})

GC_103 = Coupling(name = 'GC_103',
                  value = '(gC2*ymdo)/Mstar**2',
                  order = {'C2':1,'QXT':2})

GC_104 = Coupling(name = 'GC_104',
                  value = '(complex(0,1)*gC1*yms)/Mstar**2',
                  order = {'C1':1,'QXT':2})

GC_105 = Coupling(name = 'GC_105',
                  value = '(gC2*yms)/Mstar**2',
                  order = {'C2':1,'QXT':2})

GC_106 = Coupling(name = 'GC_106',
                  value = '(complex(0,1)*gC1*ymt)/Mstar**2',
                  order = {'C1':1,'QXT':2})

GC_107 = Coupling(name = 'GC_107',
                  value = '(gC2*ymt)/Mstar**2',
                  order = {'C2':1,'QXT':2})

GC_108 = Coupling(name = 'GC_108',
                  value = '(complex(0,1)*gC1*ymup)/Mstar**2',
                  order = {'C1':1,'QXT':2})

GC_109 = Coupling(name = 'GC_109',
                  value = '(gC2*ymup)/Mstar**2',
                  order = {'C2':1,'QXT':2})

GC_110 = Coupling(name = 'GC_110',
                  value = '-((complex(0,1)*ys)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_111 = Coupling(name = 'GC_111',
                  value = 'ys/cmath.sqrt(2)',
                  order = {'QED':1})

GC_112 = Coupling(name = 'GC_112',
                  value = '-(CKM12*ys)',
                  order = {'QED':1})

GC_113 = Coupling(name = 'GC_113',
                  value = '-(CKM22*ys)',
                  order = {'QED':1})

GC_114 = Coupling(name = 'GC_114',
                  value = '-(CKM32*ys)',
                  order = {'QED':1})

GC_115 = Coupling(name = 'GC_115',
                  value = '-(yt/cmath.sqrt(2))',
                  order = {'QED':1})

GC_116 = Coupling(name = 'GC_116',
                  value = '-((complex(0,1)*yt)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_117 = Coupling(name = 'GC_117',
                  value = 'CKM31*yt',
                  order = {'QED':1})

GC_118 = Coupling(name = 'GC_118',
                  value = 'CKM32*yt',
                  order = {'QED':1})

GC_119 = Coupling(name = 'GC_119',
                  value = 'CKM33*yt',
                  order = {'QED':1})

GC_120 = Coupling(name = 'GC_120',
                  value = '-ytau',
                  order = {'QED':1})

GC_121 = Coupling(name = 'GC_121',
                  value = 'ytau',
                  order = {'QED':1})

GC_122 = Coupling(name = 'GC_122',
                  value = '-((complex(0,1)*ytau)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_123 = Coupling(name = 'GC_123',
                  value = 'ytau/cmath.sqrt(2)',
                  order = {'QED':1})

GC_124 = Coupling(name = 'GC_124',
                  value = '-(yup/cmath.sqrt(2))',
                  order = {'QED':1})

GC_125 = Coupling(name = 'GC_125',
                  value = '-((complex(0,1)*yup)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_126 = Coupling(name = 'GC_126',
                  value = 'CKM11*yup',
                  order = {'QED':1})

GC_127 = Coupling(name = 'GC_127',
                  value = 'CKM12*yup',
                  order = {'QED':1})

GC_128 = Coupling(name = 'GC_128',
                  value = 'CKM13*yup',
                  order = {'QED':1})

GC_129 = Coupling(name = 'GC_129',
                  value = '(ee*complex(0,1)*complexconjugate(CKM11))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_130 = Coupling(name = 'GC_130',
                  value = 'ydo*complexconjugate(CKM11)',
                  order = {'QED':1})

GC_131 = Coupling(name = 'GC_131',
                  value = '-(yup*complexconjugate(CKM11))',
                  order = {'QED':1})

GC_132 = Coupling(name = 'GC_132',
                  value = '(ee*complex(0,1)*complexconjugate(CKM12))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_133 = Coupling(name = 'GC_133',
                  value = 'ys*complexconjugate(CKM12)',
                  order = {'QED':1})

GC_134 = Coupling(name = 'GC_134',
                  value = '-(yup*complexconjugate(CKM12))',
                  order = {'QED':1})

GC_135 = Coupling(name = 'GC_135',
                  value = '(ee*complex(0,1)*complexconjugate(CKM13))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_136 = Coupling(name = 'GC_136',
                  value = 'yb*complexconjugate(CKM13)',
                  order = {'QED':1})

GC_137 = Coupling(name = 'GC_137',
                  value = '-(yup*complexconjugate(CKM13))',
                  order = {'QED':1})

GC_138 = Coupling(name = 'GC_138',
                  value = '(ee*complex(0,1)*complexconjugate(CKM21))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_139 = Coupling(name = 'GC_139',
                  value = '-(yc*complexconjugate(CKM21))',
                  order = {'QED':1})

GC_140 = Coupling(name = 'GC_140',
                  value = 'ydo*complexconjugate(CKM21)',
                  order = {'QED':1})

GC_141 = Coupling(name = 'GC_141',
                  value = '(ee*complex(0,1)*complexconjugate(CKM22))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_142 = Coupling(name = 'GC_142',
                  value = '-(yc*complexconjugate(CKM22))',
                  order = {'QED':1})

GC_143 = Coupling(name = 'GC_143',
                  value = 'ys*complexconjugate(CKM22)',
                  order = {'QED':1})

GC_144 = Coupling(name = 'GC_144',
                  value = '(ee*complex(0,1)*complexconjugate(CKM23))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_145 = Coupling(name = 'GC_145',
                  value = 'yb*complexconjugate(CKM23)',
                  order = {'QED':1})

GC_146 = Coupling(name = 'GC_146',
                  value = '-(yc*complexconjugate(CKM23))',
                  order = {'QED':1})

GC_147 = Coupling(name = 'GC_147',
                  value = '(ee*complex(0,1)*complexconjugate(CKM31))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_148 = Coupling(name = 'GC_148',
                  value = 'ydo*complexconjugate(CKM31)',
                  order = {'QED':1})

GC_149 = Coupling(name = 'GC_149',
                  value = '-(yt*complexconjugate(CKM31))',
                  order = {'QED':1})

GC_150 = Coupling(name = 'GC_150',
                  value = '(ee*complex(0,1)*complexconjugate(CKM32))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_151 = Coupling(name = 'GC_151',
                  value = 'ys*complexconjugate(CKM32)',
                  order = {'QED':1})

GC_152 = Coupling(name = 'GC_152',
                  value = '-(yt*complexconjugate(CKM32))',
                  order = {'QED':1})

GC_153 = Coupling(name = 'GC_153',
                  value = '(ee*complex(0,1)*complexconjugate(CKM33))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_154 = Coupling(name = 'GC_154',
                  value = 'yb*complexconjugate(CKM33)',
                  order = {'QED':1})

GC_155 = Coupling(name = 'GC_155',
                  value = '-(yt*complexconjugate(CKM33))',
                  order = {'QED':1})

