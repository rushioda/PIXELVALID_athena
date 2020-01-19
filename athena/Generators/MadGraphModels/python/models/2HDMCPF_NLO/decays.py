# This file was automatically created by FeynRules 2.1.48
# Mathematica version: 8.0 for Mac OS X x86 (64-bit) (November 6, 2010)
# Date: Wed 2 Apr 2014 11:19:40


from object_library import all_decays, Decay
import particles as P


Decay_b = Decay(name = 'Decay_b',
                particle = P.b,
                partial_widths = {(P.h__minus__,P.t):'((3*I3a33*MB**2*complexconjugate(I3a33) - 3*I3a33*mhc**2*complexconjugate(I3a33) - 6*I4a33*MB*MT*complexconjugate(I3a33) + 3*I3a33*MT**2*complexconjugate(I3a33) + 3*I4a33*MB**2*complexconjugate(I4a33) - 3*I4a33*mhc**2*complexconjugate(I4a33) - 6*I3a33*MB*MT*complexconjugate(I4a33) + 3*I4a33*MT**2*complexconjugate(I4a33))*cmath.sqrt(MB**4 - 2*MB**2*mhc**2 + mhc**4 - 2*MB**2*MT**2 - 2*mhc**2*MT**2 + MT**4))/(96.*cmath.pi*abs(MB)**3)',
                                  (P.W__minus__,P.t):'(((3*gw**2*MB**2)/2. + (3*gw**2*MT**2)/2. + (3*gw**2*MB**4)/(2.*MW**2) - (3*gw**2*MB**2*MT**2)/MW**2 + (3*gw**2*MT**4)/(2.*MW**2) - 3*gw**2*MW**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MB)**3)'})

