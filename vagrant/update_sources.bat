@echo off

rem update_sources.bat
rem
rem Updates MapGuide/FDO source tarballs via the src_update centos6 VM
pushd sources
if exist updated rd /S /Q updated
popd
copy /Y scripts\svn_update.sh centos6\src_update
pushd centos6\src_update
call vagrant up 2>&1 | tee up.log
echo [src_update]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
call vagrant destroy -f
popd
pushd sources\updated
move /Y *.tar.gz ..
echo [src_update]: Sources updated
popd