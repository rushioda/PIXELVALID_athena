#!/bin/bash

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"exec" "/usr/bin/python" "-u" "-Wignore" "$0" "$@"

# using /usr/bin/python for pyGTK 

import re
import os
import sys
import code
import time
import atexit
import signal
import datetime
import tempfile
import commands

# tweak sys.path since threading cannot be imported with Athena 15 on SLC5/64
tmpOut = commands.getoutput('unset LD_LIBRARY_PATH; unset PYTHONPATH; /usr/bin/python -c "import sys;print sys.path"')
try:
    exec "tmpSysPath = %s" % tmpOut.split('\n')[-1]
    sys.path = tmpSysPath + sys.path
except:
    pass

import optparse
import readline
import threading

from pandatools import PdbUtils
from pandatools import Client
from pandatools import BookConfig
from pandatools import GlobalConfig
from pandatools import PLogger
from pandatools import PsubUtils
from pandatools import PandaToolsPkgInfo


# readline support
readline.parse_and_bind('tab: complete')
readline.parse_and_bind('set show-all-if-ambiguous On')

# history support
pconfDir = os.path.expanduser(os.environ['PANDA_CONFIG_ROOT'])
if not os.path.exists(pconfDir):
    os.makedirs(pconfDir)
historyFile = '%s/.history' % pconfDir
# history file
if os.path.exists(historyFile):
    readline.read_history_file(historyFile)
readline.set_history_length(1024)

# set dummy CMTSITE
if not os.environ.has_key('CMTSITE'):
    os.environ['CMTSITE'] = ''

# set grid source file
globalConf = GlobalConfig.getConfig()
if globalConf.grid_src != '' and not os.environ.has_key('PATHENA_GRID_SETUP_SH'):
    os.environ['PATHENA_GRID_SETUP_SH'] = globalConf.grid_src

# make tmp dir
tmpDir = tempfile.mkdtemp()

# set tmp dir in Client
Client.setGlobalTmpDir(tmpDir)

# fork PID 
fork_child_pid = None

# exit action
def _onExit(dirName,hFile):
    # save history only for master process
    if fork_child_pid == 0:
        readline.write_history_file(hFile)
    # remove tmp dir
    commands.getoutput('rm -rf %s' % dirName)
atexit.register(_onExit,tmpDir,historyFile)



# look for PandaTools package
for path in sys.path:
    if path == '':
        path = '.'
    if os.path.exists(path) and os.path.isdir(path) and 'pandatools' in os.listdir(path) \
           and os.path.exists('%s/pandatools/__init__.py' % path):
        # make symlink for module name
        os.symlink('%s/pandatools' % path,'%s/taskbuffer' % tmpDir)
        break
sys.path = [tmpDir]+sys.path



