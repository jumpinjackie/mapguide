#!/bin/bash

#
# run_tests.sh
#
# Simple wrapper script to execute all applicable tests on Linux
#
# You can run this script after building the main MapGuide source (make && make install)
#

# If set to 1, only a subset of the MapGuide Server test suite is run. In this mode,
# the following test suites are skipped: CoordinateSystem, ResourceService, TileService
SERVER_FAST_SUBSET=0
# The location where MapGuide is installed
MG_INST=/usr/local/mapguideopensource
PHP_CMD=$MG_INST/webserverextensions/php/bin/php
# If set to 1, will try to use a locally-built mgserver (for the server tests) and 
# built-in PHP web server hosted mapagent for running the PHP and Java test suites. 
# Otherwise, it will use the installed Apache and MapGuide Server in the above 
# installation path
USE_LOCAL_SERVER=0
# Set to 1 to run the server test suite
TEST_SERVER=0
# Set to 1 to run the PHP test suite
TEST_PHP=0
# Set to 1 to run the Java test suite
TEST_JAVA=0
# The directory where various test/status log files will be written
LOG_PATH=~
TEST_COMPONENT=
MG_SERVER_PID=
MG_SERVER_WAIT=30
WEB_SERVER_PID=
WEB_SERVER_ADDR=localhost
WEB_SERVER_PORT=8008
WEB_SERVER_WAIT=15

while [ $# -gt 0 ]; do # Until you run out of parameters...
	case "$1" in
		--prefix)
			MG_INST="$2"
			PHP_CMD=$MG_INST/webserverextensions/php/bin/php
			shift
			;;
		--logpath)
			LOG_PATH="$2"
			shift
			;;
		--with-java)
			TEST_JAVA=1
			;;
		--with-php)
			TEST_PHP=1
			;;
		--with-server)
			TEST_SERVER=1
			;;
		--with-server-fast-subset)
			TEST_SERVER=1
			SERVER_FAST_SUBSET=1
			;;
		--with-local-server)
			USE_LOCAL_SERVER=1
			;;
		--local-mgserver-wait)
			MG_SERVER_WAIT=$2
			shift
			;;
		--local-webserver-wait)
			WEB_SERVER_WAIT=$2
			shift
			;;
		--local-webserver-addr)
			WEB_SERVER_ADDR="$2"
			shift
			;;
		--local-webserver-port)
			WEB_SERVER_PORT=$2
			shift
			;;
		--help)
			echo "Usage: $0 (options)"
			echo "Options:"
			echo "  --prefix <value> [MapGuide install location]"
			echo "  --logpath <value> [Directory where logs will be written]"
			echo "  --with-java [Run Java Test Suite]"
			echo "  --with-php [Run PHP Test Suite]"
			echo "  --with-server [Run Server Test Suite]"
			echo "  --with-server-fast-subset [Run fast subset of Server Test Suite]"
			echo "  --with-local-server [Use local mgserver/PHP web server for tests]"
			echo "  --local-mgserver-wait <value> [Wait <value> seconds for local mgserver to start. No effect if not using local server]"
			echo "  --local-webserver-wait <value> [Wait <value> seconds for local webserver to start. No effect if not using local server]"
			echo "  --local-webserver-addr <value> [Start PHP web server on <value>. No effect if not using local server]"
			echo "  --local-webserver-port <value> [Start PHP web server to listen on port <value>. No effect if not using local server]"
			echo "  --help [Display this notice]"
			exit
			;;
	esac
	shift # Check next set of parameters.
done

echo "*********************************************"
echo "Variable Summary"
echo "*********************************************"
echo "MG_INST: $MG_INST"
echo "PHP_CMD: $PHP_CMD"
echo "SERVER_FAST_SUBSET: $SERVER_FAST_SUBSET"
echo "USE_LOCAL_SERVER: $USE_LOCAL_SERVER"
echo "TEST_SERVER: $TEST_SERVER"
echo "TEST_PHP: $TEST_PHP"
echo "TEST_JAVA: $TEST_JAVA"
echo "LOG_PATH: $LOG_PATH"
echo "MG_SERVER_WAIT: $MG_SERVER_WAIT"
echo "WEB_SERVER_ADDR: $WEB_SERVER_ADDR"
echo "WEB_SERVER_PORT: $WEB_SERVER_PORT"
echo "WEB_SERVER_WAIT: $WEB_SERVER_WAIT"

