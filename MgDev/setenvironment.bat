@echo off
rem ==================================================
rem setenvironment.bat
rem
rem Sets the required environment variables for the
rem build.bat script
rem
rem Compiler defaults to vc9.0. To use the vc10
rem compiler, call setenvironment.bat like so:
rem 
rem setenvironment.bat vc10
rem ==================================================
rem Top-level vars
rem ==================================================
SET OLDPATH=%PATH%
SET TYPEACTION=build
SET TYPEBUILD=Release
SET CONFIGURATION=Release
SET TYPECOMPONENT=all
SET VC_COMPILER_VERSION=9
IF "%1" == "vc10" SET VC_COMPILER_VERSION=10
rem ==================================================
rem MapGuide vars
rem ==================================================
SET MG_DEV=%CD%
SET MG_OEM=%MG_DEV%\Oem
rem DBXML seems to change frequently enough that we should externalize this information
SET MG_OEM_DBXML=dbxml
SET MG_OEM_DB=db-4.8.26
SET MG_COMMON=%MG_DEV%\Common
SET MG_SERVER=%MG_DEV%\Server
SET MG_WEB=%MG_DEV%\Web
SET MG_WEB_SRC=%MG_WEB%\src
SET MG_WEB_BIN=%MG_WEB%\bin
SET MG_UNIT_TEST=%MG_DEV%\UnitTest
@echo off
SET MG_FUSION=%MG_OEM%\Fusion
SET MG_DOC=%MG_DEV%\Doc
SET MG_DOC_XML=%MG_DOC%\mgopensource_xml
SET MG_DOC_OUTPUT=%MG_DOC%\MgOpensource_WebAPIReference
SET MG_DOC_DEVGUIDE_SAMPLES=%MG_DOC%\samples\
SET MG_BUILD_TEMP=%MG_DEV%\BuildTemp

SET MG_OUTPUT=%MG_DEV%\%TYPEBUILD%
SET MG_OUTPUT_SERVER=%MG_OUTPUT%\Server
SET MG_OUTPUT_WEB=%MG_OUTPUT%\Web
SET MG_OUTPUT_CSMAP=%MG_OUTPUT%\CS-Map
SET MG_BUILD_COMPONENT=

SET MG_DEFAULT_INSTALLDIR=C:\Program Files\MapGuideOpenSource2.0
rem ==================================================
rem Extra tool paths
rem 
rem Your paths may be different, adjust accordingly or
rem set the appropriate environment variables before
rem running
rem ==================================================

IF "%JAVA_HOME%" == "" SET JAVA_HOME=C:\Program Files\Java\jdk1.6.0_31
IF "%DOXYGEN%" == "" SET DOXYGEN=%MG_DEV%\BuildTools\doxygen
IF "%GNUWIN32%" == "" SET GNUWIN32=C:\Program Files (x86)\GnuWin32\bin
IF "%SEVENZ%" == "" SET SEVENZ=%MG_DEV%\BuildTools\WebTools\7-Zip
IF "%ANT_HOME%" == "" SET ANT_HOME=C:\apache-ant-1.8.3

SET PATH=%PATH%;%DOXYGEN%;%GNUWIN32%;%SEVENZ%;%ANT_HOME%\bin;%MG_DEV%\BuildTools\DoxyTransform
rem ==================================================
rem MSBuild Settings
rem ==================================================

rem If the NUMBER_OF_PROCESSORS environment variable is wrong for any reason. Change this value.
SET CPU_CORES=%NUMBER_OF_PROCESSORS%

rem Uncomment the line below to enable msbuild logging
rem SET MSBUILD_LOG=/l:FileLogger,Microsoft.Build.Engine;logfile=Build.log;verbosity=diagnostic
SET MSBUILD_VERBOSITY=/v:n

rem ==================================================
rem Command aliases
rem ==================================================
rem SET XCOPY=xcopy /E /Y /I /F
SET XCOPY=xcopy /E /Y /I /Q /H
SET XCOPY_SINGLE=xcopy /Y /I /Q
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%
SET ANT=ant