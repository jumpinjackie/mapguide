@echo off
SET ROOT=%CD%
SET COMPONENT=
pushd sources
if exist updated rd /S /Q updated
if exist filelist rd /S /Q filelist
if exist fdosdk_filename del /F fdosdk_filename
if exist fdosdk_rev del /F fdosdk_rev
if exist fdosdk*.tar.gz del /F fdosdk*.tar.gz
popd
pushd centos6\src_update
call vagrant destroy -f
popd
pushd ubuntu12\x86
SET COMPONENT=Ubuntu 12 Build 32-bit
echo [clean]: %COMPONENT%
call vagrant destroy -f
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
popd
pushd ubuntu14\x86
SET COMPONENT=Ubuntu 14 Build 32-bit
echo [clean]: %COMPONENT%
call vagrant destroy -f
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
popd
pushd centos6\x86
SET COMPONENT=CentOS 6 Build 32-bit
echo [clean]: %COMPONENT%
call vagrant destroy -f
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
popd
pushd ubuntu12\x64
SET COMPONENT=Ubuntu 12 Build 64-bit
echo [clean]: %COMPONENT%
call vagrant destroy -f
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
popd
pushd ubuntu14\x64
SET COMPONENT=Ubuntu 14 Build 64-bit
echo [clean]: %COMPONENT%
call vagrant destroy -f
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
popd
pushd centos6\x64
SET COMPONENT=CentOS 6 Build 64-bit
echo [clean]: %COMPONENT%
call vagrant destroy -f
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
popd
goto quit
:error
echo [ERROR]: There was an error cleaning the component %COMPONENT%
exit /B 1
:quit