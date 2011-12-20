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

our %modlist=();
our $filepath="";
our $ocfaroot=$ENV{"OCFAROOT"};
sub updatemodulelist {
    opendir(PROC,"/proc")|| die "Can not open /proc dir";
    my @pids=readdir(PROC);
    my %newlist;
    foreach $key (keys %modlist) {
       $newlist{$key}=0;
    }
    foreach $pid (@pids) {
       if ($pid =~ /^\d+$/) {
          $path=readlink "/proc/${pid}/exe";
	  if ($path) {
             print "Looking at process with path=$path\n";
	  }
	  if ($path =~ /$ocfaroot/) {
             $path =~ s/.*\///;
	     $newlist{$path}=1;
	     print "Found matching path $path\n";
	  }
       }
    }
    $changed=0;
    foreach $key (keys %newlist) {
       unless ($modlist{$key}) {
         print "New $key\n";
         $changed=1;
       }
       unless ($newlist{$key}) {
          print "Died $key\n";
          $changed=1;
       }
    }
    %modlist=%newlist;
    return $changed;
}

sub writefile {
   open(STATUS,">$filepath");
   print STATUS "<HTML><HEAD><meta http-equiv=\"refresh\" content=\"10\" </HEAD><BODY><h2>Wasstraat status </h2><OL>\n";
   foreach $key (sort keys %modlist) {
      $val=$modlist{$key};
      print STATUS "<LI><FONT ";
      if ($val == 0) {
         print STATUS "color=\"red\"";
      } 
      print STATUS ">$key</font>\n";
   }
   print STATUS "</OL></BODY></HTML>";
   close STATUS;
}

unless($ocfaroot) {
   die "No OCFAROOT set";
}
$filepath="${ocfaroot}/html/status.html";
&updatemodulelist();
&writefile();
while (1) {
   if (&updatemodulelist()) {
      &writefile();
   }
   sleep(9);
}

