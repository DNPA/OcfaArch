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

$device=$ARGV[0];
$hddev=$ARGV[1];
$dirname=$ARGV[2];
unless ($dirname) {
  print "No nasename for output dir.\n";
  exit;
}
if (-e $dirname) {
  print "$dirname already exists\n";
  exit;
}
mkdir($dirname)||die "Unable to create $dirname\n";
open(BYTEF1,">${dirname}/net_bytesread.log")|| die "Unable to create file\n";
open(PKTF1,">${dirname}/net_packetsread.log")|| die "Unable to create file\n";
open(SIZF1,">${dirname}/net_psizeread.log")|| die "Unable to create file\n";
open(BYTEF2,">${dirname}/net_byteswritten.log")|| die "Unable to create file\n";
open(PKTF2,">${dirname}/net_packetswritten.log")|| die "Unable to create file\n";
open(SIZF2,">${dirname}/net_psizewritten.log")|| die "Unable to create file\n";
open(HD1,">${dirname}/hdread.log")||die "Unable to create file\n";
open(HD2,">${dirname}/hdwrite.log")||die "Unable to create file\n";
open(LD,">${dirname}/load.log")||die "Unable to create file\n";
open(SWP,">${dirname}/swap.log")||die "Unable to create file\n";
select(BYTEF1);$|=1;
select(PKTF1);$|=1;
select(SIZF1);$|=1;
select(BYTEF2);$|=1;
select(PKTF2);$|=1;
select(SIZF2);$|=1;
select(HD1);$|=1;
select(HD2);$|=1;
select(LD);$|=1;
select(SWP);$|=1;
select(STDOUT);
$|=1;
$count=0;
$rpsize=0;
$spsize=0;
print "Writing to log files every 15 seconds. press ctrl-c to end.\n";
while (1) {
   open(DEVF,"/proc/net/dev") || die "Can not open /proc/net/dev";
   $line="";
   while(<DEVF>) {
      if (/${device}:\s*(\d.*)/) {
        $line=$1;
      }
   }
   close(DEVF);
   open(DEVF,"/proc/diskstats")|| die "Can not open /proc/diskstats";
   $line2="";
   while(<DEVF>) {
      if (/^\s*\d+\s+\d+\s+(\S+)\s+(\d+\s+\d+)/) {
         $devname=$1;
         $tmpline=$2;
	 if ($devname =~ /$hddev/) {
            $line2=$tmpline;
	 }
      }
   }
   unless ($line) {
      print "No '$device' device  found in /proc/net/dev";
      exit;
   }
   unless ($line2) {
      print "No $hddev found in /proc/diskstats\n";
      exit;
   }
   @numbers=split(/\s+/,$line);
   if ($#oldnumbers > -1) {
       $rbytes=$numbers[0] - $oldnumbers[0];
       $sbytes=$numbers[8] - $oldnumbers[8];
       $rpckts=$numbers[1] - $oldnumbers[1];
       $spckts=$numbers[9] - $oldnumbers[9];
       if ($rbytes < 0) {$rbytes=0;}
       if ($sbytes < 0) {$sbytes=0;}
       if ($rpckts < 0) {$rpckts=0;}
       if ($spckts < 0) {$spckts=0;}
       if ($sbytes) {
         $spsize=int($sbytes / $spckts);
       }
       if ($rbytes) {
         $rpsize=int($rbytes / $rpckts);
       }
       print BYTEF1 "$count\t$rbytes\n";
       print PKTF1  "$count\t$rpckts\n";
       print SIZF1  "$count\t$rpsize\n";
       print BYTEF2 "$count\t$sbytes\n";
       print PKTF2  "$count\t$spckts\n";
       print SIZF2  "$count\t$spsize\n";
   }
   @numbers2=split(/\s+/,$line2);
   if ($#oldnumbers2 > -1) {
       $rbytes=512*($numbers2[0] - $oldnumbers2[0]);
       $wbytes=512*($numbers2[1] - $oldnumbers2[1]);
       if ($rbytes<0) {$rbytes=0;}
       if ($wbytes<0) {$wbytes=0;}
       print HD1 "$count\t$rbytes\n";
       print HD2 "$count\t$wbytes\n";
   }
   open(LOAD,"/proc/loadavg") || die "Unable to open /proc/loadavg";
   $line=<LOAD>;
   close(LOAD);
   chomp($line);
   $line =~ s/\s.*$//;
   print LD "$count\t$line\n";
   $swaptotal=0;
   $swapused=0;
   open(MEMINFO,"/proc/meminfo")|| die "Unable to open /proc/meminfo";
   while(<MEMINFO>) {
     if (/^Swaptotal:\s+(\d+)/) {$swaptotal=$1;}
     if ((/^SwapFree:\s+(\d+)/) && ($swaptotal)) {
       $swapused=$swaptotal - $1;
     }  
   }
   close(MEMINFO);
   print SWP "$count\t$swapused\n";
   sleep 15;
   $count++;
   @oldnumbers=@numbers;
   @oldnumbers2=@numbers2;
}
