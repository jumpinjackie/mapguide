#!/usr/bin/perl

sub match_dir {

    local($src, $dest, @patterns) = @_;
    local(@files);
    local($file, $pattern);

    @files = `cd $src && find . -type 'd'`;

  FILE:
    foreach $file (@files) {
	
	# Check patterns to discard directories

	foreach $pattern (@patterns) {
	    if($file =~ /$pattern/) {
		next FILE;
	    }
	}
	
	# If we have not been discarded, do the copy

	$srcfile = $file;
	chop $srcfile;

	$destfile = $dest . '/' . $srcfile;

	print "../util/mkinstalldirs $destfile\n";
	system("../util/mkinstalldirs $destfile"); 
    }
}

sub match_file {

    local($src, $dest, @patterns) = @_;
    local(@files);
    local($file, $pattern);

    @files = `cd $src && find . -name '*'`;

  FILE:
    foreach $file (@files) {
	
	# Check patterns to discard directories

	foreach $pattern (@patterns) {
	    if($file =~ /$pattern/) {
		next FILE;
	    }
	}
	
	# If we have not been discarded, do the copy

	$srcfile = $file;
	chop $srcfile;

	$destfile = $dest . '/' . $srcfile;

	print "cp ../$srcfile $destfile\n";
	system("cp ../$srcfile $destfile"); 
    }
}

# Main function begins here

my $version = 2;
my $subversion = "0beta";

my $root = "libpathan-${version}.${subversion}";

my @directory_patterns = (
			  # Discard CVS directories
			  "/CVS",
			  # the distribution scripts directory
			  "/distrib",
			  # The doc/internal documentation
			  "/docs/internal",
			  # Hack for some windows libs lying around the CVS
			  "/lib",
              # Or other libs
              ".libs"
			  );

my @file_patterns =      (
			  # Discard files in CVS directories
			  "/CVS",
              # build things
              ".cache",
			  # TMAKE files
			  ".pro",
			  # The distrib/ scripts
			  "/distrib",
			  # The doc/internal documentation
			  "/docs/internal",
			  # DSLrunConfigure 
			  "DSLrunConfigure",
              # VC files
              ".obj",
              ".pdb",
              ".pch",
              ".idb",
			  # Hack for some windows libs lying around the CVS
			  "/lib"
			  );

# Build full source distribution

print "Building full SOURCE distribution...\n\n";

# Clean the distribution tree

if($ENV{'XERCESCROOT'} eq "") {
    die "\$XERCESCROOT not defined\n";
}

if($ENV{'TMAKEPATH'} eq "") {
    die "\$TMAKEPATH not defined\n";
}

print "Cleaning tree and producing external Makefiles...\n";
`cd .. && ./DSLrunConfigure --buildtype=external`;
`make -C .. distclean`;

# Make a new root

`rm -fr $root`;
`mkdir $root`;

# Right, we have binaries in the CVS in lib/ which we don't want in
# the distrib so we don't include the lib/ directory and must mkdir it.
# I'd rather the Windows libs were removed from the CVS

`mkdir ${root}/lib`;

# Copy the tree

print "Selectively copying...\n";

&match_dir("..", "$root", @directory_patterns);
&match_file("..", "$root", @file_patterns);

# This uses the DSL copy of unix2dos
print "Correcting line endings on files in Win32Projects ...\n";
`find $root/Win32Projects -type f -name *.dsp -or -name *.dsw | xargs -n 1 unix2dos -z`;

`touch $root/src/lexer/XPath2Lexer.cpp`;

# Make a tar-gz

print "Running tar...\n";
`rm -f ${root}-src.tar.gz`;

print "Gzipping...\n";
`tar cvfz ${root}-src.tar.gz $root`;

# make Windows source distrib
# Build zipfile

print "Making windows zip...\n";
`rm -f ${root}-src.zip`;
`zip -r ${root}-src.zip $root`;

