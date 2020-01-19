# This file was automatically created by FeynRules $Revision: 595 $
# Mathematica version: 9.0 for Mac OS X x86 (64-bit) (November 20, 2012)
# Date: Wed 5 Jun 2013 11:59:40


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
                value = '-2*FM0*complex(0,1)',
                order = {'NP':1})

GC_5 = Coupling(name = 'GC_5',
                value = '-(FM1*complex(0,1))/2.',
                order = {'NP':1})

GC_6 = Coupling(name = 'GC_6',
                value = '2*ee**2*FM2*complex(0,1)',
                order = {'NP':1,'QED':2})

GC_7 = Coupling(name = 'GC_7',
                value = '-(ee**2*FM3*complex(0,1))/2.',
                order = {'NP':1,'QED':2})

GC_8 = Coupling(name = 'GC_8',
                value = '-G',
                order = {'QCD':1})

GC_9 = Coupling(name = 'GC_9',
                value = 'complex(0,1)*G',
                order = {'QCD':1})

GC_10 = Coupling(name = 'GC_10',
                 value = 'complex(0,1)*G**2',
                 order = {'QCD':2})

GC_11 = Coupling(name = 'GC_11',
                 value = 'cw*complex(0,1)*gw',
                 order = {'QED':1})

GC_12 = Coupling(name = 'GC_12',
                 value = '2*cw*FM0*complex(0,1)*gw',
                 order = {'NP':1,'QED':1})

GC_13 = Coupling(name = 'GC_13',
                 value = '-(cw*FM1*complex(0,1)*gw)/2.',
                 order = {'NP':1,'QED':1})

GC_14 = Coupling(name = 'GC_14',
                 value = '-(complex(0,1)*gw**2)',
                 order = {'QED':2})

GC_15 = Coupling(name = 'GC_15',
                 value = 'cw**2*complex(0,1)*gw**2',
                 order = {'QED':2})

GC_16 = Coupling(name = 'GC_16',
                 value = '-2*FM0*complex(0,1)*gw**2',
                 order = {'NP':1,'QED':2})

GC_17 = Coupling(name = 'GC_17',
                 value = '2*cw**2*FM0*complex(0,1)*gw**2',
                 order = {'NP':1,'QED':2})

GC_18 = Coupling(name = 'GC_18',
                 value = '-(ee**2*FM0*complex(0,1)*gw**2)',
                 order = {'NP':1,'QED':4})

GC_19 = Coupling(name = 'GC_19',
                 value = '-(FM1*complex(0,1)*gw**2)',
                 order = {'NP':1,'QED':2})

GC_20 = Coupling(name = 'GC_20',
                 value = 'cw**2*FM1*complex(0,1)*gw**2',
                 order = {'NP':1,'QED':2})

GC_21 = Coupling(name = 'GC_21',
                 value = 'ee**2*FM1*complex(0,1)*gw**2',
                 order = {'NP':1,'QED':4})

GC_22 = Coupling(name = 'GC_22',
                 value = '-6*complex(0,1)*lam',
                 order = {'QED':2})

GC_23 = Coupling(name = 'GC_23',
                 value = 'ee**2*FM0*complex(0,1) + (2*cw**2*ee**2*FM2*complex(0,1))/sw**2',
                 order = {'NP':1,'QED':2})

GC_24 = Coupling(name = 'GC_24',
                 value = '(ee**2*FM1*complex(0,1))/4. + (cw**2*ee**2*FM3*complex(0,1))/(2.*sw**2)',
                 order = {'NP':1,'QED':2})

GC_25 = Coupling(name = 'GC_25',
                 value = '(3*ee**2*FM0*complex(0,1)*gw**2)/sw**2 - (3*ee**2*FM1*complex(0,1)*gw**2)/(2.*sw**2)',
                 order = {'NP':1,'QED':4})

GC_26 = Coupling(name = 'GC_26',
                 value = '(cw*ee**2*FM0*complex(0,1))/sw - (2*cw*ee**2*FM2*complex(0,1))/sw',
                 order = {'NP':1,'QED':2})

GC_27 = Coupling(name = 'GC_27',
                 value = '-(cw*ee**2*FM1*complex(0,1))/(4.*sw) + (cw*ee**2*FM3*complex(0,1))/(2.*sw)',
                 order = {'NP':1,'QED':2})

GC_28 = Coupling(name = 'GC_28',
                 value = '(ee**2*complex(0,1))/(2.*sw**2)',
                 order = {'QED':2})

GC_29 = Coupling(name = 'GC_29',
                 value = '(ee**2*FM0*complex(0,1))/sw**2',
                 order = {'NP':1,'QED':2})

GC_30 = Coupling(name = 'GC_30',
                 value = '(cw**2*ee**2*FM0*complex(0,1))/sw**2',
                 order = {'NP':1,'QED':2})

GC_31 = Coupling(name = 'GC_31',
                 value = '-(ee**2*FM1*complex(0,1))/(4.*sw**2)',
                 order = {'NP':1,'QED':2})

GC_32 = Coupling(name = 'GC_32',
                 value = '(cw**2*ee**2*FM1*complex(0,1))/(4.*sw**2)',
                 order = {'NP':1,'QED':2})

GC_33 = Coupling(name = 'GC_33',
                 value = '-((cw*ee**2*FM0*complex(0,1)*gw)/sw**2)',
                 order = {'NP':1,'QED':3})

GC_34 = Coupling(name = 'GC_34',
                 value = '(cw*ee**2*FM1*complex(0,1)*gw)/(4.*sw**2)',
                 order = {'NP':1,'QED':3})

