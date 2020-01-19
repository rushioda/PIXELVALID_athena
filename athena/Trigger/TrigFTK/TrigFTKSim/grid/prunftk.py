#!/bin/bash

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"exec" "python" "-u" "-Wignore" "$0" "$@"

import re
import os
import sys
import time
import shutil
import atexit
import datetime
import optparse
import commands
import urllib
import shelve
import copy

_FORKED_FROM = 'panda-client-0.3.24'
_FTKDEBUG = False

# default cloud/site
defaultCloud = None
defaultSite  = 'AUTO'

# max lookup for cross site option
maxCrossSite = 10

# error code
EC_Config    = 10
EC_Dataset   = 40
EC_Post      = 50
EC_Archive   = 60
EC_Split     = 70
EC_MyProxy   = 80
EC_Submit    = 90

usage = """%prog [options]

  HowTo is available at https://twiki.cern.ch/twiki/bin/view/Atlas/PandaRun"""

# command-line parameters
optP = optparse.OptionParser(usage=usage,conflict_handler="resolve")
optP.add_option('--version',action='store_const',const=True,dest='version',default=False,
                help='Displays version')
optP.add_option('--inDS',action='store',dest='inDS',default='',type='string',
                help='Name of an input dataset or dataset container')
optP.add_option('--goodRunListXML', action='store', dest='goodRunListXML', default='',
                type='string', help='Good Run List XML which will be converted to datasets by AMI')
optP.add_option('--goodRunListDataType', action='store', dest='goodRunDataType', default='',
                type='string', help='specify data type when converting Good Run List XML to datasets, e.g, AOD (default)')
optP.add_option('--goodRunListProdStep', action='store', dest='goodRunProdStep', default='',
                type='string', help='specify production step when converting Good Run List to datasets, e.g, merge (default)')
optP.add_option('--goodRunListDS', action='store', dest='goodRunListDS', default='',
                type='string', help='A comma-separated list of pattern strings. Datasets which are converted from Good Run List XML will be used when they match with one of the pattern strings. Either \ or "" is required when a wild-card is used. If this option is omitted all datasets will be used')
optP.add_option('--eventPickEvtList',action='store',dest='eventPickEvtList',default='',
                type='string', help='a file name which contains a list of runs/events for event picking')
optP.add_option('--eventPickDataType',action='store',dest='eventPickDataType',default='',
                type='string', help='type of data for event picking. one of AOD,ESD,RAW')
optP.add_option('--eventPickStreamName',action='store',dest='eventPickStreamName',default='',
                type='string', help='stream name for event picking. e.g., physics_CosmicCaloEM')
optP.add_option('--eventPickDS',action='store',dest='eventPickDS',default='',
                type='string', help='A comma-separated list of pattern strings. Datasets which are converted from the run/event list will be used when they match with one of the pattern strings. Either \ or "" is required when a wild-card is used. e.g., data\*')
optP.add_option('--express', action='store_const',const=True,dest='express',default=False,
                help="Send the job using express quota to have higher priority. The number of express subjobs in the queue and the total execution time used by express subjobs are limited (a few subjobs and several hours per day, respectively). This option is intended to be used for quick tests before bulk submission. Note that buildXYZ is not included in quota calculation. If this option is used when quota has already exceeded, the panda server will ignore the option so that subjobs have normal priorities. Also, if you submit 1 buildXYZ and N runXYZ subjobs when you only have quota of M (M < N),  only the first M runXYZ subjobs will have higher priorities")
optP.add_option('--useContElementBoundary',action='store_const',const=True,dest='useContElementBoundary',default=False,
                help="Split job in such a way that sub job do not mix files of different datasets in the input container. See --useNthFieldForLFN too")
optP.add_option('--useNthFieldForLFN',action='store',dest='useNthFieldForLFN',default=2,type='int',
                help="all output files from a dataset in the input container have the same middle name in LFN, when --useContElementBoundary is used. The middle name is extracted from the dataset name. The second field is used by default. i.e., if the dataset name is data10_7TeV.00160387.physics_Muon..., 00160387 is used and LFN is something like user.hoge.JOBSETID.00160387.blha. One can change the extracted field using this option")
optP.add_option('--outDS',action='store',dest='outDS',default='',type='string',
                help='Name of an output dataset. OUTDS will contain all output files')
optP.add_option('--destSE',action='store', dest='destSE',default='',type='string',
                help='Destination strorage element')
optP.add_option('--libDS',action='store',dest='libDS',default='',type='string',
                help='Name of a library dataset')
optP.add_option('--removedDS', action='store', dest='removedDS', default='',
                type='string', help="don't use datasets in the input dataset container")
optP.add_option('--noBuild', action='store_const',const=True,dest='nobuild',default=False,
                help='Skip buildGen')
optP.add_option('--secondaryDSs',action='store',dest='secondaryDSs',default='',type='string',
                help='List of secondary datasets when the job requires multiple inputs. See PandaRun wiki page for detail')
optP.add_option('--site',action='store',dest='site',default=defaultSite,type='string',
                help='Site name where jobs are sent (default:%s)' % defaultSite)
optP.add_option('--cloud',action='store',dest='cloud',default=None,type='string',
                help='Cloud where jobs are submitted. default is set according to your VOMS country group')
optP.add_option('--match',action='store',dest='match',default='',type='string',
                help='Use only files matching with given pattern')
optP.add_option('--antiMatch',action='store',dest='antiMatch',default='',type='string',
                help='Skip files matching with given pattern')
optP.add_option('--skipScan',action='store_const',const=True,dest='skipScan',default=False,
                help='Skip LFC lookup at job submission')
optP.add_option('--memory',action='store',dest='memory',default=-1,type='int',
                help='Required memory size')
optP.add_option('--maxCpuCount',action='store',dest='maxCpuCount',default=-1,type='int',
                help='Required CPU count in seconds. Mainly to extend time limit for looping detection')
optP.add_option('--official',action='store_const',const=True,dest='official',default=False,
                help='Produce official dataset')
optP.add_option('--descriptionInLFN',action='store',dest='descriptionInLFN',default='',
                help='LFN is user.nickname.jobsetID.something (e.g. user.harumaki.12345.AOD._00001.pool) by default. This option allows users to put a description string into LFN. i.e., user.nickname.jobsetID.description.something')
optP.add_option('--useAthenaPackages',action='store_const',const=True,dest='useAthenaPackages',default=False,
                help='Use Athena packages. See PandaRun wiki page for detail')
optP.add_option('--gluePackages', action='store', dest='gluePackages',  default='',
                help='list of glue packages which pathena cannot fine due to empty i686-slc4-gcc34-opt. e.g., External/AtlasHepMC,External/Lhapdf')
optP.add_option('--nFiles',action='store',dest='nFiles',default=0,type='int',
                help='Use a limited number of files in the input dataset')
optP.add_option('--nSkipFiles',action='store',dest='nSkipFiles',default=0,type='int',
                help='Skip N files in the input dataset')
optP.add_option('--nFilesPerJob',action='store',dest='nFilesPerJob',default=None,type='int',
                help='Number of files on which each sub-job runs (default 50). Note that this is the number of files per sub-job in the primary dataset even if --secondaryDSs is used')
optP.add_option('--nJobs',action='store',dest='nJobs',default=-1,type='int',
                help='Maximum number of sub-jobs. If the number of input files (N_in) is less than nJobs*nFilesPerJob, only N_in/nFilesPerJob sub-jobs will be instantiated')
optP.add_option('--nGBPerJob',action='store',dest='nGBPerJob',default=-1,
                help='Instantiate one sub job per NGBPERJOB GB of input files. --nGBPerJob=MAX sets the size to the default maximum value')
optP.add_option('--maxFileSize',action='store',dest='maxFileSize',default=1024*1024,type='int',
                help='Maximum size of files to be sent to WNs (default 1024*1024B)')
optP.add_option('--athenaTag',action='store',dest='athenaTag',default='',type='string',
                help='Tags to setup Athena on remote WNs, e.g., --athenaTag=AtlasProduction,14.2.24.3')
optP.add_option('--workDir',action='store',dest='workDir',default='.',type='string',
                help='All files under WORKDIR will be transfered to WNs (default=./)')
optP.add_option('--extFile',action='store',dest='extFile',default='',
                help='root or large files under WORKDIR are not sent to WNs by default. If you want to send some skipped files, specify their names, e.g., data.root,data.tgz')
optP.add_option('--excludeFile',action='store',dest='excludeFile',default='',
                help='specify a comma-separated string to exclude files and/or directories when gathering files in local working area. Either \ or "" is required when a wildcard is used. e.g., doc,\*.C')
optP.add_option('--inputFileList', action='store', dest='inputFileListName', default='',
                type='string', help='name of file which contains a list of files to be run in the input dataset')
optP.add_option('--crossSite',action='store',dest='crossSite',default=maxCrossSite,
                type='int',help='submit jobs to N sites at most when datasets in container split over many sites (N=%s by default)' % maxCrossSite)
optP.add_option('--outputs',action='store',dest='outputs',default='',type='string',
                help='Names of output files. Comma separated. e.g., --outputs out1.dat,out2.txt')
optP.add_option('--excludedSite', action='store', dest='excludedSite',  default='',
                help="list of sites which are not used for site section, e.g., ANALY_ABC,ANALY_XYZ")
optP.add_option('--noSubmit',action='store_const',const=True,dest='nosubmit',default=False,
                help="Don't submit jobs")
optP.add_option('--prodSourceLabel', action='store', dest='prodSourceLabel', default='',
                help="set prodSourceLabel")
optP.add_option('--processingType', action='store', dest='processingType', default='prun',
                help="set processingType")
optP.add_option('--seriesLabel', action='store', dest='seriesLabel',  default='',
                help="set seriesLabel")
optP.add_option('--workingGroup', action='store', dest='workingGroup',  default=None,
                help="set workingGroup")
optP.add_option('--tmpDir',action='store',dest='tmpDir',default='',type='string',
                help='Temporary directory where an archive file is created')
optP.add_option('--voms', action='store', dest='vomsRoles',  default=None, type='string',
                help="generate proxy with paticular roles. e.g., atlas:/atlas/ca/Role=production,atlas:/atlas/fr/Role=pilot")
optP.add_option('--update', action='store_const', const=True, dest='update',  default=False,
                help='Update panda-client to the latest version')
optP.add_option('--spaceToken',action='store',dest='spaceToken',default='',type='string',
                help='spacetoken for outputs. e.g., ATLASLOCALGROUPDISK')
optP.add_option('--devSrv',action='store_const',const=True,dest='devSrv',default=False,
                help="Please don't use this option. Only for developers to use the dev panda server")
optP.add_option('--outTarBall', action='store', dest='outTarBall', default='',
                type='string', help='Save a copy of local files which is the input to buildXYZ')
optP.add_option('--inTarBall', action='store', dest='inTarBall', default='',
                type='string', help='Use a saved copy of local files as input to buildXYZ')
optP.add_option('--exec',action='store',dest='jobParams',default='',type='string',
                help='execution string. e.g., --exec "./myscript arg1 arg2"')
optP.add_option('--bexec',action='store',dest='bexec',default='',type='string',
                help='execution string for build stage. e.g., --bexec "make"')
optP.add_option('--myproxy',action='store',dest='myproxy',default='myproxy.cern.ch',type='string',
                help='Name of the myproxy server')
optP.add_option('--maxNFilesPerJob',action='store',dest='maxNFilesPerJob',default=200,type='int',
                help='The maximum number of files per job is 200 by default since too many input files result in a too long command-line argument on WN which crashes the job. This option relax the limit. In many cases it is better to use this option together with --writeInputToTxt')
optP.add_option('--writeInputToTxt',action='store',dest='writeInputToTxt',default='',type='string',
                help='Write the input file list to a file so that your application gets the list from the file instead of stdin. The argument is a comma separated list of StreamName:FileName. e.g., IN:input1.txt,IN2:input2.txt')
optP.add_option('--dbRelease',action='store',dest='dbRelease',default='',
                type='string', help='DBRelease or CDRelease (DatasetName:FileName). e.g., ddo.000001.Atlas.Ideal.DBRelease.v050101:DBRelease-5.1.1.tar.gz. If --dbRelease=LATEST, the latest DBRelease is used. Most likely the --useAthenaPackages or --athenaTag option is required to setup Athena runtime on WN')
