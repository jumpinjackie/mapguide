@echo off
REM 
REM run_tests.bat
REM 
REM This batch file helps to execute the MapGuide test suite against a given
REM installation of MapGuide
REM
SET PLATFORM=x86
SET PHP_WEB_SERVER=0
SET SERVER_ADDR=localhost
SET SERVER_PORT=8008
SET TEST_SUITE=server
SET WEB_ROOT=%CD%\Release\Web\www
SET PHP_EXT_DIR=%CD%\Release\Web\Php\ext
SET SERVER_PATH=%CD%\Server\bin
SET CS_MAP_PATH=%WEB_ROOT%\..\..\CS-Map\Dictionaries
SET MAPAGENT=http://localhost:8008/mapguide/mapagent/mapagent.fcgi
SET CURRENT_DIR=%CD%
SET OLDPATH=%PATH%
SET SERVER_TEST_SCOPE=all
SET TEST_DATA_SRC=%CD%\UnitTest\TestData
SET MGSERVER_WAIT=15

SET PATH=C:\Windows\Microsoft.NET\Framework\v4.0.30319;%PATH%

:study_params
if (%1)==()            goto pre_flight_check

if "%1"=="-help"       goto help_show
if "%1"=="-h"          goto help_show

if "%1"=="-iw"         goto get_phpwebserver

if "%1"=="-ws"         goto get_webservername

if "%1"=="-wp"         goto get_webserverport

if "%1"=="-sts"        goto get_servertestscope

if "%1"=="-p"          goto get_platform
if "%1"=="-platform"   goto get_platform

if "%1"=="-t"          goto get_test
if "%1"=="-test"       goto get_test

if "%1"=="-m"          goto get_mapagent
if "%1"=="-mapagent"   goto get_mapagent

if "%1"=="-w"          goto get_webroot
if "%1"=="-webroot"    goto get_webroot

if "%1"=="-s"          goto get_serverpath
if "%1"=="-serverpath" goto get_serverpath

goto custom_error

:next_param
shift
shift
goto study_params

:get_platform
SET PLATFORM=%2
if "%2" == "x86" goto next_param
if "%2" == "x64" goto next_param
SET ERRORMSG=Unrecognised value: %2
goto custom_error

:get_phpwebserver
SET PHP_WEB_SERVER=%2
if "%2" == "1" goto next_param
if "%2" == "0" goto next_param
SET ERRORMSG=Unrecognised value: %2
goto custom_error

:get_webservername
SET SERVER_ADDR=%2
SET SERVER_ADDR=%SERVER_ADDR:"=%
goto next_param

:get_webserverport
SET SERVER_PORT=%2
goto next_param

:get_servertestscope
SET SERVER_TEST_SCOPE=%2
goto next_param

:get_test
SET TEST_SUITE=%2
if "%2"=="server" goto next_param
if "%2"=="php" goto next_param
if "%2"=="dotnet" goto next_param
if "%2"=="java" goto next_param
if "%2"=="all" goto next_param
SET ERRORMSG=Unrecognised action: %2
goto custom_error

:get_mapagent
SET MAPAGENT=%2
SET MAPAGENT=%MAPAGENT:"=%
goto next_param

:get_webroot
SET WEB_ROOT=%2
SET WEB_ROOT=%WEB_ROOT:"=%
SET CS_MAP_PATH=%WEB_ROOT%\..\..\CS-Map\Dictionaries
SET PHP_EXT_DIR=%WEB_ROOT%\..\Php\ext
goto next_param

:get_serverpath
SET SERVER_PATH=%2
SET SERVER_PATH=%SERVER_PATH:"=%
goto next_param

