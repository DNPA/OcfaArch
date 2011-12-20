#!/usr/bin/perl
use Pg;
our %EXCIST;
echo "ERROR: uiconvert.pl is depricated.\n\n";
exit -1

sub getexist {
  my ($conn,$case,$old,$oldcol)=@_;
  my $len=length($old)+1;
  my $query="select table_name from information_schema.columns where table_catalog='$case' and column_name='$oldcol' and substr(table_name,0,$len)='$old'";
  print "$query\n";
  my $res=$conn->exec($query);
  while ( my ($name)=$res->fetchrow) {
      $name =~ s/^$old//;
      $EXCISTS{$name}=1;
  }
}

$case=$ENV{"OCFACASE"};
$root=$ENV{"OCFAROOT"};
unless ($case && $root) {
  die "Bad enviroment settings case='$case' root='$root'";
}
$new=$ARGV[0];
if ($new eq "row") {
  $old="meta";
  $oldcol="xml";
  $newcol="metadataid";
} elsif ($new eq "meta") {
  $old="row";
  $oldcol="metadataid";
  $newcol="xml";
} else {
  print "Invalid conversion argument, specify either 'row' or 'meta'\n\n";
  exit 1;
}
$dsmconf= $root . "/etc/dsm.conf";
$conn=Pg::connectdb("dbname=$case user=ocfa password=ocfa")|| die "Unable to connct to database\n";
unless ($conn->status()== PGRES_CONNECTION_OK) {
  $err=$conn->errorMessage();
  print "problem: $err\n";
  exit 1;
}
getexist($conn,$case,$old,$oldcol);
@keys=keys %EXCISTS;
$count=$#keys +1;
print "$count tables to be altered\n";
foreach $meta (@keys) {
  print "Altering $old$meta to $new$meta ($old${meta}.$oldcol -> $new${meta}.$newcol)\n";
  $query1="ALTER TABLE $old$meta RENAME COLUMN $oldcol TO $newcol";
  $query2="ALTER TABLE $old$meta RENAME TO $new$meta";
  $count++;
  foreach $query ($query1,$query2) {
     my $res=$conn->exec($query);
     my $err=$conn->errorMessage();
     if ($err) {
       print "problem with : $query\n$err\n";
       exit;
     }
  }
}
