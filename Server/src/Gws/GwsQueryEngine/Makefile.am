AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = GwsQueryEngine.vcproj GwsQueryEngine.def

lib_LTLIBRARIES = libMgGwsQueryEngine.la

libMgGwsQueryEngine_la_SOURCES = \
  GwsBinaryFeature.cpp \
  GwsBinaryFeatureReader.cpp \
  GwsBinaryFeatureWriter.cpp \
  GwsBinaryReader.cpp \
  GwsBinaryWriter.cpp \
  GwsCsQueryProcessor.cpp \
  GwsExtendedFeatureDescription.cpp \
  GwsFdoCommand.cpp \
  GwsFdoCommandDelete.cpp \
  GwsFdoCommandInsert.cpp \
  GwsFdoCommandLock.cpp \
  GwsFdoCommandUnlock.cpp \
  GwsFdoCommandUpdate.cpp \
  GwsFeatureQueryDefinition.cpp \
  GwsFeatureQueryResults.cpp \
  GwsFeatureSourceQuery.cpp \
  GwsFeaturesPool.cpp \
  GwsJoinQueryDefinition.cpp \
  GwsJoinQueryResults.cpp \
  GwsMutableFeature.cpp \
  GwsNestedLoopsJoinQueryResults.cpp \
  GwsNestedLoopSortedBlockJoinQueryResults.cpp \
  GwsPooledFeature.cpp \
  GwsPreparedFeatureQuery.cpp \
  GwsPreparedJoinQuery.cpp \
  GwsPropertyDesc.cpp \
  GwsQueryDefinition.cpp \
  GwsQueryFactories.cpp \
  GwsQueryUtils.cpp \
  GwsQueryXml.cpp \
  GwsRightJoinQueryResults.cpp \
  GwsRightNestedLoopJoinQueryResults.cpp \
  GwsRightNestedLoopSortedBlockJoinQueryResults.cpp \
  GwsRightSortedJoinQueryResults.cpp \
  GwsSortMergeJoinQueryResults.cpp \
  utf8_.cpp

include_SOURCES = \
  GwsQueryEngine.cpp \
  stdafx.cpp

noinst_HEADERS = $(include_SOURCES) \
  stdafx.h \
  inc/GwsBinaryFeatureReader.h \
  inc/GwsBinaryFeatureWriter.h \
  inc/GwsBinaryReader.h \
  inc/GwsBinaryWriter.h \
  inc/GwsFdoCommand.h \
  inc/GwsMutableFeature.h \
  inc/GwsQuery.h \
  inc/GwsQueryDefinition.h \
  inc/GwsQueryEngineImp.h \
  inc/utf8_.h

INCLUDES = \
  -I$(map_fdo_include) \
  -I../Include \
  -I../GwsCommon/inc \
  -I./inc

libMgGwsQueryEngine_la_LDFLAGS = -version-info 1:0:0

