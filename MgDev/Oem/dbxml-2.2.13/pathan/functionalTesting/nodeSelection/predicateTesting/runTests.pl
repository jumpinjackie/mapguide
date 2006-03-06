#!/usr/bin/perl -w
#use File::Temp qw/ tempfile /;
#using File::Temp would be the correct thing to do, but is probably overkill
#and besides, it's not installed.

open PATHFILE, "< test.xpt" or die "can't open test.xpt: $!";
open BASEFILE, "< base.xml" or die "can't open base.xml: $!";
open FAILURES, "> failures.out" or die "can't open failures.out: $!";

my @paths = <PATHFILE>;
foreach (@paths) {chomp};
@paths = grep {/^[^#]/} @paths;

$/ = "%\n";
my @bases = <BASEFILE>;
foreach (@bases) {chomp};
#all your base...

print "Testing predicates";
my ($numfailed,$numtested) = (0,0);
for my $i (0..$#paths) {
#  if ($bases[$i] !=~ $paths[$i])  {
#    print "base and test files are no longer synchronized ";
#    print "at test $paths[$i]. Please regenerate base file.\n";
#    last;
#  };
  #Uncomment the next two lines to use File::Temp to generate tempfile.
  #($temphand, $tempname) = tempfile("PathanAxisXXXXX");
  #print $temphand "$bases[$i]";
  
  my $path = $paths[$i];
  if(!-e "../../../samples/xgrep/xgrep") {
    die "build xgrep you fool" ;
  }
  my $grepout=$path."\n".`../../../samples/xgrep/xgrep -f '$path' test.xml 2>&1`;

  if ($grepout eq $bases[$i]) {print ".";}
    else {
      print "!"; $numfailed++;
      print FAILURES "Failed with input $path.\n";
      print FAILURES "Expected output:\n$bases[$i]";
      print FAILURES "Actual output:\n$grepout\n";
    };
  $numtested++;
}
print "\nFailed $numfailed out of $numtested tests, details in ";
print "nodeSelection/predicateTesting/failures.out\n";
exit $numfailed;
