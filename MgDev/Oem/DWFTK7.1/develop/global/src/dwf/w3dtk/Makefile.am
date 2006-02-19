AUTOMAKE_OPTIONS = nostdinc
INCLUDES = -I../..

w3dtk_includedir = $(pkgincludedir)/w3dtk
w3dtk_include_HEADERS = BByteStream.h BCompress.h BEdgeBreaker.h BInternal.h BOpcodeHandler.h BOpcodeShell.h BPointCloud.h BPolyhedron.h BPolyPoly.h BStream.h BStreamFileToolkit.h Version.h W3DCamera.h W3DOpcodeHandler.h W3DStreamWriter.h
SUBDIRS = jpeg utility hoops
noinst_LTLIBRARIES =  libdwfw3dtk.la
libdwfw3dtk_la_LDFLAGS = -avoid-version 
libdwfw3dtk_la_SOURCES =  BCompress.cpp BCompressData.cpp BEdgeBreakerCommon.cpp BEdgeBreakerCompress.cpp BEdgeBreakerDecompress.cpp BOpcodeHandler.cpp BOpcodeShell.cpp BPointCloud.cpp BPolyhedron.cpp BPolyPoly.cpp BStreamFileToolkit.cpp W3DCamera.cpp W3DStreamWriter.cpp
AM_CXXFLAGS = -DBSTREAM_DISABLE_REPULSE_COMPRESSION 
libdwfw3dtk_la_LIBADD = $(top_builddir)/w3dtk/utility/libdwfw3dtkutility.la  $(top_builddir)/w3dtk/hoops/libdwfw3dtkhoops.la
libdwfw3dtk_la_DEPENDENCIES = 
