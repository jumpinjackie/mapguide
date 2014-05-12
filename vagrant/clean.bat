@echo off
SET ROOT=%CD%
SET COMPONENT=
pushd sources
rd /S /Q updated
popd
pushd ubuntu\x86
SET COMPONENT=Ubuntu Build 32-bit
echo [clean]: MapGuide Ubuntu build 32-bit
if exist build rd /S /Q build
if exist *.sh del *.sh
if exist *.pl del *.pl
call vagrant destroy -f
popd
pushd centos\x86
SET COMPONENT=CentOS Build 32-bit
echo [clean]: MapGuide CentOS build 32-bit
if exist build rd /S /Q build
if exist *.sh del *.sh
if exist *.pl del *.pl
call vagrant destroy -f
popd
pushd ubuntu\x64
SET COMPONENT=Ubuntu Build 64-bit
echo [clean]: MapGuide Ubuntu build 64-bit
if exist build rd /S /Q build
if exist *.sh del *.sh
if exist *.pl del *.pl
call vagrant destroy -f
popd
pushd centos\x64
SET COMPONENT=CentOS Build 64-bit
echo [clean]: MapGuide CentOS build 64-bit
if exist build rd /S /Q build
if exist *.sh del *.sh
if exist *.pl del *.pl
call vagrant destroy -f
popd
goto quit
:error
echo [ERROR]: There was an error cleaning the component %COMPONENT%
exit /B 1
:quit