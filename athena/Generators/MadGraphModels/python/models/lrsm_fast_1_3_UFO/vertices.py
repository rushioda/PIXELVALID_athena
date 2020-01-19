# This file was automatically created by FeynRules 2.1
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Tue 2 Dec 2014 06:46:52


from object_library import all_vertices, Vertex
import particles as P
import couplings as C
import lorentz as L


V_1 = Vertex(name = 'V_1',
             particles = [ P.g, P.g, P.g ],
             color = [ 'f(1,2,3)' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):C.GC_1})

V_2 = Vertex(name = 'V_2',
             particles = [ P.g, P.g, P.g, P.g ],
             color = [ 'f(-1,1,2)*f(3,4,-1)', 'f(-1,1,3)*f(2,4,-1)', 'f(-1,1,4)*f(2,3,-1)' ],
             lorentz = [ L.VVVV1, L.VVVV3, L.VVVV4 ],
             couplings = {(1,1):C.GC_3,(0,0):C.GC_3,(2,2):C.GC_3})

V_3 = Vertex(name = 'V_3',
             particles = [ P.a, P.W__minus__, P.W2__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):[ C.GC_139, C.GC_141 ]})

V_4 = Vertex(name = 'V_4',
             particles = [ P.a, P.W__minus__, P.W__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):[ C.GC_19, C.GC_162 ]})

V_5 = Vertex(name = 'V_5',
             particles = [ P.a, P.W2__minus__, P.W2__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):[ C.GC_23, C.GC_161 ]})

V_6 = Vertex(name = 'V_6',
             particles = [ P.a, P.W2__minus__, P.W__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):[ C.GC_139, C.GC_141 ]})

V_7 = Vertex(name = 'V_7',
             particles = [ P.a, P.a, P.W__minus__, P.W2__plus__ ],
             color = [ '1' ],
             lorentz = [ L.VVVV2 ],
             couplings = {(0,0):[ C.GC_143, C.GC_145 ]})

V_8 = Vertex(name = 'V_8',
             particles = [ P.W__minus__, P.W2__plus__, P.Z ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):[ C.GC_135, C.GC_144, C.GC_151 ]})

V_9 = Vertex(name = 'V_9',
             particles = [ P.W__minus__, P.W2__plus__, P.Z2 ],
             color = [ '1' ],
             lorentz = [ L.VVV1 ],
             couplings = {(0,0):[ C.GC_184, C.GC_146 ]})

V_10 = Vertex(name = 'V_10',
              particles = [ P.a, P.a, P.W2__minus__, P.W2__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):C.GC_191})

V_11 = Vertex(name = 'V_11',
              particles = [ P.W2__minus__, P.W2__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):[ C.GC_68, C.GC_185 ]})

V_12 = Vertex(name = 'V_12',
              particles = [ P.W2__minus__, P.W2__plus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):[ C.GC_83, C.GC_91, C.GC_160 ]})

V_13 = Vertex(name = 'V_13',
              particles = [ P.W__minus__, P.W__minus__, P.W2__plus__, P.W2__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_158, C.GC_159 ]})

V_14 = Vertex(name = 'V_14',
              particles = [ P.W__minus__, P.W2__minus__, P.W2__plus__, P.W2__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):C.GC_195})

V_15 = Vertex(name = 'V_15',
              particles = [ P.W2__minus__, P.W2__minus__, P.W2__plus__, P.W2__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_8, C.GC_179 ]})

V_16 = Vertex(name = 'V_16',
              particles = [ P.a, P.a, P.W__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_65, C.GC_164 ]})

V_17 = Vertex(name = 'V_17',
              particles = [ P.W__minus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):[ C.GC_5, C.GC_163, C.GC_171 ]})

V_18 = Vertex(name = 'V_18',
              particles = [ P.W__minus__, P.W__plus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):[ C.GC_10, C.GC_165, C.GC_170 ]})

V_19 = Vertex(name = 'V_19',
              particles = [ P.a, P.a, P.W2__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_143, C.GC_145 ]})

V_20 = Vertex(name = 'V_20',
              particles = [ P.W2__minus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):[ C.GC_135, C.GC_144, C.GC_151 ]})

V_21 = Vertex(name = 'V_21',
              particles = [ P.W2__minus__, P.W__plus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVV1 ],
              couplings = {(0,0):[ C.GC_184, C.GC_146 ]})

V_22 = Vertex(name = 'V_22',
              particles = [ P.W__minus__, P.W__minus__, P.W2__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_137, C.GC_178 ]})

