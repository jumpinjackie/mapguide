#!/bin/bash
TEMPDIR=/tmp/build_mapguide
#URL="http://download.osgeo.org/mapguide/releases/2.4.0/Test/ubuntu11"
URL=http://192.168.0.2/downloads/ubuntu11
FDOVER_MAJOR_MINOR=3.7
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.0
FDOVER=${FDOVER_MAJOR_MINOR_REV}-4308_i386
MGVER_MAJOR_MINOR=2.4
MGVER_MAJOR_MINOR_REV=${MGVER_MAJOR_MINOR}.0
MGVER=${MGVER_MAJOR_MINOR_REV}-6579_i386

# Must have root
if [[ $EUID -ne 0 ]]; then
	echo "You must run this script with superuser privileges"
	exit 1
fi

# Install required packages 
apt-get -y install dialog libexpat1 libssl0.9.8 odbcinst unixodbc libcurl3 libxslt1.1

DIALOG=${DIALOG=dialog}

fix_symlinks()
{
	echo "Fixing library symlinks for Ubuntu compatibility"
	# Resolve CentOS 5.4 / Ubuntu 9.10 shared lib differences with symlinks
	# Still applicable for Ubuntu 11.10
	if [ ! -e /lib/libcrypto.so.6 ]; then
	  ln -s /lib/libcrypto.so.0.9.8 /lib/libcrypto.so.6
	fi

	if [ ! -e /lib/libssl.so.6 ]; then
	  ln -s /lib/libssl.so.0.9.8 /lib/libssl.so.6
	fi

	if [ ! -e /lib/libexpat.so.0 ]; then
	  ln -s /lib/libexpat.so.1.5.2 /lib/libexpat.so.0
	fi

	if [ ! -e /usr/lib/libldap-2.3.so.0 ]; then
	  ln -s /usr/lib/libldap-2.4.so.2 /usr/lib/libldap-2.3.so.0
	fi

	if [ ! -e /usr/lib/liblber-2.3.so.0 ]; then
	  ln -s /usr/lib/liblber-2.4.so.2 /usr/lib/liblber-2.3.so.0
	fi
}

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

	# Disable RDBMS provider selection by default
	$DIALOG --backtitle "MapGuide Open Source Ubuntu installer" \
			--title "FDO Providers" --clear \
		    --checklist "Check the FDO Providers you want to install" 20 61 5 \
   		    arcsde    	"OSGeo FDO Provider for ArcSDE" off \
		    sdf  		"OSGeo FDO Provider for SDF" ON \
		    shp    		"OSGeo FDO Provider for SHP" ON \
		    sqlite 		"OSGeo FDO Provider for SQLite" ON \
		    gdal    	"OSGeo FDO Provider for GDAL" ON \
   		    odbc    	"OSGeo FDO Provider for ODBC" off \
   		    ogr    		"OSGeo FDO Provider for OGR" ON \
   		    kingoracle  "OSGeo FDO Provider for Oracle" off \
   		    postgis    	"OSGeo FDO Provider for PostgreSQL" off \
   		    mysql    	"OSGeo FDO Provider for MySQL" off \
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
	fix_symlinks

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
	providersxml=/usr/local/fdo-3.7.0/lib/providers.xml
	echo -ne "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\" ?>" > ${providersxml}
	echo -ne "\n<FeatureProviderRegistry>" >> ${providersxml}
	for file in $choice
	do
		case $file in
		  arcsde)
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
			;;
		  gdal)
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
			;;
		  kingoracle)
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
			;;
		  odbc)
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
			;;
		  ogr)
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
			;;
		  postgis)
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
			;;
		  mysql)
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
			;;
		  sdf)
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
			;;
		  shp)
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
			;;
		  sqlite)
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
			;;
		  wfs)
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
			;;
		  wms)
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
			;;
		esac
	done
	echo -ne "\n</FeatureProviderRegistry>" >> ${providersxml}
}

install_mapguide_packages()
{
	# Download Ubuntu packages for MapGuide
	for file in common server webextensions httpd
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
