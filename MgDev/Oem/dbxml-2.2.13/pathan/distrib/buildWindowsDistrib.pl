# Create a windows distribution similar to the devel RPM

# Note that the DLL project on Win32 Debug AND Win32 Release targets
# should be compiled before running this script

my $version = 2;
my $subversion = "0beta";

my $root = "libpathan-${version}.${subversion}";

# Delete any previous root

`rm -fr ${root}`;

# Make directories

`mkdir ${root}`;

`mkdir ${root}/lib`;

# Copy libraries

my $srcdir = "../";

`cp ${srcdir}/lib/Pathan.dll ${srcdir}/lib/Pathan.lib ${srcdir}/lib/PathanD.dll ${srcdir}/lib/PathanD.lib ${root}/lib`;

# Build zipfile

print "Making windows distro zip...\n";
`zip -r ${root}-win32.zip $root`;