Decay_h1 = Decay(name = 'Decay_h1',
                 particle = P.h1,
                 partial_widths = {(P.a,P.a):'(mh1**2*(cw**4*g1**4*TH1x1**2*vev**2 - 4*cw**3*g1**3*gw*sw*TH1x1**2*vev**2 + 6*cw**2*g1**2*gw**2*sw**2*TH1x1**2*vev**2 - 4*cw*g1*gw**3*sw**3*TH1x1**2*vev**2 + gw**4*sw**4*TH1x1**2*vev**2))/(32.*cmath.pi*abs(mh1)**3)',
                                   (P.a,P.h3):'((mh1**2 - mh3**2)*(-(cw**2*g1**2*mh1**2*TH2x1**2*TH3x3**2)/2. - (cw**2*g1**2*mh3**2*TH2x1**2*TH3x3**2)/2. + cw*g1*gw*mh1**2*sw*TH2x1**2*TH3x3**2 + cw*g1*gw*mh3**2*sw*TH2x1**2*TH3x3**2 - (gw**2*mh1**2*sw**2*TH2x1**2*TH3x3**2)/2. - (gw**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/2.))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.a,P.Z):'((mh1**2 - MZ**2)*((3*cw**4*g1**2*gw**2*TH1x1**2*vev**2)/4. + (3*cw**3*g1**3*gw*sw*TH1x1**2*vev**2)/2. - (3*cw**3*g1*gw**3*sw*TH1x1**2*vev**2)/2. + (3*cw**2*g1**4*sw**2*TH1x1**2*vev**2)/4. - 3*cw**2*g1**2*gw**2*sw**2*TH1x1**2*vev**2 + (3*cw**2*gw**4*sw**2*TH1x1**2*vev**2)/4. - (3*cw*g1**3*gw*sw**3*TH1x1**2*vev**2)/2. + (3*cw*g1*gw**3*sw**3*TH1x1**2*vev**2)/2. + (3*g1**2*gw**2*sw**4*TH1x1**2*vev**2)/4.))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.b,P.b__tilde__):'((-3*GD3x3**2*MB**2*TH2x1**2 - 12*GD3x3*MB**2*TH1x1*TH2x1*yb + 3*GD3x3*mh1**2*TH1x1*TH2x1*yb - 12*MB**2*TH1x1**2*yb**2 + 3*mh1**2*TH1x1**2*yb**2 - 6*GD3x3*MB**2*TH2x1**2*complexconjugate(GD3x3) + 3*GD3x3*mh1**2*TH2x1**2*complexconjugate(GD3x3) - 12*MB**2*TH1x1*TH2x1*yb*complexconjugate(GD3x3) + 3*mh1**2*TH1x1*TH2x1*yb*complexconjugate(GD3x3) - 3*MB**2*TH2x1**2*complexconjugate(GD3x3)**2)*cmath.sqrt(-4*MB**2*mh1**2 + mh1**4))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.h2,P.h2):'((36*l1**2*TH1x1**2*TH1x2**4*vev**2 + 18*l1*l6*TH1x1*TH1x2**4*TH2x1*vev**2 + 36*l1*l6*TH1x1**2*TH1x2**3*TH2x2*vev**2 + 24*l1*l3*TH1x1*TH1x2**3*TH2x1*TH2x2*vev**2 + 24*l1*l4*TH1x1*TH1x2**3*TH2x1*TH2x2*vev**2 + 24*l1*l5*TH1x1*TH1x2**3*TH2x1*TH2x2*vev**2 + 6*l3*l6*TH1x2**3*TH2x1**2*TH2x2*vev**2 + 6*l4*l6*TH1x2**3*TH2x1**2*TH2x2*vev**2 + 12*l1*l3*TH1x1**2*TH1x2**2*TH2x2**2*vev**2 + 12*l1*l4*TH1x1**2*TH1x2**2*TH2x2**2*vev**2 + 12*l1*l5*TH1x1**2*TH1x2**2*TH2x2**2*vev**2 + 15*l3*l6*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2 + 15*l4*l6*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2 + 18*l1*l7*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2 + 4*l3**2*TH1x2**2*TH2x1**2*TH2x2**2*vev**2 + 8*l3*l4*TH1x2**2*TH2x1**2*TH2x2**2*vev**2 + 4*l4**2*TH1x2**2*TH2x1**2*TH2x2**2*vev**2 + 8*l3*l5*TH1x2**2*TH2x1**2*TH2x2**2*vev**2 + 8*l4*l5*TH1x2**2*TH2x1**2*TH2x2**2*vev**2 + 6*l3*l6*TH1x1**2*TH1x2*TH2x2**3*vev**2 + 6*l4*l6*TH1x1**2*TH1x2*TH2x2**3*vev**2 + 4*l3**2*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2 + 8*l3*l4*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2 + 4*l4**2*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2 + 8*l3*l5*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2 + 8*l4*l5*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2 + 6*l3*l7*TH1x2*TH2x1**2*TH2x2**3*vev**2 + 6*l4*l7*TH1x2*TH2x1**2*TH2x2**3*vev**2 + l3**2*TH1x1**2*TH2x2**4*vev**2 + 2*l3*l4*TH1x1**2*TH2x2**4*vev**2 + l4**2*TH1x1**2*TH2x2**4*vev**2 + 2*l3*l5*TH1x1**2*TH2x2**4*vev**2 + 2*l4*l5*TH1x1**2*TH2x2**4*vev**2 + 3*l3*l7*TH1x1*TH2x1*TH2x2**4*vev**2 + 3*l4*l7*TH1x1*TH2x1*TH2x2**4*vev**2 + 24*l1*TH1x1*TH1x2**3*TH2x1*TH2x2*vev**2*complexconjugate(l5) + 12*l6*TH1x2**3*TH2x1**2*TH2x2*vev**2*complexconjugate(l5) + 12*l1*TH1x1**2*TH1x2**2*TH2x2**2*vev**2*complexconjugate(l5) + 30*l6*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2*complexconjugate(l5) + 8*l3*TH1x2**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l5) + 8*l4*TH1x2**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l5) + 16*l5*TH1x2**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l5) + 12*l6*TH1x1**2*TH1x2*TH2x2**3*vev**2*complexconjugate(l5) + 8*l3*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2*complexconjugate(l5) + 8*l4*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2*complexconjugate(l5) + 16*l5*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2*complexconjugate(l5) + 12*l7*TH1x2*TH2x1**2*TH2x2**3*vev**2*complexconjugate(l5) + 2*l3*TH1x1**2*TH2x2**4*vev**2*complexconjugate(l5) + 2*l4*TH1x1**2*TH2x2**4*vev**2*complexconjugate(l5) + 4*l5*TH1x1**2*TH2x2**4*vev**2*complexconjugate(l5) + 6*l7*TH1x1*TH2x1*TH2x2**4*vev**2*complexconjugate(l5) + 18*l1*TH1x1*TH1x2**4*TH2x1*vev**2*complexconjugate(l6) + 9*l6*TH1x2**4*TH2x1**2*vev**2*complexconjugate(l6) + 36*l1*TH1x1**2*TH1x2**3*TH2x2*vev**2*complexconjugate(l6) + 36*l6*TH1x1*TH1x2**3*TH2x1*TH2x2*vev**2*complexconjugate(l6) + 6*l3*TH1x2**3*TH2x1**2*TH2x2*vev**2*complexconjugate(l6) + 6*l4*TH1x2**3*TH2x1**2*TH2x2*vev**2*complexconjugate(l6) + 12*l5*TH1x2**3*TH2x1**2*TH2x2*vev**2*complexconjugate(l6) + 36*l6*TH1x1**2*TH1x2**2*TH2x2**2*vev**2*complexconjugate(l6) + 15*l3*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2*complexconjugate(l6) + 15*l4*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2*complexconjugate(l6) + 30*l5*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2*complexconjugate(l6) + 9*l7*TH1x2**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l6) + 6*l3*TH1x1**2*TH1x2*TH2x2**3*vev**2*complexconjugate(l6) + 6*l4*TH1x1**2*TH1x2*TH2x2**3*vev**2*complexconjugate(l6) + 12*l5*TH1x1**2*TH1x2*TH2x2**3*vev**2*complexconjugate(l6) + 18*l7*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2*complexconjugate(l6) + 18*l1*TH1x1*TH1x2**2*TH2x1*TH2x2**2*vev**2*complexconjugate(l7) + 9*l6*TH1x2**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l7) + 18*l6*TH1x1*TH1x2*TH2x1*TH2x2**3*vev**2*complexconjugate(l7) + 6*l3*TH1x2*TH2x1**2*TH2x2**3*vev**2*complexconjugate(l7) + 6*l4*TH1x2*TH2x1**2*TH2x2**3*vev**2*complexconjugate(l7) + 12*l5*TH1x2*TH2x1**2*TH2x2**3*vev**2*complexconjugate(l7) + 3*l3*TH1x1*TH2x1*TH2x2**4*vev**2*complexconjugate(l7) + 3*l4*TH1x1*TH2x1*TH2x2**4*vev**2*complexconjugate(l7) + 6*l5*TH1x1*TH2x1*TH2x2**4*vev**2*complexconjugate(l7) + 9*l7*TH2x1**2*TH2x2**4*vev**2*complexconjugate(l7))*cmath.sqrt(mh1**4 - 4*mh1**2*mh2**2))/(32.*cmath.pi*abs(mh1)**3)',
                                   (P.h3,P.h3):'((l3**2*TH1x1**2*TH3x3**4*vev**2 + 2*l3*l4*TH1x1**2*TH3x3**4*vev**2 + l4**2*TH1x1**2*TH3x3**4*vev**2 - 2*l3*l5*TH1x1**2*TH3x3**4*vev**2 - 2*l4*l5*TH1x1**2*TH3x3**4*vev**2 + l3*l7*TH1x1*TH2x1*TH3x3**4*vev**2 + l4*l7*TH1x1*TH2x1*TH3x3**4*vev**2 - 2*l3*TH1x1**2*TH3x3**4*vev**2*complexconjugate(l5) - 2*l4*TH1x1**2*TH3x3**4*vev**2*complexconjugate(l5) + 4*l5*TH1x1**2*TH3x3**4*vev**2*complexconjugate(l5) - 2*l7*TH1x1*TH2x1*TH3x3**4*vev**2*complexconjugate(l5) + l3*TH1x1*TH2x1*TH3x3**4*vev**2*complexconjugate(l7) + l4*TH1x1*TH2x1*TH3x3**4*vev**2*complexconjugate(l7) - 2*l5*TH1x1*TH2x1*TH3x3**4*vev**2*complexconjugate(l7) + l7*TH2x1**2*TH3x3**4*vev**2*complexconjugate(l7))*cmath.sqrt(mh1**4 - 4*mh1**2*mh3**2))/(32.*cmath.pi*abs(mh1)**3)',
                                   (P.h3,P.Z):'((-(cw**2*gw**2*mh1**2*TH2x1**2*TH3x3**2)/2. - (cw**2*gw**2*mh3**2*TH2x1**2*TH3x3**2)/2. + (cw**2*gw**2*mh1**4*TH2x1**2*TH3x3**2)/(4.*MZ**2) - (cw**2*gw**2*mh1**2*mh3**2*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (cw**2*gw**2*mh3**4*TH2x1**2*TH3x3**2)/(4.*MZ**2) + (cw**2*gw**2*MZ**2*TH2x1**2*TH3x3**2)/4. - cw*g1*gw*mh1**2*sw*TH2x1**2*TH3x3**2 - cw*g1*gw*mh3**2*sw*TH2x1**2*TH3x3**2 + (cw*g1*gw*mh1**4*sw*TH2x1**2*TH3x3**2)/(2.*MZ**2) - (cw*g1*gw*mh1**2*mh3**2*sw*TH2x1**2*TH3x3**2)/MZ**2 + (cw*g1*gw*mh3**4*sw*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (cw*g1*gw*MZ**2*sw*TH2x1**2*TH3x3**2)/2. - (g1**2*mh1**2*sw**2*TH2x1**2*TH3x3**2)/2. - (g1**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/2. + (g1**2*mh1**4*sw**2*TH2x1**2*TH3x3**2)/(4.*MZ**2) - (g1**2*mh1**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (g1**2*mh3**4*sw**2*TH2x1**2*TH3x3**2)/(4.*MZ**2) + (g1**2*MZ**2*sw**2*TH2x1**2*TH3x3**2)/4.)*cmath.sqrt(mh1**4 - 2*mh1**2*mh3**2 + mh3**4 - 2*mh1**2*MZ**2 - 2*mh3**2*MZ**2 + MZ**4))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.h__plus__,P.W__minus__):'((-(gw**2*mh1**2*TH2x1**2)/2. - (gw**2*mhc**2*TH2x1**2)/2. + (gw**2*mh1**4*TH2x1**2)/(4.*MW**2) - (gw**2*mh1**2*mhc**2*TH2x1**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x1**2)/(4.*MW**2) + (gw**2*MW**2*TH2x1**2)/4.)*cmath.sqrt(mh1**4 - 2*mh1**2*mhc**2 + mhc**4 - 2*mh1**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.h__minus__,P.h__plus__):'((l3**2*TH1x1**2*vev**2 + l3*l7*TH1x1*TH2x1*vev**2 + l3*TH1x1*TH2x1*vev**2*complexconjugate(l7) + l7*TH2x1**2*vev**2*complexconjugate(l7))*cmath.sqrt(mh1**4 - 4*mh1**2*mhc**2))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.h__minus__,P.W__plus__):'((-(gw**2*mh1**2*TH2x1**2)/2. - (gw**2*mhc**2*TH2x1**2)/2. + (gw**2*mh1**4*TH2x1**2)/(4.*MW**2) - (gw**2*mh1**2*mhc**2*TH2x1**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x1**2)/(4.*MW**2) + (gw**2*MW**2*TH2x1**2)/4.)*cmath.sqrt(mh1**4 - 2*mh1**2*mhc**2 + mhc**4 - 2*mh1**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.t,P.t__tilde__):'((-3*GU3x3**2*MT**2*TH2x1**2 + 3*GU3x3*mh1**2*TH1x1*TH2x1*yt - 12*GU3x3*MT**2*TH1x1*TH2x1*yt + 3*mh1**2*TH1x1**2*yt**2 - 12*MT**2*TH1x1**2*yt**2 + 3*GU3x3*mh1**2*TH2x1**2*complexconjugate(GU3x3) - 6*GU3x3*MT**2*TH2x1**2*complexconjugate(GU3x3) + 3*mh1**2*TH1x1*TH2x1*yt*complexconjugate(GU3x3) - 12*MT**2*TH1x1*TH2x1*yt*complexconjugate(GU3x3) - 3*MT**2*TH2x1**2*complexconjugate(GU3x3)**2)*cmath.sqrt(mh1**4 - 4*mh1**2*MT**2))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.ta__minus__,P.ta__plus__):'(mh1**2*(GL3x3*mh1**2*TH1x1*TH2x1*ytau + mh1**2*TH1x1**2*ytau**2 + GL3x3*mh1**2*TH2x1**2*complexconjugate(GL3x3) + mh1**2*TH1x1*TH2x1*ytau*complexconjugate(GL3x3)))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.W__minus__,P.W__plus__):'(((3*gw**4*TH1x1**2*vev**2)/4. + (gw**4*mh1**4*TH1x1**2*vev**2)/(16.*MW**4) - (gw**4*mh1**2*TH1x1**2*vev**2)/(4.*MW**2))*cmath.sqrt(mh1**4 - 4*mh1**2*MW**2))/(16.*cmath.pi*abs(mh1)**3)',
                                   (P.Z,P.Z):'(((3*cw**4*gw**4*TH1x1**2*vev**2)/4. + (cw**4*gw**4*mh1**4*TH1x1**2*vev**2)/(16.*MZ**4) - (cw**4*gw**4*mh1**2*TH1x1**2*vev**2)/(4.*MZ**2) + 3*cw**3*g1*gw**3*sw*TH1x1**2*vev**2 + (cw**3*g1*gw**3*mh1**4*sw*TH1x1**2*vev**2)/(4.*MZ**4) - (cw**3*g1*gw**3*mh1**2*sw*TH1x1**2*vev**2)/MZ**2 + (9*cw**2*g1**2*gw**2*sw**2*TH1x1**2*vev**2)/2. + (3*cw**2*g1**2*gw**2*mh1**4*sw**2*TH1x1**2*vev**2)/(8.*MZ**4) - (3*cw**2*g1**2*gw**2*mh1**2*sw**2*TH1x1**2*vev**2)/(2.*MZ**2) + 3*cw*g1**3*gw*sw**3*TH1x1**2*vev**2 + (cw*g1**3*gw*mh1**4*sw**3*TH1x1**2*vev**2)/(4.*MZ**4) - (cw*g1**3*gw*mh1**2*sw**3*TH1x1**2*vev**2)/MZ**2 + (3*g1**4*sw**4*TH1x1**2*vev**2)/4. + (g1**4*mh1**4*sw**4*TH1x1**2*vev**2)/(16.*MZ**4) - (g1**4*mh1**2*sw**4*TH1x1**2*vev**2)/(4.*MZ**2))*cmath.sqrt(mh1**4 - 4*mh1**2*MZ**2))/(32.*cmath.pi*abs(mh1)**3)'})