# core class for book keeping
class PBookCore:

    # constructor
    def __init__(self,enforceEnter=False,verbose=False,restoreDB=False):
        # verbose
        self.verbose = verbose
        # restore database
        self.restoreDB = restoreDB
        # initialize database
        PdbUtils.initialzieDB(self.verbose,self.restoreDB)
        # check proxy
        self.gridPassPhrase,self.vomsFQAN = PsubUtils.checkGridProxy('',enforceEnter,self.verbose)


    # synchronize database
    def sync(self):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        tmpLog.info("Synchronizing local repository ...")
        # check proxy
        self.gridPassPhrase,self.vomsFQAN = PsubUtils.checkGridProxy(self.gridPassPhrase,False,self.verbose)
        # get JobIDs in local repository
        localJobIDs = PdbUtils.getListOfJobIDs()
        # get recent JobIDs from panda server
        syncTime = datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')
        # set sync time for the first attempt
        bookConf = BookConfig.getConfig()
        if self.restoreDB:
            # reset last_synctime to restore database 
            bookConf.last_synctime = ''
            # disable
            self.restoreDB = False
            tmpLog.info("It may take several minutes to restore local repository ...")
        if bookConf.last_synctime == '':
            bookConf.last_synctime = datetime.datetime.utcnow()-datetime.timedelta(days=180)
            bookConf.last_synctime = bookConf.last_synctime.strftime('%Y-%m-%d %H:%M:%S')
        status,remoteJobIDs = Client.getJobIDsInTimeRange(bookConf.last_synctime,verbose=self.verbose)
        if status != 0:
            tmpLog.error("Failed to get JobIDs from panda server")
            return
        tmpLog.info("Got %s jobs to be updated" % len(remoteJobIDs))
        # insert if missing
        for remoteJobID in remoteJobIDs:
            # check local status
            job = None
            if remoteJobID in localJobIDs:
                # get job info from local repository
                job = PdbUtils.readJobDB(remoteJobID,self.verbose)
                # skip if frozen
                if job.dbStatus == 'frozen':
                    continue
            tmpLog.info("Updating JobID=%s ..." % remoteJobID)
            # get PandaIDs
            status,pandaIDstatus = Client.getPandIDsWithJobID(remoteJobID,verbose=self.verbose)
            if status != 0:
                tmpLog.error("Failed to get PandaIDs for %s" % remoteJobID) 
                return
            pandaIDs = pandaIDstatus.keys()
            pandaIDs.sort()
            # get full JobSpec
            pandaJobs = []            
            pandaFileInfo = {}
            pandaJobForSiteID = None
            if job == None:
                tmpIDs = [pandaIDs[0],pandaIDs[-1]]
                status,pandaJobs = Client.getFullJobStatus(tmpIDs,verbose=self.verbose)
                if status != 0:
                    tmpLog.error("Failed to get PandaJobs for %s" % remoteJobID) 
                    return
		# get slimmed file info
                status,pandaFileInfo = Client.getSlimmedFileInfoPandaIDs(pandaIDs,verbose=self.verbose)
                if status != 0:
                    tmpLog.error("Failed to get file info  for %s" % remoteJobID)
                    return
            else:
                # get one job to set computingSite which may have changed due to rebrokerage
                status,tmpPandaJobs = Client.getFullJobStatus([pandaIDs[0]],verbose=self.verbose)
                if status != 0:
                    tmpLog.error("Failed to get PandaJobs for %s" % remoteJobID)
                    return
                pandaJobForSiteID = tmpPandaJobs[0]
            # convert to local job spec
            localJob = PdbUtils.convertPtoD(pandaJobs,pandaIDstatus,job,pandaFileInfo,pandaJobForSiteID)
            # update database 
            if not remoteJobID in localJobIDs:
                # insert to DB
                try:
                    PdbUtils.insertJobDB(localJob,self.verbose)
                except:
                    tmpLog.error("Failed to insert JobID=%s to local repository" % remoteJobID)
                    return
                # set retryID
                if not localJob.provenanceID in [0,'0']:
                    try:
                        PdbUtils.setRetryID(localJob,self.verbose)
                    except:
                        tmpLog.error("Failed to set retryID for JobID=%s in local repository" % remoteJobID)
                        return
            else:
                # update
                try:
                    PdbUtils.updateJobDB(localJob,self.verbose)
                except:
                    tmpLog.error("Failed to update local repository for JobID=%s" % remoteJobID)
                    return
        # update sync time
        bookConf = BookConfig.getConfig()
        bookConf.last_synctime = syncTime
        BookConfig.updateConfig(bookConf)
        tmpLog.info("Synchronization Completed")
        

    # get local job info
    def getJobInfo(self,JobID):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # get job info from local repository
        job = PdbUtils.readJobDB(JobID,self.verbose)
        # not found
        if job == None:
            tmpLog.warning("JobID=%s not found in local repository. Synchronization may be needed" % JobID)
            return None
        # return
        return job


    # get local job/jobset info
    def getJobJobsetInfo(self,id):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # try to get jobset
        job = PdbUtils.readJobsetDB(id,self.verbose)
        # get job info from local repository
        if job == None:
            job = PdbUtils.readJobDB(id,self.verbose)
        # not found
        if job == None:
            tmpLog.warning("JobsetID/JobID=%s not found in local repository. Synchronization may be needed" % JobID)
            return None
        # return
        return job


    # get local job list
    def getLocalJobList(self):
        # get jobs
        localJobs = PdbUtils.bulkReadJobDB(self.verbose)
        # return
        return localJobs


    # get JobIDs with JobsetID
    def getJobIDsWithSetID(self,jobsetID):
        idMap = PdbUtils.getMapJobsetIDJobIDs(self.verbose)
        if idMap.has_key(jobsetID):
            return idMap[jobsetID]
        return None


    # make JobSetSpec
    def makeJobsetSpec(self,jobList):
        return PdbUtils.makeJobsetSpec(jobList)


    # get status
    def status(self,JobID,forceUpdate=False):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # check proxy
        self.gridPassPhrase,self.vomsFQAN = PsubUtils.checkGridProxy(self.gridPassPhrase,False,self.verbose)
        # get job info from local repository
        job = self.getJobInfo(JobID)
        if job == None:
	    # not found
            return None
        # update if needed
        if job.dbStatus != 'frozen' or forceUpdate:
            tmpLog.info("Getting status for JobID=%s ..." % JobID)            
            # get status from Panda server
            status,pandaIDstatus = Client.getPandIDsWithJobID(JobID,verbose=self.verbose)
            if status != 0:
                tmpLog.error("Failed to get status for JobID=%s" % JobID)
                return None
            # get one job to set computingSite which may have changed due to rebrokerage
            pandaJob = None
            if pandaIDstatus != {}:
                tmpPandaIDs = pandaIDstatus.keys()
                tmpPandaIDs.sort()
                status,tmpPandaJobs = Client.getFullJobStatus(tmpPandaIDs[:1],verbose=self.verbose)
                if status != 0:
                    tmpLog.error("Failed to get PandaJobs for %s" % JobID)
                    return None
                pandaJob = tmpPandaJobs[0]
            # convert to local job spec
            job = PdbUtils.convertPtoD([],pandaIDstatus,job,pandaJobForSiteID=pandaJob)
            # update DB
            try:
                PdbUtils.updateJobDB(job,self.verbose)
            except:
                tmpLog.error("Failed to update local repository for JobID=%s" % JobID)
                return None
            tmpLog.info("Updated JobID=%s" % JobID)                        
        # return
        return job


    # get status for JobSet and Job
    def statusJobJobset(self,id,forceUpdate=False):
        tmpJobIDList = self.getJobIDsWithSetID(id)
        if tmpJobIDList == None:
            # not a jobset
            job = self.status(id,forceUpdate)
        else:
            # jobset
            tmpJobs = []
            for tmpJobID in tmpJobIDList:
                tmpJob = self.status(tmpJobID,forceUpdate)
                tmpJobs.append(tmpJob)
            # make jobset
            job = self.makeJobsetSpec(tmpJobs)
        # return     
        return job
    

    # kill
    def kill(self,JobID):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # check proxy
        self.gridPassPhrase,self.vomsFQAN = PsubUtils.checkGridProxy(self.gridPassPhrase,False,self.verbose)
        # get jobset
        jobList = self.getJobIDsWithSetID(JobID)
        if jobList == None:
            jobList = [JobID]
        else:
            tmpMsg = "JobsetID=%s is composed of JobID=" % JobID
            for tmpJobID in jobList:
                tmpMsg += '%s,' % tmpJobID
            tmpMsg = tmpMsg[:-1]
            tmpLog.info(tmpMsg)
        for tmpJobID in jobList:    
            # get job info from local repository
            job = self.getJobInfo(tmpJobID)
            if job == None:
                tmpLog.warning("JobID=%s not found in local repository. Synchronization may be needed" % tmpJobID)            
                continue
            # skip frozen job
            if job.dbStatus == 'frozen':
                tmpLog.info('All subJobs in JobID=%s already finished/failed' % tmpJobID)
                continue
            # get PandaID list
            killJobs = job.PandaID.split(',')
            # kill
            tmpLog.info('Sending kill command ...')
            status,output = Client.killJobs(killJobs,self.verbose)
            if status != 0:
                tmpLog.error(output)
                tmpLog.error("Failed to kill JobID=%s" % tmpJobID)
                return False
            # update database
            job.commandToPilot = 'tobekilled'
            # update DB
            try:
                PdbUtils.updateJobDB(job,self.verbose)
            except:
                tmpLog.error("Failed to update local repository for JobID=%s" % tmpJobID)
                return False
            # done
            tmpLog.info('Done. JobID=%s will be killed in 30min' % tmpJobID)
        return True


    # rebrokerage
    def rebrokerage(self,JobsetID,cloud):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # check proxy
        self.gridPassPhrase,self.vomsFQAN = PsubUtils.checkGridProxy(self.gridPassPhrase,False,self.verbose)
        # get jobset
        jobList = self.getJobIDsWithSetID(JobsetID)
        if jobList == None:
            jobList = [JobsetID]
        else:
            tmpMsg = "JobsetID=%s is composed of JobID=" % JobsetID
            for tmpJobID in jobList:
                tmpMsg += '%s,' % tmpJobID
            tmpMsg = tmpMsg[:-1]
            tmpLog.info(tmpMsg)
        for JobID in jobList:    
	    # get job info using status
            job = self.status(JobID)
            if job == None:
		# not found
                continue
	    # skip frozen job
            if job.dbStatus == 'frozen':
                tmpLog.info('All subJobs in JobID=%s already finished/failed' % JobID)
                continue
	    # rebrokerage
            tmpLog.info('Sending rebrokerage request ...')
            status,output = Client.runReBrokerage(JobID,job.libDS,cloud,self.verbose)
            if status != 0:
                tmpLog.error(output)
                tmpLog.error("Failed to reassign JobID=%s" % JobID)
                return
            # done
            tmpLog.info('Done for %s' % JobID)
        return


    # clean
    def clean(self,nDays=90):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # delete
        try:
            PdbUtils.deleteOldJobs(nDays,self.verbose)
        except:
            tmpLog.error("Failed to delete old jobs")
            return
        # done
        tmpLog.info('Done')
        return


    # kill and retry
    def killAndRetry(self,JobID,newSite=False,newOpts={},ignoreDuplication=True):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # kill
        retK = self.kill(JobID)
        if not retK:
            return False
        # sleep
        tmpLog.info('Going to sleep for 5sec')
        time.sleep(5)
        nTry = 6
        for iTry in range(nTry):
            # get status
            job = self.status(JobID)
            if job == None:
                return False
            # check if frozen
            if job.dbStatus == 'frozen':
                break
            tmpLog.info('Some sub-jobs are still running')
            if iTry+1 < nTry:
                # sleep
                tmpLog.info('Going to sleep for 10min')
                time.sleep(600)
            else:
                tmpLog.info('Max attempts exceeded. Please try later')
                return False
        # retry
        self.retry(JobID,newSite=newSite,newOpts=newOpts,ignoreDuplication=ignoreDuplication)
        return
                        

    # retry
    def retry(self,JobsetID,newSite=False,newOpts={},noSubmit=False,ignoreDuplication=True):
        # get logger
        tmpLog = PLogger.getPandaLogger()
        # check proxy
        self.gridPassPhrase,self.vomsFQAN = PsubUtils.checkGridProxy(self.gridPassPhrase,False,self.verbose)
        # get jobset
        newJobsetID = -1
        jobList = self.getJobIDsWithSetID(JobsetID)
        if jobList == None:
            isJobset = False
            jobList = [JobsetID]
        else:
            isJobset = True
            tmpMsg = "JobsetID=%s is composed of JobID=" % JobsetID
            for tmpJobID in jobList:
                tmpMsg += '%s,' % tmpJobID
            tmpMsg = tmpMsg[:-1]
            tmpLog.info(tmpMsg)
        for JobID in jobList:    
            # get job info from local repository
            localJob = self.getJobInfo(JobID)
            if localJob == None:
                tmpLog.warning("JobID=%s not found in local repository. Synchronization may be needed" % JobID)            
                return None
            # skip running job
            if localJob.dbStatus != 'frozen':
                tmpLog.warning('Cannot retry running jobs')
                if isJobset:
                    continue
                else:
                    return
            # skip already retried
            if localJob.retryID != '0':
                if isJobset:
                    tmpLog.info('Skip JobID=%s since already retried by JobID=%s JobsetID=%s' % \
                                (JobID,localJob.retryID,localJob.retryJobsetID))
                    continue
                else:
                    tmpLog.warning('This job was already retried by JobID=%s' % localJob.retryID)
                    return
            # check status of buildJob
            if not localJob.buildStatus in ['','finished']:
                tmpMsgStr = 'Cannot retry since status of buildJob %s is %s (!= finished). ' \
                            % (localJob.PandaID.split(',')[0],localJob.buildStatus)
                tmpMsgStr += 'Please execute %s with the same input/output datasets (or containers). ' % localJob.jobType
                tmpMsgStr += 'It will run only on failed/cancelled/unused input files '
                tmpMsgStr += 'and append output files to the output dataset container'
                tmpLog.warning(tmpMsgStr)
                if isJobset:
                    continue
                else:
                    return
            # check opts for newSite
            if newSite:
                if not localJob.outDS.endswith('/') and not newOpts.has_key('outDS') and not newOpts.has_key('--outDS'):
                    tmpLog.warning('You need to specify --outDS in newOpts to retry at new site unless container is used as output')
                    return
            # get list of failed jobs
            pandaIDs  = localJob.PandaID.split(',')
            statusList= localJob.jobStatus.split(',')
            jobList = []
            for idx in range(len(pandaIDs)):
                # check status
                if not statusList[idx] in ['failed','cancelled']:
                    continue
                jobList.append(pandaIDs[idx])
            # no failed job
            if jobList == []:
                if isJobset:
                    tmpLog.info('Skip JobID=%s since no failed jobs' % JobID)                    
                    continue
                else:
                    tmpLog.info('No failed jobs to be retried for JobID=%s' % JobID)
                    return
            # get full job spec
            tmpLog.info("Retrying JobID=%s ..." % JobID)
            tmpLog.info("Getting job info")
            idxJL  = 0
            nQuery = 500
            pandaJobs = []
            while idxJL < len(jobList):
                # avoid burst query
                tmpLog.info(" %5s/%s" % (idxJL,len(jobList)))                
                status,oTmp = Client.getFullJobStatus(jobList[idxJL:idxJL+nQuery],
                                                      verbose=self.verbose)
                if status != 0:
                    tmpLog.error(status)
                    tmpLog.error(oTmp)
                    tmpLog.error("Cannot get job info from Panda server")
                    return
                pandaJobs += oTmp
                idxJL += nQuery
                time.sleep(1)
            tmpLog.info(" %5s/%s" % (len(jobList),len(jobList)))
            # jobdefID
            newJobdefID = PsubUtils.readJobDefID()
            # reset some parameters
            retryJobs    = []
            retrySite    = None
            retryElement = None
            retryDestSE  = None
            outDsName    = None
            shadowList   = []
            for idx in range(len(jobList)):
                job = pandaJobs[idx]
                # skip exired
                if job == None:
                    tmpLog.warning("Could not retry jobs older than 30 days : JobID=%s (PandaID=%s) expired" \
                                   % (JobID,jobList[idxJob]))
                    return
                # get shadow list
                if (not ignoreDuplication) and outDsName == None and job.prodSourceLabel == 'user':
                    # look for dataset for log since it doesn't have suffix even when --individualOutDS is used
                    for tmpFile in job.Files:
                        if tmpFile.type == 'log':
                            outDsName = tmpFile.dataset
                            break
                    # output dataset was not found    
                    if outDsName == None:
                        tmpLog.error("Could not get output dataset name for JobID=%s (PandaID=%s)" \
                                     % (JobID,job.PandaID))
                        return
                    # get files in shadow
                    if outDsName.endswith('/'):
                        shadowList = Client.getFilesInShadowDataset(outDsName,Client.suffixShadow,self.verbose)
                    else:
                        # disable duplication check mainly for old overlay jobs since non-signal files are wrongly skipped
                        #shadowList = Client.getFilesInShadowDatasetOld(outDsName,Client.suffixShadow,self.verbose)
                        pass
                # unify sitename
                if retrySite == None:
                    retrySite    = job.computingSite
                    retryElement = job.computingElement
                    retryDestSE  = job.destinationSE
                # reset
                job.jobStatus           = None
                job.commandToPilot      = None
                job.startTime           = None
                job.endTime             = None
                job.attemptNr           = 1+job.attemptNr
                for attr in job._attributes:
                    if attr.endswith('ErrorCode') or attr.endswith('ErrorDiag'):
                        setattr(job,attr,None)
                job.transExitCode       = None
                job.computingSite       = retrySite
                job.computingElement    = retryElement
                job.destinationSE       = retryDestSE
                job.dispatchDBlock      = None
                job.jobExecutionID      = JobID
                job.jobDefinitionID     = newJobdefID
                job.parentID            = job.PandaID
                if job.jobsetID != ['NULL',None,-1]:
                    job.sourceSite          = job.jobsetID
                    job.jobsetID            = newJobsetID
                skipInputList = []
                numUsedFiles = 0
                for file in job.Files:
                    file.rowID = None
                    if file.type == 'input':
                        # protection against wrong sync which doesn't update buildStatus correctly
                        if file.lfn.endswith('.lib.tgz') and file.GUID == 'NULL':
                            tmpLog.warning('GUID for %s is unknown. Cannot retry when corresponding buildJob failed' \
                                           % file.lfn)
                            return 
                        file.status = 'ready'
                        # check with shadow for non lib.tgz/DBR 
                        tmpDbrMatch = re.search('^DBRelease-.*\.tar\.gz$',file.lfn)
                        if tmpDbrMatch == None and not file.lfn.endswith('.lib.tgz'):
                            if file.lfn in shadowList:
                                skipInputList.append(file)
                            else:
                                numUsedFiles += 1
                    elif file.type in ('output','log'):
                        file.destinationSE = retryDestSE
                        file.destinationDBlock = re.sub('_sub\d+$','',file.destinationDBlock)
                        # add retry num
                        if file.dataset.endswith('/'):
                            retryMatch = re.search('_r(\d+)$',file.destinationDBlock)
                            if retryMatch == None:
                                file.destinationDBlock += '_r1'
                            else:
                                tmpDestinationDBlock = re.sub('_r(\d+)$','',file.destinationDBlock)
                                file.destinationDBlock = tmpDestinationDBlock + '_r%d' % (1+int(retryMatch.group(1)))
                        # add attempt nr
                        oldName  = file.lfn
                        file.lfn = re.sub("\.\d+$","",file.lfn)
                        file.lfn = "%s.%d" % (file.lfn,job.attemptNr)
                        newName  = file.lfn
                        # modify jobParameters
                        job.jobParameters = re.sub("'%s'" % oldName ,"'%s'" % newName,
                                                   job.jobParameters)
                        # look for output in trf
                        oldGenelicName = re.sub('\.\d+$','',oldName)
                        match = re.search(oldGenelicName+'(\.\d+)*(%20|")',job.jobParameters)
                        if match != None:
                            job.jobParameters = job.jobParameters.replace(match.group(0),newName+match.group(2))
                # all files are used by others
                if numUsedFiles == 0 and skipInputList != []:
                    continue
                # remove skipped files
                strSkipped = ''
                for tmpFile in skipInputList:
                    strSkipped += '%s,' % tmpFile.lfn
                    job.Files.remove(tmpFile)
                strSkipped = strSkipped[:-1]
                # modify jobpar
                if strSkipped != '':
                    optionToSkipFiles = '--skipInputByRetry'
                    if not optionToSkipFiles in job.jobParameters:
                        # just append
                        job.jobParameters += "%s=%s " % (optionToSkipFiles,strSkipped)
                    else:
                        # extract already skipped files
                        tmpMatch = re.search("(%s=[^ ]+)",job.jobParameters)
                        if tmpMatch == None:
                            tmpLog.error("Failed to extract arg of %s for PandaID=%s" \
                                         % (optionToSkipFiles,job.PandaID))
                            return
                        # replace
                        job.jobParameters = re.sub(tmpMatch.group(1),"%s,%s" % (tmpMatch.group(1),optionToSkipFiles),
                                                   job.jobParameters)
                if self.verbose:
                    tmpLog.debug(job.jobParameters)
                # append
                retryJobs.append(job)
            # all input files were or are being used by other jobs
            if retryJobs == []:
                tmpLog.info('All input files were or are being used by other jobs for the same output. No jobs to be retried. If you need to ignore duplication check (e.g., using the same EVNT file for multiple simulation subjobs), set ignoreDuplication=True. i.e. retry(123,ignoreDuplication=True)')
                return
	    # check voms role
            if not retryJobs[0].workingGroup in ['NULL',None,'']:
		# VOMS role was used 
                if not "--workingGroup" in job.metadata:
		    # extract voms roles from metadata
                    match =  re.search("--voms( |=)[ \"]*([^ \"]+)",job.metadata)
                    if match != None:
                        vomsRoles = match.group(2)
                    else:
                        vomsRoles = "atlas:/atlas/%s/Role=production" % retryJobs[0].workingGroup
		    # regenerate proxy with VOMS roles
                    try:
                        tmpLog.info("Checking proxy role to resubmit %s jobs" % retryJobs[0].workingGroup)
                        self.gridPassPhrase,self.vomsFQAN = PsubUtils.checkGridProxy(self.gridPassPhrase,False,
										     self.verbose,vomsRoles)
                    except:
                        tmpLog.error("Failed to generate a proxy with %s" % vomsRoles)
                        return
            # check runtime env for new site submission
            if newSite:
                if retryJobs[0].processingType == 'pathena' or '--useAthenaPackages' in retryJobs[0].metadata:
                    from pandatools import AthenaUtils
                    stA,retA = AthenaUtils.getAthenaVer()
                    if not stA:
                        tmpLog.error("Failed to get Athena rel/cache version in current runtime env")
                        return
                    athenaVer = retA['athenaVer']
                    cacheVer  = retA['cacheVer']
                    nightVer  = retA['nightVer']
                    wrongSetup = False
                    if retryJobs[0].AtlasRelease != 'Atlas-%s' % athenaVer:
                        wrongSetup = True
                        errMsg =  "Current Athena version Atlas-%s is inconsitent with the previous submission %s. " % (athenaVer,retryJobs[0].AtlasRelease)
                    elif retryJobs[0].homepackage != 'AnalysisTransforms'+cacheVer+nightVer:
                        wrongSetup = True                        
                        errMsg =  "Current cache version %s is inconsitent with the previous submission. " % cacheVer.replace('-','').replace('_','-')
                    if wrongSetup:    
                        errMsg += 'You need to have the same runtime env as before since all job spec need to be re-created to send jobs to a new site. '
                        errMsg += 'Please setup Athena correctly and restart pbook'                        
                        tmpLog.error(errMsg)
                        return
	    # test mode
            if noSubmit:
                continue
            # invoke pathena/prun to send job to new site
            if newSite:
                tmpLog.info("Constrcuting job spec again to be sent to another site ...")
                comStat= PsubUtils.execWithModifiedParams(retryJobs,newOpts,self.verbose)
                if comStat == 0:
                    # update database
                    time.sleep(2)
                    self.sync()
                else:
                    tmpLog.error("Failed to submit jobs to Panda server")                
                return
            # register datasets
            tmpOutDsLocation = Client.PandaSites[retryJobs[-1].computingSite]['ddm']
            addedDataset = []
            shadowDSname = None
            for tmpFile in retryJobs[-1].Files:
                if tmpFile.type in ['output','log'] and tmpFile.dataset.endswith('/'):
                    # add shadow
                    if shadowDSname == None and tmpFile.type == 'log':
                        shadowDSname = "%s%s" % (tmpFile.destinationDBlock,Client.suffixShadow)
                        Client.addDataset(shadowDSname,self.verbose)
                    # add datasets
                    if not tmpFile.destinationDBlock in addedDataset:
                        # create dataset
                        Client.addDataset(tmpFile.destinationDBlock,self.verbose,location=tmpOutDsLocation)
                        # add to container
                        Client.addDatasetsToContainer(tmpFile.dataset,[tmpFile.destinationDBlock],self.verbose)
                        # append
                        addedDataset.append(tmpFile.destinationDBlock)
            # submit
            tmpLog.info("Submitting job ...")            
            status,out = Client.submitJobs(retryJobs,verbose=self.verbose)
            if out == None or status != 0:
                tmpLog.error(status)
                tmpLog.error(out)
                tmpLog.error("Failed to submit jobs to Panda server")
                return
            # update database
            pandaIDstatus = {}
            newJobID = None
            for items in out:
                # get newJobID
                if newJobID == None:
                    newJobID = items[1]
                # check PandaID
                PandaID = items[0]
                if PandaID == 'NULL':
                    tmpLog.error("Panda server returned wrong IDs. It may have a temporary problem")
                    return
                # set newJobsetID
                if newJobsetID in [None,-1]:
                    newJobsetID = items[2]['jobsetID']
                # dummy statuso
                pandaIDstatus[PandaID] = ('defined','NULL')
            # set retry ID
            localJob.retryID = newJobID
            if not newJobsetID in [None,-1,'NULL']:
                localJob.retryJobsetID = newJobsetID
            try:
                PdbUtils.updateJobDB(localJob,self.verbose)
            except:
                tmpLog.error("Failed to set retryID for JobID=%s" % JobID)
                return
            # set new paramers
            newLocalJob = PdbUtils.convertPtoD(retryJobs,pandaIDstatus)
            newLocalJob.JobID = newJobID
            if not newJobsetID in [None,-1,'NULL']:
                newLocalJob.groupID = newJobsetID
            newLocalJob.creationTime = datetime.datetime.utcnow()
            # insert to DB
            try:
                PdbUtils.insertJobDB(newLocalJob,self.verbose)
            except:
                tmpLog.error("Failed to insert JobID=%s to local repository" % newJobID)
                return
            # write new jobdefID
            PsubUtils.writeJobDefID(newJobID)
            # done
            tmpMsg = 'Done. New JobID=%s' % newJobID
            if not newJobsetID in [None,-1,'NULL']:
                tmpMsg += " JobsetID=%s" % newJobsetID
            tmpLog.info(tmpMsg)
            


