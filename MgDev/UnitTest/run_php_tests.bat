@echo off
REM Web Tier Test Suite runner script
REM
REM Author: Jackie Ng (jumpinjackie@gmail.com)
REM
REM About this script:
REM
REM This batch script will automatically start the mgserver.exe with a customizable wait period.
REM Once the wait period has elapsed the Web Tier test suite will be run via the PHP CLI interpreter.
REM
REM This script harnesses everything that's available from the MapGuide source tree (under MgDev) to avoid
REM minimal external setup. 
REM
REM The only setup required is to copy a 32-bit ssleay32.dll and libeay32.dll to Oem\php\Release_TS.
REM If you have the Installer checked out, you can extract a copy of these files from the
REM binary 32-bit PHP zip under Installer\Support\Web\x86
REM
REM By default, http unit tests are skipped because of this. See remarks.
REM
REM Instructions:
REM 
REM  1. Build the Oem, Server and WebTier in release mode
REM  2. Run set_test_params.bat. You can change the values in this file beforehand.
REM  3. Run run_php_tests.bat
REM
REM Remarks:
REM
REM  1. mapagent http tests are not run by default as this requires one to be already set up under 
REM     (http://localhost:<port>/mapguide/mapagent/mapagent.fcgi). If you have already set one up
REM     you can set API_ONLY=0 in set_test_params.bat
REM
REM TODO:
REM  We really need to automate setting up the mapagent in a web server in a zero-configuration manner. Only then can we 
REM  remove the API_ONLY variable
REM
SET WEB_BIN=%CD%\..\Web\bin\release
SET SERVER_BIN=%CD%\..\Server\bin\release
SET OEM_PATH=%CD%\..\Oem
SET OEM_PHP=%OEM_PATH%\php\Release_TS
REM MENTOR_DICTIONARY_PATH fallback for MgCoordinateSystem still exists, so use it so we don't have to mess around with webconfig.ini
SET MENTOR_DICTIONARY_PATH=%OEM_PATH%\CsMap\Dictionaries
SET OEM_SQLITE=%OEM_PATH%\SQLite\bin\release
SET MAPAGENT=%CD%\..\Web\src\mapagent
IF "%PLATFORM%" == "64" (
    SET OEM_PHP=%OEM_PATH%\php\x64\Release_TS
    SET WEB_BIN=%CD%\..\Web\bin\release64
    SET SERVER_BIN=%CD%\..\Server\bin\release64
    SET OEM_SQLITE=%OEM_PATH%\SQLite\bin\release64
    SET MAPAGENT=%CD%\..\Web\src\mapagent64
)
SET OLD_PATH=%PATH%
SET PATH=%PATH%;%OEM_PHP%;%MAPAGENT%

echo ******************************************************************************
echo Platform: %PLATFORM%
echo Auto-start mgserver.exe: %START_MGSERVER%
echo Time to wait for mgserver.exe: %MGSERVER_WAIT%
echo Running Web API only: %API_ONLY%
echo PHP dir: %OEM_PHP%
echo ******************************************************************************

REM Sanity checks
IF NOT EXIST "%WEB_BIN%" (
    echo Could not find required directory %WEB_BIN%
    goto quit
)
IF NOT EXIST "%OEM_SQLITE%" (
    echo Could not find required directory %OEM_SQLITE%
    goto quit
)

REM Copy required php extensions. Copy everything from %WEB_BIN% as PHP MapGuide API requires these
echo [check]: PHP MapGuide dlls
IF NOT EXIST "%OEM_PHP%\php_MapGuideApi.dll" copy /Y "%WEB_BIN%\*.dll" "%OEM_PHP%" > NUL
echo [check]: Checking PHP SQLite dlls
IF NOT EXIST "%OEM_PHP%\php_SQLitePhpApi.dll" copy /Y "%OEM_SQLITE%\php_SQLitePhpApi.dll" "%OEM_PHP%" > NUL

REM Copy resource files to satisfy new relative path of webconfig.ini
echo [check]: MapGuide messages (en)
xcopy /S /Y /I "%MAPAGENT%\Resources" WebTier\php\Resources > NUL

IF "%START_MGSERVER%" == "1" (
    echo Starting mgserver.exe
    pushd "%SERVER_BIN%"
    start mgserver run
    REM An elegant hack to pause this script allowing mgserver to start up
    echo Waiting %MGSERVER_WAIT%s for mgserver.exe to start up
    ping -n %MGSERVER_WAIT% 127.0.0.1 > NUL
    popd
)

REM We're ready now. Dive in
echo Starting unit tests
pushd WebTier\php
IF "%API_ONLY%" == "1" (
    php.exe -n -d display_errors=Off -d extension_dir=%OEM_PHP% -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll -d extension=php_SQLitePhpApi.dll RunTests.php -apionly
) ELSE (
    php.exe -n -d display_errors=Off -d extension_dir=%OEM_PHP% -d extension=php_mbstring.dll -d extension=php_curl.dll -d extension=php_MapGuideApi.dll -d extension=php_SQLitePhpApi.dll RunTests.php
)
popd

IF "%START_MGSERVER%" == "1" (
    REM We're naturally assuming the only mgserver.exe that is running is the one this script started
    echo Terminating mgserver.exe
    taskkill /im mgserver.exe
)

:quit
SET PATH=%OLD_PATH%