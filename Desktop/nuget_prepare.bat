@echo off
rem Top-level build script for mg-desktop
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem You must call setenvironment[64].bat first with any appropriate
rem parameters before calling this script

SET NUGET_TARGET=net40
SET NUGET_CONTENT_BASE=%CD%\nuget\api-base\content\%NUGET_TARGET%\mapguide-api-base
SET NUGET_LIB_BASE=%CD%\nuget\api-base\lib\%NUGET_TARGET%
SET NUGET_CONTENT_DESKTOP=%CD%\nuget\api-desktop\content\%NUGET_TARGET%\mg-desktop
SET NUGET_LIB_DESKTOP=%CD%\nuget\api-desktop\lib\%NUGET_TARGET%
SET NUGET_CONTENT_VIEWER=%CD%\nuget\api-desktop-viewer\content\%NUGET_TARGET%\mg-desktop-viewer
SET NUGET_LIB_VIEWER=%CD%\nuget\api-desktop-viewer\lib\%NUGET_TARGET%

echo [install]: Copy api-base files to nuget staging
%XCOPY% "bin\%TYPEBUILD%\*" "%NUGET_CONTENT_BASE%" /EXCLUDE:package_excludes.txt+base_package_excludes.txt
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.Foundation.dll" "%NUGET_LIB_BASE%"
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.Geometry.dll" "%NUGET_LIB_BASE%"
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.PlatformBase.dll" "%NUGET_LIB_BASE%"
echo [install]: Copy api-desktop files to nuget staging
%XCOPY% "bin\%TYPEBUILD%\*" "%NUGET_CONTENT_DESKTOP%" /EXCLUDE:package_excludes.txt+desktop_package_excludes.txt
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.Desktop.dll" "%NUGET_LIB_DESKTOP%"
echo [install]: Copy api-desktop-viewer files to nuget staging
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.Viewer.xml" "%NUGET_LIB_VIEWER%"
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.Viewer.dll" "%NUGET_LIB_VIEWER%"
copy /y "bin\%TYPEBUILD%\OSGeo.MapGuide.Viewer.Desktop.dll" "%NUGET_LIB_VIEWER%"
echo mg-desktop is ready for nuget packaging and publishing
goto quit


:error
echo [ERROR]: There was an error building the component
exit /B 1

:quit