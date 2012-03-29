@echo off
rem MapGuide SDK build script for windows
rem
rem Author: Jackie Ng (jumpinjackie@gmail.com)
rem
rem This script will build a subset of the MapGuide source code, such that an SDK can be produced for developing MapGuide
rem applications that do not involve the server/web tier. 
rem 
rem Due to a hard dependency on ACE, the entire header collection of the ACE library is also copied to the output include directory
rem 
rem Before you start, please make sure you have done the following:
rem
rem 1) Build the FDO source and place the files in \Oem\FDO (or alternatively download and extract the latest FDO binary sdk)
rem    with the following structure:
rem     - Bin\%CONFIG% (where %CONFIG% is debug or release)
rem		- Inc
rem		- Lib
rem    
rem 2) Download and install the following:
rem     - doxygen (http://www.doxygen.org)
rem
rem 3) Change the values of DOXYGEN to match the paths of your Doxygen installations respectively
rem     
rem For a "cleaner" build experience, run this script from an exported copy of the
rem MapGuide source code (ie. without subversion folders). 
rem
rem Usage:
rem
rem build.bat [-h]
rem           [-v]
rem	          [-c=BuildType]
rem           [-a=Action]
rem           [-o=OutputDirectory]
rem
rem BuildType: Release(default), Debug
rem Action: build(default), install, clean
rem OutputDirectory: The directory where files will be copied to if -a=install, if -o=default or -o=def then 
rem files will be copied to %MG_DEFAULT_INSTALLDIR%

rem ==================================================
rem Top-level vars
rem ==================================================
SET OLDPATH=%PATH%
SET TYPEACTION=build
SET TYPEBUILD=Release
rem There are no components in an SDK build. Everything must be built! DO NOT CHANGE!
SET TYPECOMPONENT=all

rem ==================================================
rem MapGuide vars
rem ==================================================
SET MG_DEV=%CD%
SET MG_OEM=%MG_DEV%\Oem
SET MG_COMMON=%MG_DEV%\Common
SET MG_DOC=%MG_DEV%\Doc
SET MG_WEB=%MG_DEV%\Web
SET MG_WEB_SRC=%MG_WEB%\src
SET MG_WEB_BIN=%MG_WEB%\bin
SET MG_DOC_OUTPUT=%MG_DOC%\MgOpensource_WebAPIReference
SET MG_DOC_DEVGUIDE_SAMPLES=%MG_DOC%\samples\phpsamples
SET MG_BUILD_TEMP=%MG_DEV%\BuildTemp

SET MG_SDK_OUTPUT=%MG_DEV%\SDK\%TYPEBUILD%
SET MG_SDK_BIN=%MG_SDK_OUTPUT%\Bin
SET MG_SDK_LIB=%MG_SDK_OUTPUT%\Lib
SET MG_SDK_INC=%MG_SDK_OUTPUT%\Inc
SET MG_SDK_FDO=%MG_SDK_OUTPUT%\Fdo
SET MG_BUILD_COMPONENT=

SET MG_ERROR=0

rem ==================================================
rem MSBuild Settings
rem ==================================================

rem If the NUMBER_OF_PROCESSORS environment variable is wrong for any reason. Change this value.
SET CPU_CORES=%NUMBER_OF_PROCESSORS%

rem Uncomment the line below to enable msbuild logging
rem SET MSBUILD_LOG=/l:FileLogger,Microsoft.Build.Engine;logfile=Build.log;verbosity=diagnostic
SET MSBUILD_VERBOSITY=/v:q

rem ==================================================
rem Command aliases
rem ==================================================
rem SET XCOPY=xcopy /E /Y /I /F
SET XCOPY=xcopy /E /Y /I /Q
SET XCOPY_SINGLE=xcopy /Y /I /Q
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%

rem ==================================================
rem Parameter Handling
rem ==================================================

:study_params
if (%1)==() goto pre_build_check

if "%1"=="-help"    goto help_show
if "%1"=="-h"       goto help_show

if "%1"=="-c"       goto get_conf
if "%1"=="-config"  goto get_conf

if "%1"=="-a"       goto get_action
if "%1"=="-action"  goto get_action

if "%1"=="-v"       goto get_verbose
if "%1"=="-o"       goto get_output

goto custom_error

:next_param
shift
shift
goto study_params

:get_verbose
SET MSBUILD_VERBOSITY=/v:d
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%
goto next_param

