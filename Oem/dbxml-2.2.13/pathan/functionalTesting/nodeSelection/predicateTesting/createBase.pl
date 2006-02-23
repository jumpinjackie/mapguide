#!/usr/bin/perl -w

open PATHFILE, "< test.xpt" or die "can't open test.xpt: $!";
while ($path = <PATHFILE>) {
	next if $path =~ /^#/;
	chomp($path);
	print "$path\n";
	print `../../../samples/xgrep/xgrep @ARGV '$path' test.xml 2>&1`;
	print "%\n";
}