:pre_flight_check
echo ******** Test Run Summary **********************
echo Platform:           %PLATFORM%
echo Use PHP Web Server: %PHP_WEB_SERVER%
echo Test Suite to run:  %TEST_SUITE%
echo Map Agent URL:      %MAPAGENT%
echo Server Path:        %SERVER_PATH%
echo Web Root Path:      %WEB_ROOT%
echo CS-Map Path:        %CS_MAP_PATH%
echo Web Server Name:    %SERVER_ADDR%
echo Web Server Port:    %SERVER_PORT%
echo Server Test Scope:  %SERVER_TEST_SCOPE%
echo ********* Paths to check ***********************
echo PHP executable      %WEB_ROOT%\..\Php\php.exe
echo .net Assemblies     %WEB_ROOT%\mapviewernet\bin
echo Java jars           %WEB_ROOT%\WEB-INF\lib
echo ************************************************
SET WEBCONFIGINI=%WEB_ROOT%\webconfig.ini
SET PATH=%WEB_ROOT%\..\Php;%PATH%
if "%PHP_WEB_SERVER%" == "1" (
    echo [prepare]: Starting PHP web server. Waiting %MGSERVER_WAIT%s
    start php -n -d display_errors=Off -d upload_max_filesize=20M -d extension_dir="%PHP_EXT_DIR%" -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll -S %SERVER_ADDR%:%SERVER_PORT% -t "%CURRENT_DIR%\UnitTest\WebTier\MapAgent\MapAgentForms" "%CURRENT_DIR%\UnitTest\WebTier\Php\MapAgentShim\index.php"
    ping -n %MGSERVER_WAIT% 127.0.0.1 > NUL
)
:check_dotnet
echo [check]: .net
if not exist "%WEB_ROOT%\mapviewernet\bin\*.dll" goto no_dotnet
goto check_java
:no_dotnet
if "%TEST_SUITE%" == "dotnet" (
    SET ERRORMSG=Could not find .net MapGuide assemblies and unmanaged dlls in this MapGuide installation
    goto error_check
)
if "%TEST_SUITE%" == "all" (
    SET ERRORMSG=Could not find .net MapGuide assemblies and unmanaged dlls in this MapGuide installation
    goto error_check
)
:check_java
echo [check]: Java
if not exist "%WEB_ROOT%\WEB-INF\lib\*.jar" goto no_java
goto check_php
:no_java
if "%TEST_SUITE%" == "java" (
    SET ERRORMSG=Could not find required jars and unmanaged dlls in this MapGuide installation
    goto error_check
)
if "%TEST_SUITE%" == "all" (
    SET ERRORMSG=Could not find required jars and unmanaged dlls in this MapGuide installation
    goto error_check
)
:check_php
echo [check]: PHP
if not exist "%WEB_ROOT%\..\Php\php.exe" goto no_php
goto check_server
:no_php
if "%TEST_SUITE%" == "php" (
    SET ERRORMSG=Could not find required PHP executable in this MapGuide installation
    goto error_check
)
if "%TEST_SUITE%" == "all" (
    SET ERRORMSG=Could not find required PHP executable in this MapGuide installation
    goto error_check
)
:check_server
if "%TEST_SUITE%" == "server" goto prepare_server_files
if "%TEST_SUITE%" == "all" goto prepare_server_files
if "%TEST_SUITE%" == "php" goto test_php
if "%TEST_SUITE%" == "dotnet" goto test_dotnet
if "%TEST_SUITE%" == "java" goto test_java
goto test_php
:prepare_server_files
echo [prepare]: Test Data Files for Server Test Suite
if not exist "%SERVER_PATH%\..\UnitTestFiles" mkdir "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\CoordinateSystems\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\DrawingService\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\FeatureService\SDF\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\FeatureService\SHP\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\FeatureService\SQLite\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\KmlService\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\MapLayer\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\MappingService\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\MdfModel\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\PrintLayout\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\ResourceService\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\ResourceService\Shuttle.zip" "%SERVER_PATH%\..\UnitTestFiles\World.mgp"
copy /Y "%TEST_DATA_SRC%\ServerAdmin\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\SiteService\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\Symbology\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\TileService\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\Unicode\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\WebLayout\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\Wfs\*.*" "%SERVER_PATH%\..\UnitTestFiles"
copy /Y "%TEST_DATA_SRC%\Wms\*.*" "%SERVER_PATH%\..\UnitTestFiles"
:test_server
echo [test]: MapGuide Server
pushd "%SERVER_PATH%"
mgserver test %SERVER_TEST_SCOPE% UnitTestResults.xml
move /Y UnitTestResults.xml "%CURRENT_DIR%"
popd
if "%TEST_SUITE%" == "server" goto quit
if "%TEST_SUITE%" == "all" goto test_php
:test_php
echo [test]: PHP Binding
pushd "%CURRENT_DIR%\UnitTest\TestData"
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
if exist "%SERVER_PATH%" (
    REM start mgserver for test
    pushd "%SERVER_PATH%"
    start mgserver run
    REM An elegant hack to pause this script allowing mgserver to start up
    echo Waiting %MGSERVER_WAIT%s for mgserver.exe to start up
    ping -n %MGSERVER_WAIT% 127.0.0.1 > NUL
    popd
)
REM Ensure constants.php exists
if not exist "%CURRENT_DIR%\Web\src\PhpApi" mkdir "%CURRENT_DIR%\Web\src\PhpApi"
copy /Y "%WEB_ROOT%\mapadmin\constants.php" "%CURRENT_DIR%\Web\src\PhpApi"
REM Ensure we have the Sheboygan.mgp loaded first
pushd "%CURRENT_DIR%\UnitTest"
if exist "%WEB_ROOT%\webconfig.ini" SET MG_WEBCONFIG_INI=%WEB_ROOT%\webconfig.ini
php -n -d display_errors=Off -d extension_dir="%PHP_EXT_DIR%" -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll prepare.php
popd
REM Now run the test suite
pushd "%CURRENT_DIR%\UnitTest\WebTier\Php"
php.exe -n -d display_errors=On -d extension_dir="%PHP_EXT_DIR%" -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll -d extension=php_pdo_sqlite.dll RunTests.php -config "%WEB_ROOT%\webconfig.ini"
move /Y UnitTests.log "%CURRENT_DIR%\UnitTests_Php.log"
popd
if exist "%SERVER_PATH%" (
    echo [test]: Terminating mgserver.exe
    REM We're naturally assuming the only mgserver.exe that is running is the one this script started
    taskkill /f /im mgserver.exe
)
if "%TEST_SUITE%" == "php" goto quit
:test_dotnet
echo [test]: .net Binding
echo [build]: DotNet test runner
SET TEST_COMPONENT=Build DotNet test runner
if exist "%CURRENT_DIR%\UnitTest\WebTier\DotNet_x64" rd /S /Q "%CURRENT_DIR%\UnitTest\WebTier\DotNet_x64"
if exist "%CURRENT_DIR%\UnitTest\WebTier\DotNet_x86" rd /S /Q "%CURRENT_DIR%\UnitTest\WebTier\DotNet_x86"
pushd "%CURRENT_DIR%\UnitTest\WebTier\DotNet"
if exist Libs rd /S /Q Libs
if not exist "%CURRENT_DIR%\Web\bin\release" mkdir "%CURRENT_DIR%\Web\bin\release"
if not exist "%CURRENT_DIR%\Web\bin\release64" mkdir "%CURRENT_DIR%\Web\bin\release64"
if "%PLATFORM%" == "x86" copy /y "%WEB_ROOT%\mapviewernet\bin\*.dll" "%CURRENT_DIR%\Web\bin\release"
if "%PLATFORM%" == "x64" copy /y "%WEB_ROOT%\mapviewernet\bin\*.dll" "%CURRENT_DIR%\Web\bin\release64"
if not exist "%CURRENT_DIR%\Common\MapGuideCommon\Resources" mkdir "%CURRENT_DIR%\Common\MapGuideCommon\Resources"
copy /y "%WEB_ROOT%\mapagent\Resources\mapguide_en.res" "%CURRENT_DIR%\Common\MapGuideCommon\Resources"
msbuild /p:Configuration=Release;Platform=%PLATFORM% /fl /flp:logfile=build.log DotNet.sln
if "%ERRORLEVEL%" == "1" (
    set RETURN_CODE=%ERRORLEVEL%
    goto error_msbuild
)
popd
pushd "%CURRENT_DIR%\UnitTest\TestData"
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
if exist "%SERVER_PATH%" (
    REM start mgserver for test
    pushd "%SERVER_PATH%"
    start mgserver run
    REM An elegant hack to pause this script allowing mgserver to start up
    echo Waiting %MGSERVER_WAIT%s for mgserver.exe to start up
    ping -n %MGSERVER_WAIT% 127.0.0.1 > NUL
    popd
)
pushd "%CURRENT_DIR%\UnitTest\WebTier\DotNet_%PLATFORM%"
MgTestRunner.exe "%WEB_ROOT%\webconfig.ini" "%CS_MAP_PATH%"
if %ERRORLEVEL% neq 0 echo [test]: .net test runner had one or more test failures. Check log files for more information
move /Y UnitTests.log "%CURRENT_DIR%\UnitTests_DotNet.log"
popd
if exist "%SERVER_PATH%" (
    echo [test]: Terminating mgserver.exe
    REM We're naturally assuming the only mgserver.exe that is running is the one this script started
    taskkill /f /im mgserver.exe
)
if "%TEST_SUITE%" == "dotnet" goto quit
:test_java
echo [test]: Java Binding
pushd "%CURRENT_DIR%\UnitTest\TestData"
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
if exist "%SERVER_PATH%" (
    REM start mgserver for test
    pushd "%SERVER_PATH%"
    start mgserver run
    REM An elegant hack to pause this script allowing mgserver to start up
    echo Waiting %MGSERVER_WAIT%s for mgserver.exe to start up
    ping -n %MGSERVER_WAIT% 127.0.0.1 > NUL
    popd
)
if not exist "%CURRENT_DIR%\Web\src" mkdir "%CURRENT_DIR%\Web\src"
xcopy /E /Y /I /Q /H "%WEB_ROOT%\WEB-INF" "%CURRENT_DIR%\Web\src\WEB-INF"
pushd "%CURRENT_DIR%\UnitTest\WebTier\Java"
call ant checkwin_external -Dmapguide.dictpath="%CS_MAP_PATH%" -Dbinsrc.web="%WEB_ROOT%\..\Tomcat\bin" -Dmapguide.config.src="%WEB_ROOT%\webconfig.ini"
if %ERRORLEVEL% neq 0 echo [test]: Java test runner had one or more test failures. Check log files for more information
popd
pushd "%CURRENT_DIR%\UnitTest\WebTier\java_dist"
move /Y unittest.log "%CURRENT_DIR%\UnitTests_Java.log"
popd
if exist "%SERVER_PATH%" (
    echo [test]: Terminating mgserver.exe
    REM We're naturally assuming the only mgserver.exe that is running is the one this script started
    taskkill /f /im mgserver.exe
)
if "%TEST_SUITE%" == "all" goto quit
if "%TEST_SUITE%" == "java" goto quit

