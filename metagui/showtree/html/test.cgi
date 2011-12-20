#!/usr/bin/perl
print "Content-type: text/plain\n\n";
foreach $key (keys %ENV) {
  $val = $ENV{$key};
  print "$key = $val\n";
}
