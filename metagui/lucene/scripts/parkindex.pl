#! /usr/bin/perl

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
print "ERROR: parkindex.pl is depricated\n";
exit(-1);
use strict;
my $extraIndexCount;
my $caseConfFile;
my $indexParkDir;
my $indexDir;

unless ($ENV{OCFAROOT}) {
  $ENV{OCFAROOT}="/usr/local/digiwash";
}

if ($#ARGV < 1){

  print "ARGV is " . #$ARGV;
  print "Usages parkindex.pl <case> <parkeddirectory>\n";
  print "argv: ". @ARGV;
  exit(-1);
}
$caseConfFile = $ENV{OCFAROOT} . "/etc/" . $ARGV[0] . ".conf";
$indexParkDir = $ARGV[1];
$indexDir = "";
open(CONF, $caseConfFile) or die "cannot open $caseConfFile";
open(NEWCONF, ">/tmp/parkindex.tmp");
# plow through conf file replace extraIndexCount
$extraIndexCount = 0;
while (<CONF>){

  
  if (/^extraIndexCount\=/){

    $extraIndexCount = $';
    chomp $extraIndexCount;
  }
  else {

    if ($_ =~ /^indexdir\=/){

      print "pre is $`. Post is $'.";
      $indexDir = $';
      chomp $indexDir;
    }
    print NEWCONF $_;
  }
}
$indexDir == "" or die "couldn't find indexDir in $caseConfFile";
print ("Extraindexcount is $extraIndexCount");
# move index dir.
system("mv", $indexDir, $indexParkDir) == 0 or die "cannot move $indexDir to $indexParkDir";
system("mkdir $indexDir") == 0 or die "cannot create $indexParkDir";
# write new index stuff to $caseConffile
print (NEWCONF "indexdir" . $extraIndexCount . "=" . $indexParkDir . "\n");
$extraIndexCount++;
print (NEWCONF "extraIndexCount=" . $extraIndexCount);
close(NEWCONF); 
system("mv", $caseConfFile, $caseConfFile . ".backup") == 0 or die "cannot mv $caseConfFile";
system("mv", "/tmp/parkindex.tmp", $caseConfFile) == 0 or die "cannot mv /tmp/parkindex.tmp to $caseConfFile";
 
