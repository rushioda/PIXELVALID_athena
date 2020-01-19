# echo "cleanup SCTExtension SCTExtension-r652903 in /afs/cern.ch/user/v/vcairo/WorkVMM/Material/testPackage_20/InnerDetector/InDetPerformance"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.0.0/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtSCTExtensiontempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtSCTExtensiontempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=SCTExtension -version=SCTExtension-r652903 -path=/afs/cern.ch/user/v/vcairo/WorkVMM/Material/testPackage_20/InnerDetector/InDetPerformance  -quiet -without_version_directory $* >${cmtSCTExtensiontempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=SCTExtension -version=SCTExtension-r652903 -path=/afs/cern.ch/user/v/vcairo/WorkVMM/Material/testPackage_20/InnerDetector/InDetPerformance  -quiet -without_version_directory $* >${cmtSCTExtensiontempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtSCTExtensiontempfile}
  unset cmtSCTExtensiontempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtSCTExtensiontempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtSCTExtensiontempfile}
unset cmtSCTExtensiontempfile
exit $cmtcleanupstatus

