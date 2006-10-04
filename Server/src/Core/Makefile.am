AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = serverconfig.ini ServerCore.vcproj ServerCore.rc

prefix=@prefix@/server

INCLUDES = \
  -I../../../Oem/ACE/ACE_wrappers \
  -I../../../Oem/dbxml-2.2.13/xerces-c-src/src \
  -I $(map_fdo_include) \
  -I../Common \
  -I../Common/Base \
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
  -I../../../Common/Foundation \
  -I../../../Common/Geometry \
  -I../../../Common/PlatformBase \
  -I../../../Common/MapGuideCommon 

bin_PROGRAMS = mgserver

ServerCoreBuild.o : Makefile

AM_CXXFLAGS = @CXXFLAGS@ -DACE_HAS_SVC_DLL=1

mgserver_SOURCES = \
  ServerCoreBuild.cpp

include_SOURCES = \
  ClientAcceptor.cpp \
  ConnectionTimeoutEventHandler.cpp \
  DataConnectionTimeoutEventHandler.cpp \
  EventTimer.cpp \
  EventTimerManager.cpp \
  FeatureServiceCacheTimeLimitEventHandler.cpp \
  main.cpp \
  OperationThread.cpp \
  RepositoryCheckpointEventHandler.cpp \
  ResourceChangeEventHandler.cpp \
  Server.cpp \
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
  EventTimer.h \
  EventTimerManager.h \
  FeatureServiceCacheTimeLimitEventHandler.h \
  OperationThread.h \
  RepositoryCheckpointEventHandler.h \
  ResourceChangeEventHandler.h \
  Server.h \
  ServiceHandlerFactory.h \
  ServiceRegistrationEventHandler.h \
  SessionTimeoutEventHandler.h \
  SignalHandler.h \
  TimedEvent.h \
  TimedEventHandler.h \
  ../Common/ServerCommon.h

mgserver_LDADD = \
  -lACE \
  -ldb_cxx \
  -ldbxml \
  -ldwfcore \
  -ldwftk \
  -ldwfemap \
  -lexpat \
  -lFDO-3.2.0 \
  -lxalanMsg \
  -lxalan-c \
  -lxerces-c \
  -lpathan \
  -lMgFoundation \
  -lMgGeometry \
  -lMgPlatformBase \
  -lMgMapGuideCommon \
  -lMgCoordinateSystem \
  -lMgServerAdminService \
  -lMgServerBase \
  -lMgServerDrawingService \
  -lMgServerFactory \
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
  ../../../Common/Security/libMgSecurity.a

