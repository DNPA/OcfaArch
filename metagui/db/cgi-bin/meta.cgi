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

sub overviews {
  my ($dir,$case)=@_;
  my $hasoverview=0;
  unless (opendir(DIR1,$dir)) {
    print "Content-type: text/xml\n\n";
    print "<?xml-stylesheet type=\"text/xsl\" href=\"/error.xsl\" ?>\n";
    print "<error msg=\"No overview found for $case\">\n";
    print "  <note msg=\"The  makeoverviews.pl script has been depricated by the introduction of the DSM2\"></note>\n";
    #print "  <note msg=\"The current makeoverview will soon be replaced and is no longer being actively maintained\"></note>\n"; 
    print "</error>\n";
    return;
  }
  print "Content-type: text/html\n\n";
  my (@subdirs) = readdir(DIR1);
  closedir(DIR1);
  my $subdir;
  my @digestsource;
  my %esources;
  foreach $subdir (@subdirs) {
    if (-d "${dir}/$subdir") {
      unless ($subdir =~ /^\./) {
         push(@digestsource,$subdir);
         opendir(DIR1,"${dir}/$subdir");
         my (@subsubdirs) = readdir(DIR1);
	 closedir(DIR1);
	 my $subsubdir;
         foreach $subsubdir (@subsubdirs) {
            unless ($subsubdir =~ /^\./) {
	       if (($subsubdir eq "cal") ||($subsubdir eq "overview.html")||($subsubdir eq "img")){
                  $hasoverview=1;
	       } else {
                 $esources{$subsubdir}=1;
	       }
            }
         }
      }
    }
  }
  my @evidencesources=sort keys(%esources);
  print "<TABLE border=\"1\">\n";
  print "<TR>";
  foreach $subdir ("",@digestsource) {
      if ($subdir) {
         print "<th>digestsource=";
	 if ($subdir ne "ALL" ) {
	   print "$subdir</th>";
	 } else {
           print "*</th>";
	 }
      } elsif ($subdir eq "") {
         print "<th>evidencesource</th>";
      }
  }
  print "</TR>\n";
  my $evidencesource;
  foreach $evidencesource (@evidencesources) {
     print "<TR><TD><b>$evidencesource</b></TD>";
     my $digestsource;
     foreach $digestsource (@digestsource) {
        my $content=0;
	print "<TD>";
        if (-f "${dir}/${digestsource}/${evidencesource}/overview.html") {
          $content=1;
	  print "<A HREF=\"/${case}/${digestsource}/${evidencesource}/overview.html\">meta</A>";
	}
        if (-f "${dir}/${digestsource}/${evidencesource}/img/0/index.html") {
	  if ($content) {
             print " : ";
	  }
          $content=1;
	  print "<A HREF=\"/${case}/${digestsource}/${evidencesource}/img/0/index.html\">images</A>";
        }
	unless ($content) {
          print "<i>incomplete</i>";
	}
	print "</TD>";
     }
     print "</TR>\n";
  }
  if ($hasoverview) {
     print "<TR><TD><b>*</b></TD>";
     my $digestsource;
     foreach $digestsource (@digestsource) {
        my $content=0;
	print "<TD>";
        if (-f "${dir}/${digestsource}/overview.html") {
           $content=1;
           print "<A HREF=\"/${case}/${digestsource}/overview.html\">meta</A>";
        }
        if (-f "${dir}/${digestsource}/img/0/index.html") {
	  if ($content) {
             print " : ";
	  }
          $content=1;
	  print "<A HREF=\"/${case}/${digestsource}/img/0/index.html\">images</A>";
        }
	unless ($content) {
          print "<i>incomplete</i>";
	}
	print "</TD>";

     }
  }
  print "</TABLE>\n";
}
$host=$ENV{"HTTP_HOST"};
$host =~ s/^www\.//;
$host =~ s/\..*//;
&overviews("/usr/local/digiwash/html/$host",$host);
