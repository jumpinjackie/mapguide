#!/bin/sh

# Set safe shell defaults
set -euf

HEADLESS=0
TEMPDIR=/tmp/install_mapguide
URL_HOST="http://download.osgeo.org"
URL_RELPATH="/mapguide/releases"
FDOVER_MAJOR=0
FDOVER_MINOR=0
FDOVER_MAJOR_MINOR=${FDOVER_MAJOR}.${FDOVER_MINOR}
FDOVER_POINT=0
FDOVER_MAJOR_MINOR_REV=${FDOVER_MAJOR_MINOR}.${FDOVER_POINT}
FDOBUILD=0
FDOARCH=i386
FDOVER=${FDOVER_MAJOR_MINOR_REV}-${FDOBUILD}_${FDOARCH}
MGVER_MAJOR=0
MGVER_MINOR=0
MGVER_MAJOR_MINOR=${MGVER_MAJOR}.${MGVER_MINOR}
MGVER_POINT=0
MGVER_MAJOR_MINOR_REV=${MGVER_MAJOR_MINOR}.${MGVER_POINT}
MGRELEASELABEL="ReleaseLabel"
MGBUILD=0
MGARCH=i386
MGVER=${MGVER_MAJOR_MINOR_REV}-${MGBUILD}_${MGARCH}
INSTALLER_TITLE="MapGuide Open Source Ubuntu installer"

URL_ROOT="${URL_HOST}/${URL_RELPATH}/${MGVER_MAJOR_MINOR_REV}/${MGRELEASELABEL}"
URL_PART="ubuntu"
URL="$URL_ROOT/$URL_PART"

DEFAULT_SERVER_IP="127.0.0.1"

DEFAULT_ADMIN_PORT=2810
DEFAULT_CLIENT_PORT=2811
DEFAULT_SITE_PORT=2812

DEFAULT_HTTPD_PORT=8008
DEFAULT_TOMCAT_PORT=8009

csmap_choice="full"

server_ip="127.0.0.1"
webtier_server_ip="127.0.0.1"

admin_port=2810
client_port=2811
site_port=2812

httpd_port=8008
tomcat_port=8009

fdo_provider_choice=""

# Must have root
if ! [ "$(id -u)" = "0" ]; then
    echo "You must run this script with superuser privileges"
    exit 1
fi

while [ $# -gt 0 ]; do    # Until you run out of parameters...
    case "$1" in
        -headless|--headless)
            HEADLESS=1
            ;;
        -no-service-install|--no-service-install)
            NO_SERVICE_INSTALL=1
            ;;
        -no-mgserver-start|--no-mgserver-start)
            NO_MGSERVER_START=1
            ;;
        -no-tomcat-start|--no-tomcat-start)
            NO_TOMCAT_START=1
            ;;
        -no-httpd-start|--no-httpd-start)
            NO_HTTPD_START=1
            ;;
        -with-sdf|--with-sdf)
            fdo_provider_choice="$fdo_provider_choice sdf"
            #shift
            ;;
        -with-shp|--with-shp)
            fdo_provider_choice="$fdo_provider_choice shp"
            #shift
            ;;
        -with-sqlite|--with-sqlite)
            fdo_provider_choice="$fdo_provider_choice sqlite"
            #shift
            ;;
        -with-gdal|--with-gdal)
            fdo_provider_choice="$fdo_provider_choice gdal"
            #shift
            ;;
        -with-ogr|--with-ogr)
            fdo_provider_choice="$fdo_provider_choice ogr"
            #shift
            ;;
        -with-kingoracle|--with-kingoracle)
            fdo_provider_choice="$fdo_provider_choice kingoracle"
            #shift
            ;;
        -with-wfs|--with-wfs)
            fdo_provider_choice="$fdo_provider_choice wfs"
            #shift
            ;;
        -with-wms|--with-wms)
            fdo_provider_choice="$fdo_provider_choice wms"
            #shift
            ;;
        -server-ip|--server-ip)
            server_ip="$2"
            webtier_server_ip="$2"
            shift
            ;;
        -admin-port|--admin-port)
            admin_port=$2
            shift
            ;;
        -client-port|--client-port)
            client_port=$2
            shift
            ;;
        -site-port|--site-port)
            site_port=$2
            shift
            ;;
        -httpd-port|--httpd-port)
            httpd_port=$2
            shift
            ;;
        -tomcat-port|--tomcat-port)
            tomcat_port=$2
            shift
            ;;
        -help|--help)
            echo "Usage: $0 (options)"
            echo "Options:"
            echo "  --headless [Install headlessly (skip UI)]"
            echo "  --with-sdf [Include SDF Provider]"
            echo "  --with-shp [Include SHP Provider]"
            echo "  --with-sqlite [Include SQLite Provider]"
            echo "  --with-gdal [Include GDAL Provider]"
            echo "  --with-ogr [Include OGR Provider]"
            echo "  --with-kingoracle [Include King Oracle Provider]"
            echo "  --with-wfs [Include WFS Provider]"
            echo "  --with-wms [Include WMS Provider]"
            echo "  --server-ip [Server IP, default: 127.0.0.1]"
            echo "  --admin-port [Admin Server Port, default: 2810]"
            echo "  --client-port [Client Server Port, default: 2811]"
            echo "  --site-port [Site Server Port, default: 2812]"
            echo "  --httpd-port [HTTPD port, default: 8008]"
            echo "  --tomcat-port [Tomcat Port, default: 8009]"
            exit
            ;;
    esac
    shift   # Check next set of parameters.
