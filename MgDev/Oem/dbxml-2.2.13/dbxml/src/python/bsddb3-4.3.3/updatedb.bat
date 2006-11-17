@echo off
setlocal

rem Update the local copy of the DB libs and stuff.

set DBDIR=f:\db-4.1.25
rem set DBDIR=f:\db-4.0.14
rem set DBDIR=c:\projects\db-3.3.11
rem set DBDIR=c:\projects\db-3.2.9
rem set DBDIR=c:\projects\db-3.1.17

mkdir db
mkdir db\include
mkdir db\lib
mkdir db\bin

copy /v %DBDIR%\build_win32\db.h                 db\include
copy /v %DBDIR%\build_win32\Debug\*.lib          db\lib
copy /v %DBDIR%\build_win32\Debug_static\*.lib   db\lib
copy /v %DBDIR%\build_win32\Release\*.lib        db\lib
copy /v %DBDIR%\build_win32\Release_static\*.lib db\lib
copy /v %DBDIR%\build_win32\Release\*.dll        db\bin
copy /v %DBDIR%\build_win32\Release\*.exe        db\bin

endlocal