mgserver_LDFLAGS = -version-info 7:0:0 \
  -R$(map_fdo_lib) \
  -R$(map_srcdir)/Oem/ACE/ACE_wrappers/ace \
  -R$(map_srcdir)/Oem/DWFTK7.1/develop/global/src/dwfcore \
  -R$(map_srcdir)/Oem/DWFTK7.1/develop/global/src/dwf \
  -R$(map_srcdir)/Oem/DWFTK7.1/develop/global/src/dwfemap \
  -R$(map_srcdir)/Oem/dbxml-2.2.13/install/lib \
  -R$(map_srcdir)/$(map_coordsys_dir_path)/.libs \
  -R$(map_srcdir)/Common/MdfModel/.libs \
  -R$(map_srcdir)/Common/MdfParser/.libs \
  -R$(map_srcdir)/Common/Stylization/.libs \
  -R$(map_srcdir)/Common/Foundation/.libs \
  -R$(map_srcdir)/Common/Geometry/.libs \
  -R$(map_srcdir)/Common/PlatformBase/.libs \
  -R$(map_srcdir)/Common/MapGuideCommon/.libs \
  -R$(map_srcdir)/Server/src/Common/Base/.libs \
  -R$(map_srcdir)/Server/src/Common/Manager/.libs \
  -R$(map_srcdir)/Server/src/Common/Factory/.libs \
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
  -L../../../Oem/dbxml-2.2.13/install/lib \
  -L$(top_builddir)/$(map_coordsys_dir_path) \
  -L../../../Common/MdfModel \
  -L../../../Common/MdfParser \
  -L../../../Common/Stylization \
  -L../../../Common/Foundation \
  -L../../../Common/Geometry \
  -L../../../Common/PlatformBase \
  -L../../../Common/MapGuideCommon \
  -L../Common/Base \
  -L../Common/Manager \
  -L../Common/Factory \
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
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/TEST.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/TEST.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_Parcels.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_Parcels.sdf ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_BuildingOutlines.FeatureSource ../UnitTestFiles
	@cp -f ../../../UnitTest/TestData/FeatureService/SDF/Sheboygan_BuildingOutlines.sdf ../UnitTestFiles
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
	@cp -f ../../../UnitTest/TestData/CoordinateSystems/ogcwkts.txt ../UnitTestFiles
	@chmod +w ../UnitTestFiles/*.sdf

TESTS = unittest.sh

UnitTesting :
	@echo "#!/bin/sh" > unittest.sh
	@echo "if test -z \"\$$GDAL_DATA\"; then export GDAL_DATA=$(map_srcdir)/Oem/gdal-1.3.0/data; fi;" >> unittest.sh
	@echo "if test -z \"\$$PROJ_LIB\"; then export PROJ_LIB=$(map_srcdir)/Oem/proj-4.4.9/nad; fi;" >> unittest.sh
	@echo "echo GDAL_DATA=\$$GDAL_DATA" >> unittest.sh
	@echo "echo PROJ_LIB=\$$PROJ_LIB" >> unittest.sh
	@echo "./mgserver /testmode" >> unittest.sh
	@chmod a+x unittest.sh

unittest.sh : serverconfig_linux.ini UnitTestFiles $(map_unittesting_dir)

categories.txt : ../../../Common/CoordinateSystem/categories.txt
	@cp -f ../../../Common/CoordinateSystem/categories.txt .

serverconfig_linux.ini :
	@sed -e 's#^FdoPath.*=.*#FdoPath                            = $(map_fdo_lib)#' \
             -e 's#^LogsPath.*=.*#LogsPath                           = Logs#' \
             -e 's#^ResourcesPath.*=.*#ResourcesPath                      = ../Resources#' \
             -e 's#^TempPath.*=.*#TempPath                           = Temp#' \
             -e 's#^WmsDocumentPath.*=.*#WmsDocumentPath                    = Wms#' \
             -e 's#^WfsDocumentPath.*=.*#WfsDocumentPath                    = Wfs#' \
             -e 's#^TileCachePath.*=.*#TileCachePath      = Repositories/TileCache#' \
             -e 's#^LibraryResourceDataFilePath.*=.*#LibraryResourceDataFilePath        = Repositories/Library/DataFiles#' \
             -e 's#^LibraryRepositoryPath.*=.*#LibraryRepositoryPath              = Repositories/Library#' \
             -e 's#^SessionResourceDataFilePath.*=.*#SessionResourceDataFilePath        = Repositories/Session/DataFiles#' \
             -e 's#^SessionRepositoryPath.*=.*#SessionRepositoryPath              = Repositories/Session#' \
             -e 's#^SiteRepositoryPath.*=.*#SiteRepositoryPath                 = Repositories/Site#' \
             -e 's#^PackagesPath.*=.*#PackagesPath                       = Packages#' \
             -e 's#^ResourceSchemaFilePath.*=.*#ResourceSchemaFilePath             = ../../../Common/Schema#' \
             -e 's#^ResourceDataFileTrashFolderName.*=.*#ResourceDataFileTrashFolderName    = Trash#' \
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
	@cp -f ../Resources/server_en.res Resources/server_en.res

Schema :
	@mkdir Schema
	@cp -f ../../../Common/Schema/*.xsd Schema/

all : Schema Resources Logs Temp Trash Packages Repositories serverconfig_linux.ini categories.txt

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
proj4datadir = ${prefix}/bin/data
proj4naddir = ${prefix}/bin/nad
daemondir = /var/lock/mgserver
fdodir = $(map_fdo_lib)

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
	test -d $(proj4datadir) || $(mkinstalldirs) $(proj4datadir)
	test -d $(proj4naddir) || $(mkinstalldirs) $(proj4naddir)
	test -d $(fdodir) || $(mkinstalldirs) $(fdodir)
	test -d $(daemondir) || $(mkinstalldirs) $(daemondir)
	@cp -fp ../Resources/*.res $(resourcesdir)
	@cp -fp ../Wms/* $(wmsdir)
	@cp -fp ../Wfs/* $(wfsdir)
	test -d $(schemadir) || $(mkinstalldirs) $(schemadir)
	@cp -fp ../../../Common/Schema/*.xsd $(schemadir)
	@cp -fp ../../../Common/CoordinateSystem/categories.txt ${prefix}/bin/
	@cp -fp ../../../Oem/DWFTK7.1/develop/global/fonts/* ${prefix}/bin/
	@cp -fp ../../../Oem/gdal-1.3.0/data/* $(proj4datadir)
	@cp -fp ../../../Oem/proj-4.4.9/nad/* $(proj4naddir)
	@cp -fp ../../../Oem/dbxml-2.2.13/install/bin/db_archive ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.2.13/install/bin/db_checkpoint ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.2.13/install/bin/db_recover ${prefix}/bin/
	@cp -fp ../../../Oem/dbxml-2.2.13/install/bin/db_upgrade ${prefix}/bin/
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
	@echo '#!/bin/sh' > ${prefix}/bin/mgserver.sh
	@echo '' >> ${prefix}/bin/mgserver.sh
	@echo 'export GDAL_DATA=$(proj4datadir)' >> ${prefix}/bin/mgserver.sh
	@echo 'export PROJ_LIB=$(proj4naddir)' >> ${prefix}/bin/mgserver.sh
	@echo 'export LD_LIBRARY_PATH=$(fdodir):"$$LD_LIBRARY_PATH"' >> ${prefix}/bin/mgserver.sh
	@echo 'pushd ${prefix}/bin' >> ${prefix}/bin/mgserver.sh
	@cp -f ${prefix}/bin/mgserver.sh ${prefix}/bin/mgserverd.sh 
	@echo './mgserver /interactive' >> ${prefix}/bin/mgserver.sh
	@echo './mgserver /daemon' >> ${prefix}/bin/mgserverd.sh
	@echo 'popd' >> ${prefix}/bin/mgserver.sh
	@echo 'popd' >> ${prefix}/bin/mgserverd.sh
	@chmod a+x ${prefix}/bin/mgserver*.sh
	@rm -f $(fdodir)/providers.xml
	@echo '<?xml version="1.0" encoding="UTF-8" standalone="no" ?>' > $(fdodir)/providers.xml
	@echo '<FeatureProviderRegistry>' >> $(fdodir)/providers.xml
	@echo '' >> $(fdodir)/providers.xml
	@echo '  <FeatureProvider>' >> $(fdodir)/providers.xml
	@echo '    <Name>OSGeo.SHP.3.2</Name>' >> $(fdodir)/providers.xml
	@echo '    <DisplayName>OSGeo FDO Provider for SHP</DisplayName>' >> $(fdodir)/providers.xml
	@echo '    <Description>Read/write access to spatial and attribute data in an ESRI SHP file.</Description>' >> $(fdodir)/providers.xml
	@echo '    <IsManaged>False</IsManaged>' >> $(fdodir)/providers.xml
	@echo '    <Version>3.2.0.0</Version>' >> $(fdodir)/providers.xml
	@echo '    <FeatureDataObjectsVersion>3.2.0.0</FeatureDataObjectsVersion>' >> $(fdodir)/providers.xml
	@echo '    <LibraryPath>$(fdodir)/libShpProvider.so</LibraryPath>' >> $(fdodir)/providers.xml
	@echo '  </FeatureProvider>' >> $(fdodir)/providers.xml
	@echo '' >> $(fdodir)/providers.xml
	@echo '  <FeatureProvider>' >> $(fdodir)/providers.xml
	@echo '    <Name>OSGeo.SDF.3.2</Name>' >> $(fdodir)/providers.xml
	@echo '    <DisplayName>OSGeo FDO Provider for SDF</DisplayName>' >> $(fdodir)/providers.xml
	@echo '    <Description>Read/write access to Autodesk spatial database format, a file-based personal geodatabase that supports multiple features/attributes, spatial indexing, and file-locking.</Description>' >> $(fdodir)/providers.xml
	@echo '    <IsManaged>False</IsManaged>' >> $(fdodir)/providers.xml
	@echo '    <Version>3.2.0.0</Version>' >> $(fdodir)/providers.xml
	@echo '    <FeatureDataObjectsVersion>3.2.0.0</FeatureDataObjectsVersion>' >> $(fdodir)/providers.xml
	@echo '    <LibraryPath>$(fdodir)/libSDFProvider.so</LibraryPath>' >> $(fdodir)/providers.xml
	@echo '  </FeatureProvider>' >> $(fdodir)/providers.xml
	@echo '' >> $(fdodir)/providers.xml
	@echo '</FeatureProviderRegistry>' >> $(fdodir)/providers.xml
