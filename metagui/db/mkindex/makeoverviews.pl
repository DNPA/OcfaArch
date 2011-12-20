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
print "ERROR: makeoverviews.pl is depricated\n";
exit(-1)
use integer;
our $IMAGEMODE=-1;
our $ALLSOURCES=0;
our $EVIDENCESOURCE="";
#select distinct meta,owner from metakeyid mk left join metapubkeylist mp on mk.meta = mp.id ;
sub usage {
  print "Usage:\n";
  print "  makeoverview.pl ( -i | -m ) [-a | <evidencesource>] \n";
  print "    -m : create a metadata overview\n";
  print "    -i : create an images thumbnail matrix overview\n";
  print "    -a : instead of all evidence sources individual, create\n";
  print "         a cummulative overview over all evidence sources combined\n";
  print "    -h : show this message\n";
  print "\n\n";
  exit;
}

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

sub doimages {
    my ( $imgdir, $digestsource, $evidencesource, $item ) = @_;
    mkdir( $imgdir, 0755 );
    my ($subdircount) = 0;
    mkdir( "${imgdir}/$subdircount", 0755 );
    my ( $where1, $where2, $where );
    if ($evidencesource) {
        my $subquery = "";
        if ($item) {
            $subquery = "and item='$item'";
        }
        $where1 =
" and metadatainfo.itemid in (select itemid from item where evidencesource='$evidencesource' $subquery )";
    }
    if ($digestsource) {
        if ($digestsource eq "unprocessed") {
        $where2 = "and metadataid not in (select xml from metadigestsource)";
	} else {
        $where2 =
" and metadataid in (select xml from metadigestsource where meta='$digestsource')";
        }
    }
    $where = "$where1$where2";
    my $query =
"select dataid,casename,evidencesource,item,evidence,repname from metadatainfo,item,evidencestoreentity where metadataid in (select xml from metamimetop where meta ='image') and metadatainfo.itemid = item.itemid and id=dataid $where order by repname";
    my $equery = &shellescape($query);
    open( HTML, ">${imgdir}/${subdircount}/index.html" );
    open( QUERY, "echo $equery \\;|psql $case|" );
    my $count=-1;
    my $crap = <QUERY>;
    $crap = <QUERY>;
    while (<QUERY>) {
        s/^\s+//;
        s/\s+$//;
        my ( $mdataid, $mcase, $msrc, $mitem, $mid, $mspath ) =
          split( /\s*\|\s*/, $_ );
        if ( $mdataid && $mspath ) {
	    $count++;
	    if (($count % 8)== 0) {
               print HTML "<BR>\n";
	    }
            if ( $count >= 64 ) {
                $count = 0;
                $subdircount++;
                print HTML " <A HREF=\"../$subdircount\">Next</a>";
                close(HTML);
                mkdir( "${imgdir}/${subdircount}", 0755 );
                open( HTML, ">${imgdir}/${subdircount}/index.html" );
            }
            my ($inpath)  = "/var/ocfa/cases/${case}/repository/$mspath";
            my ($outpath) = "${imgdir}/${subdircount}/${mdataid}.png";
            my ($cmd2)    =
"cat $inpath|anytopnm |pnmscale -width 64 -height 64|pnmtopng >$outpath";
            `$cmd2`;
            print HTML
"<A HREF=\"/cgi-bin/evidence.cgi?case=${case}&src=${msrc}&item=${mitem}&id=${mid}\"><IMG src=\"${mdataid}.png\" width=\"64\" height=\"64\"></A>\n";
        }
    }
    close(HTML);
    close(QUERY);
}

