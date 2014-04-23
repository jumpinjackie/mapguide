AUTOMAKE_OPTIONS = foreign
prefix=@prefix@

SUBDIRS = $(map_opt_oem_subdirs)
DIST_SUBDIRS =
EXTRA_DIST = ACE CsMap jsoncpp CppUnit-1.9.14 dbxml DWFTK7.1 FDO/ProviderList/ServerCommunity fusion fusionMG gd agg-2.4 geos-2.2.0 jsoncpp LinuxApt php SQLite SWIGEx oem.sln

dist-hook:


install-exec-am:
	@mkdir -p ${prefix}/lib
	@cp -fp ACE/ACE_wrappers/ace/*.so ${prefix}/lib
	@cp -fp jsoncpp/lib/*.so ${prefix}/lib
	@cp -fpd dbxml/install/lib/libxerces-c*.so* ${prefix}/lib
	@cp -fpd dbxml/install/lib/libxqilla*.so* ${prefix}/lib
	@cp -fpd dbxml/install/lib/libdb-*.so* ${prefix}/lib
	@cp -fpd dbxml/install/lib/libdb_cxx*.so* ${prefix}/lib
	@cp -fpd dbxml/install/lib/libdbxml*.so* ${prefix}/lib
	@cp -fp  DWFTK7.1/develop/global/src/dwfcore/.libs/libdwfcore-* ${prefix}/lib
	@cp -fp  DWFTK7.1/develop/global/src/dwfemap/.libs/libdwfemap-* ${prefix}/lib
	@cp -fp  DWFTK7.1/develop/global/src/dwf/.libs/libdwftk-* ${prefix}/lib