GC_35 = Coupling(name = 'GC_35',
                 value = '-((cw**2*ee**2*FM0*complex(0,1)*gw**2)/sw**2)',
                 order = {'NP':1,'QED':4})

GC_36 = Coupling(name = 'GC_36',
                 value = '(3*cw**2*ee**2*FM1*complex(0,1)*gw**2)/(2.*sw**2)',
                 order = {'NP':1,'QED':4})

GC_37 = Coupling(name = 'GC_37',
                 value = '(ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_38 = Coupling(name = 'GC_38',
                 value = '(CKM11*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_39 = Coupling(name = 'GC_39',
                 value = '(CKM12*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_40 = Coupling(name = 'GC_40',
                 value = '(CKM21*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_41 = Coupling(name = 'GC_41',
                 value = '(CKM22*ee*complex(0,1))/(sw*cmath.sqrt(2))',
                 order = {'QED':1})

GC_42 = Coupling(name = 'GC_42',
                 value = '-(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_43 = Coupling(name = 'GC_43',
                 value = '(cw*ee*complex(0,1))/(2.*sw)',
                 order = {'QED':1})

GC_44 = Coupling(name = 'GC_44',
                 value = '-((ee**2*FM0*complex(0,1)*gw)/sw)',
                 order = {'NP':1,'QED':3})

GC_45 = Coupling(name = 'GC_45',
                 value = '-(ee**2*FM1*complex(0,1)*gw)/(4.*sw)',
                 order = {'NP':1,'QED':3})

GC_46 = Coupling(name = 'GC_46',
                 value = '(4*cw*ee**2*FM0*complex(0,1)*gw**2)/sw',
                 order = {'NP':1,'QED':4})

GC_47 = Coupling(name = 'GC_47',
                 value = '-((cw*ee**2*FM1*complex(0,1)*gw**2)/sw)',
                 order = {'NP':1,'QED':4})

GC_48 = Coupling(name = 'GC_48',
                 value = '-(ee*complex(0,1)*sw)/(6.*cw)',
                 order = {'QED':1})

GC_49 = Coupling(name = 'GC_49',
                 value = '(ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_50 = Coupling(name = 'GC_50',
                 value = 'complex(0,1)*gw*sw',
                 order = {'QED':1})

GC_51 = Coupling(name = 'GC_51',
                 value = '2*FM0*complex(0,1)*gw*sw',
                 order = {'NP':1,'QED':1})

GC_52 = Coupling(name = 'GC_52',
                 value = '-(FM1*complex(0,1)*gw*sw)/2.',
                 order = {'NP':1,'QED':1})

GC_53 = Coupling(name = 'GC_53',
                 value = '-2*cw*complex(0,1)*gw**2*sw',
                 order = {'QED':2})

GC_54 = Coupling(name = 'GC_54',
                 value = '-4*cw*FM0*complex(0,1)*gw**2*sw',
                 order = {'NP':1,'QED':2})

GC_55 = Coupling(name = 'GC_55',
                 value = '-(cw*FM1*complex(0,1)*gw**2*sw)/2.',
                 order = {'NP':1,'QED':2})

GC_56 = Coupling(name = 'GC_56',
                 value = 'complex(0,1)*gw**2*sw**2',
                 order = {'QED':2})

GC_57 = Coupling(name = 'GC_57',
                 value = '2*FM0*complex(0,1)*gw**2*sw**2',
                 order = {'NP':1,'QED':2})

GC_58 = Coupling(name = 'GC_58',
                 value = 'FM1*complex(0,1)*gw**2*sw**2',
                 order = {'NP':1,'QED':2})

GC_59 = Coupling(name = 'GC_59',
                 value = '(cw*ee*complex(0,1))/(2.*sw) + (ee*complex(0,1)*sw)/(2.*cw)',
                 order = {'QED':1})

GC_60 = Coupling(name = 'GC_60',
                 value = '-2*cw*FM0*complex(0,1)*sw + 4*cw*FM2*complex(0,1)*sw',
                 order = {'NP':1})

GC_61 = Coupling(name = 'GC_61',
                 value = '-(cw*FM1*complex(0,1)*sw)/2. + cw*FM3*complex(0,1)*sw',
                 order = {'NP':1})

GC_62 = Coupling(name = 'GC_62',
                 value = 'ee**2*complex(0,1) + (cw**2*ee**2*complex(0,1))/(2.*sw**2) + (ee**2*complex(0,1)*sw**2)/(2.*cw**2)',
                 order = {'QED':2})

GC_63 = Coupling(name = 'GC_63',
                 value = '-4*cw**2*FM2*complex(0,1) - 2*FM0*complex(0,1)*sw**2',
                 order = {'NP':1})

GC_64 = Coupling(name = 'GC_64',
                 value = '2*ee**2*FM0*complex(0,1) + (ee**2*FM0*complex(0,1)*sw**2)/cw**2',
                 order = {'NP':1,'QED':2})

GC_65 = Coupling(name = 'GC_65',
                 value = '-(cw**2*FM3*complex(0,1)) - (FM1*complex(0,1)*sw**2)/2.',
                 order = {'NP':1})

GC_66 = Coupling(name = 'GC_66',
                 value = '(ee**2*FM1*complex(0,1))/2. + (ee**2*FM1*complex(0,1)*sw**2)/(4.*cw**2)',
                 order = {'NP':1,'QED':2})

GC_67 = Coupling(name = 'GC_67',
                 value = '-2*cw**2*FM0*complex(0,1) - 4*FM2*complex(0,1)*sw**2',
                 order = {'NP':1})

GC_68 = Coupling(name = 'GC_68',
                 value = '-(cw**2*FM1*complex(0,1))/2. - FM3*complex(0,1)*sw**2',
                 order = {'NP':1})

