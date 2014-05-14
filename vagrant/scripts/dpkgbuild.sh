
# Blow away old build directory
rm -rf ${ROOT}

# Create and populate documentation directory for fdo-core
mkdir -p ${ROOT}/usr/share/doc/${PACKAGENAME}
cp debian/copyright ${ROOT}/usr/share/doc/${PACKAGENAME}
cp debian/changelog ${ROOT}/usr/share/doc/${PACKAGENAME}/changelog.Debian
gzip -9 ${ROOT}/usr/share/doc/${PACKAGENAME}/changelog.Debian


# Create directories for binary package build and copy
# install image into build directory
for dirname in ${DIRLIST}
do
  mkdir -p ${CPROOT}/${dirname}
  #cp -ar /${MGINST}/${dirname} ${CPROOT}
  echo "[rsync] From: /${MGINST}/${dirname}/ To: ${CPROOT}/${dirname}/"
  if [ -z ${EXCLUDEFILE} ]; then
    rsync -a /${MGINST}/${dirname}/ ${CPROOT}/${dirname}/
  else
    rsync -a --exclude-from=${EXCLUDEFILE} /${MGINST}/${dirname}/ ${CPROOT}/${dirname}/
  fi
done

# Remove all files in the REMOVELIST
for pat in ${REMOVELIST}
do
  for file in `find ${CPROOT} -type f | grep "${pat}"`
  do
    echo "Removing ${file}"
    rm -f ${file}
  done
done

# Symbol strip all files in the STRIPLIST
for pat in ${STRIPLIST}
do
  for file in `find ${CPROOT} -type f | grep "${pat}"`
  do
    echo "Stripping ${file}"
    strip --strip-unneeded ${file}
    chmod a-x ${file}
  done
done

# Call dpkg-shlibdeps to build out library dependencies
# This will create the substvars file
# LD_LIBRARY_PATH must be set to find private libs
mkdir -p ${ROOT}/DEBIAN
pushd ${BUILDROOT}
# Shared library symlinks point back to install directory
# Add install directory to path to compensate
# Also add FDO libs to path
export LD_LIBRARY_PATH=/usr/local/fdo-${FDOBUILD}/lib:${CPROOT}/lib:/${MGINST}/lib
echo "Using LD_LIBRARY_PATH of $LD_LIBRARY_PATH"
dpkg-shlibdeps -p${PACKAGEDIR} --ignore-missing-info ${CPROOT}/lib/*.so
dpkg-gensymbols -q -p"${PACKAGENAME}" -P"debian/${PACKAGEDIR}"
export LD_LIBRARY_PATH=

# Now generate a filled in control file for the binary package
# using information from control, changelog, and substvars
dpkg-gencontrol -p"${PACKAGENAME}" -P"debian/${PACKAGEDIR}"

# Build binary package from ROOT
# And move resulting debian package and lintian results to build directory
dpkg-deb -Zlzma --build ${ROOT}
lintian -i debian/${PACKAGEDIR}.deb > tmp/${PACKAGEDIR}.lintian
mv debian/${PACKAGEDIR}.deb bin/${PACKAGENAME}_${MGBUILD}-${BUILDNUM}_${ARCH}.deb
popd