sub dooverview {
    my ( $file,$caldir, $digestsource, $evidencesource, $item ) = @_;
    open( HTML, ">$file" );
    mkdir($caldir,0755);
    print HTML
"<HTML><HEAD><TITLE>OCFA Metadata database frontend $digestsource $evidencesource $item<</TITLE></HEAD><BODY bgcolor=\"#d0f0d0\">\n";
    print HTML
"<center><h3>OCFA Metadata database frontend $digestsource $evidencesource $item </h3></center>\n";
    $gendate = `date`;
    print HTML "<center><i>generated $gendate</i></center><br>\n";
    print HTML
"<TABLE border=\"1\"><TR><TH>name</TH><TH>Distinct</TH><TH>Value</TH><TH></TH><TR>\n";
    my ($arg1,$arg2,$arg3)=($digestsource, $evidencesource, $item);
    unless ($arg1) { $arg1="+";}
    unless ($arg2) { $arg2="+";}
    unless ($arg3) { $arg3="+";}
    open( PSCR, "${ocfaroot}/sbin/s1.pl $arg1 $arg2 $arg3 |psql $case|" );

    while (<PSCR>) {
        if (/^\s+MODULE:(\w+)/) {
	    print " Processing metadata from $1 module\n";
            print HTML
"<TR><TD colspan=\"4\"><font color=\"green\"><center>$1</center></font></TD></TR>\n";
        }
        elsif (/(\w*)TABLE:(\w+)\s+\|\s+(\d+)/) {
            $time = 0;
            if ( $1 eq "TIME" ) {
                $time = 1;
            }
            $table = $2;
            $count = $3;
	    print "   table=$table count=$count\n";
            $keyid = 0;
            if ( $table eq "keyid" ) {
	        #FIXME
                #$keyid = 1;
            }
            #print "$table\n";
            print HTML "<TR><TD>$table</TD><TD>$count</TD>";
            if ( $count == 0 ) {
                print HTML "<TD><b>No values</b></TD><TD></TD>";
            }
            else {
                if ( !($time) ) {
                    print HTML
"<TD><FORM action=\"/cgi-bin/lucene.cgi\" method=\"get\">\n";
                    print HTML
                      "<INPUT type=\"hidden\" name=\"tbl\" value=\"$table\">\n";
		    if ($digestsource) {
                       print HTML "<INPUT type=\"hidden\" name=\"dig\" value=\"$digestsource\">\n";
		    }
		    if ($evidencesource) {
		       print HTML "<INPUT type=\"hidden\" name=\"src\" value=\"$evidencesource\">\n";
                       if ($item) {
                         print HTML "<INPUT type=\"hidden\" name=\"item\" value=\"$item\">\n";
		       }
		    }
                    print HTML "<SELECT name=\"val\">\n";
                }
                else {
                    print HTML "<TD>";
                }
		my ($where3)="";
		if ($digestsource || $evidencesource) {
                   $where3="where ";
		}
		if ($digestsource) {
		   if ($digestsource eq "unprocessed") {
                   $where3 .= " xml not in (select xml from metadigestsource) ";
		   } else {
                   $where3 .= " xml in (select xml from metadigestsource where meta='$digestsource') ";
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
		$where3 = &shellescape($where3);
                if ($time) {
                    open( SEL1,
"echo select \\'METAVAL\\',substr\\(meta,1,4\\),count\\(\\*\\) from meta$table $where3 group by substr\\(meta,1,4\\) order by substr\\(meta,1,4\\) asc limit 1000\\;|psql $case|"
                    );
                }
                else {
                    if ($keyid) {
                        open( SEL1,
"echo select \\'METAVAL\\',meta,owner,count\\(\\*\\) from metakeyid mk $where3 left join metapubkeylist mp on mk.meta = mp.id group by mk.meta,mp.owner\\;|psql $case|"
                        );
                    }
                    else {
                        open( SEL1,
"echo select \\'METAVAL\\',meta,count\\(\\*\\) from meta$table $where3 group by meta order by count desc limit 200\\;|psql $case|"
                        );
                    }
                }
                while (<SEL1>) {
                    if (/\s+METAVAL\s+\|\s+(.*\S)\s+\|\s+(\d+)\s*/) {
                        $metaval = $1;
                        $metacount = $2;
                        ( $metaval, $extra ) = split( /\s*\|\s*/, $metaval );
                        $extra =~ s/\</_/;
                        $extra =~ s/\>/_/;
                        $truncval = $metaval;
                        if ( length($truncval) > 60 ) {
                            $truncval = substr( $metaval, 0, 55 ) . ".....";
                        }
                        $truncval =~ s/\</\&lt;/g;
                        $truncval =~ s/\>/\&gt;/g;
                        if ($time) {
			    print "        Creating calenderview for $metaval\n";
                            $htmlfile = "${metaval}${table}.html";
                            open( OUTFIL,">${caldir}/${metaval}${table}.html"
                              )
                              || die "Unable to open ${caldir}/${metaval}${table}.html for writing\n";
                            open( CALVIEW,
"${ocfaroot}/sbin/calenderview.pl $case $table $metaval $arg1 $arg2 $arg3 $digestsource|"
                              )
                              || die
                              "Cant run ${ocfaroot}/sbin/calenderview.pl";
                            while (<CALVIEW>) {
                                print OUTFIL $_;
                            }
                            close(CALVIEW);
                            close(OUTFIL);
                            print HTML
"<A HREF=\"cal/$htmlfile\">$metaval</a>($metacount) ";

                        }
                        else {
                            $metaval =~ s/\>/\&gt;/g;
                            $metaval =~ s/\</\&lt;/g;
                            $metaval =~ s/\"/\&quot;/g;
                            print HTML
"<OPTION value=\"$metaval\">($metacount) $truncval $extra</OPTION>\n";
                        }
                    }
                }
                if ( !($time) ) {
                    print HTML "</SELECT>";
                    if ( $count > 200 ) {
                        print HTML " <b>truncated</b> ";
                    }
                    print HTML
"</TD><TD><INPUT type=\"submit\" name=\"view\" value=\"view\"></TD></FORM>\n";
                }
                else {
                    print HTML "</TD><TD></TD>";
                }
            }
            print HTML "</TR>\n";
        }

    }
    print HTML "</TABLE></BODY></HTML>\n\n";
}

sub doitem {
    my ( $dir, $q1, $src, $item ) = @_;
    if ( -d $dir ) {
        print "item dir $dir exists, skipping\n";
        return 0;
    }
    print "processing item digestsource=$q1 evidencesource=$src item=$item \n";
    mkdir( $dir, 0755 );
    &dooverview( "$dir/overview.html","${dir}/cal", $q1, $src, $item );
    &doimages( "${dir}/img", $q1, $src, $item );
    return 1;
}

sub dosource {
    my ( $dir, $q1, $src ) = @_;
    my ($haschanged) = 0;
    print "processing evidencesource digestsource=$q1 evidencesource=$src \n";
    mkdir( $dir, 0755 );
    unless ($IMAGEMODE) {
        &dooverview( "$dir/overview.html","${dir}/cal", $q1, $src, "" );
    } else {
        &doimages( "${dir}/img", $q1, $src, "" );
    }
    return;
}

sub dosubtree {
    my ( $case, $dir, $q1 ) = @_;
    print "Generating subtree for $q1\n";
    mkdir( $dir, 0755 );
    open( SEL2, "echo select distinct evidencesource from item\\;|psql $case|" )
      || die;
    my $crap1      = <SEL2>;
    my $crap2      = <SEL2>;
    my $haschanged = 0;
    my @sources=();
    if ($EVIDENCESOURCE) {
      push(@sources,$EVIDENCESOURCE);
    } else {
      while (<SEL2>) {
        s/^\s+//;
        s/\s+$//;
        if (/^\w+$/) {
            push(@sources,$_);
        }
      }
    }
    unless ($ALLSOURCES) {
      my $source;
      print " Processing individual sources\n";
      foreach $source (@sources) {
       $haschanged = &dosource( "${dir}/$source",$q1,$source) || $haschanged;
      }
    } else {
        print " Processing digestsource digestsource=$q1 evidencesource=*\n";
	unless ($IMAGEMODE) {
          &dooverview( "${dir}/overview.html","${dir}/cal", $q1, "", "" );
	} else {
          &doimages( "${dir}/img", $q1, "", "" );
	}
    }
    return;
}
$case     = $ENV{"OCFACASE"};
$ocfaroot = $ENV{"OCFAROOT"};
unless ($case) {
    print "You should set OCFACASE\n";
    exit;
}
unless ($ocfaroot) {
    print "You should set OCFAROOT\n";
    exit;
}
foreach $arg (@ARGV) {
  if ($arg eq "-i") {
    $IMAGEMODE=1;
  } elsif ($arg eq "-a") {
    $ALLSOURCES=1;
  } elsif ($arg eq "-m") { 
    $IMAGEMODE=0;
  } elsif ($arg eq "-h") {
    &usage();
  } else {
    if ($EVIDENCESOURCE) {
      print "Badd arguments\n";
      &usage();
    }
    if ($arg =~ /^-/) {
      &usage();
    }
    $EVIDENCESOURCE=$arg;
  }
}
if ($EVIDENCESOURCE && $ALLSOURCES) {
  print "Bad arguments\n";
  &usage();
}
if ($IMAGEMODE == -1) {
  print "Bad arguments\n";
  &usage();
}
$| = 1;
print "Password: ";
$pass = <STDIN>;
chomp($pass);
$ENV{"PGPASSWORD"} = $pass;
$subdir            = 0;
$count             = 0;
mkdir( "${ocfaroot}/html/${case}", 0755 );
open( SEL3, "echo select distinct meta from metadigestsource\\;|psql $case|" );
$crap   = <SEL3>;
$crap   = <SEL3>;
$hasnew = &dosubtree( $case, "${ocfaroot}/html/${case}/unprocessed",
                             "unprocessed");
while (<SEL3>) {
    s/^\s+//;
    s/\s+$//;
    if (/^\w.*\w$/) {
        $hasnew = &dosubtree( $case, "${ocfaroot}/html/${case}/$_", $_ )
          || $hasnew;
    }
}
#if ($hasnew) {
#    &dosubtree( $case, "${ocfaroot}/html/${case}/all", "" );
#}
#else {
#    print "Nothing updated\n";
#}
