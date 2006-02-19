#
#! /bin/bash
#


cd ../../../src/dwfcore/

#
# the following are packed into a tarball so their
# permissions are preserved for the GNU make process
#

tar -czvf build_files.tar.gz --preserve AUTHORS COPYING ChangeLog Doxyfile INSTALL NEWS README TODO config.guess config.sub configure.in depcomp install-sh ltmain.sh missing mkinstalldirs
mv build_files.tar.gz ../../build/gnu/dwfcore



