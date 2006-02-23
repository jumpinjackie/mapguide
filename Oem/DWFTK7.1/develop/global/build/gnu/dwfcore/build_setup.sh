#
#! /bin/bash
#

cd ../../../src/dwfcore
tar -xzf ../../build/gnu/dwfcore/build_files.tar.gz

rm -rf autom4te.cache/
autoreconf --force



