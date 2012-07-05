#!/usr/bin/perl
use File::Copy;
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

if ($#ARGV < 1) {
   print "usage:\n\t $0 <case> <password>\n";
   exit -1;
}
$OCFAROOT=$ENV{"OCFAROOT"};
unless ($OCFAROOT) {
  print "ERROR: environment variable OCFAROOT not set\n";
  exit -2;
}
$OCFARELEASE=$ENV{"OCFARELEASE"};
unless ($OCFARELEASE) {
  $OCFARELEASE="undefined";
}
$REALOCFAROOT=readlink($OCFAROOT);
unless ($REALOCFAROOT) {
  $REALOCFAROOT=$OCFAROOT;
}
$CASE=$ARGV[0];
$DBPASSWD=$ARGV[1];

$ENV{"PGPASSWORD"}=$DBPASSWD;
#$OCFAUSER="ocfa";
#$OCFAGROUP="ocfa";
$OCFAVAR="/var/ocfa";
$OCFACASES="${OCFAVAR}/cases";
$CASEVAR="${OCFACASES}/$CASE";
$CASEINDEXDIR="${CASEVAR}/index";
$CASERARTMPDIR="${CASEVAR}/tmp_rar";
$CASEREPDIR="${CASEVAR}/repository";
$CASETHUMDIR="${CASEVAR}/thumbnails";
$CASECARVPATHDBDIR="${CASEVAR}/carvpath_db";
$GLOBHTTPDDIR="${OCFAVAR}/httpd";
$CASEWORKDIR="${CASEVAR}/work";
$CASEHASHSETDIR="${CASEVAR}/hashsets";
$CASEPPQROOTDIR="${OCFAVAR}/queues/${CASE}";
$CASEETCDIR="${CASEVAR}/etc";
$CASELOGDIR="${CASEVAR}/log";
$LOGDIR="${CASEVAR}/log";
$OFFLINEDIR="${CASEVAR}/offline";
$OCFABIN="$OCFAROOT/bin";
$OCFAETC="$OCFAROOT/etc";
$ocfauid=(getpwnam("ocfa"))[2];
$ocfagid=(getgrnam("ocfa"))[2];
unless ($ocfagid) {
   die "no ocfa group defined on system";
}
unless ($ocfauid) {
   die "no ocfa user defined on system";
} 
sub existorcreate {
  my ($cdir)=@_;
  unless (-d $cdir) {
    unless (mkdir($cdir,0750)) {
      die "Unable to create missing $cdir dir\n";
    }
    unless (chown(-1,$ocfagid,$cdir)) {
      die "Unable to change group ownership of newly created $cdir to ocfa";
    }
    chown($ocfauid,-1,$cdir);
  }
}
sub createnoexist {
   my ($cdir)=@_;
   if (-d $cdir) {
      die "$cdir already exists";
   }
   &existorcreate($cdir);
}
#
# intialize db
$tmp=`$OCFABIN/createdb.sh $CASE`;
print "$tmp\n";
if ($?) {
  print "Problem creating database !!\n";
  return;
}

