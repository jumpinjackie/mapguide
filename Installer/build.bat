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
rem   1. Wix 3.5 (http://wix.codeplex.com)
rem   2. Paraffin (http://www.wintellect.com/CS/blogs/jrobbins/archive/2008/12/22/paraffin-3-0-now-with-full-wix-3-0-support.aspx)
rem
rem Usage:
rem
rem build.bat [-h]
rem           [-v]
rem           [-a=Action]
rem           [-lang=Culture]
rem           [-srv=ServerDirectory]
rem	      	  [-source=SourceDirectory]
rem	      	  [-version=MapGuideVersion]
rem	      	  [-name=MapGuideInstallerFilename]
rem	      	  [-title=MapGuideInstallerTitle]
rem
rem Action: build(default), clean, prepare, generate (only use generate when creating new GIDs)
rem SourceDirectory: The directory that was used for MapGuide build output 
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
SET CULTURE=en-US
SET INSTALLER_VERSION_MAJOR_MINOR=3.1
SET INSTALLER_VERSION_MAJOR_MINOR_REV=%INSTALLER_VERSION_MAJOR_MINOR%.0
SET INSTALLER_NAME=MapGuideOpenSource-%INSTALLER_VERSION_MAJOR_MINOR_REV%-Trunk-%CULTURE%-%TYPEBUILD%-%PLATFORM_CLR%
SET INSTALLER_VERSION=%INSTALLER_VERSION_MAJOR_MINOR_REV%.0
SET INSTALLER_TITLE="MapGuide Open Source %INSTALLER_VERSION_MAJOR_MINOR% Trunk (%TYPEBUILD%)"
rem Default to no (omit the ArcSDE installer feature and don't look for ArcSDE provider dlls in staging area)
rem Only when we're packaging against the official FDO SDK can we set this to yes
SET MG_WITH_ARCSDE=no
SET MG_REG_KEY=Software\OSGeo\MapGuide\%INSTALLER_VERSION_MAJOR_MINOR_REV%
SET MG_SOURCE=%CD%\..\MgDev\%TYPEBUILD%
SET MG_SOURCE_WEB=%CD%\..\MgDev\Web
SET MG_SOURCE_INC=%MG_SOURCE%
rem Set to NO to build installers quicker (at the expense of file size)
SET MAX_COMPRESSION=YES

rem Unlike other parameters, this is an env-var to faciliate ease of invocation from a CI like Jenkins
IF "%WITH_ARCSDE%"=="yes" SET MG_WITH_ARCSDE=yes

rem ==================================================
rem MapGuide Installer vars
rem ==================================================
SET INSTALLER_DEV=%CD%
SET INSTALLER_OUTPUT=%INSTALLER_DEV%\Output\%CULTURE%
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
SET PATH=%PATH%;%PARAFFIN%;%NSIS%;%WIX%\bin\;%CD%\..\MgDev\BuildTools\WebTools\7-Zip

rem ==================================================
rem Web dependencies
rem ==================================================
SET HTTPD_VERSION=2.4.18
SET PHP_VERSION=5.6.23
SET TOMCAT_VERSION=7.0.68
SET HTTPD_PACKAGE=httpd-%HTTPD_VERSION%-%PLATFORM_CLR%-VC11.zip
SET TOMCAT_PACKAGE_NAME=apache-tomcat-%TOMCAT_VERSION%
SET TOMCAT_PACKAGE=%MG_OEM%\LinuxApt\%TOMCAT_PACKAGE_NAME%.tar.gz
SET PHP_TS_PACKAGE=php-%PHP_VERSION%-%PLATFORM_CLR%-VC11.zip

:study_params
if (%1)==() goto start_build

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-a"       goto get_action
if "%1"=="-action"  goto get_action

if "%1"=="-lang"	goto get_language

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

:get_action
SET TYPEACTION=%2
if "%2"=="build" goto next_param
if "%2"=="clean" goto next_param
if "%2"=="generate" goto next_param
if "%2"=="prepare" goto next_param
SET ERRORMSG=Unrecognised action: %2
goto custom_error

:start_build
echo ===================================================
echo Configuration is [%TYPEBUILD%]
echo CPU type is: [%PLATFORM_CLR%]
echo Action is [%TYPEACTION%]
echo CPU cores: %CPU_CORES%
echo Installer Output Directory: %INSTALLER_OUTPUT%
echo MG Source Directory: %MG_SOURCE%
echo Locale: %CULTURE%
echo Registry Root: %MG_REG_KEY%
echo Bundling ArcSDE Provider: %MG_WITH_ARCSDE%
echo ===================================================

if "%TYPEACTION%"=="build" goto build
if "%TYPEACTION%"=="prepare" goto prepare
if "%TYPEACTION%"=="clean" goto clean
if "%TYPEACTION%"=="generate" goto generate

:clean
echo [clean]: Installer Pre-Reqs
%MSBUILD_CLEAN% InstallerPreReq.sln
echo [clean]: Installer
%MSBUILD_CLEAN_CLR% InstallerWix.sln
goto quit

:prepare
echo [prepare] MapGuide Installer
if not exist "%MG_SOURCE%\Server" goto error_mg_server_not_found
if not exist "%MG_SOURCE%\Web" goto error_mg_web_not_found
if not exist "%MG_SOURCE%\CS-Map" goto error_mg_csmap_not_found
echo [prepare] Installer Pre-Requisites
%MSBUILD% InstallerPreReq.sln
copy %INSTALLER_FDO_REG_UTIL%\%TYPEBUILD%\FdoRegUtil.exe %MG_SOURCE%\Server\FDO
popd
echo [prepare] Unpack Apache httpd
pushd "%INSTALLER_DEV_SUPPORT%\Web\%PLATFORM_CLR%"
7z x %HTTPD_PACKAGE% -y -o"%MG_SOURCE%\Web"
popd
echo [prepare] Unpack Tomcat
pushd "%INSTALLER_DEV_SUPPORT%\Web\%PLATFORM_CLR%"
copy /Y mod_jk.so "%MG_SOURCE%\Web\Apache24\modules"
REM we unpack to root because Tomcat is the root dir in the zip file
7z x "%TOMCAT_PACKAGE%" -y -o"%MG_SOURCE%\Web"
popd
pushd "%MG_SOURCE%\Web"
if exist %TOMCAT_PACKAGE_NAME%.tar (
    7z x %TOMCAT_PACKAGE_NAME%.tar -y
    del %TOMCAT_PACKAGE_NAME%.tar
)
if exist Tomcat (
xcopy /S /Y apache-tomcat-%TOMCAT_VERSION%\*.* Tomcat
rd /S /Q apache-tomcat-%TOMCAT_VERSION%
) else (
rename apache-tomcat-%TOMCAT_VERSION% Tomcat
)
popd
echo [prepare] Unpack PHP (Thread Safe)
pushd "%INSTALLER_DEV_SUPPORT%\Web\%PLATFORM_CLR%"
7z x %PHP_TS_PACKAGE% -y -o"%MG_SOURCE%\Web\Php"
popd
echo [prepare] Copy support dlls for PostgreSQL provider
copy /Y "%MG_SOURCE%\Web\Php\libpq.dll" "%MG_SOURCE%\Server\FDO"
copy /Y "%MG_SOURCE%\Web\Php\libeay32.dll" "%MG_SOURCE%\Server\FDO"
copy /Y "%MG_SOURCE%\Web\Php\ssleay32.dll" "%MG_SOURCE%\Server\FDO"
rem copy template configs on top
echo [prepare] Tomcat config
%XCOPY% "%INSTALLER_DEV%\Support\Web\%PLATFORM_CLR%\configs\Tomcat" "%MG_SOURCE%\Web\Tomcat" /EXCLUDE:svn_excludes.txt
echo [prepare] Php config
%XCOPY% "%INSTALLER_DEV%\Support\Web\%PLATFORM_CLR%\configs\Php" "%MG_SOURCE%\Web\Php" /EXCLUDE:svn_excludes.txt
echo [prepare] Apache2 config
%XCOPY% "%INSTALLER_DEV%\Support\Web\%PLATFORM_CLR%\configs\Apache2" "%MG_SOURCE%\Web\Apache24" /EXCLUDE:svn_excludes.txt
echo [prepare] Apache2 mapagent so
copy /Y "%MG_BUILD_MAPAGENT%" "%MG_SOURCE%\Web\Apache24\modules"
echo [prepare] FDO providers.xml
copy /Y "%INSTALLER_DEV%\Support\Web\%PLATFORM_CLR%\configs\FDO\providers.xml" "%MG_SOURCE%\Server\FDO\"

goto quit

:generate
echo [generate]: MapGuide Installer

SET PARAFFIN=paraffin.exe -guids
IF "%PLATFORM_CLR%" == "x64" SET PARAFFIN=paraffin.exe -Win64 -guids

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
%PARAFFIN% -dir %MG_SOURCE%\Web\Apache24 -alias $(var.MgSource)\Web\Apache24 -custom APACHEFILES -dirref WEBEXTENSIONSLOCATION -dirExclude .svn %WIX_INC_WEB%\incApacheFiles.wxs

echo [generate]: Web - Php TS
%PARAFFIN% -dir %MG_SOURCE%\Web\Php -alias $(var.MgSource)\Web\Php -custom PHPFILES -dirref WEBEXTENSIONSLOCATION %WIX_INC_WEB%\incPhpFiles.wxs

echo [generate]: Web - Tomcat
%PARAFFIN% -dir %MG_SOURCE%\Web\Tomcat -alias $(var.MgSource)\Web\Tomcat -custom TOMCATFILES -dirref WEBEXTENSIONSLOCATION %WIX_INC_WEB%\incTomcatFiles.wxs

echo [generate]: Web - Help
%PARAFFIN% -dir %MG_SOURCE%\Web\www\help -alias $(var.MgSource)\Web\www\help -custom HELPFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incHelpFiles.wxs

echo [generate]: Web - Developer's Guide Samples (PHP)
%PARAFFIN% -dir %MG_SOURCE%\Web\www\phpsamples -alias $(var.MgSource)\Web\www\phpsamples -custom PHPDEVGUIDEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incPhpDevGuideFiles.wxs

echo [generate]: Web - Developer's Guide Samples (Java)
%PARAFFIN% -dir %MG_SOURCE%\Web\www\javasamples -alias $(var.MgSource)\Web\www\javasamples -custom JAVADEVGUIDEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incJavaDevGuideFiles.wxs

echo [generate]: Web - Developer's Guide Samples (DotNet)
%PARAFFIN% -dir %MG_SOURCE%\Web\www\dotnetsamples -alias $(var.MgSource)\Web\www\dotnetsamples -custom DOTNETDEVGUIDEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incDotNetDevGuideFiles.wxs

echo [generate]: Web - Developer's Guide Samples (PHP)
%PARAFFIN% -dir %MG_SOURCE%\Web\www\phpviewersample -alias $(var.MgSource)\Web\www\phpviewersample -custom PHPVIEWERSAMPLEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incPhpViewerSampleFiles.wxs

echo [generate]: Web - Developer's Guide Samples (Java)
%PARAFFIN% -dir %MG_SOURCE%\Web\www\javaviewersample -alias $(var.MgSource)\Web\www\javaviewersample -custom JAVAVIEWERSAMPLEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incJavaViewerSampleFiles.wxs

echo [generate]: Web - Developer's Guide Samples (DotNet)
%PARAFFIN% -dir %MG_SOURCE%\Web\www\dotnetviewersample -alias $(var.MgSource)\Web\www\dotnetviewersample -custom DOTNETVIEWERSAMPLEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incDotNetViewerSampleFiles.wxs

echo [generate]: Web - OpenLayers Samples
%PARAFFIN% -dir %MG_SOURCE%\Web\www\ol2samples -alias $(var.MgSource)\Web\www\ol2samples -custom OPENLAYERSSAMPLEFILES -dirref WEBROOTLOCATION %WIX_INC_WEB%\incOpenLayersSampleFiles.wxs

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
SET RUN_BUILD=%MSBUILD_CLR% /p:OutputName=%INSTALLER_NAME%;MgCulture=%CULTURE%;MgTitle=%INSTALLER_TITLE%;MgVersion=%INSTALLER_VERSION%;MgRegKey=%MG_REG_KEY%;MgPlatform=%PLATFORM_CLR%;Have_ArcSde=%MG_WITH_ARCSDE%
if not ""=="%MG_SOURCE_INC%" set RUN_BUILD=%RUN_BUILD%;MgSource=%MG_SOURCE_INC%

echo [build]: Installer 
%RUN_BUILD% InstallerWix.sln
if "%errorlevel%"=="1" goto error
pushd "%INSTALLER_DEV_BOOTSTRAP%"
echo [bootstrap]: Copying vcredist
copy /Y vcredist_2012_%PLATFORM_CLR%.exe "%INSTALLER_OUTPUT%\vcredist_2012_%PLATFORM_CLR%.exe"
copy /Y vc_redist.%PLATFORM_CLR%.exe "%INSTALLER_OUTPUT%\vc_redist.%PLATFORM_CLR%.exe"
popd
if "%errorlevel%"=="1" goto error
if "%MAX_COMPRESSION%"=="YES" goto build_max_compress
goto build_min_compress

:build_min_compress
pushd "%INSTALLER_DEV_BOOTSTRAP%"
echo [bootstrap]: Create self-extracting package
makensis /DCPU=%PLATFORM_CLR% /DINSTALLER_ROOT=%INSTALLER_DEV% /DNSISDIR=%NSIS% /DOUTNAME=%INSTALLER_NAME% /DCULTURE=%CULTURE% /DMG_VERSION=%INSTALLER_VERSION% Setup.nsi
popd
if "%errorlevel%"=="1" goto error
echo [build]: Installer created at %INSTALLER_OUTPUT%\%INSTALLER_NAME%.exe
goto quit

:build_max_compress
pushd "%INSTALLER_DEV_BOOTSTRAP%"
echo [bootstrap]: Create self-extracting package (MAX compression)
makensis /DCPU=%PLATFORM_CLR% /DINSTALLER_ROOT=%INSTALLER_DEV% /DNSISDIR=%NSIS% /DOUTNAME=%INSTALLER_NAME% /DCULTURE=%CULTURE% /DMAXCOMPRESSION /DMG_VERSION=%INSTALLER_VERSION% Setup.nsi
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
echo           [-a=Action]
echo           [-lang=Culture]
echo           [-source=SourceDirectory]
echo	         [-version=MapGuideVersion]
echo	         [-name=MapGuideInstallerFilename]
echo	         [-title=MapGuideInstallerTitle]
echo
echo Help:	-h
echo Action: build(default), clean, prepare, generate (only use generate for creating new GIDs, or if not installing from ..\MgDev\Release)
echo SourceDirectory: The directory that the MapGuide build process installed the source files into
echo MapGuideVersion: The version associated with the installer in the format 2.1.0.0
echo MapGuideInstallerFilename: File name of output .exe in the format MapGuideOpenSource-2.1.0-Something (such as Beta, RC1, Final, etc)
echo MapGuideInstallerTitle: Title to appear in the installer.  Typically this will be set in the language file, but unofficial releases should override this
echo ************************************************************************
:quit
SET TYPEACTION=
SET INSTALLER_OUTPUT=
SET INSTALLER_DEV=
SET PATH=%OLDPATH%
