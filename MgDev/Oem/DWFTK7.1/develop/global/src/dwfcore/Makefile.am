AUTOMAKE_OPTIONS = nostdinc
INCLUDES = -I..
SUBDIRS = ansi mac x86 zip zlib


pkginclude_HEADERS = BufferInputStream.h BufferOutputStream.h Comparator.h Core.h CountedObject.h DecompressingInputStream.h Exception.h File.h FileDescriptor.h FileInputStream.h FileOutputStream.h Hash.h InputStream.h Iterator.h MIME.h MonitoredInputStream.h OutputStream.h Owner.h Pointer.h SkipList.h STL.h StreamFileDescriptor.h String.h Synchronization.h TempFile.h ThreadPool.h Timer.h UnzippingInputStream.h UTF8EncodingOutputStream.h UUID.h Version.h XMLEncodingOutputStream.h ZipFileDescriptor.h ZippingOutputStream.h

lib_LTLIBRARIES =  libdwfcore.la

libdwfcore_la_LDFLAGS = -release 1.1.1

libdwfcore_la_SOURCES = BufferInputStream.cpp BufferOutputStream.cpp DecompressingInputStream.cpp Exception.cpp File.cpp FileInputStream.cpp FileOutputStream.cpp MIME.cpp MonitoredInputStream.cpp Owner.cpp ThreadPool.cpp UnzippingInputStream.cpp UTF8EncodingOutputStream.cpp UUID.cpp XMLEncodingOutputStream.cpp ZipFileDescriptor.cpp ZippingOutputStream.cpp 

libdwfcore_la_LIBADD = $(top_builddir)/ansi/libdwfcoreansi.la $(top_builddir)/mac/libdwfcoremac.la  $(top_builddir)/x86/libdwfcorex86.la $(top_builddir)/zlib/libdwfcorezlib.la $(top_builddir)/zip/libdwfcorezip.la
