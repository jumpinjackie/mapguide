@echo off
rem ==================================================
rem setenvironment.bat
rem
rem Sets the required environment variables for the
rem build.bat script
rem
rem This script must be run from the VS2012 command prompt
rem ==================================================
rem Top-level vars
rem ==================================================
SET OLDPATH=%PATH%
SET TYPEACTION=build
SET TYPECOMPONENT=all

SET TYPEBUILD=unknown
SET CONFIGURATION=unknown
SET PLATFORM=unknown
SET PLATFORM_CLR=unknown

SET PCONF=release
if not "%1" == "" set PCONF=%1

if "%PCONF%"=="debug" (
SET TYPEBUILD=Debug
SET CONFIGURATION=Debug
SET PLATFORM=Win32
SET PLATFORM_CLR=x86
)

IF "%PCONF%"=="release" (
SET TYPEBUILD=Release
SET CONFIGURATION=Release
SET PLATFORM=Win32
SET PLATFORM_CLR=x86
)

if "%TYPEBUILD%"=="unknown" (
    echo Unknown configuration: %PCONF%
    goto help_show
)
rem ==================================================
rem MapGuide vars
rem ==================================================
SET MG_DEV=%CD%
SET MG_OEM=%MG_DEV%\Oem
SET MG_OEM_DBXML=dbxml
SET MG_OEM_DB=db-4.8.26
SET MG_FDO_BIN=%MG_OEM%\FDO\Bin\%TYPEBUILD%
SET MG_FDO_INC=%MG_OEM%\FDO\Inc
SET MG_FDO_LIB=%MG_OEM%\FDO\Lib
SET MG_COMMON=%MG_DEV%\Common
SET MG_SERVER=%MG_DEV%\Server
SET MG_WEB=%MG_DEV%\Web
SET MG_WEB_SRC=%MG_WEB%\src
SET MG_WEB_BIN=%MG_WEB%\bin
SET MG_UNIT_TEST=%MG_DEV%\UnitTest
SET MG_FUSION=%MG_OEM%\Fusion
SET MG_DOC=%MG_DEV%\Doc
SET MG_DOC_OUTPUT=%MG_DOC%\MgOpensource_WebAPIReference
SET MG_DOC_DEVGUIDE_SAMPLES=%MG_DOC%\samples
SET MG_BUILD_TEMP=%MG_DEV%\BuildTemp

rem === Intermediate build vars ===
SET MG_BUILD_DBXML_EXE_PATH=%MG_OEM%\%MG_OEM_DBXML%\bin\%CONFIGURATION%
SET MG_BUILD_SQLITE_PHP_API=%MG_OEM%\SQLite\bin\%TYPEBUILD%\php_SQLitePhpApi.dll
SET MG_BUILD_MAPAGENT_PATH=%MG_WEB_SRC%\mapagent
SET MG_BUILD_MAPAGENT=%MG_BUILD_MAPAGENT_PATH%\mod_mgmapagent.so

SET MG_OUTPUT=%MG_DEV%\%TYPEBUILD%
SET MG_OUTPUT_SERVER=%MG_OUTPUT%\Server
SET MG_OUTPUT_WEB=%MG_OUTPUT%\Web
SET MG_OUTPUT_CSMAP=%MG_OUTPUT%\CS-Map
SET MG_BUILD_COMPONENT=

SET MG_DEFAULT_INSTALLDIR=C:\Program Files\MapGuideOpenSource2.0
SET MG_ERROR=0
rem ==================================================
rem Extra tool paths
rem 
rem Your paths may be different, adjust accordingly or
rem set the appropriate environment variables before
rem running
rem ==================================================

IF "%JAVA_HOME%" == "" SET JAVA_HOME=C:\Program Files\Java\jdk1.6.0_31
IF "%DOXYGEN%" == "" SET DOXYGEN=%MG_DEV%\BuildTools\doxygen
IF "%GNUWIN32%" == "" SET GNUWIN32=C:\Program Files (x86)\GnuWin32\bin
IF "%SEVENZ%" == "" SET SEVENZ=%MG_DEV%\BuildTools\WebTools\7-Zip
IF "%ANT_HOME%" == "" SET ANT_HOME=C:\apache-ant-1.8.3

SET PATH=%PATH%;%DOXYGEN%;%GNUWIN32%;%SEVENZ%;%ANT_HOME%\bin
rem ==================================================
rem PHP/httpd env vars for Visual Studio
rem ==================================================

