AUTOMAKE_OPTIONS = foreign
prefix=@prefix@

SUBDIRS = $(map_opt_oem_subdirs)
DIST_SUBDIRS = 
EXTRA_DIST = ACE CppUnit-1.9.14 dbxml-2.2.13 DWFTK7.1 FDO/ProviderList/ServerCommunity fcgi gd gdal-1.4.0 geos-2.2.0 php proj-4.5.0 SQLite SWIGEx oem.sln

dist-hook:
	rm -f $(distdir)/gdal-1.4.0/port/cpl_config.h

install-exec-am:
	@cp -fp ACE/ACE_wrappers/ace/*.so ${prefix}/lib
	@cp -fp fcgi/fcgi-2.4.0/libfcgi/.libs/libfcgi.so ${prefix}/lib
	@cp -fpd dbxml-2.2.13/install/lib/libpathan.so* ${prefix}/lib
	@cp -fpd dbxml-2.2.13/install/lib/libxerces-c.so* ${prefix}/lib
	@cp -fpd dbxml-2.2.13/install/lib/libxquery*.so ${prefix}/lib
	@cp -fpd dbxml-2.2.13/install/lib/libdb-4*.so ${prefix}/lib
	@cp -fpd dbxml-2.2.13/install/lib/libdb_cxx*.so ${prefix}/lib
	@cp -fpd dbxml-2.2.13/install/lib/libdbxml*.so ${prefix}/lib

