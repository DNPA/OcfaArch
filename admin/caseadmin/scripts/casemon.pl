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
use Sys::Syslog qw(:DEFAULT setlogsock);
use strict;
our  %W32;
our  %TGM;
our  %JAVAMODULE;
our %RUNNING;
our %BROKEN;
our %BAD;
our $OCFASBIN;
our $OCFALIB;
our %DEBUGMODULES;
# map that contains whether a certain javamodules was logged.  
our %SINGLELOGNOTICES;
sub proclookup {
  my ($modulename)=@_;
  open(PS,"ps axo pid,args|") || die "Can not start ps";
  while(<PS>) {
    s/^\s+//;
    my ($pid,$cmd,$argument)=split(/\s+/);
    $cmd =~ s/.*\///;
    if ($cmd eq "w32proxy") {
      $cmd=$argument;
    } 
    if ($cmd eq "tree"){
      $cmd=$argument;
    }
    if ($cmd eq $modulename) {
      syslog("info", "[INFO] Module found, returning pid=$pid\n");
      return $pid;
    }
  }
  return 0;
}

sub still_running {
  my ($modulename)=@_;
  my $pid=$RUNNING{$modulename};
  foreach my $look (1 .. 2) {
  open(PS,"ps o pid,args -p $pid|") || die "Can not start ps";
  while (<PS>) {
     s/^\s+//;
     my ($pid,$cmd,$argument)=split(/\s+/);
     $cmd =~ s/.*\///;
     if ($cmd eq "w32proxy") {
       $cmd=$argument;
     }
     if ($cmd eq "tree"){
      $cmd=$argument;
     }
     if ($cmd eq $modulename) {
       return 1;
     }
  }
   if ($look  == 1) {sleep(1);}
  }
  syslog("err", "[ERROR] $modulename seems to have crashed  !!!!!!!!\n");
  return 0;   
}

sub startmodule {
   my ($module,$conf,$nocredits)=@_;
   if ($module && $conf) {
      syslog("notice","[NOTICE] Starting $module using conf=$conf\n");
      my $mod=$module;
      if ($module eq "anycastrelay") {
         syslog("notice", "[NOTICE] Module is anycastrelay\n");
         $mod="anycastrelay anycast anycast";
      }
      if ($module eq "indexer") {
         syslog("info", "[INFO] Looking if we need to remove lucene lock files\n");
         opendir(TMP,"/tmp") || die "Unable to open /tmp dir for reading\n";
         my @files=readdir(TMP);
         closedir(TMP);
         my $file;
         foreach $file (@files) {
            if ($file =~ /^lucene-.*-write.lock/) {
               syslog("warning", "[WARNING] found lucene lock file /tmp/$file ,removing\n");
               unlink("/tmp/$file");
            }
         }
         syslog("notice", "[NOTICE] Lucene lockfile search done\n");
      }
      if ($W32{$module}) {
        syslog("warning", "[WARNING] Module $module is w32proxy\n");
        $mod="w32proxy $module";
      }
      if ($TGM{$module}) {
        syslog("warning", "[NOTICE] Module $module is tree module\n");
        $mod="tree $module";
      }
      my $systemarg="$OCFASBIN/$mod 2>&1 > /dev/null &";
      #my $systemarg="env OCFACONF=$conf $OCFASBIN/$mod 2>&1 > /dev/null &";
#       if ($JAVAMODULE{$module}) {
#         my $javaclass=$JAVAMODULE{$module};
#         syslog("warning", "[NOTICE] Module $module is a Java module,\n");
#         $systemarg="java -jar $OCFALIB/OcfaJavaLib.jar $javaclass 2>&1 > /dev/null &"; 
#       }
      syslog("notice","[NOTICE] Starting '$OCFASBIN/$mod'  using $conf, command='$systemarg'\n");
      system($systemarg);
      sleep(4);
      my $pid=proclookup($module);
      if ($pid == 0) {
         syslog("err", "[ERROR] Module $module crashed on startup or shortly after, marking as bad   !!!\n");
	 $BAD{$module}++;
	 #print "bad count for $module is $BAD{$module} max=3\n";
	 $RUNNING{$module}=0;
      } else {
         syslog("notice","[NOTICE] Module $module started successfully\n");
	 $RUNNING{$module}=$pid;
	 unless ($nocredits) {
	   $BAD{$module} -=10;
	 }
	 syslog("notice","[NOTICE] bad count for $module is $BAD{$module} max=3\n");
	 if ($DEBUGMODULES{$module}) {
            syslog("warning", "[WARNING] Module $module is configured for debugging, starting xterm\n");
	    system("/usr/bin/xterm -e /usr/bin/gdb $module $pid  2>&1 > /dev/null &");
	    syslog("warning", "[WARNING] Waiting for 20 seconds, please type 'cont' in the debug xterm\n");
	    sleep(20);
	    syslog("warning", "[WARNING] Continuing after sleep\n");
	 }
      }
   }
}

