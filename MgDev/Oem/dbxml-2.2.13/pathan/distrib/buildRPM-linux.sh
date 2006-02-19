#!/bin/bash

version=2;
subversion=0alpha;
root="libpathan-${version}.${subversion}"; # pathan tar gz name, eg. pathan-1.0.tar.gz

# Check for tar gzip

if [ -a "${root}-src.tar.gz" ]
    then echo
    else echo "Run buildTGZ.pl first."; exit 1;
fi

# Clean and remake RPM build directories

rm -fr pathan_rpm
mkdir pathan_rpm
mkdir pathan_rpm/BUILD
mkdir pathan_rpm/RPMS
mkdir pathan_rpm/SOURCES
mkdir pathan_rpm/SPECS
mkdir pathan_rpm/SRPMS

rm -fr pathan_root
mkdir pathan_root

# Copy targz and spec file

cp ${root}-src.tar.gz pathan_rpm/SOURCES
cp pathan.spec pathan_rpm/SPECS

# Run RPM

TOPDIR=`cd pathan_rpm && pwd;`
echo $TOPDIR

BUILDROOT=`cd pathan_root && pwd`

cd pathan_rpm/SPECS

# info@decisionsoft.com - official DSL key

# RedHat

rpmbuild -ba -vv --sign --buildroot ${BUILDROOT} --define "_topdir ${TOPDIR}" --define "_signature GPG" --define "_gpg_name info@decisionsoft.com" --define "_gpg_path $HOME/.gnupg" pathan.spec 