optP.add_option('--dbRunNumber',action='store',dest='dbRunNumber', default='',
                type='string', help='RunNumber for DBRelease or CDRelease. If this option is used some redundant files are removed to save disk usage when unpacking DBRelease tarball. e.g., 0091890')
optP.add_option('--notExpandDBR',action='store_const',const=True,dest='notExpandDBR',default=False,
                help='By default, DBRelease.tar.gz is expanded on WN and gets deleted after changing environment variables accordingly. If you need tar.gz, use this option')
optP.add_option('--provenanceID',action='store',dest='provenanceID',default=-1,type='int',
                help='provenanceID')
optP.add_option('-v',action='store_const',const=True,dest='verbose',default=False,
                help='Verbose')
optP.add_option('--long', action='store_const',const=True,dest='long',default=False,
                help='Send job to a long queue')
optP.add_option('--outputPath',action='store',dest='outputPath', default='',
                type='string', help='Physical path of output directory relative to a root path')
optP.add_option('--useOldStyleOutput',action='store_const',const=True,dest='useOldStyleOutput',default=False,
                help="use output dataset and long LFN instead of output dataset container and short LFN")
optP.add_option('--disableRebrokerage',action='store_const',const=True,dest='disableRebrokerage',default=False,
                help="disable auto-rebrokerage")
optP.add_option('--useChirpServer',action='store',dest='useChirpServer', default='',
                type='string', help='The CHIRP server where output files are written to. e.g., --useChirpServer voatlas92.cern.ch')
# internal parameters
optP.add_option('--panda_srvURL', action='store', dest='panda_srvURL', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_srcName', action='store', dest='panda_srcName', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_inDS', action='store', dest='panda_inDS', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_inDSForEP', action='store', dest='panda_inDSForEP', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_origFullExecString', action='store', dest='panda_origFullExecString', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_jobsetID',action='store',dest='panda_jobsetID',default=-1,
                type='int', help='internal parameter for jobsetID')
optP.add_option('--panda_dbRelease', action='store', dest='panda_dbRelease', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_exec', action='store', dest='panda_exec', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_bexec', action='store', dest='panda_bexec', default='',
                type='string', help='internal parameter')
optP.add_option('--panda_suppressMsg',action='store_const',const=True,dest='panda_suppressMsg',default=False,
                help='internal parameter')
optP.add_option('--requireLFC',action='store_const',const=True,dest='requireLFC',default=False,
                help='Require that LFC lookup is 100% successful (a single miss will fail the submission)')
optP.add_option('--load_xml',action='store',dest='load_xml',default=None,
                help='Expert mode: load complete submission configuration from an XML file ')
optP.add_option('--forceStaged',action='store_const',const=True,dest='forceStaged',default=False,
                help='Force files from primary DS to be staged to local disk, even if direct-access is possible')
optP.add_option('--forceStagedSecondary',action='store_const',const=True,dest='forceStagedSecondary',default=False,
                help='Force files from secondary DSs to be staged to local disk, even if direct-access is possible')

# parse options
options,args = optP.parse_args()
if options.verbose:
    print options

# display version
from pandatools import PandaToolsPkgInfo
if options.version:
    print "Version: %s" % PandaToolsPkgInfo.release_version
    sys.exit(0)

from pandatools import Client
from pandatools import PsubUtils
from pandatools import AthenaUtils
from pandatools import GlobalConfig
from pandatools import PLogger

# update panda-client
if options.update:
    res = PsubUtils.updatePackage(options.verbose)
    if res:
        sys.exit(0)
    else:
        sys.exit(1)

# full execution string
fullExecString = PsubUtils.convSysArgv()

# set dummy CMTSITE
if not os.environ.has_key('CMTSITE'):
    os.environ['CMTSITE'] = ''

# set grid source file
globalConf = GlobalConfig.getConfig()
if globalConf.grid_src != '' and not os.environ.has_key('PATHENA_GRID_SETUP_SH'):
    os.environ['PATHENA_GRID_SETUP_SH'] = globalConf.grid_src

# get logger
tmpLog = PLogger.getPandaLogger()

# use dev server
if options.devSrv:
    Client.useDevServer()
    
# set server
if options.panda_srvURL != '':
    Client.setServer(options.panda_srvURL)

# version check
PsubUtils.checkPandaClientVer(options.verbose)

# suffix for shadow dataset
suffixShadow = Client.suffixShadow

# load submission configuration from xml file (if provided)
xconfig = None
if options.load_xml:
    import parsexml
    xconfig = parsexml.dom_parser(options.load_xml)
    xconfig.dump(options.verbose)
    if options.outDS=='':
        options.outDS=xconfig.outDS()
    options.outputs='WILL_BE_REDEFINED_FOR_EACH_SUBJOB'
    options.jobParams='WILL_BE_REDEFINED_FOR_EACH_SUBJOB'
    options.inDS=xconfig.inDS()
    # inDS match and secondaryDS filter will be determined later from xconfig
    options.match=''
    options.secondaryDSs = xconfig.secondaryDSs_config(filter=False)

# exclude sites
if options.excludedSite != '':
    Client.excludeSite(options.excludedSite)

# use certain sites
useRandomCloud = []
if re.search(',',options.site) != None:
    options.site,useRandomCloud = Client.useCertainSites(options.site)

# set maxNFilesPerJob
PsubUtils.limit_maxNumInputs = options.maxNFilesPerJob 

# site specified
siteSpecified = True
if options.site == 'AUTO':
    siteSpecified = False

# use outputPath as outDS
if Client.isDQ2free(options.site):
    if options.outDS != '':
        options.outputPath = options.outDS
    else:
        options.outputPath = './'
    options.outDS = options.outputPath
else:
    # enforce to use output dataset container
    if not options.useOldStyleOutput and not options.outDS.endswith('/'):
        options.outDS = options.outDS + '/'

# keep original outDS
original_outDS_Name = options.outDS
     
# reset crossSite unless container is used for output 
if not original_outDS_Name.endswith('/'):
    options.crossSite = 0
    options.panda_jobsetID = None

usingContainerForOut = original_outDS_Name.endswith('/')

# set inDS for recursive goodRunListXML
if options.panda_inDS != '':
    options.inDS = options.panda_inDS
    options.goodRunListXML = ''

# set inDS for recursive event picking
if options.panda_inDSForEP != '':
    options.inDS = options.panda_inDSForEP
    options.eventPickEvtList = ''

# libDS
libds_file = '%s/libds_prun.dat' % os.environ['PANDA_CONFIG_ROOT']
if options.libDS == 'LAST':
    if not os.path.exists(libds_file):
        tmpLog.error("LAST cannot be used until you submit at least one job without --libDS")
        sys.exit(EC_Config)
    # read line
    tmpFile = open(libds_file)
    tmpLibDS = tmpFile.readline()
    tmpFile.close()
    # remove \n
    tmpLibDS = tmpLibDS.replace('\n','')
    # set
    options.libDS = tmpLibDS

# check DBRelease
if options.dbRelease != '' and (options.dbRelease.find(':') == -1 and options.dbRelease !='LATEST'):
    tmpLog.error("invalid argument for --dbRelease. Must be DatasetName:FileName or LATEST")  
    sys.exit(EC_Config)

# check unmerge dataset
PsubUtils.checkUnmergedDataset(options.inDS,options.secondaryDSs)

# Good Run List
if options.goodRunListXML != '' and options.inDS != '':
    tmpLog.error("cannnot use --goodRunListXML and --inDS at the same time")
    sys.exit(EC_Config)

# event picking
if options.eventPickEvtList != '' and options.inDS != '':
    tmpLog.error("cannnot use --eventPickEvtList and --inDS at the same time")
    sys.exit(EC_Config)

# param check for event picking
if options.eventPickEvtList != '':
    if options.eventPickDataType == '':
        tmpLog.error("--eventPickDataType must be specified")
        sys.exit(EC_Config)

# check writeInputToTxt
if options.writeInputToTxt != '':
    # remove %
    options.writeInputToTxt = options.writeInputToTxt.replace('%','')
    # loop over all StreamName:FileName
    for tmpItem in options.writeInputToTxt.split(','):
        tmpItems = tmpItem.split(':')
        if len(tmpItems) != 2:
            tmpLog.error("invalid StreamName:FileName in --writeInputToTxt : %s" % tmpItem)
            sys.exit(EC_Config)

# check grid-proxy
gridPassPhrase,vomsFQAN = PsubUtils.checkGridProxy('',False,options.verbose,options.vomsRoles)

# add allowed sites
if not siteSpecified:
    tmpSt = Client.addAllowedSites(options.verbose)
    if not tmpSt:
        tmpLog.error("Failed to get allowed site list")
        sys.exit(EC_Config)

# removed datasets
if options.removedDS == '':
    options.removedDS = []
else:
    options.removedDS = options.removedDS.split(',')

# convert in/outTarBall to full path
if options.inTarBall != '':
    options.inTarBall = os.path.abspath(os.path.expanduser(options.inTarBall))
if options.outTarBall != '':
    options.outTarBall = os.path.abspath(os.path.expanduser(options.outTarBall))

# save current dir
curDir = os.path.realpath(os.getcwd())

# check working dir
options.workDir = os.path.realpath(options.workDir)
if options.workDir != curDir and (not curDir.startswith(options.workDir+'/')):
    tmpLog.error("you need to run prun in a directory under %s" % options.workDir)
    sys.exit(EC_Config)

# run dir
runDir = '.'
if curDir != options.workDir:
    # remove special characters
    wDirString=re.sub('[\+]','.',options.workDir)
    runDir = re.sub('^'+wDirString+'/','',curDir)

# create tmp dir
if options.tmpDir == '':
    tmpDir = '%s/%s' % (curDir,commands.getoutput('uuidgen'))
else:
    tmpDir = '%s/%s' % (options.tmpDir,commands.getoutput('uuidgen'))    
os.makedirs(tmpDir)

# exit action
delFilesOnExit = []
def _onExit(dir,files):
    for tmpFile in files:
        commands.getoutput('rm -rf %s' % tmpFile)        
    commands.getoutput('rm -rf %s' % dir)
atexit.register(_onExit,tmpDir,delFilesOnExit)

# parse tag
athenaVer = ''
cacheVer  = ''
nightVer  = ''
groupArea = ''
if options.useAthenaPackages:
    # get Athena versions
    stA,retA = AthenaUtils.getAthenaVer()
    # failed
    if not stA:
        tmpLog.error("You need to setup Athena runtime to use --useAthenaPackages")
        sys.exit(EC_CMT)
    workArea  = retA['workArea'] 
    athenaVer = 'Atlas-%s' % retA['athenaVer'] 
    groupArea = retA['groupArea'] 
    cacheVer  = retA['cacheVer'] 
    nightVer  = retA['nightVer']
    # override run directory
    sString=re.sub('[\+]','.',workArea)
    runDir = re.sub('^%s' % sString, '', curDir)
    if runDir == curDir:
        tmpLog.error("you need to run prun in a directory under %s" % workArea)
        sys.exit(EC_Config)
    elif runDir == '':
        runDir = '.'
    elif runDir.startswith('/'):
        runDir = runDir[1:]
    runDir = runDir+'/'
elif options.athenaTag != '':
    # get list of Athena projects
    listProjects = Client.getCachePrefixes(options.verbose)
    items = options.athenaTag.split(',')
    for item in items:
        # releases
        match = re.search('^(\d+\.\d+\.\d+)',item)
        if match != None:
            athenaVer = 'Atlas-%s' % match.group(1)
            # cache
            cmatch = re.search('^(\d+\.\d+\.\d+\.\d+\.*\d*)$',item)
            if cmatch != None:
                cacheVer += '_%s' % cmatch.group(1)
        # project
        if item.startswith('Atlas') or item in listProjects:
            # ignore AtlasOffline
            if item in ['AtlasOffline']:
                continue
            cacheVer = '-'+item+cacheVer
        # nightlies    
        if item.startswith('rel_'):
            if 'dev' in items:
                nightVer  = '/dev'
            elif 'bugfix' in items:
                nightVer  = '/bugfix'
            else:
                tmpLog.error("unsupported nightly %s" % line)
                sys.exit(EC_Config)
    # check cache
    if re.search('^-.+_.+$',cacheVer) == None:
        if re.search('^_\d+\.\d+\.\d+\.\d+$',cacheVer) != None:
            # use AtlasProduction
            cacheVer = '-AtlasProduction'+cacheVer
        else:
            # unknown
            cacheVer = ''

