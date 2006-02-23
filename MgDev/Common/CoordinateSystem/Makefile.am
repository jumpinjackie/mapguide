AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = CoordinateSystem.vcproj categories.txt

INCLUDES = \
  -I../../Oem/gdal-1.3.0/ogr \
  -I../../Oem/gdal-1.3.0/port \
  -I../../Oem/proj-4.4.9/src

lib_LTLIBRARIES = libMgCoordinateSystem.la

libMgCoordinateSystem_la_SOURCES = \
  CoordSys.cpp \
  CoordSysCatalog.cpp \
  CoordSysCategory.cpp \
  CoordSysCategoryCollection.cpp \
  CoordSysComputationFailedException.cpp \
  CoordSysConversionFailedException.cpp \
  CoordSysEnvelope.cpp \
  CoordSysException.cpp \
  CoordSysFactory.cpp \
  CoordSysInformation.cpp \
  CoordSysInitializationFailedException.cpp \
  CoordSysInvalidArgumentException.cpp \
  CoordSysInvalidCoordinateSystemException.cpp \
  CoordSysInvalidCoordinateSystemTypeException.cpp \
  CoordSysInvalidCoordinateSystemUnitsException.cpp \
  CoordSysMeasure.cpp \
  CoordSysMeasureFailedException.cpp \
  CoordSysNullArgumentException.cpp \
  CoordSysNullReferenceException.cpp \
  CoordSysOutOfMemoryException.cpp \
  CoordSysTransform.cpp \
  CoordSysTransformation.cpp \
  CoordSysTransformFailedException.cpp

include_SOURCES = \
  Dll.cpp

noinst_HEADERS = $(include_SOURCES) \
  CoordSys.h \
  CoordSysCatalog.h \
  CoordSysCategory.h \
  CoordSysCategoryCollection.h \
  CoordSysComputationFailedException.h \
  CoordSysConversionFailedException.h \
  CoordSysEnvelope.h \
  CoordSysException.h \
  CoordSysFactory.h \
  CoordSysInformation.h \
  CoordSysInitializationFailedException.h \
  CoordSysInvalidArgumentException.h \
  CoordSysInvalidCoordinateSystemException.h \
  CoordSysInvalidCoordinateSystemTypeException.h \
  CoordSysInvalidCoordinateSystemUnitsException.h \
  CoordSysMeasure.h \
  CoordSysMeasureFailedException.h \
  CoordSysNullArgumentException.h \
  CoordSysNullReferenceException.h \
  CoordSysOutOfMemoryException.h \
  CoordSysTransform.h \
  CoordSysTransformation.h \
  CoordSysTransformFailedException.h \
  CoordSysCommon.h \
  CoordSysDimension.h \
  CoordSysType.h \
  CoordSysUtil.h \
  CriticalSection.h \
  CoordSysIncludes.h \
  stdafx.h

libMgCoordinateSystem_la_LIBADD = -lgdal -lproj

libMgCoordinateSystem_la_LDFLAGS = -version-info 7:0:0 \
                -L../../Oem/gdal-1.3.0 \
                -L../../Oem/proj-4.4.9/src
