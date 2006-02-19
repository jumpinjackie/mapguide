# PREAMBLE

# Pathan RPM spec file

Summary: Provides an X-Path parsing and evaluation API on top of the Xerces C++ library.
Name: libpathan
Version: 2.0beta
Release: 0
Copyright: DecisionSoft Open Source Licence http://software.decisionsoft.com/licence.html
Group: Development/Libraries
Source: libpathan-2.0beta-src.tar.gz
Vendor: Decisionsoft Ltd
Packager: Stephen White <swhite@decisionsoft.com>

%description 
Pathan provides X-Path parsing and evaluation functionality for the
Xerces C++ library. Pathan requires the Xerces C++ library for which
official rpms are not available.

%package devel
Summary: Devel package for the Pathan X-Path parsing and evaluation library.
Group: Development/Libraries
Requires: libpathan

%description devel
This package provides the development headers and a static library for Pathan.

# PREP SECTION

%prep

%setup

# BUILD SECTION

%build
./runConfigure --compiletype=optimised --prefix=${RPM_BUILD_ROOT}/usr
make

# INSTALL SECTION

%install
make install

# libpathan.la stores the path of the libraries. Change this.
sed s^libdir=.*^libdir=/usr/lib^ < ${RPM_BUILD_ROOT}/usr/lib/libpathan.la > ${RPM_BUILD_ROOT}/usr/lib/libpathan.la.new
mv ${RPM_BUILD_ROOT}/usr/lib/libpathan.la.new ${RPM_BUILD_ROOT}/usr/lib/libpathan.la


# POST RPM INSTALL SECTION

%post
ldconfig

%post devel
ldconfig

# FILE LIST

%files
%defattr(-, root, root)
%attr(755, root, root) /usr/lib/libpathan.so.3.0.1
%attr(755, root, root) /usr/lib/libpathan.so.3
%attr(755, root, root) /usr/lib/libpathan.so
%doc LICENSE.TXT

%files devel
%defattr(-, root, root)
%attr(644, root, root) /usr/include/pathan/*.h*
%attr(644, root, root) /usr/include/pathan/*/*.h*
%attr(644, root, root) /usr/include/pathan/*/*/*.h*
%attr(755, root, root) /usr/lib/libpathan.la
%attr(755, root, root) /usr/lib/libpathan.a
%doc LICENSE.TXT

# CHANGELOG

%changelog
* Fri Jul 30 2004 Stephen White <swhite@decisionsoft.com>
- updated for beta release

* Wed Sep 3 2003 Stephen White <swhite@decisionsoft.com>
- updated to Pathan 2.0