V_23 = Vertex(name = 'V_23',
              particles = [ P.W__minus__, P.W2__minus__, P.W2__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_158, C.GC_159 ]})

V_24 = Vertex(name = 'V_24',
              particles = [ P.W2__minus__, P.W2__minus__, P.W2__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):C.GC_195})

V_25 = Vertex(name = 'V_25',
              particles = [ P.W__minus__, P.W__minus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_7, C.GC_180 ]})

V_26 = Vertex(name = 'V_26',
              particles = [ P.W__minus__, P.W2__minus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_137, C.GC_178 ]})

V_27 = Vertex(name = 'V_27',
              particles = [ P.W2__minus__, P.W2__minus__, P.W__plus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_158, C.GC_159 ]})

V_28 = Vertex(name = 'V_28',
              particles = [ P.a, P.W__minus__, P.W2__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_140, C.GC_147, C.GC_153 ]})

V_29 = Vertex(name = 'V_29',
              particles = [ P.a, P.W2__minus__, P.W2__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_189, C.GC_103 ]})

V_30 = Vertex(name = 'V_30',
              particles = [ P.a, P.W__minus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_20, C.GC_166, C.GC_173 ]})

V_31 = Vertex(name = 'V_31',
              particles = [ P.a, P.W2__minus__, P.W__plus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_140, C.GC_147, C.GC_153 ]})

V_32 = Vertex(name = 'V_32',
              particles = [ P.W__minus__, P.W2__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_136, C.GC_182, C.GC_149, C.GC_155 ]})

V_33 = Vertex(name = 'V_33',
              particles = [ P.W2__minus__, P.W2__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_125, C.GC_186, C.GC_106 ]})

V_34 = Vertex(name = 'V_34',
              particles = [ P.W__minus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_6, C.GC_193, C.GC_168, C.GC_176 ]})

V_35 = Vertex(name = 'V_35',
              particles = [ P.W2__minus__, P.W__plus__, P.Z, P.Z ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_136, C.GC_182, C.GC_149, C.GC_155 ]})

V_36 = Vertex(name = 'V_36',
              particles = [ P.a, P.W__minus__, P.W2__plus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_142, C.GC_148, C.GC_152 ]})

V_37 = Vertex(name = 'V_37',
              particles = [ P.a, P.W2__minus__, P.W2__plus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_190, C.GC_101 ]})

V_38 = Vertex(name = 'V_38',
              particles = [ P.a, P.W__minus__, P.W__plus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_50, C.GC_167, C.GC_172 ]})

V_39 = Vertex(name = 'V_39',
              particles = [ P.a, P.W2__minus__, P.W__plus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV5 ],
              couplings = {(0,0):[ C.GC_142, C.GC_148, C.GC_152 ]})

V_40 = Vertex(name = 'V_40',
              particles = [ P.W__minus__, P.W2__plus__, P.Z, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_138, C.GC_183, C.GC_154, C.GC_157 ]})

V_41 = Vertex(name = 'V_41',
              particles = [ P.W2__minus__, P.W2__plus__, P.Z, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_187, C.GC_104, C.GC_107 ]})

V_42 = Vertex(name = 'V_42',
              particles = [ P.W__minus__, P.W__plus__, P.Z, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_11, C.GC_194, C.GC_174, C.GC_177 ]})

V_43 = Vertex(name = 'V_43',
              particles = [ P.W2__minus__, P.W__plus__, P.Z, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_138, C.GC_183, C.GC_154, C.GC_157 ]})

V_44 = Vertex(name = 'V_44',
              particles = [ P.W__minus__, P.W2__plus__, P.Z2, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_181, C.GC_150, C.GC_156 ]})

V_45 = Vertex(name = 'V_45',
              particles = [ P.W2__minus__, P.W2__plus__, P.Z2, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_110, C.GC_188, C.GC_105 ]})

V_46 = Vertex(name = 'V_46',
              particles = [ P.W__minus__, P.W__plus__, P.Z2, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_12, C.GC_192, C.GC_169, C.GC_175 ]})

V_47 = Vertex(name = 'V_47',
              particles = [ P.W2__minus__, P.W__plus__, P.Z2, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.VVVV2 ],
              couplings = {(0,0):[ C.GC_181, C.GC_150, C.GC_156 ]})

V_48 = Vertex(name = 'V_48',
              particles = [ P.e__plus__, P.v1, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_196})

V_49 = Vertex(name = 'V_49',
              particles = [ P.mu__plus__, P.v2, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_198})