:get_output
SET MG_OUTPUT=%2
if "%2" == "default" SET MG_OUTPUT=%MG_DEV%
if "%2" == "def" SET MG_OUTPUT=%MG_DEV%
SET MG_SDK_OUTPUT=%MG_OUTPUT%\SDK\%TYPEBUILD%
SET MG_SDK_BIN=%MG_SDK_OUTPUT%\Bin
SET MG_SDK_LIB=%MG_SDK_OUTPUT%\Lib
SET MG_SDK_INC=%MG_SDK_OUTPUT%\Inc
goto next_param

:get_conf
SET TYPEBUILD=%2
SET MSBUILD=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% %MSBUILD_VERBOSITY% %MSBUILD_LOG%
SET MSBUILD_CLEAN=msbuild.exe /nologo /m:%CPU_CORES% /p:Configuration=%TYPEBUILD% /t:Clean %MSBUILD_VERBOSITY%
SET MG_OUTPUT=%MG_DEV%\%TYPEBUILD%
SET MG_OUTPUT_SERVER=%MG_OUTPUT%\Server
SET MG_OUTPUT_WEB=%MG_OUTPUT%\Web

if "%2"=="Release" goto next_param
if "%2"=="Debug" goto next_param
SET ERRORMSG=Unrecognised configuration: %2
goto custom_error

:get_action
SET TYPEACTION=%2
if "%2"=="build" goto next_param
if "%2"=="clean" goto next_param
if "%2"=="install" goto next_param
SET ERRORMSG=Unrecognised action: %2
goto custom_error

rem ==================================================
rem Check Environment Vars
rem ==================================================
:pre_build_check
rem :check_java
rem if exist "%JAVA_HOME%" goto check_ant
rem SET ERRORMSG=Unable to find java sdk installation. Please install the java sdk and set the JAVA_HOME environment variable
rem goto custom_error_no_help
rem :check_ant
rem if exist "%ANT_HOME%" goto check_fdo
rem SET ERRORMSG=Unable to find Apache ant installation. Please install apache ant and set the ANT_HOME environment variable
rem goto custom_error_no_help
:check_fdo
:check_fdo_bin
if exist "%MG_OEM%\FDO\Bin\%TYPEBUILD%" goto check_fdo_inc
SET ERRORMSG=Unable to find FDO binaries. Copy the FDO binaries %MG_OEM%\FDO\Bin\%TYPEBUILD%
goto custom_error_no_help
:check_fdo_inc
if exist "%MG_OEM%\FDO\Bin\%TYPEBUILD%" goto check_fdo_lib
SET ERRORMSG=Unable to find FDO headers. Copy the FDO headers into %MG_OEM%\Inc
goto custom_error_no_help
:check_fdo_lib
if exist "%MG_OEM%\FDO\Bin\%TYPEBUILD%" goto start_build
SET ERRORMSG=Unable to find FDO lib files. Copy the FDO lib files into %MG_OEM%\Lib
goto custom_error_no_help

:start_build
echo ===================================================
echo Configuration is [%TYPEBUILD%]
echo Action is [%TYPEACTION%] on [%TYPECOMPONENT%]
echo Deployment Directory for SDK: %MG_SDK_OUTPUT%
echo CPU cores: %CPU_CORES%
echo ===================================================

if "%TYPEACTION%"=="build" goto build
if "%TYPEACTION%"=="clean" goto clean
if "%TYPEACTION%"=="install" goto install

:clean
if "%TYPECOMPONENT%"=="oem" goto clean_oem
if "%TYPECOMPONENT%"=="common" goto clean_common
if "%TYPECOMPONENT%"=="server" goto clean_server
if "%TYPECOMPONENT%"=="web" goto clean_web
if "%TYPECOMPONENT%"=="doc" goto clean_doc

:clean_oem
echo [clean]: Clean Oem
%MSBUILD_CLEAN% %MG_OEM%\Oem.sln
echo [clean]: Clean Oem - CS-Map
%MSBUILD_CLEAN% %MG_OEM%\CSMap\VC90\OpenSource.sln
if not "%TYPECOMPONENT%"=="all" goto quit

:clean_common
echo [clean]: Clean Common
%MSBUILD_CLEAN% %MG_COMMON%\Common.sln
rem if not "%TYPECOMPONENT%"=="all" goto quit
goto quit

