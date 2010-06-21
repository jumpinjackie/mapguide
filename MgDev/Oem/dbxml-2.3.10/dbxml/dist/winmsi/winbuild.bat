@echo off
::	$Id: winbuild.bat,v 1.6 2006/11/07 23:21:16 gmf Exp $
::	Helper script to build Berkeley DBXML libraries and executables
::	using MSDEV
::

cd build_windows

:: One of these calls should find the desired batch file

call :TryBat "c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat" && goto BATFOUND1

call :TryBat "c:\Program Files\Microsoft Visual Studio .NET\Common7\Tools\vsvars32.bat" && goto BATFOUND2

call :TryBat "c:\Program Files\Microsoft Visual Studio.NET\Common7\Tools\vsvars32.bat" && goto BATFOUND3

goto BATNOTFOUND

:BATFOUND1
echo Using Visual Studio .NET 2003
goto BATFOUND

:BATFOUND2
echo Using Visual Studio .NET
echo *********** CHECK: Make sure the binaries are built with the same system libraries that are shipped.
goto BATFOUND

:BATFOUND3
echo Using Visual Studio.NET
echo *********** CHECK: Make sure the binaries are built with the same system libraries that are shipped.
goto BATFOUND

:BATFOUND

::devenv /useenv /build Release /project instenv ..\instenv\instenv.sln >> ..\winbld.out 2>&1
::if not %errorlevel% == 0 goto ERROR

echo Building Berkeley DBXML
devenv /useenv /build Debug bdbxml_msi.sln >> ..\winbld.out 2>&1
if not %errorlevel% == 0 goto ERROR
devenv /useenv /build Release bdbxml_msi.sln >> ..\winbld.out 2>&1
if not %errorlevel% == 0 goto ERROR
echo Copying Build Artifacts
copyFiles7.bat

:: There doesn't appear to be static versions
:::: devenv /useenv /build "Debug Static" bdbxml_msi.sln >> ..\winbld.out 2>&1
::if not %errorlevel% == 0 goto ERROR
::devenv /useenv /build "Release Static" bdbxml_msi.sln >> ..\winbld.out 2>&1
::if not %errorlevel% == 0 goto ERROR

:: devenv /useenv /build Debug /project ex_repquote bdbxml_msi.sln >> ..\winbld.out 2>&1
:: if not %errorlevel% == 0 goto ERROR

:: set PROJRET=0
:: for %%p in (db_java db_tcl dbxml_java dbxml_tcl dbxml_python) do call :BuildProject %p bdbxml_msi.sln
:: if not %PROJRET% == 0 goto ERROR

goto END


:ERROR
echo *********** ERROR: during win_build.bat *************
echo *********** ERROR: during win_build.bat *************  >> ..\winbld.err
exit 1
goto END

:NSTOP
echo *********** ERROR: win_build.bat stop requested *************
echo *********** ERROR: win_built.bat stop requested *************  >> ..\winbld.err
exit 2
goto END

:BATNOTFOUND
echo *********** ERROR: VC Config batch file not found *************
echo *********** ERROR: VC Config batch file not found *************  >> ..\winbld.err
exit 3
goto END

:: TryBat(BATPATH)
:: If the BATPATH exists, use it and return 0,
:: otherwise, return 1.

:TryBat
:: Filename = %1
if not exist %1 exit /b 1
call %1
exit /b 0
goto :EOF

:BuildProject
:: Projectname = %1
:: Filename = %2
echo   Building %1
echo devenv /useenv /build Debug /project %1 %2 >> ..\winbld.out 2>&1
devenv /useenv /build Debug /project %1 %2 >> ..\winbld.out 2>&1
if not %errorlevel% == 0 goto PROJERROR
echo devenv /useenv /build Release /project %1 %2 >> ..\winbld.out 2>&1
devenv /useenv /build Release /project %1 %2 >> ..\winbld.out 2>&1
if not %errorlevel% == 0 goto PROJERROR
exit /b 0
goto :EOF
:PROJERROR
set PROJRET=1
exit /b 1
goto :EOF


:END