done

echo "[install]: Installing pre-requisite packages"
if [ "$HEADLESS" != "1" ]
then
# Install required packages
apt-get -y -q install openjdk-7-jre libxml2 dialog libexpat1 libssl1.0.0 odbcinst unixodbc libcurl3 libxslt1.1 libmysqlclient18 libpq5
else
# Install required packages 
apt-get -y -q install openjdk-7-jre libxml2 libexpat1 libssl1.0.0 odbcinst unixodbc libcurl3 libxslt1.1 libmysqlclient18 libpq5
fi

DIALOG=${DIALOG=dialog}

main()
{
    if [ "$HEADLESS" != "1" ]
    then
        dialog_welcome
        dialog_fdo_provider
        dialog_server
        dialog_webtier
        #dialog_coordsys
    fi
    dump_configuration
    install_fdo
    install_mapguide_packages
    post_install
}

set_server_vars()
{
    vars=$(cat $1)
    set $vars
    server_ip=${1:-$DEFAULT_SERVER_IP}
    admin_port=${2:-$DEFAULT_ADMIN_PORT}
    client_port=${3:-$DEFAULT_CLIENT_PORT}
    site_port=${4:-$DEFAULT_SITE_PORT}
}

set_webtier_vars()
{
    vars=$(cat $1)
    set $vars
    webtier_server_ip=${1:-$DEFAULT_SERVER_IP}
    httpd_port=${2:-$DEFAULT_HTTPD_PORT}
    tomcat_port=${3:-$DEFAULT_TOMCAT_PORT}
}

dump_configuration()
{
    echo "********* Configuration Summary ************"
    echo " Default Ports (Server)"
    echo "  Admin: ${DEFAULT_ADMIN_PORT}"
    echo "  Client: ${DEFAULT_CLIENT_PORT}"
    echo "  Site: ${DEFAULT_SITE_PORT}"
    echo " Default Ports (WebTier)"
    echo "  Apache: ${DEFAULT_HTTPD_PORT}"
    echo "  Tomcat: ${DEFAULT_TOMCAT_PORT}"
    echo " Configured Ports (Server)"
    echo "  Admin: ${admin_port}"
    echo "  Client: ${client_port}"
    echo "  Site: ${site_port}"
    echo " Configured Ports (WebTier)"
    echo "  Apache: ${httpd_port}"
    echo "  Tomcat: ${tomcat_port}"
    echo " Other choices"
    echo "  FDO: ${fdo_provider_choice}"
    echo "  CS-Map: ${csmap_choice}"
    echo "  Server IP: ${server_ip}"
    echo "********************************************"
}

