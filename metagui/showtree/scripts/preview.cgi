#! /usr/bin/perl

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

$OCFAROOT=$ENV{"OCFAROOT"};
unless ($OCFAROOT) {
	          print "Content-type: text/html\n\n<H1>Please start your webserver with a valid OCFAROOT enviroment</H1>\n";
		  exit;
}
$TOOLDIR="${OCFAROOT}/bin";
unless (-d $TOOLDIR) {
                print "Content-type: text/html\n\n<H1>Tooldir $TOOLDIR not available</H1>\n";
            exit;
}
@parts=split(/\&/,$ENV{"QUERY_STRING"});
foreach $part (@parts) {
  ($name,$val)=split(/=/,$part);
  $name=~ s/\W//gs;
  $val=~ s/[^a-z0-9A-Z\.]//gs;
  if ($name eq "case") { $case=$val;}
  if ($name eq "src") {$src=$val;}
  if ($name eq "item") {$item=$val;}
  if ($name eq "id") {$id=$val;}
  if ($name eq "mime") {$mime=$val;}
}
my $command = "${TOOLDIR}/showdatapath $case $src $item $id";
#print $command;
unless (-f qx<$command>) {
  print "Location: /icons/broken.png\n\n";
  exit;
}
if ($mime =~ /^text/) {
  print "Location: /icons/text.png\n\n";
}
elsif ($mime =~ /^image/) {
  print "Location: /icons/image1.png\n\n";
}
elsif ($mime =~ /^audio/) {
    print "Location: /icons/sound2.png\n\n";
  }
elsif ($mime =~ /^video/) {
  print "Location: /icons/movie.png\n\n";
}
  elsif ($mime =~ /^application/) {
    print "Location: /icons/binary.png\n\n";
  }
elsif ($mime =~ /^message/) {
  print "Location: /icons/quill.png\n\n";
  } 
else {
  print "Location: /icons/unknown.png\n\n";
}


