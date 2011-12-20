#!/usr/bin/perl
unless (-f "./step1.xslt") { die "Need step1.xslt to be in current directory";}
while (<>) {
  s/\r//g;
  chomp;
  unless (-f $_) {
    die "Not a valid path: $_";
  }
  open(XSLTPROC,"xsltproc step1.xslt $_|") || die "Problem starting xsltproc";
  while (<XSLTPROC>) {
    unless (/^<\?xml .*\>/) {
       print "$_";
    }
  }
}
