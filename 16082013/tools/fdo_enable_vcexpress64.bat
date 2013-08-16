@echo off
REM Description: Helper script to enable building 64-bit FDO with the 64-bit C++ compiler from the Windows 7.1 SDK
REM Author: Jackie Ng (jumpinjackie@gmail.com)
REM Usage: fdo_enable_vcexpress64.bat 
SET FDOROOT=%1
if not exist "%FDOROOT%" goto usage
if exist %FDOROOT%\Fdo\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Fdo\build.bat
    echo Updated %FDOROOT%\Fdo\build.bat
)
if exist %FDOROOT%\Providers\ArcSDE\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\ArcSDE\build.bat
    echo Updated %FDOROOT%\Providers\ArcSDE\build.bat
)
if exist %FDOROOT%\Providers\GDAL\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\GDAL\build.bat
    echo Updated %FDOROOT%\Providers\GDAL\build.bat
)
if exist %FDOROOT%\Providers\GenericRdbms\Src\MySQL\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\GenericRdbms\Src\MySQL\build.bat
    echo Updated %FDOROOT%\Providers\GenericRdbms\Src\MySQL\build.bat
)
if exist %FDOROOT%\Providers\GenericRdbms\Src\ODBC\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\GenericRdbms\Src\ODBC\build.bat
    echo Updated %FDOROOT%\Providers\GenericRdbms\Src\ODBC\build.bat
)
if exist %FDOROOT%\Providers\GenericRdbms\Src\PostGis\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\GenericRdbms\Src\PostGis\build.bat
    echo Updated %FDOROOT%\Providers\GenericRdbms\Src\PostGis\build.bat
)
if exist %FDOROOT%\Providers\GenericRdbms\Src\SQLServerSpatial\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\GenericRdbms\Src\SQLServerSpatial\build.bat
    echo Updated %FDOROOT%\Providers\GenericRdbms\Src\SQLServerSpatial\build.bat
)
if exist %FDOROOT%\Providers\KingOracle\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\KingOracle\build.bat
    echo Updated %FDOROOT%\Providers\KingOracle\build.bat
)
if exist %FDOROOT%\Providers\OGR\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\OGR\build.bat
    echo Updated %FDOROOT%\Providers\OGR\build.bat
)
if exist %FDOROOT%\Providers\SDF\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\SDF\build.bat
    echo Updated %FDOROOT%\Providers\SDF\build.bat
)
if exist %FDOROOT%\Providers\SHP\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\SHP\build.bat
    echo Updated %FDOROOT%\Providers\SHP\build.bat
)
if exist %FDOROOT%\Providers\SQLite\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\SQLite\build.bat
    echo Updated %FDOROOT%\Providers\SQLite\build.bat
)
if exist %FDOROOT%\Providers\WFS\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\WFS\build.bat
    echo Updated %FDOROOT%\Providers\WFS\build.bat
)
if exist %FDOROOT%\Providers\WMS\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Providers\WMS\build.bat
    echo Updated %FDOROOT%\Providers\WMS\build.bat
)
if exist %FDOROOT%\Thirdparty\build.bat (
    sed -i "s/REM SET EXTRA_MSBUILD_PROPERTIES/SET EXTRA_MSBUILD_PROPERTIES/g" %FDOROOT%\Thirdparty\build.bat
    echo Updated %FDOROOT%\Thirdparty\build.bat
)
goto exit
:usage
echo Usage - fdo_enable_vcexpress64.bat FDO_SRC_ROOT
:error
exit /B 1
:exit
exit /B 0