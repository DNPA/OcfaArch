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

sub hasdata {
   my ($date) = @_;
   return $HASDATE{$date};
}
our %HASDATE;
our @MDAYS=(0,31,28,31,30,31,30,31,31,30,31,30,31);
our @MONTH=("","Januari","Februari","Maart","April","Mei","Juni","Juli","Augustus","September","October","November","December");
our @MSTART =(0,0,3,3,6,1,4,6,2,5,0,3,5);
our @MSTARTL=(0,0,3,4,0,2,5,0,3,6,1,4,6);
sub getmonthstart {
   my ($year,$month) =@_;
   my $mstart;
   my $leap = not ($year % 4);
   if ($leap) {
       $mstart = $MSTARTL[$month]; 
   } else {
       $mstart = $MSTART[$month];
   }
   $mstart = ($mstart +  int(($year-1)/4) + $year +6 )%7;
   
   return $mstart;
}
sub monthstart {
    my ($wday) =@_;
    my $emptycel;
    print "    <TR>";
    foreach $emptycel (1 .. $wday) {
        print "<TD></TD>";
    }
}
sub month {
   my ($year,$month,$hquery) =@_;
   my ($wday)= &getmonthstart($year,$month);
   print "  <table border=\"1\"><TR><TH colspan=\"7\">$MONTH[$month]</TH></TR><TR><th>Zo</th><th>Ma</th><th>Di</th><th>Wo</th><th>do</th><th>Vr</th><th>Za</th></tr>\n";
   &monthstart($wday);
   foreach $mday (1 .. $MDAYS[$month]) {
        $month +=0;
        if ($month < 10 ) {$month="0$month";}
	if ($mday < 10 ) {$mday ="0$mday";}
	if (&hasdata("${year}:${month}:${mday}")) {
          print "<td><b><A HREF=\"http://${CASE}.ocfa.loc/cgi-bin/lucene.cgi?tbl=${TABLE}&like=10&val=${year}:${month}:${mday}&view=view${hquery}\">$mday</A></b></td>";
	} elsif (&hasdata("$year-$month-$mday")) {
          print "<td><b><A HREF=\"http://${CASE}.ocfa.loc/cgi-bin/lucene.cgi?tbl=${TABLE}&like=10&val=$year-$month-$mday&view=view${hquery}\">$mday</A></b></td>";
	} else {
          print "<td><i>$mday</i></td>";
	}
	$wday++;
	$wday = $wday % 7;
	if ($wday == 0) {
            print "</tr>\n    ";
	}
   }
   print "</table>\n";
}
sub year {
    my ($year,$hquery) =@_;
    my ($season,$month);
    print "<TABLE border=\"0\">\n";
    foreach $season (0 .. 3) {
        print "<TR>\n";
	foreach $month ((3*$season+1) ..  (3*$season+3)) {
	    print " <TD valign=\"top\">\n";
            &month($year,$month,$hquery);
	    print " </TD>\n";
	}
	print "</TR>\n";
    }
    print "</TABLE>\n";
}
$TABLE=$ARGV[1];
$CASE=$ARGV[0];
$YEAR=$ARGV[2];
$digestsource=$ARGV[3];
$evidencesource=$ARGV[4];
if ($digestsource eq "+") {$digestsource="";}
if ($evidencesource eq "+") {$evidencesource="";}
my ($where)="";
my ($hquery)="";
if ($digestsource) {
  if ($digestsource eq "unprocessed") {
    $where .= " and xml not in \\(select xml from metadigestsource\\) ";
  } else {
    $where .= " and xml in \\(select xml from metadigestsource where meta=\\'$digestsource\\'\\) ";
  }
  $hquery="&dig=$digestsource";
}
if ($evidencesource) {
  $where .= " and xml in \\(select metadataid from metadatainfo where itemid in \\(select itemid from item where evidencesource=\\'$evidencesource\\' \\)\\)";
  $hquery .= "&src=$evidencesource";
}

unless ($ENV{"PGPASSWORD"}) {
  print STDERR "PGPASSWORD not set\n";
  exit;
}
open(PSQL,"echo select distinct substr\\(meta,0,11\\) from meta$TABLE where substr\\(meta,0,5\\) = $YEAR $where order by substr\\; |psql $CASE|") || die "Can nor run psql";
while(<PSQL>) {
   if (/(\S+)/) {
     $HASDATE{$1}=1;       
   }
}
unless ($YEAR) {
   print "Usage: \n\tcalenderview.pl <case> <table> <year>\n";
   exit;
}
print "<HTML><HEAD><TITLE>Calenderview $YEAR $CASE ($TABLE)</TITLE></HEAD><BODY bgcolor=\"#d0f0d0\">\n";
print "<center><H1>$YEAR ($TABLE)</H1>\n";
&year($YEAR,$hquery);
print "</center></BODY></HTML>\n";

