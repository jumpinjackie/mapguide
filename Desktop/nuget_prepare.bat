@echo off
rem Top-level build script for mg-desktop
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem You must call setenvironment[64].bat first with any appropriate
rem parameters before calling this script

SET NUGET_BASEDIR=%CD%\nuget
SET NUGET_TARGET=net40
SET NUGET_CONTENT_BASE=%NUGET_BASEDIR%\api-base\content\%NUGET_TARGET%\mapguide-api-base
SET NUGET_LIB_BASE=%NUGET_BASEDIR%\api-base\lib\%NUGET_TARGET%
SET NUGET_CONTENT_DESKTOP=%NUGET_BASEDIR%\api-desktop\content\%NUGET_TARGET%\mg-desktop
SET NUGET_LIB_DESKTOP=%NUGET_BASEDIR%\api-desktop\lib\%NUGET_TARGET%
SET NUGET_CONTENT_VIEWER=%NUGET_BASEDIR%\api-desktop-viewer\content\%NUGET_TARGET%\mg-desktop-viewer
SET NUGET_LIB_VIEWER=%NUGET_BASEDIR%\api-desktop-viewer\lib\%NUGET_TARGET%
SET NUGET_CONTENT_WEB=%NUGET_BASEDIR%\api-web\content\%NUGET_TARGET%\mapguide-api-web
SET NUGET_LIB_WEB=%NUGET_BASEDIR%\api-web\lib\%NUGET_TARGET%
SET WEB_BASEDIR=%CD%\..\Web\bin\%TYPEBUILD%
SET DESKTOP_BASEDIR=bin\%TYPEBUILD%
SET SIGNER_ROOT=%CD%\DesktopUnmanagedApi\DotNet

echo [install]: Copy api-base files to nuget staging
%XCOPY% "%DESKTOP_BASEDIR%\*" "%NUGET_CONTENT_BASE%" /EXCLUDE:package_excludes.txt+base_package_excludes.txt
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Foundation.dll" "%NUGET_LIB_BASE%"
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Geometry.dll" "%NUGET_LIB_BASE%"
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.PlatformBase.dll" "%NUGET_LIB_BASE%"
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Foundation.xml" "%NUGET_LIB_BASE%"
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Geometry.xml" "%NUGET_LIB_BASE%"
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.PlatformBase.xml" "%NUGET_LIB_BASE%"
echo [install]: Copy api-desktop files to nuget staging
%XCOPY% "%DESKTOP_BASEDIR%\*" "%NUGET_CONTENT_DESKTOP%" /EXCLUDE:package_excludes.txt+desktop_package_excludes.txt
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Desktop.dll" "%NUGET_LIB_DESKTOP%"
echo [install]: Copy api-web files to nuget staging
copy /y "%WEB_BASEDIR%\MgWebApp.dll" "%NUGET_CONTENT_WEB%"
copy /y "%WEB_BASEDIR%\MgWebSupport.dll" "%NUGET_CONTENT_WEB%"
copy /y "%WEB_BASEDIR%\MgMapGuideCommon.dll" "%NUGET_CONTENT_WEB%"
copy /y "%WEB_BASEDIR%\MapGuideCommonUnmanagedApi.dll" "%NUGET_CONTENT_WEB%"
copy /y "%WEB_BASEDIR%\lib_json.dll" "%NUGET_CONTENT_WEB%"
copy /y "%WEB_BASEDIR%\WebUnmanagedApi.dll" "%NUGET_CONTENT_WEB%"
copy /y "%WEB_BASEDIR%\OSGeo.MapGuide.MapGuideCommon.dll" "%NUGET_LIB_WEB%"
copy /y "%WEB_BASEDIR%\OSGeo.MapGuide.Web.dll" "%NUGET_LIB_WEB%"
copy /y "%WEB_BASEDIR%\OSGeo.MapGuide.MapGuideCommon.xml" "%NUGET_LIB_WEB%"
copy /y "%WEB_BASEDIR%\OSGeo.MapGuide.Web.xml" "%NUGET_LIB_WEB%"
copy /y "%SIGNER_ROOT%\SignMapGuideApi.exe" "%NUGET_LIB_WEB%"
copy /y "%SIGNER_ROOT%\maestroapi.key" "%NUGET_LIB_WEB%"
echo [build]: Signing MapGuide API assemblies
pushd "%NUGET_LIB_WEB%"
SignMapGuideApi.exe
rd /S /Q Backup
del maestroapi.key
del SignMapGuideApi.exe
popd
echo [install]: Copy api-desktop-viewer files to nuget staging
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Viewer.xml" "%NUGET_LIB_VIEWER%"
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Viewer.dll" "%NUGET_LIB_VIEWER%"
copy /y "%DESKTOP_BASEDIR%\OSGeo.MapGuide.Viewer.Desktop.dll" "%NUGET_LIB_VIEWER%"
echo mg-desktop is ready for nuget packaging and publishing
goto quit


:error
echo [ERROR]: There was an error building the component
exit /B 1

:quit