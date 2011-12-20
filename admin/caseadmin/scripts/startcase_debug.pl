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
echo "ERROR: startcase_debug.pl is depricated.\n\n";
exit -1
use POSIX ":sys_wait_h";
use strict;
my $OCFASBIN=$ENV{OCFAROOT} . "/sbin";
#
# mapping from modulenames to module executables.
#
my %moduleMappings = 
  ('midas' => 'midasindexer',
   'file'  => 'jFile');

my %PIDMAP;

sub dieonsig {
   my $kid=0;
   do {
     $kid=waitpid(-1,&WNOHANG); 
     if ($kid > 0) {
        my $module=$PIDMAP{$kid};
        print STDERR "\n*******************************\n*Some module died unexpectedly*\n*            $module $kid             *\n*******************************\n";
     } 
   } until $kid == -1;
   return;
}
unless ($ENV{OCFAROOT}) {
  $ENV{OCFAROOT}="/usr/local/digiwash/bin";
}
if ($#ARGV == -1){

  print "Usages startcase.pl <case>\n";
  print "argv: ". @ARGV;
  exit(-1);
}
my $caseConfFile = $ENV{OCFAROOT} . "/etc/" . $ARGV[0] . ".conf";
my $printConfigCommand=$ENV{OCFAROOT} . "/bin/printconfig rulelist";
my $ruleListFile = `env OCFACONF=$caseConfFile $printConfigCommand`;
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
$SIG{'CHLD'}='dieonsig';
$ENV{"OCFACONF"}=$caseConfFile;
#
# start anycast
#
my $pid1;
if ($pid1=fork) {
  print STDERR "\n**************************\n*Forking child process anycast*\n**************************\n";
  $PIDMAP{$pid1}="anycast";
} elsif (defined $pid1) {
  exec("$OCFASBIN/anycastrelay anycast anycast");
  die "\nprocess spawn problem : anycast\n";
} else {
  die "\nProblem forking anycast\n";
}
sleep(2);
my $pid2;
if ($pid2= fork) {
  print STDERR "\n**************************\n*Forking child process router*\n**************************\n";
  $PIDMAP{$pid2}="router";
} elsif (defined $pid2) {
  exec("$OCFASBIN/router");
  die "\nprocess spawn problem : router\n";
} else {
  die "\nProblem forking router\n";
}
my $key;
foreach $key (sort keys %MODULE){

  #print STDERR "\nStartCase: key is " . $key . "modulemapping is " . $moduleMappings{$key} . "\n";
  if (defined $moduleMappings{$key}){

    $key = $moduleMappings{$key};
    #print "\nStartCase: key now is " . $key . "\n";
  }
  my $moduleCommand = $OCFASBIN . "/" . $key;
  my $pid;
  if ($pid = fork()) {
     print STDERR "\n**************************\n*Forking child process $key*\n**************************\n";
     $PIDMAP{$pid}=$key;     
  } elsif (defined $pid) {
     exec($moduleCommand);
     exit;
  } else {
     print "* Startcase: !!! Problem forking   *\n";
     exit;
  }
}

while ( 1 ){

  sleep(10);
}