rem :clean_doc
rem echo [clean]: Clean Doc
rem if exist %MG_DOC_OUTPUT% rd /S /Q %MG_DOC_OUTPUT%
rem goto quit


rem =======================================================
rem Build Targets.... the all targets just carry on
rem =======================================================

:build
if "%TYPECOMPONENT%"=="oem" goto build_oem
if "%TYPECOMPONENT%"=="common" goto build_common
if "%TYPECOMPONENT%"=="server" goto build_server
if "%TYPECOMPONENT%"=="web" goto build_web
if "%TYPECOMPONENT%"=="doc" goto build_doc
if "%TYPECOMPONENT%"=="all" goto build_oem
if "%TYPECOMPONENT%"=="allnodoc" goto build_oem
SET ERRORMSG=Unrecognised component: %TYPECOMPONENT%
goto custom_error

:build_oem
echo [build]: Building Oem
%MSBUILD% %MG_OEM%\Oem.sln
if "%errorlevel%"=="1" goto error
rem CsMap is not in Oem.sln, so we need to build that separately
echo [build]: Building Oem - CSMap
%MSBUILD% %MG_OEM%\CsMap\VC90\OpenSource.sln
if "%errorlevel%"=="1" goto error

:build_common
echo [build]: Building Common
%MSBUILD% %MG_COMMON%\Common.sln
if "%errorlevel%"=="1" goto error

:build_web
rem Shortcut method. Takes longer, but we get the SWIG'd binaries GUARANTEED!
echo [build]: Building Web Tier
%MSBUILD% "%MG_WEB_SRC%\WebTier.sln"
if "%errorlevel%"=="1" goto error

rem :build_swig
rem echo [build]: SWIG wrappers - Unmanaged
rem %MSBUILD% "%MG_WEB%\DotNetUnmanagedApi\Foundation\FoundationApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetUnmanagedApi\Geometry\GeometryApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetUnmanagedApi\MapGuideCommon\MapGuideCommonApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetUnmanagedApi\PlatformBase\PlatformBaseApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetUnmanagedApi\Web\WebApi.vcproj"
rem echo [build]: SWIG wrappers - .net assemblies
rem %MSBUILD% "%MG_WEB%\DotNetApi\Foundation\FoundationDotNetApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetApi\Geometry\GeometryDotNetApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetApi\MapGuideCommon\MapGuideCommonDotNetApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetApi\PlatformBase\PlatformBaseDotNetApi.vcproj"
rem %MSBUILD% "%MG_WEB%\DotNetUnmanagedApi\Web\WebApi.vcproj"

rem :build_web
rem echo [build]: Building Web Tier
rem %MSBUILD% %MG_WEB_SRC%\WebTier.sln
rem if "%errorlevel%"=="1" goto error
rem if "%TYPECOMPONENT%"=="oem" 	 goto quit
rem if "%TYPECOMPONENT%"=="common" 	 goto quit
rem if "%TYPECOMPONENT%"=="server" 	 goto quit
rem if "%TYPECOMPONENT%"=="web" 	 goto quit
rem if "%TYPECOMPONENT%"=="doc" 	 goto quit
rem if "%TYPECOMPONENT%"=="allnodoc" goto quit

rem this is left out with component allnodoc

rem :build_doc
rem echo [build]: Building Doc
rem pushd %MG_DOC%
rem call MgOpenSource_run_doxygen.bat
rem popd
rem goto quit

rem =======================================================
rem INSTALL Targets.... the all targets just carry on
rem =======================================================

