REM This bat script copies artifacts from 
REM XQilla, BDB XML, and Berkeley DB to a common tree.
REM Files include libraries, debug files (.pdb) and header files
REM This simplifies building as well as creation of a binary
REM release for Windows
@ECHO OFF
SET INSTALL=..\..
SET XERCES_BUILD=%INSTALL%\xerces-c-src\Build\Win32\VC10
SET XQILLA_BUILD=%INSTALL%\xqilla\build\windows\vc8
SET XML_BUILD=%INSTALL%\dbxml\build_windows
SET DB_BUILD=%INSTALL%\db-4.8.26\build_windows


SET LIB_DEST=%INSTALL%\lib
SET BIN_DEST=%INSTALL%\bin
SET JAR_DEST=%INSTALL%\jar
SET BIN_DEBUG_DEST=%INSTALL%\bin\debug

IF NOT EXIST %XML_BUILD% IF EXIST %INSTALL%\xml\build_windows SET XML_BUILD=%INSTALL%\xml\build_windows

echo %XML_BUILD%

mkdir %LIB_DEST%
mkdir %BIN_DEST%
mkdir %BIN_DEBUG_DEST%
mkdir %JAR_DEST%

REM Copy Xerces libraries
XCOPY %XERCES_BUILD%\Release\*.lib %LIB_DEST% /I/Y/R
XCOPY %XERCES_BUILD%\Debug\*.lib %LIB_DEST% /I/Y/R

REM Copy Xerces dlls
XCOPY %XERCES_BUILD%\Debug\*.dll %BIN_DEBUG_DEST% /I/Y/R
XCOPY %XERCES_BUILD%\Debug\*pdb %BIN_DEBUG_DEST% /I/Y/R
XCOPY %XERCES_BUILD%\Release\*.dll %BIN_DEST% /I/Y/R
XCOPY %XERCES_BUILD%\Release\*pdb %BIN_DEST% /I/Y/R

REM Copy XQilla libraries
XCOPY %XQILLA_BUILD%\Debug32\*.lib %LIB_DEST% /I/Y/R
XCOPY %XQILLA_BUILD%\Release32\*.lib %LIB_DEST% /I/Y/R

REM Copy XQilla dlls and exe
XCOPY %XQILLA_BUILD%\Release32\*.dll %BIN_DEST% /I/Y/R
XCOPY %XQILLA_BUILD%\Release32\*.pdb %BIN_DEST% /I/Y/R
XCOPY %XQILLA_BUILD%\Release32\*.exe %BIN_DEST% /I/Y/R
XCOPY %XQILLA_BUILD%\Debug32\*.dll %BIN_DEBUG_DEST% /I/Y/R
XCOPY %XQILLA_BUILD%\Debug32\*.pdb %BIN_DEBUG_DEST% /I/Y/R
XCOPY %XQILLA_BUILD%\Debug32\*.exe %BIN_DEBUG_DEST% /I/Y/R

REM Copy BDB XML libraries and jar
XCOPY %XML_BUILD%\Debug32\*.lib %LIB_DEST% /I/Y/R
XCOPY %XML_BUILD%\Release32\*.lib %LIB_DEST% /I/Y/R
XCOPY %XML_BUILD%\Debug32\*.jar %JAR_DEST% /I/Y/R
XCOPY %XML_BUILD%\Release32\*.jar %JAR_DEST% /I/Y/R

REM Copy BDB XML dlls and exe
XCOPY %XML_BUILD%\Release32\*.dll %BIN_DEST% /I/Y/R
XCOPY %XML_BUILD%\Release32\*.pdb %BIN_DEST% /I/Y/R
XCOPY %XML_BUILD%\Release32\*.exe %BIN_DEST% /I/Y/R
XCOPY %XML_BUILD%\Debug32\*.dll %BIN_DEBUG_DEST% /I/Y/R
XCOPY %XML_BUILD%\Debug32\*.pdb %BIN_DEBUG_DEST% /I/Y/R
XCOPY %XML_BUILD%\Debug32\*.exe %BIN_DEBUG_DEST% /I/Y/R

REM Copy DB libraries and jar
XCOPY %DB_BUILD%\Debug32\*.lib %LIB_DEST% /I/Y/R
XCOPY %DB_BUILD%\Release32\*.lib %LIB_DEST% /I/Y/R
XCOPY %DB_BUILD%\Debug32\*.jar %JAR_DEST% /I/Y/R
XCOPY %DB_BUILD%\Release32\*.jar %JAR_DEST% /I/Y/R

REM Copy DB dlls and exe
XCOPY %DB_BUILD%\Release32\*.dll %BIN_DEST% /I/Y/R
XCOPY %DB_BUILD%\Release32\*.pdb %BIN_DEST% /I/Y/R
XCOPY %DB_BUILD%\Release32\*.exe %BIN_DEST% /I/Y/R
XCOPY %DB_BUILD%\Debug32\*.dll %BIN_DEBUG_DEST% /I/Y/R
XCOPY %DB_BUILD%\Debug32\*.pdb %BIN_DEBUG_DEST% /I/Y/R
XCOPY %DB_BUILD%\Debug32\*.exe %BIN_DEBUG_DEST% /I/Y/R

REM Copy headers
copyHeaders.bat

