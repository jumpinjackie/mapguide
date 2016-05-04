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
rem 1) Build the FDO source and place the files in \Oem\FDO (or alternatively download and extract the latest FDO binary sdk)
rem    with the following structure:
rem     - Bin\%CONFIG% (where %CONFIG% is debug or release, debug64 or release64 for 64-bit FDO)
rem     - Inc
rem     - Lib
rem     - Lib64 (for 64-bit FDO)
rem    
rem 2) Download and install the following:
rem     - Java SDK (http://java.sun.com)
rem     - ant (http://ant.apache.org)
rem     - doxygen (http://www.doxygen.org)
rem
rem 3) Call setenvironment.bat or setenvironment64.bat depending on whether you're building 32-bit or 64-bit MapGuide. Adjust any paths in those two files as appropriate so that it points to your specific JDK, ANT and Doxygen directories
rem     
rem For a "cleaner" build experience, run this script from an exported copy of the
rem MapGuide source code (ie. without subversion folders). 
rem
rem Usage:
rem
rem build.bat [-h]
rem           [-w=Component]
rem           [-a=Action]
rem           [-o=OutputDirectory]
rem
rem Component: all(default), oem, server, web, doc, common
rem Action: build(default), install, clean
rem OutputDirectory: The directory where files will be copied to if -a=install, if -o=default or -o=def then 
rem files will be copied to %MG_DEFAULT_INSTALLDIR%
rem
rem Please note that -a=install does nothing if -w=oem

rem ==================================================
rem Parameter Handling
rem ==================================================

:study_params
if (%1)==() goto pre_build_check

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-a"       goto get_action
if "%1"=="-action"  goto get_action

if "%1"=="-w"       goto get_component
if "%1"=="-with"    goto get_component

if "%1"=="-o"       goto get_output

goto custom_error

:next_param
shift
shift
goto study_params

:get_output
SET MG_OUTPUT=%2
if "%2" == "default" SET MG_OUTPUT=%MG_DEFAULT_INSTALLDIR%
if "%2" == "def" SET MG_OUTPUT=%MG_DEFAULT_INSTALLDIR%
SET MG_OUTPUT_SERVER=%MG_OUTPUT%\Server
SET MG_OUTPUT_WEB=%MG_OUTPUT%\Web
SET MG_OUTPUT_CSMAP=%MG_OUTPUT%\CS-Map
SET MG_OUTPUT_TEST=%MG_OUTPUT%\Test
goto next_param

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
if "%2"=="test" goto next_param
if "%2"=="all" goto next_param
if "%2"=="allnodoc" goto next_param
SET ERRORMSG=Unrecognised component: %2
goto custom_error

rem ==================================================
rem Check Environment Vars
rem ==================================================

:pre_build_check
:check_java
if exist "%JAVA_HOME%" goto check_ant
SET ERRORMSG=Unable to find java sdk installation. Please install the java sdk and set the JAVA_HOME environment variable in setenvironment.bat
goto custom_error_no_help
:check_ant
if exist "%ANT_HOME%" goto check_fdo
SET ERRORMSG=Unable to find Apache ant installation. Please install apache ant and set the ANT_HOME environment variable in setenvironment.bat
goto custom_error_no_help
:check_fdo
:check_fdo_bin
if exist "%MG_FDO_BIN%" goto check_fdo_inc
SET ERRORMSG=Unable to find FDO binaries. Copy the FDO binaries to %MG_FDO_BIN%
goto custom_error_no_help
:check_fdo_inc
if exist "%MG_FDO_INC%" goto check_fdo_lib
SET ERRORMSG=Unable to find FDO headers. Copy the FDO headers into %MG_FDO_INC%
goto custom_error_no_help
:check_fdo_lib
if exist "%MG_FDO_LIB%" goto start_build
SET ERRORMSG=Unable to find FDO lib files. Copy the FDO lib files into %MG_FDO_LIB%
goto custom_error_no_help

:start_build
echo ===================================================
echo Configuration is [%TYPEBUILD%, "%CONFIGURATION%|%PLATFORM%"]
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
echo [clean]: Clean Oem - CS-Map
%MSBUILD_CLEAN% %MG_OEM%\CSMap\VC110\OpenSource.sln
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


rem =======================================================
rem Build Targets.... the all targets just carry on
rem =======================================================

:build
if "%TYPECOMPONENT%"=="oem" goto build_oem
if "%TYPECOMPONENT%"=="server" goto build_server
if "%TYPECOMPONENT%"=="web" goto build_web
if "%TYPECOMPONENT%"=="doc" goto build_doc
if "%TYPECOMPONENT%"=="all" goto build_oem
if "%TYPECOMPONENT%"=="allnodoc" goto build_oem
SET ERRORMSG=Unrecognised component: %TYPECOMPONENT%
goto custom_error



:build_oem
echo [build]: Building Oem
%MSBUILD% %MG_OEM%\Oem.sln
if "%errorlevel%"=="1" goto error
rem CsMap is not in Oem.sln, so we need to build that separately
echo [build]: Building Oem - CSMap
%MSBUILD% %MG_OEM%\CsMap\VC140\OpenSource.sln
if "%errorlevel%"=="1" goto error
if "%TYPECOMPONENT%"=="oem" 	goto quit
if "%TYPECOMPONENT%"=="server" 	goto quit
if "%TYPECOMPONENT%"=="web" 	goto quit
if "%TYPECOMPONENT%"=="doc" 	goto quit

:build_server
echo [build]: Building Server
%MSBUILD% %MG_SERVER%\Server.sln
if "%errorlevel%"=="1" goto error
if "%TYPECOMPONENT%"=="oem" 	goto quit
if "%TYPECOMPONENT%"=="server" 	goto quit
if "%TYPECOMPONENT%"=="web" 	goto quit
if "%TYPECOMPONENT%"=="doc" 	goto quit

:build_web
echo [build]: Building Web Tier
%MSBUILD% %MG_WEB_SRC%\WebTier.sln
if "%errorlevel%"=="1" goto error
if "%TYPECOMPONENT%"=="oem" 	 goto quit
if "%TYPECOMPONENT%"=="server" 	 goto quit
if "%TYPECOMPONENT%"=="web" 	 goto quit
if "%TYPECOMPONENT%"=="doc" 	 goto quit
if "%TYPECOMPONENT%"=="allnodoc" goto quit

rem this is left out with component allnodoc

:build_doc
echo [build]: Building Doc
pushd %MG_DOC%
call MgOpenSource_run_doxygen.bat
popd
echo [build]: MapGuide Developer's Guide
pushd %MG_DOC%\devguide
call make.bat html
popd
echo [build]: Sheboygan Sample data set
pushd %MG_DEV%\UnitTest\TestData\Samples\Sheboygan
call build.bat
popd
goto quit

rem =======================================================
rem INSTALL Targets.... the all targets just carry on
rem =======================================================


:install
if "%TYPECOMPONENT%"=="all" goto install_server
if "%TYPECOMPONENT%"=="allnodoc" goto install_server
if "%TYPECOMPONENT%"=="server" goto install_server
if "%TYPECOMPONENT%"=="web" goto install_web
if "%TYPECOMPONENT%"=="doc" goto install_doc
if "%TYPECOMPONENT%"=="test" goto install_test
SET ERRORMSG=Unrecognised component: %TYPECOMPONENT%
goto custom_error

:install_server
echo [install]: Server
echo [install]: Server - Binaries
%XCOPY_SINGLE% "%MG_SERVER%\bin\%TYPEBUILD%" "%MG_OUTPUT_SERVER%\bin" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - FDO
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\FDO" "%MG_OUTPUT_SERVER%\FDO" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - Logs
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\Logs" "%MG_OUTPUT_SERVER%\Logs" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - Packages
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\Packages" "%MG_OUTPUT_SERVER%\Packages" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - Repositories
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\Repositories" "%MG_OUTPUT_SERVER%\Repositories" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - Resources
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\Resources" "%MG_OUTPUT_SERVER%\Resources" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - Schema
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\Schema" "%MG_OUTPUT_SERVER%\Schema" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - Temp
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\Temp" "%MG_OUTPUT_SERVER%\Temp" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - WFS
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\wfs" "%MG_OUTPUT_SERVER%\wfs" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - WMS
%XCOPY% "%MG_SERVER%\bin\%TYPEBUILD%\wms" "%MG_OUTPUT_SERVER%\wms" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Server - DBXML
copy /Y "%MG_BUILD_DBXML_EXE_PATH%\*.exe" "%MG_OUTPUT_SERVER%\bin"
echo [install]: Server - RepositoryAdmin
%XCOPY% "%MG_SERVER%\RepositoryAdmin" "%MG_OUTPUT_SERVER%\RepositoryAdmin" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: CsMap Dictionaries
%XCOPY% "%MG_OEM%\CsMap\Dictionaries" "%MG_OUTPUT_CSMAP%\Dictionaries" /EXCLUDE:svn_excludes.txt+csmap_excludes.txt+%TYPEBUILD%_excludes.txt
if "%TYPECOMPONENT%"=="server" goto quit
if "%TYPECOMPONENT%"=="web" goto quit

:install_web
echo [install]: web Tier

echo [install]: Web Tier - localized
%XCOPY% "%MG_WEB_SRC%\localized" "%MG_OUTPUT_WEB%\www\localized" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapadmin
%XCOPY% "%MG_WEB_SRC%\mapadmin" "%MG_OUTPUT_WEB%\www\mapadmin" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapagent
%XCOPY% "%MG_BUILD_MAPAGENT_PATH%" "%MG_OUTPUT_WEB%\www\mapagent" /EXCLUDE:svn_excludes.txt+mapagent_excludes.txt+%TYPEBUILD%_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\www\mapagent" /EXCLUDE:php_excludes.txt+dotnetapi_excludes.txt+dotnetxml_excludes.txt+java_excludes.txt+svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapagent test forms
%XCOPY% "%MG_UNIT_TEST%\WebTier\MapAgent\MapAgentForms" "%MG_OUTPUT_WEB%\www\mapagent" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - stdicons
%XCOPY% "%MG_WEB_SRC%\stdicons" "%MG_OUTPUT_WEB%\www\stdicons" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - viewerfiles
%XCOPY% "%MG_WEB_SRC%\viewerfiles" "%MG_OUTPUT_WEB%\www\viewerfiles" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - schemareport
%XCOPY% "%MG_WEB_SRC%\schemareport" "%MG_OUTPUT_WEB%\www\schemareport" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - webconfig.ini
copy /Y "%MG_WEB_SRC%\webconfig.ini" "%MG_OUTPUT_WEB%\www"
echo [install]: Web Tier - mapviewerphp
%XCOPY% "%MG_WEB_SRC%\mapviewerphp" "%MG_OUTPUT_WEB%\www\mapviewerphp" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - Tomcat - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\Tomcat\bin"/EXCLUDE:java_excludes.txt+dotnetapi_excludes.txt+dotnetxml_excludes.txt+svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - php - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\Php"/EXCLUDE:php_excludes.txt+dotnetapi_excludes.txt+dotnetxml_excludes.txt+svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - php ext - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\Php\ext" /EXCLUDE:php_excludes.txt+dotnetapi_excludes.txt+dotnetxml_excludes.txt+svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapviewernet
%XCOPY% "%MG_WEB_SRC%\mapviewernet" "%MG_OUTPUT_WEB%\www\mapviewernet" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapviewernet - MapGuide API unmanaged binaries
%XCOPY% "%MG_WEB_BIN%\%TYPEBUILD%" "%MG_OUTPUT_WEB%\www\mapviewernet\bin" /EXCLUDE:svn_excludes.txt+dotnet_excludes.txt+%TYPEBUILD%_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapviewerjava
%XCOPY% "%MG_WEB_SRC%\mapviewerjava" "%MG_OUTPUT_WEB%\www\mapviewerjava" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Web Tier - mapviewerjava - WEB-INF
%XCOPY% "%MG_WEB_SRC%\WEB-INF" "%MG_OUTPUT_WEB%\www\WEB-INF" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
REM Required for Web Tier unit tests
copy /Y "%MG_BUILD_SQLITE_PHP_API%" "%MG_OUTPUT_WEB%\Php\ext"
echo [build]: Web Tier - Clean fusion
call %ANT% clean -f "%MG_OEM%\fusion\build.xml"
echo [build]: Web Tier - Build Fusion API docs
call %ANT% docs -f "%MG_OEM%\fusion\build.xml"
echo [install]: Web Tier - fusion
call %ANT% deploy -Ddeploy.home="%MG_OUTPUT_WEB%\www\fusion" -f "%MG_OEM%\fusion\build.xml"
echo [install]: Fusion - build pack
call %ANT% selfbuild -Dselfbuild.home="%MG_OUTPUT%\fusion-selfbuild" -f "%MG_OEM%\fusion\build.xml"
echo [install]: Web Tier - fusion templates
%XCOPY% "%MG_OEM%\fusionMG" "%MG_OUTPUT_WEB%\www\fusion" /EXCLUDE:svn_excludes.txt+%CONFIGURATION%_excludes.txt
echo [install]: Web Tier - Apache module
if not exist "%MG_OUTPUT_WEB%\Apache24\modules" mkdir "%MG_OUTPUT_WEB%\Apache24\modules"
%XCOPY% /F "%MG_BUILD_MAPAGENT%" "%MG_OUTPUT_WEB%\Apache24\modules"
if "%TYPECOMPONENT%"=="server" goto quit
if "%TYPECOMPONENT%"=="web" goto quit

:install_doc
echo [install]: Documentation
%XCOPY% "%MG_DOC_OUTPUT%" "%MG_OUTPUT_WEB%\www\help\webapi" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: MapGuide Developer's Guide
%XCOPY% "%MG_DOC%\devguide\build\html" "%MG_OUTPUT_WEB%\www\help\devguide" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Developer's Guide Sample Code (PHP)
%XCOPY% "%MG_DOC_DEVGUIDE_SAMPLES%\phpsamples" "%MG_OUTPUT_WEB%\www\phpsamples" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Developer's Guide Sample Code (Java)
%XCOPY% "%MG_DOC_DEVGUIDE_SAMPLES%\javasamples" "%MG_OUTPUT_WEB%\www\javasamples" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Developer's Guide Sample Code (DotNet)
%XCOPY% "%MG_DOC_DEVGUIDE_SAMPLES%\dotnetsamples" "%MG_OUTPUT_WEB%\www\dotnetsamples" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Developer's Guide Sample Code (PHP)
%XCOPY% "%MG_DOC_DEVGUIDE_SAMPLES%\phpviewersample" "%MG_OUTPUT_WEB%\www\phpviewersample" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Developer's Guide Sample Code (Java)
%XCOPY% "%MG_DOC_DEVGUIDE_SAMPLES%\javaviewersample" "%MG_OUTPUT_WEB%\www\javaviewersample" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Developer's Guide Sample Code (DotNet)
%XCOPY% "%MG_DOC_DEVGUIDE_SAMPLES%\dotnetviewersample" "%MG_OUTPUT_WEB%\www\dotnetviewersample" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: OpenLayers Samples
%XCOPY% "%MG_DOC_DEVGUIDE_SAMPLES%\ol2samples" "%MG_OUTPUT_WEB%\www\ol2samples" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Samples landing page
copy /Y "%MG_DOC_DEVGUIDE_SAMPLES%\samples.php" "%MG_OUTPUT_WEB%\www"
echo [install]: Viewer API documentation
%XCOPY% "%MG_DOC%\viewerapi" "%MG_OUTPUT_WEB%\www\help\viewerapi" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
echo [install]: Copy dlls into .net samples
%XCOPY% "%MG_OUTPUT_WEB%\www\mapviewernet\bin\*.dll" "%MG_OUTPUT_WEB%\www\dotnetsamples\bin" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
%XCOPY% "%MG_OUTPUT_WEB%\www\mapviewernet\bin\*.dll" "%MG_OUTPUT_WEB%\www\dotnetviewersample\bin" /EXCLUDE:svn_excludes.txt+%TYPEBUILD%_excludes.txt
if not exist "%MG_DEV%\UnitTest\TestData\Samples\Sheboygan\Sheboygan.mgp" (
    echo [build]: Sheboygan Sample data set
    pushd %MG_DEV%\UnitTest\TestData\Samples\Sheboygan
    call build.bat
    popd
)
copy /Y "%MG_DEV%\UnitTest\TestData\Samples\Sheboygan\Sheboygan.mgp" "%MG_OUTPUT%"
if "%TYPECOMPONENT%"=="doc" goto quit
:install_test
echo [install]: Test Suite
%XCOPY% "%MG_UNIT_TEST%" "%MG_OUTPUT_TEST%\UnitTest"
if not exist "%MG_OUTPUT_TEST%\Oem\SQLite\bin\%TYPEBUILD%" mkdir "%MG_OUTPUT_TEST%\Oem\SQLite\bin\%TYPEBUILD%"
copy /Y "%MG_BUILD_SQLITE_DOTNET_API%" "%MG_OUTPUT_TEST%\Oem\SQLite\bin\%TYPEBUILD%"
copy /Y test_readme.txt "%MG_OUTPUT_TEST%"
copy /Y run_tests.bat "%MG_OUTPUT_TEST%"
copy /Y run_tests.sh "%MG_OUTPUT_TEST%"
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
echo           [-w=Component]
echo           [-a=Action]
echo           [-o=OutputDirectory]
echo Help:                  -h[elp]
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
