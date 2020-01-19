# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



def GetCoolChannelSelection(partition, gain, withPS):
    if (gain<0 or gain>3):
        print "ERROR Gain out of range: ", gain
        return ""
    AccList=[]
    PSList=[]
    if (gain==0):
        g=0
    elif (gain==1): #Medium
        g=239
    elif (gain==2):
        g=475
    else:
        print "ERROR Gain out of range: ", gain
        return ""
    retString=""
    if (partition=="EMBA"):
        retstring="%i:%i"%(g+35,g+66)
        if (withPS):
            retstring+=",%s:%s"%(g+149,g+180)
            
    elif (partition=="EMBC"):
        retstring="%i:%i"%(g+3,g+34)
        if (withPS):
            retstring+=",%s:%s"%(g+117,g+148)
    
    elif (partition=="EMECA"):
        retstring="%i:%i,%i,%i,%i,%i"\
                     %(g+92,g+116,g+232,g+234,g+236,g+238)
        if (withPS):
            retstring+=",%s:%s"%(g+210,g+230)
            
    elif (partition=="EMECA"):
        retstring="%i:%i,%i,%i,%i,%i"\
                   %(g+67,g+91,g+231,g+233,g+235,g+237)
        if (withPS):
            retstring+=",%s:%s"%(g+181,g+209)
            
    return retstring