# good run list
if options.goodRunListXML != '':
    # look for pyAMI
    status,options.inDS,tmpAmiFileList = AthenaUtils.convertGoodRunListXMLtoDS(options.goodRunListXML,
                                                                               options.goodRunDataType,
                                                                               options.goodRunProdStep,
                                                                               options.goodRunListDS,
                                                                               options.verbose)
    if not status:
        tmpLog.error("failed to convert GoodRunListXML")
        sys.exit(EC_Config)
    if options.inDS == '':
        tmpLog.error("no datasets were extracted from AMI using %s" % options.goodRunListXML)
        sys.exit(EC_Config)
    # write input files   
    if options.inputFileListName == '':    
        options.inputFileListName = '%s/ami_%s.dat' % (curDir,commands.getoutput('uuidgen'))
        evFH = open(options.inputFileListName,'w') 
        for tmpLFN in tmpAmiFileList:
            evFH.write('%s\n' % tmpLFN)
        # close        
        evFH.close()
        # add to be deleted on exit
        delFilesOnExit.append(options.inputFileListName)

# event picking
if options.eventPickEvtList != '':
    # convert run/evt list to dataset/LFN list
    epDsLFNs,epGuidEvtMap = PsubUtils.getDSsFilesByRunsEvents(curDir,
                                                              options.eventPickEvtList,
                                                              options.eventPickDataType,
                                                              options.eventPickStreamName,
                                                              options.eventPickDS,
                                                              options.verbose)
    # set param
    options.inDS = ''
    options.filelist = []
    tmpDsNameList = []
    tmpLFNList = []    
    options.inputFileListName = '%s/ep_%s.dat' % (curDir,commands.getoutput('uuidgen'))
    evFH = open(options.inputFileListName,'w') 
    for tmpGUID,tmpDsLFNs in epDsLFNs.iteritems():
        tmpDsName,tmpLFN = tmpDsLFNs
        # set fileFist
        if not tmpLFN in tmpLFNList:
            evFH.write('%s\n' % tmpLFN)
        # set inDS    
        if not tmpDsName in tmpDsNameList:
            tmpDsNameList.append(tmpDsName)
            options.inDS += '%s,' % tmpDsName
    options.inDS = options.inDS[:-1]
    # close        
    evFH.close()
    # add to be deleted on exit
    delFilesOnExit.append(options.inputFileListName)

# additinal files
if options.extFile == '':
    options.extFile = []
else:
    tmpItems = options.extFile.split(',')
    options.extFile = []
    # convert * to .*
    for tmpItem in tmpItems:
        options.extFile.append(tmpItem.replace('*','.*'))

# glue packages
options.gluePackages = options.gluePackages.split(',')
try:
    options.gluePackages.remove('')
except:
    pass

# set excludeFile
if options.excludeFile != '':
    AthenaUtils.setExcludeFile(options.excludeFile)

# LFN matching
if options.match != '':
    # convert . to \.
    options.match = options.match.replace('.','\.')
    # convert * to .*
    options.match = options.match.replace('*','.*')

# LFN anti-matching
if options.antiMatch != '':
    # convert . to \.
    options.antiMatch = options.antiMatch.replace('.','\.')
    # convert * to .*
    options.antiMatch = options.antiMatch.replace('*','.*')

# get job script
jobScript = ''
if options.panda_bexec != '':
    options.bexec = urllib.unquote(options.panda_bexec)
if options.panda_exec != '':
    options.jobParams = urllib.unquote(options.panda_exec)
if options.jobParams == '':
    tmpLog.error("you need to give --exec\n  prun [--inDS inputdataset] --outDS outputdataset --exec 'myScript arg1 arg2 ...'")
    sys.exit(EC_Config)
orig_execStr  = options.jobParams
orig_bexecStr = options.bexec

# replace : to = for backward compatibility
for optArg in ['RNDM']:
    options.jobParams = re.sub('%'+optArg+':','%'+optArg+'=',options.jobParams)

# check output dataset
if options.outDS == '':
    tmpLog.error("no outDS is given\n  prun [--inDS inputdataset] --outDS outputdataset --exec 'myScript arg1 arg2 ...'")
    sys.exit(EC_Config)

# secondary datasets
if options.secondaryDSs != '':
    if options.inDS == '':
        tmpLog.error("Primary input dataset is not defined using --inDS although --secondaryDSs is set")
        sys.exit(EC_Config)
    # parse
    tmpMap = {}
    for tmpItem in options.secondaryDSs.split(','):
        tmpItems = tmpItem.split(':')
        if len(tmpItems) in [3,4,5]:
            tmpDsName = tmpItems[2]
            # change ^ to ,
            tmpDsName = tmpDsName.replace('^',',')
            # make map
            tmpMap[tmpDsName] = {'nFiles'     : int(tmpItems[1]),
                                 'streamName' : tmpItems[0],
                                 'pattern'    : '',
                                 'nSkip'      : 0,
                                 'files'      : []}
            # using filtering pattern
            if len(tmpItems) >= 4:
                tmpMap[tmpItems[2]]['pattern'] = tmpItems[3]
            # nSkip
            if len(tmpItems) >= 5:
                tmpMap[tmpItems[2]]['nSkip'] = int(tmpItems[4])
        else:
            tmpLog.error("Wrong format %s in --secondaryDSs. Must be StreamName:nFilesPerJob:DatasetName[:Pattern[:nSkipFiles]]" \
                         % tmpItem)
            sys.exit(EC_Config)
    # set
    options.secondaryDSs = tmpMap
else:
    options.secondaryDSs = {}

# correct site
if options.site != 'AUTO':
    origSite = options.site
    # patch for BNL
    if options.site in ['BNL',"ANALY_BNL"]:
        options.site = "ANALY_BNL_ATLAS_1"
    # patch for CERN
    if options.site in ['CERN']:
        options.site = "ANALY_CERN"        
    # try to convert DQ2ID to PandaID
    pID = PsubUtils.convertDQ2toPandaID(options.site)
    if pID != '':
        options.site = pID
    # add ANALY
    if not options.site.startswith('ANALY_'):
        options.site = 'ANALY_%s' % options.site
    # check
    if not Client.PandaSites.has_key(options.site):
        tmpLog.error("unknown siteID:%s" % origSite)
        sys.exit(EC_Config)
    # set cloud
    options.cloud = Client.PandaSites[options.site]['cloud']

# get the latest of DBRelease
if options.dbRelease == 'LATEST' or '%DB:LATEST' in options.jobParams:
    if options.panda_dbRelease == '':
        options.dbRelease = Client.getLatestDBRelease(options.verbose)
        options.panda_dbRelease = options.dbRelease
    else:
        options.dbRelease = options.panda_dbRelease
        
# get DN
distinguishedName = PsubUtils.getDN()

# get nickname
nickName = PsubUtils.getNickname()

if distinguishedName == '' and nickName == '':
    sys.exit(EC_Config)

# check outDS format
if not PsubUtils.checkOutDsName(options.outDS,distinguishedName,options.official,nickName,options.site,vomsFQAN):
    tmpLog.error("invalid output datasetname:%s" % options.outDS)
    sys.exit(EC_Config)

# check destSE
if options.destSE != '':
    if not PsubUtils.checkDestSE(options.destSE,options.outDS,options.verbose):
        sys.exit(EC_Config)

# check if output dataset already exists
outputDSexist = False
outputContExist = False
if not Client.isDQ2free(options.site):
    tmpDatasets = Client.getDatasets(options.outDS,options.verbose)
    if len(tmpDatasets) != 0:
        if original_outDS_Name.endswith('/'):
            outputContExist = True
        else:
            outputDSexist = True
        # get real datasetname for case sensitivity    
        options.outDS = PsubUtils.getRealDatasetName(options.outDS,tmpDatasets)

# check if shadow dataset exists
shadowDSexist = False
if not Client.isDQ2free(options.site):
    tmpDatasets = Client.getDatasets("%s%s" % (options.outDS,suffixShadow),options.verbose)
    if len(tmpDatasets) != 0:
        shadowDSexist = True

# set site/cloud if output/lib dataset already exists
checkLibDS = True
if outputDSexist and options.destSE == '':
    checkLibDS = False
    if options.verbose:
        tmpLog.debug("get locations for outDS:%s" % options.outDS)
    # get locations for outDS    
    outDSlocations = Client.getLocations(options.outDS,[],'',True,options.verbose)
    if outDSlocations == []:
        tmpLog.error("cannot find locations for existing output datasete:%s" % options.outDS)
        sys.exit(EC_Config)
    # keep origianl site
    if Client.PandaSites.has_key(options.site) and \
       (Client.PandaSites[options.site]['ddm'] in outDSlocations or \
        Client.convSrmV2ID(Client.PandaSites[options.site]['ddm']) in outDSlocations):
        pass
    else:
        # convert DQ2ID to Panda siteID
        tmpConvIDs = []
        for outDSlocation in outDSlocations:
            tmpConvIDs += Client.convertDQ2toPandaIDList(outDSlocation)
        # not found
        if tmpConvIDs == []:
            checkLibDS = True
            options.destSE = outDSlocations[0]
            tmpLog.info("set destSE:%s because outDS:%s already exists there" % \
                        (options.destSE,options.outDS))
	# chose one
        if len(tmpConvIDs) == 1:
            convID = tmpConvIDs[0]
        else:
            # run brokerage to be sent to free site
            convID = PsubUtils.runBrokerageForCompSite(tmpConvIDs,athenaVer,cacheVer,options.verbose)
        # set    
        options.site  = convID
        options.cloud = Client.PandaSites[convID]['cloud']
        tmpLog.info("set site:%s cloud:%s because outDS:%s already exists at %s" % \
                    (options.site,options.cloud,options.outDS,outDSlocations))                

if options.libDS != '' and not Client.isDQ2free(options.site):
    # get real datasetname for case sensitivity    
    tmpDatasets = Client.getDatasets(options.libDS,options.verbose)
    options.libDS = PsubUtils.getRealDatasetName(options.libDS,tmpDatasets)
    if options.verbose:
        tmpLog.debug("get locations for libDS:%s" % options.libDS)
    # get locations for libDS    
    libDSlocations = Client.getLocations(options.libDS,[],'',True,options.verbose)
    if libDSlocations == []:
        tmpLog.error("cannot find locations for existing lib datasete:%s" % options.libDS)
        sys.exit(EC_Config)
    # check consistency with outDS location    
    if not checkLibDS:
        PsubUtils.checkLocationConsistency(outDSlocations,libDSlocations)
    # keep origianl site
    if Client.PandaSites.has_key(options.site) and \
       (Client.PandaSites[options.site]['ddm'] in libDSlocations or \
        Client.convSrmV2ID(Client.PandaSites[options.site]['ddm']) in libDSlocations):
        pass
    else:
        # convert DQ2ID to Panda siteID
        tmpConvIDs = []
        for libDSlocation in libDSlocations:
            tmpConvIDs += Client.convertDQ2toPandaIDList(libDSlocation)
        # not found
        if tmpConvIDs == []:
            tmpLog.error("cannot find supported sites for existing lib datasete:%s" % options.libDS)
            sys.exit(EC_Config)
	# chose one
        if len(tmpConvIDs) == 1:
            convID = tmpConvIDs[0]
        else:
            # run brokerage to be sent to free site
            convID = PsubUtils.runBrokerageForCompSite(tmpConvIDs,athenaVer,cacheVer,options.verbose)
        # set    
        options.site  = convID
        options.cloud = Client.PandaSites[convID]['cloud']
        tmpLog.info("set site:%s cloud:%s because libDS:%s exists at %s" % \
                    (options.site,options.cloud,options.libDS,libDSlocations))                
        
# set cloud for brokerage
expCloudFlag = False
if options.cloud == None:
    options.cloud = PsubUtils.getCloudUsingFQAN(defaultCloud,options.verbose,useRandomCloud)
else:
    # use cloud explicitly    
    expCloudFlag = True
if not PsubUtils.checkValidCloud(options.cloud):
    tmpLog.error("unsupported cloud:%s" % options.cloud)
    sys.exit(EC_Config)


