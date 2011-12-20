#!/usr/bin/perl
unless (-f "step2.xslt") {
  die "step2.xslt should be in the current dir";
}
open(XSLTPROC,"|xsltproc step2.xslt - |xmllint --format -")|| die "Problem starting saxon";
select(XSLTPROC);
print "<?xml version='1.0'?>\n";
print "<ocfadump>\n";
print " <modlist>\n";
foreach $module ("kickstart","eekickstart","antiword","pdftotext","bzip","gzip"
                 ,"tar","zip","mailwash","sleuthkit","strings") {
  print "   <module>$module</module>\n";
}
print " </modlist>\n";
while (<>) {
  print "  $_";
}
print "</ocfadump>\n";
close XSLTPROC;
