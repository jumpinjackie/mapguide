@echo off
SET SLN_DIR=%~dp0
SET CFG=%1
SET CFG=%CFG:"=%
SET PLAT=%2
SET PLAT=%PLAT:"=%
echo Solution Dir: %SLN_DIR%
echo Configuration: %CFG%
echo Platform: %PLAT%
if not exist "%SLN_DIR%Libs\" (
    echo Libs dir does not exist. Creating it
    pushd %SLN_DIR%
    mkdir Libs
    popd
    goto copy
)
if not exist "%SLN_DIR%Libs\SqliteDotNet.dll" goto copy
if not exist "%SLN_DIR%Libs\OSGeo.MapGuide.Foundation.dll" goto copy
if not exist "%SLN_DIR%Libs\OSGeo.MapGuide.Geometry.dll" goto copy
if not exist "%SLN_DIR%Libs\OSGeo.MapGuide.PlatformBase.dll" goto copy
if not exist "%SLN_DIR%Libs\OSGeo.MapGuide.MapGuideCommon.dll" goto copy
if not exist "%SLN_DIR%Libs\OSGeo.MapGuide.Web.dll" goto copy
echo Files already exist. Nothing to do
goto done

:copy
if "%PLAT%"=="x64" (
    echo Copying x64 binaries from %SLN_DIR%..\..\..\Web\bin\%CFG%64
    copy /Y "%SLN_DIR%..\..\..\Web\bin\%CFG%64\*.*" "%SLN_DIR%Libs\"
    echo Copying x64 SqliteDotNet.dll from %SLN_DIR%..\..\..\Oem\SQLite\bin\%CFG%64\SqliteDotNet.dll
    copy /Y "%SLN_DIR%..\..\..\Oem\SQLite\bin\%CFG%64\SqliteDotNet.dll" "%SLN_DIR%Libs\"
) else (
    echo Copying x86 binaries from %SLN_DIR%..\..\..\Web\bin\%CFG%
    copy /Y "%SLN_DIR%..\..\..\Web\bin\%CFG%\*.*" "%SLN_DIR%Libs\"
    echo Copying x86 SqliteDotNet.dll
    copy /Y "%SLN_DIR%..\..\..\Oem\SQLite\bin\%CFG%\SqliteDotNet.dll" "%SLN_DIR%Libs\"
)
:done