# get DB datasets
dbrFiles  = {}
dbrDsList = []
dbrDsSize = 0
if options.dbRelease != '':
    tmpDbrDS,tmpDbrLFN  = options.dbRelease.split(':')
    # get files in the dataset
    if not tmpDbrDS in dbrDsList:
        tmpVerboseFlag = options.crossSite in [0,maxCrossSite] or options.verbose
        if tmpVerboseFlag and not options.panda_suppressMsg:
            tmpLog.info("query files in %s" % tmpDbrDS)
        tmpList = Client.queryFilesInDataset(tmpDbrDS,options.verbose)
        # append
        dbrDsList.append(tmpDbrDS)
        for tmpLFN,tmpVal in tmpList.iteritems():
            dbrFiles[tmpLFN] = tmpVal
    # check
    if not dbrFiles.has_key(tmpDbrLFN):
        tmpLog.error("%s is not in %s" % (tmpDbrLFN,tmpDbrDS))
        sys.exit(EC_Dataset)
    # DBR size                                                                                                                          
    dbrDsSize += long(dbrFiles[tmpDbrLFN]['fsize'])

# get files in input dataset
inputGroupFilesMap = {}
if options.inDS != '':
    # query files in shadow dataset
    shadowList = []
    if shadowDSexist and not outputContExist:
        shadowList = Client.getFilesInShadowDatasetOld(options.outDS,suffixShadow,options.verbose)
    elif outputContExist:
        shadowList = Client.getFilesInShadowDataset(original_outDS_Name,suffixShadow,options.verbose)
    # query files in dataset
    recursiveFlag = True
    if options.crossSite in [0,maxCrossSite]:
        recursiveFlag = False
    inputFileMap,inputDsString = Client.getFilesInDatasetWithFilter(options.inDS,options.match,shadowList,options.inputFileListName,
                                                                    options.verbose,dsStringFlag=True,isRecursive=recursiveFlag,
                                                                    antiFilter=options.antiMatch)
    # empty
    if inputFileMap == {} and shadowList == []:
        errStr = "No files were found in %s (log.tgz was ignored). " % options.inDS
        if options.match != '':
            errStr += "Make sure that you specify a correct string in --match"
            if options.antiMatch != '':
                errStr += " and/or --antiMatch"
        else:
            errStr += "The dataset is empty"
            if options.antiMatch != '':
                errStr += " or --antiMatch is wrong"
        tmpLog.error(errStr)
        sys.exit(EC_Config)
    # if list of files was given in the xml file, check that they are all present:
    if options.load_xml:
        required_files = xconfig.files_in_DS(options.inDS)
        assert set(required_files)<=set(inputFileMap.keys()),'ERROR: xml config requires files that are missing in inDS=%s'%options.inDS
        for key in inputFileMap.keys():
            if key not in required_files:
                del inputFileMap[key]
    # check if all files were used
    unUsedFlag = False
    for tmpLFN in inputFileMap.keys():
        if not tmpLFN in shadowList:
            unUsedFlag = True
            break
    if not unUsedFlag:
        # exit
        tmpMsg  = "Done. No jobs to be submitted since all input files were (or are being) processed for the outDS. "
        tmpMsg += "The intention is to avoid duplication in the outDS. " 
        tmpMsg += "If you need to run again on the input dataset please use new outDS"
        tmpLog.info(tmpMsg)
        sys.exit(0)
    # sort
    inputFileList = inputFileMap.keys()
    inputFileList.sort()
    missList = []
    # query files in secondary datasets
    for tmpDsName,tmpValMap in options.secondaryDSs.iteritems():
        tmpSecFileMap = Client.getFilesInDatasetWithFilter(tmpDsName,tmpValMap['pattern'],shadowList,'',options.verbose)
        # if list of files was given in the xml file, check that they are all present:
        if options.load_xml:
            required_files = xconfig.files_in_DS(tmpDsName)
            assert set(required_files)<=set(tmpSecFileMap.keys()),'ERROR: xml config requires files that are missing in secondaryDS=%s'%tmpDsName
            for key in tmpSecFileMap.keys():
                if key not in required_files:
                    del tmpSecFileMap[key]
        # append
        options.secondaryDSs[tmpDsName]['fileMap'] = tmpSecFileMap
    # get locations
    if inputFileList != []:
        if options.site == "AUTO":
            if inputDsString == '':
                tmpDsForLookUpList = [options.inDS] + options.secondaryDSs.keys()
            else:
                tmpDsForLookUpList = [inputDsString] + options.secondaryDSs.keys()
            tmpDsForLookUpList += dbrDsList    
            tmpDsLocationForTest = {}
            dsLocationMap = None
            dsLocationMapBack = None
            for tmpDsForLookUp in tmpDsForLookUpList:
                tmpDsLocationMap,tmpDsLocationMapBack,dsTapeSites,tmpDsUsedDsMap = \
                                                                                 Client.getLocations(tmpDsForLookUp,[],options.cloud,False,
                                                                                                     options.verbose,expCloud=expCloudFlag,
                                                                                                     getReserved=True,getTapeSites=True,
                                                                                                     removeDS=True,
                                                                                                     removedDatasets=options.removedDS,
                                                                                                     useOutContainer=usingContainerForOut)
                # use UsedDsMap of primary dataset
                if tmpDsForLookUp == tmpDsForLookUpList[0]:
                    dsUsedDsMap = tmpDsUsedDsMap
                # no location
                if tmpDsLocationMap == {} and tmpDsLocationMapBack == {}:
                    if dsTapeSites != []:
                        tmpLog.error("Tape sites %s hold the dataset. Please request subscription to disk area first if needed" % dsTapeSites)
                    if options.crossSite in [0,maxCrossSite] or options.verbose:
                        if expCloudFlag:
                            tmpLog.error("could not find supported/online sites in the %s cloud for %s" % (options.cloud,tmpDsForLookUp))
                        else:
                            tmpLog.error("could not find supported/online sites for %s" % tmpDsForLookUp)
                    else:
                        errStr = "could not find supported/online sites for the missing files"
                        tmpLog.error(errStr)
                    sys.exit(EC_Dataset)
                # append
                tmpDsLocationForTest[tmpDsForLookUp] = {'pri':tmpDsLocationMap,
                                                        'sec':tmpDsLocationMapBack}
                # set initial value
                if dsLocationMap == None:
                    dsLocationMap = copy.deepcopy(tmpDsLocationMap)
                    dsLocationMapBack = copy.deepcopy(tmpDsLocationMapBack)
            # choose common locations
            if len(tmpDsLocationForTest) > 1:
                # primary sites
                for tmpDsForLookUp,tmpVal in tmpDsLocationForTest.iteritems():
                    tmpDsLocationMap = tmpVal['pri'] 
                    tmpDsLocationList = dsLocationMap.keys()
                    for tmpDsLocation in tmpDsLocationList:
                        if not tmpDsLocationMap.has_key(tmpDsLocation):
                            del dsLocationMap[tmpDsLocation]
                # secondary sites
                for tmpDsForLookUp,tmpVal in tmpDsLocationForTest.iteritems():
                    tmpDsLocationMap = tmpVal['sec'] 
                    tmpDsLocationList = dsLocationMapBack.keys()
                    for tmpDsLocation in tmpDsLocationList:
                        if not tmpDsLocationMap.has_key(tmpDsLocation):
                            del dsLocationMapBack[tmpDsLocation]
                # check
                if dsLocationMap == {} and dsLocationMapBack == {}:
                    errStr = "Could not find common sites for input datasets\n\n"
                    for tmpDsForLookUp,tmpVal in tmpDsLocationForTest.iteritems():
                        tmpSiteStr = ''
                        for tmpDsLocationMap in [tmpVal['pri'],tmpVal['sec']]:
                            for tmpSiteList in tmpDsLocationMap.values():
                                tmpSiteStr += "%s," % tmpSiteList[0]
                        tmpSiteStr = tmpSiteStr[:-1]   
                        errStr += "   %s\n      exists at %s\n" % (tmpDsForLookUp,tmpSiteStr)
                    errStr += "\nYou need to request DDM subscription to aggregate them to a site"    
                    tmpLog.error(errStr)
                    sys.exit(EC_Dataset)
            # run brokerage
            tmpDsLocationMapList = [dsLocationMap]
            if dsLocationMapBack != {}:
                tmpDsLocationMapList.append(dsLocationMapBack)
            tmpBrokerErr = ''   
            for idxDsLocationMap,tmpDsLocationMap in enumerate(tmpDsLocationMapList):    
                tmpSites = []
                for tmpItems in tmpDsLocationMap.values():
                    for tmpItem in tmpItems:
                        # convert to long
                        if options.long:
                            tmpItem = Client.convertToLong(tmpItem)
                        tmpSites.append(tmpItem)
                if tmpSites == []:
                    continue
                status,out = Client.runBrokerage(tmpSites,athenaVer,verbose=options.verbose,trustIS=True,cacheVer=cacheVer)
                if status != 0:
                    tmpLog.error('failed to run brokerage for automatic assignment: %s' % out)
                    sys.exit(EC_Config)
                if out.startswith('ERROR :'):
                    if idxDsLocationMap == 0:
                        tmpBrokerErr += out
                        if expCloudFlag:
                            tmpBrokerErr += " Could not find sites in the %s cloud.\n" % options.cloud
                        else:
                            tmpBrokerErr += " Could not find sites\n"
                    else:
                        if tmpBrokerErr != '':
                            tmpBrokerErr += out
                            tmpBrokerErr += " Could not find sites in other clouds.\n"
                        else:
                            tmpBrokerErr += out
                            tmpBrokerErr += " Could not find sites.\n"
                    if idxDsLocationMap+1 >= len(tmpDsLocationMapList):
                        tmpLog.error('brokerage failed')
                        print tmpBrokerErr[:-1]
                        sys.exit(EC_Config)
                    continue
                if not Client.PandaSites.has_key(out):
                    tmpLog.error('brokerage gave wrong PandaSiteID:%s' % out)
                    sys.exit(EC_Config)
                break    
            # set site/cloud
            options.site  = out
            options.cloud = Client.PandaSites[options.site]['cloud']
	    # get expiring files
            expFilesMap = Client.getExpiringFiles(options.inDS,options.removedDS,options.site,options.verbose)
            # update used the list of used datasets
            for tmpDsUsedDsMapKey,tmpDsUsedDsVal in dsUsedDsMap.iteritems():
                if options.site in [tmpDsUsedDsMapKey,Client.convertToLong(tmpDsUsedDsMapKey)]:
                    for tmpDsUsedDsValItem in tmpDsUsedDsVal:
                        # ignore expiring datasets
                        if not tmpDsUsedDsValItem in expFilesMap['datasets']:
                            options.removedDS.append(tmpDsUsedDsValItem)
                    break
        # scan local replica catalog
        dsLocation = Client.PandaSites[options.site]['ddm']
        if options.skipScan:
            # skip LFC scan
            missList = []
        elif Client.getLFC(dsLocation) != None:
            tmpLog.info("scanning LFC %s for %s" % (Client.getLFC(dsLocation),options.site))
            # LFC
            if options.nFiles == 0 and options.nSkipFiles != 0:
                missList = Client.getMissLFNsFromLFC(inputFileMap,options.site,True,options.verbose,
                                                     dsStr=options.inDS,removedDS=options.removedDS)
            else:
                missList = Client.getMissLFNsFromLFC(inputFileMap,options.site,True,options.verbose,
                                                     options.nFiles+options.nSkipFiles,
                                                     dsStr=options.inDS,removedDS=options.removedDS)
        elif Client.getLRC(dsLocation) != None:
            tmpLog.info("scanning LRC %s for %s" % (Client.getLRC(dsLocation),options.site))
            # LRC
            missList = Client.getMissLFNsFromLRC(inputFileMap,Client.getLRC(dsLocation),options.verbose)
        else:
            missList = []
        if options.verbose:
            tmpLog.debug("%s holds %s files" % (options.site,len(inputFileList)-len(missList)))
        if options.requireLFC:
            assert missList==[],'ERROR: LFC scan revealed missing file(s) in inDS=%s on site %s: %s'%(options.inDS,options.site,' '.join(missList))
        # no files available
        if len(inputFileList) == len(missList):
            tmpStr = "No files available on disk at %s" % options.site
            if options.crossSite == 0:
                tmpLog.error(tmpStr)
                sys.exit(EC_Dataset)
            else:
                tmpLog.info(tmpStr)
        # remove missing files
        newInputFiles = []
        for inputFile in inputFileList:
            if not inputFile in missList:
                newInputFiles.append(inputFile)
        inputFileList = newInputFiles
        # check for secondary datasets
        for tmpDsName,tmpDsValMap in options.secondaryDSs.iteritems():
            if not options.skipScan:
                tmpLog.info("scanning LFC for %s" % tmpDsName)
                if tmpDsValMap['nFiles'] == 0 and tmpDsValMap['nSkip'] != 0:
                    secMissList = Client.getMissLFNsFromLFC(tmpDsValMap['fileMap'],options.site,True,options.verbose,
                                                            dsStr=tmpDsName)
                else:
                    secMissList = Client.getMissLFNsFromLFC(tmpDsValMap['fileMap'],options.site,True,options.verbose,
                                                            tmpDsValMap['nFiles']+tmpDsValMap['nSkip'],shadowList,
                                                            dsStr=tmpDsName)
            else:
                secMissList = []
            if options.requireLFC:
                assert secMissList==[],'ERROR: LFC scan revealed missing file(s) in %s on site %s: %s'%(tmpDsName,options.site,' '.join(secMissList))
            # remove missing
            tmpSecFileList = tmpDsValMap['fileMap'].keys()
            tmpSecFileList.sort()
            options.secondaryDSs[tmpDsName]['files'] = []
            for tmpSecFile in tmpSecFileList:
                if tmpSecFile in secMissList:
                    continue
                # append
                options.secondaryDSs[tmpDsName]['fileMap'][tmpSecFile]['LFN'] = tmpSecFile
                options.secondaryDSs[tmpDsName]['files'].append(options.secondaryDSs[tmpDsName]['fileMap'][tmpSecFile])
            # skip files    
            options.secondaryDSs[tmpDsName]['files'] = options.secondaryDSs[tmpDsName]['files'][options.secondaryDSs[tmpDsName]['nSkip']:]
            # change nFiles=0 to the real size 
            if options.secondaryDSs[tmpDsName]['nFiles'] == 0:
                options.secondaryDSs[tmpDsName]['nFiles'] = len(options.secondaryDSs[tmpDsName]['files'])
    # update shadow just in case since LFC scan may take long time
    if not options.skipScan:
        shadowUpdated = False
        if shadowDSexist and not outputContExist:
            # using += just in case
            shadowList += Client.getFilesInShadowDatasetOld(options.outDS,suffixShadow,options.verbose)
            shadowUpdated = True
        elif outputContExist:
            # using += just in case            
            shadowList += Client.getFilesInShadowDataset(original_outDS_Name,suffixShadow,options.verbose)
            shadowUpdated = True
        # remove
        if shadowUpdated:
            tmpInputList = []
            for tmpFileName in inputFileList:
                if not tmpFileName in shadowList:
                    tmpInputList.append(tmpFileName)
            inputFileList = tmpInputList        
    # truncate files
    inputFileList = inputFileList[options.nSkipFiles:]
    # no available input files
    if inputFileList == []:
        if options.site == 'AUTO':
            # no lookup was made
            tmpStr  = "all input files had already been used"
        else:
            tmpStr  = "all input files at %s had already been used" % options.site
        tmpStr += "\n  prun runs on files which were failed or were not used in\n"
        tmpStr += "  previous submissions if it runs with the same inDS and outDS"
        if options.crossSite == 0:
            tmpLog.error(tmpStr)
            sys.exit(EC_Dataset)
        else:
            # go back to current dir
            os.chdir(curDir)
            # try another site if input files remain
            options.crossSite -= 1
            if options.crossSite > 0 and options.inDS != '' and not siteSpecified:
                if missList != []:
                    PsubUtils.runPrunRec(missList,tmpDir,fullExecString,options.nFiles,inputFileMap,
                                         options.site,options.crossSite,"",options.removedDS,
					 options.inDS,options.goodRunListXML,options.eventPickEvtList,
                                         options.panda_dbRelease,options.panda_jobsetID,
                                         orig_bexecStr,orig_execStr,options.verbose)
            # succeeded
            sys.exit(0)
    # truncate
    if options.nFiles != 0:
        inputFileList = inputFileList[:options.nFiles]
    # group files by dataset
    if options.useContElementBoundary:
        tmpLog.info("checking boundaries of container elements")
        inputGroupFilesMap = PsubUtils.groupFilesByDataset(options.inDS,inputDsString,
                                                           inputFileList,options.verbose)
