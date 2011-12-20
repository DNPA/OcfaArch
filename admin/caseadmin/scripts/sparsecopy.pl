#!/usr/bin/perl
use strict;
use IO::Seekable;
use FileHandle;
sub usage {
   print "Usage:\n\tsparsecopy.pl <sourcefile> <destinationfile>\n\n";
}
my $from=$ARGV[0];
my $to=$ARGV[1];
unless (-f $from) { &usage(); exit 3 }
if (-f $to) {
   print "$to already exists\n";
   exit 1;
}
my @stat=stat($from);
my $fullsize=$stat[7];
my $blcksize=512;
my $blocks=int(($fullsize+511)/512);
unless ($blocks) {
  print "Problem statting $from\n";
  exit 4;
}
unless (open(ZERO,"/dev/zero")) {
  print "Unable to open /dev/zero\n";
  exit 2;
}
my $nullblock;
read(ZERO,$nullblock,$blcksize);
close(ZERO);
unless (sysopen(INFIL,$from,O_RDONLY|O_LARGEFILE)) {
  print "Problem opening source $from\n";
  exit 6;
}
unless (sysopen(OUTFIL,$to,O_WRONLY | O_CREAT | O_EXCL| O_LARGEFILE)) {
  print "Problem opening destination $to $? $!\n";
  exit 7;
}
my $sparsecount=0;
$|=1;
print "copying $blocks blocks x $blcksize ($fullsize)\n";
foreach my $blcount ( 1 .. ($blocks -1)) {
   if (($blcount % 10000) == 0) {
     print "\r$blcount\t$sparsecount";
   }
   my $datablock;
   my $len=sysread(INFIL,$datablock,$blcksize);
   if ($len != $blcksize) {
      print "Only $len bytes available where $blcksize were requested (block $blcount)\n";
   }
   if ($datablock eq $nullblock) {
      sysseek(OUTFIL,$blcksize,SEEK_CUR);
      $sparsecount++;
   } else {
      syswrite(OUTFIL,$datablock,$blcksize);
   }
}
my $percentage=int($sparsecount*10000 / $blocks)/100;
print "\r$blocks\t$sparsecount sparse  ($percentage \%)\n";
my $datablock;
sysread(INFIL,$datablock,$blcksize);
syswrite(OUTFIL,$datablock,$blcksize);
close(INFIL);
close(OUTFIL);
