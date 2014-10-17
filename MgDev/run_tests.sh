#!/bin/bash

#
# run_tests.sh
#
# Simple wrapper script to execute all applicable tests on Linux
#
SERVER_FAST_SUBSET=1
MG_INST=/usr/local/mapguideopensource-3.0.0
PHP_CMD=$MG_INST/webserverextensions/php/bin/php
USE_LOCAL_SERVER=1
TEST_SERVER=1
TEST_PHP=1
TEST_JAVA=1
LOG_PATH=~
TEST_COMPONENT=
MG_SERVER_PID=
MG_SERVER_WAIT=30
WEB_SERVER_PID=
WEB_SERVER_ADDR=localhost
WEB_SERVER_PORT=8018
WEB_SERVER_WAIT=15

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
	echo "[web server]: Starting (with ${WEB_SERVER_WAIT}s wait)"
	$PHP_CMD -d upload_max_filesize=20M -S "$WEB_SERVER_ADDR:$WEB_SERVER_PORT" -t UnitTest/WebTier/MapAgent/MapAgentForms UnitTest/WebTier/Php/MapAgentShim/index.php > $LOG_PATH/php_web_server.log 2>&1 &
	WEB_SERVER_PID=$!
	# Give some time for the mgserver to actually fully initialize
	sleep $WEB_SERVER_WAIT
	echo "[web server]: Started ($WEB_SERVER_PID)"
}

stop_php_web_server()
{
	echo "[web server]: Stopping"
	kill $WEB_SERVER_PID
	echo "[web server]: Stopped ($WEB_SERVER_PID)"
	WEB_SERVER_PID=
}

start_mapguide_server()
{
	pushd Server/src/Core > /dev/null
		echo "[mgserver]: Starting (with ${MG_SERVER_WAIT}s wait)"
		./mgserver run > $LOG_PATH/mapguide_server_run.log 2>&1 &
		MG_SERVER_PID=$!
		# Give some time for the mgserver to actually fully initialize
		sleep $MG_SERVER_WAIT
		echo "[mgserver]: Started ($MG_SERVER_PID)"
	popd > /dev/null
}

stop_mapguide_server()
{
	echo "[mgserver]: Stopping"
	kill $MG_SERVER_PID
	echo "[mgserver]: Stopped ($MG_SERVER_PID)"
	MG_SERVER_PID=
}

# Build server unit test suite (regardless of whether we're running
# server tests or not)

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
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		start_mapguide_server
		start_php_web_server
	fi
	pushd UnitTest > /dev/null
		TEST_COMPONENT="Prepare PHP test suite"
		echo "[test]: $TEST_COMPONENT"
		$PHP_CMD prepare.php
		check_test
	popd > /dev/null
	pushd UnitTest/WebTier/Php > /dev/null
		TEST_COMPONENT="PHP Web Tier Tests"
		echo "[test]: $TEST_COMPONENT"
		# Inject these vars into $_SERVER in PHP CLI
		export SERVER_ADDR=$WEB_SERVER_ADDR
		export SERVER_PORT=$WEB_SERVER_PORT
		export WEBCONFIGINI=$PWD/../../webconfig.ini
		$PHP_CMD RunTests.php 2>&1 | tee $LOG_PATH/php_test.log
		check_test
	popd > /dev/null
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		stop_mapguide_server
		stop_php_web_server
	fi
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
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		start_mapguide_server
	fi
	pushd UnitTest/WebTier/Java > /dev/null
		TEST_COMPONENT="Java Web Tier Tests"
		echo "[test]: $TEST_COMPONENT"
		ant checkunix -l $LOG_PATH/java_test_output.log
		check_test
	popd > /dev/null
	if [ $USE_LOCAL_SERVER -eq 1 ]; then
		stop_mapguide_server
	fi
fi

if [ -f "$LOG_PATH/unit_test_status.log" ]; then
	echo "[warning]: One or more tests did not fully pass. Overall status results shown below:"
	cat $LOG_PATH/unit_test_status.log
	echo "[warning]: Check $LOG_PATH for additional logs and test results"
else
	echo "[test]: No overall warnings logged. Tests look OK. Check logged files in $LOG_PATH for more information"
fi
