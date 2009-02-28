@echo off
rem MapGuide build script for windows
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem This script will build all the MapGuide components, if the output path is specified
rem it will also copy all binaries to the output path. Otherwise it will use the default
rem value of MG_OUTPUT defined in this file
rem 
rem Before you start, please make sure you have done the following:
rem
rem 1) Build the FDO source and place the files in \Oem\FDO (or alternatively download and extract the latest FDO 3.4 binary sdk)
rem    with the following structure:
rem     - Bin\%CONFIG% (where %CONFIG% is debug or release)
rem		- Inc
rem		- Lib
rem    
rem 2) Download and install the following:
rem     - Java SDK (http://java.sun.com)
rem     - ant (http://ant.apache.org)
rem
rem 3) Change the values of JAVA_HOME and ANT_HOME to match the paths of your Java and Apache Ant installations.
rem     
rem For a "cleaner" build experience, run this script from an exported copy of the
rem MapGuide source code (ie. without subversion folders). 
rem
rem Usage:
rem
rem build.bat [-h]
rem           [-v]
rem           [-c=BuildType]
rem           [-w=Component]
rem           [-a=Action]
rem           [-o=OutputDirectory]
rem
rem BuildType: Release(default), Debug
rem Component: all(default), oem, server, web, doc
rem Action: build(defualt), install, clean
rem OutputDirectory: The directory where files will be copied to if -a=install
rem
rem Please note that -a=install does nothing if -w=oem

rem ==================================================
rem Top-level vars
rem ==================================================
SET OLDPATH=%PATH%
SET TYPEACTION=build
SET TYPEBUILD=Release
SET TYPECOMPONENT=all

rem ==================================================
rem MapGuide vars
rem ==================================================
SET MG_DEV=%CD%
SET MG_OEM=%MG_DEV%\Oem
SET MG_SERVER=%MG_DEV%\Server
SET MG_WEB=%MG_DEV%\Web
SET MG_WEB_SRC=%MG_WEB%\src
SET MG_WEB_BIN=%MG_WEB%\bin
SET MG_UNIT_TEST=%MG_DEV%\UnitTest
SET MG_FUSION=%MG_OEM%\Fusion
SET MG_DOC=%MG_DEV%\Doc
SET MG_DOC_OUTPUT=%MG_DOC%\MgOpensource_WebAPIReference
SET MG_BUILD_TEMP=%MG_DEV%\BuildTemp

SET MG_OUTPUT=%MG_DEV%\%TYPEBUILD%
SET MG_OUTPUT_SERVER=%MG_OUTPUT%\Server
SET MG_OUTPUT_WEB=%MG_OUTPUT%\WebServerExtensions
SET MG_BUILD_COMPONENT=

SET MG_ERROR=0

rem ==================================================
rem Extra tool paths
rem 
rem Your paths may be different, adjust accordingly
rem ==================================================
SET JAVA_HOME=C:\Program Files\Java\jdk1.6.0
SET DOXYGEN=%MG_DEV%\BuildTools\doxygen
SET GNUWIN32=C:\Program Files\GnuWin32\bin
SET SEVENZ=%MG_DEV%\BuildTools\WebTools\7-Zip
SET ANT_HOME=%MG_DEV%\BuildTools\apache-ant-1.7.1

SET PATH=%PATH%;%DOXYGEN%;%GNUWIN32%;%SEVENZ%;%ANT_HOME%\bin

rem ==================================================
rem MSBuild Settings
rem ==================================================

rem If the NUMBER_OF_PROCESSORS environment variable is wrong for any reason. Change this value.
SET CPU_CORES=%NUMBER_OF_PROCESSORS%

rem Uncomment the line below to enable msbuild logging
rem SET MSBUILD_LOG=/l:FileLogger,Microsoft.Build.Engine;logfile=Build.log;verbosity=diagnostic
SET MSBUILD_VERBOSITY=/v:q

rem ==================================================
rem Command aliases
rem ==================================================
rem SET XCOPY=xcopy /E /Y /I /F
SET XCOPY=xcopy /E /Y /I /Q
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%
SET ANT=ant

:study_params
if (%1)==() goto pre_build_check

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-c"       goto get_conf
if "%1"=="-config"  goto get_conf

if "%1"=="-a"       goto get_action
if "%1"=="-action"  goto get_action

if "%1"=="-w"      goto get_component
if "%1"=="-with"    goto get_component

if "%1"=="-v"       goto get_verbose
if "%1"=="-o"       goto get_output

goto custom_error

:next_param
shift
shift
goto study_params

:get_verbose
SET MSBUILD_VERBOSITY=/v:d
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%
goto next_param

:get_output
SET MG_OUTPUT=%2
SET MG_OUTPUT_SERVER=%MG_OUTPUT%\Server
SET MG_OUTPUT_WEB=%MG_OUTPUT%\WebServerExtensions
goto next_param

:get_conf
SET TYPEBUILD=%2
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%
SET MG_OUTPUT=%MG_DEV%\%TYPEBUILD%
SET MG_OUTPUT_SERVER=%MG_OUTPUT%\Server
SET MG_OUTPUT_WEB=%MG_OUTPUT%\WebServerExtensions

if "%2"=="Release" goto next_param
if "%2"=="Debug" goto next_param
SET ERRORMSG=Unrecognised configuration: %2
goto custom_error

:get_action
SET TYPEACTION=%2
if "%2"=="build" goto next_param
if "%2"=="clean" goto next_param
if "%2"=="install" goto next_param
SET ERRORMSG=Unrecognised action: %2
goto custom_error

:get_component
SET TYPECOMPONENT=%2
if "%2"=="oem" goto next_param
if "%2"=="server" goto next_param
if "%2"=="web" goto next_param
if "%2"=="doc" goto next_param
if "%2"=="all" goto next_param
SET ERRORMSG=Unrecognised component: %2
goto custom_error

:pre_build_check
:check_java
if exist "%JAVA_HOME%" goto check_ant
SET ERRORMSG=Unable to find java sdk installation. Please install the java sdk and set the JAVA_HOME environment variable
goto custom_error_no_help
:check_ant
if exist "%ANT_HOME%" goto check_fdo
SET ERRORMSG=Unable to find Apache ant installation. Please install apache ant and set the ANT_HOME environment variable
goto custom_error_no_help
:check_fdo
:check_fdo_bin
if exist "%MG_OEM%\FDO\Bin\%TYPEBUILD%" goto check_fdo_inc
SET ERRORMSG=Unable to find FDO binaries. Copy the FDO binaries %MG_OEM%\FDO\Bin\%TYPEBUILD%
goto custom_error_no_help
:check_fdo_inc
if exist "%MG_OEM%\FDO\Bin\%TYPEBUILD%" goto check_fdo_lib
SET ERRORMSG=Unable to find FDO headers. Copy the FDO headers into %MG_OEM%\Inc
goto custom_error_no_help
:check_fdo_lib
if exist "%MG_OEM%\FDO\Bin\%TYPEBUILD%" goto start_build
SET ERRORMSG=Unable to find FDO lib files. Copy the FDO lib files into %MG_OEM%\Lib
goto custom_error_no_help

:start_build
echo ===================================================
echo Configuration is [%TYPEBUILD%]
echo Action is [%TYPEACTION%] on [%TYPECOMPONENT%]
echo Deployment Directory for Server: %MG_OUTPUT_SERVER%
echo Deployment Directory for Web: %MG_OUTPUT_WEB%
echo CPU cores: %CPU_CORES%
echo ===================================================

if "%TYPEACTION%"=="build" goto build
if "%TYPEACTION%"=="clean" goto clean
if "%TYPEACTION%"=="install" goto install

:clean
if "%TYPECOMPONENT%"=="oem" goto clean_oem
if "%TYPECOMPONENT%"=="server" goto clean_server
if "%TYPECOMPONENT%"=="web" goto clean_web
if "%TYPECOMPONENT%"=="doc" goto clean_doc

:clean_oem
echo [clean]: Clean Oem
%MSBUILD_CLEAN% %MG_OEM%\Oem.sln
echo [clean]: Clean Oem - CSMap
%MSBUILD_CLEAN% %MG_OEM%\CSMap\OpenSource.sln
if not "%TYPECOMPONENT%"=="all" goto quit

:clean_server
echo [clean]: Clean Server
%MSBUILD_CLEAN% %MG_SERVER%\Server.sln
if not "%TYPECOMPONENT%"=="all" goto quit

:clean_web
echo [clean]: Clean Web
%MSBUILD_CLEAN% %MG_WEB_SRC%\WebTier.sln
echo [clean]: Clean fusion
%ANT% clean -f %MG_OEM%\fusion\build.xml
if not "%TYPECOMPONENT%"=="all" goto quit

:clean_doc
echo [clean]: Clean Doc
if exist %MG_DOC_OUTPUT% rd /S /Q %MG_DOC_OUTPUT%
goto quit

:build
if "%TYPECOMPONENT%"=="oem" goto build_oem
if "%TYPECOMPONENT%"=="server" goto build_server
if "%TYPECOMPONENT%"=="web" goto build_web
if "%TYPECOMPONENT%"=="doc" goto build_doc
if "%TYPECOMPONENT%"=="all" goto build_oem
SET ERRORMSG=Unrecognised component: %TYPECOMPONENT%
goto custom_error

:build_oem
echo [build]: Building Oem
%MSBUILD% %MG_OEM%\Oem.sln
if "%errorlevel%"=="1" goto error
rem CsMap is not in Oem.sln, so we need to build that separately
echo [build]: Building Oem - CSMap
%MSBUILD% %MG_OEM%\CsMap\OpenSource.sln
if "%errorlevel%"=="1" goto error
if not "%TYPECOMPONENT%"=="all" goto quit

:build_server
echo [build]: Building Server
%MSBUILD% %MG_SERVER%\Server.sln
if "%errorlevel%"=="1" goto error
if not "%TYPECOMPONENT%"=="all" goto quit

:build_web
echo [build]: Building Web Tier
%MSBUILD% %MG_WEB_SRC%\WebTier.sln
if "%errorlevel%"=="1" goto error
if not "%TYPECOMPONENT%"=="all" goto quit

:build_doc
echo [build]: Building Doc
pushd %MG_DOC%
call MgOpenSource_run_doxygen.bat
popd
goto quit

:install
if "%TYPECOMPONENT%"=="server" goto install_server
if "%TYPECOMPONENT%"=="web" goto install_web
if "%TYPECOMPONENT%"=="doc" goto install_doc
if "%TYPECOMPONENT%"=="all" goto install_server
SET ERRORMSG=Unrecognised component: %TYPECOMPONENT%
goto custom_error

:install_server
echo [install]: Server
echo [install]: Server - Binaries
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%" "%MG_OUTPUT_SERVER%\bin" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - RepositoryAdmin
%XCOPY% "%MG_SERVER%\RepositoryAdmin" "%MG_OUTPUT_SERVER%\RepositoryAdmin" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: CsMap Dictionaries
SET CSMAP_DICT=%MG_OUTPUT_SERVER%\CsMap\Dictionaries
if not exist "%CSMAP_DICT%" mkdir "%CSMAP_DICT%"
copy /Y "%MG_OEM%\CsMap\Dictionaries\*.MRT" "%CSMAP_DICT%"
copy /Y "%MG_OEM%\CsMap\Dictionaries\*.CSD" "%CSMAP_DICT%"
if not "%TYPECOMPONENT%"=="all" goto quit

:install_doc
echo [install]: Documentation
%XCOPY% "%MG_DOC_OUTPUT%" "%MG_OUTPUT_WEB%\Help" /EXCLUDE:svn_excludes.txt
if not "%TYPECOMPONENT%"=="all" goto quit

:install_web
echo [install]: web Tier

echo [install]: Web Tier - localized
%XCOPY% "%MG_WEB_SRC%\localized" "%MG_OUTPUT_WEB%\www\localized" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - mapadmin
%XCOPY% "%MG_WEB_SRC%\mapadmin" "%MG_OUTPUT_WEB%\www\mapadmin" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - mapagent
%XCOPY% "%MG_WEB_SRC%\mapagent" "%MG_OUTPUT_WEB%\www\mapagent" /EXCLUDE:svn_excludes.txt+mapagent_excludes.txt
echo [install]: Web Tier - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\www\mapagent" /EXCLUDE:php_excludes.txt+java_excludes.txt+svn_excludes.txt
echo [install]: Web Tier - mapagent test forms
%XCOPY% "%MG_UNIT_TEST%\WebTier\MapAgent\MapAgentForms" "%MG_OUTPUT_WEB%\www\mapagent" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - stdicons
%XCOPY% "%MG_WEB_SRC%\stdicons" "%MG_OUTPUT_WEB%\www\stdicons" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - viewerfiles
%XCOPY% "%MG_WEB_SRC%\viewerfiles" "%MG_OUTPUT_WEB%\www\viewerfiles" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - webstudio
%XCOPY% "%MG_WEB_SRC%\webstudio" "%MG_OUTPUT_WEB%\www\webstudio" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - schemareport
%XCOPY% "%MG_WEB_SRC%\schemareport" "%MG_OUTPUT_WEB%\www\schemareport" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - webconfig.ini
copy /Y "%MG_WEB_SRC%\webconfig.ini" "%MG_OUTPUT_WEB%\www"
echo [install]: Web Tier - mapviewerphp
%XCOPY% "%MG_WEB_SRC%\mapviewerphp" "%MG_OUTPUT_WEB%\www\mapviewerphp" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - php - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\Php"/EXCLUDE:php_excludes.txt+svn_excludes.txt
echo [install]: Web Tier - php ext - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\Php\ext" /EXCLUDE:php_excludes.txt+svn_excludes.txt
echo [install]: Web Tier - mapviewernet
%XCOPY% "%MG_WEB_SRC%\mapviewernet" "%MG_OUTPUT_WEB%\www\mapviewernet" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - mapviewernet - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\www\mapviewernet\bin" /EXCLUDE:dotnet_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapviewerjava
%XCOPY% "%MG_WEB_SRC%\mapviewerjava" "%MG_OUTPUT_WEB%\www\mapviewerjava" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - mapviewerjava - WEB-INF
%XCOPY% "%MG_WEB_SRC%\WEB-INF" "%MG_OUTPUT_WEB%\www\mapviewerjava\WEB-INF" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - fusion templates
%XCOPY% "%MG_OEM%\fusionMG" "%MG_OUTPUT_WEB%\www\fusion" /EXCLUDE:svn_excludes.txt
echo [install]: Web Tier - Apache module
if not exist "%MG_OUTPUT_WEB%\Apache2\modules" mkdir "%MG_OUTPUT_WEB%\Apache2\modules"
%XCOPY% /F "%MG_WEB_SRC%\mapagent\mod_mgmapagent.so" "%MG_OUTPUT_WEB%\Apache2\modules"
echo [install]: Web Tier - fusion
%ANT% deploy -Ddeploy.home=%MG_OUTPUT_WEB%\www\fusion -f %MG_OEM%\fusion\build.xml

goto quit

:error
echo [ERROR]: There was an error building the component
exit /B 1

:custom_error_no_help
echo [ERROR]: %ERRORMSG%
SET ERRORMSG=
exit /B 1

:custom_error
echo [ERROR]: %ERRORMSG%
SET ERRORMSG=
echo Please use the format:
:help_show
echo ************************************************************************
echo build.bat [-h]
echo           [-v]
echo           [-c=BuildType]
echo           [-w=Component]
echo           [-a=Action]
echo           [-o=OutputDirectory]
echo Help:                  -h[elp]
echo Verbose:               -v
echo BuildType:             -c[onfig]=Release(default), Debug
echo Action:                -a[ction]=build(default),
echo                                  install,
echo                                  clean,
echo Component:             -w[ith]=all(default),
echo                                oem,
echo                                server,
echo                                web,
echo                                doc
echo ************************************************************************
:quit
SET TYPEACTION=
SET TYPEBUILD=
SET MG_OUTPUT=
SET MG_BUILD_COMPONENT=
SET MG_DEV=
SET MG_INSTALLER=
SET MG_OEM=
SET MG_SERVER=
SET MG_WEB=
SET MG_WEB_SRC=
SET MG_WEB_BIN=
SET MG_UNIT_TEST=
SET MG_FUSION=
SET MG_DOC=
SET MG_DOC_OUTPUT=
SET MG_ERROR=
SET JAVA_HOME=
SET DOXYGEN=
SET GNUWIN32=

SET MSBUILD_LOG=
SET MSBUILD_VERBOSITY=
SET XCOPY=
SET MSBUILD=
SET PATH=%OLDPATH%