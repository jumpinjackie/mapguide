AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = serverconfig.ini ServerCore.vcproj ServerCore.rc
mentor_dict_install_dir=@prefix@/share/gis/coordsys
mentor_dict_source_dir=$(map_srcdir)/Oem/CsMap/Dictionaries
root_prefix=@prefix@

prefix=@prefix@/server

INCLUDES = \
  -I../../../Oem/ACE/ACE_wrappers \
  -I../../../Oem/dbxml-2.3.10/xerces-c-src/src \
  -I $(map_fdo_include) \
  -I $(map_fdo_include)/ExpressionEngine \
  -I../Common \
  -I../Common/Base \
  -I../Common/Cache \
  -I../Common/Manager \
  -I../Common/Thread \
  -I../Services/Drawing \
  -I../Services/Feature \
  -I../Services/Kml \
  -I../Services/Mapping \
  -I../Services/Rendering \
  -I../Services/Resource \
  -I../Services/ServerAdmin \
  -I../Services/Site \
  -I../Services/Tile \
  -I../../../Common \
  -I../../../Common/Foundation \
  -I../../../Common/Geometry \
  -I../../../Common/PlatformBase \
  -I../../../Common/MapGuideCommon \
  -I../../../Oem/gd/freetype/include \
  -I../../../Common/MdfModel \
  -I../../../Common/MdfParser \
  -I../../../Common/Stylization \
  -I../../../Common/Renderers

bin_PROGRAMS = mgserver

ServerCoreBuild.o : Makefile

AM_CXXFLAGS = @CXXFLAGS@ -DACE_HAS_SVC_DLL=1

mgserver_SOURCES = \
  ServerCoreBuild.cpp

include_SOURCES = \
  ClientAcceptor.cpp \
  ConnectionTimeoutEventHandler.cpp \
  DataConnectionTimeoutEventHandler.cpp \
  DataTransactionTimeoutEventHandler.cpp \
  EventTimer.cpp \
  EventTimerManager.cpp \
  FeatureServiceCacheTimeLimitEventHandler.cpp \
  main.cpp \
  OperationThread.cpp \
  PerformanceLoggingEventHandler.cpp \
  RepositoryCheckpointEventHandler.cpp \
  ResourceChangeEventHandler.cpp \
  Server.cpp \
  ServerFactory.cpp \
  ServiceHandlerFactory.cpp \
  ServiceRegistrationEventHandler.cpp \
  SessionTimeoutEventHandler.cpp \
  SignalHandler.cpp \
  TimedEvent.cpp \
  TimedEventHandler.cpp

noinst_HEADERS = $(include_SOURCES) \
  ClientAcceptor.h \
  ConnectionTimeoutEventHandler.h \
  DataConnectionTimeoutEventHandler.h \
  DataTransactionTimeoutEventHandler.h \
  EventTimer.h \
  EventTimerManager.h \
  FeatureServiceCacheTimeLimitEventHandler.h \
  OperationThread.h \
  PerformanceLoggingEventHandler.h \
  RepositoryCheckpointEventHandler.h \
  ResourceChangeEventHandler.h \
  Server.h \
  ServiceHandlerFactory.h \
  ServiceRegistrationEventHandler.h \
  SessionTimeoutEventHandler.h \
  SignalHandler.h \
  TimedEvent.h \
  TimedEventHandler.h

mgserver_LDADD = \
  -lACE \
  -ldb_cxx \
  -ldbxml \
  -ldwfcore \
  -ldwftk \
  -ldwfemap \
  -lexpat \
  -lFDO-3.5.0 \
  -lExpressionEngine \
  -lxalanMsg \
  -lxalan-c \
  -lxerces-c \
  -lxqilla \
  -lMgFoundation \
  -lMgGeometry \
  -lMgPlatformBase \
  -lMgMapGuideCommon \
  -lMgServerAdminService \
  -lMgServerBase \
  -lMgServerCache \
  -lMgServerDrawingService \
  -lMgServerFeatureService \
  -lMgServerKmlService \
  -lMgServerManager \
  -lMgServerMappingService \
  -lMgServerRenderingService \
  -lMgServerResourceService \
  -lMgServerSiteService \
  -lMgServerThread \
  -lMgServerTileService \
  -lMgMdfModel \
  -lMgMdfParser \
  -lMgStylization \
  -lMgRenderers \
  ../../../Common/Security/libMgSecurity.a

