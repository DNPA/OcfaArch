#!/usr/bin/perl
use strict;
use warnings;
use Pg;
my $case=$ENV{"OCFACASE"};
my $root=$ENV{"OCFAROOT"};
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
my $table;
foreach $table ("distinctstringmeta","distinctyearmeta") {
  my $query="delete from $table where item_id=$item";
  print "$query ; \n";
  my $res=$conn->exec($query);
}
my $type;
foreach $type (1,4) {
  $table="distinctstringmeta";
  if ($type == 4) {
    $table="distinctyearmeta";
  }
  my $query="select 'insert into $table (metaname_id,item_id,digestsource,val,cnt) select distinct ' || metaname_id || ',$item,rds.meta,rmt.meta,count(*) from rowdigestsource rds right outer join row' || name || ' rmt on rds.metadataid = rmt.metadataid join metadatainfo mdi on rmt.metadataid=mdi.metadataid where itemid=$item group by rds.meta,rmt.meta;'   from metaname where scalartype=$type;";
  #print "$query\n";
  my $res=$conn->exec($query);
  while ( my ($query2)=$res->fetchrow) {
     print "$query2 ;\n";
     my $res2=$conn->exec($query2);
  }
}