else:
    # set cloud for no input dataset
    if options.site == "AUTO":
	# get sites belonging to a cloud and others
        tmpPriSites = []
        tmpSecSites = []
        for tmpID,spec in Client.PandaSites.iteritems():
            if spec['status']=='online':
		# exclude long,xrootd,local queues
                if Client.isExcudedSite(tmpID):
                    continue
                # convert to long
                if options.long:
                    tmpID = Client.convertToLong(tmpID)
                # check cloud if it is specified
                if spec['cloud']==options.cloud or not expCloudFlag:
                    tmpPriSites.append(tmpID)
                elif not expCloudFlag:
                    tmpSecSites.append(tmpID)
        tmpSitesList = [tmpPriSites]
        if not expCloudFlag:
            tmpSitesList.append(tmpSecSites)
        # run brokerage
        tmpBrokerErr = ''
        for idxTmpSites,tmpSites in enumerate(tmpSitesList):
            status,out = Client.runBrokerage(tmpSites,athenaVer,verbose=options.verbose,trustIS=True,cacheVer=cacheVer)    
            if status != 0:
                tmpLog.error('failed to run brokerage for automatic assignment: %s' % out)  
                sys.exit(EC_Config)
            if out.startswith('ERROR :'):
                tmpBrokerErr += out
                if idxTmpSites == 0:
                    tmpBrokerErr += " Could not find sites in the %s cloud.\n" % options.cloud
                else:
                    tmpBrokerErr += " Could not find sites in other clouds.\n"
                if idxTmpSites+1 >= len(tmpSitesList):
                    tmpLog.error('brokerage failed')
                    print tmpBrokerErr[:-1]
                    sys.exit(EC_Config)
                continue    
            if not Client.PandaSites.has_key(out):
                tmpLog.error('brokerage gave wrong PandaSiteID:%s' % out)
                sys.exit(EC_Config)
            break
        # set site
        options.site = out

# reset destSE if it is redundant
tmpOutDsLocation = Client.PandaSites[options.site]['ddm']
if options.spaceToken != '':
    if Client.PandaSites[options.site]['setokens'].has_key(options.spaceToken):
        tmpOutDsLocation = Client.PandaSites[options.site]['setokens'][options.spaceToken]
if options.destSE == tmpOutDsLocation:
    options.destSE = ''

# long queue
if options.long and not options.site.startswith('ANALY_LONG_'):
    options.site = Client.convertToLong(options.site)

#####################################################################
# archive sources and send it to HTTP-reachable location

# create archive
if options.panda_srcName != '':
    # reuse src
    if options.verbose:    
        tmpLog.debug('reuse source files')
    archiveName = options.panda_srcName
    # go to tmp dir
    os.chdir(tmpDir)
else:
    if options.inTarBall == '':
        if options.useAthenaPackages:
            # gather Athena packages

            # set extFile
            AthenaUtils.setExtFile(options.extFile)
            if options.libDS == '' and not options.nobuild:
                # archive sources
                archiveName,archiveFullName = AthenaUtils.archiveSourceFiles(workArea,runDir,curDir,tmpDir,
                                                                         options.verbose,options.gluePackages)
            else:
                # archive jobO
                archiveName,archiveFullName = AthenaUtils.archiveJobOFiles(workArea,runDir,curDir,
                                                                           tmpDir,options.verbose)
            # archive InstallArea
            if options.libDS == '':
                AthenaUtils.archiveInstallArea(workArea,groupArea,archiveName,archiveFullName,
                                               tmpDir,options.nobuild,options.verbose)
        else:        
            # gather normal files

            # go to workdir
            os.chdir(options.workDir)

            # gather files under work dir
            tmpLog.info("gathering files under %s" % options.workDir)

            # get files in the working dir
            skippedExt   = ['.o','.a','.so']
            skippedFlag  = False
            workDirFiles = []
            for tmpRoot,tmpDirs,tmpFiles in os.walk('.'):
                emptyFlag    = True
                for tmpFile in tmpFiles:
                    tmpPath = '%s/%s' % (tmpRoot,tmpFile)
                    # get size
                    try:
                        size = os.path.getsize(tmpPath)
                    except:
                        # skip dead symlink
                        if options.verbose:
                            type,value,traceBack = sys.exc_info()
                            print "  Ignore : %s:%s" % (type,value)
                        continue
                    # check exclude files
                    excludeFileFlag = False
                    for tmpPatt in AthenaUtils.excludeFile:
                        if re.search(tmpPatt,tmpPath) != None:
                            excludeFileFlag = True
                            break
                    if excludeFileFlag:
                        continue
                    # skipped extension
                    isSkippedExt = False
                    for tmpExt in skippedExt:
                        if tmpPath.endswith(tmpExt):
                            isSkippedExt = True
                            break
                    # check root
                    isRoot = False
                    if re.search('\.root(\.\d+)*$',tmpPath) != None:
                        isRoot = True
                    # extra files
                    isExtra = False
                    for tmpExt in options.extFile:
                        if re.search(tmpExt+'$',tmpPath) != None:
                            isExtra = True
                            break
                    # regular files
                    if not isExtra:
                        # unset emptyFlag even if all files are skipped
                        emptyFlag = False
                        # skipped extensions
                        if isSkippedExt:
                            print "  skip %s %s" % (str(skippedExt),tmpPath)
                            skippedFlag = True
                            continue
                        # skip root
                        if isRoot:
                            print "  skip root file %s" % tmpPath
                            skippedFlag = True
                            continue
                        # check size
                        if size > options.maxFileSize:
                            print "  skip large file %s:%sB>%sB" % (tmpPath,size,options.maxFileSize)
                            skippedFlag = True
                            continue

                    # remove ./
                    tmpPath = re.sub('^\./','',tmpPath)
                    # append
                    workDirFiles.append(tmpPath)
                    emptyFlag = False
                # add empty directory		
                if emptyFlag and tmpDirs==[] and tmpFiles==[]:
                    tmpPath = re.sub('^\./','',tmpRoot)
                    # check exclude pattern
                    excludePatFlag = False
                    for tmpPatt in AthenaUtils.excludeFile:
                        if re.search(tmpPatt,tmpPath) != None:
                            excludePatFlag = True
                            break
                    if excludePatFlag:
                        continue
                    # skip tmpDir
                    if tmpPath.split('/')[-1] == tmpDir.split('/')[-1]:
                        continue
                    # append
                    workDirFiles.append(tmpPath)
            if skippedFlag:
                tmpLog.info("please use --maxFileSize or --extFile if you need to send the skipped files to WNs")

            # create archive
            if options.libDS != '' or options.nobuild:
                # use 'jobO' for libDS/noBuild
                archiveName     = 'jobO.%s.tar' % commands.getoutput('uuidgen')
            else:
                # use 'sources' for normal build
                archiveName     = 'sources.%s.tar' % commands.getoutput('uuidgen')
            archiveFullName = "%s/%s" % (tmpDir,archiveName)

            # collect files
            for tmpFile in workDirFiles:
                if os.path.islink(tmpFile):
                    status,out = commands.getstatusoutput('tar -rh %s -f %s' % (tmpFile,archiveFullName))
                else:
                    status,out = commands.getstatusoutput('tar rf %s %s' % (archiveFullName,tmpFile))
                if options.verbose:
                    print tmpFile
                if status != 0 or out != '':
                    print out

        # go to tmpdir
        os.chdir(tmpDir)

        # compress
        status,out = commands.getstatusoutput('gzip %s' % archiveName)
        archiveName += '.gz'
        if status !=0 or options.verbose:
            print out

        # check archive
        status,out = commands.getstatusoutput('ls -l %s' % archiveName)
        if options.verbose:
            print out
        if status != 0:
            tmpLog.error("Failed to archive working area.\n        If you see 'Disk quota exceeded', try '--tmpDir /tmp'") 
            sys.exit(EC_Archive)

        # check symlinks
        if options.useAthenaPackages:
            tmpLog.info("checking symbolic links")
            status,out = commands.getstatusoutput('tar tvfz %s' % archiveName)
            if status != 0:
                tmpLog.error("Failed to expand archive")
                sys.exit(EC_Archive)
            symlinks = []    
            for line in out.split('\n'):
                items = line.split()
                if items[0].startswith('l') and items[-1].startswith('/'):
                    symlinks.append(line)
            if symlinks != []:
                tmpStr  = "Found some unresolved symlinks which may cause a problem\n"
                tmpStr += "     See, e.g., http://savannah.cern.ch/bugs/?43885\n"
                tmpStr += "   Please ignore if you believe they are harmless"
                tmpLog.warning(tmpStr)
                for symlink in symlinks:
                    print "  %s" % symlink
    else:
        # go to tmp dir
        os.chdir(tmpDir)
        # use a saved copy
        if options.libDS == '' and not options.nobuild:
            archiveName     = 'sources.%s.tar' % commands.getoutput('uuidgen')
            archiveFullName = "%s/%s" % (tmpDir,archiveName)
        else:
            archiveName     = 'jobO.%s.tar' % commands.getoutput('uuidgen')
            archiveFullName = "%s/%s" % (tmpDir,archiveName)
        # make copy to avoid name duplication
        shutil.copy(options.inTarBall,archiveFullName)

    # save
    if options.outTarBall != '':
        shutil.copy(archiveName,options.outTarBall)