mgserver_LDFLAGS = \
  -R$(map_fdo_lib) \
  -R$(map_srcdir)/Oem/ACE/ACE_wrappers/ace \
  -R$(map_srcdir)/Oem/DWFTK7.1/develop/global/src/dwfcore \
  -R$(map_srcdir)/Oem/DWFTK7.1/develop/global/src/dwf \
  -R$(map_srcdir)/Oem/DWFTK7.1/develop/global/src/dwfemap \
  -R$(map_srcdir)/Oem/dbxml-2.3.10/install/lib \
  -R$(map_srcdir)/Common/CoordinateSystem/.libs \
  -R$(map_srcdir)/Common/MdfModel/.libs \
  -R$(map_srcdir)/Common/MdfParser/.libs \
  -R$(map_srcdir)/Common/Stylization/.libs \
  -R$(map_srcdir)/Common/Renderers/.libs \
  -R$(map_srcdir)/Common/Foundation/.libs \
  -R$(map_srcdir)/Common/Geometry/.libs \
  -R$(map_srcdir)/Common/PlatformBase/.libs \
  -R$(map_srcdir)/Common/MapGuideCommon/.libs \
  -R$(map_srcdir)/Server/src/Common/Base/.libs \
  -R$(map_srcdir)/Server/src/Common/Cache/.libs \
  -R$(map_srcdir)/Server/src/Common/Manager/.libs \
  -R$(map_srcdir)/Server/src/Common/Thread/.libs \
  -R$(map_srcdir)/Server/src/Services/Drawing/.libs \
  -R$(map_srcdir)/Server/src/Services/Feature/.libs \
  -R$(map_srcdir)/Server/src/Services/Kml/.libs \
  -R$(map_srcdir)/Server/src/Services/Mapping/.libs \
  -R$(map_srcdir)/Server/src/Services/Rendering/.libs \
  -R$(map_srcdir)/Server/src/Services/Resource/.libs \
  -R$(map_srcdir)/Server/src/Services/ServerAdmin/.libs \
  -R$(map_srcdir)/Server/src/Services/Site/.libs \
  -R$(map_srcdir)/Server/src/Services/Tile/.libs \
  -R$(map_srcdir)/Server/src/$(map_unittesting_dir)/.libs \
  -L$(map_fdo_lib) \
  -L../../../Oem/ACE/ACE_wrappers/ace \
  -L../../../Oem/DWFTK7.1/develop/global/src/dwfcore \
  -L../../../Oem/DWFTK7.1/develop/global/src/dwf \
  -L../../../Oem/DWFTK7.1/develop/global/src/dwfemap \
  -L../../../Oem/dbxml-2.3.10/install/lib \
  -L$(top_builddir)/Common/CoordinateSystem \
  -L../../../Common/MdfModel \
  -L../../../Common/MdfParser \
  -L../../../Common/Stylization \
  -L../../../Common/Renderers \
  -L../../../Common/Foundation \
  -L../../../Common/Geometry \
  -L../../../Common/PlatformBase \
  -L../../../Common/MapGuideCommon \
  -L../Common/Base \
  -L../Common/Cache \
  -L../Common/Manager \
  -L../Common/Thread \
  -L../Services/Drawing \
  -L../Services/Feature \
  -L../Services/Kml \
  -L../Services/Mapping \
  -L../Services/Rendering \
  -L../Services/Resource \
  -L../Services/ServerAdmin \
  -L../Services/Site \
  -L../Services/Tile