# keep original help
_orgHelp = help



# main for interactive session
def intmain(pbookCore,comString):

    # help
    def help(*arg):
        """Print help"""
        # use built-in help
        if len(arg) > 0:
            apply(_orgHelp,arg)
            return
        # print available methods
        str = """The following commands are available:

   sync
   show
   kill
   retry
   clean
   reassign
   killAndRetry
         
For more info, do help(show) for example."""
        print str
        
                                        
    # show status
    def show(JobID=None,upperJobID=None,forceUpdate=False,showPandaIDinState='',longFormat=False):
        """Print job records. All jobs will be shown if JobID is omitted. Jobs between JobID and upperJobID are shown when upperJobID is specified. Only running jobs and last N jobs will be shown if show('running') and show(-N), respectively. If forceUpdate=True, it will try to retrieve job infomation from the panda server. This may be useful to fix the local DB when synchronization between the local DB and the panda DB is lost somehow. When longFormat=True, jobs in the job set will be individually shown. showPandaIDinState takes a list of states so that PandaIDs in one of those states will be shown.
        
         example:
           >>> show()
           >>> show(12)
           >>> show(12,15)           
           >>> show(-5)
           >>> show('running')
           >>> show(15,showPandaIDinState='activated,running')
           >>> show(15,longFormat=True)
         """
        if JobID == None or JobID < 0 or upperJobID != None:
            # check range
            if upperJobID != None:
                if JobID > upperJobID:
                    tmpLog = PLogger.getPandaLogger()
                    tmpLog.error("upper JobID must be larger than %s" % JobID)
                    return
                if JobID < 0:
                    tmpLog = PLogger.getPandaLogger()
                    tmpLog.error("JobID must be positive when upper JobID is given")
                    return
            # show all local info
            jobList = pbookCore.getLocalJobList()
            if JobID < 0:
                jobList = jobList[JobID:]
            # print
            for job in jobList:
                if upperJobID != None:
                    if hasattr(job,'JobID'):
                        if job.JobID < JobID or job.JobID > upperJobID:
                            continue
                    else:
                        if long(job.JobsetID) < JobID or long(job.JobsetID) > upperJobID:
                            continue
                print "======================================"
                if showPandaIDinState != '':
                    job.flag_showSubstatus = showPandaIDinState
                if longFormat:
                    job.flag_longFormat = True
                print job
        elif JobID == 'running':
            # show runnig jobs
            jobList = pbookCore.getLocalJobList()
            for job in jobList:
                if job.dbStatus != 'frozen':
                    if hasattr(job,'JobID'):
                        # job
                        show(job.JobID,forceUpdate=forceUpdate,
                             showPandaIDinState=showPandaIDinState,longFormat=longFormat)
                    else:
                        # jobset
                        show(long(job.JobsetID),forceUpdate=forceUpdate,
                             showPandaIDinState=showPandaIDinState,longFormat=longFormat)
        else:
            tmpJobIDList = pbookCore.getJobIDsWithSetID(JobID)
            if tmpJobIDList == None:
                # not a jobset
                job = pbookCore.status(JobID,forceUpdate)
            else:
                # jobset
                tmpJobs = []
                for tmpJobID in tmpJobIDList:
                    tmpJob = pbookCore.status(tmpJobID,forceUpdate)
                    tmpJobs.append(tmpJob)
                # make jobset
                job = pbookCore.makeJobsetSpec(tmpJobs)
            # print
            print "======================================"
            if showPandaIDinState:
                job.flag_showSubstatus = showPandaIDinState
            if longFormat:
                job.flag_longFormat = True
            print job


    # kill
    def kill(JobID,upperJobID=None):
        """Kill all subJobs in JobID. Jobs bwtween JobID and UpperJobID will be killed if upperJobID is given

         example:
           >>> kill(15)
           >>> kill(15,20)
        """
        if upperJobID != None:
            # check range
            if JobID > upperJobID:
                tmpLog = PLogger.getPandaLogger()
                tmpLog.error("upper JobID must be larger than %s" % JobID)
            else:
                # kill jobs between the range
                for tmpJobID in range(JobID,upperJobID+1):
                    retK = pbookCore.kill(tmpJobID)
        else:            
            retK = pbookCore.kill(JobID)


    # retry
    def retry(JobID,upperJobID=None,newSite=False,newOpts={},noSubmit=False,ignoreDuplication=True):
        """Retry failed/cancelled subJobs in JobID or JobsetID. Jobs between JobID and UpperJobID will be retried if upperJobID is given. Set newSite=True when you want to retry at new site, which essentially runs pathena/prun again because new sub jobs need to be created for the new site. This means that you need to have the same runtime env (such as Athena version, run dir, source files) as the previous submission.  newOpts is a map of options and new arguments which is passed to pathena/prun. If input files were used or are being used by other jobs for the same output dataset container, those file are skipped to avoid job duplication when retrying failed subjobs. If you need to disable duplication check (e.g., you are using the same EVNT file for multiple simulation subjobs, set ignoreDuplication=True

         example:
           >>> retry(15)
           >>> retry(15,20)
           >>> retry(15,newSite=True)
           >>> retry(15,ignoreDuplication=True)
        """
        if upperJobID != None:
            # check range
            if JobID > upperJobID:
                tmpLog = PLogger.getPandaLogger()
                tmpLog.error("upper JobID must be larger than %s" % JobID)
            else:
                # retry jobs between the range
                for tmpJobID in range(JobID,upperJobID+1):
                    pbookCore.retry(tmpJobID,newSite=newSite,newOpts=newOpts,noSubmit=noSubmit,ignoreDuplication=ignoreDuplication)
        else:            
            pbookCore.retry(JobID,newSite=newSite,newOpts=newOpts,noSubmit=noSubmit,ignoreDuplication=ignoreDuplication)


    # rebrokerage
    def reassign(JobID,cloud=None):
        """Reassign JobID to another optimal site. When cloud is specified one of sites in the cloud is chosen. One can not use this command more than once within one hour 

         example:
           >>> reassign(15)
           >>> reassign(15,'CA')
        """
        pbookCore.rebrokerage(JobID,cloud)


    # delete jobs older than nDays
    def clean(nDays):
        """Delete jobs older than nDays from local database. Old jobs are automatically deleted 90 days after they were created, to keep the database size reasonable

         example:
           >>> clean(60)
        """
        pbookCore.clean(nDays)


    # kill and retry
    def killAndRetry(JobID,newSite=False,newOpts={},ignoreDuplication=False):
        """Kill JobID and then retry failed/cancelled sub-jobs. Concerning newSite and newOpts, see help(retry)

         example:
           >>> killAndRetry(15)
        """
        ret = pbookCore.killAndRetry(JobID,newSite=newSite,newOpts=newOpts,ignoreDuplication=ignoreDuplication)


    # synchronize local repository
    def sync():
        """Synchronize local repository

         example:
           >>> sync()
        """
        pbookCore.sync()


    # execut command in the batch mode
    if comString != '':
        try:
            # decompose to function name and argument
            match = re.search('^([^\(]+)\((\d*)\)$',comString)
            comName = match.group(1)
            comArg  = match.group(2)
            if comArg != '':
                # convert to int
                comArg = (long(comArg),)
            else:
                # for functions without arguments
                comArg = ()
            # exec : exec cannot be used due to unqualified exec with nested functions
            apply(locals()[comName],comArg)
        except:
            pass
        # exit
        sys.exit(0)
    # run sync
    pbookCore.sync()
    # delete old jobs
    pbookCore.clean()
    # go to interactive prompt
    code.interact(banner="\nStart pBook %s" % PandaToolsPkgInfo.release_version,
                  local=locals())


