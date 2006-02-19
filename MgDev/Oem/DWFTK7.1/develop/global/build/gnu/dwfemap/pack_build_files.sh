#
#! /bin/bash
#

cd ../../../src/dwfemap/
tar -czvf build_files.tar.gz --preserve AUTHORS COPYING ChangeLog Doxyfile INSTALL NEWS README TODO config.guess config.sub configure.in depcomp install-sh ltmain.sh missing mkinstalldirs
mv build_files.tar.gz ../../build/gnu/dwfemap

