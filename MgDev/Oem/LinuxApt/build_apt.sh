#
#! /bin/bash
#

# MapGuide Open Source Apache, Php, Tomcat Build Script
#
#**********************************************************
# Initialization
#**********************************************************

echo
echo "MapGuide Open Source build/install script for Apache, Php, Tomcat"
echo "-----------------------------------------------------------------"
echo

INSTALLDIR=/usr/local/mapguideopensource

HTTPD_VER=2.4.18
PHP_VER=5.6.23
TOMCAT_VER=7.0.68
CONNECTOR_VER=1.2.41
APR_VER=1.5.2
APR_UTIL_VER=1.5.4
PORT=8008
TOMCAT=0
TOMCAT_PORT=8009
START_HTTPD=0
START_TOMCAT=0

while [ $# -gt 0 ]; do    # Until you run out of parameters . . .
  case "$1" in
    -port|--port)
              PORT="$2"
              shift
              ;;
    -tomcat-port|--tomcat-port)
              TOMCAT_PORT="$2"
              shift
              ;;
    -prefix|--prefix)
              INSTALLDIR="$2"
              shift
              ;;
    -with-tomcat|--with-tomcat)
              TOMCAT=1
              shift
              ;;
    -start-tomcat|--start-tomcat)
              START_TOMCAT=1
              shift
              ;;
    -start-httpd|--start-httpd)
              START_HTTPD=1
              shift
              ;;
    -help|--help)
              echo "Usage: $0 (options)"
              echo "Options:"
              echo "  --port [Apache port number]"
              echo "  --prefix [installation directory]"
              echo "  --start-httpd Automatically start httpd once compiled"
              echo "  --with-tomcat Configure and install Tomcat"
              echo "  --tomcat-port [Tomcat port number]"
              echo "  --start-tomcat Automatically start tomcat once configured"
              echo "  --help Display usage"
              exit 0
              ;;
  esac
  shift       # Check next set of parameters.
done

INSTALLWEB=$INSTALLDIR/webserverextensions

echo "Apache will be built/installed to $INSTALLWEB/apache2."
echo "Php will be built/installed to $INSTALLWEB/php."
if [ "$TOMCAT" = "1" ]; then
  echo "Tomcat will be installed to $INSTALLWEB/tomcat."
fi
echo
echo "Apache will be configured to run on port $PORT."
if [ "$TOMCAT" = "1" ]; then
  echo "Tomcat will be configured to run on port $TOMCAT_PORT."
fi


# exit status of previous command
#echo $? 

check_apache_build ()
{
  error=$?
  if [ $error -ne 0 ]; then
    echo "Apache build failed ($error)."
    exit $error
  fi
}

check_php_build ()
{
  error=$?
  if [ $error -ne 0 ]; then
    echo "Php build failed ($error)."
    exit $error
  fi
}

check_apache_install ()
{
  error=$?
  if [ $error -ne 0 ]; then
    echo "Apache install failed ($error)."
    exit $error
  fi
}

check_php_install ()
{
  error=$?
  if [ $error -ne 0 ]; then
    echo "Php install failed ($error)."
    exit $error
  fi
}

check_tomcat_build ()
{
  error=$?
  if [ $error -ne 0 ]; then
    echo "Tomcat build failed ($error)."
    exit $error
  fi
}

check_tomcat_install ()
{
  error=$?
  if [ $error -ne 0 ]; then
    echo "Tomcat install failed ($error)."
    exit $error
  fi
}

#**********************************************************
# Apache build procedure
# Notes: Be sure to install pcre-devel (on CentOS) or an
# equivalent library in other Linux distributions
#**********************************************************
echo "Apache Httpd build started"
tar -jxf httpd-$HTTPD_VER.tar.bz2
if [ ! -d "httpd-$HTTPD_VER/srclib/apr" ]; then
	tar -jxf apr-$APR_VER.tar.bz2
	mv apr-$APR_VER httpd-$HTTPD_VER/srclib/apr
fi
if [ ! -d "httpd-$HTTPD_VER/srclib/aprutil" ]; then
	tar -jxf apr-util-$APR_UTIL_VER.tar.bz2
	mv apr-util-$APR_UTIL_VER httpd-$HTTPD_VER/srclib/apr-util
fi
pushd httpd-$HTTPD_VER
./configure --prefix=$INSTALLWEB/apache2 --enable-mods-shared=all \
--with-included-apr --with-port=$PORT
check_apache_build
make
check_apache_build
popd
echo "Apache Httpd build completed"