GC_69 = Coupling(name = 'GC_69',
                 value = '-2*cw*ee**2*FM0*complex(0,1)*gw - (cw**3*ee**2*FM0*complex(0,1)*gw)/sw**2 - (ee**2*FM0*complex(0,1)*gw*sw**2)/cw',
                 order = {'NP':1,'QED':3})

GC_70 = Coupling(name = 'GC_70',
                 value = '-(cw*ee**2*FM1*complex(0,1)*gw)/2. - (cw**3*ee**2*FM1*complex(0,1)*gw)/(4.*sw**2) - (ee**2*FM1*complex(0,1)*gw*sw**2)/(4.*cw)',
                 order = {'NP':1,'QED':3})

GC_71 = Coupling(name = 'GC_71',
                 value = '2*ee**2*FM0*complex(0,1)*gw**2 + (ee**2*FM0*complex(0,1)*gw**2*sw**2)/cw**2',
                 order = {'NP':1,'QED':4})

GC_72 = Coupling(name = 'GC_72',
                 value = '-2*cw**2*ee**2*FM0*complex(0,1)*gw**2 + cw**2*ee**2*FM1*complex(0,1)*gw**2 - (cw**4*ee**2*FM0*complex(0,1)*gw**2)/sw**2 + (cw**4*ee**2*FM1*complex(0,1)*gw**2)/(2.*sw**2) - ee**2*FM0*complex(0,1)*gw**2*sw**2 + (ee**2*FM1*complex(0,1)*gw**2*sw**2)/2.',
                 order = {'NP':1,'QED':4})

GC_73 = Coupling(name = 'GC_73',
                 value = 'ee**2*FM1*complex(0,1)*gw**2 + (ee**2*FM1*complex(0,1)*gw**2*sw**2)/(2.*cw**2)',
                 order = {'NP':1,'QED':4})

GC_74 = Coupling(name = 'GC_74',
                 value = '(cw**3*ee**2*FM0*complex(0,1))/sw - (2*cw**3*ee**2*FM2*complex(0,1))/sw + 2*cw*ee**2*FM0*complex(0,1)*sw - 4*cw*ee**2*FM2*complex(0,1)*sw + (ee**2*FM0*complex(0,1)*sw**3)/cw - (2*ee**2*FM2*complex(0,1)*sw**3)/cw',
                 order = {'NP':1,'QED':2})

GC_75 = Coupling(name = 'GC_75',
                 value = '(cw**3*ee**2*FM1*complex(0,1))/(4.*sw) - (cw**3*ee**2*FM3*complex(0,1))/(2.*sw) + (cw*ee**2*FM1*complex(0,1)*sw)/2. - cw*ee**2*FM3*complex(0,1)*sw + (ee**2*FM1*complex(0,1)*sw**3)/(4.*cw) - (ee**2*FM3*complex(0,1)*sw**3)/(2.*cw)',
                 order = {'NP':1,'QED':2})

GC_76 = Coupling(name = 'GC_76',
                 value = '-((cw**2*ee**2*FM0*complex(0,1)*gw)/sw) - 2*ee**2*FM0*complex(0,1)*gw*sw - (ee**2*FM0*complex(0,1)*gw*sw**3)/cw**2',
                 order = {'NP':1,'QED':3})

GC_77 = Coupling(name = 'GC_77',
                 value = '(cw**2*ee**2*FM1*complex(0,1)*gw)/(4.*sw) + (ee**2*FM1*complex(0,1)*gw*sw)/2. + (ee**2*FM1*complex(0,1)*gw*sw**3)/(4.*cw**2)',
                 order = {'NP':1,'QED':3})

GC_78 = Coupling(name = 'GC_78',
                 value = '(2*cw**3*ee**2*FM0*complex(0,1)*gw**2)/sw - (cw**3*ee**2*FM1*complex(0,1)*gw**2)/sw + 4*cw*ee**2*FM0*complex(0,1)*gw**2*sw - 2*cw*ee**2*FM1*complex(0,1)*gw**2*sw + (2*ee**2*FM0*complex(0,1)*gw**2*sw**3)/cw - (ee**2*FM1*complex(0,1)*gw**2*sw**3)/cw',
                 order = {'NP':1,'QED':4})

GC_79 = Coupling(name = 'GC_79',
                 value = 'cw**2*ee**2*FM0*complex(0,1) + 4*cw**2*ee**2*FM2*complex(0,1) + (2*cw**4*ee**2*FM2*complex(0,1))/sw**2 + 2*ee**2*FM0*complex(0,1)*sw**2 + 2*ee**2*FM2*complex(0,1)*sw**2 + (ee**2*FM0*complex(0,1)*sw**4)/cw**2',
                 order = {'NP':1,'QED':2})

GC_80 = Coupling(name = 'GC_80',
                 value = '(cw**2*ee**2*FM1*complex(0,1))/4. + cw**2*ee**2*FM3*complex(0,1) + (cw**4*ee**2*FM3*complex(0,1))/(2.*sw**2) + (ee**2*FM1*complex(0,1)*sw**2)/2. + (ee**2*FM3*complex(0,1)*sw**2)/2. + (ee**2*FM1*complex(0,1)*sw**4)/(4.*cw**2)',
                 order = {'NP':1,'QED':2})

GC_81 = Coupling(name = 'GC_81',
                 value = '2*cw**2*ee**2*FM0*complex(0,1) + 2*cw**2*ee**2*FM2*complex(0,1) + (cw**4*ee**2*FM0*complex(0,1))/sw**2 + ee**2*FM0*complex(0,1)*sw**2 + 4*ee**2*FM2*complex(0,1)*sw**2 + (2*ee**2*FM2*complex(0,1)*sw**4)/cw**2',
                 order = {'NP':1,'QED':2})