# look for pandatools package
for path in sys.path:
    if path == '':
        path = curDir
    if os.path.exists(path) and os.path.isdir(path) and 'pandatools' in os.listdir(path):
        # make symlink for module name.
        os.symlink('%s/pandatools' % path,'taskbuffer')
        break

# append tmpdir to import taskbuffer module
sys.path = [tmpDir]+sys.path
from taskbuffer.JobSpec  import JobSpec
from taskbuffer.FileSpec import FileSpec


# max total filesize on each WN
maxTotalSize = Client.PandaSites[options.site]['maxinputsize'] * 1024*1024
if maxTotalSize in [0,'']:
    maxTotalSize = Client.maxTotalSize
safetySize   = Client.safetySize

# check nGBPerJob
if options.nGBPerJob != -1:
    if options.nGBPerJob == 'MAX':
	# use site max value
        options.nGBPerJob = maxTotalSize
    else:
        # convert to Bytes
        options.nGBPerJob = long(int(options.nGBPerJob)*1024*1024*1024)
        maxTotalSize = options.nGBPerJob

# read jobID
jobDefinitionID = 1
jobid_file = '%s/pjobid.dat' % os.environ['PANDA_CONFIG_ROOT']
if os.path.exists(jobid_file):
    try:
        # read line
        tmpJobIdFile = open(jobid_file)
        tmpID = tmpJobIdFile.readline()
        tmpJobIdFile.close()
        # remove \n
        tmpID = tmpID.replace('\n','')
        # convert to int
        jobDefinitionID = long(tmpID) + 1
    except:
        pass

# check permission
if not Client.checkSiteAccessPermission(options.site,options.workingGroup,options.verbose):
    sys.exit(EC_Config)

if options.verbose:
    print "== parameters =="
    print "Site       : %s" % options.site
    print "Athena     : %s" % athenaVer
    if groupArea != '':
        print "Group Area : %s" % groupArea
    if cacheVer != '':
        print "Cache      : %s" % cacheVer[1:]
    if nightVer != '':
        print "Nightly    : %s" % nightVer[1:]
    print "RunDir     : %s" % runDir
    print "exec       : %s" % options.jobParams


# job name
jobName = 'prun.%s' % commands.getoutput('uuidgen')

# make jobs
jobList = []

# buildJob
if options.nobuild:
    pass
elif options.libDS == '':
    jobB = JobSpec()
    jobB.jobDefinitionID   = jobDefinitionID
    if not options.panda_jobsetID in [None,'NULL']:
        jobB.jobsetID      = options.panda_jobsetID
    jobB.jobName           = jobName
    jobB.lockedby          = 'panda-client-%s' % PandaToolsPkgInfo.release_version
    if options.provenanceID != -1:
        jobB.jobExecutionID = options.provenanceID
    if options.panda_origFullExecString == '':    
        jobB.metadata = fullExecString
    else:
        jobB.metadata = urllib.unquote(options.panda_origFullExecString)        
    if athenaVer != '':
        jobB.AtlasRelease  = athenaVer
        jobB.homepackage   = 'AnalysisTransforms'+cacheVer+nightVer
    jobB.transformation    = '%s/buildGen-00-00-01' % Client.baseURLSUB
    if options.express:
        jobB.specialHandling = 'express'
    jobB.cmtConfig         = AthenaUtils.getCmtConfig(athenaVer,cacheVer,nightVer)
    if nickName == '':
        # old convention
        tmpDsPrefix = 'user%s.%s' % (time.strftime('%y',time.gmtime()),distinguishedName)
    else:
        tmpDsPrefix = 'user.%s' % nickName
    libDsName = '%s.%s.%s.lib._%06d' % (tmpDsPrefix,
                                        time.strftime('%m%d%H%M%S',time.gmtime()),
                                        datetime.datetime.utcnow().microsecond,
                                        jobDefinitionID)
    if Client.isDQ2free(options.site):
        # user specified output path 
        jobB.destinationDBlock = re.sub('/+$','',options.outputPath)+'/%s' % libDsName
    else:
        jobB.destinationDBlock = libDsName
    if Client.isDQ2free(options.site):
        # flag to use DQ2-free output
        jobB.destinationSE = 'local'
    else:
        jobB.destinationSE = options.site
    if options.prodSourceLabel != '':
        jobB.prodSourceLabel = options.prodSourceLabel
    else:
        jobB.prodSourceLabel   = 'panda'
    if options.processingType != '':    
        jobB.processingType = options.processingType
    if options.seriesLabel != '':    
        jobB.prodSeriesLabel = options.seriesLabel
    jobB.workingGroup      = options.workingGroup    
    jobB.assignedPriority  = 2000
    jobB.computingSite     = options.site
    jobB.cloud             = Client.PandaSites[options.site]['cloud']
    # lib.tgz
    fileBO = FileSpec()
    fileBO.lfn               = '%s.lib.tgz' % libDsName
    fileBO.type              = 'output'
    fileBO.dataset           = jobB.destinationDBlock
    fileBO.destinationSE     = jobB.destinationSE
    fileBO.destinationDBlock = jobB.destinationDBlock
    jobB.addFile(fileBO)
    fileBI = FileSpec()
    fileBI.lfn = archiveName
    fileBI.type = 'input'
    jobB.jobParameters     = '-i %s -o %s ' % (fileBI.lfn,fileBO.lfn)
    if options.bexec != '':
        jobB.jobParameters += '--bexec "%s" ' % urllib.quote(options.bexec)
        jobB.jobParameters += '-r %s ' % runDir
    # source URL
    matchURL = re.search("(http.*://[^/]+)/",Client.baseURLSSL)
    if matchURL != None:
        jobB.jobParameters += "--sourceURL %s " % matchURL.group(1)
    # use Athena packages
    if options.useAthenaPackages:
        jobB.jobParameters += "--useAthenaPackages "
    # log    
    file = FileSpec()
    file.lfn               = '%s.log.tgz' % libDsName
    file.type              = 'log'
    file.dataset           = jobB.destinationDBlock
    file.destinationSE     = jobB.destinationSE
    file.destinationDBlock = jobB.destinationDBlock
    jobB.addFile(file)
    # set space token
    for file in jobB.Files:
        if file.type in ['output','log']:
            if options.spaceToken != '':
                if Client.PandaSites[options.site]['setokens'].has_key(options.spaceToken):
                    file.destinationDBlockToken = options.spaceToken
            else:
                defaulttoken = Client.PandaSites[options.site]['defaulttoken']
                file.destinationDBlockToken = Client.getDefaultSpaceToken(vomsFQAN,defaulttoken)
    # append
    if options.verbose:    
        tmpLog.debug(jobB.jobParameters)
    jobList.append(jobB)
else:
    # query files in lib dataset to reuse libraries
    if not Client.isDQ2free(options.site):
        tmpLog.info("query files in libDS:%s" % options.libDS)
        tmpList = Client.queryFilesInDataset(options.libDS,options.verbose)
    else:
        # don't check for DQ2 free sites
        tmpList = {}
    tmpFileList = []
    tmpGUIDmap = {}
    for fileName in tmpList.keys():
        # ignore log file
        if re.search('\.log(\.\d+)*(\.tgz)*$',fileName) != None or \
               re.search('\.log(\.tgz)*(\.\d+)*$',fileName) != None:
            continue
        tmpFileList.append(fileName)
        tmpGUIDmap[fileName] = tmpList[fileName]['guid'] 
    # incomplete libDS
    if tmpFileList == []:
        # query files in dataset from Panda
        status,tmpMap = Client.queryLastFilesInDataset([options.libDS],options.verbose)
        # look for lib.tgz
        for fileName in tmpMap[options.libDS]:
            # ignore log file
            if re.search('\.log(\.\d+)*(\.tgz)*$',fileName) != None or \
                   re.search('\.log(\.tgz)*(\.\d+)*$',fileName) != None:
                continue
            tmpFileList.append(fileName)
            tmpGUIDmap[fileName] = None
    # incomplete libDS
    if tmpFileList == []:
        if not Client.isDQ2free(options.site):
            tmpLog.error("dataset %s is empty" % (options.libDS))
            sys.exit(EC_Dataset)
        else:
            # try lib.tgz for DQ2 free sites as this might be in ArchivedDB 
            fileName = "%s.lib.tgz" % options.libDS
            tmpFileList.append(fileName)
            tmpGUIDmap[fileName] = commands.getoutput('uuidgen')
    # check file list                
    if len(tmpFileList) != 1:
        tmpLog.error("dataset %s contains multiple lib files %s" % (options.libDS,tmpFileList))
        sys.exit(EC_Dataset)
    # instantiate FileSpec
    fileBO = FileSpec()
    fileBO.lfn = tmpFileList[0]
    fileBO.GUID = tmpGUIDmap[fileBO.lfn]
    fileBO.dataset = options.libDS
    fileBO.destinationDBlock = options.libDS
    if fileBO.GUID != 'NULL':
        fileBO.status = 'ready'

# modify outDS name when container is used for output
if original_outDS_Name.endswith('/'):
    options.outDS = re.sub('/$','.%s' % time.strftime('%y%m%d%H%M%S'),options.outDS)
    # check outDS
    tmpDatasets = Client.getDatasets(options.outDS,options.verbose)
    if len(tmpDatasets) != 0:
        outputDSexist = True

# get files in existing output dataset
existingFilesInOutDS = {}
if options.outputs != '' and (outputDSexist or outputContExist):
    # query files in dataset from DDM
    tmpLog.info("query files in %s" % original_outDS_Name)
    existingFilesInOutDS = Client.queryFilesInDataset(original_outDS_Name,options.verbose)
    # query files in dataset from Panda
    status,tmpMap = Client.queryLastFilesInDataset([original_outDS_Name],options.verbose)
    for tmpLFN in tmpMap[original_outDS_Name]:
        if not tmpLFN in existingFilesInOutDS:
            existingFilesInOutDS[tmpLFN] = None

if options.inDS != '':
    if len(missList) == 0:
        if options.secondaryDSs == {}:
            tmpLog.info("all files are available at %s" % options.site)
        else:
            tmpLog.info("all files in the primary DS are available at %s" % options.site)            
    else:
        tmpLog.info("%s files are missing or unchecked at %s" % (len(missList),options.site))
    # print the number of used files
    if options.secondaryDSs == {}:
        tmpLog.info("use %s files" % len(inputFileList))
    else:
        tmpLog.info("use %s files in the primary DS" % len(inputFileList))        

# disp datasetname to identify inputs
commonDispName = 'user_disp.%s' % commands.getoutput('uuidgen')

# check access method
isDirectAccess = PsubUtils.isDirectAccess(options.site)

# create single submission block when ContElementBoundary is not used
if inputGroupFilesMap=={} or not options.useContElementBoundary:
    if options.inDS != '':
        inputGroupFilesMap[options.inDS] = inputFileList
    else:
        inputGroupFilesMap['singleblock'] = []

