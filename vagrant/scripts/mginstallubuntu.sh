#!/bin/bash
TEMPDIR=/tmp/build_mapguide
URL="http://download.osgeo.org/mapguide/releases/2.6/RC1/ubuntu12"
#URL="http://192.168.0.5/downloads/ubuntu12"
FDOVER_MAJOR_MINOR=3.9
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.0
FDOBUILD=0
FDOARCH=i386
FDOVER=${FDOVER_MAJOR_MINOR_REV}-${FDOBUILD}_${FDOARCH}
MGVER_MAJOR_MINOR=2.6
MGVER_MAJOR_MINOR_REV=${MGVER_MAJOR_MINOR}.0
MGBUILD=0
MGARCH=i386
MGVER=${MGVER_MAJOR_MINOR_REV}-${MGBUILD}_${MGARCH}

# Must have root
if [[ $EUID -ne 0 ]]; then
	echo "You must run this script with superuser privileges"
	exit 1
fi

# Install required packages 
apt-get -y install dialog libexpat1 libssl1.0.0 odbcinst unixodbc libcurl3 libxslt1.1

DIALOG=${DIALOG=dialog}

main()
{
	dialog_welcome
	dialog_fdo_provider
	start_install
	install_mapguide_packages
	post_install
}

dialog_welcome()
{
	$DIALOG --backtitle "MapGuide Open Source Ubuntu installer" \
	        --title "Welcome" --clear \
        	--yesno "Welcome to the MapGuide Open Source Ubuntu installer. Would you like to proceed?" 10 30

	case $? in
	  1)
		echo "Cancelled"
		exit 1;;
	  255)
		echo "Cancelled"
		exit 255;;
	esac
}

dialog_fdo_provider()
{
	tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
	trap "rm -f $tempfile" 0 1 2 5 15

	#arcsde    	"OSGeo FDO Provider for ArcSDE" off \
	# Disable RDBMS provider selection by default
	$DIALOG --backtitle "MapGuide Open Source Ubuntu installer" \
			--title "FDO Providers" --clear \
		    --checklist "Check the FDO Providers you want to install" 20 61 5 \
		    sdf  		"OSGeo FDO Provider for SDF" ON \
		    shp    		"OSGeo FDO Provider for SHP" ON \
		    sqlite 		"OSGeo FDO Provider for SQLite" ON \
		    gdal    	"OSGeo FDO Provider for GDAL" ON \
   		    ogr    		"OSGeo FDO Provider for OGR" ON \
   		    kingoracle  "OSGeo FDO Provider for Oracle" off \
		    rdbms	"RDBMS FDO Providers (ODBC, MySQL, PostgreSQL)" off \
   		    wfs    		"OSGeo FDO Provider for WFS" ON \
		    wms   		"OSGeo FDO Provider for WMS" ON  2> $tempfile

	choice=`cat $tempfile | sed s/\"//g`
	case $? in
	  1)
		echo "Cancelled"
		exit 1;;
	  255)
		echo "Cancelled"
		exit 255;;
	esac
}