# main for GUI session
def guimain(pbookCore):
    import gtk
    from pandatools import BookGUI
    pbookGuiMain = BookGUI.PBookGuiMain(pbookCore)
    # get logger
    tmpLog = PLogger.getPandaLogger()
    tmpLog.info("Start pBook %s" % PandaToolsPkgInfo.release_version)
    # GTK main
    gtk.main()
    

# kill whole process
def catch_sig(sig, frame):
    # cleanup
    _onExit(tmpDir,historyFile)
    # kill
    commands.getoutput('kill -9 -- -%s' % os.getpgrp())


# overall main
if __name__ == "__main__":

    # parse option
    parser = optparse.OptionParser()
    parser.add_option("-v",action="store_true",dest="verbose",default=False,
                      help="verbose")
    parser.add_option("--gui",action="store_true",dest="gui",default=False,
                      help="use GUI")
    parser.add_option('-c',action='store',dest='comString',default='',type='string',
                      help='execute a command in the batch mode')
    parser.add_option("--restoreDB",action="store_true",dest="restoreDB",default=False,
                      help="restore local database")
    parser.add_option("--noPass",action="store_true",dest="noPass",default=False,
                      help="disable to enter pass phrase for GUI")
    parser.add_option('--version',action='store_const',const=True,dest='version',default=False,
                      help='Displays version')
    parser.add_option('--devSrv',action='store_const',const=True,dest='devSrv',default=False,
                      help="Please don't use this option. Only for developers to use the dev panda server")
    
    options,args = parser.parse_args()

    # display version
    if options.version:
        print "Version: %s" % PandaToolsPkgInfo.release_version
        sys.exit(0)

    # use dev server
    if options.devSrv:
        Client.useDevServer()

    # fork for Ctl-c
    fork_child_pid = os.fork()
    if fork_child_pid == -1:
        print "ERROR : Failed to fork"
        sys.exit(1)
    if fork_child_pid == 0:
        # main
        if options.gui:
            # instantiate core with pass phrase
            if options.noPass:
                pbookCore = PBookCore(False,options.verbose)
            else:
                pbookCore = PBookCore(True,options.verbose)                
            # GUI
            guimain(pbookCore)
        else:
            # instantiate core
            pbookCore = PBookCore(False,options.verbose,options.restoreDB)
            # CUI
            intmain(pbookCore,options.comString)
    else:
        # set handler
        signal.signal(signal.SIGINT, catch_sig)
        signal.signal(signal.SIGHUP, catch_sig)
        signal.signal(signal.SIGTERM,catch_sig)
        os.wait()
    