V_50 = Vertex(name = 'V_50',
              particles = [ P.ta__plus__, P.v3, P.W__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_200})

V_51 = Vertex(name = 'V_51',
              particles = [ P.e__plus__, P.v1, P.W2__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_197})

V_52 = Vertex(name = 'V_52',
              particles = [ P.mu__plus__, P.v2, P.W2__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_199})

V_53 = Vertex(name = 'V_53',
              particles = [ P.ta__plus__, P.v3, P.W2__minus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_201})

V_54 = Vertex(name = 'V_54',
              particles = [ P.v1, P.v1, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_111, C.GC_51 ],(0,1):[ C.GC_114, C.GC_54 ]})

V_55 = Vertex(name = 'V_55',
              particles = [ P.v2, P.v2, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_112, C.GC_52 ],(0,1):[ C.GC_115, C.GC_55 ]})

V_56 = Vertex(name = 'V_56',
              particles = [ P.v3, P.v3, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_113, C.GC_53 ],(0,1):[ C.GC_116, C.GC_56 ]})

V_57 = Vertex(name = 'V_57',
              particles = [ P.v1, P.v1, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_119, C.GC_24 ],(0,1):[ C.GC_122, C.GC_27 ]})

V_58 = Vertex(name = 'V_58',
              particles = [ P.v2, P.v2, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_120, C.GC_25 ],(0,1):[ C.GC_123, C.GC_28 ]})

V_59 = Vertex(name = 'V_59',
              particles = [ P.v3, P.v3, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_121, C.GC_26 ],(0,1):[ C.GC_124, C.GC_29 ]})

V_60 = Vertex(name = 'V_60',
              particles = [ P.e__plus__, P.e__minus__, P.a ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_18,(0,1):[ C.GC_16, C.GC_21 ]})

V_61 = Vertex(name = 'V_61',
              particles = [ P.mu__plus__, P.mu__minus__, P.a ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_18,(0,1):[ C.GC_16, C.GC_21 ]})

V_62 = Vertex(name = 'V_62',
              particles = [ P.ta__plus__, P.ta__minus__, P.a ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_18,(0,1):[ C.GC_16, C.GC_21 ]})

V_63 = Vertex(name = 'V_63',
              particles = [ P.v1, P.e__minus__, P.W2__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_197})

V_64 = Vertex(name = 'V_64',
              particles = [ P.v2, P.mu__minus__, P.W2__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_199})

V_65 = Vertex(name = 'V_65',
              particles = [ P.v3, P.ta__minus__, P.W2__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_201})

V_66 = Vertex(name = 'V_66',
              particles = [ P.v1, P.e__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_196})

V_67 = Vertex(name = 'V_67',
              particles = [ P.v2, P.mu__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_198})

V_68 = Vertex(name = 'V_68',
              particles = [ P.v3, P.ta__minus__, P.W__plus__ ],
              color = [ '1' ],
              lorentz = [ L.FFV2 ],
              couplings = {(0,0):C.GC_200})

V_69 = Vertex(name = 'V_69',
              particles = [ P.e__plus__, P.e__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_109, C.GC_49 ],(0,1):[ C.GC_49, C.GC_64, C.GC_67, C.GC_93 ]})

V_70 = Vertex(name = 'V_70',
              particles = [ P.mu__plus__, P.mu__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_109, C.GC_49 ],(0,1):[ C.GC_49, C.GC_64, C.GC_67, C.GC_93 ]})

V_71 = Vertex(name = 'V_71',
              particles = [ P.ta__plus__, P.ta__minus__, P.Z ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_109, C.GC_49 ],(0,1):[ C.GC_49, C.GC_64, C.GC_67, C.GC_93 ]})

V_72 = Vertex(name = 'V_72',
              particles = [ P.e__plus__, P.e__minus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_118, C.GC_14 ],(0,1):[ C.GC_14, C.GC_127, C.GC_82 ]})

V_73 = Vertex(name = 'V_73',
              particles = [ P.mu__plus__, P.mu__minus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_118, C.GC_14 ],(0,1):[ C.GC_14, C.GC_127, C.GC_82 ]})

V_74 = Vertex(name = 'V_74',
              particles = [ P.ta__plus__, P.ta__minus__, P.Z2 ],
              color = [ '1' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_118, C.GC_14 ],(0,1):[ C.GC_14, C.GC_127, C.GC_82 ]})

V_75 = Vertex(name = 'V_75',
              particles = [ P.u__tilde__, P.u, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_17,(0,1):[ C.GC_15, C.GC_22 ]})

