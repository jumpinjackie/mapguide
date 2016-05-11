@echo off

rem ==================================================
rem build_target.bat
rem
rem Builds MapGuide/FDO in a specific vagrant VM
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

if "%1"=="-h"         goto help_show
if "%1"=="-distro"    goto get_distro
if "%1"=="-target"    goto get_target
if "%1"=="-ext"       goto get_extension
if "%1"=="-teardown"  goto get_teardown
goto custom_error

:next_param
shift
shift
goto study_params

:get_distro
SET VM_DISTRO=%2
goto next_param

:get_target
SET VM_TARGET=%2
IF "%2"=="x86" goto next_param
IF "%2"=="x64" goto next_param
SET ERRORMSG=Unrecognised target: %2
goto custom_error

:get_extension
SET BUILD_ARTIFACT_EXTENSION=%2
IF "%2"=="deb" goto next_param
IF "%2"=="tar.xz" goto next_param
SET ERRORMSG=Unrecognised extension: %2
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
IF "%BUILD_ARTIFACT_EXTENSION%"=="" goto help_show

SET BUILD_ARTIFACT_DIR=builds\%VM_DISTRO%_%VM_TARGET%
SET BUILD_LOG_DIR=%BUILD_ARTIFACT_DIR%\logs
SET VAGRANTFILE_DIR=%VM_DISTRO%\%VM_TARGET%
SET BUILD_ARTIFACT_SRC_DIR=%VAGRANTFILE_DIR%\build

IF NOT EXIST %VAGRANTFILE_DIR% (
    SET ERRORMSG=No vagrantfile exists at [%VAGRANTFILE_DIR%]
    goto custom_error
)

rem Ensure build artifact dir exists
IF NOT EXIST builds mkdir builds
IF NOT EXIST %BUILD_ARTIFACT_DIR% mkdir %BUILD_ARTIFACT_DIR%
IF NOT EXIST %BUILD_LOG_DIR% mkdir %BUILD_LOG_DIR%

echo ***************** VM Build Summary *********************
echo Distro:                     %VM_DISTRO%
echo Target:                     %VM_TARGET%
echo Teardown?:                  %VM_TEARDOWN%
echo Artifact Extension:         %BUILD_ARTIFACT_EXTENSION%
echo Using vagrantfile from:     %VAGRANTFILE_DIR%
echo Build Artifacts will be in: %BUILD_ARTIFACT_DIR%
echo ********************************************************

rem Copy build/provisioning scripts to vagrantfile dir
xcopy /S /Y /I scripts\* %VAGRANTFILE_DIR%

:spin_up_vm
echo [build]: Spin up VM at [%VAGRANTFILE_DIR%]
pushd %VAGRANTFILE_DIR%
if exist build rd /S /Q build
call vagrant up 2>&1 | tee up.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if not exist build\*.%BUILD_ARTIFACT_EXTENSION% goto error
popd
goto copy_artifacts

:copy_artifacts
echo [build]: Copying artifacts from [%BUILD_ARTIFACT_SRC_DIR%] to [%BUILD_ARTIFACT_DIR%]
move /Y %BUILD_ARTIFACT_SRC_DIR%\*.sh %BUILD_ARTIFACT_DIR%
move /Y %BUILD_ARTIFACT_SRC_DIR%\*.%BUILD_ARTIFACT_EXTENSION% %BUILD_ARTIFACT_DIR%
echo [build]: Copying logs from [%BUILD_ARTIFACT_SRC_DIR%] to [%BUILD_LOG_DIR%]
move /Y %BUILD_ARTIFACT_SRC_DIR%\*.log %BUILD_LOG_DIR%
move /Y %BUILD_ARTIFACT_SRC_DIR%\*.xml %BUILD_LOG_DIR%
echo [build]: Generate MD5 hashes in [%BUILD_ARTIFACT_DIR%]
pushd %BUILD_ARTIFACT_DIR%
if exist md5sums.txt del md5sums.txt
md5sum *.sh > md5sums.txt
md5sum *.%BUILD_ARTIFACT_EXTENSION% >> md5sums.txt
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
echo           [-ext=Extension]
echo           [-teardown=Teardown]
echo Help:            -h
echo Distro:          -distro=ubuntu12,
echo                          ubuntu14
echo                          centos6
echo TargetName:      -target=x86,
echo                          x64
echo Extension:       -ext=deb,
echo                       tar.xz
echo Teardown:        -teardown=1,
echo                            0
echo ************************************************************************
:quit