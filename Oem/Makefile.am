AUTOMAKE_OPTIONS = foreign
prefix=@prefix@

SUBDIRS = $(map_opt_oem_subdirs)
DIST_SUBDIRS =
EXTRA_DIST = ACE jsoncpp CppUnit-1.9.14 dbxml-2.3.10 DWFTK7.1 FDO/ProviderList/ServerCommunity fusionframework gd agg-2.4 gdal-1.4.4 geos-2.2.0 jsoncpp LinuxApt php proj-4.5.0 SQLite SWIGEx oem.sln

dist-hook:
	rm -f $(distdir)/gdal-1.4.4/port/cpl_config.h

install-exec-am:
	@mkdir -p ${prefix}/lib
	@cp -fp ACE/ACE_wrappers/ace/*.so ${prefix}/lib
	@cp -fp jsoncpp/lib/*.so ${prefix}/lib
	@cp -fpd gdal-1.4.4/.libs/libgdal.so* ${prefix}/lib
	@cp -fpd proj-4.5.0/src/.libs/libproj.so* ${prefix}/lib
	@cp -fpd dbxml-2.3.10/install/lib/libxerces-c*.so* ${prefix}/lib
	@cp -fpd dbxml-2.3.10/install/lib/libxqilla*.so* ${prefix}/lib
	@cp -fpd dbxml-2.3.10/install/lib/libdb-*.so* ${prefix}/lib
	@cp -fpd dbxml-2.3.10/install/lib/libdb_cxx*.so* ${prefix}/lib
	@cp -fpd dbxml-2.3.10/install/lib/libdbxml*.so* ${prefix}/lib
	@cp -fp  DWFTK7.1/develop/global/src/dwfcore/.libs/libdwfcore-* ${prefix}/lib
	@cp -fpd DWFTK7.1/develop/global/src/dwfcore/.libs/libdwfcore ${prefix}/lib
	@cp -fp  DWFTK7.1/develop/global/src/dwfemap/.libs/libdwfemap-* ${prefix}/lib
	@cp -fpd DWFTK7.1/develop/global/src/dwfemap/.libs/libdwfemap ${prefix}/lib
	@cp -fp  DWFTK7.1/develop/global/src/dwf/.libs/libdwftk-* ${prefix}/lib
	@cp -fpd DWFTK7.1/develop/global/src/dwf/.libs/libdwftk ${prefix}/lib