start_install()
{
	# set initial registration state
	arcsde_registered=0
	gdal_registered=0
	kingoracle_registered=0
	rdbms_registered=0
	ogr_registered=0
	sdf_registered=0
	shp_registered=0
	sqlite_registered=0
	wfs_registered=0
	wms_registered=0

	# Include core and rdbms packages regardless of choice.
	choice="core rdbms $choice"
	# Download and install Ubuntu packages for FDO
	for file in $choice
	do
	  #echo "Downloading ${URL}/fdo-${file}_${FDOVER}.deb"
	  wget -N ${URL}/fdo-${file}_${FDOVER}.deb
	  #echo "Installing fdo-${file}_${FDOVER}.deb"
  	  dpkg -E -G --install fdo-${file}_${FDOVER}.deb
	done

	# Nuke the old providers.xml, we're rebuiding it
	providersxml=/usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}/lib/providers.xml
	echo -ne "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\" ?>" > ${providersxml}
	echo -ne "\n<FeatureProviderRegistry>" >> ${providersxml}
	for file in $choice
	do
		case $file in
		  arcsde)
			if [ $arcsde_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering ArcSDE Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.ArcSDE.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for ArcSDE</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read/write access to an ESRI ArcSDE-based data store, using Oracle and SQL Server</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libArcSDEProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			arcsde_registered=1
			;;
		  gdal)
			if [ $gdal_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering GDAL Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.Gdal.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for GDAL</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>FDO Provider for GDAL</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libGRFPProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			gdal_registered=1
			;;
		  kingoracle)
			if [ $kingoracle_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering King Oracle Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.KingOracle.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for Oracle</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read/write access to spatial and attribute data in Oracle Spatial</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libKingOracleProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			kingoracle_registered=1
			;;
		  rdbms)
			if [ $rdbms_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering ODBC Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.ODBC.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for ODBC</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>FDO Provider for ODBC</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libFdoODBC.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			echo "Registering PostgreSQL Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.PostgreSQL.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for PostgreSQL</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read/write access to PostgreSQL/PostGIS-based data store. Supports spatial data types and spatial query operations</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libFdoPostgreSQL.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			echo "Registering MySQL Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.MySQL.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for MySQL</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>FDO Provider for MySQL</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libFdoMySQL.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			rdbms_registered=1
			;;
		  ogr)
			if [ $ogr_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering OGR Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.OGR.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for OGR</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>FDO Access to OGR Data Sources</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libOGRProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			ogr_registered=1
			;;
		  sdf)
			if [ $sdf_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering SDF Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.SDF.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for SDF</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read/write access to Autodesk's spatial database format, a file-based geodatabase that supports multiple features/attributes, spatial indexing and file-locking</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libSDFProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			sdf_registered=1
			;;
		  shp)
			if [ $shp_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering SHP Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.SHP.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for SHP</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read/write access to spatial and attribute data in an ESRI SHP file</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libSHPProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			shp_registered=1
			;;
		  sqlite)
			if [ $sqlite_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering SQLite Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.SQLite.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for SQLite</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read/write access to feature data in a SQLite file</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libSQLiteProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			sqlite_registered=1
			;;
		  wfs)
			if [ $wfs_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering WFS Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.WFS.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for WFS</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read access to OGC WFS-based data store</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libWFSProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			wfs_registered=1
			;;
		  wms)
			if [ $wms_registered -eq 1 ];
			then
				continue
			fi
			echo "Registering WMS Provider"
			echo -ne "\n  <FeatureProvider>" >> ${providersxml}
			echo -ne "\n    <Name>OSGeo.WMS.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
			echo -ne "\n    <DisplayName>OSGeo FDO Provider for WMS</DisplayName>" >> ${providersxml}
			echo -ne "\n    <Description>Read access to OGC WMS-based data store</Description>" >> ${providersxml}
			echo -ne "\n    <IsManaged>False</IsManaged>" >> ${providersxml}
			echo -ne "\n    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
			echo -ne "\n    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
			echo -ne "\n    <LibraryPath>libWMSProvider.so</LibraryPath>" >> ${providersxml}
			echo -ne "\n  </FeatureProvider>" >> ${providersxml}
			wms_registered=1
			;;
		esac
	done
	echo -ne "\n</FeatureProviderRegistry>" >> ${providersxml}
}

install_mapguide_packages()
{
	# Download Ubuntu packages for MapGuide
	for file in platformbase coordsys common server webextensions httpd
	do
	  echo "Downloading: ${URL}/mapguideopensource-${file}_${MGVER}.deb"
	  wget -N ${URL}/mapguideopensource-${file}_${MGVER}.deb
 	  echo "Installing: mapguideopensource-${file}_${MGVER}.deb"
   	  dpkg -E -G --install mapguideopensource-${file}_${MGVER}.deb
	done
}

post_install()
{
	echo "Creating lock file directory for MapGuide Server"
	# Create lock file directory for Server
	if [ ! -d /var/lock/mgserver ]; then
	  mkdir /var/lock/mgserver
	fi
	echo "Starting httpd"
	pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/bin
	./apachectl start
	popd
	echo "Starting mgserver"
	pushd /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin
	./mgserverd.sh
	popd
	echo "DONE!"
}

# Create temporary download directory
mkdir -p ${TEMPDIR}
pushd ${TEMPDIR}
main
popd