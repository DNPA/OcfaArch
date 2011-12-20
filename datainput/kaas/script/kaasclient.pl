#!/usr/bin/perl
use strict;
use LWP::UserAgent;
use HTTP::Request::Common;
use JSON;
my $userAgent = LWP::UserAgent->new(agent => 'OcfaKaasTestPoster');
my %message;
my ($service,$inv,$source,$item,$uri) = @ARGV;
unless ($uri) {
   print STDERR "Usage: \n\tkaasclient.pl <case> <source> <item> <uri>\n";
   print STDERR "\n\nPlease note that this script is just a proof of concept script\n";
   exit(1);
}
$message{"case"} = $inv;
$message{"source"} = $source;
$message{"item"} = $item;
$message{"uri"} = $uri;
$message{"securityhash"} = "509131ffb84";
my $message = to_json(\%message);
my $response = $userAgent->request(POST "$service",
	                           Content_Type => 'application/json', Content => $message);
if ($response->is_success) {
    print "Command has been given, consult ocfa loging for results.\n";
} else {
    print $response->error_as_HTML;
}
