<?
  //Base location of this webpage (todo: autodetect somehow!)
  $baseurl='http://tkittel.web.cern.ch/tkittel/testpmb/';

  //PMB project area - for convenience
  $location_pmbhome='/afs/cern.ch/atlas/project/pmb';

  // Archive, custom archive, and index of archive:
  $location_archivebase=$location_pmbhome.'/archive/rtt';
  $location_customarchivebase=$location_pmbhome.'/archive/custom';
  $location_archive_subdir='archive';
  $location_archivebase_nosymlinks=$location_pmbhome.'/'.$location_archive_subdir;
  $location_archive_indexdir=$location_archivebase.'/index/';//fixme: trailing slash?

  //RTT jobs on afs:
  $location_rttbase='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt';

  //Autogenerated output for RTT monitoring pages:
  $location_rttmondata=$location_pmbhome.'/www/rtt-mon/data';
  $location_rttmondata_linkname='data';
  $location_rttmon_subdir='rtt-mon';

  //Oprofile results:
  $location_oprofile=$location_pmbhome.'/www/oprofile';
  $location_oprofile_linkname='oprofile';

  //Archive monitoring:
  $location_archmon=$location_pmbhome.'/archive/custom_webcache';
  $location_archmon_subdir='arch-mon';
  $location_archmon_pagedirs=Array('pages_custom','pages_custom_devel','pages_oldschool');

  //misc web areas temp and perm:
  $location_temp=$location_pmbhome.'/www/temp';
  $location_perm=$location_pmbhome.'/www/perm';


  ///////////////////////////////////////////////////////////////////////

  $sl=dirname(__FILE__).'/'.$location_rttmon_subdir.'/'.$location_rttmondata_linkname;
  if (!is_link($sl)||readlink($sl)!=$location_rttmondata) {
    print "Missing symlink. Run ln -s $location_rttmondata $sl"; 
    exit;
  }
  $sl=dirname(__FILE__).'/'.$location_oprofile_linkname;
  if (!is_link($sl)||readlink($sl)!=$location_oprofile) {
    print "Missing symlink. Run ln -s $location_oprofile $sl"; 
    exit;
  }
  $sl=dirname(__FILE__).'/'.$location_archmon_subdir.'/index_devel.php';
  $l=dirname(__FILE__).'/'.$location_archmon_subdir.'/index.php';
  if (!is_link($sl)||readlink($sl)!=$l) {
    print "Missing symlink. Run ln -s $l $sl"; 
    exit;
  }
  foreach ($location_archmon_pagedirs as $pd) {
    $sl=dirname(__FILE__).'/'.$location_archmon_subdir.'/'.$pd;
    $l=$location_archmon.'/'.$pd;
    if (!is_link($sl)||readlink($sl)!=$l) {
      print "Missing symlink. Run ln -s $l $sl";
      exit;
    }
  }
  $sl=dirname(__FILE__).'/temp';
  if (!is_link($sl)||readlink($sl)!=$location_temp) {
    print "Missing symlink. Run ln -s $location_temp $sl"; 
    exit;
  }
  $sl=dirname(__FILE__).'/perm';
  if (!is_link($sl)||readlink($sl)!=$location_perm) {
    print "Missing symlink. Run ln -s $location_perm $sl"; 
    exit;
  }

?>