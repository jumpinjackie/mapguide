#
#! /bin/bash
#

cd ../../../src/dwfemap
tar -xzf ../../build/gnu/dwfemap/build_files.tar.gz

rm -rf autom4te.cache/
autoreconf --force

