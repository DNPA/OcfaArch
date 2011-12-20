#!/usr/bin/perl

#  The Open Computer Forensics Architecture.
#  Copyright (C) 2003..2006 KLPD  <ocfa@dnpa.nl>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

use strict;
echo "ERROR: startcase.pl is depricated, you may want to use casemon.pl instead\n\n";
exit -1
my $OCFASBIN=$ENV{OCFAROOT} . "/sbin";



unless ($ENV{OCFAROOT}) {
  $ENV{OCFAROOT}="/usr/local/digiwash/bin";
}
if ($#ARGV == -1){

  print "Usages $0 <case name>\n";
  print "argv: ". @ARGV;
  exit(-1);
}
#my $w32modulesfile= $ENV{OCFAROOT} . "/etc/w32modules";
#my $caseConfFile = $ENV{OCFAROOT} . "/etc/" . $ARGV[0] . ".conf";
my $w32modulesfile= "/var/ocfa/" . $ARGV[0] . "/etc/w32modules";
my $caseConfFile = "/var/ocfa/" . $ARGV[0] . "/etc/ocfa.conf";
my $printConfigCommand=$ENV{OCFAROOT} . "/bin/printconfig rulelist";
my $ruleListFile = `env OCFACONF=$caseConfFile $printConfigCommand`;
my %W32;
open(W32MODULES,$w32modulesfile) || die "Can not open $w32modulesfile";
while (<W32MODULES>) {
  s/\r//g;
  chomp;
  $W32{$_}=1;
}
print "\n\n\n\n\n\nNOTE: startcase.pl is depricated, you may want to use casemon.pl instead\n\n";
sleep(6);
print "rulelist file is " . $ruleListFile;
open(CONF, $ruleListFile) || die "Can not open $ruleListFile";
my $head;
my %MODULE;
$head=<CONF>;
while (<CONF>){

  my $line;
  my @line; 
  $line=$_;
  unless ($line =~ /^#/) {
    chop;
    @line=split(/\;/,$_);
    if ($line[6]) {
      $MODULE{$line[6]}=1;
    }
  }
}
#
# start anycast
#
system("env OCFACONF=$caseConfFile $OCFASBIN/anycastrelay anycast anycast 2>&1 > /dev/null &");
sleep(2);
print "starting router\n";
system("env OCFACONF=$caseConfFile $OCFASBIN/router 2>&1 > /dev/null &");
print "old router started\n";
my $key;
foreach $key (sort keys %MODULE){

  print "\nStartCase: key is $key \n";
  my $moduleCommand;
  if ($W32{$key}) {
    print "$key was found to be a win32 module, using proxy\n";
    $moduleCommand= $OCFASBIN . "/w32proxy " . $key;
  } else {
    $moduleCommand= $OCFASBIN . "/" . $key;
  }
  system("env OCFACONF=$caseConfFile $moduleCommand 2>&1 >/dev/null &");
}

