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

pushd Desktop
echo [build]: Desktop API
%MSBUILD% MgDesktopApi%VS_SLN_SUFFIX%.sln
echo [build]: .net components
%MSBUILD% MgDesktopDotNet%VS_SLN_SUFFIX%.sln
popd
echo [install]: binaries
%XCOPY% "Desktop\bin\%TYPEBUILD%" "%MG_OUTPUT_DESKTOP%" /EXCLUDE:svn_excludes.txt+%CONFIGURATION%_excludes.txt