#**********************************************************
# Apache shutdown  procedure
# Notes: none
#**********************************************************
echo "Checking for existing Apache install"
if [ -d $INSTALLWEB/apache2/bin ]; then
  echo "Attempting to shutdown Apache"
  pushd $INSTALLWEB/apache2/bin
  ./apachectl stop
  popd
  echo "Attempting to remove old Apache and Php"
  pushd $INSTALLWEB
  rm -rf apache2
  rm -rf php
  popd
  echo "Completed uninstall of Apache and Php"
fi

#**********************************************************
# Apache install procedure
# Notes: none
#**********************************************************
echo "Apache install started"
pushd httpd-$HTTPD_VER
make install
check_apache_install
popd

pushd $INSTALLWEB/apache2/conf
# Prep httpd.conf to read mapguide configuration
echo "" > tmpfile
echo "Include conf/mapguide.conf" >> tmpfile
cp httpd.conf httpd.conf.mgorig_
cat httpd.conf.mgorig_ tmpfile > httpd.conf
rm httpd.conf.mgorig_
popd

echo "Apache install completed"

pushd $INSTALLWEB/apache2/conf
cat > mapguide.conf <<END-OF-CONFIGURATION


END-OF-CONFIGURATION

popd

# Create dummy mapagent.fcgi and symlinks for module.
mkdir -p $INSTALLWEB/www/mapagent
mkdir $INSTALLWEB/bin
pushd $INSTALLWEB/www/mapagent
cat > mapagent.fcgi <<END-OF-FCGI
// Empty file.  IIS application mappings and Apache script aliases are
used to bind this file to the actual MapAgent binaries
END-OF-FCGI
popd

mkdir -p $INSTALLWEB/lib
pushd $INSTALLWEB/apache2/modules
ln -s ../../lib/mod_mgmapagent.so mod_mgmapagent.so
popd

#**********************************************************
# Php build procedure
# Notes: none
#**********************************************************
echo "Php build started"
tar -jxf php-$PHP_VER.tar.bz2
pushd php-$PHP_VER
./configure --prefix=$INSTALLWEB/php --with-apxs2=$INSTALLWEB/apache2/bin/apxs --with-openssl \
--with-curl --enable-xml --enable-wddx --enable-shared  \
--enable-safe-mode --with-zlib --enable-zip --enable-mbstring=all --with-xsl=/usr/lib --with-gd --with-png --with-jpeg --with-freetype
check_php_build
make
check_php_build
popd
echo "Php build completed"

#**********************************************************
# Php install procedure
# Notes: none
#**********************************************************
echo "Php install started"
pushd php-$PHP_VER
make install
check_php_install
popd
echo "Php install completed"

#**********************************************************
# Tomcat connector build/install procedure
# Notes: none
#**********************************************************
if [ "$TOMCAT" = "1" ]; then
echo "Tomcat connector build/install started"
tar -zxf tomcat-connectors-$CONNECTOR_VER-src.tar.gz
pushd tomcat-connectors-$CONNECTOR_VER-src/native

./configure --with-apxs=$INSTALLWEB/apache2/bin/apxs
check_tomcat_build
make
check_tomcat_build
make install
check_tomcat_install
popd
echo "Tomcat connector build/install completed"
fi

#**********************************************************
# Tomcat install procedure
# Notes: none
#**********************************************************
if [ "$TOMCAT" = "1" ]; then
echo "Tomcat install started"
tar -zxf apache-tomcat-$TOMCAT_VER.tar.gz -C $INSTALLWEB
check_tomcat_install
pushd $INSTALLWEB
mv apache-tomcat-$TOMCAT_VER tomcat
check_tomcat_install
popd
echo "Tomcat install completed"
fi

#**********************************************************
# Configuration file updates for Apache
# Notes: none
#**********************************************************

echo "Apache configuration started"

pushd $INSTALLWEB/apache2/conf
# Prep httpd.conf to read mapguide configuration
echo "" > tmpfile
echo "Include conf/mapguide.conf" >> tmpfile
cp httpd.conf httpd.conf.mgorig_
# Activate mod_rewrite
sed -i "s/#LoadModule rewrite_module modules\/mod_rewrite.so/LoadModule rewrite_module modules\/mod_rewrite.so/g" httpd.conf.mgorig_
cat httpd.conf.mgorig_ tempfile > httpd.conf
rm httpd.conf.mgorig_
popd

