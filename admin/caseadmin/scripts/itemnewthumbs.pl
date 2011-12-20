#!/usr/bin/perl
use strict;
use warnings;
use Pg;
our $case=$ENV{"OCFACASE"};
our $root=$ENV{"OCFAROOT"};
sub makethumb {
  my ($repname)=@_;
  unless (-f "/var/ocfa/cases/$case/thumbnails$repname") {
    print "Processing $repname\n";
    my($root,$d1,$d2,$file)=split('/',$repname);
    mkdir("/var/ocfa/cases/$case/thumbnails",0750);
    mkdir("/var/ocfa/cases/$case/thumbnails/$d1",0750);
    mkdir("/var/ocfa/cases/$case/thumbnails/$d1/$d2",0750);
    `/usr/bin/anytopnm /var/ocfa/cases/$case/repository$repname | pnmscale -width 64 -height 64|/usr/bin/pnmtopng > /var/ocfa/cases/$case/thumbnails$repname`;
  } else {
    print "file $repname exists\n";
  }
}
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
my $query1="
select mse.repname,'insert into thumbnails (metadataid,digestsource,item_id,repname) VALUES('|| rds.metadataid || ',\\''|| rds.meta || '\\',$item,\\'' || mse.repname || '\\')' 
    from rowdigestsource rds 
    right outer join 
         rowmimetop  rmt on rds.metadataid = rmt.metadataid 
    join 
         metadatainfo mdi on rmt.metadataid=mdi.metadataid 
    join 
         metastoreentity mse on mse.id=rmt.metadataid 
    where 
         itemid=$item and 
	 rmt.meta='image' and 
	 not mse.repname in (select repname from thumbnails) 
union select '','insert into thumbnails (metadataid,digestsource,item_id,repname) VALUES('|| rds.metadataid || ',\\''|| rds.meta || '\\',$item,\\'' || mse.repname || '\\')' 
   from rowdigestsource rds 
   right outer join 
        rowmimetop  rmt on rds.metadataid = rmt.metadataid 
   join 
        metadatainfo mdi on rmt.metadataid=mdi.metadataid 
   join 
        metastoreentity mse on mse.id=rmt.metadataid 
   where 
        itemid=$item and 
	rmt.meta='image' and 
	mse.repname in (select repname from thumbnails)  
group by rds.meta,rds.metadataid,mse.repname";

my $res=$conn->exec($query1); 
while (my ($repname,$query2)=$res->fetchrow) {
    if ($repname) {
       &makethumb($repname);
    } else {
       print "skipping known data\n";
    }
    my $res2=$conn->exec($query2);
}
print "Done\n";
