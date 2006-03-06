#!/usr/bin/perl -w
open LOOKUP, "FunctionLookupImpl.cpp" or die "Can't open lookup file";
my $func = "";
while (<LOOKUP>) {
    if (/insertFunction/) {
        $func = "";
    } else {
        print $func;
    }
    if (/fn:/) {
        $func = $_;
    }
    
}