check_test()
{
	error=$?
	if [ $error -ne 0 ]; then
		echo "[warning]: ${TEST_COMPONENT} - Test returned non-zero result ($error)"
		echo "[warning]: ${TEST_COMPONENT} - Test returned non-zero result ($error)" >> $LOG_PATH/unit_test_status.log
	else
		echo "[test]: ${TEST_COMPONENT} - Test returned exit code ($error)"	
	fi
}

start_php_web_server()
{
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		echo "[web server]: Starting (with ${WEB_SERVER_WAIT}s wait)"
		$PHP_CMD -d upload_max_filesize=20M -S "$WEB_SERVER_ADDR:$WEB_SERVER_PORT" -t UnitTest/WebTier/MapAgent/MapAgentForms UnitTest/WebTier/Php/MapAgentShim/index.php > $LOG_PATH/php_web_server.log 2>&1 &
		WEB_SERVER_PID=$!
		# Give some time for the mgserver to actually fully initialize
		sleep $WEB_SERVER_WAIT
		echo "[web server]: Started ($WEB_SERVER_PID)"
	else
		pushd $MG_INST/webserverextensions/apache2/bin > /dev/null
			echo "[web server]: Starting Apache"
			./apachectl restart
			echo "[web server]: Started"
		popd > /dev/null
	fi
}

stop_php_web_server()
{
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		echo "[web server]: Stopping"
		kill $WEB_SERVER_PID
		echo "[web server]: Stopped ($WEB_SERVER_PID)"
		WEB_SERVER_PID=
	else
		pushd $MG_INST/webserverextensions/apache2/bin > /dev/null
			echo "[web server]: Stopping Apache"
			./apachectl stop
			echo "[web server]: Stopped"
		popd > /dev/null
	fi
}

start_mapguide_server()
{
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		pushd Server/src/Core > /dev/null
			echo "[mgserver]: Starting (with ${MG_SERVER_WAIT}s wait)"
			./mgserver run > $LOG_PATH/mapguide_server_run.log 2>&1 &
			MG_SERVER_PID=$!
			# Give some time for the mgserver to actually fully initialize
			sleep $MG_SERVER_WAIT
			echo "[mgserver]: Started ($MG_SERVER_PID)"
		popd > /dev/null
	else
		pushd $MG_INST/server/bin > /dev/null
			echo "[mgserver]: Starting mgserver"
			./mgserverd.sh
			echo "[mgserver]: Started"
		popd
	fi
}

stop_mapguide_server()
{
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		echo "[mgserver]: Stopping"
		kill $MG_SERVER_PID
		echo "[mgserver]: Stopped ($MG_SERVER_PID)"
		MG_SERVER_PID=
	else
		echo "[mgserver]: Stopping"
		pkill -f -u root "mgserer daemon"
		echo "[mgserver]: Stopped"
	fi
}

# Build server unit test suite (regardless of whether we're running
# server tests or not)

echo "[build]: Sheboygan sample data set"
pushd UnitTest/TestData/Samples/Sheboygan > /dev/null
./build.sh 2>&1 > $LOG_PATH/make_package.log
popd > /dev/null

echo "[build]: Server unit tests"
pushd Server/src/UnitTesting > /dev/null
make libMgUnitTesting.la 2>&1 > /dev/null
popd > /dev/null

pushd Server/src/Core > /dev/null
make unittest.sh 2>&1 > /dev/null
popd > /dev/null

# Clear existing status log file
rm -f $LOG_PATH/unit_test_status.log