dialog_welcome()
{
    $DIALOG --backtitle "$INSTALLER_TITLE" \
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
    tempfile=$(mktemp 2>/dev/null) || tempfile=/tmp/test$$
    trap 'rm -f $tempfile' 0 1 2 5 15

    #arcsde    	"OSGeo FDO Provider for ArcSDE" off \
    # Disable RDBMS provider selection by default
    $DIALOG --backtitle "$INSTALLER_TITLE" \
            --title "FDO Providers" --clear \
            --checklist "Check the FDO Providers you want to install" 20 61 5 \
            sdf  		"OSGeo FDO Provider for SDF" ON \
            shp    		"OSGeo FDO Provider for SHP" ON \
            sqlite 		"OSGeo FDO Provider for SQLite" ON \
            gdal    	"OSGeo FDO Provider for GDAL" ON \
            ogr    		"OSGeo FDO Provider for OGR" ON \
            kingoracle  "OSGeo FDO Provider for Oracle" off \
            rdbms	    "RDBMS FDO Providers (ODBC, MySQL, PostgreSQL)" off \
            wfs    		"OSGeo FDO Provider for WFS" ON \
            wms   		"OSGeo FDO Provider for WMS" ON  2> $tempfile

    fdo_provider_choice=$(cat $tempfile | sed s/\"//g)
    case $? in
      1)
        echo "Cancelled"
        exit 1;;
      255)
        echo "Cancelled"
        exit 255;;
    esac
}

dialog_server()
{
    tempfile=$(mktemp 2>/dev/null) || tempfile=/tmp/form.$$
    dialog --backtitle "$INSTALLER_TITLE" --title "Server Configuration" \
            --form "\nSet the port numbers that the MapGuide Server will listen on" 25 60 16 \
            "Server IP:"   1 1 "${DEFAULT_SERVER_IP}"   1 25 25 30 \
            "Admin Port:"  2 1 "${DEFAULT_ADMIN_PORT}"  2 25 25 30 \
            "Client Port:" 3 1 "${DEFAULT_CLIENT_PORT}" 3 25 25 30 \
            "Site Port:"   4 1 "${DEFAULT_SITE_PORT}"   4 25 25 30 2> $tempfile
    case $? in
      1)
        echo "Cancelled"
        exit 1;;
      255)
        echo "Cancelled"
        exit 255;;
    esac
    set_server_vars $tempfile
    rm $tempfile
}

dialog_webtier()
{
    tempfile=$(mktemp 2>/dev/null) || tempfile=/tmp/form.$$
    dialog --backtitle "$INSTALLER_TITLE" --title "Web Tier Configuration" \
            --form "\nSet the port numbers that Apache/Tomcat will listen on" 25 60 16 \
            "Connect to Server IP:" 1 1 "${DEFAULT_SERVER_IP}"   1 25 25 30 \
            "Apache Port:"          2 1 "${DEFAULT_HTTPD_PORT}"  2 25 25 30 \
            "Tomcat Port:"          3 1 "${DEFAULT_TOMCAT_PORT}" 3 25 25 30 2> $tempfile
    case $? in
      1)
        echo "Cancelled"
        exit 1;;
      255)
        echo "Cancelled"
        exit 255;;
    esac
    set_webtier_vars $tempfile
    rm $tempfile
}

dialog_coordsys()
{
    tempfile=$(mktemp 2>/dev/null) || tempfile=/tmp/test$$
    trap 'rm -f $tempfile' 0 1 2 5 15

    dialog --backtitle "$INSTALLER_TITLE" \
            --title "Coordinate System Configuration" --clear \
            --radiolist "Choose the CS-Map profile you want for this MapGuide Installation" 20 80 5 \
            "full" "Download/Install the full set of data files" ON \
            "lite" "Download/Install the lite configuration (no grid files)" off  2> $tempfile
    csmap_choice=$(cat $tempfile)
    case $? in
      1)
        echo "Cancelled"
        exit 1;;
      255)
        echo "Cancelled"
        exit 255;;
    esac
}

