#!/usr/bin/perl
echo "ERROR: initoverviewdb.pl is depricated.\n\n";
exit -1
use Pg;
our %METANAMES;
our %METATYPES;

sub generics {
  print "select q1;\n";
  print "create table metaclasification (\n";
  print "  metaclasification_id serial NOT NULL,\n";
  print "  PRIMARY KEY(metaclasification_id),\n";
  print "  val varchar(255)\n";
  print ");\n";
  print "select q2;\n";
  print "create table metaname (\n";
  print "  metaname_id serial NOT NULL,\n";
  print "  PRIMARY KEY(metaname_id),\n";
  print "  name varchar(255),\n";
  print "  metaclasification_id int references metaclasification,\n";
  print "  scalartype int\n";
  print ");\n";
  print "select q3;\n";
  print "create table category (\n";
  print "   category_id serial NOT NULL,\n";
  print "   PRIMARY KEY(category_id),\n";
  print "   name varchar(255),\n";
  print "   metaname_id int references metaname,\n";
  print "   metaval varchar(255)\n";
  print ");\n";
  print "select q4;\n";
  print "create table evidencesource (\n";
  print "  evidencesource_id serial NOT NULL,\n";
  print "  PRIMARY KEY(evidencesource_id),\n";
  print "  val varchar(255)\n";
  print ");\n";
  print "select q5;\n";
  print "create table digestsource (\n";
  print "   digestsource_id serial NOT NULL,\n";
  print "   PRIMARY KEY(digestsource_id),\n";
  print "   val varchar(255)\n";
  print ");\n";
  print "select q7;\n";
  print "create table timesource (\n";
  print "   timesource_id serial NOT NULL,\n";
  print "   PRIMARY KEY(timesource_id),\n";
  print "   val varchar(255),\n";
  print "   error int\n";
  print ");\n";
  print "select q8;\n";
  print "create table timeevents (\n";
  print "  id serial,\n";
  print "  metadataid int,\n";
  print "  metaname_id int references metaname,\n";
  print "  digestsource_id int references digestsource,\n";
  print "  timesource_id int references timesource,\n";
  print "  time timestamp,\n";
  print "  evidencesource_id int references evidencesource\n";
  print ");\n";
  print "select q9;\n";
  print "create table mdistinctcount (\n";
  print "  id serial,\n";
  print "  metaname_id int references metaname,\n";
  print "  digestsource_id int references digestsource,\n";
  print "  scalartype int,\n";
  print "  category_id int references category,\n";
  print "  cnt  int,\n";
  print "  evidencesource_id int references evidencesource\n";
  print ");\n";
  print "select q10;\n";
  print "create table thumbnails (\n";
  print "  id serial,\n";
  print "  metadataid int,\n";
  print "  digestsource_id int references digestsource,\n";
  print "  thumb oid,\n";
  print "  evidencesource_id int references evidencesource\n";
  print ");\n";
  print "select q11;\n";
  print "create table parentpeer (\n";
  print "  id serial,\n";
  print "  metadataid int,\n";
  print "  parentpeer int,\n";
  print "  digestsource_id int references digestsource,\n";
  print "  evidencesource_id int references evidencesource,\n";
  print "  ppeerdigestsource_ int references digestsource(digestsource_id),\n";
  print "  ppeerevidencesource_id int references evidencesource(evidencesource_id)\n";
  print ");\n";
}
sub bigmeta {
  print "select q12;\n";
  print "create table bigmeta (\n";
  print "  id serial,\n";
  print "  metadataid int,\n";
  while (($name,$val) = each %METANAMES) {
     my ($group,$istime) = split(/:/,$val);
     if ($istime) {
       print "  m$name timestamp,\n";
     } else {
       print "  m$name varchar(255),\n";
     }
  }
  print "  evidencesource_id int references evidencesource\n";
  print ");\n";
}
sub distinct {
  while (($name,$val) = each %METANAMES) {
     print "select q13;\n";
     print "create table distinct$name (\n";
     print "  id serial,\n";
     my ($group,$istime) = split(/:/,$val);
     if ($istime) {
        print "  val timestamp,\n";
	print "  timesource_id int references timesource,\n";
     } else {
        print "  val varchar(255),\n";
     }
     print "  digestsource_id int references digestsource,\n";
     print "  cnt int,\n";
     print "  evidencesource_id int references evidencesource\n";
     print ");\n";
  }
}
sub fillmetaclasification {
  my %groups;
  while (($name,$val) = each %METANAMES) {
    my ($group,$istime) = split(/:/,$val);
    $groups{$group}=1;
  }
  foreach my $group (keys %groups) {
     print "select q14;\n";
     print "insert into metaclasification (val) VALUES ('$group');\n";
  }
};

sub fillmetaname {
  while (($name,$val) = each %METANAMES) {
    my ($group,$istime) = split(/:/,$val);
    print "select q15;\n";
    print "insert into metaname (name,scalartype,metaclasification_id) select '$name',$istime, metaclasification_id from metaclasification where val='$group';\n";
  }
}

sub setcats {

}

sub getconf {
  my ($dsmconf)=@_;
  open(DSMCONF,$dsmconf) || die "unable to open '$dsmconf'\n";
  while (<DSMCONF>) {
   if (/^#\s+(\w+.*\w+)\s+#/) {
    $metagroup=$1;
   }
   if (/^(\w\S+)/) {
     $metaname=lc($1);
     $metaname =~ s/[^a-z0-9_]/_/g;
     $METANAMES{$metaname}="$metagroup";
   }
  }
  close(DSMCONF);

}

sub gettypes {
  my ($conn,$case)=@_;
  my $query="select table_name,data_type from information_schema.columns where table_catalog='$case' and column_name='meta' and substr(table_name,0,4)='row'";
  my $res=$conn->exec($query);
  while( my ($name,$type)=$res->fetchrow) {
     $name =~ s/^row//;
     $type =~ s/\s.*//;
     $METATYPES{$name}=$type;
  }
}
sub getexist {
  my ($conn,$case)=@_;
  my $query=
}


$case=$ENV{"OCFACASE"};
$root=$ENV{"OCFAROOT"};
unless ($case && $root) {
  die "Bad enviroment settings case='$case' root='$root'";
}
$dsmconf= $root . "/etc/dsm.conf";
getconf($dsmconf);
$conn=Pg::connectdb("dbname=$case user=ocfa password=ocfa")|| die "Unable to connct to database\n";
unless ($conn->status()== PGRES_CONNECTION_OK) {
  $err=$conn->errorMessage();
  print "problem: $err\n";
  exit 1;
}
gettypes($conn,$case);
getexist($conn,$case);
foreach $meta (keys %METATYPES) {
  $type=$METATYPES{$meta};
  $class=$METANAMES{$meta};
  unless ($class) {
    $class="other";
  }
  print "class=$class\tmeta=$meta\ttype=$type\n"; 
}
#generics();
#bigmeta();
#distinct();
#fillmetaclasification();
#fillmtype();
#setcats();

