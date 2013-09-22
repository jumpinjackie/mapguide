@echo off
SET SEVENZ=%CD%\BuildTools\WebTools\7-zip\7z.exe
SET HTTPD_VER=2.4.6
SET PHP_VER=5.5.3
pushd Oem\LinuxApt
if exist httpd-%HTTPD_VER% (
    echo httpd already unpacked. Nothing to do here.
    goto check_php
)
:unpack_httpd
if not exist httpd-%HTTPD_VER%.tar "%SEVENZ%" x httpd-%HTTPD_VER%.tar.bz2
if not exist httpd-%HTTPD_VER% "%SEVENZ%" x httpd-%HTTPD_VER%.tar
:check_php
if exist php-%PHP_VER% (
    echo PHP already unpacked. Nothing to do here
    goto patch_php_headers
)
:unpack_php
if not exist php-%PHP_VER%.tar "%SEVENZ%" x php-%PHP_VER%.tar.bz2
if not exist php-%PHP_VER% "%SEVENZ%" x php-%PHP_VER%.tar
:patch_php_headers
echo Patching PHP headers
copy /Y ..\php\main\*.h php-%PHP_VER%\main
copy /Y ..\php\TSRM\*.h php-%PHP_VER%\TSRM
:patch_httpd_headers
echo Patching HTTPD headers
copy /Y ..\httpd\include\*.h httpd-%HTTPD_VER%\include
:done
popd