# loop over all blocks
totalIndexJob     = 0
for b_inputDatasetName,b_inputFileList in inputGroupFilesMap.iteritems():
    if options.load_xml:
        # xml file explicitly creates each job, so we don't need to do anything here
        assert len(inputGroupFilesMap)==1, 'ERROR: cannot use --useContElementBoundary flag with --load_xml'
        b_nJobs = options.nJobs = xconfig.nJobs()
        b_nFilesPerJob = options.nFilesPerJob = -1
    else:
        # bootstrap nJobs/nFilesPerJob from user settings
        b_nJobs        = options.nJobs
        b_nFilesPerJob = options.nFilesPerJob
        # set number of files per job
        if options.inDS != '':
            b_nFilesPerJob,nFilesEachSubJob = PsubUtils.calculateNumSplitPrun(b_nFilesPerJob,options.nGBPerJob,
                                                                              b_inputFileList,inputFileMap,maxTotalSize,
                                                                              dbrDsSize,safetySize)
        # calculate number of jobs
        if b_nJobs == -1:
            if options.inDS == '':
                b_nJobs = 1
            elif options.nGBPerJob < 0:
                rest = len(b_inputFileList) % b_nFilesPerJob
                if rest == 0:
                    b_nJobs = len(b_inputFileList) / b_nFilesPerJob
                else:
                    b_nJobs = (len(b_inputFileList)-rest) / b_nFilesPerJob
                    b_nJobs += 1
            else:
                # nGBPerJob
                b_nJobs = len(nFilesEachSubJob)
    # LFN description
    b_descriptionInLFN = options.descriptionInLFN
    if options.useContElementBoundary:
        tmpDescription = PsubUtils.extractNthFieldFromDS(b_inputDatasetName,options.useNthFieldForLFN)
        if b_descriptionInLFN != '':
            b_descriptionInLFN += '.%s' % tmpDescription
        else:
            b_descriptionInLFN = tmpDescription
    # runJobs
    indexOffsetMap    = {}
    jobsetIDMapForLFN = {}
    numFilesMap       = {}
    for iJob in range(b_nJobs):
        if _FTKDEBUG:
            print 'FTK  : JOB',totalIndexJob
        if options.load_xml:
            jobXML = xconfig.jobs[totalIndexJob]
            options.jobParams = jobXML.exec_string()
            options.outputs = jobXML.outputs()
        jobR = JobSpec()
        jobR.jobDefinitionID   = jobDefinitionID
        if not options.panda_jobsetID in [None,'NULL']:
            jobR.jobsetID      = options.panda_jobsetID
        jobR.jobName           = jobName
        jobR.lockedby          = 'panda-client-%s' % PandaToolsPkgInfo.release_version
        if options.provenanceID != -1:
            jobR.jobExecutionID = options.provenanceID
        if options.panda_origFullExecString == '':
            jobR.metadata = fullExecString
        else:
            jobR.metadata = urllib.unquote(options.panda_origFullExecString)
        if athenaVer != '':
            jobR.AtlasRelease  = athenaVer
            jobR.homepackage   = 'AnalysisTransforms'+cacheVer+nightVer
        jobR.transformation    = '%s/runGen-00-00-02' % Client.baseURLSUB
        if options.express:
            jobR.specialHandling = 'express'
        jobR.cmtConfig         = AthenaUtils.getCmtConfig(athenaVer,cacheVer,nightVer)
        # memory
        if options.memory != -1:
            jobR.minRamCount = options.memory
        # CPU count
        if options.maxCpuCount != -1:
            jobR.maxCpuCount = options.maxCpuCount
        jobR.destinationDBlock = original_outDS_Name
        if Client.isDQ2free(options.site):
            # flag to use non-DQ2 output
            jobR.destinationSE = 'local'
        elif options.destSE != '':
            # write outputs to destSE
            jobR.destinationSE = options.destSE
        else:                
            jobR.destinationSE = options.site
        if options.prodSourceLabel != '':
            jobR.prodSourceLabel = options.prodSourceLabel
        else:
            jobR.prodSourceLabel   = 'user'
        if options.processingType != '':
            jobR.processingType = options.processingType
        if options.seriesLabel != '':    
            jobR.prodSeriesLabel = options.seriesLabel
        jobR.workingGroup      = options.workingGroup    
        jobR.assignedPriority  = 1000
        jobR.computingSite     = options.site
        jobR.cloud             = Client.PandaSites[options.site]['cloud']
        jobR.jobParameters     = '-j "%s" ' % jobScript
        # source URL
        matchURL = re.search("(http.*://[^/]+)/",Client.baseURLSSL)
        if matchURL != None:
            jobR.jobParameters += "--sourceURL %s " % matchURL.group(1)
        # use Athena packages
        if options.useAthenaPackages:
            jobR.jobParameters += "--useAthenaPackages "
        # write input to txt
        if options.writeInputToTxt != '':
            jobR.jobParameters += "--writeInputToTxt %s " % options.writeInputToTxt
        if options.jobParams != '':
            # random seed
            tmpJobO = options.jobParams
            for tmpItem in tmpJobO.split():
                match = re.search('%RNDM=(\d+)( |$|\'|\"|;)',tmpItem)
                if match:
                    tmpRndmNum = int(match.group(1)) + iJob
                    # replace parameters
                    tmpJobO = re.sub(match.group(0),'%s%s' % (tmpRndmNum,match.group(2)),tmpJobO)
            # DBR
            if '%DB:LATEST' in tmpJobO and options.dbRelease != '':
                tmpJobO = tmpJobO.replace('%DB:LATEST',options.dbRelease.split(':')[-1])
            jobR.jobParameters += '-p "%s" ' % urllib.quote(tmpJobO)
            if _FTKDEBUG:
                print 'FTK  : PARS', jobR.jobParameters
        # source files
        if not options.nobuild:
            fileS = FileSpec()
            fileS.lfn            = fileBO.lfn
            fileS.GUID           = fileBO.GUID
            fileS.type           = 'input'
            fileS.status         = fileBO.status
            fileS.dataset        = fileBO.destinationDBlock
            fileS.dispatchDBlock = fileBO.destinationDBlock
            jobR.addFile(fileS)
            jobR.jobParameters  += '-l %s ' % fileS.lfn    
        # additional files for libDS or noBuild
        if options.libDS != "" or options.nobuild:
            jobR.jobParameters += '-a %s ' % archiveName
        # input files
        if options.inDS != '':
            totalSize = 0
            if options.load_xml:
                # input files are listed explicitly for each subjob in the xml file
                inList = [v for v in b_inputFileList if v in jobXML.files_in_DS(options.inDS)]
            elif options.nGBPerJob < 0:
                indexIn = iJob*b_nFilesPerJob
                inList = b_inputFileList[indexIn:indexIn+b_nFilesPerJob]
            else:
                # nGBPerJob
                indexIn = 0
                for tmpNFiles in nFilesEachSubJob[:iJob]:
                    indexIn += tmpNFiles
                inList = b_inputFileList[indexIn:indexIn+nFilesEachSubJob[iJob]]
            if inList == []:
                break
            for tmpLFN in inList:
                vals = inputFileMap[tmpLFN]
                fileI = FileSpec()
                fileI.lfn        = tmpLFN
                fileI.GUID       = vals['guid']
                fileI.fsize      = vals['fsize']
                fileI.md5sum     = vals['md5sum']
                if vals.has_key('dataset'):
                    fileI.dataset    = vals['dataset']
                else:
                    fileI.dataset    = options.inDS
                fileI.prodDBlock = fileI.dataset
                if options.forceStaged:
                    fileI.prodDBlockToken = 'local'
                fileI.type       = 'input'
                fileI.status     = 'ready'
                fileI.dispatchDBlock = commonDispName
                jobR.addFile(fileI)
                try:
                    totalSize += long(fileI.fsize)
                except:
                    pass
            tmpInputMap = {'IN': copy.deepcopy(inList)}  
            # secondary datasets
            secMap = {}
            for tmpSecDS,tmpSecVal in options.secondaryDSs.iteritems():
                if options.load_xml:
                    # if xml file manages which secondaryDSs files are used in each job:
                    tmpSecList = [v for v in tmpSecVal['files'] if v['LFN'] in jobXML.files_in_DS(tmpSecDS)]
                else:
                    # if prun manages secondaryDSs:
                    tmpIndexSec = iJob*tmpSecVal['nFiles']
                    tmpSecList  = tmpSecVal['files'][tmpIndexSec:tmpIndexSec+tmpSecVal['nFiles']]
                    # check length
                    if len(tmpSecList) < tmpSecVal['nFiles']:
                        errStr  = "only %s files in %s are available at %s which is insufficient for splitting. " % \
                                  (len(tmpSecVal['files']),tmpSecDS,options.site)
                        errStr += "nJobs*nFilesPerJob=%s*%s=%s files are required" % (b_nJobs,tmpSecVal['nFiles'],
                                                                                      b_nJobs*tmpSecVal['nFiles'])
                        tmpLog.error(errStr)
                        sys.exit(EC_Split)
                # add FileSpec
                tmpSecLfnList = []
                for vals in tmpSecList:
                    fileI = FileSpec()
                    fileI.lfn        = vals['LFN']
                    fileI.GUID       = vals['guid']
                    fileI.fsize      = vals['fsize']
                    fileI.md5sum     = vals['md5sum']
                    fileI.dataset    = tmpSecDS
                    fileI.prodDBlock = tmpSecDS
                    fileI.type       = 'input'
                    fileI.status     = 'ready'
                    fileI.dispatchDBlock = commonDispName
                    if options.forceStagedSecondary:
                        fileI.prodDBlockToken = 'local'
                    jobR.addFile(fileI)
                    inList.append(fileI.lfn)
                    tmpSecLfnList.append(fileI.lfn)
                    try:
                        totalSize += long(fileI.fsize)
                    except:
                        pass
                # append to map
                if len(tmpSecLfnList)>0:
                    tmpInputMap[tmpSecVal['streamName']] = tmpSecLfnList
            # size check    
            if (not isDirectAccess) and totalSize > maxTotalSize-dbrDsSize:
                errStr  = "A subjob has %s input files and requires %sMB of disk space " \
                      % (len(inList), ((totalSize+dbrDsSize) >> 20))
                if dbrDsSize != 0:
                    errStr += "(DBRelease=%sMB)" % (dbrDsSize>>20)
                errStr += ". It must be less than %sMB to avoid overflowing the remote disk. " \
                      % (maxTotalSize >> 20)
                errStr += "Please split the job using --nFilesPerJob. If file sizes vary in large range "
                errStr += "--nGBPerJob may help. e.g., --nGBPerJob=MAX"
                tmpLog.error(errStr)            
                sys.exit(EC_Split)
            # set parameter
            jobR.jobParameters += '-i "%s" ' % inList
            if _FTKDEBUG:
                print 'FTK  : INDS LIST:',inList
            if options.secondaryDSs != {}:
                jobR.jobParameters += '--inMap "%s" ' % tmpInputMap
                # count number of files
                for tmpStreamName,tmpFileListForCount in tmpInputMap.iteritems():
                    if not numFilesMap.has_key(tmpStreamName):
                        numFilesMap[tmpStreamName] = 0
                    numFilesMap[tmpStreamName] += len(tmpFileListForCount)
                if _FTKDEBUG:
                    print 'FTK  : SECDS MAP:',tmpInputMap
        # DBRelease
        if options.dbRelease != '':
            tmpItems = options.dbRelease.split(':')
            tmpDbrDS  = tmpItems[0]
            tmpDbrLFN = tmpItems[1]
            # instantiate  FileSpec
            fileName = tmpDbrLFN
            vals     = dbrFiles[tmpDbrLFN]
            file = FileSpec()
            file.lfn            = fileName
            file.GUID           = vals['guid']
            file.fsize          = vals['fsize']
            file.md5sum         = vals['md5sum']
            file.dataset        = tmpDbrDS
            file.prodDBlock     = tmpDbrDS
            file.dispatchDBlock = tmpDbrDS
            file.type       = 'input'
            file.status     = 'ready'
            jobR.addFile(file)
            # set DBRelease parameter
            jobR.jobParameters += '--dbrFile %s ' % file.lfn
            if options.dbRunNumber != '':
                jobR.jobParameters += '--dbrRun %s ' % options.dbRunNumber
            # expansion
            if options.notExpandDBR:
                jobR.jobParameters += '--notExpandDBR '
        # output files
        outMap = {}
        dsNameWoSlash = re.sub('/$','',original_outDS_Name)
        lfnPrefixForLog = dsNameWoSlash
        if options.outputs != '':
            for tmpLFN in options.outputs.split(','):
                # set offset to 0 for fresh output dataset
                if existingFilesInOutDS == {}:
                    indexOffsetMap[tmpLFN] = 0
                    jobsetIDMapForLFN[tmpLFN] = ''
                    if dsNameWoSlash.startswith('group'):
                        # append group job SN since group.xyz.jobsetID may be duplicated
                        jobsetIDMapForLFN[tmpLFN] += "$GROUPJOBSN_"
                    jobsetIDMapForLFN[tmpLFN] += '$JOBSETID'
                # offset is already obtained
                if indexOffsetMap.has_key(tmpLFN):
                    idxOffset = indexOffsetMap[tmpLFN]
                    getOffset = False
                else:
                    idxOffset = 0
                    getOffset = True
                # new LFN
                matchLFN = re.search('^([^\.]+)\.([^\.]+)\.',dsNameWoSlash)
                if matchLFN != None and original_outDS_Name.endswith('/'):
                    flagUseNewLFN = True
                    if jobsetIDMapForLFN.has_key(tmpLFN) and jobsetIDMapForLFN[tmpLFN] != None:
                        lfnPrefix = '%s.%s.%s' % (matchLFN.group(1),matchLFN.group(2),jobsetIDMapForLFN[tmpLFN])
                    else:
                        lfnPrefix = '%s.%s.$JOBSETID' % (matchLFN.group(1),matchLFN.group(2))
                    if b_descriptionInLFN != '':
                        lfnPrefix += '.%s' % b_descriptionInLFN
                else:
                    flagUseNewLFN = False
                    lfnPrefix = dsNameWoSlash
                if options.load_xml and jobXML.prepend_string():
                    tmpNewLFN = '%s._%05d.%s.%s' % (lfnPrefix,idxOffset+totalIndexJob+1,jobXML.prepend_string(),tmpLFN)
                else:
                    tmpNewLFN = '%s._%05d.%s' % (lfnPrefix,idxOffset+totalIndexJob+1,tmpLFN)
                lfnPrefixForLog = lfnPrefix
                # change * to XYZ and add .tgz
                if tmpNewLFN.find('*') != -1:
                    tmpNewLFN = tmpNewLFN.replace('*','XYZ')
                    tmpNewLFN = '%s.tgz' % tmpNewLFN
                # get offset
                if getOffset:
                    oldHead = '%s._%05d.' % (lfnPrefix,idxOffset+totalIndexJob+1)
                    if not flagUseNewLFN:
                        # old LFN format : Datasetname.XYZ
                        filePatt = tmpNewLFN.replace(oldHead,'%s\._(\d+)\.' % lfnPrefix)
                        # get offset
                        idxOffset = PsubUtils.getMaxIndex(existingFilesInOutDS,filePatt)
                        # append
                        indexOffsetMap[tmpLFN] = idxOffset
                        # correct
                        newHead = '%s._%05d.' % (dsNameWoSlash,idxOffset+totalIndexJob+1)
                        tmpNewLFN = tmpNewLFN.replace(oldHead,newHead)
                    else:
                        # short LFN : user.nickname.XYZ
                        filePatt = tmpNewLFN.replace(oldHead,'%s\._(\d+)\.' % lfnPrefix.replace('$JOBSETID','([_\d]+)'))
                        # get offset
                        idxOffset,jobsetIDinLFN = PsubUtils.getMaxIndex(existingFilesInOutDS,filePatt,shortLFN=True)
                        # append
                        indexOffsetMap[tmpLFN] = idxOffset
                        jobsetIDMapForLFN[tmpLFN] = jobsetIDinLFN
                        # correct
                        if jobsetIDinLFN != None:
                            newHead = '%s._%05d.' % (lfnPrefix.replace('$JOBSETID',jobsetIDinLFN),idxOffset+totalIndexJob+1)
                            tmpNewLFN = tmpNewLFN.replace(oldHead,newHead)
                        elif dsNameWoSlash.startswith('group'):
                            # append group job SN since group.xyz.jobsetID may be duplicated 
                            newHead = '%s._%05d.' % (lfnPrefix.replace('$JOBSETID','$GROUPJOBSN_$JOBSETID'),idxOffset+totalIndexJob+1)
                            tmpNewLFN = tmpNewLFN.replace(oldHead,newHead)
                # set file spec
                file = FileSpec()
                file.lfn               = tmpNewLFN              
                file.type              = 'output'
                file.dataset           = original_outDS_Name
                file.destinationSE     = jobR.destinationSE
                file.destinationDBlock = options.outDS
                jobR.addFile(file)
                outMap[tmpLFN] = file.lfn
            # set parameter
            jobR.jobParameters += '-o "%s" ' % str(outMap)
            if _FTKDEBUG:
                print 'FTK  : OUT MAP:',outMap
        # log
        file = FileSpec()
        if options.load_xml and jobXML.prepend_string():
            # optionally append additional metadata to log file name according to xml file settings
            file.lfn               = '%s._$PANDAID.%s.log.tgz' % (lfnPrefixForLog,jobXML.prepend_string())
        else:
            file.lfn               = '%s._$PANDAID.log.tgz' % lfnPrefixForLog
        file.type              = 'log'
        file.dataset           = original_outDS_Name
        file.destinationSE     = jobR.destinationSE
        file.destinationDBlock = options.outDS
        jobR.addFile(file)
        jobR.jobParameters += '-r %s ' % runDir
        # set space token
        for file in jobR.Files:
            if file.type in ['output','log']:
                if options.spaceToken != '':
                    if Client.PandaSites[options.site]['setokens'].has_key(options.spaceToken):
                        file.destinationDBlockToken = options.spaceToken
                else:
                    defaulttoken = Client.PandaSites[options.site]['defaulttoken']
                    file.destinationDBlockToken = Client.getDefaultSpaceToken(vomsFQAN,defaulttoken)
        # set prodDBlock
        jobR.prodDBlock = PsubUtils.getProdDBlock(jobR,options.inDS)
        # write outputs to Chirp Server
        if options.useChirpServer != '':
            PsubUtils.setCHIRPtokenToOutput(jobR,options.useChirpServer)
        # check job spec
        PsubUtils.checkJobSpec(jobR)
        # append
        if options.verbose:    
            tmpLog.debug(jobR.jobParameters)
        jobList.append(jobR)
        totalIndexJob += 1