GC_82 = Coupling(name = 'GC_82',
                 value = '(cw**2*ee**2*FM1*complex(0,1))/2. + (cw**2*ee**2*FM3*complex(0,1))/2. + (cw**4*ee**2*FM1*complex(0,1))/(4.*sw**2) + (ee**2*FM1*complex(0,1)*sw**2)/4. + ee**2*FM3*complex(0,1)*sw**2 + (ee**2*FM3*complex(0,1)*sw**4)/(2.*cw**2)',
                 order = {'NP':1,'QED':2})

GC_83 = Coupling(name = 'GC_83',
                 value = '-(cw**2*ee**2*FM0*complex(0,1)*gw**2) - 2*ee**2*FM0*complex(0,1)*gw**2*sw**2 - (ee**2*FM0*complex(0,1)*gw**2*sw**4)/cw**2',
                 order = {'NP':1,'QED':4})

GC_84 = Coupling(name = 'GC_84',
                 value = '-(cw**2*ee**2*FM1*complex(0,1)*gw**2)/2. - ee**2*FM1*complex(0,1)*gw**2*sw**2 - (ee**2*FM1*complex(0,1)*gw**2*sw**4)/(2.*cw**2)',
                 order = {'NP':1,'QED':4})

GC_85 = Coupling(name = 'GC_85',
                 value = '2*ee**2*FM2*complex(0,1)*v',
                 order = {'NP':1,'QED':1})

GC_86 = Coupling(name = 'GC_86',
                 value = '-(ee**2*FM3*complex(0,1)*v)/2.',
                 order = {'NP':1,'QED':1})

GC_87 = Coupling(name = 'GC_87',
                 value = '-(ee**2*FM0*complex(0,1)*gw**2*v)',
                 order = {'NP':1,'QED':3})

GC_88 = Coupling(name = 'GC_88',
                 value = 'ee**2*FM1*complex(0,1)*gw**2*v',
                 order = {'NP':1,'QED':3})

GC_89 = Coupling(name = 'GC_89',
                 value = '-6*complex(0,1)*lam*v',
                 order = {'QED':1})

GC_90 = Coupling(name = 'GC_90',
                 value = '(ee**2*complex(0,1)*v)/(2.*sw**2)',
                 order = {'QED':1})

GC_91 = Coupling(name = 'GC_91',
                 value = '(ee**2*FM0*complex(0,1)*v)/sw**2',
                 order = {'NP':1,'QED':1})

GC_92 = Coupling(name = 'GC_92',
                 value = '(cw**2*ee**2*FM0*complex(0,1)*v)/sw**2',
                 order = {'NP':1,'QED':1})

GC_93 = Coupling(name = 'GC_93',
                 value = '-(ee**2*FM1*complex(0,1)*v)/(4.*sw**2)',
                 order = {'NP':1,'QED':1})

GC_94 = Coupling(name = 'GC_94',
                 value = '(cw**2*ee**2*FM1*complex(0,1)*v)/(4.*sw**2)',
                 order = {'NP':1,'QED':1})

GC_95 = Coupling(name = 'GC_95',
                 value = '-((cw*ee**2*FM0*complex(0,1)*gw*v)/sw**2)',
                 order = {'NP':1,'QED':2})

GC_96 = Coupling(name = 'GC_96',
                 value = '(cw*ee**2*FM1*complex(0,1)*gw*v)/(4.*sw**2)',
                 order = {'NP':1,'QED':2})

GC_97 = Coupling(name = 'GC_97',
                 value = '-((cw**2*ee**2*FM0*complex(0,1)*gw**2*v)/sw**2)',
                 order = {'NP':1,'QED':3})

GC_98 = Coupling(name = 'GC_98',
                 value = '(3*cw**2*ee**2*FM1*complex(0,1)*gw**2*v)/(2.*sw**2)',
                 order = {'NP':1,'QED':3})

GC_99 = Coupling(name = 'GC_99',
                 value = '-((ee**2*FM0*complex(0,1)*gw*v)/sw)',
                 order = {'NP':1,'QED':2})

GC_100 = Coupling(name = 'GC_100',
                  value = '-(ee**2*FM1*complex(0,1)*gw*v)/(4.*sw)',
                  order = {'NP':1,'QED':2})

GC_101 = Coupling(name = 'GC_101',
                  value = '(4*cw*ee**2*FM0*complex(0,1)*gw**2*v)/sw',
                  order = {'NP':1,'QED':3})

GC_102 = Coupling(name = 'GC_102',
                  value = '-((cw*ee**2*FM1*complex(0,1)*gw**2*v)/sw)',
                  order = {'NP':1,'QED':3})

GC_103 = Coupling(name = 'GC_103',
                  value = 'ee**2*FM2*complex(0,1)*v**2',
                  order = {'NP':1})

GC_104 = Coupling(name = 'GC_104',
                  value = '-(ee**2*FM3*complex(0,1)*v**2)/4.',
                  order = {'NP':1})

GC_105 = Coupling(name = 'GC_105',
                  value = '-(ee**2*FM0*complex(0,1)*gw**2*v**2)/2.',
                  order = {'NP':1,'QED':2})

GC_106 = Coupling(name = 'GC_106',
                  value = '(ee**2*FM1*complex(0,1)*gw**2*v**2)/2.',
                  order = {'NP':1,'QED':2})