setlogsock('unix');
openlog("casemon.pl", 'ndelay,pid', 'user');
unless ($ENV{OCFAROOT}) {
  $ENV{OCFAROOT}="/usr/local/digiwash2.3";
}
$OCFASBIN=$ENV{OCFAROOT} . "/sbin";
$OCFALIB=$ENV{OCFAROOT} . "/lib";
$ENV{"PPQROOT"}="/var/ocfa/queues/$ARGV[0]";
if ($#ARGV == -1){
  print "Usages $0 <case name>\n";
  print "argv: ". @ARGV;
  exit(-1);
}
my $w32modulesfile= "/var/ocfa/cases/" .  $ARGV[0] . "/etc/w32modules";
my $caseConfFile =  "/var/ocfa/cases/" .  $ARGV[0] . "/etc/ocfa.conf";
if (open(W32MODULES,$w32modulesfile)) {
  while (<W32MODULES>) {
    s/\r//g;
    chomp;
    $W32{$_}=1;
  }
  close(W32MODULES);
}

my $treemodulesfile= "/var/ocfa/cases/" .  $ARGV[0] . "/etc/treemodules";
if (open(TREEMODULES,$treemodulesfile)) {
  while (<TREEMODULES>) {
    s/\r//g;
    chomp;
    $TGM{$_}=1;
  }
  close(TREEMODULES);
}

my $javamodulesfile = "/var/ocfa/cases/" .  $ARGV[0] . "/etc/javamodules";
if (open(JAVAMODULES,$javamodulesfile)) {
  while (<JAVAMODULES>) {
     s/\r//g;
     chomp;
     my ($module,$class) = split(/\s+/);
     if ($class) {
       $JAVAMODULE{$module}=$class;
     }
  }
  close(JAVAMODULES);
}

my $debugmodules = "/var/ocfa/cases/" .  $ARGV[0] . "/etc/debugmodules";
if (open(DEBUGMODULES,$debugmodules)) {
  while (<DEBUGMODULES>) {
     s/\r//g;
     chomp;
     $DEBUGMODULES{$_}=1;
  }
}
if (proclookup("anycastrelay")) {
   syslog("err", "[ERROR] There already is an anycastrelay running on this system\n");
   exit 1;
} else {
   syslog("notice", "[NOTICE] Looking if we need to remove lucene lock files\n");
   opendir(TMP,"/tmp") || die "Unable to open /tmp dir for reading\n";
   my @files=readdir(TMP);
   closedir(TMP);
   my $file;
   foreach $file (@files) {
      if ($file =~ /^lucene-.*-write.lock/) {
         syslog("warning", "[WARNING] found lucene lock file /tmp/$file ,removing\n");
	 unlink("/tmp/$file");
      }
   }
   syslog("notice", "[NOTICE] Lucene lockfile search done\n");
}
#
# start anycast
#
startmodule("anycastrelay",$caseConfFile);
sleep(1);
my $anycast=proclookup("anycastrelay");

if ($anycast == 0) {
   syslog("err", "[ERROR] started anycastrelay crashed on startup or shortly after, aborting\n");
   exit 2;
}

#
# start java casemon if library is present.
#
#
if (-f "$OCFALIB/OcfaJavaLib.jar"){
	system("java -jar $OCFALIB/OcfaJavaLib.jar &");
}
$RUNNING{"anycastrelay"}=$anycast;
while(still_running("anycastrelay")) {
  open(HAVEDATA,"$OCFASBIN/ppqhasdata|")|| die "Can not start $OCFASBIN/ppqhasdata"; 
  while (<HAVEDATA>) {
     s/\r//g;
     chomp;
     if ($_) {
       my $module=$_;
       if ($RUNNING{$module}) {
        if ($BROKEN{$module}) {
            syslog("warning","[WARNING] Skipping $module module, known to be broken\n");
	}
	else {
         if (still_running($module) == 0) {
	    if ($BAD{$module} >= 3) {
	      syslog("err", "[ERROR] The module $module has to high a crash rate, marking as bad !!!!\n");
              $BROKEN{$module}=1;
	    } else {
              syslog("err", "[ERROR] The $module module seems to have crashed, lets wait a few seconds before respawning\n");
	      sleep(5);
	      syslog("err", "[ERROR] respawning $module\n");
	      startmodule($module,$caseConfFile,1);
	      $BAD{$module}++;
	    }
	  } else {
             $BAD{$module}-=1;
	     if ($BAD{$module} < -20) {
                $BAD{$module}=-20;
	     } else {
               #print "\n$module badcount=$BAD{$module}";
	     }
	  }
	 }
       } else {
         unless  ($BROKEN{$module}) {
           syslog("notice", "[NOTICE] Data for new module $module\n");
	   if ($BAD{$module} >= 3) {
              syslog("err", "[ERROR] The module $module has to high a crash rate, marking as bad!!\n");
	      $BROKEN{$module}=1;
	   } elsif ($JAVAMODULE{$module}){
		# added check so that this message only appears once for each java module.
	     unless ($SINGLELOGNOTICES{$module}){

	       syslog("notice", "[NOTICE] The module $module will have to be started by the java casemonitor\n");
	       $SINGLELOGNOTICES{$module} = 1;	
	     }   
	   } else {
              startmodule($module,$caseConfFile);
	   }
	 }
       }
     }
  }
  select undef,undef,undef,0.05;
}
syslog("err", "[ERROR] Anycast relay has stopped\n");
exit 0;