V_76 = Vertex(name = 'V_76',
              particles = [ P.c__tilde__, P.c, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_17,(0,1):[ C.GC_15, C.GC_22 ]})

V_77 = Vertex(name = 'V_77',
              particles = [ P.t__tilde__, P.t, P.a ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_17,(0,1):[ C.GC_15, C.GC_22 ]})

V_78 = Vertex(name = 'V_78',
              particles = [ P.u__tilde__, P.u, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_79 = Vertex(name = 'V_79',
              particles = [ P.c__tilde__, P.c, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_80 = Vertex(name = 'V_80',
              particles = [ P.t__tilde__, P.t, P.g ],
              color = [ 'T(3,2,1)' ],
              lorentz = [ L.FFV1 ],
              couplings = {(0,0):C.GC_2})

V_81 = Vertex(name = 'V_81',
              particles = [ P.d__tilde__, P.u, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_208,(0,1):C.GC_227})

V_82 = Vertex(name = 'V_82',
              particles = [ P.s__tilde__, P.u, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_210,(0,1):C.GC_229})

V_83 = Vertex(name = 'V_83',
              particles = [ P.b__tilde__, P.u, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_212,(0,1):C.GC_231})

V_84 = Vertex(name = 'V_84',
              particles = [ P.d__tilde__, P.c, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_214,(0,1):C.GC_233})

V_85 = Vertex(name = 'V_85',
              particles = [ P.s__tilde__, P.c, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_216,(0,1):C.GC_235})

V_86 = Vertex(name = 'V_86',
              particles = [ P.b__tilde__, P.c, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_218,(0,1):C.GC_237})

V_87 = Vertex(name = 'V_87',
              particles = [ P.d__tilde__, P.t, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_220,(0,1):C.GC_239})

V_88 = Vertex(name = 'V_88',
              particles = [ P.s__tilde__, P.t, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_222,(0,1):C.GC_241})

V_89 = Vertex(name = 'V_89',
              particles = [ P.b__tilde__, P.t, P.W__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_224,(0,1):C.GC_243})

V_90 = Vertex(name = 'V_90',
              particles = [ P.d__tilde__, P.u, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_209,(0,1):C.GC_226})

V_91 = Vertex(name = 'V_91',
              particles = [ P.s__tilde__, P.u, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_211,(0,1):C.GC_228})

V_92 = Vertex(name = 'V_92',
              particles = [ P.b__tilde__, P.u, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_213,(0,1):C.GC_230})

V_93 = Vertex(name = 'V_93',
              particles = [ P.d__tilde__, P.c, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_215,(0,1):C.GC_232})

V_94 = Vertex(name = 'V_94',
              particles = [ P.s__tilde__, P.c, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_217,(0,1):C.GC_234})

V_95 = Vertex(name = 'V_95',
              particles = [ P.b__tilde__, P.c, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_219,(0,1):C.GC_236})

V_96 = Vertex(name = 'V_96',
              particles = [ P.d__tilde__, P.t, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_221,(0,1):C.GC_238})

V_97 = Vertex(name = 'V_97',
              particles = [ P.s__tilde__, P.t, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_223,(0,1):C.GC_240})

V_98 = Vertex(name = 'V_98',
              particles = [ P.b__tilde__, P.t, P.W2__minus__ ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):C.GC_225,(0,1):C.GC_242})

V_99 = Vertex(name = 'V_99',
              particles = [ P.u__tilde__, P.u, P.Z ],
              color = [ 'Identity(1,2)' ],
              lorentz = [ L.FFV2, L.FFV3 ],
              couplings = {(0,0):[ C.GC_108, C.GC_48 ],(0,1):[ C.GC_48, C.GC_63, C.GC_66, C.GC_92 ]})

V_100 = Vertex(name = 'V_100',
               particles = [ P.c__tilde__, P.c, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_108, C.GC_48 ],(0,1):[ C.GC_48, C.GC_63, C.GC_66, C.GC_92 ]})

V_101 = Vertex(name = 'V_101',
               particles = [ P.t__tilde__, P.t, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_108, C.GC_48 ],(0,1):[ C.GC_48, C.GC_63, C.GC_66, C.GC_92 ]})

V_102 = Vertex(name = 'V_102',
               particles = [ P.u__tilde__, P.u, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_117, C.GC_13 ],(0,1):[ C.GC_13, C.GC_128, C.GC_81 ]})

