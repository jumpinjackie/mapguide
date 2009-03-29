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
rem           [-srv=ServerDirectory]
rem	      	  [-web=WebExtensionsDirectory]
rem
rem BuildType: Release(default), Debug
rem Action: build(default), clean, regen, prepare, generate (only use generate for creating new GIDs, or if not installing from c:\Staging\)
rem ServerDirectory: The MapGuide Server directory where paraffin will generate the files from
rem WebExtensionsDirectory: The MapGuide Web Extensions directory where paraffin will generate the files from

rem ==================================================
rem Top-level vars
rem ==================================================
SET OLDPATH=%PATH%
SET TYPEACTION=build
SET TYPEBUILD=Release

rem ==================================================
rem MapGuide Installer vars
rem ==================================================
SET INSTALLER_DEV=%CD%
SET INSTALLER_OUTPUT=%INSTALLER_DEV%\Output
SET INSTALLER_DEV_SUPPORT=%INSTALLER_DEV%\Support
SET INSTALLER_DEV_BOOTSTRAP=%INSTALLER_DEV%\Bootstrapper
rem Make sure this matches the output file name in MapGuide.wixproj
SET INSTALLER_NAME=MapGuideOpenSource-2.1.0-Unofficial

SET INSTALLER_DEV_CSMAP=%INSTALLER_DEV%\Libraries\CS Map
SET INSTALLER_DEV_FDO=%INSTALLER_DEV%\Libraries\FDO
SET INSTALLER_DEV_MGSERVER=%INSTALLER_DEV%\Libraries\MapGuide Server
SET INSTALLER_DEV_MGWEB=%INSTALLER_DEV%\Libraries\MapGuide Web Extensions
SET INSTALLER_DEV_INSTALLER=%INSTALLER_DEV%\Installers\MapGuide

SET MG_SERVER=%CD%\..\MgDev\%TYPEBUILD%\Server\
SET MG_WEB=%CD%\..\MgDev\%TYPEBUILD%\WebServerExtensions\

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
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%

:study_params
if (%1)==() goto start_build

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-c"       goto get_conf
if "%1"=="-config"  goto get_conf

if "%1"=="-a"       goto get_action
if "%1"=="-action"  goto get_action

if "%1"=="-v"       goto get_verbose
if "%1"=="-srv"		 goto get_server
if "%1"=="-web"		 goto get_webextensions

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

:get_server
SET MG_SERVER=%2
goto next_param

:get_webextensions
SET MG_WEB=%2
goto next_param

:get_conf
SET TYPEBUILD=%2
SET INSTALLER_OUTPUT=%CD%\Installers\MapGuide\bin\%TYPEBUILD%
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%
SET MG_SERVER=..\MgDev\%TYPEBUILD%\Server\
SET MG_WEB=..\MgDev\%TYPEBUILD%\WebServerExtensions\

if "%2"=="Release" goto next_param
if "%2"=="Debug" goto next_param
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
echo MG Server Source Directory: %MG_SERVER%
echo MG Web Source Directory: %MG_WEB%
echo ===================================================

if "%TYPEACTION%"=="build" goto build
if "%TYPEACTION%"=="prepare" goto prepare
if "%TYPEACTION%"=="clean" goto clean
if "%TYPEACTION%"=="generate" goto generate
if "%TYPEACTION%"=="regen" goto regen

:clean
echo [clean]: CS-Map
pushd "%INSTALLER_DEV_CSMAP%"
%MSBUILD_CLEAN% "CS Map.wixproj"
popd
echo [clean]: FDO
pushd "%INSTALLER_DEV_FDO%"
%MSBUILD_CLEAN% "FDO.wixproj"
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
if not exist "%MG_SERVER%" goto error_mg_server_not_found
if not exist "%MG_WEB%" goto error_mg_web_not_found
rem copy support files into server and web directories
echo [prepare] Tomcat
%XCOPY% "%INSTALLER_DEV%\Support\Web\Tomcat" "%MG_WEB%\Tomcat" /EXCLUDE:svn_excludes.txt
echo [prepare] Php
%XCOPY% "%INSTALLER_DEV%\Support\Web\Php" "%MG_WEB%\Php" /EXCLUDE:svn_excludes.txt
echo [prepare] Apache2
%XCOPY% "%INSTALLER_DEV%\Support\Web\Apache2" "%MG_WEB%\Apache2" /EXCLUDE:svn_excludes.txt

goto quit

:regen
echo [regen]: MapGuide Installer

SET PARAFFIN=paraffin.exe -update 

