# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Because the obvious solution doesn't work on lxplus :(

# import locale
# locale.setlocale(locale.LC_NUMERIC, "")
# locale.format("%i", num, True)

# http://code.activestate.com/recipes/498181-add-thousands-separator-commas-to-formatted-number/

# Code from Michael Robellard's comment made 28 Feb 2010
def splitThousands(s, tSep=',', dSep='.'):
    if s == None:
        return 0
    if isinstance(s, int) or isinstance(s, long) or isinstance(s, float):
        s = str(s)
    if s[0] == '-' or s[0] == '+':
        lhs=s[0]
        s=s[1:]
    else:
        lhs=''
    if dSep != '' and s.rfind(dSep)>0:
        rhs=s[s.rfind(dSep)+1:]
        s=s[:s.rfind(dSep)]
        if len(s) <= 3: return lhs + s + dSep + rhs
        return lhs + splitThousands(s[:-3], tSep) + tSep + s[-3:] + dSep + rhs
    else:
        if len(s) <= 3: return lhs + s
        return lhs + splitThousands(s[:-3], tSep) + tSep + s[-3:]
