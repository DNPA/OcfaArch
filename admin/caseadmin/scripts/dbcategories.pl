#!/usr/bin/perl
use strict;
use warnings;
use Pg;
echo "ERROR: dbcategories is depricated.\n\n";
exit -1

my $case=$ENV{"OCFACASE"};
my $root=$ENV{"OCFAROOT"};
unless ($case) {$case="";}
unless ($root) {$root="";}
unless ($case && $root) {
  die "Bad enviroment settings case='$case' root='$root'";
}
my $dsmconf= $root . "/etc/dsm.conf";
open(DSMCONF,$dsmconf) || die "unable to open '$dsmconf'\n";
my $conn=Pg::connectdb("dbname=$case user=ocfa password=ocfa")|| die "Unable to connct to database\n";
unless ($conn->status()== PGRES_CONNECTION_OK) {
  my $err=$conn->errorMessage();
  print "problem: $err\n";
  exit 1;
}
my $scalartype;
foreach $scalartype (1..4) {
   my $dtype=" = 'character varying'";
   if ($scalartype == 2) { $dtype=" = 'integer'";}
   elsif ($scalartype == 3) { $dtype = " = 'real'";}
   elsif ($scalartype == 4) {$dtype = " like 'timestamp%'";}
   my $query="insert into metaname (name,scalartype) select  substr(table_name,4),$scalartype  from information_schema.columns where table_catalog='testcase' and column_name='meta' and substr(table_name,0,4)='row' and data_type $dtype and not substr(table_name,4) in (select name from metaname)";
   my $res=$conn->exec($query);
}

my $metagroup="";
while (<DSMCONF>) {
   if (/^#\s+(\w+.*\w+)\s+#/) {
    $metagroup=$1;
   }
   if (/^(\w\S+)/) {
     my $metaname=lc($1);
     $metaname =~ s/[^a-z0-9_]/_/g;
     my $query="insert into clasification (module,metaname_id) select '$metagroup',(select metaname_id from metaname where name='$metaname')";
     my $res=$conn->exec($query);
   }
}
close(DSMCONF);