Decay_h2 = Decay(name = 'Decay_h2',
                 particle = P.h2,
                 partial_widths = {(P.a,P.a):'(mh2**2*(cw**4*g1**4*TH1x2**2*vev**2 - 4*cw**3*g1**3*gw*sw*TH1x2**2*vev**2 + 6*cw**2*g1**2*gw**2*sw**2*TH1x2**2*vev**2 - 4*cw*g1*gw**3*sw**3*TH1x2**2*vev**2 + gw**4*sw**4*TH1x2**2*vev**2))/(32.*cmath.pi*abs(mh2)**3)',
                                   (P.a,P.h3):'((mh2**2 - mh3**2)*(-(cw**2*g1**2*mh2**2*TH2x2**2*TH3x3**2)/2. - (cw**2*g1**2*mh3**2*TH2x2**2*TH3x3**2)/2. + cw*g1*gw*mh2**2*sw*TH2x2**2*TH3x3**2 + cw*g1*gw*mh3**2*sw*TH2x2**2*TH3x3**2 - (gw**2*mh2**2*sw**2*TH2x2**2*TH3x3**2)/2. - (gw**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/2.))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.a,P.Z):'((mh2**2 - MZ**2)*((3*cw**4*g1**2*gw**2*TH1x2**2*vev**2)/4. + (3*cw**3*g1**3*gw*sw*TH1x2**2*vev**2)/2. - (3*cw**3*g1*gw**3*sw*TH1x2**2*vev**2)/2. + (3*cw**2*g1**4*sw**2*TH1x2**2*vev**2)/4. - 3*cw**2*g1**2*gw**2*sw**2*TH1x2**2*vev**2 + (3*cw**2*gw**4*sw**2*TH1x2**2*vev**2)/4. - (3*cw*g1**3*gw*sw**3*TH1x2**2*vev**2)/2. + (3*cw*g1*gw**3*sw**3*TH1x2**2*vev**2)/2. + (3*g1**2*gw**2*sw**4*TH1x2**2*vev**2)/4.))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.b,P.b__tilde__):'((-3*GD3x3**2*MB**2*TH2x2**2 - 12*GD3x3*MB**2*TH1x2*TH2x2*yb + 3*GD3x3*mh2**2*TH1x2*TH2x2*yb - 12*MB**2*TH1x2**2*yb**2 + 3*mh2**2*TH1x2**2*yb**2 - 6*GD3x3*MB**2*TH2x2**2*complexconjugate(GD3x3) + 3*GD3x3*mh2**2*TH2x2**2*complexconjugate(GD3x3) - 12*MB**2*TH1x2*TH2x2*yb*complexconjugate(GD3x3) + 3*mh2**2*TH1x2*TH2x2*yb*complexconjugate(GD3x3) - 3*MB**2*TH2x2**2*complexconjugate(GD3x3)**2)*cmath.sqrt(-4*MB**2*mh2**2 + mh2**4))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.h1,P.h1):'((36*l1**2*TH1x1**4*TH1x2**2*vev**2 + 36*l1*l6*TH1x1**3*TH1x2**2*TH2x1*vev**2 + 12*l1*l3*TH1x1**2*TH1x2**2*TH2x1**2*vev**2 + 12*l1*l4*TH1x1**2*TH1x2**2*TH2x1**2*vev**2 + 12*l1*l5*TH1x1**2*TH1x2**2*TH2x1**2*vev**2 + 6*l3*l6*TH1x1*TH1x2**2*TH2x1**3*vev**2 + 6*l4*l6*TH1x1*TH1x2**2*TH2x1**3*vev**2 + l3**2*TH1x2**2*TH2x1**4*vev**2 + 2*l3*l4*TH1x2**2*TH2x1**4*vev**2 + l4**2*TH1x2**2*TH2x1**4*vev**2 + 2*l3*l5*TH1x2**2*TH2x1**4*vev**2 + 2*l4*l5*TH1x2**2*TH2x1**4*vev**2 + 18*l1*l6*TH1x1**4*TH1x2*TH2x2*vev**2 + 24*l1*l3*TH1x1**3*TH1x2*TH2x1*TH2x2*vev**2 + 24*l1*l4*TH1x1**3*TH1x2*TH2x1*TH2x2*vev**2 + 24*l1*l5*TH1x1**3*TH1x2*TH2x1*TH2x2*vev**2 + 15*l3*l6*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2 + 15*l4*l6*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2 + 18*l1*l7*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2 + 4*l3**2*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2 + 8*l3*l4*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2 + 4*l4**2*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2 + 8*l3*l5*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2 + 8*l4*l5*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2 + 3*l3*l7*TH1x2*TH2x1**4*TH2x2*vev**2 + 3*l4*l7*TH1x2*TH2x1**4*TH2x2*vev**2 + 6*l3*l6*TH1x1**3*TH2x1*TH2x2**2*vev**2 + 6*l4*l6*TH1x1**3*TH2x1*TH2x2**2*vev**2 + 4*l3**2*TH1x1**2*TH2x1**2*TH2x2**2*vev**2 + 8*l3*l4*TH1x1**2*TH2x1**2*TH2x2**2*vev**2 + 4*l4**2*TH1x1**2*TH2x1**2*TH2x2**2*vev**2 + 8*l3*l5*TH1x1**2*TH2x1**2*TH2x2**2*vev**2 + 8*l4*l5*TH1x1**2*TH2x1**2*TH2x2**2*vev**2 + 6*l3*l7*TH1x1*TH2x1**3*TH2x2**2*vev**2 + 6*l4*l7*TH1x1*TH2x1**3*TH2x2**2*vev**2 + 12*l1*TH1x1**2*TH1x2**2*TH2x1**2*vev**2*complexconjugate(l5) + 12*l6*TH1x1*TH1x2**2*TH2x1**3*vev**2*complexconjugate(l5) + 2*l3*TH1x2**2*TH2x1**4*vev**2*complexconjugate(l5) + 2*l4*TH1x2**2*TH2x1**4*vev**2*complexconjugate(l5) + 4*l5*TH1x2**2*TH2x1**4*vev**2*complexconjugate(l5) + 24*l1*TH1x1**3*TH1x2*TH2x1*TH2x2*vev**2*complexconjugate(l5) + 30*l6*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2*complexconjugate(l5) + 8*l3*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2*complexconjugate(l5) + 8*l4*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2*complexconjugate(l5) + 16*l5*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2*complexconjugate(l5) + 6*l7*TH1x2*TH2x1**4*TH2x2*vev**2*complexconjugate(l5) + 12*l6*TH1x1**3*TH2x1*TH2x2**2*vev**2*complexconjugate(l5) + 8*l3*TH1x1**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l5) + 8*l4*TH1x1**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l5) + 16*l5*TH1x1**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l5) + 12*l7*TH1x1*TH2x1**3*TH2x2**2*vev**2*complexconjugate(l5) + 36*l1*TH1x1**3*TH1x2**2*TH2x1*vev**2*complexconjugate(l6) + 36*l6*TH1x1**2*TH1x2**2*TH2x1**2*vev**2*complexconjugate(l6) + 6*l3*TH1x1*TH1x2**2*TH2x1**3*vev**2*complexconjugate(l6) + 6*l4*TH1x1*TH1x2**2*TH2x1**3*vev**2*complexconjugate(l6) + 12*l5*TH1x1*TH1x2**2*TH2x1**3*vev**2*complexconjugate(l6) + 18*l1*TH1x1**4*TH1x2*TH2x2*vev**2*complexconjugate(l6) + 36*l6*TH1x1**3*TH1x2*TH2x1*TH2x2*vev**2*complexconjugate(l6) + 15*l3*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2*complexconjugate(l6) + 15*l4*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2*complexconjugate(l6) + 30*l5*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2*complexconjugate(l6) + 18*l7*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2*complexconjugate(l6) + 9*l6*TH1x1**4*TH2x2**2*vev**2*complexconjugate(l6) + 6*l3*TH1x1**3*TH2x1*TH2x2**2*vev**2*complexconjugate(l6) + 6*l4*TH1x1**3*TH2x1*TH2x2**2*vev**2*complexconjugate(l6) + 12*l5*TH1x1**3*TH2x1*TH2x2**2*vev**2*complexconjugate(l6) + 9*l7*TH1x1**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l6) + 18*l1*TH1x1**2*TH1x2*TH2x1**2*TH2x2*vev**2*complexconjugate(l7) + 18*l6*TH1x1*TH1x2*TH2x1**3*TH2x2*vev**2*complexconjugate(l7) + 3*l3*TH1x2*TH2x1**4*TH2x2*vev**2*complexconjugate(l7) + 3*l4*TH1x2*TH2x1**4*TH2x2*vev**2*complexconjugate(l7) + 6*l5*TH1x2*TH2x1**4*TH2x2*vev**2*complexconjugate(l7) + 9*l6*TH1x1**2*TH2x1**2*TH2x2**2*vev**2*complexconjugate(l7) + 6*l3*TH1x1*TH2x1**3*TH2x2**2*vev**2*complexconjugate(l7) + 6*l4*TH1x1*TH2x1**3*TH2x2**2*vev**2*complexconjugate(l7) + 12*l5*TH1x1*TH2x1**3*TH2x2**2*vev**2*complexconjugate(l7) + 9*l7*TH2x1**4*TH2x2**2*vev**2*complexconjugate(l7))*cmath.sqrt(-4*mh1**2*mh2**2 + mh2**4))/(32.*cmath.pi*abs(mh2)**3)',
                                   (P.h3,P.h3):'((l3**2*TH1x2**2*TH3x3**4*vev**2 + 2*l3*l4*TH1x2**2*TH3x3**4*vev**2 + l4**2*TH1x2**2*TH3x3**4*vev**2 - 2*l3*l5*TH1x2**2*TH3x3**4*vev**2 - 2*l4*l5*TH1x2**2*TH3x3**4*vev**2 + l3*l7*TH1x2*TH2x2*TH3x3**4*vev**2 + l4*l7*TH1x2*TH2x2*TH3x3**4*vev**2 - 2*l3*TH1x2**2*TH3x3**4*vev**2*complexconjugate(l5) - 2*l4*TH1x2**2*TH3x3**4*vev**2*complexconjugate(l5) + 4*l5*TH1x2**2*TH3x3**4*vev**2*complexconjugate(l5) - 2*l7*TH1x2*TH2x2*TH3x3**4*vev**2*complexconjugate(l5) + l3*TH1x2*TH2x2*TH3x3**4*vev**2*complexconjugate(l7) + l4*TH1x2*TH2x2*TH3x3**4*vev**2*complexconjugate(l7) - 2*l5*TH1x2*TH2x2*TH3x3**4*vev**2*complexconjugate(l7) + l7*TH2x2**2*TH3x3**4*vev**2*complexconjugate(l7))*cmath.sqrt(mh2**4 - 4*mh2**2*mh3**2))/(32.*cmath.pi*abs(mh2)**3)',
                                   (P.h3,P.Z):'((-(cw**2*gw**2*mh2**2*TH2x2**2*TH3x3**2)/2. - (cw**2*gw**2*mh3**2*TH2x2**2*TH3x3**2)/2. + (cw**2*gw**2*mh2**4*TH2x2**2*TH3x3**2)/(4.*MZ**2) - (cw**2*gw**2*mh2**2*mh3**2*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (cw**2*gw**2*mh3**4*TH2x2**2*TH3x3**2)/(4.*MZ**2) + (cw**2*gw**2*MZ**2*TH2x2**2*TH3x3**2)/4. - cw*g1*gw*mh2**2*sw*TH2x2**2*TH3x3**2 - cw*g1*gw*mh3**2*sw*TH2x2**2*TH3x3**2 + (cw*g1*gw*mh2**4*sw*TH2x2**2*TH3x3**2)/(2.*MZ**2) - (cw*g1*gw*mh2**2*mh3**2*sw*TH2x2**2*TH3x3**2)/MZ**2 + (cw*g1*gw*mh3**4*sw*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (cw*g1*gw*MZ**2*sw*TH2x2**2*TH3x3**2)/2. - (g1**2*mh2**2*sw**2*TH2x2**2*TH3x3**2)/2. - (g1**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/2. + (g1**2*mh2**4*sw**2*TH2x2**2*TH3x3**2)/(4.*MZ**2) - (g1**2*mh2**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (g1**2*mh3**4*sw**2*TH2x2**2*TH3x3**2)/(4.*MZ**2) + (g1**2*MZ**2*sw**2*TH2x2**2*TH3x3**2)/4.)*cmath.sqrt(mh2**4 - 2*mh2**2*mh3**2 + mh3**4 - 2*mh2**2*MZ**2 - 2*mh3**2*MZ**2 + MZ**4))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.h__plus__,P.W__minus__):'((-(gw**2*mh2**2*TH2x2**2)/2. - (gw**2*mhc**2*TH2x2**2)/2. + (gw**2*mh2**4*TH2x2**2)/(4.*MW**2) - (gw**2*mh2**2*mhc**2*TH2x2**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x2**2)/(4.*MW**2) + (gw**2*MW**2*TH2x2**2)/4.)*cmath.sqrt(mh2**4 - 2*mh2**2*mhc**2 + mhc**4 - 2*mh2**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.h__minus__,P.h__plus__):'((l3**2*TH1x2**2*vev**2 + l3*l7*TH1x2*TH2x2*vev**2 + l3*TH1x2*TH2x2*vev**2*complexconjugate(l7) + l7*TH2x2**2*vev**2*complexconjugate(l7))*cmath.sqrt(mh2**4 - 4*mh2**2*mhc**2))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.h__minus__,P.W__plus__):'((-(gw**2*mh2**2*TH2x2**2)/2. - (gw**2*mhc**2*TH2x2**2)/2. + (gw**2*mh2**4*TH2x2**2)/(4.*MW**2) - (gw**2*mh2**2*mhc**2*TH2x2**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x2**2)/(4.*MW**2) + (gw**2*MW**2*TH2x2**2)/4.)*cmath.sqrt(mh2**4 - 2*mh2**2*mhc**2 + mhc**4 - 2*mh2**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.t,P.t__tilde__):'((-3*GU3x3**2*MT**2*TH2x2**2 + 3*GU3x3*mh2**2*TH1x2*TH2x2*yt - 12*GU3x3*MT**2*TH1x2*TH2x2*yt + 3*mh2**2*TH1x2**2*yt**2 - 12*MT**2*TH1x2**2*yt**2 + 3*GU3x3*mh2**2*TH2x2**2*complexconjugate(GU3x3) - 6*GU3x3*MT**2*TH2x2**2*complexconjugate(GU3x3) + 3*mh2**2*TH1x2*TH2x2*yt*complexconjugate(GU3x3) - 12*MT**2*TH1x2*TH2x2*yt*complexconjugate(GU3x3) - 3*MT**2*TH2x2**2*complexconjugate(GU3x3)**2)*cmath.sqrt(mh2**4 - 4*mh2**2*MT**2))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.ta__minus__,P.ta__plus__):'(mh2**2*(GL3x3*mh2**2*TH1x2*TH2x2*ytau + mh2**2*TH1x2**2*ytau**2 + GL3x3*mh2**2*TH2x2**2*complexconjugate(GL3x3) + mh2**2*TH1x2*TH2x2*ytau*complexconjugate(GL3x3)))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.W__minus__,P.W__plus__):'(((3*gw**4*TH1x2**2*vev**2)/4. + (gw**4*mh2**4*TH1x2**2*vev**2)/(16.*MW**4) - (gw**4*mh2**2*TH1x2**2*vev**2)/(4.*MW**2))*cmath.sqrt(mh2**4 - 4*mh2**2*MW**2))/(16.*cmath.pi*abs(mh2)**3)',
                                   (P.Z,P.Z):'(((3*cw**4*gw**4*TH1x2**2*vev**2)/4. + (cw**4*gw**4*mh2**4*TH1x2**2*vev**2)/(16.*MZ**4) - (cw**4*gw**4*mh2**2*TH1x2**2*vev**2)/(4.*MZ**2) + 3*cw**3*g1*gw**3*sw*TH1x2**2*vev**2 + (cw**3*g1*gw**3*mh2**4*sw*TH1x2**2*vev**2)/(4.*MZ**4) - (cw**3*g1*gw**3*mh2**2*sw*TH1x2**2*vev**2)/MZ**2 + (9*cw**2*g1**2*gw**2*sw**2*TH1x2**2*vev**2)/2. + (3*cw**2*g1**2*gw**2*mh2**4*sw**2*TH1x2**2*vev**2)/(8.*MZ**4) - (3*cw**2*g1**2*gw**2*mh2**2*sw**2*TH1x2**2*vev**2)/(2.*MZ**2) + 3*cw*g1**3*gw*sw**3*TH1x2**2*vev**2 + (cw*g1**3*gw*mh2**4*sw**3*TH1x2**2*vev**2)/(4.*MZ**4) - (cw*g1**3*gw*mh2**2*sw**3*TH1x2**2*vev**2)/MZ**2 + (3*g1**4*sw**4*TH1x2**2*vev**2)/4. + (g1**4*mh2**4*sw**4*TH1x2**2*vev**2)/(16.*MZ**4) - (g1**4*mh2**2*sw**4*TH1x2**2*vev**2)/(4.*MZ**2))*cmath.sqrt(mh2**4 - 4*mh2**2*MZ**2))/(32.*cmath.pi*abs(mh2)**3)'})

