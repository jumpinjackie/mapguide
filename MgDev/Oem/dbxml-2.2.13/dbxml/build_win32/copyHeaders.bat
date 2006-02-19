REM This bat script copies header files from Xerces, Pathan,
REM XQuery, BDB XML, and Berkeley DB to a common tree.
REM This simplifies building as well as creation of a binary
REM release for Windows
@ECHO OFF
SET INSTALL=..\..
SET XERCES_BUILD=%INSTALL%\xerces-c-src\src\xercesc
SET PATHAN_BUILD=%INSTALL%\pathan
SET XQUERY_BUILD=%INSTALL%\xquery-1.2.0
SET XML_BUILD=%INSTALL%\dbxml
SET DB_BUILD=%INSTALL%\db-4.3.29


SET DEST=%INSTALL%\include
SET XERCES_DEST=%DEST%\xercesc

IF NOT EXIST %XML_BUILD% IF EXIST %INSTALL%\xml SET XML_BUILD=%INSTALL%\xml

echo %XML_BUILD%

mkdir %DEST%

REM Copy Xerces headers
FOR %%A IN (sax sax2 framework framework\psvi dom dom\deprecated internal parsers util util\Compilers util\MsgLoaders util\MsgLoaders\ICU util\MsgLoaders\InMemory util\MsgLoaders\MsgCatalog util\MsgLoaders\Win32 util\Platforms util\Platforms\Win32 util\regx util\Transcoders util\Transcoders\ICU util\Transcoders\Iconv util\Transcoders\Win32 validators validators\common validators\datatype validators\schema validators\schema\identity) DO CALL :docopyhpp %XERCES_BUILD%\%%A %XERCES_DEST%\%%A

REM Copy Pathan headers
CALL :docopyall %PATHAN_BUILD%\include %DEST%

REM Copy XQuery headers
CALL :docopyall %XQUERY_BUILD%\include %DEST%

REM Copy BDB XML headers
CALL :docopyall %XML_BUILD%\include %DEST%

REM Copy Berkeley DB headers
CALL :docopyh %DB_BUILD%\build_win32 %DEST%

GOTO :EOF

:docopyhpp
ECHO Copying %1 to %2
mkdir %2
XCOPY %1\*.hpp %2 /S/I/Y/R
GOTO:EOF

:docopyall
ECHO Copying %1 to %2
REM mkdir %2
XCOPY %1 %2 /S/I/Y/R
GOTO:EOF

:docopyh
ECHO Copying %1 to %2
REM mkdir %2
XCOPY %1\*.h %2 /Y/R
GOTO:EOF
