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

use Pg;

print "Content-type: text/xml\n\n";
print "<?xml version=\"1.0\"?>\n";

# read environment variables
my $case=$ENV{"OCFACASE"};
my $ocfaroot=$ENV{"OCFAROOT"};

unless ($case) {
   print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
   print "<error msg=\"No OCFACASE found in webserver enviroment\"></error>\n";
   exit 0;
}
unless ($ocfaroot) {
   print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
   print "<error msg=\"No OCFAROOT found in webserver enviroment\"></error>\n";
   exit 0;
}

# Determine case config filename and check if present
my $conf_file = "/var/ocfa/cases/" . $case . "/etc/ocfa.conf";
unless (-f $conf_file) {
   print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
   print "<error msg=\"Not possible to open config file $conf_file for reading the database host ip address.\"></error>\n";
   exit 0;
}

# Read Case specific config file
my $dbhost = "";
my $dbuser = "";
my %cfg_rule;
#Get real database host from config file
open CFGFILE, "<$conf_file";
while(<CFGFILE>) {
  chomp $_;
  my ($key,$val) = split /=/, $_, 2;
  $cfg_rule{$key} = $val;
}
if(defined($cfg_rule{'storedbhost'}) &&  defined($cfg_rule{'storedbuser'})) {
  $dbhost = $cfg_rule{'storedbhost'};
  $dbuser = $cfg_rule{'storedbuser'};
} else {
   print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
   print "<error msg=\"Database host and user are NOT defined in configuration file $conf_file.\"></error>\n";
   exit 0;
}


# connect to database
$conn=Pg::connectdb("dbname=$case host=$dbhost user=$dbuser password=ocfa");
unless ($conn) {
   print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
   print "<error msg=\"Unable to connect to database $case on host $dbhost with username $dbuser.\"></error>\n";
   exit 0;
}
unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    $err =~ s/\"/'/g;
    print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
    print "<error msg=\"$err\"></error>\n";
    exit 0;
}

# construct query string
#Alternative query string, only usefull with updated stylesheet that we havn't got yet.
my $query="select '<item src=\"'||evidencesource||'\" name=\"'||item||'\" kickdone=\"true\" />' from item where hidden=false and casename='tr2' and item in (select item from  metadatainfo where evidence='0') union select '<item src=\"'||evidencesource||'\" name=\"'||item||'\" kickdone=\"false\" />' from item where hidden=false and casename='tr2' and not item in (select item from  metadatainfo where evidence='0')";
my $query="select '<item src=\"'||evidencesource||'\" name=\"'||item||'\" />' from item where hidden=false and casename='$case' order by evidencesource,item";

# execute query
my $res=$conn->exec($query);
unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    $err =~ s/\"/'/g;
    print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
    print "<error msg=\"$err\"></error>\n";
    exit 0;
}


# print output from query
print "<?xml-stylesheet type=\"text/xsl\" href=\"/index.xsl\" ?>\n";
print "<case name=\"$case\">\n";

while (my ($item)=$res->fetchrow) {
    print "$item\n";
}

print "</case>\n";


