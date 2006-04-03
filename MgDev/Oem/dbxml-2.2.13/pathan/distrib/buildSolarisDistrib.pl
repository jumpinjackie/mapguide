# Create a Solaris distribution similar to the devel RPM

# This script builds and packages the Solaris binaries

my $TAR = "/usr/local/bin/tar";

my $version = 1;
my $subversion = 1;

my $root = "libpathan-${version}.${subversion}";

# Do Solaris build

if($ENV{'XERCESCROOT'} eq "") {
    die "\$XERCESCROOT not defined\n";
}

# Delete any previous root

system("rm -fr ${root}");

# Make directories

system("mkdir ${root}");

# Find ${root} absolute path

$ROOTABS = `cd ${root} && pwd`;
chop $ROOTABS;
$PREFIX = $ROOTABS . "/usr/local";

print "Building for Solaris...\n";

system("cd .. && ./runConfigure --prefix=${PREFIX} && cd distrib");

system("make -C ..");

system("make -C .. install");

# Make solaris distribution tree

system("rm -fr ${root}-sparc");
system("mkdir ${root}-sparc");
system("mkdir ${root}-sparc/lib");
system("cp -R ${PREFIX}/include ${root}-sparc");
system("cp ${PREFIX}/lib/libpathan.a ${root}-sparc/lib");
system("cp ${PREFIX}/lib/libpathan.so.1.0.0 ${root}-sparc/lib");
system("cd ${root}-sparc/lib && ln -s libpathan.so.1.0.0 libpathan.so && cd ../..");
system("cd ${root}-sparc/lib && ln -s libpathan.so.1.0.0 libpathan.so.0 && cd ../..");

# Build archive

system("${TAR} cvfz libpathan-1.1-sparc.tar.gz ${root}-sparc");
