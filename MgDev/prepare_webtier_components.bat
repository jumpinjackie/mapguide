@echo off
SET SEVENZ=%CD%\BuildTools\WebTools\7-zip\7z.exe
SET HTTPD_VER=2.4.18
SET PHP_VER=5.6.23
pushd Oem\LinuxApt
:check_httpd
if not exist httpd-%HTTPD_VER% goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\httpd.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\http_config.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\ap_config.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\http_log.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\http_protocol.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\http_main.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\util_script.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\http_core.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\apr_strings.h goto unpack_httpd
if not exist httpd-%HTTPD_VER%\include\apr_tables.h goto unpack_httpd
goto check_php
:unpack_httpd
if not exist httpd-%HTTPD_VER%.tar "%SEVENZ%" x httpd-%HTTPD_VER%.tar.bz2
"%SEVENZ%" x -y httpd-%HTTPD_VER%.tar
:check_php
rem *sigh* there is not a 100% reliable way to nuke a directory in windows from the cmd line to make way 
rem for a new MapGuide build, meaning we could have empty PHP directories from a previous build that would 
rem pass the previous iteration of this check to determine if PHP needs to be unpacked. So check for the specific
rem headers that we know we are going to be using and if a single one of these headers cannot be found then
rem do the archive unpacking
if not exist php-%PHP_VER% goto unpack_php
if not exist php-%PHP_VER%\ext goto unpack_php
if not exist php-%PHP_VER%\ext\standard goto unpack_php
if not exist php-%PHP_VER%\ext\standard\info.h goto unpack_php
if not exist php-%PHP_VER%\main goto unpack_php
if not exist php-%PHP_VER%\main\php.h goto unpack_php
if not exist php-%PHP_VER%\main\php_ini.h goto unpack_php
if not exist php-%PHP_VER%\main\config.w32.h goto unpack_php
if not exist php-%PHP_VER%\main\win95nt.h goto unpack_php
if not exist php-%PHP_VER%\TSRM goto unpack_php
if not exist php-%PHP_VER%\TSRM\readdir.h goto unpack_php
if not exist php-%PHP_VER%\Zend goto unpack_php
if not exist php-%PHP_VER%\Zend\zend.h goto unpack_php
if not exist php-%PHP_VER%\Zend\zend_exceptions.h goto unpack_php
if not exist php-%PHP_VER%\Zend\zend_API.h goto unpack_php
goto patch_php_headers
:unpack_php
if not exist php-%PHP_VER%.tar "%SEVENZ%" x php-%PHP_VER%.tar.bz2
"%SEVENZ%" x -y php-%PHP_VER%.tar
:patch_php_headers
echo Patching PHP headers
copy /Y ..\php\main\*.h php-%PHP_VER%\main\
copy /Y ..\php\TSRM\*.h php-%PHP_VER%\TSRM\
copy /Y ..\php\Zend\*.h php-%PHP_VER%\Zend\
:patch_httpd_headers
echo Patching HTTPD headers
copy /Y ..\httpd\include\*.h httpd-%HTTPD_VER%\include\
:done
popd