V_103 = Vertex(name = 'V_103',
               particles = [ P.c__tilde__, P.c, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_117, C.GC_13 ],(0,1):[ C.GC_13, C.GC_128, C.GC_81 ]})

V_104 = Vertex(name = 'V_104',
               particles = [ P.t__tilde__, P.t, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_117, C.GC_13 ],(0,1):[ C.GC_13, C.GC_128, C.GC_81 ]})

V_105 = Vertex(name = 'V_105',
               particles = [ P.d__tilde__, P.d, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_16, C.GC_90 ],(0,1):[ C.GC_15, C.GC_21 ]})

V_106 = Vertex(name = 'V_106',
               particles = [ P.s__tilde__, P.s, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_16, C.GC_90 ],(0,1):[ C.GC_15, C.GC_21 ]})

V_107 = Vertex(name = 'V_107',
               particles = [ P.b__tilde__, P.b, P.a ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_16, C.GC_90 ],(0,1):[ C.GC_15, C.GC_21 ]})

V_108 = Vertex(name = 'V_108',
               particles = [ P.d__tilde__, P.d, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_109 = Vertex(name = 'V_109',
               particles = [ P.s__tilde__, P.s, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_110 = Vertex(name = 'V_110',
               particles = [ P.b__tilde__, P.b, P.g ],
               color = [ 'T(3,2,1)' ],
               lorentz = [ L.FFV1 ],
               couplings = {(0,0):C.GC_2})

V_111 = Vertex(name = 'V_111',
               particles = [ P.u__tilde__, P.d, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_245,(0,1):C.GC_262})

V_112 = Vertex(name = 'V_112',
               particles = [ P.c__tilde__, P.d, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_251,(0,1):C.GC_268})

V_113 = Vertex(name = 'V_113',
               particles = [ P.t__tilde__, P.d, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_257,(0,1):C.GC_274})

V_114 = Vertex(name = 'V_114',
               particles = [ P.u__tilde__, P.s, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_247,(0,1):C.GC_264})

V_115 = Vertex(name = 'V_115',
               particles = [ P.c__tilde__, P.s, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_253,(0,1):C.GC_270})

V_116 = Vertex(name = 'V_116',
               particles = [ P.t__tilde__, P.s, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_259,(0,1):C.GC_276})

V_117 = Vertex(name = 'V_117',
               particles = [ P.u__tilde__, P.b, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_249,(0,1):C.GC_266})

V_118 = Vertex(name = 'V_118',
               particles = [ P.c__tilde__, P.b, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_255,(0,1):C.GC_272})

V_119 = Vertex(name = 'V_119',
               particles = [ P.t__tilde__, P.b, P.W2__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_261,(0,1):C.GC_278})

V_120 = Vertex(name = 'V_120',
               particles = [ P.u__tilde__, P.d, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_244,(0,1):C.GC_263})

V_121 = Vertex(name = 'V_121',
               particles = [ P.c__tilde__, P.d, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_250,(0,1):C.GC_269})

V_122 = Vertex(name = 'V_122',
               particles = [ P.t__tilde__, P.d, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_256,(0,1):C.GC_275})

V_123 = Vertex(name = 'V_123',
               particles = [ P.u__tilde__, P.s, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_246,(0,1):C.GC_265})

V_124 = Vertex(name = 'V_124',
               particles = [ P.c__tilde__, P.s, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_252,(0,1):C.GC_271})

V_125 = Vertex(name = 'V_125',
               particles = [ P.t__tilde__, P.s, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_258,(0,1):C.GC_277})

V_126 = Vertex(name = 'V_126',
               particles = [ P.u__tilde__, P.b, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_248,(0,1):C.GC_267})

V_127 = Vertex(name = 'V_127',
               particles = [ P.c__tilde__, P.b, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_254,(0,1):C.GC_273})

V_128 = Vertex(name = 'V_128',
               particles = [ P.t__tilde__, P.b, P.W__plus__ ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):C.GC_260,(0,1):C.GC_279})

V_129 = Vertex(name = 'V_129',
               particles = [ P.d__tilde__, P.d, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_4, C.GC_48, C.GC_100 ],(0,1):[ C.GC_48, C.GC_63, C.GC_67, C.GC_93 ]})

V_130 = Vertex(name = 'V_130',
               particles = [ P.s__tilde__, P.s, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_4, C.GC_48, C.GC_100 ],(0,1):[ C.GC_48, C.GC_63, C.GC_67, C.GC_93 ]})

V_131 = Vertex(name = 'V_131',
               particles = [ P.b__tilde__, P.b, P.Z ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_4, C.GC_48, C.GC_100 ],(0,1):[ C.GC_48, C.GC_63, C.GC_67, C.GC_93 ]})