rem Comment out this line if you intend to build against a different version of PHP/httpd
rem Be sure to change the paths below as well if that's the case
call prepare_webtier_components.bat

rem Set the env vars that VS will reference
SET PHP_SRC=%MG_OEM%\LinuxApt\php-5.5.3
SET PHP_LIB=%MG_OEM%\php\Release_TS
SET HTTPD_SRC=%MG_OEM%\LinuxApt\httpd-2.4.6
SET HTTPD_LIB=%MG_OEM%\httpd\lib
rem Sanity checks
if not exist "%PHP_SRC%" echo Environment variable PHP_SRC does not point to a valid directory (%PHP_SRC%). Please edit setenvironment64.bat to ensure PHP_SRC points to a valid directory
if not exist "%PHP_LIB%\php5ts.lib" echo Environment variable PHP_LIB does not point to a valid directory (%PHP_LIB%). Could not find php5ts.lib in this directory. Please edit setenvironment64.bat to ensure PHP_LIB points to a valid directory
if not exist "%HTTPD_SRC%" echo Environment variable HTTPD_SRC does not point to a valid directory (%HTTPD_SRC%). Please edit setenvironment64.bat to ensure HTTPD_SRC points to a valid directory
if not exist "%HTTPD_LIB%\libhttpd.lib" echo Environment variable HTTPD_LIB does not point to a valid directory (%HTTPD_LIB%). Could not find libhttpd.lib in this directory. Please edit setenvironment64.bat to ensure HTTPD_LIB points to a valid directory
if not exist "%HTTPD_LIB%\libapr-1.lib" echo Environment variable HTTPD_LIB does not point to a valid directory (%HTTPD_LIB%). Could not find libapr-1.lib in this directory. Please edit setenvironment64.bat to ensure HTTPD_LIB points to a valid directory
if not exist "%HTTPD_LIB%\libaprutil-1.lib" echo Environment variable HTTPD_LIB does not point to a valid directory (%HTTPD_LIB%). Could not find libaprutil-1.lib in this directory. Please edit setenvironment64.bat to ensure HTTPD_LIB points to a valid directory

rem ==================================================
rem MSBuild Settings
rem ==================================================

rem If the NUMBER_OF_PROCESSORS environment variable is wrong for any reason. Change this value.
SET CPU_CORES=%NUMBER_OF_PROCESSORS%

rem Uncomment the line below to enable msbuild logging
rem SET MSBUILD_LOG=/l:FileLogger,Microsoft.Build.Engine;logfile=Build.log;verbosity=diagnostic
SET MSBUILD_VERBOSITY=/v:n

rem ==================================================
rem Command aliases
rem ==================================================
SET XCOPY=xcopy /E /Y /I /Q /H
SET XCOPY_SINGLE=xcopy /Y /I /Q
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLR=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM_CLR% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% /t:Clean %MSBUILD_VERBOSITY%
SET MSBUILD_CLEAN_CLR=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM_CLR% /t:Clean %MSBUILD_VERBOSITY%
SET ANT=ant

echo ======== Environment Variable Summary =============
echo PHP_SRC    = %PHP_SRC%
echo PHP_LIB    = %PHP_LIB%
echo HTTPD_SRC  = %HTTPD_SRC%
echo HTTPD_LIB  = %HTTPD_LIB%
echo Configuration is [%TYPEBUILD%, "%CONFIGURATION%|%PLATFORM%"]
echo Deployment Directory for Server: %MG_OUTPUT_SERVER%
echo Deployment Directory for Web: %MG_OUTPUT_WEB%
echo CPU cores: %CPU_CORES%
echo =================== FDO ===========================
echo Binaries: %MG_FDO_BIN%
echo Headers: %MG_FDO_INC%
echo Libs: %MG_FDO_LIB%
echo =================== Oem ===========================
echo DBXML executables: %MG_BUILD_DBXML_EXE_PATH%
echo SQLite PHP API: %MG_BUILD_SQLITE_PHP_API%
echo mod_mgmapagent.so: %MG_BUILD_MAPAGENT%
echo ===================================================
echo Type build.bat to build MapGuide

goto done

:help_show
echo ************************************************************************
echo Usage: setenvironment.bat [debug|release]
echo If unspecified, configuration will default to release
echo ************************************************************************
:done