#!/usr/bin/perl
echo "ERROR: extratables.pl is depricated.\n\n";
exit -1
use Pg;
our %METANAMES;
our %METATYPES;
our %EXIST;
our %CLASIFICATIONS;

sub createdistinct {
  my ($conn,$case,$meta,$type)=@_;
     print "create table distinct$meta (\n";
     print "  id serial,\n";
     if ($type =~ /^time/) {
        print "  val timestamp,\n";
	print "  timesource_id int references timesource,\n";
     } elsif ($type =~ /^int/) {
        print "val int,\n";
     }
     else {
        print "  val varchar(255),\n";
     }
     print "  digestsource_id int references digestsource,\n";
     print "  cnt int,\n";
     print "  evidencesource_id int references evidencesource\n";
     print ");\n";
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
  my $query="select table_name from information_schema.columns where table_catalog='$case' and column_name='val' and substr(table_name,0,9)='distinct'";
  my $res=$conn->exec($query);
  while ( my ($name,$type)=$res->fetchrow) {
      $name =~ s/^distinct//;
      $EXIST{$name}=1;
  }
}

sub getclassifications {
   my ($conn,$case)=@_;
   my $query="select val from metaclasification";
   my $res=$con->exec($query);
   while ( my ($class)=$res->fetchrow) {
     $CLASIFICATIONS{$class}=1;  
   }
}

sub ensureclass {
   my ($conn,$case,$class)=@_;
   unless ($CLASIFICATION{$class}) {
      my $query="insert into metaclasification (val) VALUES ('$class')";
      my $res=$conn->exec($query);
      $CLASIFICATIONS{$class}=1;
   }
}

sub insertmetaname {

};
sub alterbigmeta {

};

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
  my $exist=$EXIST{$meta};
  unless ($exist) {
    print "class=$class\tmeta=$meta\ttype=$type\n";
    ensureclass($conn,$case,$class);
    createdistinct($conn,$case,$meta,$type);
    insertmetaname($meta,$type,$class);
    alterbigmeta($meta,$type);
  }
}

