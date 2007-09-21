@echo off

echo.
echo.
echo --- Recovering Site repository ...
echo.
..\bin\db_recover -cv -h "%cd%\..\Repositories\Site"
echo.

echo.
echo --- Recovering Library repository ...
echo.
..\bin\db_recover -cv -h "%cd%\..\Repositories\Library"
echo.

@echo on
