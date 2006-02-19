#!/usr/bin/perl


@files = glob("xgrep-test*.sh");

foreach $file (@files)
{
	$file =~ /(.+)\.sh/;
	$basename = $1;

	print "Working on $basename...";
	$cmd = "./" . $file . " > " . $basename . ".out";
	`$cmd`;
	$cmd = "diff $basename.base $basename.out";
	
	$diffs = `$cmd`;
	if($diffs ne "") {
		print "          FAILED\n";
		$failures = 1;
	} else {
		print "passed\n";
	}
}

if($failures == 1) {
	print "\nThere have been failures.\n";
} else {
	print "\nAll tests successful.\n";
	`rm *.out`;
}





