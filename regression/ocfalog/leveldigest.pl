#!/usr/bin/perl
use strict;
open(LOG,"/var/log/ocfa.log")|| die "Unable to open /var/log/ocfa.log for reading";
my %firstlogline;
my %processes;
my %loglevels;
while(<LOG>) {
  if (/(\w+::\w+\[\d+\]):\s*\[(\w+)\]\s?(\w.*)/) {
    my $process=$1;
    my $level=$2;
    my $logline=$3;
    $processes{$process}=1;
    $loglevels{$level}=1;
    my $val1=$firstlogline{"$process$level"};
    unless ($val1) {
         $firstlogline{"$process$level"}=$logline;
    }
  }
}

foreach my $level (keys %loglevels) {
  print "===================== LEVEL : $level ===========================\n";
  foreach my $process (sort keys %processes) {
    my $logline=$firstlogline{"$process$level"};
    if ($logline) {
       print "  $process : $logline\n";
    }
  }
}