# print the number of used files including secondary DSs
if options.inDS != '' and options.secondaryDSs != {}:
    tmpStreamKeys = numFilesMap.keys()
    tmpStreamKeys.sort()
    msgStr = 'use'
    for tmpStreamKey in tmpStreamKeys:
        msgStr += ' %s files for %s,' % (numFilesMap[tmpStreamKey],tmpStreamKey)
    tmpLog.info(msgStr[:-1])


# jobsetID
jobsetID = options.panda_jobsetID

# no submit 
if options.nosubmit:
    tmpLog.info("%s subjobs to be submitted to %s" % (len(jobList),options.site))

# submit 
if not options.nosubmit:

    # upload proxy for glexec
    if Client.PandaSites.has_key(options.site):
        # delegation
        delResult = PsubUtils.uploadProxy(options.site,options.myproxy,gridPassPhrase,
                                          Client.PandaClouds[options.cloud]['pilotowners'],
                                          options.verbose)
        if not delResult:
            tmpLog.error("failed to upload proxy")
            sys.exit(EC_MyProxy)

    # upload sources via HTTP POST
    if options.panda_srcName == '':
        tmpLog.info("upload source files")
        status,out = Client.putFile(archiveName,options.verbose)
        if out != 'True':
            print out
            tmpLog.error("failed to upload source files with %s" % status)
            sys.exit(EC_Post)

    # dataset location
    tmpOutDsLocation = Client.PandaSites[options.site]['ddm']
    if options.spaceToken != '':
        if Client.PandaSites[options.site]['setokens'].has_key(options.spaceToken):
            tmpOutDsLocation = Client.PandaSites[options.site]['setokens'][options.spaceToken]
    # always use site's SE for libDS
    tmpLibDsLocation = tmpOutDsLocation

    # dataset registration
    if not Client.isDQ2free(options.site):
        # register output dataset
        if (not outputDSexist) or options.destSE != '':
            Client.addDataset(options.outDS,options.verbose,location=tmpOutDsLocation,dsExist=outputDSexist)

        # register output dataset container
        if original_outDS_Name.endswith('/'):
            # create
            if not outputContExist:
                Client.createContainer(original_outDS_Name,options.verbose)
            # add dataset
            if not outputDSexist:
                Client.addDatasetsToContainer(original_outDS_Name,[options.outDS],options.verbose)

        # register shadow dataset
        if not outputDSexist:
            Client.addDataset("%s%s" % (options.outDS,suffixShadow),options.verbose)

        # register libDS
        if options.libDS == '' and not options.nobuild:
            Client.addDataset(jobB.destinationDBlock,options.verbose,location=tmpLibDsLocation)

    # submit
    tmpLog.info("submit %s subjobs to %s" % (len(jobList),options.site))
    status,out = Client.submitJobs(jobList,options.verbose)
    if not Client.PandaSites[options.site]['status'] in ['online','brokeroff']:
        tmpLog.warning("%s is %s. Your jobs will wait until it becomes online" % \
                       (options.site,Client.PandaSites[options.site]['status']))

    # result
    print '==================='
    if out == []:
        tmpLog.error("Job submission was denied")
        sys.exit(EC_Submit)
    # check length
    if len(jobList) != len(out):
        wStr  = "Only %s/%s sub-jobs were accepted " % (len(out),len(jobList))
        wStr += "since the number of sub-jobs in a single submission is limited. "
        wStr += "Please submit the same job again (i.e., the same inDS and outDS). "
        wStr += "New sub-jobs will run only on unused files and outputs will be added to the same outDS\n"
        tmpLog.warning(wStr)
    outstr   = ''
    buildStr = ''
    runStr   = ''
    for index,o in enumerate(out):
        if o != None:
            if index==0:
                # set JobID
                jobID = o[1]
                # set jobsetID
                jobsetID = o[2]['jobsetID']
            if index==0 and options.libDS=='' and not options.nobuild:
                outstr += "  > build\n"
                outstr += "    PandaID=%s\n" % o[0]
                buildStr = '%s' % o[0]            
            elif (index==1 and options.libDS=='' and not options.nobuild) or \
                 (index==0 and (options.libDS!='' or options.nobuild)):
                outstr += "  > run\n"
                outstr += "    PandaID=%s" % o[0]
                runStr = '%s' % o[0]                        
            elif index+1==len(out):
                outstr += "-%s" % o[0]
                runStr += '-%s' % o[0]
    if not jobsetID in [None,'NULL']:                            
        print ' JobsetID  : %s' % jobsetID
    print ' JobID  : %s' % jobID
    print ' Status : %d' % status
    print outstr

    # create dir for DB
    dbdir = os.path.expanduser(os.environ['PANDA_CONFIG_ROOT'])
    if not os.path.exists(dbdir):
        os.makedirs(dbdir)

    # record jobID
    tmpJobIdFile = open(jobid_file,'w')
    tmpJobIdFile.write(str(jobID))
    tmpJobIdFile.close()

    # record libDS
    if options.libDS == '' and not options.nobuild:
        tmpFile = open(libds_file,'w')
        tmpFile.write(jobB.destinationDBlock)
        tmpFile.close()

# go back to current dir
os.chdir(curDir)

# check site occupancy
if siteSpecified or expCloudFlag:
    Client.checkQueuedAnalJobs(options.site,options.verbose)

# try another site if input files remain
options.crossSite -= 1
if options.crossSite > 0 and options.inDS != '' and not siteSpecified:
    if missList != []:
        PsubUtils.runPrunRec(missList,tmpDir,fullExecString,options.nFiles,inputFileMap,
                             options.site,options.crossSite,archiveName,options.removedDS,
			     options.inDS,options.goodRunListXML,options.eventPickEvtList,
			     options.panda_dbRelease,jobsetID,
                             orig_bexecStr,orig_execStr,options.verbose)
# succeeded
sys.exit(0)
