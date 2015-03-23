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
SET LIB_DEST=..\..\lib\%4
SET JAR_DEST=..\..\jar\%4
SET BIN_DEST=..\..\bin\%4
SET DLL_DEST=..\..\bin\%4

mkdir %DLL_DEST%
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
xcopy %3%pname%.exe %BIN_DEST% /S/Y/Q
xcopy %3%pname%.pdb %BIN_DEST% /S/Y/Q
GOTO :EOF

:dll
ECHO "installing dll %1"
SET pname=%1
SET libname=%1
IF "%5" == "debug" set libname=%libname%d
IF "%5" == "debug" set pname=%pname%d
xcopy %3%libname%.dll %DLL_DEST% /S/Y/Q
if "%6" NEQ "" xcopy %3%libname%.lib %LIB_DEST% /S/Y/Q
xcopy %3%pname%.pdb %DLL_DEST% /S/Y/Q


