AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = UnitTesting.vcproj UnitTesting.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../Oem/ACE/ACE_wrappers \
  -I../../../Oem/CppUnit-1.9.14/include \
  -I../../../Oem/dbxml/xerces-c-src/src \
  -I../../../Common/Foundation \
  -I../../../Common/Geometry \
  -I../../../Common/PlatformBase \
  -I../../../Common/MapGuideCommon \
  -I../../../Common/MdfModel \
  -I../../../Common/MdfParser \
  -I../../../Common/Stylization \
  -I../Common \
  -I../Common/Manager \
  -I../Services/Feature \
  -I../Services/Mapping \
  -I../Services/Rendering \
  -I../Services/Resource \
  -I../Services/ServerAdmin \
  -I../Services/Site \
  -I../Services/Kml \
  -I../Services/Tile \
  -I$(map_fdo_include)

check_LTLIBRARIES = libMgUnitTesting.la

libMgUnitTesting_la_SOURCES = \
  TestCoordinateSystem.cpp \
  TestFeatureService.cpp \
  TestGeometry.cpp\
  TestKmlService.cpp \
  TestLogManager.cpp \
  TestLogManagerThread.cpp \
  TestMappingService.cpp \
  TestMdfModel.cpp \
  TestPerformance.cpp \
  TestRenderingService.cpp \
  TestResourceService.cpp \
  TestServerAdminService.cpp \
  TestServerManager.cpp \
  TestServiceManager.cpp \
  TestSiteManager.cpp \
  TestSiteService.cpp \
  TestTileService.cpp \
  TestProfilingService.cpp \
  TestTransformMesh.cpp \
  UnitTesting.cpp

noinst_HEADERS = \
  CppUnitExtensions.h \
  TestCoordinateSystem.h \
  TestFeatureService.h \
  TestGeometry.h \
  TestKmlService.h \
  TestLogManager.h \
  TestLogManagerThread.h \
  TestMappingService.h \
  TestMdfModel.h \
  TestPerformance.h \
  TestRenderingService.h \
  TestResourceService.h \
  TestServerAdminService.h \
  TestServerManager.h \
  TestServiceManager.h \
  TestSiteManager.h \
  TestSiteService.h \
  TestTileService.h \
  TestProfilingService.h \
  TestTransformMesh.h \
  UnitTesting.h

libMgUnitTesting_la_LIBADD = -lcppunit

libMgUnitTesting_la_LDFLAGS = -rpath /createshared -release $(PACKAGE_VERSION) \
  -L../../../Oem/CppUnit-1.9.14/src/cppunit
