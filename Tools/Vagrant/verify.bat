@echo off

rem ==================================================
rem verify.bat
rem
rem Runs MapGuide/FDO verification in a specific 
rem vagrant VM
rem
rem Requires the tee and md5sum utilities, which is 
rem part of GNU on Windows
rem 
rem https://github.com/bmatzelle/gow
rem
rem ==================================================

rem ==================================================
rem Parameter Handling
rem ==================================================

:study_params
if (%1)==() goto start_build

if "%1"=="-h"            goto help_show
if "%1"=="-distro"       goto get_distro
if "%1"=="-target"       goto get_target
if "%1"=="-teardown"     goto get_teardown
if "%1"=="-downloadhost" goto get_downloadhost
if "%1"=="-distdir"      goto get_distdir
goto custom_error

:next_param
shift
shift
goto study_params

:get_distro
SET VM_DISTRO=%2
goto next_param

:get_downloadhost
SET DOWNLOAD_HOST=%2
goto next_param

:get_distdir
SET MG_DIST_DIR=%2
goto next_param

:get_target
SET VM_TARGET=%2
IF "%2"=="x86" goto next_param
IF "%2"=="x64" goto next_param
SET ERRORMSG=Unrecognised target: %2
goto custom_error

:get_teardown
SET VM_TEARDOWN=%2
IF "%2"=="1" goto next_param
IF "%2"=="0" goto next_param
SET ERRORMSG=Teardown flag must be 1 or 0. Got: %2
goto custom_error

:start_build
IF "%VM_DISTRO%"=="" goto help_show
IF "%VM_TARGET%"=="" goto help_show
IF "%VM_TEARDOWN%"=="" goto help_show
IF "%DOWNLOAD_HOST%"=="" goto help_show
IF "%MG_DIST_DIR%"=="" goto help_show

SET MG_TEST_TARGET=%VM_DISTRO%_%VM_TARGET%
SET VAGRANTFILE_DIR=%VM_DISTRO%\test_%VM_TARGET%

IF NOT EXIST %VAGRANTFILE_DIR% (
    SET ERRORMSG=No vagrantfile exists at [%VAGRANTFILE_DIR%]
    goto custom_error
)

echo ***************** VM Build Summary *********************
echo Distro:                 %VM_DISTRO%
echo Target:                 %VM_TARGET%
echo Teardown?:              %VM_TEARDOWN%
echo Testing Target:         %MG_TEST_TARGET%
echo Using vagrantfile from: %VAGRANTFILE_DIR%
echo Download host:          %DOWNLOAD_HOST%
echo Dist part:              %MG_DIST_DIR%
echo ********************************************************

rem Copy build/provisioning scripts to vagrantfile dir
xcopy /S /Y /I scripts\test_provision* %VAGRANTFILE_DIR%
xcopy /S /Y /I scripts\smoke_test.sh %VAGRANTFILE_DIR%

:spin_up_vm
echo [build]: Spin up VM at [%VAGRANTFILE_DIR%]
pushd %VAGRANTFILE_DIR%
call vagrant up 2>&1 | tee up.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
popd
IF "%VM_TEARDOWN%"=="1" goto tear_down_vm
goto quit

:tear_down_vm
echo [build]: Tear down VM at [%VAGRANTFILE_DIR%]
pushd %VAGRANTFILE_DIR%
call vagrant destroy -f
popd
goto quit

:error
echo [ERROR]: There was an error building the component
exit /B 1
:custom_error
echo [ERROR]: %ERRORMSG%
SET ERRORMSG=
:help_show
echo Usage:
echo ************************************************************************
echo build.bat [-h]
echo           [-distro=Distro]
echo           [-target=TargetName]
echo           [-teardown=Teardown]
echo           [-downloadhost=DownloadHost]
echo           [-distdir=DistDir]
echo Help:            -h
echo Distro:          -distro=ubuntu12,
echo                          ubuntu14
echo                          centos6
echo TargetName:      -target=x86,
echo                          x64
echo Teardown:        -teardown=1,
echo                            0
echo ************************************************************************
:quit