GC_107 = Coupling(name = 'GC_107',
                  value = '(ee**2*FM0*complex(0,1)*v**2)/(2.*sw**2)',
                  order = {'NP':1})

GC_108 = Coupling(name = 'GC_108',
                  value = '(cw**2*ee**2*FM0*complex(0,1)*v**2)/(2.*sw**2)',
                  order = {'NP':1})

GC_109 = Coupling(name = 'GC_109',
                  value = '-(ee**2*FM1*complex(0,1)*v**2)/(8.*sw**2)',
                  order = {'NP':1})

GC_110 = Coupling(name = 'GC_110',
                  value = '(cw**2*ee**2*FM1*complex(0,1)*v**2)/(8.*sw**2)',
                  order = {'NP':1})

GC_111 = Coupling(name = 'GC_111',
                  value = '-(cw*ee**2*FM0*complex(0,1)*gw*v**2)/(2.*sw**2)',
                  order = {'NP':1,'QED':1})

GC_112 = Coupling(name = 'GC_112',
                  value = '(cw*ee**2*FM1*complex(0,1)*gw*v**2)/(8.*sw**2)',
                  order = {'NP':1,'QED':1})

GC_113 = Coupling(name = 'GC_113',
                  value = '-(cw**2*ee**2*FM0*complex(0,1)*gw**2*v**2)/(2.*sw**2)',
                  order = {'NP':1,'QED':2})

GC_114 = Coupling(name = 'GC_114',
                  value = '(3*cw**2*ee**2*FM1*complex(0,1)*gw**2*v**2)/(4.*sw**2)',
                  order = {'NP':1,'QED':2})

GC_115 = Coupling(name = 'GC_115',
                  value = '-(ee**2*FM0*complex(0,1)*gw*v**2)/(2.*sw)',
                  order = {'NP':1,'QED':1})

GC_116 = Coupling(name = 'GC_116',
                  value = '-(ee**2*FM1*complex(0,1)*gw*v**2)/(8.*sw)',
                  order = {'NP':1,'QED':1})

GC_117 = Coupling(name = 'GC_117',
                  value = '(2*cw*ee**2*FM0*complex(0,1)*gw**2*v**2)/sw',
                  order = {'NP':1,'QED':2})

GC_118 = Coupling(name = 'GC_118',
                  value = '-(cw*ee**2*FM1*complex(0,1)*gw**2*v**2)/(2.*sw)',
                  order = {'NP':1,'QED':2})

GC_119 = Coupling(name = 'GC_119',
                  value = 'ee**2*FM0*complex(0,1)*v + (2*cw**2*ee**2*FM2*complex(0,1)*v)/sw**2',
                  order = {'NP':1,'QED':1})

GC_120 = Coupling(name = 'GC_120',
                  value = '(ee**2*FM1*complex(0,1)*v)/4. + (cw**2*ee**2*FM3*complex(0,1)*v)/(2.*sw**2)',
                  order = {'NP':1,'QED':1})

GC_121 = Coupling(name = 'GC_121',
                  value = '(3*ee**2*FM0*complex(0,1)*gw**2*v)/sw**2 - (3*ee**2*FM1*complex(0,1)*gw**2*v)/(2.*sw**2)',
                  order = {'NP':1,'QED':3})

GC_122 = Coupling(name = 'GC_122',
                  value = '(cw*ee**2*FM0*complex(0,1)*v)/sw - (2*cw*ee**2*FM2*complex(0,1)*v)/sw',
                  order = {'NP':1,'QED':1})

GC_123 = Coupling(name = 'GC_123',
                  value = '-(cw*ee**2*FM1*complex(0,1)*v)/(4.*sw) + (cw*ee**2*FM3*complex(0,1)*v)/(2.*sw)',
                  order = {'NP':1,'QED':1})

GC_124 = Coupling(name = 'GC_124',
                  value = 'ee**2*complex(0,1)*v + (cw**2*ee**2*complex(0,1)*v)/(2.*sw**2) + (ee**2*complex(0,1)*sw**2*v)/(2.*cw**2)',
                  order = {'QED':1})

GC_125 = Coupling(name = 'GC_125',
                  value = '2*ee**2*FM0*complex(0,1)*v + (ee**2*FM0*complex(0,1)*sw**2*v)/cw**2',
                  order = {'NP':1,'QED':1})

GC_126 = Coupling(name = 'GC_126',
                  value = '(ee**2*FM1*complex(0,1)*v)/2. + (ee**2*FM1*complex(0,1)*sw**2*v)/(4.*cw**2)',
                  order = {'NP':1,'QED':1})

GC_127 = Coupling(name = 'GC_127',
                  value = '-2*cw*ee**2*FM0*complex(0,1)*gw*v - (cw**3*ee**2*FM0*complex(0,1)*gw*v)/sw**2 - (ee**2*FM0*complex(0,1)*gw*sw**2*v)/cw',
                  order = {'NP':1,'QED':2})

GC_128 = Coupling(name = 'GC_128',
                  value = '-(cw*ee**2*FM1*complex(0,1)*gw*v)/2. - (cw**3*ee**2*FM1*complex(0,1)*gw*v)/(4.*sw**2) - (ee**2*FM1*complex(0,1)*gw*sw**2*v)/(4.*cw)',
                  order = {'NP':1,'QED':2})

GC_129 = Coupling(name = 'GC_129',
                  value = '2*ee**2*FM0*complex(0,1)*gw**2*v + (ee**2*FM0*complex(0,1)*gw**2*sw**2*v)/cw**2',
                  order = {'NP':1,'QED':3})