UnitTestFiles :
	@mkdir -p ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/ResourceService/LibraryRepositoryContent.xml ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/ResourceService/SampleRepositoryHeader.xml ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/ResourceService/SampleRepositoryContent.xml ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/ResourceService/Shuttle.zip ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/ResourceService/Shuttle.zip ../UnitTestFiles/World.mgp
	@cp -f ../../../UnitTest/TestData/ResourceService/World_Countries.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/ResourceService/TEST.LayerDefinition ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/TEST.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/TEST.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_Parcels.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_Parcels.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_BuildingOutlines.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_BuildingOutlines.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_VotingDistricts.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_VotingDistricts.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/TESTChainedInner1ToManyJoin.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Empty.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Empty.sdf ../UnitTestFiles	
	@cp -f ../../../UnitTest/TestData/FeatureService/SHP/Redding_Parcels.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SHP/Redding_Parcels.shp ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SHP/Redding_Parcels.dbf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SHP/Redding_Parcels.shx ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_AllElements.pl ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_NoLegend.pl ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_SymbolMart.sl ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_Symbols.dwf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_Sheboygan.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_HydrographicPolygons.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_HydrographicPolygons.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_HydrographicPolygons.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_Rail.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_Rail.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MappingService/UT_Rail.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MdfModel/MdfTestTypeStyles.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MdfModel/MdfTestCompoundSymbol.sd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/MdfModel/MdfTestSimpleSymbol.sd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/areasymbol.sd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/linesymbol.sd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/symbol.sd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/symbolp.sd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/MTYP1500a.sd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyLines.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyLines.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyLines.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyLines.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyLinesCrossTick.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyLinesCrossTick.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyPoints.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyPoints.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyPoints.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyPoints.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyPointsParam.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyPointsParam.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyRoads.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyRoads.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyRoads.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyRoads.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyParcels.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_SymbologyPolygons.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_Annotation1.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_Annotation1.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_Annotation2.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_Annotation2.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_Annotation3.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/Symbology/UT_Annotation3.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_BaseMap.mdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_Parcels.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_Parcels.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_Parcels.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_RoadCenterLines.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_RoadCenterLines.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_RoadCenterLines.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_VotingDistricts.fs ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_VotingDistricts.ldf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/TileService/UT_VotingDistricts.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/ServerAdmin/1.1.1.mgd ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/CoordinateSystems/epsgcodes.txt ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/CoordinateSystems/ogcwkts.txt ../UnitTestFiles
	@chmod +w ../UnitTestFiles/*.sdf

TESTS = unittest.sh

UnitTesting :
	test -d $(mentor_dict_install_dir) || (mkdir -p $(mentor_dict_install_dir); \
				cp -f -r $(mentor_dict_source_dir)/* $(mentor_dict_install_dir);)
	@echo "#!/bin/sh" > unittest.sh
	@echo "export MENTOR_DICTIONARY_PATH=$(mentor_dict_install_dir)" >> unittest.sh
	@echo "echo MENTOR_DICTIONARY_PATH=\$$MENTOR_DICTIONARY_PATH" >> unittest.sh
	@echo "ulimit -s 1024" >> unittest.sh
	@echo "./mgserver test" >> unittest.sh
	@chmod a+x unittest.sh

unittest.sh : all serverconfig_linux.ini UnitTestFiles $(map_unittesting_dir)

serverconfig_linux.ini :
	@sed -e 's#^FdoPath.*=.*#FdoPath                            = $(map_fdo_lib)#' \
             -e 's#^LogsPath.*=.*#LogsPath                           = Logs/#' \
             -e 's#^ResourcesPath.*=.*#ResourcesPath                      = Resources/#' \
             -e 's#^TempPath.*=.*#TempPath                           = Temp/#' \
             -e 's#^WmsDocumentPath.*=.*#WmsDocumentPath                    = Wms/#' \
             -e 's#^WfsDocumentPath.*=.*#WfsDocumentPath                    = Wfs/#' \
             -e 's#^TileCachePath.*=.*#TileCachePath                      = Repositories/TileCache/#' \
             -e 's#^LibraryResourceDataFilePath.*=.*#LibraryResourceDataFilePath        = Repositories/Library/DataFiles/#' \
             -e 's#^LibraryRepositoryPath.*=.*#LibraryRepositoryPath              = Repositories/Library/#' \
             -e 's#^SessionResourceDataFilePath.*=.*#SessionResourceDataFilePath        = Repositories/Session/DataFiles/#' \
             -e 's#^SessionRepositoryPath.*=.*#SessionRepositoryPath              = Repositories/Session/#' \
             -e 's#^SiteRepositoryPath.*=.*#SiteRepositoryPath                 = Repositories/Site/#' \
             -e 's#^PackagesPath.*=.*#PackagesPath                       = Packages/#' \
             -e 's#^ResourceSchemaFilePath.*=.*#ResourceSchemaFilePath             = ../../../Common/Schema/#' \
             -e 's#^ResourceDataFileTrashFolderName.*=.*#ResourceDataFileTrashFolderName    = Trash/#' \
             -e 's#^LegendFont.*=.*#LegendFont                         = Complex#' \
           serverconfig.ini > serverconfig_linux.ini
	@cp -f serverconfig_linux.ini serverconfig.ini

Repositories :
	@mkdir Repositories
	@mkdir -p Repositories/Library/DataFiles
	@mkdir -p Repositories/Session/DataFiles
	@mkdir -p Repositories/Site
	@mkdir -p Repositories/TileCache

Trash :
	@mkdir Trash

Packages :
	@mkdir Packages

Temp :
	@mkdir Temp

Logs :
	@mkdir Logs

Resources :
	@mkdir Resources
	@cp -f ../../../Common/MapGuideCommon/Resources/mapguide_en.res Resources/mapguide_en.res

Schema :
	@mkdir Schema
	@cp -f ../../../Common/Schema/*.xsd Schema/

all : Schema Resources Logs Temp Trash Packages Repositories serverconfig_linux.ini

logfiledir = ${prefix}/Logs
tempdir = ${prefix}/Temp
trashdir = ${prefix}/Trash
packagesdir = ${prefix}/Packages
repositoriesdir = ${prefix}/Repositories
repositoryadmindir = ${prefix}/RepositoryAdmin
resourcesdir = ${prefix}/Resources
schemadir = ${prefix}/Schema
wmsdir = ${prefix}/Wms
wfsdir = ${prefix}/Wfs
fdodir = $(map_fdo_lib)
daemondir = /var/lock/mgserver

install-data-local:
	test -d $(logfiledir) || $(mkinstalldirs) $(logfiledir)
	test -d $(tempdir) || $(mkinstalldirs) $(tempdir)
	test -d $(trashdir) || $(mkinstalldirs) $(trashdir)
	test -d $(packagesdir) || $(mkinstalldirs) $(packagesdir)
	test -d $(repositoriesdir) || $(mkinstalldirs) $(repositoriesdir)
	test -d $(repositoriesdir)/Library || $(mkinstalldirs) $(repositoriesdir)/Library
	test -d $(repositoriesdir)/Library/DataFiles || $(mkinstalldirs) $(repositoriesdir)/Library/DataFiles
	test -d $(repositoriesdir)/Session || $(mkinstalldirs) $(repositoriesdir)/Session
	test -d $(repositoriesdir)/Session/DataFiles || $(mkinstalldirs) $(repositoriesdir)/Session/DataFiles
	test -d $(repositoriesdir)/Site || $(mkinstalldirs) $(repositoriesdir)/Site
	test -d $(repositoryadmindir) || $(mkinstalldirs) $(repositoryadmindir)
	test -d $(resourcesdir) || $(mkinstalldirs) $(resourcesdir)
	test -d $(wmsdir) || $(mkinstalldirs) $(wmsdir)
	test -d $(wfsdir) || $(mkinstalldirs) $(wfsdir)
	test -d $(fdodir) || $(mkinstalldirs) $(fdodir)
	test -d $(daemondir) || $(mkinstalldirs) $(daemondir)
	@cp -fp ../../../Common/MapGuideCommon/Resources/*.res $(resourcesdir)
	@cp -fp ../Wms/* $(wmsdir)
	@cp -fp ../Wfs/* $(wfsdir)
	test -d $(schemadir) || $(mkinstalldirs) $(schemadir)
	test -d $(mentor_dict_install_dir) || ($(mkinstalldirs) $(mentor_dict_install_dir); \
				cp -f -r $(mentor_dict_source_dir)/* $(mentor_dict_install_dir);)
	@cp -fp ../../../Common/Schema/*.xsd $(schemadir)
	@cp -fp ../../../Oem/DWFTK7.1/develop/global/fonts/* ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_archive ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_checkpoint ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_dump ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_hotbackup ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_load ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_recover ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_upgrade ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/db_verify ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/dbxml ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/dbxml_dump ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.3.10/install/bin/dbxml_load ${prefix}/bin/
	@cp -fp ../../RepositoryAdmin/*.sh $(repositoryadmindir)
	@cp -fp ../../RepositoryAdmin/*.php $(repositoryadmindir)
	@cp -fp ../../RepositoryAdmin/*.inc $(repositoryadmindir)
	@cp -fp ../../RepositoryAdmin/*.pdf $(repositoryadmindir)
	@cp -fp ../../RepositoryAdmin/license.txt $(repositoryadmindir)
	@cp -fp ../../RepositoryAdmin/php.ini $(repositoryadmindir)
	@pushd $(fdodir); ln -s libxalanMsg.so.17.0 libxalanMsg.so.17; ln -s libxalanMsg.so.17.0 libxalanMsg.so; popd;
	@pushd $(fdodir); ln -s libxerces-c.so.25.0 libxerces-c.so.25; ln -s libxerces-c.so.25.0 libxerces-c.so; popd;

install-data-hook:
	@sed -e 's#^FdoPath.*=.*#FdoPath                            = $(map_fdo_lib)#' \
             -e 's#^LogsPath.*=.*#LogsPath                           = ${prefix}/Logs/#' \
             -e 's#^ResourcesPath.*=.*#ResourcesPath                      = ${prefix}/Resources/#' \
             -e 's#^TempPath.*=.*#TempPath                           = ${prefix}/Temp/#' \
             -e 's#^WmsDocumentPath.*=.*#WmsDocumentPath                    = ${prefix}/Wms/#' \
             -e 's#^WfsDocumentPath.*=.*#WfsDocumentPath                    = ${prefix}/Wfs/#' \
             -e 's#^TileCachePath.*=.*#TileCachePath      = ${prefix}/Repositories/TileCache/#' \
             -e 's#^LibraryResourceDataFilePath.*=.*#LibraryResourceDataFilePath        = ${prefix}/Repositories/Library/DataFiles/#' \
             -e 's#^LibraryRepositoryPath.*=.*#LibraryRepositoryPath              = ${prefix}/Repositories/Library/#' \
             -e 's#^SessionResourceDataFilePath.*=.*#SessionResourceDataFilePath        = ${prefix}/Repositories/Session/DataFiles/#' \
             -e 's#^SessionRepositoryPath.*=.*#SessionRepositoryPath              = ${prefix}/Repositories/Session/#' \
             -e 's#^SiteRepositoryPath.*=.*#SiteRepositoryPath                 = ${prefix}/Repositories/Site/#' \
             -e 's#^PackagesPath.*=.*#PackagesPath                       = ${prefix}/Packages/#' \
             -e 's#^ResourceSchemaFilePath.*=.*#ResourceSchemaFilePath             = ${prefix}/Schema/#' \
             -e 's#^ResourceDataFileTrashFolderName.*=.*#ResourceDataFileTrashFolderName    = Trash#' \
           serverconfig.ini > ${prefix}/bin/serverconfig.ini
	@echo '#!/bin/bash' > ${prefix}/bin/mgserver.sh
	@echo '' >> ${prefix}/bin/mgserver.sh
	@echo 'export MENTOR_DICTIONARY_PATH=$(mentor_dict_install_dir)' >> ${prefix}/bin/mgserver.sh
	@echo 'export LD_LIBRARY_PATH=$(fdodir):"$$LD_LIBRARY_PATH"' >> ${prefix}/bin/mgserver.sh
	@echo 'ulimit -s 1024' >> ${prefix}/bin/mgserver.sh
	@echo 'pushd ${prefix}/bin' >> ${prefix}/bin/mgserver.sh
	@cp -f ${prefix}/bin/mgserver.sh ${prefix}/bin/mgserverd.sh
	@echo 'if [ ! -d ${daemondir} ]; then' >> ${prefix}/bin/mgserverd.sh
	@echo '  mkdir ${daemondir}' >> ${prefix}/bin/mgserverd.sh
	@echo 'fi' >> ${prefix}/bin/mgserverd.sh
	@echo './mgserver run' >> ${prefix}/bin/mgserver.sh
	@echo './mgserver daemon' >> ${prefix}/bin/mgserverd.sh
	@echo 'popd' >> ${prefix}/bin/mgserver.sh
	@echo 'popd' >> ${prefix}/bin/mgserverd.sh
	@chmod a+x ${prefix}/bin/mgserver*.sh