SET WIX_INC_SERVER="%INSTALLER_DEV%\Libraries\MapGuide Server\FileIncludes"
SET WIX_INC_WEB="%INSTALLER_DEV%\Libraries\MapGuide Web Extensions\FileIncludes"
SET WIX_INC_FDO="%INSTALLER_DEV%\Libraries\FDO\FileIncludes"
SET WIX_INC_CSMAP="%INSTALLER_DEV%\Libraries\CS Map\FileIncludes"

echo [regen]: Server - binaries
%PARAFFIN% %WIX_INC_SERVER%\incBinFiles.wxs
move /Y %WIX_INC_SERVER%\incBinFiles.PARAFFIN %WIX_INC_SERVER%\incBinFiles.wxs

echo [regen]: Server - FDO
%PARAFFIN% %WIX_INC_FDO%\incFdoFiles.wxs
move /Y %WIX_INC_FDO%\incFdoFiles.PARAFFIN %WIX_INC_FDO%\incFdoFiles.wxs

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

echo [regen]: Server - CS-Map dictionaries
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
SET WIX_INC_FDO="%INSTALLER_DEV%\Libraries\FDO\FileIncludes"
SET WIX_INC_CSMAP="%INSTALLER_DEV%\Libraries\CS Map\FileIncludes"

echo [generate]: Server - binaries
%PARAFFIN% -dir %MG_SERVER%\bin -alias $(env.MG_SERVER)\bin -custom SRVBINFILES -ext pdb -dirref SERVERROOTLOCATION -norecurse %WIX_INC_SERVER%\incBinFiles.wxs

echo [generate]: Server - FDO
%PARAFFIN% -dir %MG_SERVER%\bin\FDO -alias $(env.MG_SERVER)\bin\FDO -custom FDOFILES -dirref SERVERROOTLOCATION %WIX_INC_FDO%\incFdoFiles.wxs

echo [generate]: Server - RepositoryAdmin
%PARAFFIN% -dir %MG_SERVER%\RepositoryAdmin -alias $(env.MG_SERVER)\RepositoryAdmin -custom SRVREPADMINFILES -dirref SERVERROOTLOCATION %WIX_INC_SERVER%\incRepositoryAdminFiles.wxs

echo [generate]: Server - resources          
%PARAFFIN% -dir %MG_SERVER%\bin\Resources -alias $(env.MG_SERVER)\bin\Resources -custom SRVRESOURCESFILES -dirref SERVERROOTLOCATION %WIX_INC_SERVER%\incResourcesFiles.wxs
	  
echo [generate]: Server - schema
%PARAFFIN% -dir %MG_SERVER%\bin\Schema -alias $(env.MG_SERVER)\bin\Schema -custom SRVSCHEMAFILES -dirref SERVERROOTLOCATION %WIX_INC_SERVER%\incSchemaFiles.wxs

echo [generate]: Server - WMS
%PARAFFIN% -dir %MG_SERVER%\bin\wms -alias $(env.MG_SERVER)\bin\wms -custom SRVWMSFILES -dirref SERVERROOTLOCATION -norecurse %WIX_INC_SERVER%\incWmsFiles.wxs

echo [generate]: Server - WFS
%PARAFFIN% -dir %MG_SERVER%\bin\wfs -alias $(env.MG_SERVER)\bin\wfs -custom SRVWFSFILES -dirref SERVERROOTLOCATION -norecurse %WIX_INC_SERVER%\incWfsFiles.wxs

echo [generate]: Server - CS-Map dictionaries
%PARAFFIN% -dir %MG_SERVER%\CsMap\Dictionaries -alias $(env.MG_SERVER)\CsMap\Dictionaries -custom CSMAPDICTFILES -dirref CSMAPLOCATION -ext ASC -ext C -ext CNT -ext GID -ext HLP -ext MAK -ext NMK -ext VCPROJ -ext USER %WIX_INC_CSMAP%\incCSMapDictionaryFiles.wxs

echo [generate]: Web - Apache
%PARAFFIN% -dir %MG_WEB%\Apache2 -alias $(env.MG_WEB)\Apache2 -custom APACHEFILES -dirref WEBEXTENSIONSLOCATION -multiple %WIX_INC_WEB%\incApacheFiles.wxs

echo [generate]: Web - Php
%PARAFFIN% -dir %MG_WEB%\Php -alias $(env.MG_WEB)\Php -custom PHPFILES -dirref WEBEXTENSIONSLOCATION -multiple %WIX_INC_WEB%\incPhpFiles.wxs

echo [generate]: Web - Tomcat
%PARAFFIN% -dir %MG_WEB%\Tomcat -alias $(env.MG_WEB)\Tomcat -custom TOMCATFILES -dirref WEBEXTENSIONSLOCATION -multiple %WIX_INC_WEB%\incTomcatFiles.wxs