:error_msbuild
echo [error]: An error occured with %TEST_COMPONENT% (exit code: %RETURN_CODE%)
goto quit

:error_check
echo [ERROR]: %ERRORMSG%
SET ERRORMSG=
SET PATH=%OLDPATH%
popd
if "%PHP_WEB_SERVER%" == "1" (
    echo [cleanup]: Terminating PHP web server
    REM We're naturally assuming the only php.exe that is running is the one this script started
    taskkill /f /im php.exe
)
exit /B 1

:custom_error
echo [ERROR]: %ERRORMSG%
SET ERRORMSG=
echo Please use the format:
:help_show
echo ************************************************************************
echo run_tests.bat [-h]
echo               [-p=Platform]
echo               [-t=TestSuite]
echo               [-m=MapAgent]
echo               [-w=WebRoot]
echo               [-s=ServerPath]
echo               [-sts=ServerTestScope]
echo               [-ws=WebServerName]
echo               [-wp=WebServerPort]
echo               [-iw=UsePHPWebServer]
echo
echo UsePHPWebServer: -iw=1 (default),0
echo Platform: -p[latform]=x86 (default)
echo                       x64
echo TestSuite: -t[est]=server (default)
echo                    php
echo                    dotnet
echo                    java
echo                    all
echo MapAgent:  -m[apagent]=your mapagent url
echo WebRoot:   -w[ebroot]=path to your web root www
echo ServerPath: -s[erverpath]=path to your server bin directory
echo WebServerName: -ws=your web server name
echo WebServerPort: -wp=your web server port
echo ************************************************************************
:quit
SET PATH=%OLDPATH%
SET TYPEACTION=
if "%PHP_WEB_SERVER%" == "1" (
    echo [cleanup]: Terminating PHP web server
    REM We're naturally assuming the only php.exe that is running is the one this script started
    taskkill /f /im php.exe
)
popd