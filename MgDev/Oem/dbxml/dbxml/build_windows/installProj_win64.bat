@ECHO OFF
REM This bat script copies build artifacts to the BDB XML
REM install tree
REM Usage: 
REM	installProj.bat name <prog|dll> OutDir <debug|release> [copyLib]
REM  name is dll or program name
REM  "prog" if it's a program, "dll" if it's a library
REM  OutDir is the $(OutDir) from the project -- probably "Release" or "Debug"
REM  "debug" if it's a debug build (will copy .pdb and put binaries in debug
REM     directory
REM  copyLib if not null, and dll, will copy name[d]./lib to LIB_DEST
REM This script assumes all directories have been created
REM
REM
SET LIB_DEST=..\..\lib64
SET JAR_DEST=..\..\jar64
SET BIN_DEST=..\..\bin64
SET DLL_DEST=..\..\bin64

IF %4 == debug SET BIN_DEST=%BIN_DEST%\debug
IF %4 == debug SET DLL_DEST=%DLL_DEST%\debug

mkdir %LIB_DEST%
mkdir %BIN_DEST%
mkdir %JAR_DEST%

IF %2 == prog GOTO program
IF %2 == dll GOTO dll

ECHO "USAGE: installProj.bat name <prog|dll> OutDir <debug|release> [copyLib]"
GOTO :EOF

:program
ECHO "installing program %1"
SET pname=%1
xcopy %3\%pname%.exe %BIN_DEST% /S/Y
xcopy %3\%pname%.pdb %BIN_DEST% /S/Y/Q
GOTO :EOF

:dll
ECHO "installing dll %1"
SET pname=%1
SET libname=%1
IF "%4" == "debug" set libname=%libname%d
xcopy %3\%libname%.dll %DLL_DEST% /S/Y
if "%5" NEQ "" xcopy %3\%libname%.lib %LIB_DEST% /S/Y
IF "%4" == "debug" xcopy %3\%pname%.pdb %DLL_DEST% /S/Y


