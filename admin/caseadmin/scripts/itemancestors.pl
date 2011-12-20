#!/usr/bin/perl
use strict;
use warnings;
use Pg;
our $case=$ENV{"OCFACASE"};
our $root=$ENV{"OCFAROOT"};
my $item=$ARGV[0];
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
my $conn=Pg::connectdb("dbname=$case user=ocfa password=ocfa")|| die "Unable to connct to database\n";
unless ($conn->status()== PGRES_CONNECTION_OK) {
    my $err=$conn->errorMessage();
    print "problem: $err\n";
    exit 1;
}
my $query1="select 'insert into ancestor (metadataid,ancestorid) VALUES(' || descendant.metadataid || ',' || ancestor.metadataid || ')' from metadatainfo descendant,metadatainfo ancestor  where 
  (not ancestor.dataid is NULL) and 
  (not descendant.dataid is NULL)  and 
  ancestor.itemid=$item and 
  descendant.itemid=$item and 
  substr(descendant.evidence,0,length(ancestor.evidence)+1)=ancestor.evidence and 
  not descendant.evidence=ancestor.evidence
";
my $res=$conn->exec($query1); 
while (my ($query2)=$res->fetchrow) {
    my $res2=$conn->exec($query2);
}
print "Done\n";
