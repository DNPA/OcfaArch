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

sub highlight {
  my ($buf,$ishtml,$intag,@words)=@_;
  if ($ishtml) 
  {
     if ($intag) 
     {
       #If we start off inside of a tag, dont process till end of tag, 
       #and call highlight on the rest
       my ($pre,$buf2)=split(/\>/,$buf,2);
       my ($buf3,$intag)=&highlight($buf2,$ishtml,0,@words);
       $buf=$pre . ">" . $buf3;
       return ($buf,$intag);
     } else 
     {
       my (@parts)=split(/\</,$buf);
       my $buf2="";
       my $start=shift @parts;
       #Highlight anything before the first tag as plain text.
       ($buf2)=&highlight($start,0,0,@words);
       my $lintag=0;
       #Process the remaining tag/plain pairs
       foreach $part (@parts) 
       {
         if ($part =~ /\>/) 
	 {
          ($tagdata,$plaindata)=split(/\>/,$part,2);
	  #dont process till end of tag, #and call highlight on the plain text
	   my ($buf3)=&highlight($plaindata,0,0,@words);
	   $buf2 .= "<" . $tagdata . ">" . $buf3;
	   $lintag=0;
	 }
	 else 
	 {
           #If the last tag has no closing > that means we are still inside te tag
           $lintag=1;
	   $buf2 .= "<" . $part;
	 }
        }
      return($buf2,$lintag);
     }
  } 
  else 
  {
     #Highlight a plain text section.
     my $cindex;
     #Use up to 3 colors to highlight words
     @colors=("#ffff66","#a0ffff","#99ff99", "#FF9999", "#ff66ff");
     #HTML escape '<', '>' and '&'
     $buf =~ s/\&/SomeRandomString174411/g;
     $buf =~ s/SomeRandomString174411/\&amp;/g;
     $buf =~ s/\</\&lt;/g;
     $buf =~ s/\>/\&gt;/g;
     #Highlight for each of the words.
     foreach $cindex (0 .. $#words) 
     {
       my $word=$words[$cindex];
       if($word){
	 #Pick a color to highlight with
	 my $col=$colors[$cindex % 5];
	 #Get all the words to be highlighted into @wtokens
	 my (@wtokens)= ($buf =~ /$word/ig);
	 #Get all the text between the highlightable words into @stokens
	 my (@stokens)= split(/$word/i,$buf);
	 $buf="";
	 my $x;
	 #Interleave @stokens with @wtokens to re-atain the original sequence
	 foreach $x (0 .. $#stokens) {
	   $buf .= $stokens[$x];
	   #Highligth the word tokens.
	   $buf .= "\<b style=\"COLOR: black; BACKGROUND-COLOR: $col\" \>";
	   $buf .= $wtokens[$x];
	   $buf .= "\<\/b\>";
	 }
       }
     }
     return ($buf,0);
  }
}
$OCFAROOT=$ENV{"OCFAROOT"};
unless ($OCFAROOT) {
	          print "Content-type: text/html\n\n<H1>Please start your webserver with a valid OCFAROOT enviroment</H1>\n";
            exit;
}
@parts=split(/\&/,$ENV{"QUERY_STRING"});
@words=();
foreach $part (@parts) {
  ($name,$val)=split(/=/,$part);
  $name=~ s/\W//gs;
  $val=~ s/[^a-z0-9A-Z\.\/\-\+]//gs;
  if ($name eq "words") {@words=split(/\+/,$val);}
  if ($name eq "mime") {
    if ($val =~ /^(\w+\/\w+)/) {
       $mime=$1;
    }
    else {
       $mime=$val;
    }
  }
}
$dohighlight=0;
$ishtml=0;
if ($mime eq "text/html") {
   $ishtml=1;
   if ($#words > -1) { $dohighlight=1;}
}
elsif ($mime =~ /^text\//) {
   if ($#words > -1) { $dohighlight=1;}
}
unless (open(QS,"./view.cgi|")) {
  print "Content-type: text/html\n\n";
  print "OOPS\n";
  exit;
}
$dohead=1;
$intag=0;
while(read(QS,$buf,2048)) 
{
  if ($dohead) 
  {
    if ($mime) 
    {
      if (($dohighlight) && (!($ishtml))) 
      {
        print "Content-type: text/html\n\n";
    	print "<HTML><HEAD><TITLE>OCFA HIGHLIGHT</TITLE></HEAD><BODY><PRE>\n";
      } 
      else 
      {
        print "Content-type: $mime\n\n";
      }
    } 
    else 
    {
      print "Content-type: text/plain\n\n";
    }
    $dohead=0;
  }
  if ($dohighlight) 
  {
    ($buf,$intag)=&highlight($buf,$ishtml,$intag,@words);
  }
  print STDOUT $buf;
}
if ($dohead) 
{
  print "Content-type: text/html\n\n<H3>NO DATA</H3>\n";
  print "./view.cgi $QUERY_STRING \n";
}
else 
{
  if (($dohighlight) && (!($ishtml))) 
  {
     print "</PRE></BODY></HTML>\n";
  }
}
