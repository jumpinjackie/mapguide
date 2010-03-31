AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = ServerTileService.vcproj ServerTileService.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../../Oem/ACE/ACE_wrappers \
  -I../../../../Common/Foundation \
  -I../../../../Common/Geometry \
  -I../../../../Common/PlatformBase \
  -I../../../../Common/MapGuideCommon \
  -I../../Common \
  -I../../Common/Base \
  -I../../Common/Manager \
  -I../../../../Common/MdfModel

lib_LTLIBRARIES = libMgServerTileService.la

libMgServerTileService_la_SOURCES = \
  ServerTileServiceBuild.cpp

include_SOURCES = \
  OpClearCache.cpp \
  OpGetTile.cpp \
  OpGetDefaultTileSizeX.cpp \
  OpGetDefaultTileSizeY.cpp \
  OpSetTile.cpp \
  ServerTileService.cpp \
  TileCache.cpp \
  TileOperation.cpp \
  TileOperationFactory.cpp \
  TileServiceHandler.cpp

noinst_HEADERS = $(include_SOURCES) \
  OpClearCache.h \
  OpGetTile.h \
  OpGetDefaultTileSizeX.h \
  OpGetDefaultTileSizeY.h \
  OpSetTile.h \
  ServerTileDllExport.h \
  ServerTileService.h \
  TileCache.h \
  TileOperation.h \
  TileOperationFactory.h \
  TileServiceHandler.h

libMgServerTileService_la_LDFLAGS = -release $(PACKAGE_VERSION)