GC_130 = Coupling(name = 'GC_130',
                  value = '-2*cw**2*ee**2*FM0*complex(0,1)*gw**2*v + cw**2*ee**2*FM1*complex(0,1)*gw**2*v - (cw**4*ee**2*FM0*complex(0,1)*gw**2*v)/sw**2 + (cw**4*ee**2*FM1*complex(0,1)*gw**2*v)/(2.*sw**2) - ee**2*FM0*complex(0,1)*gw**2*sw**2*v + (ee**2*FM1*complex(0,1)*gw**2*sw**2*v)/2.',
                  order = {'NP':1,'QED':3})

GC_131 = Coupling(name = 'GC_131',
                  value = 'ee**2*FM1*complex(0,1)*gw**2*v + (ee**2*FM1*complex(0,1)*gw**2*sw**2*v)/(2.*cw**2)',
                  order = {'NP':1,'QED':3})

GC_132 = Coupling(name = 'GC_132',
                  value = '(cw**3*ee**2*FM0*complex(0,1)*v)/sw - (2*cw**3*ee**2*FM2*complex(0,1)*v)/sw + 2*cw*ee**2*FM0*complex(0,1)*sw*v - 4*cw*ee**2*FM2*complex(0,1)*sw*v + (ee**2*FM0*complex(0,1)*sw**3*v)/cw - (2*ee**2*FM2*complex(0,1)*sw**3*v)/cw',
                  order = {'NP':1,'QED':1})

GC_133 = Coupling(name = 'GC_133',
                  value = '(cw**3*ee**2*FM1*complex(0,1)*v)/(4.*sw) - (cw**3*ee**2*FM3*complex(0,1)*v)/(2.*sw) + (cw*ee**2*FM1*complex(0,1)*sw*v)/2. - cw*ee**2*FM3*complex(0,1)*sw*v + (ee**2*FM1*complex(0,1)*sw**3*v)/(4.*cw) - (ee**2*FM3*complex(0,1)*sw**3*v)/(2.*cw)',
                  order = {'NP':1,'QED':1})

GC_134 = Coupling(name = 'GC_134',
                  value = '-((cw**2*ee**2*FM0*complex(0,1)*gw*v)/sw) - 2*ee**2*FM0*complex(0,1)*gw*sw*v - (ee**2*FM0*complex(0,1)*gw*sw**3*v)/cw**2',
                  order = {'NP':1,'QED':2})

GC_135 = Coupling(name = 'GC_135',
                  value = '(cw**2*ee**2*FM1*complex(0,1)*gw*v)/(4.*sw) + (ee**2*FM1*complex(0,1)*gw*sw*v)/2. + (ee**2*FM1*complex(0,1)*gw*sw**3*v)/(4.*cw**2)',
                  order = {'NP':1,'QED':2})

GC_136 = Coupling(name = 'GC_136',
                  value = '(2*cw**3*ee**2*FM0*complex(0,1)*gw**2*v)/sw - (cw**3*ee**2*FM1*complex(0,1)*gw**2*v)/sw + 4*cw*ee**2*FM0*complex(0,1)*gw**2*sw*v - 2*cw*ee**2*FM1*complex(0,1)*gw**2*sw*v + (2*ee**2*FM0*complex(0,1)*gw**2*sw**3*v)/cw - (ee**2*FM1*complex(0,1)*gw**2*sw**3*v)/cw',
                  order = {'NP':1,'QED':3})

GC_137 = Coupling(name = 'GC_137',
                  value = 'cw**2*ee**2*FM0*complex(0,1)*v + 4*cw**2*ee**2*FM2*complex(0,1)*v + (2*cw**4*ee**2*FM2*complex(0,1)*v)/sw**2 + 2*ee**2*FM0*complex(0,1)*sw**2*v + 2*ee**2*FM2*complex(0,1)*sw**2*v + (ee**2*FM0*complex(0,1)*sw**4*v)/cw**2',
                  order = {'NP':1,'QED':1})

GC_138 = Coupling(name = 'GC_138',
                  value = '(cw**2*ee**2*FM1*complex(0,1)*v)/4. + cw**2*ee**2*FM3*complex(0,1)*v + (cw**4*ee**2*FM3*complex(0,1)*v)/(2.*sw**2) + (ee**2*FM1*complex(0,1)*sw**2*v)/2. + (ee**2*FM3*complex(0,1)*sw**2*v)/2. + (ee**2*FM1*complex(0,1)*sw**4*v)/(4.*cw**2)',
                  order = {'NP':1,'QED':1})

GC_139 = Coupling(name = 'GC_139',
                  value = '2*cw**2*ee**2*FM0*complex(0,1)*v + 2*cw**2*ee**2*FM2*complex(0,1)*v + (cw**4*ee**2*FM0*complex(0,1)*v)/sw**2 + ee**2*FM0*complex(0,1)*sw**2*v + 4*ee**2*FM2*complex(0,1)*sw**2*v + (2*ee**2*FM2*complex(0,1)*sw**4*v)/cw**2',
                  order = {'NP':1,'QED':1})

GC_140 = Coupling(name = 'GC_140',
                  value = '(cw**2*ee**2*FM1*complex(0,1)*v)/2. + (cw**2*ee**2*FM3*complex(0,1)*v)/2. + (cw**4*ee**2*FM1*complex(0,1)*v)/(4.*sw**2) + (ee**2*FM1*complex(0,1)*sw**2*v)/4. + ee**2*FM3*complex(0,1)*sw**2*v + (ee**2*FM3*complex(0,1)*sw**4*v)/(2.*cw**2)',
                  order = {'NP':1,'QED':1})

