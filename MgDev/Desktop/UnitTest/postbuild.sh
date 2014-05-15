#!/bin/bash

OUTDIR=$PWD/../UnitTestFiles
ROOTDIR=$1
UNITTEST_DIR=$ROOTDIR/../UnitTest/TestData

echo "Performing post-build actions for unit test"
echo "Output directory is: $OUTDIR"
echo "Source dir is: $UNITTEST_DIR"

if [ ! -d $PWD/../UnitTestFiles ]; then
    mkdir -p $PWD/../UnitTestFiles
fi

echo "Copy $ROOTDIR/DesktopTestData to $OUTDIR"
cp $ROOTDIR/DesktopTestData/* $OUTDIR/
for dirname in CoordinateSystems DrawingService KmlService MapLayer MappingService MdfModel PrintLayout ResourceService ServerAdmin SiteService Symbology TileService Unicode WebLayout Wfs Wms
do
    SRC_DIR=$UNITTEST_DIR/$dirname
    echo "Copy $SRC_DIR to $OUTDIR"
    cp $SRC_DIR/* $OUTDIR/
done
for dirname in SDF SHP SQLite
do
    SRC_DIR=$UNITTEST_DIR/FeatureService/$dirname
    echo "Copy $SRC_DIR to $OUTDIR"
    cp $SRC_DIR/* $OUTDIR/
done
