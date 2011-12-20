#!/usr/bin/perl
$OCFACASE=$ENV{"OCFACASE"};
open(SQL,"|psql ${OCFACASE} | sed -e 's/:.*//'|sort -n")|| die;
print SQL  "select distinct(substring(line,strpos(line,'line=')+5,3)) from loglines;";
close(SQL);
