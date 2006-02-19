@echo off
REM ----------------
REM For Windows only
REM ----------------

REM **** Do not edit these unless you know what you are doing *****
SET WEB_BIN_DEBUG=..\..\bin\debug
SET WEB_BIN_RELEASE=..\..\bin\release

SET WEB_ROOT=..\..

SET COMMON_ROOT=..\..\..\Common

SET COMMON_BIN_DEBUG=..\..\..\Common\bin\debug
SET COMMON_BIN_RELEASE=..\..\..\Common\bin\release

REM **** Edit this to point to your PHP Extension directory *****
SET PHP_EXT_DIR=C:\PHP\ext

IF "%1"=="BUILD" GOTO copy_build
IF "%1"=="RELEASE" GOTO copy_release
IF "%1"=="DEBUG" GOTO copy_debug

REM ---- DEFAULT -----
:default
echo Need to use RELEASE, DEBUG or BUILD arguments
GOTO end_copy

REM ---- RELEASE -----
REM This assumes all DLLs are copied to the release folder
REM and that this batch file is being run in the MapGuideApi folder

:copy_release
echo Copying ACE Release DLL to %WEB_BIN_RELEASE%
copy "..\..\..\Oem\ACE\ACE_wrappers\lib\ACE.dll" %WEB_BIN_RELEASE%

echo Copying MgCommon Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgCommon.dll %WEB_BIN_RELEASE%

echo Copying MgCoordinateSystem Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgCoordinateSystem.dll %WEB_BIN_RELEASE%

echo Copying MgMdfModel Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgMdfModel.dll %WEB_BIN_RELEASE%

echo Copying MgMdfParser Release DLL to %WEB_BIN_RELEASE%
copy %COMMON_BIN_RELEASE%\MgMdfParser.dll %WEB_BIN_RELEASE%

echo Copying PROJ4 DLL to %WEB_BIN_RELEASE%
copy "..\..\..\Oem\proj-4.4.9\src\proj.dll" %WEB_BIN_RELEASE%

echo Copying Xerces Release DLL to %WEB_BIN_RELEASE%
copy "..\..\..\Oem\dbxml-2.2.13\bin\xerces-c_2_7.dll" %WEB_BIN_RELEASE%

echo Copying LIBFCGI Release DLL to %WEB_ROOT%\src\MapAgent
copy "..\..\..\Oem\fcgi\fcgi-2.4.0\libfcgi\Release\libfcgi.dll" %WEB_ROOT%\src\MapAgent

goto end_copy

REM ---- DEBUG -----
REM This assumes all DLLs are copied to the debug folder
REM and that this batch file is being run in the MapGuideApi folder

:copy_debug
echo Copying ACE Debug DLL to %WEB_BIN_DEBUG%
copy "..\..\..\Oem\ACE\ACE_wrappers\lib\ACEd.dll" %WEB_BIN_DEBUG%

echo Copying MgCommon Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgCommond.dll %WEB_BIN_DEBUG%

echo Copying MgCoordinateSystem Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgCoordinateSystemd.dll %WEB_BIN_DEBUG%

echo Copying MgMdfModel Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgMdfModeld.dll %WEB_BIN_DEBUG%

echo Copying MgMdfParser Debug DLL to %WEB_BIN_DEBUG%
copy %COMMON_BIN_DEBUG%\MgMdfParserd.dll %WEB_BIN_DEBUG%

echo Copying PROJ4 DLL to %WEB_BIN_DEBUG%
copy "..\..\..\Oem\proj-4.4.9\src\proj.dll" %WEB_BIN_DEBUG%

echo Copying Xerces Debug DLL to %WEB_BIN_DEBUG%
copy "..\..\..\Oem\dbxml-2.2.13\bin\debug\xerces-c_2_7D.dll" %WEB_BIN_DEBUG%

echo Copying LIBFCGI Debug DLL to %WEB_ROOT%\src\MapAgent
copy "..\..\..\Oem\fcgi\fcgi-2.4.0\libfcgi\Debug\libfcgid.dll" %WEB_ROOT%\src\MapAgent

goto end_copy

REM ---- BUILD -----
:copy_build
REM This assumes the batch file is being run in the build folder
REM and that the dependent DLLs are in the current directory

SET /P COPY_DIR="Where do you want to copy the files to? "

echo Copying all dependent DLLs to %COPY_DIR%
copy Mg*.dll %COPY_DIR%
copy ACE*.dll %COPY_DIR%
copy libfcgi*.dll %COPY_DIR%
copy proj.dll %COPY_DIR%

goto end_copy

:end_copy

pause
