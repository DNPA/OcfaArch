#!/usr/bin/perl
my $case=$ENV{"OCFACASE"};
my $carvfsdir="/var/ocfa/cases/$case/carvfs/";
my $rawdir=$carvfsdir . "data";
my $mountdir=$carvfsdir . "mnt";
$mountcount=0;
$remountcount=0;
print "Looking if we need to do something about CarvFS.\n";
if (opendir(DIR,$rawdir)) {
  my @rawdirfiles=readdir(DIR);
  closedir(DIR);
  foreach $rawfile (@rawdirfiles) {
      if ($rawfile =~ /^(\d+)\.dd/) {
         my $rawnum=$1;
         my $rawpath=$rawdir . "/" . $rawfile;
         my $testfile="$mountdir/$rawnum/CarvFS.crv";
         unless (-f $testfile) {
            my $cmd="/usr/local/bin/carvfs $mountdir raw $rawnum $rawpath";
            $result=`$cmd`;
            unless (-f $testfile) {
               print "Invoked command: $cmd\n";
               print "$result\n";
               print "ERROR: mounting archive as carvfs failed.\n";
            } else {
               $remountcount++;
               $mountcount++;
            }
         } else {
            $mountcount++;
         }
      }
  }
}
if ($mountcount) {
  if ($remountcount == $mountcount) {
     print "Notice: CarvFS used previously, remounted archive files with carvfs to make repository consistent again.\n";
  } elsif ($remountcount) {
     print "Notice: CarvFs archive found to be mounted only partially. Remounted other parts of archive to make repository consistent again.\n";
  } elsif ($mountcount) {
     print "CarvFs is active. Reposistory CarvFs links should be consistent.\n";
  } else {
     print "No CarvFs archive found (this is not a problem, it just means that the e01cp treegraph module has never been used for this investigation).\n";
  }
}
