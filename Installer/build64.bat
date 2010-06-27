@echo off
rem MapGuide Installer build script for windows
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem This script will build all the MapGuide Installer components, if the output path is specified
rem it will also copy the installer to the specified output path. Otherwise it will use the default
rem value of INSTALLER_OUTPUT defined in this file
rem
rem Requirements:
rem   1. Wix 3.0 (latest beta) (http://wix.sourceforge.net)
rem   2. Paraffin (http://www.wintellect.com/CS/blogs/jrobbins/archive/2008/12/22/paraffin-3-0-now-with-full-wix-3-0-support.aspx)
rem
rem Usage:
rem
rem build.bat [-h]
rem           [-v]
rem           [-c=BuildType]
rem           [-a=Action]
rem           [-lang=Culture]
rem           [-srv=ServerDirectory]
rem	      	  [-source=SourceDirectory]
rem	      	  [-version=MapGuideVersion]
rem	      	  [-name=MapGuideInstallerFilename]
rem	      	  [-title=MapGuideInstallerTitle]
rem
rem BuildType: Release(default), Debug, Release64, Debug64
rem Action: build(default), clean, regen, prepare, generate (only use generate when creating new GIDs)
rem SourceDirectory: The directory that was used for MapGuide build ouput 
rem	MapGuideVersion: The version associated with the installer in the format 2.1.0.0
rem	MapGuideInstallerFilename: File name of output .exe in the format MapGuideOpenSource-2.1.0-Something
rem	MapGuideInstallerTitle: Title to appear in the installer.

rem ==================================================
rem Top-level vars
rem ==================================================
SET OLDPATH=%PATH%

rem ==================================================
rem Command Line Option Defaults
rem ==================================================

SET TYPEACTION=build
SET CPUTYPE=x64
SET TYPEBUILD=Release64
SET CONFIGURATION=Release
SET PLATFORM=x64
SET CULTURE=en-US
SET INSTALLER_VERSION_MAJOR_MINOR_REV=2.2.0
SET INSTALLER_NAME=MapGuideOpenSource-%INSTALLER_VERSION_MAJOR_MINOR_REV%-Trunk-%CULTURE%-%TYPEBUILD%
SET INSTALLER_VERSION=%INSTALLER_VERSION_MAJOR_MINOR_REV%.0
SET INSTALLER_TITLE="MapGuide Open Source 2.2 Trunk (%TYPEBUILD%)"
SET MG_REG_KEY=Software\OSGeo\MapGuide\%INSTALLER_VERSION_MAJOR_MINOR_REV%
SET MG_SOURCE=%CD%\..\MgDev\%TYPEBUILD%
SET MG_SOURCE_INC=
rem Set to NO to build installers quicker (at the expense of file size)
SET MAX_COMPRESSION=YES

rem ==================================================
rem MapGuide Installer vars
rem ==================================================
SET INSTALLER_DEV=%CD%
SET INSTALLER_OUTPUT=%INSTALLER_DEV%\Output\%CULTURE%\%TYPEBUILD%
SET INSTALLER_DEV_SUPPORT=%INSTALLER_DEV%\Support
SET INSTALLER_DEV_BOOTSTRAP=%INSTALLER_DEV%\Bootstrapper

SET INSTALLER_FDO_REG_UTIL=%INSTALLER_DEV%\FdoRegUtil
SET INSTALLER_DEV_CSMAP=%INSTALLER_DEV%\Libraries\CS Map
SET INSTALLER_DEV_FDO=%INSTALLER_DEV%\Libraries\FDO
SET INSTALLER_DEV_MGSERVER=%INSTALLER_DEV%\Libraries\MapGuide Server
SET INSTALLER_DEV_MGWEB=%INSTALLER_DEV%\Libraries\MapGuide Web Extensions
SET INSTALLER_DEV_INSTALLER=%INSTALLER_DEV%\Installers\MapGuide

SET NSIS=%CD%\Support\NSIS
SET PARAFFIN=%CD%
SET PATH=%PATH%;%PARAFFIN%;%NSIS%

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
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% /t:Clean %MSBUILD_VERBOSITY%

:study_params
if (%1)==() goto start_build

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-c"       goto get_conf
if "%1"=="-config"  goto get_conf

if "%1"=="-a"       goto get_action
if "%1"=="-action"  goto get_action

if "%1"=="-lang"	goto get_language

if "%1"=="-v"       goto get_verbose
if "%1"=="-source"		 goto get_source

if "%1"=="-version"       goto get_version
if "%1"=="-name"		 goto get_name
if "%1"=="-title"		 goto get_title

goto custom_error

:next_param
shift
shift
goto study_params

:get_language
SET CULTURE=%2
SET INSTALLER_OUTPUT=%INSTALLER_DEV%\Output\%CULTURE%
goto next_param

:get_verbose
SET MSBUILD_VERBOSITY=/v:d
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% /t:Clean %MSBUILD_VERBOSITY%
goto next_param

:get_source
SET MG_SOURCE=%2
SET MG_SOURCE_INC=%MG_SOURCE%
goto next_param

:get_version
SET INSTALLER_VERSION=%2
goto next_param

:get_name
SET INSTALLER_NAME=%2
goto next_param

:get_title
SET INSTALLER_TITLE=%2
goto next_param

:get_conf
SET TYPEBUILD=%2
SET CONFIGURATION=invalid
SET PLATFORM=invalid

IF "%2"=="Release" (
SET CONFIGURATION=Release
SET PLATFORM=Win32
)

IF "%2"=="Release64" (
SET CONFIGURATION=Release
SET PLATFORM=x64
)

if "%2"=="Debug" (
SET CONFIGURATION=Debug
SET PLATFORM=Win32
)

if "%2"=="Debug64" (
SET CONFIGURATION=Debug
SET PLATFORM=x64
)
if "%2"=="Debug" goto next_param
SET INSTALLER_OUTPUT=%CD%\Installers\MapGuide\bin\%TYPEBUILD%
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%CONFIGURATION% /p:Platform=%PLATFORM% /t:Clean %MSBUILD_VERBOSITY%
SET MG_SOURCE=%CD%\..\MgDev\%TYPEBUILD%
IF NOT ""=="%MG_SOURCE_INC%" SET MG_SOURCE_INC=%MG_SOURCE%

if NOT %CONFIGURATION%=="invalid" goto next_param
SET ERRORMSG=Unrecognised configuration: %2
goto custom_error

:get_action
SET TYPEACTION=%2
if "%2"=="build" goto next_param
if "%2"=="clean" goto next_param
if "%2"=="regen" goto next_param
if "%2"=="generate" goto next_param
if "%2"=="prepare" goto next_param
SET ERRORMSG=Unrecognised action: %2
goto custom_error

:start_build
echo ===================================================
echo Configuration is [%TYPEBUILD%]
echo Action is [%TYPEACTION%]
echo CPU cores: %CPU_CORES%
echo Installer Output Directory: %INSTALLER_OUTPUT%
echo MG Source Directory: %MG_SOURCE%
echo Locale: %CULTURE%
echo Registry Root: %MG_REG_KEY%
echo ===================================================

if "%TYPEACTION%"=="build" goto build
if "%TYPEACTION%"=="prepare" goto prepare
if "%TYPEACTION%"=="clean" goto clean
if "%TYPEACTION%"=="generate" goto generate
if "%TYPEACTION%"=="regen" goto regen

:clean
echo [clean]: FdoRegUtil
pushd %INSTALLER_FDO_REG_UTIL%
%MSBUILD_CLEAN% FdoRegUtil.vcproj
popd
echo [clean]: CS-Map
pushd "%INSTALLER_DEV_CSMAP%"
%MSBUILD_CLEAN% "CS Map.wixproj"
popd
echo [clean]: MapGuide Server
pushd "%INSTALLER_DEV_MGSERVER%"
%MSBUILD_CLEAN% "MapGuide Server.wixproj"
popd
echo [clean]: MapGuide Web
pushd "%INSTALLER_DEV_MGWEB%"
%MSBUILD_CLEAN% "MapGuide Web Extensions.wixproj"
popd
echo [clean]: Installer
%MSBUILD_CLEAN% Installer.sln
popd
goto quit

:prepare
echo [prepare] MapGuide Installer
if not exist "%MG_SOURCE%\Server" goto error_mg_server_not_found
if not exist "%MG_SOURCE%\Web" goto error_mg_web_not_found
if not exist "%MG_SOURCE%\CS-Map" goto error_mg_csmap_not_found
echo [prepare] FdoRegUtil.exe
pushd %INSTALLER_FDO_REG_UTIL%
%MSBUILD% FdoRegUtil.vcproj
copy %INSTALLER_FDO_REG_UTIL%\%TYPEBUILD%\FdoRegUtil.exe %MG_SOURCE%\Server\FDO
popd
rem copy support files into server and web directories
echo [prepare] Tomcat
%XCOPY% "%INSTALLER_DEV%\Support\Web\%CPUTYPE%\Tomcat" "%MG_SOURCE%\Web\Tomcat" /EXCLUDE:svn_excludes.txt
echo [prepare] Php
%XCOPY% "%INSTALLER_DEV%\Support\Web\%CPUTYPE%\Php" "%MG_SOURCE%\Web\Php" /EXCLUDE:svn_excludes.txt
echo [prepare] Apache2
%XCOPY% "%INSTALLER_DEV%\Support\Web\%CPUTYPE%\Apache2" "%MG_SOURCE%\Web\Apache2" /EXCLUDE:svn_excludes.txt

goto quit

:regen
echo [regen]: MapGuide Installer

SET PARAFFIN=paraffin.exe -update 

SET WIX_INC_SERVER="%INSTALLER_DEV%\Libraries\MapGuide Server\FileIncludes"
SET WIX_INC_WEB="%INSTALLER_DEV%\Libraries\MapGuide Web Extensions\FileIncludes"
SET WIX_INC_CSMAP="%INSTALLER_DEV%\Libraries\CS Map\FileIncludes"

echo [regen]: Server - binaries
%PARAFFIN% %WIX_INC_SERVER%\incBinFiles.wxs
move /Y %WIX_INC_SERVER%\incBinFiles.PARAFFIN %WIX_INC_SERVER%\incBinFiles.wxs

echo [regen]: Server - RepositoryAdmin
%PARAFFIN% %WIX_INC_SERVER%\incRepositoryAdminFiles.wxs
move /Y %WIX_INC_SERVER%\incRepositoryAdminFiles.PARAFFIN %WIX_INC_SERVER%\incRepositoryAdminFiles.wxs

echo [regen]: Server - resources          
%PARAFFIN% %WIX_INC_SERVER%\incResourcesFiles.wxs
move /Y %WIX_INC_SERVER%\incResourcesFiles.PARAFFIN %WIX_INC_SERVER%\incResourcesFiles.wxs
	  
echo [regen]: Server - schema
%PARAFFIN% %WIX_INC_SERVER%\incSchemaFiles.wxs
move /Y %WIX_INC_SERVER%\incSchemaFiles.PARAFFIN %WIX_INC_SERVER%\incSchemaFiles.wxs

echo [regen]: Server - WMS
%PARAFFIN% %WIX_INC_SERVER%\incWmsFiles.wxs
move /Y %WIX_INC_SERVER%\incWmsFiles.PARAFFIN %WIX_INC_SERVER%\incWmsFiles.wxs

echo [regen]: Server - WFS
%PARAFFIN% %WIX_INC_SERVER%\incWfsFiles.wxs
move /Y %WIX_INC_SERVER%\incWfsFiles.PARAFFIN %WIX_INC_SERVER%\incWfsFiles.wxs

echo [regen]: CS-Map - dictionaries
%PARAFFIN% %WIX_INC_CSMAP%\incCSMapDictionaryFiles.wxs
move /Y %WIX_INC_CSMAP%\incCSMapDictionaryFiles.PARAFFIN %WIX_INC_CSMAP%\incCSMapDictionaryFiles.wxs

echo [regen]: Web - Apache
%PARAFFIN% %WIX_INC_WEB%\incApacheFiles.wxs
move /Y %WIX_INC_WEB%\incApacheFiles.PARAFFIN %WIX_INC_WEB%\incApacheFiles.wxs

echo [regen]: Web - Php
%PARAFFIN% %WIX_INC_WEB%\incPhpFiles.wxs
move /Y %WIX_INC_WEB%\incPhpFiles.PARAFFIN %WIX_INC_WEB%\incPhpFiles.wxs

echo [regen]: Web - Tomcat
%PARAFFIN% %WIX_INC_WEB%\incTomcatFiles.wxs
move /Y %WIX_INC_WEB%\incTomcatFiles.PARAFFIN %WIX_INC_WEB%\incTomcatFiles.wxs

echo [regen]: Web - Help
%PARAFFIN% %WIX_INC_WEB%\incHelpFiles.wxs
move /Y %WIX_INC_WEB%\incHelpFiles.PARAFFIN %WIX_INC_WEB%\incHelpFiles.wxs

echo [regen]: Web - devguide
%PARAFFIN% %WIX_INC_WEB%\incDevGuideFiles.wxs
move /Y %WIX_INC_WEB%\incDevGuideFiles.PARAFFIN %WIX_INC_WEB%\incDevGuideFiles.wxs

echo [regen]: Web - mapagent
%PARAFFIN% %WIX_INC_WEB%\incMapAgentFiles.wxs
move /Y %WIX_INC_WEB%\incMapAgentFiles.PARAFFIN %WIX_INC_WEB%\incMapAgentFiles.wxs

echo [regen]: Web - mapviewernet
%PARAFFIN% %WIX_INC_WEB%\incMapViewerAspxFiles.wxs
move /Y %WIX_INC_WEB%\incMapViewerAspxFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerAspxFiles.wxs

echo [regen]: Web - mapviewerphp
%PARAFFIN% %WIX_INC_WEB%\incMapViewerPhpFiles.wxs
move /Y %WIX_INC_WEB%\incMapViewerPhpFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerPhpFiles.wxs

echo [regen]: Web - mapviewerjava
%PARAFFIN% %WIX_INC_WEB%\incMapViewerJspFiles.wxs
move /Y %WIX_INC_WEB%\incMapViewerJspFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerJspFiles.wxs
         
echo [regen]: Web - fusion
%PARAFFIN% %WIX_INC_WEB%\incFusionFiles.wxs
move /Y %WIX_INC_WEB%\incFusionFiles.PARAFFIN %WIX_INC_WEB%\incFusionFiles.wxs

echo [regen]: Web - misc web root
%PARAFFIN% %WIX_INC_WEB%\incWebRootFiles.wxs
move /Y %WIX_INC_WEB%\incWebRootFiles.PARAFFIN %WIX_INC_WEB%\incWebRootFiles.wxs
%PARAFFIN% %WIX_INC_WEB%\incMapViewerFiles.wxs 
move /Y %WIX_INC_WEB%\incMapViewerFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerFiles.wxs 
%PARAFFIN% %WIX_INC_WEB%\incMapViewerStdiconFiles.wxs 
move /Y %WIX_INC_WEB%\incMapViewerStdiconFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerStdiconFiles.wxs 
%PARAFFIN% %WIX_INC_WEB%\incMapViewerSchemareportFiles.wxs 
move /Y %WIX_INC_WEB%\incMapViewerSchemareportFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerSchemareportFiles.wxs 
%PARAFFIN% %WIX_INC_WEB%\incMapViewerMapAdminFiles.wxs 
move /Y %WIX_INC_WEB%\incMapViewerMapAdminFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerMapAdminFiles.wxs 
%PARAFFIN% %WIX_INC_WEB%\incMapViewerLocalizedFiles.wxs 
move /Y %WIX_INC_WEB%\incMapViewerLocalizedFiles.PARAFFIN %WIX_INC_WEB%\incMapViewerLocalizedFiles.wxs 
%PARAFFIN% %WIX_INC_WEB%\incWebInfFiles.wxs
move /Y %WIX_INC_WEB%\incWebInfFiles.PARAFFIN %WIX_INC_WEB%\incWebInfFiles.wxs

goto quit

:generate
echo [generate]: MapGuide Installer

SET PARAFFIN=paraffin.exe -guids -direXclude .svn 

SET WIX_INC_SERVER="%INSTALLER_DEV%\Libraries\MapGuide Server\FileIncludes"
SET WIX_INC_WEB="%INSTALLER_DEV%\Libraries\MapGuide Web Extensions\FileIncludes"
SET WIX_INC_CSMAP="%INSTALLER_DEV%\Libraries\CS Map\FileIncludes"

echo [generate]: Server - binaries
%PARAFFIN% -dir %MG_SOURCE%\Server\bin -alias $(var.MgSource)\Server\bin -custom SRVBINFILES -dirref SERVERROOTLOCATION -norecurse %WIX_INC_SERVER%\incBinFiles.wxs

echo [generate]: Server - RepositoryAdmin
%PARAFFIN% -dir %MG_SOURCE%\Server\RepositoryAdmin -alias $(var.MgSource)\Server\RepositoryAdmin -custom SRVREPADMINFILES -dirref SERVERROOTLOCATION %WIX_INC_SERVER%\incRepositoryAdminFiles.wxs

echo [generate]: Server - resources          
%PARAFFIN% -dir %MG_SOURCE%\Server\Resources -alias $(var.MgSource)\Server\Resources -custom SRVRESOURCESFILES -dirref SERVERROOTLOCATION %WIX_INC_SERVER%\incResourcesFiles.wxs
	  
echo [generate]: Server - schema
%PARAFFIN% -dir %MG_SOURCE%\Server\Schema -alias $(var.MgSource)\Server\Schema -custom SRVSCHEMAFILES -dirref SERVERROOTLOCATION %WIX_INC_SERVER%\incSchemaFiles.wxs

echo [generate]: Server - WMS
%PARAFFIN% -dir %MG_SOURCE%\Server\wms -alias $(var.MgSource)\Server\wms -custom SRVWMSFILES -dirref SERVERROOTLOCATION -norecurse %WIX_INC_SERVER%\incWmsFiles.wxs

echo [generate]: Server - WFS
%PARAFFIN% -dir %MG_SOURCE%\Server\wfs -alias $(var.MgSource)\Server\wfs -custom SRVWFSFILES -dirref SERVERROOTLOCATION -norecurse %WIX_INC_SERVER%\incWfsFiles.wxs

echo [generate]: CS-Map - dictionaries
%PARAFFIN% -dir %MG_SOURCE%\CS-Map\Dictionaries -alias $(var.MgSource)\CS-Map\Dictionaries -custom CSMAPDICTFILES -dirref CSMAPLOCATION -ext ASC -ext C -ext CNT -ext GID -ext HLP -ext MAK -ext NMK -ext VCPROJ -ext USER %WIX_INC_CSMAP%\incCSMapDictionaryFiles.wxs

echo [generate]: Web - Apache
%PARAFFIN% -dir %MG_SOURCE%\Web\Apache2 -alias $(var.MgSource)\Web\Apache2 -custom APACHEFILES -dirref WEBEXTENSIONSLOCATION %WIX_INC_WEB%\incApacheFiles.wxs

echo [generate]: Web - Php
%PARAFFIN% -dir %MG_SOURCE%\Web\Php -alias $(var.MgSource)\Web\Php -custom PHPFILES -dirref WEBEXTENSIONSLOCATION %WIX_INC_WEB%\incPhpFiles.wxs

echo [generate]: Web - Tomcat
%PARAFFIN% -dir %MG_SOURCE%\Web\Tomcat -alias $(var.MgSource)\Web\Tomcat -custom TOMCATFILES -dirref WEBEXTENSIONSLOCATION %WIX_INC_WEB%\incTomcatFiles.wxs

echo [generate]: Web - Help
%PARAFFIN% -dir %MG_SOURCE%\Web\www\help -alias $(var.MgSource)\Web\www\help -custom HELPFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incHelpFiles.wxs

echo [generate]: Web - Developer's Guide Samples
%PARAFFIN% -dir %MG_SOURCE%\Web\www\devguide -alias $(var.MgSource)\Web\www\devguide -custom DEVGUIDEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incDevGuideFiles.wxs

echo [generate]: Web - mapagent
%PARAFFIN% -dir %MG_SOURCE%\Web\www\mapagent -alias $(var.MgSource)\Web\www\mapagent -custom MAPAGENTFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapAgentFiles.wxs

echo [generate]: Web - mapviewernet
%PARAFFIN% -dir %MG_SOURCE%\Web\www\mapviewernet -alias $(var.MgSource)\Web\www\mapviewernet -custom MAPVIEWERASPXFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerAspxFiles.wxs

echo [generate]: Web - mapviewerphp
%PARAFFIN% -dir %MG_SOURCE%\Web\www\mapviewerphp -alias $(var.MgSource)\Web\www\mapviewerphp -custom MAPVIEWERPHPFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerPhpFiles.wxs

echo [generate]: Web - mapviewerjava
%PARAFFIN% -dir %MG_SOURCE%\Web\www\mapviewerjava -alias $(var.MgSource)\Web\www\mapviewerjava -custom MAPVIEWERJSPFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerJspFiles.wxs
         
echo [generate]: Web - fusion
%PARAFFIN% -dir %MG_SOURCE%\Web\www\fusion -alias $(var.MgSource)\Web\www\fusion -custom FUSIONFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incFusionFiles.wxs

echo [generate]: Web - misc web root
%PARAFFIN% -dir %MG_SOURCE%\Web\www -alias $(var.MgSource)\Web\www -custom WEBROOTFILES -dirref WEBEXTENSIONSLOCATION -norecurse %WIX_INC_WEB%\incWebRootFiles.wxs
%PARAFFIN% -dir %MG_SOURCE%\Web\www\viewerfiles -alias $(var.MgSource)\Web\www\viewerfiles -custom MAPVIEWERFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerFiles.wxs 
%PARAFFIN% -dir %MG_SOURCE%\Web\www\stdicons -alias $(var.MgSource)\Web\www\stdicons -custom MAPVIEWERSTDICONFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerStdiconFiles.wxs 
%PARAFFIN% -dir %MG_SOURCE%\Web\www\schemareport -alias $(var.MgSource)\Web\www\schemareport -custom MAPVIEWERSCHEMAREPORTFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerSchemareportFiles.wxs 
%PARAFFIN% -dir %MG_SOURCE%\Web\www\mapadmin -alias $(var.MgSource)\Web\www\mapadmin -custom MAPVIEWERMAPADMINFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerMapAdminFiles.wxs 
%PARAFFIN% -dir %MG_SOURCE%\Web\www\localized -alias $(var.MgSource)\Web\www\localized -custom MAPVIEWERLOCALIZEDFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incMapViewerLocalizedFiles.wxs 
%PARAFFIN% -dir %MG_SOURCE%\Web\www\WEB-INF -alias $(var.MgSource)\Web\www\WEB-INF -custom WEBINFFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incWebInfFiles.wxs

goto quit

:build
echo [build]: Installer
SET RUN_BUILD=%MSBUILD% /p:OutputName=%INSTALLER_NAME%;MgCulture=%CULTURE%;MgTitle=%INSTALLER_TITLE%;MgVersion=%INSTALLER_VERSION%;MgRegKey=%MG_REG_KEY%
if not ""=="%MG_SOURCE_INC%" set RUN_BUILD=%RUN_BUILD%;MgSource=%MG_SOURCE_INC%
set RUN_BUILD=%RUN_BUILD% Installer.sln
%RUN_BUILD% 
if "%errorlevel%"=="1" goto error
pushd "%INSTALLER_DEV_BOOTSTRAP%"
echo [bootstrap]: Creating
%MSBUILD% /p:TargetFile=%INSTALLER_NAME%.msi Bootstrap.proj
popd
if "%errorlevel%"=="1" goto error
if "%MAX_COMPRESSION%"=="YES" goto build_max_compress
goto build_min_compress

:build_min_compress
pushd "%INSTALLER_DEV_BOOTSTRAP%"
echo [bootstrap]: Create self-extracting package
makensis /DINSTALLER_ROOT=%INSTALLER_DEV% /DNSISDIR=%NSIS% /DOUTNAME=%INSTALLER_NAME% /DCULTURE=%CULTURE% /DMG_VERSION=%INSTALLER_VERSION% Setup.nsi
popd
if "%errorlevel%"=="1" goto error
echo [build]: Installer created at %INSTALLER_OUTPUT%\%INSTALLER_NAME%.exe
goto quit

:build_max_compress
pushd "%INSTALLER_DEV_BOOTSTRAP%"
echo [bootstrap]: Create self-extracting package (MAX compression)
makensis /DINSTALLER_ROOT=%INSTALLER_DEV% /DNSISDIR=%NSIS% /DOUTNAME=%INSTALLER_NAME% /DCULTURE=%CULTURE% /DMAXCOMPRESSION /DMG_VERSION=%INSTALLER_VERSION% Setup.nsi
popd
if "%errorlevel%"=="1" goto error
echo [build]: Installer created at %INSTALLER_OUTPUT%\%INSTALLER_NAME%.exe
goto quit

:error_mg_server_not_found
echo [ERROR]: Could not find MapGuide Server directory at %MG_SOURCE%\Server
exit /B 1

:error_mg_web_not_found
echo [ERROR]: Could not find MapGuide Web Extensions directory at %MG_SOURCE%\Web
exit /B 1

:error_mg_csmap_not_found
echo [ERROR]: Could not find CS-Map directory at %MG_SOURCE%\CS-Map
exit /B 1

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
echo           [-a=Action]
echo           [-lang=Culture]
echo           [-source=SourceDirectory]
echo	         [-version=MapGuideVersion]
echo	         [-name=MapGuideInstallerFilename]
echo	         [-title=MapGuideInstallerTitle]
echo
echo Help:	-h
echo Verbose: -v
echo BuildType: Release(default), Debug
echo Action: build(default), clean, regen, prepare, generate (only use generate for creating new GIDs, or if not installing from ..\MgDev\Release)
echo SourceDirectory: The directory that the MapGuide build process installed the source files into
echo MapGuideVersion: The version associated with the installer in the format 2.1.0.0
echo MapGuideInstallerFilename: File name of output .exe in the format MapGuideOpenSource-2.1.0-Something (such as Beta, RC1, Final, etc)
echo MapGuideInstallerTitle: Title to appear in the installer.  Typically this will be set in the language file, but unofficial releases should override this
echo ************************************************************************
:quit
SET TYPEACTION=
SET TYPEBUILD=
SET INSTALLER_OUTPUT=
SET INSTALLER_DEV=
SET MSBUILD_LOG=
SET MSBUILD_VERBOSITY=
SET XCOPY=
SET MSBUILD=
SET PATH=%OLDPATH%
