#!/usr/bin/perl

#  The Open Computer Forensics Architecture.
#  Copyright (C) 2003..2008 KLPD  <ocfa@dnpa.nl>
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
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

use strict;
use warnings;
use Pg;

# read environment parameters
my $case=$ENV{"OCFACASE"};
unless ($case) {
   print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
   print "<error>No OCFACASE found in webserver enviroment</error>\n";
   exit 0;
}



# check if user is sure to remove evidence
my $answer = "no";
print "\n\nDANGER!\n=======\n";
print "THIS SCRIPT WILL DELETE EVIDENCE FROM THE REPOSITORY.\n";
print "Alle evidence with children from case $case will be deleted.\nBE VERY SURE TO LET THIS HAPPEN!\n\nProceed [yes|no]? ";

$answer = <STDIN>;
chomp $answer;

if($answer eq "yes"){
  print "\nYour answer was yes, so we proceed...\n";
} else {
  print "\nYour answer was: $answer\n";
  print "Script will NOT proceed. Leaving, bye, bye.\n\n";
  exit 0;
}


my %scip_mimetypes = qw(application/msword leave
                  application/pdf leave
                  application/rtf leave
                  image/jpg leave
                  video/unknown leave);

my $conn=Pg::connectdb("dbname=$case user=ocfa password=ocfa");
unless ($conn) {
   print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
   print "<error>Unable to connect to database</error>\n";
   exit 0;
}

unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
    print "<error>$err</error>\n";
    exit 0;
}


my $query ="SELECT mi.evidence, m.meta, e.repname from metadatainfo mi, rowmimetype m, evidencestoreentity e where mi.metadataid = m.metadataid and mi.dataid=e.id order by evidence";


my $res=$conn->exec($query);

unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
    print "<error>$err</error>\n";
    exit 0;
}

my $rep_name = "";
my $rep_path = "/var/ocfa/cases/" . $case . "/repository";
my $teller = 0;
my $prv="";
my $cur="";
my $prep="";
my $crep="";
my $pmeta="";
my $cmeta="";

while(my ($evidence,$meta,$repname) = $res->fetchrow) {
#  print "$evidence\t$meta\t$repname\n";

  $prv = $cur;
  $cur = $evidence;
  $prep = $crep;
  $crep = $repname;
  $pmeta = $cmeta;
  $cmeta = $meta;

  if($cur && $prv) {
     my @cjobs = split /\./, $cur;
     my $ext = pop @cjobs;
     my $part = join ".", @cjobs;
     if($prv eq $part) {
        # Previous is a part of the current, so previous evidence
        # may be deleted

        #Letop: Alle waarden van previous noodzakelijk
        if(exists($scip_mimetypes{$pmeta})) {
          #print "LEAVE evidence\n";
        } else { 
          $rep_name = $rep_path . $prep;
          print "unlink $rep_name\n";
          if(-f $rep_name) {
             unlink($rep_name);
          } else {
             print "ERROR: File doesn't exists!\n";
          }
          $teller++;
        }
     } #previous is a part of the current evidence
  } # if cur and prv

} # while evidence

#
#### possible to remove evidence with digestsource = NIST
#
my $query2 ="SELECT e.repname, d.meta FROM metadatainfo mi, evidencestoreentity e, rowdigestsource d WHERE mi.dataid=e.id AND d.metadataid=mi.metadataid AND d.meta like 'NIST%'";

my $res2=$conn->exec($query2);

unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
    print "<error>$err</error>\n";
    exit 0;
}

while(my ($repname,$meta) = $res2->fetchrow) {
#  print "$meta\t$repname\n";
  $rep_name = $rep_path . $repname;
  print "unlink $rep_name\n";
  unlink($rep_name);
}