GC_141 = Coupling(name = 'GC_141',
                  value = '-(cw**2*ee**2*FM0*complex(0,1)*gw**2*v) - 2*ee**2*FM0*complex(0,1)*gw**2*sw**2*v - (ee**2*FM0*complex(0,1)*gw**2*sw**4*v)/cw**2',
                  order = {'NP':1,'QED':3})

GC_142 = Coupling(name = 'GC_142',
                  value = '-(cw**2*ee**2*FM1*complex(0,1)*gw**2*v)/2. - ee**2*FM1*complex(0,1)*gw**2*sw**2*v - (ee**2*FM1*complex(0,1)*gw**2*sw**4*v)/(2.*cw**2)',
                  order = {'NP':1,'QED':3})

GC_143 = Coupling(name = 'GC_143',
                  value = '(ee**2*FM0*complex(0,1)*v**2)/2. + (cw**2*ee**2*FM2*complex(0,1)*v**2)/sw**2',
                  order = {'NP':1})

GC_144 = Coupling(name = 'GC_144',
                  value = '(ee**2*FM1*complex(0,1)*v**2)/8. + (cw**2*ee**2*FM3*complex(0,1)*v**2)/(4.*sw**2)',
                  order = {'NP':1})

GC_145 = Coupling(name = 'GC_145',
                  value = '(3*ee**2*FM0*complex(0,1)*gw**2*v**2)/(2.*sw**2) - (3*ee**2*FM1*complex(0,1)*gw**2*v**2)/(4.*sw**2)',
                  order = {'NP':1,'QED':2})

GC_146 = Coupling(name = 'GC_146',
                  value = '(cw*ee**2*FM0*complex(0,1)*v**2)/(2.*sw) - (cw*ee**2*FM2*complex(0,1)*v**2)/sw',
                  order = {'NP':1})

GC_147 = Coupling(name = 'GC_147',
                  value = '-(cw*ee**2*FM1*complex(0,1)*v**2)/(8.*sw) + (cw*ee**2*FM3*complex(0,1)*v**2)/(4.*sw)',
                  order = {'NP':1})

GC_148 = Coupling(name = 'GC_148',
                  value = 'ee**2*FM0*complex(0,1)*v**2 + (ee**2*FM0*complex(0,1)*sw**2*v**2)/(2.*cw**2)',
                  order = {'NP':1})

GC_149 = Coupling(name = 'GC_149',
                  value = '(ee**2*FM1*complex(0,1)*v**2)/4. + (ee**2*FM1*complex(0,1)*sw**2*v**2)/(8.*cw**2)',
                  order = {'NP':1})

GC_150 = Coupling(name = 'GC_150',
                  value = '-(cw*ee**2*FM0*complex(0,1)*gw*v**2) - (cw**3*ee**2*FM0*complex(0,1)*gw*v**2)/(2.*sw**2) - (ee**2*FM0*complex(0,1)*gw*sw**2*v**2)/(2.*cw)',
                  order = {'NP':1,'QED':1})

GC_151 = Coupling(name = 'GC_151',
                  value = '-(cw*ee**2*FM1*complex(0,1)*gw*v**2)/4. - (cw**3*ee**2*FM1*complex(0,1)*gw*v**2)/(8.*sw**2) - (ee**2*FM1*complex(0,1)*gw*sw**2*v**2)/(8.*cw)',
                  order = {'NP':1,'QED':1})

GC_152 = Coupling(name = 'GC_152',
                  value = 'ee**2*FM0*complex(0,1)*gw**2*v**2 + (ee**2*FM0*complex(0,1)*gw**2*sw**2*v**2)/(2.*cw**2)',
                  order = {'NP':1,'QED':2})

GC_153 = Coupling(name = 'GC_153',
                  value = '-(cw**2*ee**2*FM0*complex(0,1)*gw**2*v**2) + (cw**2*ee**2*FM1*complex(0,1)*gw**2*v**2)/2. - (cw**4*ee**2*FM0*complex(0,1)*gw**2*v**2)/(2.*sw**2) + (cw**4*ee**2*FM1*complex(0,1)*gw**2*v**2)/(4.*sw**2) - (ee**2*FM0*complex(0,1)*gw**2*sw**2*v**2)/2. + (ee**2*FM1*complex(0,1)*gw**2*sw**2*v**2)/4.',
                  order = {'NP':1,'QED':2})

GC_154 = Coupling(name = 'GC_154',
                  value = '(ee**2*FM1*complex(0,1)*gw**2*v**2)/2. + (ee**2*FM1*complex(0,1)*gw**2*sw**2*v**2)/(4.*cw**2)',
                  order = {'NP':1,'QED':2})

GC_155 = Coupling(name = 'GC_155',
                  value = '(cw**3*ee**2*FM0*complex(0,1)*v**2)/(2.*sw) - (cw**3*ee**2*FM2*complex(0,1)*v**2)/sw + cw*ee**2*FM0*complex(0,1)*sw*v**2 - 2*cw*ee**2*FM2*complex(0,1)*sw*v**2 + (ee**2*FM0*complex(0,1)*sw**3*v**2)/(2.*cw) - (ee**2*FM2*complex(0,1)*sw**3*v**2)/cw',
                  order = {'NP':1})

GC_156 = Coupling(name = 'GC_156',
                  value = '(cw**3*ee**2*FM1*complex(0,1)*v**2)/(8.*sw) - (cw**3*ee**2*FM3*complex(0,1)*v**2)/(4.*sw) + (cw*ee**2*FM1*complex(0,1)*sw*v**2)/4. - (cw*ee**2*FM3*complex(0,1)*sw*v**2)/2. + (ee**2*FM1*complex(0,1)*sw**3*v**2)/(8.*cw) - (ee**2*FM3*complex(0,1)*sw**3*v**2)/(4.*cw)',
                  order = {'NP':1})

