@echo off
REM ----------------
REM For Windows only
REM ----------------

CALL setupvars.bat

IF "%1" == "NOCONFIRM" GOTO noconfirm_delete

:confirm_delete
REM *******************************************
REM *Delete Extension Dlls WITH CONFIRMATION  *
REM *******************************************

echo Deleting php_MapAgent DLLs from %PHP_EXT_DIR%
del %PHP_EXT_DIR%\php_MapAgent*.dll /P

REM ******************************************
REM * Delete Release Dlls WITH CONFIRMATION  *
REM ******************************************

echo Deleting Release DLLs
del %WEB_BIN_RELEASE%\*.dll /P
del %WEB_BIN_RELEASE%\*.ini /P


REM *****************************************
REM *  Delete Debug Dlls WITH CONFIRMATION  *
REM *****************************************

echo Deleting Debug DLLs
del %WEB_BIN_DEBUG%\*.dll /P
del %WEB_BIN_DEBUG%\*.ini /P

goto end_delete

:noconfirm_delete

REM **********************************************
REM *Delete Extension Dlls WITH NO CONFIRMATION  *
REM **********************************************

echo Deleting php_MapAgent DLLs from %PHP_EXT_DIR%
del %PHP_EXT_DIR%\php_MapAgent*.dll

REM ********************************************
REM * Delete Release Dlls WITH NO CONFIRMATION *
REM ********************************************

echo Deleting Release DLLs
del %WEB_BIN_RELEASE%\*.dll
del %WEB_BIN_RELEASE%\*.ini


REM *******************************************
REM *  Delete Debug Dlls WITH NO CONFIRMATION *
REM *******************************************

echo Deleting Debug DLLs
del %WEB_BIN_DEBUG%\*.dll
del %WEB_BIN_DEBUG%\*.ini


:end_delete
