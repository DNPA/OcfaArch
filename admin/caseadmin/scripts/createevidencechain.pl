#!/usr/bin/perl

use strict;
use warnings;
use Pg;
our $case=$ENV{"OCFACASE"};
our $root=$ENV{"OCFAROOT"};
my $item=$ARGV[0];
my $user=$ARGV[1] || "ocfa";
my $passwd=$ARGV[2] || "ocfa";
unless ($case) {$case="";}
unless ($root) {$root="";}
unless ($case && $root) {
  die "Bad enviroment settings case='$case' root='$root'";
}
unless ($item) {
  print "No item id specified\n";
  exit 1;
}
unless ($item =~ /^\d+$/) {
  print "Invalid item id format, should be a number\n";
  exit 1;
}
my $connectstr = "dbname=\"$case\" user=$user password=$passwd";
print "ConnectString: $connectstr\n"; 
my $conn=Pg::connectdb($connectstr)|| die "Unable to connct to database\n";
unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    print "problem: $err\n";
    exit 1;
}
my $query1="select evidence, metadataid from metadatainfo where itemid = $item order by evidence";

sub isPrefix($$){
 my $prefix = shift;
 my $str = shift;
 return ($str =~ /^${prefix}\..*/);
}

my @stack;
my $top;
my $i=0;
my $evidence;
my $metadataid;
my $res=$conn->exec($query1);
while (($evidence, $metadataid) = $res->fetchrow) {
  chomp $evidence;
  if (@stack > 0) {
    while (!isPrefix($stack[@stack-1]->{node},$evidence)){
      my %tmp = %{pop @stack};
      $conn->exec("INSERT INTO evidencechain(metadataid, itemid, leftix, rightix) VALUES( '$tmp{metadataid}', $item,  $tmp{left}, $i)");
      if (my $err = $conn->errorMessage()){
	print "$err";
      }
      $i++;
    }
  }
  push @stack, {node => $evidence, metadataid => $metadataid, left => $i};
  $i++;
}
if (@stack > 0) {
  do {
    my %tmp = %{pop @stack};
    $conn->exec("INSERT INTO evidencechain(metadataid, itemid, leftix, rightix) VALUES( '$tmp{metadataid}', $item, $tmp{left}, $i)");
    if (my $err = $conn->errorMessage()){
      print "$err";
    }
    $i++;
  } until (@stack == 0);
}


print "Done\n";
