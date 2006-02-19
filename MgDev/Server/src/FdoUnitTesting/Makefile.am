AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = FdoUnitTesting.vcproj FdoUnitTesting.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../Oem/ACE/ACE_wrappers \
  -I../../../Oem/CppUnit-1.9.14/include \
  -I../../../Common/Service \
  -I$(top_builddir)/$(map_coordsys_include_path)\
  -I../../../Common/Service/Services \
  -I../../../Common/Service/System \
  -I../../../Common/Service/Geometry \
  -I../../../Common/Service/MapLayer \
  -I../Common \
  -I../Common/Manager \
  -I../Services/Feature \
  -I../Services/Mapping \
  -I../Services/Resource \
  -I../Services/ServerAdmin \
  -I../Services/Site \
  -I$(map_fdo_include)

check_LTLIBRARIES = libMgFdoUnitTesting.la

libMgFdoUnitTesting_la_SOURCES = \
  FdoUnitTesting.cpp \
  TestFeatureService.cpp

noinst_HEADERS = \
  FdoUnitTesting.h \
  TestFeatureService.h

libMgFdoUnitTesting_la_LIBADD = -lcppunit

libMgFdoUnitTesting_la_LDFLAGS = -rpath /createshared -version-info 7:0:0 \
  -L../../../Oem/CppUnit-1.9.14/src/cppunit