:install
:install_sdk
echo [install]: SDK
if not exist "%MG_SDK_BIN%" mkdir "%MG_SDK_BIN%"
if not exist "%MG_SDK_LIB%" mkdir "%MG_SDK_LIB%"
if not exist "%MG_SDK_INC%" mkdir "%MG_SDK_INC%"
echo [install]: SDK - Oem (ACE)
rem ACE
copy /Y "%MG_OEM%\ACE\ACE_wrappers\lib\ACE.dll" "%MG_SDK_BIN%"
copy /Y "%MG_OEM%\ACE\ACE_wrappers\lib\ACE.pdb" "%MG_SDK_BIN%"
copy /Y "%MG_OEM%\ACE\ACE_wrappers\lib\ACE.lib" "%MG_SDK_LIB%"
rem Due to ACE headers being leaked out in public MG headers, we need these too
%XCOPY% /S "%MG_OEM%\ACE\ACE_wrappers\ace\*.h" "%MG_SDK_INC%\ACE_wrappers\ACE" /EXCLUDE:svn_excludes.txt
%XCOPY% /S "%MG_OEM%\ACE\ACE_wrappers\ace\*.inl" "%MG_SDK_INC%\ACE_wrappers\ACE" /EXCLUDE:svn_excludes.txt
%XCOPY% /S "%MG_OEM%\ACE\ACE_wrappers\ace\*.cpp" "%MG_SDK_INC%\ACE_wrappers\ACE" /EXCLUDE:svn_excludes.txt
echo [install]: SDK - Oem (GEOS)
rem Same with GEOS
copy /Y "%MG_OEM%\geos-2.2.0\VisualStudio\%TYPEBUILD%\GEOS.dll" "%MG_SDK_BIN%"
copy /Y "%MG_OEM%\geos-2.2.0\VisualStudio\%TYPEBUILD%\GEOS.pdb" "%MG_SDK_BIN%"
echo [install]: SDK - Oem (xerces)
rem Same with xerces
copy /Y "%MG_OEM%\dbxml\xerces-c-src\Build\Win32\VC9\%TYPEBUILD%\xerces-c_3_1mg.dll" "%MG_SDK_BIN%"
copy /Y "%MG_OEM%\dbxml\xerces-c-src\Build\Win32\VC9\%TYPEBUILD%\xerces-c_3_1mg.pdb" "%MG_SDK_BIN%"
copy /Y "%MG_OEM%\dbxml\xerces-c-src\Build\Win32\VC9\%TYPEBUILD%\xerces-c_3mg.lib" "%MG_SDK_LIB%"
%XCOPY% /S "%MG_OEM%\dbxml\xerces-c-src\src\*.h" "%MG_SDK_INC%\xerces" /EXCLUDE:svn_excludes.txt
%XCOPY% /S "%MG_OEM%\dbxml\xerces-c-src\src\*.hpp" "%MG_SDK_INC%\xerces" /EXCLUDE:svn_excludes.txt
%XCOPY% /S "%MG_OEM%\dbxml\xerces-c-src\src\*.c" "%MG_SDK_INC%\xerces" /EXCLUDE:svn_excludes.txt
echo [install]: SDK - Oem (DWF Toolkit)
rem DWF Toolkit isn't technically required, but it's hard to implement your own MgDrawingService without it
%XCOPY% /S "%MG_OEM%\DWFTK7.1\develop\global\lib\static\%TYPEBUILD%\vc8.0\*.lib" "%MG_SDK_LIB%\dwftk"
%XCOPY% /S "%MG_OEM%\DWFTK7.1\develop\global\src\dwf\*.*" "%MG_SDK_INC%\DWFTK7.1\dwf" /EXCLUDE:svn_excludes.txt
%XCOPY% /S "%MG_OEM%\DWFTK7.1\develop\global\src\dwfcore\*.*" "%MG_SDK_INC%\DWFTK7.1\dwfcore" /EXCLUDE:svn_excludes.txt
%XCOPY% /S "%MG_OEM%\DWFTK7.1\develop\global\src\dwfemap\*.*" "%MG_SDK_INC%\DWFTK7.1\dwfemap" /EXCLUDE:svn_excludes.txt
echo [install]: SDK - FDO
rem FDO SDK. Copy each subdir into each respective "Fdo" subdirectory to distinguish FDO files from MG files
%XCOPY% "%MG_OEM%\FDO\bin\%TYPEBUILD%" "%MG_SDK_BIN%\Fdo"
%XCOPY% "%MG_OEM%\FDO\Inc" "%MG_SDK_INC%\Fdo"
%XCOPY% "%MG_OEM%\FDO\Lib" "%MG_SDK_LIB%\Fdo"
echo [install]: SDK - MG Common
rem Common binaries
copy /Y "%MG_COMMON%\bin\%TYPEBUILD%\*.dll" "%MG_SDK_BIN%
copy /Y "%MG_COMMON%\lib\%TYPEBUILD%\*.lib" "%MG_SDK_LIB%
copy /Y "%MG_COMMON%\bin\%TYPEBUILD%\*.pdb" "%MG_SDK_BIN%
rem GD binary (required by Renderers)
copy /Y "%MG_OEM%\gd\build\win32\gd\%TYPEBUILD%\gd.dll" "%MG_SDK_BIN%
copy /Y "%MG_OEM%\gd\build\win32\gd\%TYPEBUILD%\gd.pdb" "%MG_SDK_BIN%
rem Foundation
if not exist "%MG_SDK_INC%\Foundation" mkdir "%MG_SDK_INC%\Foundation"
%XCOPY% /S "%MG_COMMON%\Foundation\*.h" "%MG_SDK_INC%\Foundation" /EXCLUDE:svn_excludes.txt
rem Geometry
if not exist "%MG_SDK_INC%\Geometry" mkdir "%MG_SDK_INC%\Geometry"
%XCOPY% /S "%MG_COMMON%\Geometry\*.h" "%MG_SDK_INC%\Geometry" /EXCLUDE:svn_excludes.txt
rem MdfModel
if not exist "%MG_SDK_INC%\MdfModel" mkdir "%MG_SDK_INC%\MdfModel"
%XCOPY% /S "%MG_COMMON%\MdfModel\*.h" "%MG_SDK_INC%\MdfModel" /EXCLUDE:svn_excludes.txt
rem MdfParser
if not exist "%MG_SDK_INC%\MdfParser" mkdir "%MG_SDK_INC%\MdfParser"
%XCOPY% /S "%MG_COMMON%\MdfParser\*.h" "%MG_SDK_INC%\MdfParser" /EXCLUDE:svn_excludes.txt
rem PlatformBase
if not exist "%MG_SDK_INC%\PlatformBase" mkdir "%MG_SDK_INC%\PlatformBase"
%XCOPY% /S "%MG_COMMON%\PlatformBase\*.h" "%MG_SDK_INC%\PlatformBase" /EXCLUDE:svn_excludes.txt
rem Renderers
if not exist "%MG_SDK_INC%\Renderers" mkdir "%MG_SDK_INC%\Renderers"
%XCOPY% /S "%MG_COMMON%\Renderers\*.h" "%MG_SDK_INC%\Renderers" /EXCLUDE:svn_excludes.txt
rem Schema
if not exist "%MG_SDK_INC%\Schema" mkdir "%MG_SDK_INC%\Schema"
%XCOPY% "%MG_COMMON%\Schema" "%MG_SDK_INC%\Schema" /EXCLUDE:svn_excludes.txt
rem Stylization
if not exist "%MG_SDK_INC%\Stylization" mkdir "%MG_SDK_INC%\Stylization"
%XCOPY% /S "%MG_COMMON%\Stylization\*.h" "%MG_SDK_INC%\Stylization" /EXCLUDE:svn_excludes.txt
rem Delete some stuff that should not be there
pushd "%MG_SDK_BIN%"
del GeometryConsoleTest.pdb
popd
goto quit

