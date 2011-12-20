#!/usr/bin/perl
use strict;
open(LOG,"/var/log/ocfa.log")|| die "Unable to open /var/log/ocfa.log for reading";
my %reported;
while(<LOG>) {
  if (/(\w+::\w+\[\d+\]):\s*\[(\w+)\]\s?(\w.*)/) {
    my $process=$1;
    my $level=$2;
    my $logline=$3;
    if ($logline =~ /OcfaException/) {
      my $val1=$reported{$process};
      unless ($val1) {
         print;
         $reported{"$process"}=1;
      }
    }
  }
}