&existorcreate($OCFAVAR);
&existorcreate($OCFACASES);
&existorcreate($GLOBHTTPDDIR);
&createnoexist($CASEVAR);
&createnoexist($CASEINDEXDIR);
&createnoexist($CASERARTMPDIR);
&createnoexist($CASEREPDIR);
&createnoexist($CASETHUMDIR);
&createnoexist($CASEWORKDIR);
&createnoexist($CASECARVPATHDBDIR);
&createnoexist($LOGDIR);
&createnoexist($CASEHASHSETDIR);
&createnoexist($CASEPPQROOTDIR);
&createnoexist($CASEETCDIR);
&createnoexist($OFFLINEDIR);
$CASE_CONF_FILE="${CASEETCDIR}/ocfa.conf";
print "\n";
open(CONF,">$CASE_CONF_FILE")|| die "Unable to open $CASE_CONF_FILE for writing\n";
open(TEMPLATE,"$OCFAROOT/etc/template.conf")|| die "Unable to open $OCFAROOT/etc/template.conf for reading\n";
while(<TEMPLATE>) {
  print CONF $_;
}
close(TEMPLATE);
print CONF "# case specific settings\n";
print CONF "storedbname=$CASE\n";
print CONF "indexdir=$CASEINDEXDIR\n";
print CONF "rartmpdir=$CASERARTMPDIR\n";
print CONF "indexfile=$CASE\n";
print CONF "digestdbdirvar=$CASEHASHSETDIR\n";
print CONF "workdirroot=$CASEWORKDIR\n";
print CONF "ppqrootdir=$CASEPPQROOTDIR\n";
print CONF "varroot=$CASEVAR\n";
print CONF "logdir=$CASELOGDIR\n";
print CONF "storedbpasswd=$DBPASSWD\n";
print CONF "repository=$CASEREPDIR\n";
print CONF "thumbnails=$CASETHUMDIR\n";
close(CONF);
unless (chown(-1,$ocfagid,$CASE_CONF_FILE)) {
      die "Unable to change group ownership of newly created $CASE_CONF_FILE to ocfa";
}
chown($ocfauid,-1,$CASE_CONF_FILE);	  
copy("$OCFAETC/dsm.conf","$CASEETCDIR/dsm.conf") || die "Problem copying $OCFAETC/dsm.conf to $CASEETCDIR/dsm.conf";
copy("$OCFAETC/rulelist.csv","$CASEETCDIR/rulelist.csv") || die "Problem copying $OCFAETC/rulelist.csv to $CASEETCDIR/rulelist.csv";
if ( -f "$OCFAETC/w32modules.example" ){ copy("$OCFAETC/w32modules.example","$CASEETCDIR/w32modules");}
if ( -f "$OCFAETC/treemodules-cp.example") {
  copy("$OCFAETC/treemodules-cp.example","$CASEETCDIR/treemodules-cp");
  copy("$OCFAETC/treemodules-no-cp.example","$CASEETCDIR/treemodules-no-cp");
  my $treemodulestarget="$CASEETCDIR/treemodules-no-cp";
  my $hasscalpel=0;
  my $hasphotorec=0;
  if (-f "$REALOCFAROOT/sbin/photorec") {
     $hasphotorec=1;
  }
  if (-f "$REALOCFAROOT/lib/libcarver.so") {
     $hasscalpel=1;
  }
  if ($hasscalpel) {
      if ($hasphotorec) {
        my $input = 0;
        while (not (($input == 1) || ($input == 2))) {  
          print "\n\n\n\n";
          print "IMPORTANT: The OCFA installation was built with two different carvers:\n";
          print "  1) Photorec: A more advanced carver WITHOUT carvpath support.\n";
          print "               This carver will yield less false positives, but the lack\n";
          print "               of CarvPath support will result in slower runs and more use\n";
          print "               of diskspace.\n";
          print "  2) Scalpel:  A less advanced but CarvPath aware carver.\n";
          print "               This carver will run faster, make your runs take up less storage,\n";
          print "               but will generate more false positives and will require you to\n";
          print "               exclusively use kicktree with rawcp/e01cp module for entering evidence.\n";
          print "Choose 1 or 2 :";
          $input = <STDIN>;
        }
        if ($input == 2) {
          $treemodulestarget="$CASEETCDIR/treemodules-cp";
        }
      } else {
          print "WARNING: Using the scalpel carver (the alternative scalpel carver isn't installed)\n";
          print "         This will currently require you to exclusively use kicktree with rawcp/e01cp\n";
          print "         module for entering evidence.\n"; 
          $treemodulestarget="$CASEETCDIR/treemodules-cp";
      }
  } elsif ($hasphotorec) {
      print "NOTICE: Using the photorec carver (the alternative scalpel carver isn't installed)\n"; 
      print "        Usage of this carver  will negatively impact processing time and storage use,\n";
      print "        but should allow you more flexibility with respect to entering evidence.\n"; 
  }
  symlink($treemodulestarget,"$CASEETCDIR/treemodules");
}
if ( -f "$OCFAETC/javamodules.example"){ copy("$OCFAETC/javamodules.example","$CASEETCDIR/javamodules");}
if ( -f "$OCFAETC/debugmodules.example"){copy("$OCFAETC/debugmodules.example","$CASEETCDIR/debugmodules");}
if ( -f "$OCFAETC/scalpel.conf.example"){copy("$OCFAETC/scalpel.conf.example","$CASEETCDIR/scalpel.conf");}

open(REL,">${CASEETCDIR}/release") || die "Unable to open ${CASEETCDIR}/release for writing\n";
print REL "$OCFARELEASE\n";
close(REL);
# create virtual host.
$VIRTUALHOST="${CASE}.ocfa.loc";
open(VH,">${CASEETCDIR}/apache_vhost.conf") || die "Unable to open ${CASEETCDIR}/apache_vhost.conf for writing\n";
print VH "<VirtualHost \*:80>\n";
print VH "  ServerName $VIRTUALHOST\n";
print VH "  SetEnv OCFACASE $CASE\n";
print VH "  SetEnv OCFAROOT $OCFAROOT\n";
print VH "  Setenv PPQROOT $CASEPPQROOTDIR\n";
print VH "  DocumentRoot $REALOCFAROOT/html\n";
print VH "  ScriptAlias /cgi-bin/ $REALOCFAROOT/cgi-bin/\n";
print VH "  AddHandler cgi-script .cgi\n";
print VH "  <Directory $REALOCFAROOT/cgi-bin/>\n";
print VH "     AllowOverride None\n";
print VH "     Options ExecCGI -MultiViews +SymLinksIfOwnerMatch\n";
print VH "     Order allow,deny\n";
print VH "     Allow from all\n";				
print VH "  </Directory>\n";
print VH "  <Directory $REALOCFAROOT/html/>\n";
print VH "     DirectoryIndex  index.cgi index.html\n";
print VH "     Options Indexes FollowSymLinks MultiViews ExecCGI\n";
print VH "     AllowOverride None\n";
print VH "     Order allow,deny\n";
print VH "     allow from all\n";
print VH "  </Directory>\n";
print VH "</VirtualHost>\n";
close(VH);
unless (chown(-1,$ocfagid,"${CASEETCDIR}/apache_vhost.conf")) {
  die "Unable to change group ownership of newly created ${CASEETCDIR}/apache_vhost.conf to ocfa";
}
chown($ocfauid,-1,"${CASEETCDIR}/apache_vhost.conf");
symlink("${CASEETCDIR}/apache_vhost.conf","${GLOBHTTPDDIR}/${CASE}.conf");
print "\n\nPlease restart apache for all changes to take effect\n";
