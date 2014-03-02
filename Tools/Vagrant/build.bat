@echo off
SET ROOT=%CD%
SET COMPONENT=
echo [build]: Prepare scripts for provisioning
pushd scripts
copy /Y *.* %ROOT%\centos\x86
copy /Y *.* %ROOT%\ubuntu\x86
popd
pushd ubuntu\x86
SET COMPONENT=Ubuntu Build
echo [build]: MapGuide Ubuntu build
if exist build rd /S /Q build
vagrant up
if "%errorlevel%"=="1" goto error
vagrant destroy -f
popd
pushd centos\x86
SET COMPONENT=CentOS Build
echo [build]: MapGuide CentOS build
if exist build rd /S /Q build
vagrant up
if "%errorlevel%"=="1" goto error
vagrant destroy -f
popd
goto quit
:error
echo [ERROR]: There was an error building the component %COMPONENT%
exit /B 1
:quit