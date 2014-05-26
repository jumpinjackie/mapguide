@echo off
SET ROOT=%CD%
SET COMPONENT=
pushd sources
if exist updated rd /S /Q updated
if exist filelist rd /S /Q filelist
if exist fdosdk_filename del /F fdosdk_filename
if exist fdosdk_rev del /F fdosdk_rev
del /F fdosdk*.tar.gz
popd
pushd ubuntu\x86
SET COMPONENT=Ubuntu Build 32-bit
echo [clean]: MapGuide Ubuntu build 32-bit
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
call vagrant destroy -f
popd
pushd centos\x86
SET COMPONENT=CentOS Build 32-bit
echo [clean]: MapGuide CentOS build 32-bit
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
call vagrant destroy -f
popd
pushd ubuntu\x64
SET COMPONENT=Ubuntu Build 64-bit
echo [clean]: MapGuide Ubuntu build 64-bit
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
call vagrant destroy -f
popd
pushd centos\x64
SET COMPONENT=CentOS Build 64-bit
echo [clean]: MapGuide CentOS build 64-bit
if exist build rd /S /Q build
if exist filelist rd /S /Q filelist
if exist *.sh del *.sh
if exist *.pl del *.pl
if exist *.txt del *.txt
call vagrant destroy -f
popd
goto quit
:error
echo [ERROR]: There was an error cleaning the component %COMPONENT%
exit /B 1
:quit