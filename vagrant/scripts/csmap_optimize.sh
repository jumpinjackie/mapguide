#!/bin/bash

# csmap_optimize.sh
#
# This script optimizes the size of CS-Map dictionary files by removing references
# to all country-specific grid files which represents the bulk of the distribution
# size
#
# This script is meant to be called from within dpkgmapguide.sh

EXCLUDEFILE=
PACKAGENAME=mapguideopensource-coordsys-lite
PACKAGEDIR=mapguidecoordsyslite
ROOT=${BUILDROOT}/debian/mapguidecoordsyslite
TREE=${BUILDROOT}/debian
CPROOT=${ROOT}/${MGINST}
DIRLIST="share"
REMOVELIST="\.a\$ \.la\$ \.c\$ \.o\$ \.mak\$ \.nmk\$"
STRIPLIST="\.so\$"
CSMAP_OUT=/tmp/csmap_build_area/share/gis/coordsys

if [ ! -d /tmp/csmap_build_area ]; then
    mkdir -p /tmp/csmap_build_area
fi

comment_gdc()
{
    GDCFILE=$1
    sed -i 's/.\\Australia/#.\\Australia/g' $GDCFILE
    sed -i 's/.\\Canada/#.\\Canada/g' $GDCFILE
    sed -i 's/.\\France/#.\\France/g' $GDCFILE
    sed -i 's/.\\Germany/#.\\Germany/g' $GDCFILE
    sed -i 's/.\\Japan/#.\\Japan/g' $GDCFILE
    sed -i 's/.\\NewZealand/#.\\NewZealand/g' $GDCFILE
    sed -i 's/.\\Spain/#.\\Spain/g' $GDCFILE
    sed -i 's/.\\Switzerland/#.\\Switzerland/g' $GDCFILE
    sed -i 's/.\\Usa/#.\\Usa/g' $GDCFILE
    sed -i 's/.\\WW15MGH.GRD/#.\\WW15MGH.GRD/g' $GDCFILE
    echo "Commented out grid file references in $GDCFILE"
}

# Copy CS-Map files to build area. We rely on the fact that the MapGuide build system
# also leaves a copy of the CS_Comp executable in this directory which we'll use to 
# re-compile our dictionary files after some modifications
cp -R /${MGINST}/share/gis/coordsys/* /tmp/csmap_build_area
pushd /tmp/csmap_build_area
    # Comment out grid file entries in all gdc files
    for gdcf in *.gdc
    do
        comment_gdc $gdcf
    done
    # Re-compile
    ./CS_Comp -b . .
    if [ ! -d $CSMAP_OUT ]; then
        mkdir -p $CSMAP_OUT
    fi
    # Copy output
    cp *.CSD $CSMAP_OUT
    cp NameMapper.csv $CSMAP_OUT
    cp OSTN02._02 $CSMAP_OUT
    cp OSTN97._nt $CSMAP_OUT
popd
MGINST=tmp/csmap_build_area
source ./dpkgbuild.sh
rm -rf /tmp/csmap_build_area
