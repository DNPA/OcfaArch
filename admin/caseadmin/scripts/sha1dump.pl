#!/usr/bin/perl
use strict;
use warnings;
use Pg;
our $case=$ENV{"OCFACASE"};
our $root=$ENV{"OCFAROOT"};
unless ($case) {$case="";}
unless ($root) {$root="";}
unless ($case && $root) {
  die "Bad enviroment settings case='$case' root='$root'";
}
my $conn=Pg::connectdb("dbname=$case user=ocfa password=ocfa")|| die "Unable to connct to database\n";
unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    print "problem: $err\n";
    exit 1;
}
my $query1="select repname  from evidencestoreentity order by repname";
print "Fetching SHA1 hashes from the repository\n";
my $res=$conn->exec($query1); 
mkdir("/var/ocfa/sha1dumps",0755);
my $outfile="/var/ocfa/sha1dumps/${case}.txt";
open(OUT,">$outfile");
while (my ($repname)=$res->fetchrow) {
    $repname =~ s/\///g;
    print OUT "$repname\n";
}
close(OUT);
print "Compressing $outfile\n";
`bzip2 $outfile`;
print "Done\n";