# Required modifications for envvars
pushd $INSTALLWEB/apache2/bin
cat >> envvars <<END-OF-ENVVARS
export LD_LIBRARY_PATH=$INSTALLWEB/lib:$INSTALLWEB/php/lib:$INSTALLDIR/lib
export MENTOR_DICTIONARY_PATH=$INSTALLDIR/share/gis/coordsys
END-OF-ENVVARS
popd

# Required modifications to httpd.conf, append to mapguide.conf
pushd $INSTALLWEB/apache2/conf
cat >> mapguide.conf <<END-OF-CONFIGURATION

# Environment variables for MapGuide
SetEnv LD_LIBRARY_PATH "$INSTALLWEB/lib:$INSTALLWEB/php/lib:$INSTALLDIR/lib"
SetEnv MENTOR_DICTIONARY_PATH "$INSTALLDIR/share/gis/coordsys"

LoadModule mgmapagent_module modules/mod_mgmapagent.so

RewriteEngine On

#START NormalCGI PHP configuration
#ScriptAlias /php/ "$INSTALLWEB/php/bin/"
#Action application/x-httpd-php "/php/php"
#AddType application/x-httpd-php .php
#END NormalCGI PHP configuration

# CGI agent alias
#ScriptAlias /mapguide/mapagent/mapagent.fcgi "$INSTALLWEB/www/mapagent/mapagent"

# mapviewerajax to mapviewerphp rewrite rules
# comment out for java api/viewer
RewriteRule ^/mapguide/mapviewerajax/([^\?])(.*)$ /mapguide/mapviewerphp/\$1\$2 [PT]
RewriteRule ^/mapguide/mapviewerajax/(.*)$ /mapguide/mapviewerphp/ajaxviewer.php\$1 [PT]
RewriteRule ^/mapguide/mapviewerdwf/([^\?])(.*)$ /mapguide/mapviewerphp/\$1\$2 [PT]
RewriteRule ^/mapguide/mapviewerdwf/(.*)$ /mapguide/mapviewerphp/dwfviewer.php\$1 [PT]

# mapviewerajax to mapviewerjava aliases
# uncomment for java api/viewer
#RewriteRule ^/mapguide/mapviewerajax/([^\?])(.*)$ /mapguide/mapviewerjava/\$1\$2 [PT]
#RewriteRule ^/mapguide/mapviewerajax/(.*)$ /mapguide/mapviewerjava/ajaxviewer.jsp\$1 [PT]
#RewriteRule ^/mapguide/mapviewerdwf/([^\?])(.*)$ /mapguide/mapviewerjava/\$1\$2 [PT]
#RewriteRule ^/mapguide/mapviewerdwf/(.*)$ /mapguide/mapviewerjava/dwfviewer.jsp\$1 [PT]

Alias /mapguide "$INSTALLWEB/www/"

<Directory "$INSTALLWEB/www/">
  AllowOverride All
  Options All
  Require all granted

  AddHandler php5-script .php
  AddHandler mgmapagent_handler fcgi

  RewriteEngine on
  RewriteRule .* - [E=REMOTE_USER:%{HTTP:Authorization},L]
</Directory>

END-OF-CONFIGURATION
popd

echo "Apache configuration completed"

#**********************************************************
# Tomcat configuration procedure
# Notes: none
#**********************************************************
if [ "$TOMCAT" = "1" ]; then
echo "Tomcat configuration started"
pushd $INSTALLWEB/apache2/conf
cat >> mapguide.conf <<END-OF-CONFIGURATION

