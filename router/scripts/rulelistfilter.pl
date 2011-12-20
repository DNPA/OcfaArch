#!/usr/bin/perl
while(<>) {
  if (/^\s*(\d+)/) {
    $line{$1}=1;
  }
}
open(RULELIST,"etc/rulelist.csv") || die;
while(<RULELIST>) {
  $lineno++;
  if ((/ACT_COMMIT/)||(/ACT_FORWARD/)) {
    if ($line{$lineno}) {
      print "+ ";
    } else {
      print "- ";
    }
  } else {
    print "  ";
  }
  print;
}