V_132 = Vertex(name = 'V_132',
               particles = [ P.d__tilde__, P.d, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_9, C.GC_13, C.GC_102 ],(0,1):[ C.GC_13, C.GC_126, C.GC_82 ]})

V_133 = Vertex(name = 'V_133',
               particles = [ P.s__tilde__, P.s, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_9, C.GC_13, C.GC_102 ],(0,1):[ C.GC_13, C.GC_126, C.GC_82 ]})

V_134 = Vertex(name = 'V_134',
               particles = [ P.b__tilde__, P.b, P.Z2 ],
               color = [ 'Identity(1,2)' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_9, C.GC_13, C.GC_102 ],(0,1):[ C.GC_13, C.GC_126, C.GC_82 ]})

V_135 = Vertex(name = 'V_135',
               particles = [ P.N1, P.N1, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_31, C.GC_32 ],(0,1):[ C.GC_40, C.GC_41 ]})

V_136 = Vertex(name = 'V_136',
               particles = [ P.N2, P.N2, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_34, C.GC_35 ],(0,1):[ C.GC_43, C.GC_44 ]})

V_137 = Vertex(name = 'V_137',
               particles = [ P.N3, P.N3, P.a ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_37, C.GC_38 ],(0,1):[ C.GC_46, C.GC_47 ]})

V_138 = Vertex(name = 'V_138',
               particles = [ P.N1, P.N1, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_57, C.GC_69, C.GC_70, C.GC_94 ],(0,1):[ C.GC_60, C.GC_75, C.GC_76, C.GC_97 ]})

V_139 = Vertex(name = 'V_139',
               particles = [ P.N2, P.N2, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_58, C.GC_71, C.GC_72, C.GC_95 ],(0,1):[ C.GC_61, C.GC_77, C.GC_78, C.GC_98 ]})

V_140 = Vertex(name = 'V_140',
               particles = [ P.N3, P.N3, P.Z ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_59, C.GC_73, C.GC_74, C.GC_96 ],(0,1):[ C.GC_62, C.GC_79, C.GC_80, C.GC_99 ]})

V_141 = Vertex(name = 'V_141',
               particles = [ P.N1, P.N1, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_30, C.GC_129, C.GC_84 ],(0,1):[ C.GC_39, C.GC_132, C.GC_87 ]})

V_142 = Vertex(name = 'V_142',
               particles = [ P.N2, P.N2, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_33, C.GC_130, C.GC_85 ],(0,1):[ C.GC_42, C.GC_133, C.GC_88 ]})

V_143 = Vertex(name = 'V_143',
               particles = [ P.N3, P.N3, P.Z2 ],
               color = [ '1' ],
               lorentz = [ L.FFV2, L.FFV3 ],
               couplings = {(0,0):[ C.GC_36, C.GC_131, C.GC_86 ],(0,1):[ C.GC_45, C.GC_134, C.GC_89 ]})

V_144 = Vertex(name = 'V_144',
               particles = [ P.e__plus__, P.N1, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_203})

V_145 = Vertex(name = 'V_145',
               particles = [ P.mu__plus__, P.N2, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_205})

V_146 = Vertex(name = 'V_146',
               particles = [ P.ta__plus__, P.N3, P.W__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_207})

V_147 = Vertex(name = 'V_147',
               particles = [ P.e__plus__, P.N1, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_202})

V_148 = Vertex(name = 'V_148',
               particles = [ P.mu__plus__, P.N2, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_204})

V_149 = Vertex(name = 'V_149',
               particles = [ P.ta__plus__, P.N3, P.W2__minus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_206})

V_150 = Vertex(name = 'V_150',
               particles = [ P.N1, P.e__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_202})

V_151 = Vertex(name = 'V_151',
               particles = [ P.N2, P.mu__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_204})

V_152 = Vertex(name = 'V_152',
               particles = [ P.N3, P.ta__minus__, P.W2__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_206})

V_153 = Vertex(name = 'V_153',
               particles = [ P.N1, P.e__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_203})

V_154 = Vertex(name = 'V_154',
               particles = [ P.N2, P.mu__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_205})

V_155 = Vertex(name = 'V_155',
               particles = [ P.N3, P.ta__minus__, P.W__plus__ ],
               color = [ '1' ],
               lorentz = [ L.FFV3 ],
               couplings = {(0,0):C.GC_207})

