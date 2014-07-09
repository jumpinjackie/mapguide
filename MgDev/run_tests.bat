@echo off
SET PLAT=x86
SET CONFIG=Release
SET CONF=Release
SET TEST_COMPONENT=
IF NOT "%1"=="" SET PLAT=%1
IF "%PLAT%"=="x64" SET CONF=Release64
REM Time to wait for mgserver.exe to startup in seconds
SET MGSERVER_WAIT=15
REM Vars to inject into $_SERVER in PHP CLI
SET SERVER_ADDR=localhost
SET WEBCONFIGINI=%CD%\UnitTest\webconfig.ini
SET SERVER_PORT=8018
REM SET WEBCONFIGINI=C:\Program Files\OSGeo\MapGuide\Web\www\webconfig.ini
REM SET SERVER_PORT=80
SET PHP_TEST_CWD=%CD%\Web\src\mapagent
SET PHP_EXT_DIR=C:\Program Files\OSGeo\MapGuide\Web\Php\ext

SET START_MGSERVER=1
SET START_WEBSERVER=0
SET RUN_SERVER_TESTS=0
SET RUN_PHP_TESTS=0
SET RUN_DOTNET_TESTS=1

SET RETURN_CODE=0

echo *************** TEST SUMMARY ******************
echo Platform: %PLAT%
echo Configuration: %CONF%
echo ***********************************************
:test_server
if "%RUN_SERVER_TESTS%" == "1" (
    echo [test]: MapGuide Server
    SET TEST_COMPONENT=MapGuide Server Tests
    pushd Server\bin\%CONF%
    rem Before we run, nuke the Repositories/ directory to ensure a clean slate
    rd /S /Q Repositories
    mgserver.exe test
    if %ERRORLEVEL% neq 0 echo [test]: MapGuide Server tests had one or more failures. Check UnitTestResults.xml for more information
    rem Nuke again for future tests
    rd /S /Q Repositories
    popd
)
:build_tests
if "%RUN_DOTNET_TESTS%" == "1" (
    echo [build]: DotNet test runner
    SET TEST_COMPONENT=Build DotNet test runner
    pushd UnitTest\WebTier\DotNet
    msbuild /p:Configuration=%CONFIG%;Platform=%PLAT% /fl /flp:logfile=build.log DotNet.sln
    if "%ERRORLEVEL%" == "1" (
        set RETURN_CODE=%ERRORLEVEL%
        goto error
    )
    popd
)
:start_mgserver
if "%START_MGSERVER%" == "1" (
    echo [test]: Starting MapGuide Server. Waiting %MGSERVER_WAIT%s
    pushd Server\bin\%CONF%
    rem Before we run, nuke the Repositories/ directory to ensure a clean slate
    rd /S /Q Repositories
    start mgserver run
    REM An elegant hack to pause this script allowing mgserver to start up
    echo Waiting %MGSERVER_WAIT%s for mgserver.exe to start up
    ping -n %MGSERVER_WAIT% 127.0.0.1 > NUL
    popd
)
:prepare_webconfig
pushd UnitTest
SET TEST_COMPONENT=Prepare webtier test suites
php -n -d display_errors=Off -d extension_dir="%PHP_EXT_DIR%" -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll -d extension=php_SQLitePhpApi.dll prepare.php
if %ERRORLEVEL% neq 0 (
    set RETURN_CODE=%ERRORLEVEL%
    goto error
)
popd
:start_php_webserver
if "%START_WEBSERVER%" == "1" (
    echo [test]: Starting PHP web server. Waiting %MGSERVER_WAIT%s
    start php -n -d display_errors=Off -d upload_max_filesize=20M -d extension_dir="%PHP_EXT_DIR%" -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll -S %SERVER_ADDR%:%SERVER_PORT% -t %CD%\UnitTest\WebTier\MapAgent\MapAgentForms %CD%\UnitTest\WebTier\Php\MapAgentShim\index.php
    ping -n %MGSERVER_WAIT% 127.0.0.1 > NUL
)
:test_php
if "%RUN_PHP_TESTS%" == "1" (
    echo [test]: PHP tests
    REM Clear out old dbs before running
    pushd UnitTest\TestData
    if exist Unicode\UnicodeTest.db del /F Unicode\UnicodeTest.db
    if exist WmsTest\WmsTest.db del /F WmsTest\WmsTest.db
    if exist WebLayout\WebLayoutTest.db del /F WebLayout\WebLayoutTest.db
    if exist WfsTest\WfsTest.db del /F WfsTest\WfsTest.db
    if exist MapLayer\MapLayerTest.db del /F MapLayer\MapLayerTest.db
    if exist ServerAdmin\ServerAdminTest.db del /F ServerAdmin\ServerAdminTest.db
    if exist MappingService\MappingServiceTest.db del /F MappingService\MappingServiceTest.db
    if exist SiteService\SiteServiceTest.db del /F SiteService\SiteServiceTest.db
    if exist FeatureService\FeatureServiceTest.db del /F FeatureService\FeatureServiceTest.db
    if exist DrawingService\DrawingServiceTest.db del /F DrawingService\DrawingServiceTest.db
    if exist ResourceService\ResourceServiceTest.db del /F ResourceService\ResourceServiceTest.db
    popd
    pushd UnitTest\WebTier\Php
    php.exe -n -d display_errors=Off -d extension_dir="%PHP_EXT_DIR%" -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll -d extension=php_pdo_sqlite.dll RunTests.php
    popd
)
:test_dotnet
if "%RUN_DOTNET_TESTS%" == "1" (
    echo [test]: .net tests
    REM Clear out old dbs before running
    pushd UnitTest\TestData
    if exist Unicode\UnicodeTest.db del /F Unicode\UnicodeTest.db
    if exist WmsTest\WmsTest.db del /F WmsTest\WmsTest.db
    if exist WebLayout\WebLayoutTest.db del /F WebLayout\WebLayoutTest.db
    if exist WfsTest\WfsTest.db del /F WfsTest\WfsTest.db
    if exist MapLayer\MapLayerTest.db del /F MapLayer\MapLayerTest.db
    if exist ServerAdmin\ServerAdminTest.db del /F ServerAdmin\ServerAdminTest.db
    if exist MappingService\MappingServiceTest.db del /F MappingService\MappingServiceTest.db
    if exist SiteService\SiteServiceTest.db del /F SiteService\SiteServiceTest.db
    if exist FeatureService\FeatureServiceTest.db del /F FeatureService\FeatureServiceTest.db
    if exist DrawingService\DrawingServiceTest.db del /F DrawingService\DrawingServiceTest.db
    if exist ResourceService\ResourceServiceTest.db del /F ResourceService\ResourceServiceTest.db
    popd
    pushd UnitTest\WebTier\DotNet_%PLAT%
    MgTestRunner.exe "%WEBCONFIGINI%" "../../../Oem/CsMap/Dictionaries"
    if %ERRORLEVEL% neq 0 echo [test]: .net test runner had one or more test failures. Check log files for more information
    popd
)
:stop_mgserver
if "%START_MGSERVER%" == "1" (
    echo [test]: Terminating mgserver.exe
    REM We're naturally assuming the only mgserver.exe that is running is the one this script started
    taskkill /im mgserver.exe
)
:stop_php_webserver
if "%START_WEBSERVER%" == "1" (
    echo [test]: Terminating php.exe
    REM We're naturally assuming the only php.exe that is running is the one this script started
    taskkill /im php.exe
)
goto done
:error
echo [error]: An error occured with %TEST_COMPONENT% (exit code: %RETURN_CODE%)
goto quit
:done

:quit