#Tomcat Integration
# Load mod_jk module
# Update this path to match your modules location
LoadModule    jk_module  modules/mod_jk.so
# Declare the module for <IfModule directive> (remove this line on Apache 2.x)
# AddModule     mod_jk.c
# Where to find workers.properties
# Update this path to match your conf directory location (put workers.properties next to httpd.conf)
JkWorkersFile "$INSTALLWEB/apache2/conf/workers.properties"
# Where to put jk logs
# Update this path to match your logs directory location (put mod_jk.log next to access_log)
JkLogFile     "$INSTALLWEB/apache2/logs/mod_jk.log"
# Set the jk log level [debug/error/info]
JkLogLevel    info
# Select the log format
JkLogStampFormat "[%a %b %d %H:%M:%S %Y] "
# JkOptions indicate to send SSL KEY SIZE, 
JkOptions     +ForwardKeySize +ForwardURICompat -ForwardDirectories
# JkRequestLogFormat set the request format 
JkRequestLogFormat     "%w %V %T"
# Send everything for context /examples to worker named worker1 (ajp13)
JkMount  /mapguide/mapviewerjava/* worker1
JkMount  /mapguide/javaviewersample/* worker1

END-OF-CONFIGURATION

cat > workers.properties <<END-OF-CONFIGURATION
# Define 1 real worker using ajp13
worker.list=worker1
# Set properties for worker1 (ajp13)
worker.worker1.type=ajp13
worker.worker1.host=localhost
worker.worker1.port=$TOMCAT_PORT
worker.worker1.lbfactor=50
worker.worker1.cachesize=10
worker.worker1.cache_timeout=600
worker.worker1.socket_keepalive=1
worker.worker1.recycle_timeout=300
END-OF-CONFIGURATION

popd

if [ ! -d $INSTALLWEB/tomcat/conf/Catalina/localhost ]; then
mkdir -p $INSTALLWEB/tomcat/conf/Catalina/localhost
fi

pushd $INSTALLWEB/tomcat/conf/Catalina/localhost
cat > mapguide.xml <<END_OF_CONFIGURATION
<!--

    Context configuration file for the Tomcat Manager Web App

    $Id: manager.xml,v 1.3 2004/08/26 17:03:34 remm Exp $

-->


<Context docBase="$INSTALLWEB/www"
         privileged="true" antiResourceLocking="false" antiJARLocking="false">

  <!-- Link to the user database we will get roles from -->
  <ResourceLink name="users" global="UserDatabase"
                type="org.apache.catalina.UserDatabase"/>

</Context>
END_OF_CONFIGURATION

popd

mv -f $INSTALLWEB/tomcat/conf/server.xml $INSTALLWEB/tomcat/conf/server_orig.xml
cp -f server.xml $INSTALLWEB/tomcat/conf

pushd $INSTALLWEB/tomcat/bin
if [ ! -e startup.sh.orig ]; then
mv startup.sh startup.sh.orig
fi

LIB_PATH=$INSTALLWEB/lib:$INSTALLDIR/lib
cat > startup.sh <<END_OF_CONFIGURATION
#!/bin/bash
export LD_LIBRARY_PATH=$LIB_PATH
export MENTOR_DICTIONARY_PATH=$INSTALLDIR/share/gis/coordsys
export JAVA_OPTS="-Djava.library.path=$LIB_PATH"
END_OF_CONFIGURATION

cat startup.sh.orig >> startup.sh
chmod 755 startup.sh
popd

echo "Tomcat configuration completed"
fi


#**********************************************************
# Configuration file updates for Php
# Notes: none
#**********************************************************

# Required modifications to php.ini
# Uses php.ini-mapguide as a template and replaces the ext path
# with the appropriate installdir path
# Note:  Custom built PHP will look in php/lib for ini file

echo "Php configuration started"

BACKINSTALL=${INSTALLWEB//\//\\\/}
cat php.ini-mapguide | sed "s/INSTALLDIR/$BACKINSTALL\/lib/g" > $INSTALLWEB/php/lib/php.ini

echo "Php configuration completed"

echo "Adding test pages"

if [ ! -d "$INSTALLWEB/www" ]; then
	mkdir $INSTALLWEB/www
fi
cat >> $INSTALLWEB/www/index.html <<ENDOFINDEX
<html>
<body>
<p>If you are seeing this page then Apache Httpd has been successfully
configured and installed for MapGuide.
</body>
</html>
ENDOFINDEX

cat >> $INSTALLWEB/www/phpinfo.php <<ENDOFINFO
<?php
phpinfo();
?>
ENDOFINFO

echo "Installation complete."

if [ "$START_HTTPD" = "1" ]; then
#**********************************************************
# Start Apache
# Notes: none
#**********************************************************
echo "Bringing Apache online..."
pushd $INSTALLWEB/apache2/bin
./apachectl start
popd
echo "Apache is now online."
fi

#**********************************************************
# Tomcat startup procedure
# Notes: none
#**********************************************************
if [ "$TOMCAT" = "1" -a "$START_TOMCAT" = "1" ]; then
echo "Tomcat startup"
pushd $INSTALLWEB/tomcat/bin
./startup.sh
popd

echo "Tomcat startup completed"
fi
