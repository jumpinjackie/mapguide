@echo off
rem Top-level build script for mg-desktop
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem You must call setenvironment[64].bat first with any appropriate
rem parameters before calling this script
SET MG_VERSION=2.4.0.0
SET MG_CPU=x86
IF NOT "%1" == "" SET MG_VERSION=%1
IF NOT "%2" == "" SET MG_CPU=%2

echo Version: %MG_VERSION%
echo CPU: %MG_CPU%

IF "%MG_CPU%" == "x64" (
    SET TYPEBUILD=release64
) ELSE (
    SET TYPEBUILD=release
)

SET NUGET_ROOT=%CD%\nuget
SET NUGET_BASEDIR=%NUGET_ROOT%\%MG_CPU%
SET NUGET_TARGET=net40
SET NUGET_BASE_ROOT=%NUGET_BASEDIR%\api-base
SET NUGET_CONTENT_BASE=%NUGET_BASE_ROOT%\mapguide-api-base
SET NUGET_LIB_BASE=%NUGET_BASE_ROOT%\lib\%NUGET_TARGET%
SET NUGET_DESKTOP_ROOT=%NUGET_BASEDIR%\api-desktop
SET NUGET_CONTENT_DESKTOP=%NUGET_DESKTOP_ROOT%\mg-desktop
SET NUGET_LIB_DESKTOP=%NUGET_DESKTOP_ROOT%\lib\%NUGET_TARGET%
SET NUGET_VIEWER_ROOT=%NUGET_BASEDIR%\api-desktop-viewer
SET NUGET_CONTENT_VIEWER=%NUGET_VIEWER_ROOT%\mg-desktop-viewer
SET NUGET_LIB_VIEWER=%NUGET_VIEWER_ROOT%\lib\%NUGET_TARGET%
SET NUGET_WEB_ROOT=%NUGET_BASEDIR%\api-web
SET NUGET_CONTENT_WEB=%NUGET_WEB_ROOT%\mapguide-api-web
SET NUGET_LIB_WEB=%NUGET_WEB_ROOT%\lib\%NUGET_TARGET%
SET NUGET_CSMAP_ROOT=%NUGET_ROOT%\cs-map-dicts
SET NUGET_CONTENT_CSMAP=%NUGET_CSMAP_ROOT%\dictionaries
SET WEB_BASEDIR=%CD%\Web\bin\%TYPEBUILD%
SET DESKTOP_SRC=%CD%\Desktop\MgDesktop
SET DESKTOP_BASEDIR=%CD%\%TYPEBUILD%\Desktop
SET SIGNER_ROOT=%CD%\Desktop\DesktopUnmanagedApi\DotNet

IF NOT EXIST "%DESKTOP_BASEDIR%" (
    echo mg-desktop base directory not found: %DESKTOP_BASEDIR%
    goto error
) ELSE (
    echo mg-desktop base directory: %DESKTOP_BASEDIR%
)
IF NOT EXIST "%WEB_BASEDIR%" (
    echo MapGuide Web base directory not found: %WEB_BASEDIR%
    goto error
) ELSE (
    echo MapGuide Web base directory: %WEB_BASEDIR%
)
IF NOT EXIST "%DESKTOP_BASEDIR%\Dictionaries" (
    echo CS-Map dictionaries directory not found: "%DESKTOP_BASEDIR%\Dictionaries"
    goto error
) ELSE (
    echo CS-Map dictionaries directory: "%DESKTOP_BASEDIR%\Dictionaries"
)
IF NOT EXIST "%SIGNER_ROOT%" (
    echo Signer root directory not found: %SIGNER_ROOT%
    goto error
) ELSE (
    echo Signer root directory: %SIGNER_ROOT%
)

IF NOT EXIST "%NUGET_CONTENT_BASE%" MKDIR "%NUGET_CONTENT_BASE%"
IF NOT EXIST "%NUGET_CONTENT_DESKTOP%" MKDIR "%NUGET_CONTENT_DESKTOP%"
IF NOT EXIST "%NUGET_CONTENT_WEB%" MKDIR "%NUGET_CONTENT_WEB%"
IF NOT EXIST "%NUGET_CONTENT_CSMAP%" MKDIR "%NUGET_CONTENT_CSMAP%"

