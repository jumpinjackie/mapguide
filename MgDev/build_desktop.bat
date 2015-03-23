@echo off
rem Top-level build script for mg-desktop
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem You must call setenvironment[64].bat first with any appropriate
rem parameters before calling this script
rem
rem This script presumes common/oem components have already been built
rem so you should ideally run this after running build.bat

SET MG_OUTPUT_DESKTOP=%MG_OUTPUT%\Desktop
SET MG_OUTPUT_SAMPLES=%MG_OUTPUT%\DesktopSamples

echo ===================================================
echo Configuration is [%TYPEBUILD%, "%CONFIGURATION%|%PLATFORM%"]
echo Output Directory for mg-desktop: %MG_OUTPUT_DESKTOP%
echo Output Directory for samples: %MG_OUTPUT_SAMPLES%
echo CPU cores: %CPU_CORES%
echo ===================================================

pushd Desktop
echo [build]: Desktop API (%PLATFORM%, %CONFIGURATION%)
%MSBUILD% /p:Platform=%PLATFORM% /p:Configuration=%CONFIGURATION% MgDesktopApi.sln
if "%errorlevel%"=="1" goto error
echo [build]: .net components (%PLATFORM%)
%MSBUILD% /p:Platform=%PLATFORM_CLR% /p:Configuration=%CONFIGURATION% MgDesktopDotNet.sln
if "%errorlevel%"=="1" goto error
popd
echo [install]: binaries
%XCOPY% "Desktop\bin\%TYPEBUILD%" "%MG_OUTPUT_DESKTOP%" /EXCLUDE:svn_excludes.txt+%CONFIGURATION%_excludes.txt
pushd "%MG_OUTPUT_DESKTOP%"
REM Remove WebTier stuff that has no place in mg-desktop
if exist MgMapGuideCommon.dll del MgMapGuideCommon.dll
if exist OSGeo.MapGuide.MapGuideCommon.xml del OSGeo.MapGuide.MapGuideCommon.xml
if exist OSGeo.MapGuide.Web.xml del OSGeo.MapGuide.Web.xml
if exist Backup rd /S /Q Backup
popd
echo [install]: Samples
pushd Desktop
%XCOPY% Samples "%MG_OUTPUT_SAMPLES%" /EXCLUDE:samples_exclude.txt
popd
goto quit

:error
echo [ERROR]: There was an error building the component
exit /B 1

:quit