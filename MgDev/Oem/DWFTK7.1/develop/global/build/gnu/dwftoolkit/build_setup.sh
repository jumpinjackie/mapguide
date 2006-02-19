#
#! /bin/bash
#

cd ../../../src/dwf
tar -xzf ../../build/gnu/dwftoolkit/build_files.tar.gz

rm -rf autom4te.cache/
autoreconf --force