install_fdo()
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
    fdo_provider_choice="core rdbms $fdo_provider_choice"
    # Download and install Ubuntu packages for FDO
    for file in $fdo_provider_choice
    do
        download_file="${TEMPDIR}/fdo-${file}_${FDOVER}.deb"
        wget -N -O "${download_file}" "${URL}/fdo-${file}_${FDOVER}.deb"
        dpkg -E -G --install "${download_file}"
    done

    # Nuke the old providers.xml, we're rebuiding it
    providersxml=/usr/local/fdo-${FDOVER_MAJOR_MINOR_REV}/lib/providers.xml
    echo "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\" ?>" > ${providersxml}
    echo "<FeatureProviderRegistry>" >> ${providersxml}
    for file in $fdo_provider_choice
    do
        case $file in
          arcsde)
            if [ $arcsde_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering ArcSDE Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.ArcSDE.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for ArcSDE</DisplayName>" >> ${providersxml}
            echo "    <Description>Read/write access to an ESRI ArcSDE-based data store, using Oracle and SQL Server</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libArcSDEProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            arcsde_registered=1
            ;;
          gdal)
            if [ $gdal_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering GDAL Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.Gdal.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for GDAL</DisplayName>" >> ${providersxml}
            echo "    <Description>FDO Provider for GDAL</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libGRFPProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            gdal_registered=1
            ;;
          kingoracle)
            if [ $kingoracle_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering King Oracle Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.KingOracle.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for Oracle</DisplayName>" >> ${providersxml}
            echo "    <Description>Read/write access to spatial and attribute data in Oracle Spatial</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libKingOracleProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            kingoracle_registered=1
            ;;
          rdbms)
            if [ $rdbms_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering ODBC Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.ODBC.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for ODBC</DisplayName>" >> ${providersxml}
            echo "    <Description>FDO Provider for ODBC</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libFdoODBC.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            echo "Registering PostgreSQL Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.PostgreSQL.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for PostgreSQL</DisplayName>" >> ${providersxml}
            echo "    <Description>Read/write access to PostgreSQL/PostGIS-based data store. Supports spatial data types and spatial query operations</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libFdoPostgreSQL.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            echo "Registering MySQL Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.MySQL.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for MySQL</DisplayName>" >> ${providersxml}
            echo "    <Description>FDO Provider for MySQL</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libFdoMySQL.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            rdbms_registered=1
            ;;
          ogr)
            if [ $ogr_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering OGR Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.OGR.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for OGR</DisplayName>" >> ${providersxml}
            echo "    <Description>FDO Access to OGR Data Sources</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libOGRProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            ogr_registered=1
            ;;
          sdf)
            if [ $sdf_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering SDF Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.SDF.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for SDF</DisplayName>" >> ${providersxml}
            echo "    <Description>Read/write access to Autodesk's spatial database format, a file-based geodatabase that supports multiple features/attributes, spatial indexing and file-locking</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libSDFProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            sdf_registered=1
            ;;
          shp)
            if [ $shp_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering SHP Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.SHP.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for SHP</DisplayName>" >> ${providersxml}
            echo "    <Description>Read/write access to spatial and attribute data in an ESRI SHP file</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libSHPProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            shp_registered=1
            ;;
          sqlite)
            if [ $sqlite_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering SQLite Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.SQLite.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for SQLite</DisplayName>" >> ${providersxml}
            echo "    <Description>Read/write access to feature data in a SQLite file</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libSQLiteProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            sqlite_registered=1
            ;;
          wfs)
            if [ $wfs_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering WFS Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.WFS.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for WFS</DisplayName>" >> ${providersxml}
            echo "    <Description>Read access to OGC WFS-based data store</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libWFSProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            wfs_registered=1
            ;;
          wms)
            if [ $wms_registered -eq 1 ];
            then
                continue
            fi
            echo "Registering WMS Provider"
            echo "  <FeatureProvider>" >> ${providersxml}
            echo "    <Name>OSGeo.WMS.${FDOVER_MAJOR_MINOR}</Name>" >> ${providersxml}
            echo "    <DisplayName>OSGeo FDO Provider for WMS</DisplayName>" >> ${providersxml}
            echo "    <Description>Read access to OGC WMS-based data store</Description>" >> ${providersxml}
            echo "    <IsManaged>False</IsManaged>" >> ${providersxml}
            echo "    <Version>${FDOVER_MAJOR_MINOR_REV}.0</Version>" >> ${providersxml}
            echo "    <FeatureDataObjectsVersion>${FDOVER_MAJOR_MINOR_REV}.0</FeatureDataObjectsVersion>" >> ${providersxml}
            echo "    <LibraryPath>libWMSProvider.so</LibraryPath>" >> ${providersxml}
            echo "  </FeatureProvider>" >> ${providersxml}
            wms_registered=1
            ;;
        esac
    done
    echo "</FeatureProviderRegistry>" >> ${providersxml}
}

install_mapguide_packages()
{
    # Download Ubuntu packages for MapGuide
    mapguide_packages="platformbase coordsys common server webextensions httpd"
    if [ "$csmap_choice" = "lite" ]; then
        mapguide_packages="platformbase coordsys-lite common server webextensions httpd"
    fi
    for file in $mapguide_packages
    do
        download_file="${TEMPDIR}/mapguideopensource-${file}_${MGVER}.deb"
        wget -N -O "${download_file}" "${URL}/mapguideopensource-${file}_${MGVER}.deb"
        dpkg -E -G --install "${download_file}"
    done
}