GC_157 = Coupling(name = 'GC_157',
                  value = '-(cw**2*ee**2*FM0*complex(0,1)*gw*v**2)/(2.*sw) - ee**2*FM0*complex(0,1)*gw*sw*v**2 - (ee**2*FM0*complex(0,1)*gw*sw**3*v**2)/(2.*cw**2)',
                  order = {'NP':1,'QED':1})

GC_158 = Coupling(name = 'GC_158',
                  value = '(cw**2*ee**2*FM1*complex(0,1)*gw*v**2)/(8.*sw) + (ee**2*FM1*complex(0,1)*gw*sw*v**2)/4. + (ee**2*FM1*complex(0,1)*gw*sw**3*v**2)/(8.*cw**2)',
                  order = {'NP':1,'QED':1})

GC_159 = Coupling(name = 'GC_159',
                  value = '(cw**3*ee**2*FM0*complex(0,1)*gw**2*v**2)/sw - (cw**3*ee**2*FM1*complex(0,1)*gw**2*v**2)/(2.*sw) + 2*cw*ee**2*FM0*complex(0,1)*gw**2*sw*v**2 - cw*ee**2*FM1*complex(0,1)*gw**2*sw*v**2 + (ee**2*FM0*complex(0,1)*gw**2*sw**3*v**2)/cw - (ee**2*FM1*complex(0,1)*gw**2*sw**3*v**2)/(2.*cw)',
                  order = {'NP':1,'QED':2})

GC_160 = Coupling(name = 'GC_160',
                  value = '(cw**2*ee**2*FM0*complex(0,1)*v**2)/2. + 2*cw**2*ee**2*FM2*complex(0,1)*v**2 + (cw**4*ee**2*FM2*complex(0,1)*v**2)/sw**2 + ee**2*FM0*complex(0,1)*sw**2*v**2 + ee**2*FM2*complex(0,1)*sw**2*v**2 + (ee**2*FM0*complex(0,1)*sw**4*v**2)/(2.*cw**2)',
                  order = {'NP':1})

GC_161 = Coupling(name = 'GC_161',
                  value = '(cw**2*ee**2*FM1*complex(0,1)*v**2)/8. + (cw**2*ee**2*FM3*complex(0,1)*v**2)/2. + (cw**4*ee**2*FM3*complex(0,1)*v**2)/(4.*sw**2) + (ee**2*FM1*complex(0,1)*sw**2*v**2)/4. + (ee**2*FM3*complex(0,1)*sw**2*v**2)/4. + (ee**2*FM1*complex(0,1)*sw**4*v**2)/(8.*cw**2)',
                  order = {'NP':1})

GC_162 = Coupling(name = 'GC_162',
                  value = 'cw**2*ee**2*FM0*complex(0,1)*v**2 + cw**2*ee**2*FM2*complex(0,1)*v**2 + (cw**4*ee**2*FM0*complex(0,1)*v**2)/(2.*sw**2) + (ee**2*FM0*complex(0,1)*sw**2*v**2)/2. + 2*ee**2*FM2*complex(0,1)*sw**2*v**2 + (ee**2*FM2*complex(0,1)*sw**4*v**2)/cw**2',
                  order = {'NP':1})

GC_163 = Coupling(name = 'GC_163',
                  value = '(cw**2*ee**2*FM1*complex(0,1)*v**2)/4. + (cw**2*ee**2*FM3*complex(0,1)*v**2)/4. + (cw**4*ee**2*FM1*complex(0,1)*v**2)/(8.*sw**2) + (ee**2*FM1*complex(0,1)*sw**2*v**2)/8. + (ee**2*FM3*complex(0,1)*sw**2*v**2)/2. + (ee**2*FM3*complex(0,1)*sw**4*v**2)/(4.*cw**2)',
                  order = {'NP':1})

GC_164 = Coupling(name = 'GC_164',
                  value = '-(cw**2*ee**2*FM0*complex(0,1)*gw**2*v**2)/2. - ee**2*FM0*complex(0,1)*gw**2*sw**2*v**2 - (ee**2*FM0*complex(0,1)*gw**2*sw**4*v**2)/(2.*cw**2)',
                  order = {'NP':1,'QED':2})

GC_165 = Coupling(name = 'GC_165',
                  value = '-(cw**2*ee**2*FM1*complex(0,1)*gw**2*v**2)/4. - (ee**2*FM1*complex(0,1)*gw**2*sw**2*v**2)/2. - (ee**2*FM1*complex(0,1)*gw**2*sw**4*v**2)/(4.*cw**2)',
                  order = {'NP':1,'QED':2})

GC_166 = Coupling(name = 'GC_166',
                  value = '-((complex(0,1)*yb)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_167 = Coupling(name = 'GC_167',
                  value = '-((complex(0,1)*yt)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_168 = Coupling(name = 'GC_168',
                  value = '-((complex(0,1)*ytau)/cmath.sqrt(2))',
                  order = {'QED':1})

GC_169 = Coupling(name = 'GC_169',
                  value = '(ee*complex(0,1)*complexconjugate(CKM11))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_170 = Coupling(name = 'GC_170',
                  value = '(ee*complex(0,1)*complexconjugate(CKM12))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_171 = Coupling(name = 'GC_171',
                  value = '(ee*complex(0,1)*complexconjugate(CKM21))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

GC_172 = Coupling(name = 'GC_172',
                  value = '(ee*complex(0,1)*complexconjugate(CKM22))/(sw*cmath.sqrt(2))',
                  order = {'QED':1})