:error
echo [ERROR]: There was an error building the component
exit /B 1

:custom_error_no_help
echo [ERROR]: %ERRORMSG%
SET ERRORMSG=
exit /B 1

:custom_error
echo [ERROR]: %ERRORMSG%
SET ERRORMSG=
echo Please use the format:
:help_show
echo ************************************************************************
echo build.bat [-h]
echo           [-v]
echo           [-c=BuildType]
echo           [-w=Component]
echo           [-a=Action]
echo           [-o=OutputDirectory]
echo Help:                  -h[elp]
echo Verbose:               -v
echo BuildType:             -c[onfig]=Release(default), Debug
echo Action:                -a[ction]=build(default),
echo                                  install,
echo                                  clean,
echo Component:             -w[ith]=all(default),
echo                                oem,
echo                                common,
echo                                server,
echo                                web,
echo                                doc
echo ************************************************************************
:quit
SET TYPEACTION=
SET TYPEBUILD=
SET MG_OUTPUT=
SET MG_BUILD_COMPONENT=
SET MG_DEV=
SET MG_INSTALLER=
SET MG_OEM=
SET MG_SERVER=
SET MG_WEB=
SET MG_WEB_SRC=
SET MG_WEB_BIN=
SET MG_UNIT_TEST=
SET MG_FUSION=
SET MG_DOC=
SET MG_DOC_OUTPUT=
SET MG_ERROR=

SET MSBUILD_LOG=
SET MSBUILD_VERBOSITY=
SET XCOPY=
SET MSBUILD=
SET PATH=%OLDPATH%