post_install()
{
    echo "[config]: Updating serverconfig.ini with configuration choices"
    sed -i 's/MachineIp.*= '"${DEFAULT_SERVER_IP}"'/MachineIp = '"${server_ip}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin/serverconfig.ini
    sed -i 's/IpAddress.*= '"${DEFAULT_SERVER_IP}"'/IpAddress = '"${server_ip}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin/serverconfig.ini
    sed -i 's/Port.*= '"${DEFAULT_ADMIN_PORT}"'/Port = '"${admin_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin/serverconfig.ini
    sed -i 's/Port.*= '"${DEFAULT_CLIENT_PORT}"'/Port = '"${client_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin/serverconfig.ini
    sed -i 's/Port.*= '"${DEFAULT_SITE_PORT}"'/Port = '"${site_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin/serverconfig.ini
    echo "[config]: Updating webconfig.ini with configuration choices"
    sed -i 's/IpAddress.*= '"${DEFAULT_SERVER_IP}"'/IpAddress = '"${webtier_server_ip}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/www/webconfig.ini
    sed -i 's/Port.*= '"${DEFAULT_ADMIN_PORT}"'/Port = '"${admin_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/www/webconfig.ini
    sed -i 's/Port.*= '"${DEFAULT_CLIENT_PORT}"'/Port = '"${client_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/www/webconfig.ini
    sed -i 's/Port.*= '"${DEFAULT_SITE_PORT}"'/Port = '"${site_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/www/webconfig.ini
    echo "[config]: Updating httpd.conf with configuration choices"
    sed -i 's/Listen '"${DEFAULT_HTTPD_PORT}"'/Listen '"${httpd_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/conf/httpd.conf
    sed -i 's/worker.worker1.port='"${DEFAULT_TOMCAT_PORT}"'/worker.worker1.port='"${tomcat_port}"'/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/conf/workers.properties
    sed -i 's/Connector port=\"'"${DEFAULT_TOMCAT_PORT}"'\"/Connector port=\"'"${tomcat_port}"'\"/g' /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/tomcat/conf/server.xml
    echo "[config]: Updating tomcat configs with configuration choices"

    echo "[config]: Fixing permissions for certain folders"
    chmod 777 /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/www/TempDir

    if [ "$HEADLESS" = "1" ] && [ "$NO_SERVICE_INSTALL" = "1" ];
    then
        echo "[config]: Skipping service registration as --headless and --no-service-install specified"
    else
        echo "[config]: Registering Services"
        ln -s /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/server/bin/mapguidectl /etc/init.d/mapguide
        ln -s /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/apache2/bin/apachectl /etc/init.d/apache-mapguide
        update-rc.d mapguide defaults 35 65
        update-rc.d apache-mapguide defaults 30 70
    fi
    
    if [ "$HEADLESS" = "1" ] && [ "$NO_HTTPD_START" = "1" ];
    then
        echo "[install]: Skipping httpd auto-start as --headless and --no-httpd-start specified"
    else
        if [ -e "/etc/init.d/apache-mapguide" ]; 
        then
            echo "[install]: Starting httpd"
            /etc/init.d/apache-mapguide start
        else
            echo "[install]: WARNING - apache-mapguide service entry not found"
        fi
    fi
    if [ "$HEADLESS" = "1" ] && [ "$NO_MGSERVER_START" = "1" ];
    then
        echo "[install]: Skipping mgserver auto-start as --headless and --no-mgserver-start specified"
    else
        if [ -e "/etc/init.d/mapguide" ]; 
        then
            echo "[install]: Starting mgserver"
            /etc/init.d/mapguide start
        else
            echo "[install]: WARNING - mgserver service entry not found"
        fi
    fi
    if [ "$HEADLESS" = "1" ] && [ "$NO_TOMCAT_START" = "1" ];
    then
        echo "[install]: Skipping tomcat auto-start as --headless and --no-tomcat-start specified"
    else
        echo "[install]: Starting tomcat"
        # This can be allowed to fail
        sh /usr/local/mapguideopensource-${MGVER_MAJOR_MINOR_REV}/webserverextensions/tomcat/bin/startup.sh || true
    fi
    echo "DONE!"
}

# Create temporary download directory
mkdir -p ${TEMPDIR}
main
