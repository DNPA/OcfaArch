#! /usr/bin/perl
use strict;
use DBI;
if (!defined($ARGV[0])){

  print "Usage $0 <case>";
  exit (-1);
}

print <<EOF;
  print <<EOF;
<html> 
  <head>
     <title>Mimetype overview for $ARGV[0]</title>
  </head>
<body>
EOF

open(MIMETYPES, "mime.types");
while (<MIMETYPES>){ 



  giveExtensionOverview($_, $ARGV[0]);   

}  
  print <<EOF;
</body>
</html>
EOF


sub giveExtensionOverview($$){

  my $line = shift;
  my $case = shift;
  my @mimetypeline = split;
  my $mimetype = shift @mimetypeline;
  if ($#mimetypeline >= 0){
    
    print "<h3>Metamimetypes for $mimetype</h3>";
     
    
    giveAnExtensionOverview($case, $mimetype, @mimetypeline);
    
  }    

}


sub giveAnExtensionOverview($$;@){

  my $case = shift;
  my $mimetype = shift;
  my @extensionlist = @_;

  print <<EOF;
<h2> Talking about $mimetype</h2>
<h3>giving extensions for $mimetype whose files do not have mimetype $mimetype</h3>
<table>
<tr>
<td>extension</td><td>mimetype</td><td>count</td>
</tr>
EOF

  my $query = "select count(*), metafileextension.meta, metamimetype.meta from  metadatainfo inner join item on metadatainfo.itemid = item.itemid "
    . " inner join metamimetype on metamimetype.xml = metadatainfo.metadataid "
  	. " inner join metafileextension on metadatainfo.metadataid = metafileextension.xml "
 	  . "where metamimetype.meta != '" . $mimetype . "' and (metafileextension.meta = '" . $extensionlist[0] ."'";

  #shift @extensionlist;
  my $extension; 
  
  foreach $extension (@extensionlist[1,$#extensionlist]){
    
    $query = $query  . " or  metafileextension.meta = '" . $extension . "'";
  }
  $query = $query . ") group by metafileextension.meta, metamimetype.meta order by metafileextension.meta; ";
 #print "query is $query\n";
  my $dbh  = DBI->connect("dbi:Pg:dbname=$case", "", "");
  my $sth = $dbh->prepare($query);
  $sth->execute();
  while (my @row = $sth->fetchrow_array()) {
    
    print "<tr>";
    if ($row[0] == 0){

      #print "<td> $row[1]</td><td>$row[2]</td><td>$row[0]</td>\n";
    }
    else {
      my $link = $row[1] . $row[2];
      $link =~ s/\//_/g;
      $link = $case . "/". $link . ".html";
      createLinkPage($case, $row[1], $row[2], $link, $dbh);
       print "<td> $row[1]</td><td>$row[2]</td><td><a href=\"$link\">$row[0]</a></td></tr>\n";
    }
  }
    print "</table>\n";

  print <<EOF;
  <h4> Files met mimetype $mimetype en extensie:</h4>
  <table>
    <tr><td>Extension</td><td>count</td>
EOF

  $query = "select count(*), metafileextension.meta from  metadatainfo inner join item on metadatainfo.itemid = item.itemid "
    . " inner join metamimetype on metamimetype.xml = metadatainfo.metadataid "
  	. " inner join metafileextension on metadatainfo.metadataid = metafileextension.xml "
 	  . " where metamimetype.meta = '" . $mimetype . "'"
            . " group by metafileextension.meta order by metafileextension.meta;";
  my $sth = $dbh->prepare($query);
  $sth->execute();
#  print "\n query is $query\n\n";
  while (my @row = $sth->fetchrow_array()) {

    my $color;
    if (isInList($row[1], @extensionlist) == 1){

      print "<tr><td fgcolor=\"#00FF00\"> $row[1]</td><td style=\"color: green\">$row[0]</td></tr>\n";
    }
    else {
      my $link = $mimetype . $row[1];
      $link =~ s/\//_/g;
      $link = $case . "/" . $link . ".html";
      print "<tr><td fgcolor=\"#FF0000\">$row[1]</td><td ><a href=\"$link\" style=\"color: red\">$row[0]</a></td></tr>\n";
      createLinkPage($case, $mimetype, $row[1], $link, $dbh);
    }
  }
  print "</table>\n";
}

sub createLinkPage($$$$$){

  my $case = shift;
  my $mimetype = shift;
  my $extension = shift;
  my $linkpage = shift;
  my $dbh = shift;
  my $query;
  open (LINKPAGE, ">" . $linkpage);
  print LINKPAGE <<EOF;
<html><head><title>evidences with type $mimetype and extension $extension </title></head>
<body>
<h1> evidences with type $mimetype and extension $extension</h1>
<table>
EOF

    $query = "select item.casename as case, item.evidencesource as source, item.item as item, metadatainfo.evidence, metadatainfo.location from "
      . "metadatainfo inner join item on metadatainfo.itemid = item.itemid "
	. " inner join metamimetype on metamimetype.xml = metadatainfo.metadataid "
	  . " inner join metafileextension on metadatainfo.metadataid = metafileextension.xml "
	    . " where metafileextension.meta = '" . $extension . "' and metamimetype.meta = '"
	      . $mimetype . "'";
  my $sth = $dbh->prepare($query);
  $sth->execute();
#  print LINKPAGE "\n $query is $query\n";
  while (my $rowref = $sth->fetchrow_hashref()) {
    
    my $link = "/cgi-bin/evidence.cgi?case=" . $rowref->{"case"} . "&src=" . $rowref->{"source"}
      . "&item=" . $rowref->{"item"} . "&id=" . $rowref->{"evidence"};
    print LINKPAGE "<tr><td><a href=\"$link\">" . $rowref->{"location"} . "</a></td></tr>";
  }
  print LINKPAGE "</table></body></html>\n";
  close LINKPAGE;
}
  #group by metafileextension.meta order by metafileextension.meta;";
  
  
  
  # my $queryTemplate;
 #  $queryTemplate = "select item.casename, item.evidencesource, item.item, metadatainfo.evidence, metadatainfo.location from "
#      . "metadatainfo inner join item on metadatainfo.itemid = item.itemid "
#       . " inner join metamimetype on metamimetype.xml = metadatainfo.metadataid "
#. " inner join metafileextension on metafileextension.xml = metafileextension.xml "
#group by metafileextension.meta order by metafileextension.meta;";


sub isInList($@){

  my $element = shift;
  my @list = @_;
  
  my $listElement;
  #print "<p> checking $element with @list \n";
  foreach $listElement (@list){

    if ($element eq $listElement){

   #   print "$element == $listElement</p>";
      return 1;
    }
  }
    return 0;
}
