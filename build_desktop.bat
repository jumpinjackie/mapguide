@echo off
rem Top-level build script for mg-desktop
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem You must call setenvironment[64].bat first with any appropriate
rem parameters before calling this script

rem ==================================================
rem Update solution suffix if using VC10 compiler.
rem NOTE: VS10 solution files are suffixed with _VS2010
rem which is why we can do it like this
rem ==================================================

SET MG_OUTPUT_DESKTOP=%MG_OUTPUT%\Desktop
SET VS_SLN_SUFFIX=
IF "%VC_COMPILER_VERSION%" == "10" SET VS_SLN_SUFFIX=_VS2010

SET DESKTOP_PLATFORM_UNMANAGED=Win32
SET DESKTOP_PLATFORM=%1
IF "%DESKTOP_PLATFORM%" == "" SET DESKTOP_PLATFORM=x86
IF "%DESKTOP_PLATFORM%" == "x64" SET DESKTOP_PLATFORM_UNMANAGED=x64

pushd Desktop
echo [build]: Desktop API (%DESKTOP_PLATFORM_UNMANAGED%)
%MSBUILD% /p:Platform=%DESKTOP_PLATFORM_UNMANAGED% MgDesktopApi%VS_SLN_SUFFIX%.sln
if "%errorlevel%"=="1" goto error
echo [build]: .net components (%DESKTOP_PLATFORM%)
%MSBUILD% /p:Platform=%DESKTOP_PLATFORM% MgDesktopDotNet%VS_SLN_SUFFIX%.sln
if "%errorlevel%"=="1" goto error
popd
echo [install]: binaries
%XCOPY% "Desktop\bin\%TYPEBUILD%" "%MG_OUTPUT_DESKTOP%" /EXCLUDE:svn_excludes.txt+%CONFIGURATION%_excludes.txt
goto quit

:error
echo [ERROR]: There was an error building the component
exit /B 1

:quit