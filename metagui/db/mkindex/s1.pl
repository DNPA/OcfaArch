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

sub shellescape {
    my ($input) = @_;
    my $char;
    my @tokens;
    foreach $char ( "(", ")", "'", ";" ) {
      $input .= "DummyTrailer1";
      @tokens = split( "\\$char", $input );
      $input = join( "\\$char", @tokens );
      $input =~ s/DummyTrailer1$//;
    }
    return $input;
}
							
$ocfaroot=$ENV{"OCFAROOT"};
unless ($ocfaroot) {
   die "OCFAROOT is not set";
}
($digestsource,$evidencesource,$item)=@ARGV;
if ($digestsource eq "+") {$digestsource="";}
if ($evidencesource eq "+") {$evidencesource="";}
if ($item eq "+") {$item="";}

my ($where3)="";
if ($digestsource || $evidencesource) {
                $where3="where ";
}
if ($digestsource) {
                if ($digestsource eq "unprocessed") {
                   $where3 .= "xml not in (select xml from metadigestsource) ";
		} else {
                   $where3 .= "xml in (select xml from metadigestsource where meta='$digestsource') ";
		}
		   if ($evidencesource) {
                     $where3 .= "and ";
		   }
}
if ($evidencesource) {
		   my $where4="";
		   if ($item) {
                      $where4="and item='$item'";
		   }
                   $where3 .= "xml in (select metadataid from metadatainfo where itemid in (select itemid from item where evidencesource='$evidencesource' $where4 ))";
}
#$where3 = &shellescape($where3);

open(DSMCONF,"${ocfaroot}/etc/dsm.conf")|| die "Unable to open dsm.conf for reading";
while(<DSMCONF>) {
  unless (/^#/) {
    s/\-/\_/g;
    if (/^([a-z0-8\_]+)$/) {
      $table="meta$1";
      $tn=$1;
      if (($table =~ /time/) && (!($table =~ /exposuretime/))) {
         print "select \'TIMETABLE:$tn\',count(distinct substr(meta,1,4)) from ${table} $where3;\n";
 
      } else {
         print "select \'TABLE:$tn\',count(distinct meta) from ${table} $where3;\n";
      }
    }
  }
  elsif (/^#\s+(\S.*\S)\s*#$/) {
     unless ($1 eq "pkr") {
      print  "select \'MODULE:$1\';\n";
     }
  }
}
