# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from TrigValJobOptionBlock    import TrigValJobOptionBlock
from TrigValExecutableBlock   import TrigValExecutableBlock
from TrigValConfigBlock       import TrigValConfigBlock
from TrigValDetectorBlock     import TrigValDetectorBlock
from TrigValSliceBlock        import TrigValSliceBlock
from TrigValMenuBlock         import TrigValMenuBlock
from TrigValInputBlock        import TrigValInputBlock
from TrigValOutputBlock       import TrigValOutputBlock
from TrigValValidationBlock   import TrigValValidationBlock
from TrigValSpecialFlagsBlock import TrigValSpecialFlagsBlock 
from TrigValTemporaryFixBlock import TrigValTemporaryFixBlock
from TrigValValidationBlock   import TrigValValidationBlock


##################################################

class TrigValJobOptionBuilder :
  def __init__(self, detector = "All", slice = "All", menu = "Default", 
               executable = "Athena", config = "Modern", input = "RDO",
               output = "", specialFlags = "", validation = "") :

    self.Input         = TrigValInputBlock(        input        )
    self.Executable    = TrigValExecutableBlock(   executable   )
    self.Config        = TrigValConfigBlock(       config       )    
    self.Output        = TrigValOutputBlock(       output       )
    self.SpecialFlags  = TrigValSpecialFlagsBlock( specialFlags )
    self.Detector      = TrigValDetectorBlock(     detector     )
    # temporary fix for the Muon and Bphysics slices
    if ( slice == "Muon" and config == "OldConfig" ) :
      print "WARNING: temporary fix for Muon slice"
      self.Slice         = TrigValSliceBlock( "MuonOldConfig" )
    elif ( slice == "Bphysics" and config == "OldConfig" ) :
      print "WARNING: temporary fix for Bphysics slice"
      self.Slice         = TrigValSliceBlock( "BphysicsOldConfig" )      
    else :
      self.Slice         = TrigValSliceBlock(        slice        )
    self.Menu          = TrigValMenuBlock(         menu         )
    self.Validation    = TrigValValidationBlock(   validation   )
    self.TemporaryFix  = TrigValTemporaryFixBlock( 'Fix' )  # choose from ['Fix','None']
   

    self.blockOrder = ['Input', 'Executable', 'Config', 'Output','SpecialFlags', 'Detector', 'Menu',
                       'Validation',  'TemporaryFix', 'Slice' ]


    self.selfConsistent =  self.consistentTags()
    if( not self.selfConsistent ): self.die()


    self.jobNamePath = "TriggerTest"
    if( output == 'ESD' or output == 'AOD' or output == 'ESDAOD' or input=='ESD' or input == 'AOD' ) :
       self.jobNamePath = "TrigAnalysisTest"



  #################### this will go on top of each jobO : #############################
  def printHead(self, jobO):

    tmpStr="# To generate "+jobO.name+" use  TriggerTest/python/trigValMakeJobOptions.py \n"

    self.HeadComment="""######################################################################################
#
# This file is automatically generated with TriggerTest/python/TrigvalJobOptionBuilder.py
"""
    self.HeadComment += tmpStr
    self.HeadComment+="""#
######################################################################################

"""
    self.write(jobO, self.HeadComment)


  ############ restrict particular combinations of tags here############################
  def consistentTags(self) :
    #if( self.Input.current() == '' ) :
    #   print " TrigValJobOptionBuilder   ERROR : wrong I/O "+input+" to "+output
    #   return False
    #else :
       return True
      
  ####################################################################################  

  def die(self) :
     print " TrigValJobOptionBuilder kills the job"
     import sys 
     sys.exit(2)

  def write(self, jobO, whatToWrite) :
    try :
      jobO.write(whatToWrite)
    except IOError:
      print "TrigValJobOptionBuilder    ERROR:   Can not open "+topOname
      self.die() 
    


  def jobName(self) :
    if( not  self.selfConsistent ) :
       print " TrigValJobOptionBuilder   ERROR : inconsistent set of tags"
       return ""
    else :
       jobname = self.jobNamePath+"/"+"test"+self.Detector.jobNameBlock()
       jobname += self.Slice.jobNameBlock()
       jobname += self.Menu.jobNameBlock()
       jobname += self.Executable.jobNameBlock()
       jobname += self.Config.jobNameBlock()
       jobname += self.Input.jobNameBlock() + self.Output.jobNameBlock()
       jobname += self.SpecialFlags.jobNameBlock()
       jobname += ".py"       
       return jobname


  def includeRecExCommon_flags(self, jobO) : 
     self.write(jobO,self.Executable.includeRecExCommon_flags()) 


  def includeRecExCommon_topOptions(self, jobO) :
     self.write(jobO,self.Executable.includeRecExCommon_topOptions()) 

  #################################################################
  def makeJobOption(self) :
     if( not  self.selfConsistent ) : return None
     topOname = self.jobName()
     try :
       import os
       if( not os.path.isdir(self.jobNamePath )) :
          os.makedirs( self.jobNamePath )
       jobO = open( topOname, mode = "w")
       print "Creating top jobOption "+topOname

       #### print comments to topO
       self.printHead(jobO)

       #### this goes before RecExCommon_flags
       for x in  self.blockOrder :
         block = self.__dict__.get(x)
         if( isinstance(block, TrigValJobOptionBlock)) :
            block.includeJobProperties(jobO)

       #### include RecExCommon_flags
       self.includeRecExCommon_flags(jobO)  

       #### this goes between RecExCommon_flags and  RecExCommon_topOption
       for x in   self.blockOrder :
         block = self.__dict__.get(x)
         if( isinstance(block, TrigValJobOptionBlock)) :
            block.modifyJobFlags(jobO)

       #### include RecExCommon_topOption
       self.includeRecExCommon_topOptions(jobO)  


       #### this goes after  RecExCommon_topOption
       for x in   self.blockOrder : 
         block = self.__dict__.get(x)
         if( isinstance(block, TrigValJobOptionBlock)) :
            block.addExtraTopOption(jobO)

       #### jobO is done
       jobO.close()

       #### now grep for WARNING
       jobO = open( topOname, mode ="r")
       foundWarning=False
       import string
       for line in jobO :
          if (( string.find(line,"WARNING" )>0 or string.find(line,"ERROR" )>0 ) and string.find(line,"OutputLevel") == 0 ) :
             foundWarning=True
       if( foundWarning ) :
          jobO.seek(0)
          print "There are WARNING/ERROR in "+topOname +"  :"
          for line in jobO :
            if ( string.find(line,"WARNING" )>0 or string.find(line,"ERROR" )>0 ) :
               print line
       jobO.close()

     except IOError:
         print "TrigValJobOptionBuilder    ERROR:   Can not open "+topOname
         
    ### end of makeJobOption


  def help(self) :
     tmpHelp="""

To generate jobOptions please use e.g :
  builder = TrigValJobOptionBuilder('Calo','All','All','Athena', 'RDO', "", "")
  builder.makeJobOption()
where options are :
"""
     print tmpHelp
     for x in  self.__dict__ :
        block = self.__dict__.get(x)
        if( isinstance(block, TrigValJobOptionBlock)) :
           block.help()