Decay_h3 = Decay(name = 'Decay_h3',
                 particle = P.h3,
                 partial_widths = {(P.a,P.h1):'((-mh1**2 + mh3**2)*(-(cw**2*g1**2*mh1**2*TH2x1**2*TH3x3**2)/2. - (cw**2*g1**2*mh3**2*TH2x1**2*TH3x3**2)/2. + cw*g1*gw*mh1**2*sw*TH2x1**2*TH3x3**2 + cw*g1*gw*mh3**2*sw*TH2x1**2*TH3x3**2 - (gw**2*mh1**2*sw**2*TH2x1**2*TH3x3**2)/2. - (gw**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/2.))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.a,P.h2):'((-mh2**2 + mh3**2)*(-(cw**2*g1**2*mh2**2*TH2x2**2*TH3x3**2)/2. - (cw**2*g1**2*mh3**2*TH2x2**2*TH3x3**2)/2. + cw*g1*gw*mh2**2*sw*TH2x2**2*TH3x3**2 + cw*g1*gw*mh3**2*sw*TH2x2**2*TH3x3**2 - (gw**2*mh2**2*sw**2*TH2x2**2*TH3x3**2)/2. - (gw**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/2.))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.b,P.b__tilde__):'((3*GD3x3**2*MB**2*TH3x3**2 - 6*GD3x3*MB**2*TH3x3**2*complexconjugate(GD3x3) + 3*GD3x3*mh3**2*TH3x3**2*complexconjugate(GD3x3) + 3*MB**2*TH3x3**2*complexconjugate(GD3x3)**2)*cmath.sqrt(-4*MB**2*mh3**2 + mh3**4))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.h1,P.Z):'((-(cw**2*gw**2*mh1**2*TH2x1**2*TH3x3**2)/2. - (cw**2*gw**2*mh3**2*TH2x1**2*TH3x3**2)/2. + (cw**2*gw**2*mh1**4*TH2x1**2*TH3x3**2)/(4.*MZ**2) - (cw**2*gw**2*mh1**2*mh3**2*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (cw**2*gw**2*mh3**4*TH2x1**2*TH3x3**2)/(4.*MZ**2) + (cw**2*gw**2*MZ**2*TH2x1**2*TH3x3**2)/4. - cw*g1*gw*mh1**2*sw*TH2x1**2*TH3x3**2 - cw*g1*gw*mh3**2*sw*TH2x1**2*TH3x3**2 + (cw*g1*gw*mh1**4*sw*TH2x1**2*TH3x3**2)/(2.*MZ**2) - (cw*g1*gw*mh1**2*mh3**2*sw*TH2x1**2*TH3x3**2)/MZ**2 + (cw*g1*gw*mh3**4*sw*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (cw*g1*gw*MZ**2*sw*TH2x1**2*TH3x3**2)/2. - (g1**2*mh1**2*sw**2*TH2x1**2*TH3x3**2)/2. - (g1**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/2. + (g1**2*mh1**4*sw**2*TH2x1**2*TH3x3**2)/(4.*MZ**2) - (g1**2*mh1**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (g1**2*mh3**4*sw**2*TH2x1**2*TH3x3**2)/(4.*MZ**2) + (g1**2*MZ**2*sw**2*TH2x1**2*TH3x3**2)/4.)*cmath.sqrt(mh1**4 - 2*mh1**2*mh3**2 + mh3**4 - 2*mh1**2*MZ**2 - 2*mh3**2*MZ**2 + MZ**4))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.h2,P.Z):'((-(cw**2*gw**2*mh2**2*TH2x2**2*TH3x3**2)/2. - (cw**2*gw**2*mh3**2*TH2x2**2*TH3x3**2)/2. + (cw**2*gw**2*mh2**4*TH2x2**2*TH3x3**2)/(4.*MZ**2) - (cw**2*gw**2*mh2**2*mh3**2*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (cw**2*gw**2*mh3**4*TH2x2**2*TH3x3**2)/(4.*MZ**2) + (cw**2*gw**2*MZ**2*TH2x2**2*TH3x3**2)/4. - cw*g1*gw*mh2**2*sw*TH2x2**2*TH3x3**2 - cw*g1*gw*mh3**2*sw*TH2x2**2*TH3x3**2 + (cw*g1*gw*mh2**4*sw*TH2x2**2*TH3x3**2)/(2.*MZ**2) - (cw*g1*gw*mh2**2*mh3**2*sw*TH2x2**2*TH3x3**2)/MZ**2 + (cw*g1*gw*mh3**4*sw*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (cw*g1*gw*MZ**2*sw*TH2x2**2*TH3x3**2)/2. - (g1**2*mh2**2*sw**2*TH2x2**2*TH3x3**2)/2. - (g1**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/2. + (g1**2*mh2**4*sw**2*TH2x2**2*TH3x3**2)/(4.*MZ**2) - (g1**2*mh2**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (g1**2*mh3**4*sw**2*TH2x2**2*TH3x3**2)/(4.*MZ**2) + (g1**2*MZ**2*sw**2*TH2x2**2*TH3x3**2)/4.)*cmath.sqrt(mh2**4 - 2*mh2**2*mh3**2 + mh3**4 - 2*mh2**2*MZ**2 - 2*mh3**2*MZ**2 + MZ**4))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.h__plus__,P.W__minus__):'((-(gw**2*mh3**2*TH3x3**2)/2. - (gw**2*mhc**2*TH3x3**2)/2. + (gw**2*mh3**4*TH3x3**2)/(4.*MW**2) - (gw**2*mh3**2*mhc**2*TH3x3**2)/(2.*MW**2) + (gw**2*mhc**4*TH3x3**2)/(4.*MW**2) + (gw**2*MW**2*TH3x3**2)/4.)*cmath.sqrt(mh3**4 - 2*mh3**2*mhc**2 + mhc**4 - 2*mh3**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.h__minus__,P.W__plus__):'((-(gw**2*mh3**2*TH3x3**2)/2. - (gw**2*mhc**2*TH3x3**2)/2. + (gw**2*mh3**4*TH3x3**2)/(4.*MW**2) - (gw**2*mh3**2*mhc**2*TH3x3**2)/(2.*MW**2) + (gw**2*mhc**4*TH3x3**2)/(4.*MW**2) + (gw**2*MW**2*TH3x3**2)/4.)*cmath.sqrt(mh3**4 - 2*mh3**2*mhc**2 + mhc**4 - 2*mh3**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.t,P.t__tilde__):'((3*GU3x3**2*MT**2*TH3x3**2 + 3*GU3x3*mh3**2*TH3x3**2*complexconjugate(GU3x3) - 6*GU3x3*MT**2*TH3x3**2*complexconjugate(GU3x3) + 3*MT**2*TH3x3**2*complexconjugate(GU3x3)**2)*cmath.sqrt(mh3**4 - 4*mh3**2*MT**2))/(16.*cmath.pi*abs(mh3)**3)',
                                   (P.ta__minus__,P.ta__plus__):'(GL3x3*mh3**4*TH3x3**2*complexconjugate(GL3x3))/(16.*cmath.pi*abs(mh3)**3)'})

Decay_h__plus__ = Decay(name = 'Decay_h__plus__',
                        particle = P.h__plus__,
                        partial_widths = {(P.h1,P.W__plus__):'((-(gw**2*mh1**2*TH2x1**2)/2. - (gw**2*mhc**2*TH2x1**2)/2. + (gw**2*mh1**4*TH2x1**2)/(4.*MW**2) - (gw**2*mh1**2*mhc**2*TH2x1**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x1**2)/(4.*MW**2) + (gw**2*MW**2*TH2x1**2)/4.)*cmath.sqrt(mh1**4 - 2*mh1**2*mhc**2 + mhc**4 - 2*mh1**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mhc)**3)',
                                          (P.h2,P.W__plus__):'((-(gw**2*mh2**2*TH2x2**2)/2. - (gw**2*mhc**2*TH2x2**2)/2. + (gw**2*mh2**4*TH2x2**2)/(4.*MW**2) - (gw**2*mh2**2*mhc**2*TH2x2**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x2**2)/(4.*MW**2) + (gw**2*MW**2*TH2x2**2)/4.)*cmath.sqrt(mh2**4 - 2*mh2**2*mhc**2 + mhc**4 - 2*mh2**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mhc)**3)',
                                          (P.h3,P.W__plus__):'((-(gw**2*mh3**2*TH3x3**2)/2. - (gw**2*mhc**2*TH3x3**2)/2. + (gw**2*mh3**4*TH3x3**2)/(4.*MW**2) - (gw**2*mh3**2*mhc**2*TH3x3**2)/(2.*MW**2) + (gw**2*mhc**4*TH3x3**2)/(4.*MW**2) + (gw**2*MW**2*TH3x3**2)/4.)*cmath.sqrt(mh3**4 - 2*mh3**2*mhc**2 + mhc**4 - 2*mh3**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(16.*cmath.pi*abs(mhc)**3)',
                                          (P.t,P.b__tilde__):'((-3*I3a33*MB**2*complexconjugate(I3a33) + 3*I3a33*mhc**2*complexconjugate(I3a33) + 6*I4a33*MB*MT*complexconjugate(I3a33) - 3*I3a33*MT**2*complexconjugate(I3a33) - 3*I4a33*MB**2*complexconjugate(I4a33) + 3*I4a33*mhc**2*complexconjugate(I4a33) + 6*I3a33*MB*MT*complexconjugate(I4a33) - 3*I4a33*MT**2*complexconjugate(I4a33))*cmath.sqrt(MB**4 - 2*MB**2*mhc**2 + mhc**4 - 2*MB**2*MT**2 - 2*mhc**2*MT**2 + MT**4))/(16.*cmath.pi*abs(mhc)**3)',
                                          (P.vt,P.ta__plus__):'(GL3x3*mhc**4*complexconjugate(GL3x3))/(16.*cmath.pi*abs(mhc)**3)'})

Decay_t = Decay(name = 'Decay_t',
                particle = P.t,
                partial_widths = {(P.h__plus__,P.b):'((3*I1a33*MB**2*complexconjugate(I1a33) - 3*I1a33*mhc**2*complexconjugate(I1a33) - 6*I2a33*MB*MT*complexconjugate(I1a33) + 3*I1a33*MT**2*complexconjugate(I1a33) + 3*I2a33*MB**2*complexconjugate(I2a33) - 3*I2a33*mhc**2*complexconjugate(I2a33) - 6*I1a33*MB*MT*complexconjugate(I2a33) + 3*I2a33*MT**2*complexconjugate(I2a33))*cmath.sqrt(MB**4 - 2*MB**2*mhc**2 + mhc**4 - 2*MB**2*MT**2 - 2*mhc**2*MT**2 + MT**4))/(96.*cmath.pi*abs(MT)**3)',
                                  (P.W__plus__,P.b):'(((3*gw**2*MB**2)/2. + (3*gw**2*MT**2)/2. + (3*gw**2*MB**4)/(2.*MW**2) - (3*gw**2*MB**2*MT**2)/MW**2 + (3*gw**2*MT**4)/(2.*MW**2) - 3*gw**2*MW**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(96.*cmath.pi*abs(MT)**3)'})

Decay_W__plus__ = Decay(name = 'Decay_W__plus__',
                        particle = P.W__plus__,
                        partial_widths = {(P.c,P.s__tilde__):'(gw**2*MW**4)/(16.*cmath.pi*abs(MW)**3)',
                                          (P.h1,P.h__plus__):'((-(gw**2*mh1**2*TH2x1**2)/2. - (gw**2*mhc**2*TH2x1**2)/2. + (gw**2*mh1**4*TH2x1**2)/(4.*MW**2) - (gw**2*mh1**2*mhc**2*TH2x1**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x1**2)/(4.*MW**2) + (gw**2*MW**2*TH2x1**2)/4.)*cmath.sqrt(mh1**4 - 2*mh1**2*mhc**2 + mhc**4 - 2*mh1**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.h2,P.h__plus__):'((-(gw**2*mh2**2*TH2x2**2)/2. - (gw**2*mhc**2*TH2x2**2)/2. + (gw**2*mh2**4*TH2x2**2)/(4.*MW**2) - (gw**2*mh2**2*mhc**2*TH2x2**2)/(2.*MW**2) + (gw**2*mhc**4*TH2x2**2)/(4.*MW**2) + (gw**2*MW**2*TH2x2**2)/4.)*cmath.sqrt(mh2**4 - 2*mh2**2*mhc**2 + mhc**4 - 2*mh2**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.h3,P.h__plus__):'((-(gw**2*mh3**2*TH3x3**2)/2. - (gw**2*mhc**2*TH3x3**2)/2. + (gw**2*mh3**4*TH3x3**2)/(4.*MW**2) - (gw**2*mh3**2*mhc**2*TH3x3**2)/(2.*MW**2) + (gw**2*mhc**4*TH3x3**2)/(4.*MW**2) + (gw**2*MW**2*TH3x3**2)/4.)*cmath.sqrt(mh3**4 - 2*mh3**2*mhc**2 + mhc**4 - 2*mh3**2*MW**2 - 2*mhc**2*MW**2 + MW**4))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.t,P.b__tilde__):'(((-3*gw**2*MB**2)/2. - (3*gw**2*MT**2)/2. - (3*gw**2*MB**4)/(2.*MW**2) + (3*gw**2*MB**2*MT**2)/MW**2 - (3*gw**2*MT**4)/(2.*MW**2) + 3*gw**2*MW**2)*cmath.sqrt(MB**4 - 2*MB**2*MT**2 + MT**4 - 2*MB**2*MW**2 - 2*MT**2*MW**2 + MW**4))/(48.*cmath.pi*abs(MW)**3)',
                                          (P.u,P.d__tilde__):'(gw**2*MW**4)/(16.*cmath.pi*abs(MW)**3)',
                                          (P.ve,P.e__plus__):'(gw**2*MW**4)/(48.*cmath.pi*abs(MW)**3)',
                                          (P.vm,P.mu__plus__):'(gw**2*MW**4)/(48.*cmath.pi*abs(MW)**3)',
                                          (P.vt,P.ta__plus__):'(gw**2*MW**4)/(48.*cmath.pi*abs(MW)**3)'})

Decay_Z = Decay(name = 'Decay_Z',
                particle = P.Z,
                partial_widths = {(P.a,P.h1):'((-mh1**2 + MZ**2)*((3*cw**4*g1**2*gw**2*TH1x1**2*vev**2)/4. + (3*cw**3*g1**3*gw*sw*TH1x1**2*vev**2)/2. - (3*cw**3*g1*gw**3*sw*TH1x1**2*vev**2)/2. + (3*cw**2*g1**4*sw**2*TH1x1**2*vev**2)/4. - 3*cw**2*g1**2*gw**2*sw**2*TH1x1**2*vev**2 + (3*cw**2*gw**4*sw**2*TH1x1**2*vev**2)/4. - (3*cw*g1**3*gw*sw**3*TH1x1**2*vev**2)/2. + (3*cw*g1*gw**3*sw**3*TH1x1**2*vev**2)/2. + (3*g1**2*gw**2*sw**4*TH1x1**2*vev**2)/4.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.a,P.h2):'((-mh2**2 + MZ**2)*((3*cw**4*g1**2*gw**2*TH1x2**2*vev**2)/4. + (3*cw**3*g1**3*gw*sw*TH1x2**2*vev**2)/2. - (3*cw**3*g1*gw**3*sw*TH1x2**2*vev**2)/2. + (3*cw**2*g1**4*sw**2*TH1x2**2*vev**2)/4. - 3*cw**2*g1**2*gw**2*sw**2*TH1x2**2*vev**2 + (3*cw**2*gw**4*sw**2*TH1x2**2*vev**2)/4. - (3*cw*g1**3*gw*sw**3*TH1x2**2*vev**2)/2. + (3*cw*g1*gw**3*sw**3*TH1x2**2*vev**2)/2. + (3*g1**2*gw**2*sw**4*TH1x2**2*vev**2)/4.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.b,P.b__tilde__):'(((-3*cw**2*gw**2*MB**2)/2. + (3*cw**2*gw**2*MZ**2)/2. - 7*cw*g1*gw*MB**2*sw + cw*g1*gw*MZ**2*sw - (17*g1**2*MB**2*sw**2)/6. + (5*g1**2*MZ**2*sw**2)/6.)*cmath.sqrt(-4*MB**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.c,P.c__tilde__):'(MZ**2*((3*cw**2*gw**2*MZ**2)/2. - cw*g1*gw*MZ**2*sw + (17*g1**2*MZ**2*sw**2)/6.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.d,P.d__tilde__):'(MZ**2*((3*cw**2*gw**2*MZ**2)/2. + cw*g1*gw*MZ**2*sw + (5*g1**2*MZ**2*sw**2)/6.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.e__minus__,P.e__plus__):'(MZ**2*((cw**2*gw**2*MZ**2)/2. - cw*g1*gw*MZ**2*sw + (5*g1**2*MZ**2*sw**2)/2.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.h1,P.h3):'((-(cw**2*gw**2*mh1**2*TH2x1**2*TH3x3**2)/2. - (cw**2*gw**2*mh3**2*TH2x1**2*TH3x3**2)/2. + (cw**2*gw**2*mh1**4*TH2x1**2*TH3x3**2)/(4.*MZ**2) - (cw**2*gw**2*mh1**2*mh3**2*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (cw**2*gw**2*mh3**4*TH2x1**2*TH3x3**2)/(4.*MZ**2) + (cw**2*gw**2*MZ**2*TH2x1**2*TH3x3**2)/4. - cw*g1*gw*mh1**2*sw*TH2x1**2*TH3x3**2 - cw*g1*gw*mh3**2*sw*TH2x1**2*TH3x3**2 + (cw*g1*gw*mh1**4*sw*TH2x1**2*TH3x3**2)/(2.*MZ**2) - (cw*g1*gw*mh1**2*mh3**2*sw*TH2x1**2*TH3x3**2)/MZ**2 + (cw*g1*gw*mh3**4*sw*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (cw*g1*gw*MZ**2*sw*TH2x1**2*TH3x3**2)/2. - (g1**2*mh1**2*sw**2*TH2x1**2*TH3x3**2)/2. - (g1**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/2. + (g1**2*mh1**4*sw**2*TH2x1**2*TH3x3**2)/(4.*MZ**2) - (g1**2*mh1**2*mh3**2*sw**2*TH2x1**2*TH3x3**2)/(2.*MZ**2) + (g1**2*mh3**4*sw**2*TH2x1**2*TH3x3**2)/(4.*MZ**2) + (g1**2*MZ**2*sw**2*TH2x1**2*TH3x3**2)/4.)*cmath.sqrt(mh1**4 - 2*mh1**2*mh3**2 + mh3**4 - 2*mh1**2*MZ**2 - 2*mh3**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.h2,P.h3):'((-(cw**2*gw**2*mh2**2*TH2x2**2*TH3x3**2)/2. - (cw**2*gw**2*mh3**2*TH2x2**2*TH3x3**2)/2. + (cw**2*gw**2*mh2**4*TH2x2**2*TH3x3**2)/(4.*MZ**2) - (cw**2*gw**2*mh2**2*mh3**2*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (cw**2*gw**2*mh3**4*TH2x2**2*TH3x3**2)/(4.*MZ**2) + (cw**2*gw**2*MZ**2*TH2x2**2*TH3x3**2)/4. - cw*g1*gw*mh2**2*sw*TH2x2**2*TH3x3**2 - cw*g1*gw*mh3**2*sw*TH2x2**2*TH3x3**2 + (cw*g1*gw*mh2**4*sw*TH2x2**2*TH3x3**2)/(2.*MZ**2) - (cw*g1*gw*mh2**2*mh3**2*sw*TH2x2**2*TH3x3**2)/MZ**2 + (cw*g1*gw*mh3**4*sw*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (cw*g1*gw*MZ**2*sw*TH2x2**2*TH3x3**2)/2. - (g1**2*mh2**2*sw**2*TH2x2**2*TH3x3**2)/2. - (g1**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/2. + (g1**2*mh2**4*sw**2*TH2x2**2*TH3x3**2)/(4.*MZ**2) - (g1**2*mh2**2*mh3**2*sw**2*TH2x2**2*TH3x3**2)/(2.*MZ**2) + (g1**2*mh3**4*sw**2*TH2x2**2*TH3x3**2)/(4.*MZ**2) + (g1**2*MZ**2*sw**2*TH2x2**2*TH3x3**2)/4.)*cmath.sqrt(mh2**4 - 2*mh2**2*mh3**2 + mh3**4 - 2*mh2**2*MZ**2 - 2*mh3**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.h__minus__,P.h__plus__):'((-(cw**2*gw**2*mhc**2) + (cw**2*gw**2*MZ**2)/4. + 2*cw*g1*gw*mhc**2*sw - (cw*g1*gw*MZ**2*sw)/2. - g1**2*mhc**2*sw**2 + (g1**2*MZ**2*sw**2)/4.)*cmath.sqrt(-4*mhc**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.mu__minus__,P.mu__plus__):'(MZ**2*((cw**2*gw**2*MZ**2)/2. - cw*g1*gw*MZ**2*sw + (5*g1**2*MZ**2*sw**2)/2.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.s,P.s__tilde__):'(MZ**2*((3*cw**2*gw**2*MZ**2)/2. + cw*g1*gw*MZ**2*sw + (5*g1**2*MZ**2*sw**2)/6.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.t,P.t__tilde__):'(((-3*cw**2*gw**2*MT**2)/2. + (3*cw**2*gw**2*MZ**2)/2. - 11*cw*g1*gw*MT**2*sw - cw*g1*gw*MZ**2*sw + (7*g1**2*MT**2*sw**2)/6. + (17*g1**2*MZ**2*sw**2)/6.)*cmath.sqrt(-4*MT**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ta__minus__,P.ta__plus__):'(MZ**2*((cw**2*gw**2*MZ**2)/2. - cw*g1*gw*MZ**2*sw + (5*g1**2*MZ**2*sw**2)/2.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.u,P.u__tilde__):'(MZ**2*((3*cw**2*gw**2*MZ**2)/2. - cw*g1*gw*MZ**2*sw + (17*g1**2*MZ**2*sw**2)/6.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.ve,P.ve__tilde__):'(MZ**2*((cw**2*gw**2*MZ**2)/2. + cw*g1*gw*MZ**2*sw + (g1**2*MZ**2*sw**2)/2.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.vm,P.vm__tilde__):'(MZ**2*((cw**2*gw**2*MZ**2)/2. + cw*g1*gw*MZ**2*sw + (g1**2*MZ**2*sw**2)/2.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.vt,P.vt__tilde__):'(MZ**2*((cw**2*gw**2*MZ**2)/2. + cw*g1*gw*MZ**2*sw + (g1**2*MZ**2*sw**2)/2.))/(48.*cmath.pi*abs(MZ)**3)',
                                  (P.W__minus__,P.W__plus__):'((-12*cw**2*gw**2*MW**2 - 17*cw**2*gw**2*MZ**2 + (4*cw**2*gw**2*MZ**4)/MW**2 + (cw**2*gw**2*MZ**6)/(4.*MW**4))*cmath.sqrt(-4*MW**2*MZ**2 + MZ**4))/(48.*cmath.pi*abs(MZ)**3)'})