echo [install]: Copy api-base files to nuget staging
xcopy /S /Y "%DESKTOP_BASEDIR%\*" "%NUGET_CONTENT_BASE%" /EXCLUDE:package_excludes.txt+base_package_excludes.txt
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Foundation.dll" "%NUGET_LIB_BASE%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Geometry.dll" "%NUGET_LIB_BASE%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.PlatformBase.dll" "%NUGET_LIB_BASE%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Foundation.xml" "%NUGET_LIB_BASE%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Geometry.xml" "%NUGET_LIB_BASE%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.PlatformBase.xml" "%NUGET_LIB_BASE%"
echo [install]: Copy api-desktop files to nuget staging
xcopy /S /Y "%DESKTOP_BASEDIR%\*" "%NUGET_CONTENT_DESKTOP%" /EXCLUDE:package_excludes.txt+desktop_package_excludes.txt
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Desktop.dll" "%NUGET_LIB_DESKTOP%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Desktop.xml" "%NUGET_LIB_DESKTOP%"
echo [install]: Copy api-web files to nuget staging
xcopy /Y /I "%WEB_BASEDIR%\MgWebApp.dll" "%NUGET_CONTENT_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\MgWebSupport.dll" "%NUGET_CONTENT_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\MgHttpHandler.dll" "%NUGET_CONTENT_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\MgMapGuideCommon.dll" "%NUGET_CONTENT_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\MapGuideCommonUnmanagedApi.dll" "%NUGET_CONTENT_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\lib_json.dll" "%NUGET_CONTENT_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\WebUnmanagedApi.dll" "%NUGET_CONTENT_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\OSGeo.MapGuide.MapGuideCommon.dll" "%NUGET_LIB_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\OSGeo.MapGuide.Web.dll" "%NUGET_LIB_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\OSGeo.MapGuide.MapGuideCommon.xml" "%NUGET_LIB_WEB%"
xcopy /Y /I "%WEB_BASEDIR%\OSGeo.MapGuide.Web.xml" "%NUGET_LIB_WEB%"
xcopy /Y /I "%SIGNER_ROOT%\SignMapGuideApi.exe" "%NUGET_LIB_WEB%"
xcopy /Y /I "%SIGNER_ROOT%\maestroapi.key" "%NUGET_LIB_WEB%"
echo [install]: Copy cs-map dictionaries to nuget staging
xcopy /S /Y "%DESKTOP_BASEDIR%\Dictionaries\*" "%NUGET_CONTENT_CSMAP%"
echo [build]: Signing MapGuide API assemblies
pushd "%NUGET_LIB_WEB%"
SignMapGuideApi.exe
rd /S /Q Backup
del maestroapi.key
del SignMapGuideApi.exe
popd
echo [install]: Copy api-desktop-viewer files to nuget staging
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Viewer.xml" "%NUGET_LIB_VIEWER%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Viewer.dll" "%NUGET_LIB_VIEWER%"
xcopy /Y /I "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Viewer.Desktop.dll" "%NUGET_LIB_VIEWER%"
echo [prepare]: Nuspec files for %MG_VERSION% (%MG_CPU%)
REM prep nuspec files
pushd "%NUGET_ROOT%"
echo [prepare]: mapguide-api-base
call BatchSubstitute.bat MG_CPU %MG_CPU% mapguide-api-base.nuspec.tpl > temp.txt
call BatchSubstitute.bat MG_VERSION %MG_VERSION% temp.txt > mapguide-api-base-%MG_CPU%.nuspec
echo [prepare]: mapguide-api-web
call BatchSubstitute.bat MG_CPU %MG_CPU% mapguide-api-web.nuspec.tpl > temp.txt
call BatchSubstitute.bat MG_VERSION %MG_VERSION% temp.txt > mapguide-api-web-%MG_CPU%.nuspec
echo [prepare]: mg-desktop-net40
call BatchSubstitute.bat MG_CPU %MG_CPU% mg-desktop-net40.nuspec.tpl > temp.txt
call BatchSubstitute.bat MG_VERSION %MG_VERSION% temp.txt > mg-desktop-net40-%MG_CPU%.nuspec
echo [prepare]: mg-desktop-viewer-net40
call BatchSubstitute.bat MG_CPU %MG_CPU% mg-desktop-viewer-net40.nuspec.tpl > temp.txt
call BatchSubstitute.bat MG_VERSION %MG_VERSION% temp.txt > mg-desktop-viewer-net40-%MG_CPU%.nuspec
echo [prepare]: cs-map-dictionaries
call BatchSubstitute.bat MG_CPU %MG_CPU% cs-map-dictionaries.nuspec.tpl > temp.txt
call BatchSubstitute.bat MG_VERSION %MG_VERSION% temp.txt > cs-map-dictionaries.nuspec
del temp.txt
echo [prepare]: NuGet Package readmes
type api-base-readme.txt > "%NUGET_BASE_ROOT%\readme.txt"
type api-desktop-readme.txt > "%NUGET_DESKTOP_ROOT%\readme.txt"
type "%DESKTOP_SRC%\changelog.txt" >> "%NUGET_DESKTOP_ROOT%\readme.txt"
type api-desktop-viewer-readme.txt > "%NUGET_VIEWER_ROOT%\readme.txt"
type "%DESKTOP_SRC%\changelog.txt" >> "%NUGET_VIEWER_ROOT%\readme.txt"
type api-web-readme.txt > "%NUGET_WEB_ROOT%\readme.txt"
type cs-map-dict-readme.txt > "%NUGET_CSMAP_ROOT%\readme.txt"
popd
echo mg-desktop is ready for nuget packaging and publishing
goto quit

:error
echo [ERROR]: There was an error building the component
exit /B 1

:quit