echo [generate]: Web - Help
%PARAFFIN% -dir %MG_WEB%\Help -alias $(env.MG_WEB)\Help -custom HELPFILES -dirref WEBEXTENSIONSLOCATION -multiple %WIX_INC_WEB%\incHelpFiles.wxs

echo [generate]: Web - Developer's Guide Samples
%PARAFFIN% -dir %MG_WEB%\www\devguide -alias $(env.MG_WEB)\www\devguide -custom DEVGUIDEFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incDevGuideFiles.wxs

echo [generate]: Web - mapagent
%PARAFFIN% -dir %MG_WEB%\www\mapagent -alias $(env.MG_WEB)\www\mapagent -ext pdb -custom MAPAGENTFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapAgentFiles.wxs

echo [generate]: Web - mapviewernet
%PARAFFIN% -dir %MG_WEB%\www\mapviewernet -alias $(env.MG_WEB)\www\mapviewernet -custom MAPVIEWERASPXFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerAspxFiles.wxs

echo [generate]: Web - mapviewerphp
%PARAFFIN% -dir %MG_WEB%\www\mapviewerphp -alias $(env.MG_WEB)\www\mapviewerphp -custom MAPVIEWERPHPFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerPhpFiles.wxs

echo [generate]: Web - mapviewerjava
%PARAFFIN% -dir %MG_WEB%\www\mapviewerjava -alias $(env.MG_WEB)\www\mapviewerjava -custom MAPVIEWERJSPFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerJspFiles.wxs
         
echo [generate]: Web - fusion
%PARAFFIN% -dir %MG_WEB%\www\fusion -alias $(env.MG_WEB)\www\fusion -custom FUSIONFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incFusionFiles.wxs

echo [generate]: Web - misc web root
%PARAFFIN% -dir %MG_WEB%\www -alias $(env.MG_WEB)\www -custom WEBROOTFILES -dirref WEBEXTENSIONSLOCATION -multiple -norecurse %WIX_INC_WEB%\incWebRootFiles.wxs
%PARAFFIN% -dir %MG_WEB%\www\viewerfiles -alias $(env.MG_WEB)\www\viewerfiles -custom MAPVIEWERFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerFiles.wxs 
%PARAFFIN% -dir %MG_WEB%\www\stdicons -alias $(env.MG_WEB)\www\stdicons -custom MAPVIEWERSTDICONFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerStdiconFiles.wxs 
%PARAFFIN% -dir %MG_WEB%\www\schemareport -alias $(env.MG_WEB)\www\schemareport -custom MAPVIEWERSCHEMAREPORTFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerSchemareportFiles.wxs 
%PARAFFIN% -dir %MG_WEB%\www\mapadmin -alias $(env.MG_WEB)\www\mapadmin -custom MAPVIEWERMAPADMINFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerMapAdminFiles.wxs 
%PARAFFIN% -dir %MG_WEB%\www\localized -alias $(env.MG_WEB)\www\localized -custom MAPVIEWERLOCALIZEDFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incMapViewerLocalizedFiles.wxs 
%PARAFFIN% -dir %MG_WEB%\www\WEB-INF -alias $(env.MG_WEB)\www\WEB-INF -custom WEBINFFILES -dirref WEBROOTLOCATION -multiple %WIX_INC_WEB%\incWebInfFiles.wxs

goto quit

:build
echo [build]: Installer
%MSBUILD% Installer.sln
if "%errorlevel%"=="1" goto error
pushd "%INSTALLER_DEV_BOOTSTRAP%"
echo [bootstrap]: Creating
%MSBUILD% /p:TargetFile=%INSTALLER_NAME%.msi Bootstrap.proj
if "%errorlevel%"=="1" goto error
echo [bootstrap]: Create self-extracting package
makensis /DINSTALLER_ROOT=%INSTALLER_DEV% /DNSISDIR=%NSIS% /DOUTNAME=%INSTALLER_NAME% Setup.nsi
if "%errorlevel%"=="1" goto error
popd
echo [build]: Installer created at %INSTALLER_OUTPUT%\%INSTALLER_NAME%.exe

goto quit

:error_mg_server_not_found
echo [ERROR]: Could not find MapGuide Server directory at %MG_SERVER%
exit /B 1

:error_mg_web_not_found
echo [ERROR]: Could not find MapGuide Web Extensions directory at %MG_WEB%
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
echo           [-srv=ServerDirectory]
echo           [-web=WebExtensionsDirectory]
echo Help:	-h
echo Verbose: -v
echo BuildType: Release(default), Debug
echo Action: build(default), clean, regen, prepare, generate  (only use generate for new full reinstall releases)
echo ServerDirectory: The MapGuide Server directory where paraffin will generate the files from
echo WebExtensionsDirectory: The MapGuide Web Extensions directory where paraffin will generate the files from
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