pushd Server/src/Core > /dev/null
if [ $TEST_SERVER -eq 1 ]; then
	if [ $SERVER_FAST_SUBSET -eq 1 ]; then
		for comp in FeatureService Geometry KmlService LogManager MappingService MdfModel Misc Performance RenderingService ServerAdminService ServerManager ServiceManager SiteManager SiteService ProfilingService TransformMesh
		do
			TEST_COMPONENT="MapGuide Server Test Suite ($comp)"
			echo "[test]: $TEST_COMPONENT"
			./mgserver test $comp $LOG_PATH/UnitTestResult_${comp}.xml 2>&1 | tee $LOG_PATH/mapguide_${comp}_unit_test.log
			check_test
		done
	else
		TEST_COMPONENT="MapGuide Server Test Suite (all)"
		echo "[test]: $TEST_COMPONENT"
		./mgserver test 2>&1 | tee $LOG_PATH/mapguide_test.log
		check_test
	fi
else
	echo "[test]: Skipping Server Test Suite"
fi
popd > /dev/null

if [ $TEST_PHP -eq 1 ]; then
	#clear out old dbs
	pushd UnitTest/TestData > /dev/null
		rm -rf Unicode/UnicodeTest.db
		rm -rf WmsTest/WmsTest.db
		rm -rf WebLayout/WebLayoutTest.db
		rm -rf WfsTest/WfsTest.db
		rm -rf MapLayer/MapLayerTest.db
		rm -rf ServerAdmin/ServerAdminTest.db
		rm -rf MappingService/MappingServiceTest.db
		rm -rf SiteService/SiteServiceTest.db
		rm -rf FeatureService/FeatureServiceTest.db
		rm -rf DrawingService/DrawingServiceTest.db
		rm -rf ResourceService/ResourceServiceTest.db
	popd > /dev/null
	start_mapguide_server
	start_php_web_server
	pushd UnitTest > /dev/null
		TEST_COMPONENT="Prepare PHP test suite"
		echo "[test]: $TEST_COMPONENT"
		$PHP_CMD prepare.php 2>&1 | tee $LOG_PATH/prepare.log
		check_test
	popd > /dev/null
	pushd UnitTest/WebTier/Php > /dev/null
		TEST_COMPONENT="PHP Web Tier Tests"
		echo "[test]: $TEST_COMPONENT"
		# Inject these vars into $_SERVER in PHP CLI
		export SERVER_ADDR=$WEB_SERVER_ADDR
		export SERVER_PORT=$WEB_SERVER_PORT
		export WEBCONFIGINI=$PWD/../../webconfig.ini
		$PHP_CMD -d error_reporting=E_ALL^E_DEPRECATED RunTests.php 2>&1 | tee $LOG_PATH/php_test.log
		check_test
	popd > /dev/null
	stop_mapguide_server
	stop_php_web_server
fi

if [ $TEST_JAVA -eq 1 ]; then
	#clear out old dbs
	pushd UnitTest/TestData > /dev/null
		rm -rf Unicode/UnicodeTest.db
		rm -rf WmsTest/WmsTest.db
		rm -rf WebLayout/WebLayoutTest.db
		rm -rf WfsTest/WfsTest.db
		rm -rf MapLayer/MapLayerTest.db
		rm -rf ServerAdmin/ServerAdminTest.db
		rm -rf MappingService/MappingServiceTest.db
		rm -rf SiteService/SiteServiceTest.db
		rm -rf FeatureService/FeatureServiceTest.db
		rm -rf DrawingService/DrawingServiceTest.db
		rm -rf ResourceService/ResourceServiceTest.db
	popd > /dev/null
	start_mapguide_server
	pushd UnitTest/WebTier/Java > /dev/null
		TEST_COMPONENT="Java Web Tier Tests"
		echo "[test]: $TEST_COMPONENT"
		ant checkunix -l $LOG_PATH/java_test_output.log
		check_test
	popd > /dev/null
	stop_mapguide_server
fi

if [ -f "$LOG_PATH/unit_test_status.log" ]; then
	echo "[warning]: One or more tests did not fully pass. Overall status results shown below:"
	cat $LOG_PATH/unit_test_status.log
	echo "[warning]: Check $LOG_PATH for additional logs and test results"
else
	echo "[test]: No overall warnings logged. Tests look OK. Check logged files in $LOG_PATH for more information"
fi
