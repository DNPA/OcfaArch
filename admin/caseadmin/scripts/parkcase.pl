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
echo "ERROR: parkcase.pl is depricated.\n\n";
exit -1
if ($#ARGV < 1) {
   print "usage:\n\t $0 <case> <indexdirectory>\n";
   exit -1;
}
$OCFAROOT=$ENV{"OCFAROOT"};
unless ($OCFAROOT) {
  print "WARNING using default value instead of OCFAROOT\n";
  $OCFAROOT="/usr/local/digiwash/";
}
$CASE=$ARGV[0];
$indexdir=$ARGV[1];
$PARKEDCASE=$ARGV[0] . "parked";
$ENV{"OCFACASE="}=$CASE;
# park the index/
$OCFABIN="$OCFAROOT/bin";
system("$OCFABIN/parkmidas.pl $CASE $indexdir");
#$DBPASSWD=`$OCFABIN/printenv storedbpasswd`;
#$ENV{"PGPASSWORD"}=$DBPASSWD;
$OCFAUSER="ocfa";
$OCFAGROUP="ocfa";
$OCFAVAR="/var/ocfa";
$GLOBHTTPDDIR="${OCFAVAR}/httpd";
$ocfauid=(getpwnam("ocfa"))[2];
$ocfagid=(getgrnam("ocfa"))[2];
unless ($ocfagid) {
  die "no ocfa group defined on system";
}
unless ($ocfauid) {
  die "no ocfa user defined on system";
} 
#
# intialize db
print("createdb $PARKEDCASE --template $CASE");
system("createdb $PARKEDCASE --template $CASE");
#print "$tmp\n";

$PARKEDCASE_CONF_FILE="${OCFAROOT}/etc/${PARKEDCASE}.conf";
print "\n";
open(CONF,">$PARKEDCASE_CONF_FILE")|| die "Unable to open $PARKEDCASE_CONF_FILE for writing\n";
open(OLDCONF,"$OCFAROOT/etc/$CASE.conf")|| die "Unable to open $OCFAROOT/etc/template.conf for reading\n";

my $extraIndexCount;

while(<OLDCONF>) {
  print "\$_ is $_\n";
  if ($_=~ /^storedbname=/){
    
    print CONF "storedbname=$PARKEDCASE\n";
  }
  elsif ($_ =~/^extraIndexCount=/){
    
    $extraIndexCount = $';
    print "found extraindexcound ${extraIndexCount}\n";
    chomp $extraIndexCount;
    $extraIndexCount--;
  }
  elsif ($_ =~/^storedbpasswd=/){
    
    $ENV{"PGPASSWORD"} =  $';
    print "found storedbpasswd $' \n";
  }
  elsif (($_=~ /^indexdir=/)){
    
    # do nothing we will find out the new indexdir.
    #    print CONF "indexdir=$PARKEDCASEMIDASDIR\n";
    print "Doing nothing with $_\n";
  }
  else {
    
    print "copying $_\n";
    print CONF $_;
  }
}
close(OLDCONF);
if (extraIndexCount >= 0){
  print CONF "extraIndexCount=$extraIndexCount\n";
  print CONF "indexdir=$indexdir\n";
}
close(CONF);
  unless (chown(-1,$ocfagid,$PARKEDCASE_CONF_FILE)) {
    die "Unable to change group ownership of newly created $PARKEDCASE_CONF_FILE to ocfa";
  }
chown($ocfauid,-1,$PARKEDCASE_CONF_FILE);	  
#make the parked one queryable
system("$OCFABIN/queryingdb.sh $PARKEDCASE");

# create virtual host.

$VIRTUALHOST="${PARKEDCASE}.ocfa.loc";
open(VH,">${GLOBHTTPDDIR}/${PARKEDCASE}.conf")|| die "Unable to open ${GLOBHTTPDDIR}/${PARKEDCASE}.conf for writing\n";
print VH "<VirtualHost \*:80>\n";
print VH "  ServerName $VIRTUALHOST\n";
print VH "  SetEnv OCFACASE $PARKEDCASE\n";
print VH "  SetEnv OCFAROOT $OCFAROOT\n";
#print VH "  Setenv PPQROOT $PARKEDCASEPPQROOTDIR\n";
print VH "  DocumentRoot $OCFAROOT/html\n";
print VH "  ScriptAlias /cgi-bin/ $OCFAROOT/cgi-bin/\n";
print VH "  <Directory $OCFAROOT/cgi-bin/>\n";
print VH "     AllowOverride None\n";
print VH "     Options ExecCGI -MultiViews +SymLinksIfOwnerMatch\n";
print VH "     Order allow,deny\n";
print VH "     Allow from all\n";				
print VH "  </Directory>\n";
print VH "  <Directory $OCFAROOT/html/>\n";
print VH "     Options Indexes FollowSymLinks MultiViews\n";
print VH "     AllowOverride None\n";
print VH "     Order allow,deny\n";
print VH "     allow from all\n";
print VH "  </Directory>\n";
print VH "</VirtualHost>\n";
close(VH);
unless (chown(-1,$ocfagid,"${GLOBHTTPDDIR}/${PARKEDCASE}.conf")) {
  die "Unable to change group ownership of newly created ${GLOBHTTPDDIR}/${PARKEDCASE}.conf to ocfa";
}
chown($ocfauid,-1,"${GLOBHTTPDDIR}/${PARKEDCASE}.conf");

print "\n\nPlease restart apache for all changes to take effect\n";
