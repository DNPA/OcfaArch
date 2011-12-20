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
print "ERROR: makemetaindex.pl is depricated\n";
exit(-1)
#select distinct meta,owner from metakeyid mk left join metapubkeylist mp on mk.meta = mp.id ;
$case=$ENV{"OCFACASE"};
$ocfaroot=$ENV{"OCFAROOT"};
unless ($case) {
  print "You should set OCFACASE\n";
  exit;
}
unless ($ocfaroot) {
  print "You should set OCFAROOT\n";
  exit;
}
$|=1;
print "Password: ";
$pass=<>;
chomp($pass);
$ENV{"PGPASSWORD"}=$pass;
open(PSCR,"${ocfaroot}/sbin/s1.pl |psql $case|")|| die "Unable to run ${ocfaroot}/sbin/s1.pl script";
open(HTML,">${ocfaroot}/html/meta${case}.html") || die "Unable to open ${ocfaroot}/html/meta${case}.html for writing\n";
print HTML "<HTML><HEAD><TITLE>OCFA Metadata database frontend $case</TITLE></HEAD><BODY>\n";
print HTML "<HTML><HEAD><TITLE>OCFA Metadata database frontend $case</TITLE></HEAD><BODY bgcolor=\"#d0f0d0\">\n";
print HTML "<center><h3>OCFA Metadata database frontend $case</h3></center>\n";
$gendate=`date`;
print HTML "<center><i>generated $gendate</i></center><br>\n";
print HTML "<TABLE border=\"1\"><TR><TH>name</TH><TH>Distinct</TH><TH>Value</TH><TH></TH><TR>\n";
while(<PSCR>) {
  if (/^\s+MODULE:(\w+)/) {
    print HTML "<TR><TD colspan=\"4\"><font color=\"green\"><center>$1</center></font></TD></TR>\n";
  }
  elsif (/(\w*)TABLE:(\w+)\s+\|\s+(\d+)/) {
      $time=0;
      if ($1 eq "TIME") {
        $time=1;
      }
      $table=$2;
      $count=$3;
      $keyid=0;
      if ($table eq "keyid") {
         $keyid=1;
      }
      print "$table\n";
      print HTML "<TR><TD>$table</TD><TD>$count</TD>";
      if ($count == 0) {
         print HTML "<TD><b>No values</b></TD><TD></TD>";
      }
      else {
        unless ($time) {
         print HTML "<TD><FORM action=\"cgi-bin/lucene.cgi\" method=\"get\">\n";
	 print HTML "<INPUT type=\"hidden\" name=\"tbl\" value=\"$table\">\n";
	 print HTML "<SELECT name=\"val\">\n";
        } else {
         print HTML "<TD>";
	}
	if ($time) {
           open(SEL1,"echo select \\'METAVAL\\',substr\\(meta,1,4\\),count\\(\\*\\) from meta$table group by substr\\(meta,1,4\\) order by substr\\(meta,1,4\\) asc limit 1000\\;|psql $case|"); 
	} else {
	   if ($keyid) {
              open(SEL1,"echo select \\'METAVAL\\',meta,owner,count\\(\\*\\) from metakeyid mk left join metapubkeylist mp on mk.meta = mp.id group by mk.meta,mp.owner\\;|psql $case|");
	   } else {
              open(SEL1,"echo select \\'METAVAL\\',meta,count\\(\\*\\) from meta$table group by meta order by count desc limit 200\\;|psql $case|");
	   }
	}
	while(<SEL1>) { 
	    if (/\s+METAVAL\s+\|\s+(.*\S)\s+\|\s+(\d+)\s*/) {
	      $metaval=$1;
	      ($metaval,$extra)=split(/\s*\|\s*/,$metaval);
	      $extra =~ s/\</_/;
	      $extra =~ s/\>/_/;
	      $truncval=$metaval;
	      if (length($truncval) > 60) {
                 $truncval=substr($metaval,0,55) . ".....";
	      }
	      $metacount=$2;
	      if ($time) {
	        $htmlfile="${case}_${metaval}${table}.html";
		open(OUTFIL,">${ocfaroot}/html/cal/${case}_${metaval}${table}.html")|| die "Unable to open ${ocfaroot}/html/cal/${case}_${metaval}${table}.html for writing\n";
		open(CALVIEW,"${ocfaroot}/sbin/calenderview.pl $case $table $metaval|")|| die "Cant run ${ocfaroot}/sbin/calenderview.pl";
		while(<CALVIEW>) {
                   print OUTFIL $_;
		}
		close(CALVIEW);
		close(OUTFIL);
                print HTML "<A HREF=\"cal/$htmlfile\">$metaval</a>($metacount) ";
		
	      } else {
	        $metaval =~ s/\>/\&gt;/;
		$metaval =~ s/\</\&lt;/;
                print HTML "<OPTION value=\"$metaval\">($metacount) $truncval $extra</OPTION>\n";
              }
	    }
	 }
	 unless ($time) { 
	   print HTML "</SELECT>";
	   if ($count > 200) {
             print HTML " <b>truncated</b> ";
	   }
	   print HTML "</TD><TD><INPUT type=\"submit\" name=\"view\" value=\"view\"></TD></FORM>\n";
	 } else {
           print HTML "</TD><TD></TD>";
	 }
      } 
      print HTML "</TR>\n";
  }
}
print HTML "</TABLE></BODY></HTML>\n\n"
