#
# Build the distribution package.
#
# A set of commands intended to be cut and pasted into a bash window.
export D=`pwd`

# Update the release number.
cd $D/dist
vi RELEASE
export VERSION=`sh -c '. RELEASE; echo $XQUERY_VERSION'`
echo "Version: $VERSION"

# Make sure the source tree is up-to-date, generate new support files,
# commit anything that's changed.
#cd $D && cvs -z3 -q update
#cd $D/dist && sh s_all
#cd $D && cvs -z3 -q commit

# Copy a development tree into a release tree.
export R=/var/tmp/xquery-$VERSION
rm -rf $R && mkdir -p $R
cd $D
rm -f xaa xab
cvs -z3 -q status | grep "Repository revision" | sed -e 's;.*CVSROOT/xquery/;;' -e 's;.*CVSROOT/;;' -e 's;,v$;;' | split -l 1000
rm -f /tmp/tmp.tar
tar cf /tmp/tmp.tar `cat xaa`
if [ -f xab ]; then
    tar rf /tmp/tmp.tar `cat xab`
fi
cd $R
tar xpf /tmp/tmp.tar
rm /tmp/tmp.tar 

# ACQUIRE ROOT PRIVILEGES
cd $R && find . -type d | xargs chmod 775
cd $R && find . -type f | xargs chmod 444
cd $R && chmod 664 build_win32/*.dsp
cd $R && chmod 664 build_win32/*.vcproj
cd $R/dist && sh s_perm

#chown -R 100.100 $R
# DISCARD ROOT PRIVILEGES

exit 0

# Create the tar archive release.
# Create the zip source release.
setenv T "$R/../xquery-$VERSION.tar.gz"
cd $R/.. && tar cf - xquery-$VERSION | gzip --best > $T
chmod 444 $T
setenv T "/var/tmp/xquery-$VERSION.zip"
cd $R/.. && zip -r -9 $T xquery-$VERSION
chmod 444 $T

