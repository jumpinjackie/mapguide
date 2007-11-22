AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerCache.vcproj ServerCache.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Common/Foundation \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../../../Common/Geometry \
  -I../../../../Common/MdfModel \
  -I../../../../Oem/ACE/ACE_wrappers

lib_LTLIBRARIES = libMgServerCache.la

libMgServerCache_la_SOURCES = \
  ServerCacheBuild.cpp

include_SOURCES = \
  ServerCache.cpp \
  ServerCacheEntry.cpp \
  ServerCacheItem.cpp \
  FeatureServiceCache.cpp \
  FeatureServiceCacheEntry.cpp \
  FeatureSourceCacheItem.cpp \
  SpatialContextCacheItem.cpp

noinst_HEADERS = $(include_SOURCES) \
  ServerCache.h \
  ServerCacheDllExport.h \
  ServerCacheEntry.h \
  ServerCacheItem.h \
  FeatureServiceCache.h \
  FeatureServiceCacheEntry.h \
  FeatureSourceCacheItem.h \
  SpatialContextCacheItem.h

libMgServerCache_la_LDFLAGS = -version-info 7:0:0
