# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id$
#
# File: CaloClusterCorrection/python/CaloSwPhioff_v4_data.py
# Created: Sep 2010, sss
# Purpose: Phi position corrections, v4, data version.
#
# It turns out that the direction of the accordions is backwards in the MC
# in the barrel.
# So the correction required in the data is opposite that of the MC.
# So this correction is identical to _v4, except that the sign is flipped
# in the barrel.
#

from CaloClusterCorrection.common import *
from CaloClusterCorrection.CaloSwPhioff_v4 import \
     CaloSwPhioff_v4_b2_parms, CaloSwPhioff_v4_e2_parms


class CaloSwPhioff_v4data_b2_parms:
    pass


def _flip_phi2 (corr2):
    return [[eta, -off] for (eta,off) in corr2]
def _flip_phi1 (corr1):
    return [_flip_phi2(c) for c in corr1]
def _flip_phi (corr):
    return dict([[k, _flip_phi1(v)] for k,v in corr.items()])

    
def _copy_parms (src, dst):
    for (k, v) in src.__dict__.items():
        if k == 'correction':
            v = _flip_phi (v)
        setattr (dst, k, v)
    return

_copy_parms (CaloSwPhioff_v4_b2_parms, CaloSwPhioff_v4data_b2_parms)


CaloSwPhioff_v4data_parms = { EMB2 : CaloSwPhioff_v4data_b2_parms,
                              EME2 : CaloSwPhioff_v4_e2_parms,
                              }


