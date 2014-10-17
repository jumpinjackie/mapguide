#!/bin/bash

#
# run_tests.sh
#
# Simple wrapper script to execute all applicable tests on Linux
SERVER_FAST_SUBSET=1
MG_INST=/usr/local/mapguideopensource
PHP_CMD=$MG_INST/webserverextensions/php/bin/php
TEST_SERVER=1
TEST_PHP=0
TEST_JAVA=1
LOG_PATH=~
TEST_COMPONENT=
MG_SERVER_PID=

check_test()
{
	error=$?
	if [ $error -ne 0 ]; then
		echo "[warning]: ${TEST_COMPONENT} - Test returned non-zero result ($error)"
		echo "[warning]: ${TEST_COMPONENT} - Test returned non-zero result ($error)" >> $LOG_PATH/unit_test_status.log
	fi
}

start_mapguide_server()
{
	pushd Server/src/Core > /dev/null
		echo "[test]: Start mgserver (with 30s wait)"
		./mgserver run > $LOG_PATH/mapguide_server_run.log 2>&1 &
		MG_SERVER_PID=$!
		# Give some time for the mgserver to actually fully initialize
		sleep 30
		echo "[test]: mgserver started ($MG_SERVER_PID)"
	popd > /dev/null
}

stop_mapguide_server()
{
	echo "[test]: Stop mgserver"
	kill $MG_SERVER_PID
	echo "[test]: mgserver stopped ($MG_SERVER_PID)"
	MG_SERVER_PID=
}

# Build server unit test suite (regardless of whether we're running
# server tests or not)

echo "[build]: Server unit tests"
pushd Server/src/UnitTesting > /dev/null
make libMgUnitTesting.la
popd > /dev/null

pushd Server/src/Core > /dev/null
make unittest.sh
popd > /dev/null

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
	pushd UnitTest\WebTier\Php > /dev/null
		TEST_COMPONENT="PHP Web Tier Tests"
		echo "[test]: $TEST_COMPONENT"
		$PHP_CMD RunTests.php
		check_test
	popd > /dev/null
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
		ant checkunix 2>&1 | tee $LOG_PATH/java_test_output.log
		check_test
	popd > /dev/null
	stop_mapguide_server
fi
