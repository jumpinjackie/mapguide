@echo off
REM ----------------
REM For Windows only
REM ----------------

REM **** Do not edit these unless you know what you are doing *****
set WEB_BIN_DEBUG=..\..\bin\debug
set WEB_BIN_RELEASE=..\..\bin\release

set WEB_ROOT=..\..

set COMMON_ROOT=..\..\..\Common

set COMMON_BIN_DEBUG=..\..\..\Common\bin\debug
set COMMON_BIN_RELEASE=..\..\..\Common\bin\release

REM **** Edit this to point to your PHP Extension directory *****
set PHP_EXT_DIR=C:\PHP\ext

if "%1"=="BUILD" goto copy_build
if "%1"=="RELEASE" goto copy_release
if "%1"=="RELEASE64" goto copy_release64
if "%1"=="DEBUG" goto copy_debug

REM ---- DEFAULT -----
:default
echo Need to use RELEASE, DEBUG or BUILD arguments
goto end_copy

REM ---- RELEASE -----
REM This assumes all DLLs are copied to the release folder
REM and that this batch file is being run in the MapGuideApi folder

:copy_release
if not exist %WEB_BIN_RELEASE% mkdir %WEB_BIN_RELEASE%

echo Copying ACE Release DLL to %WEB_BIN_RELEASE%
copy "..\..\..\Oem\ACE\ACE_wrappers\lib\ACE.dll" %WEB_BIN_RELEASE%

echo Copying JSON Release DLL to %WEB_BIN_RELEASE%
copy "..\..\..\Oem\jsoncpp\lib\lib_json.dll" %WEB_BIN_RELEASE%

echo Copying MgFoundation Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgFoundation.dll %WEB_BIN_RELEASE%

echo Copying MgGeometry Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgGeometry.dll %WEB_BIN_RELEASE%

echo Copying MgPlatformBase Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgPlatformBase.dll %WEB_BIN_RELEASE%

echo Copying MgMapGuideCommon Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgMapGuideCommon.dll %WEB_BIN_RELEASE%

echo Copying MgMdfModel Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgMdfModel.dll %WEB_BIN_RELEASE%

echo Copying MgMdfParser Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgMdfParser.dll %WEB_BIN_RELEASE%

echo Copying Xerces Release DLL to %WEB_BIN_RELEASE%
copy "..\..\..\Oem\dbxml-2.3.10\bin\xerces-c_2_7.dll" %WEB_BIN_RELEASE%

goto end_copy

:copy_release64
if not exist %WEB_BIN_RELEASE%64 mkdir %WEB_BIN_RELEASE%64

echo Copying ACE Release DLL to %WEB_BIN_RELEASE%64
copy "..\..\..\Oem\ACE\ACE_wrappers\lib64\ACE.dll" %WEB_BIN_RELEASE%64

echo Copying JSON Release DLL to %WEB_BIN_RELEASE%64
copy "..\..\..\Oem\jsoncpp\lib64\lib_json.dll" %WEB_BIN_RELEASE%64

echo Copying MgFoundation Release DLL to %WEB_BIN_RELEASE%64
copy %COMMON_BIN_RELEASE%64\MgFoundation.dll %WEB_BIN_RELEASE%64

echo Copying MgGeometry Release DLL to %WEB_BIN_RELEASE%64
copy %COMMON_BIN_RELEASE%64\MgGeometry.dll %WEB_BIN_RELEASE%64

echo Copying MgPlatformBase Release DLL to %WEB_BIN_RELEASE%64
copy %COMMON_BIN_RELEASE%64\MgPlatformBase.dll %WEB_BIN_RELEASE%64

echo Copying MgMapGuideCommon Release DLL to %WEB_BIN_RELEASE%64
copy %COMMON_BIN_RELEASE%64\MgMapGuideCommon.dll %WEB_BIN_RELEASE%64

echo Copying MgMdfModel Release DLL to %WEB_BIN_RELEASE%64
copy %COMMON_BIN_RELEASE%64\MgMdfModel.dll %WEB_BIN_RELEASE%64

echo Copying MgMdfParser Release DLL to %WEB_BIN_RELEASE%64
copy %COMMON_BIN_RELEASE%64\MgMdfParser.dll %WEB_BIN_RELEASE%64

echo Copying Xerces Release DLL to %WEB_BIN_RELEASE%64
copy "..\..\..\Oem\dbxml-2.3.10\bin64\xerces-c_2_7.dll" %WEB_BIN_RELEASE%64

goto end_copy

REM ---- DEBUG -----
REM This assumes all DLLs are copied to the debug folder
REM and that this batch file is being run in the MapGuideApi folder

:copy_debug
if not exist %WEB_BIN_DEBUG% mkdir %WEB_BIN_DEBUG%

echo Copying ACE Debug DLL to %WEB_BIN_DEBUG%
copy "..\..\..\Oem\ACE\ACE_wrappers\lib\ACEd.dll" %WEB_BIN_DEBUG%

echo Copying JSON Debug DLL to %WEB_BIN_DEBUG%
copy "..\..\..\Oem\jsoncpp\lib\lib_jsond.dll" %WEB_BIN_DEBUG%

echo Copying MgFoundation Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgFoundationd.dll %WEB_BIN_DEBUG%

echo Copying MgGeometry Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgGeometryd.dll %WEB_BIN_DEBUG%

echo Copying MgPlatformBase Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgPlatformBased.dll %WEB_BIN_DEBUG%

echo Copying MgMapGuideCommon Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgMapGuideCommond.dll %WEB_BIN_DEBUG%

echo Copying MgMdfModel Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgMdfModeld.dll %WEB_BIN_DEBUG%

echo Copying MgMdfParser Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgMdfParserd.dll %WEB_BIN_DEBUG%

echo Copying Xerces Debug DLL to %WEB_BIN_DEBUG%
copy "..\..\..\Oem\dbxml-2.3.10\bin\debug\xerces-c_2_7D.dll" %WEB_BIN_DEBUG%

goto end_copy

REM ---- BUILD -----
REM This assumes the batch file is being run in the build folder
REM and that the dependent DLLs are in the current directory

:copy_build
set /P COPY_DIR="Where do you want to copy the files to? "

echo Copying all dependent DLLs to %COPY_DIR%
copy Mg*.dll %COPY_DIR%
copy ACE*.dll %COPY_DIR%
copy lib_json*.dll %COPY_DIR%
copy isapi*.dll %COPY_DIR%

goto end_copy

:end_copy

pause
