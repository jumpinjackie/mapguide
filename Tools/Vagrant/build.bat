@echo off

rem build.bat
rem
rem Builds MapGuide/FDO in CentOS and Ubuntu VMs all driven by vagrant
rem
rem NOTE: CentOS builds must *always* precede the Ubuntu builds as Ubuntu builds may depend
rem on the package FDO SDK tarball produced by the CentOS build

SET BUILD_UBUNTU_32=1
SET BUILD_UBUNTU_64=1
SET BUILD_CENTOS_32=0
SET BUILD_CENTOS_64=0
SET TEARDOWN_UBUNTU_32=1
SET TEARDOWN_UBUNTU_64=1
SET TEARDOWN_CENTOS_32=0
SET TEARDOWN_CENTOS_64=0
SET ROOT=%CD%
SET COMPONENT=
echo **************** Build Summary *********************
echo Building Ubuntu (32-bit): %BUILD_UBUNTU_32% (teardown=%TEARDOWN_UBUNTU_32%)
echo Building Ubuntu (64-bit): %BUILD_UBUNTU_64% (teardown=%TEARDOWN_UBUNTU_64%)
echo Building CentOS (32-bit): %BUILD_CENTOS_32% (teardown=%TEARDOWN_CENTOS_32%)
echo Building CentOS (64-bit): %BUILD_CENTOS_64% (teardown=%TEARDOWN_CENTOS_64%)
echo ****************************************************
:prepare
echo [build]: Prepare scripts for provisioning
if %BUILD_CENTOS_32% == 1 xcopy /S /Y /I scripts\* %ROOT%\centos\x86
if %BUILD_UBUNTU_32% == 1 xcopy /S /Y /I scripts\* %ROOT%\ubuntu\x86
if %BUILD_CENTOS_64% == 1 xcopy /S /Y /I scripts\* %ROOT%\centos\x64
if %BUILD_UBUNTU_64% == 1 xcopy /S /Y /I scripts\* %ROOT%\ubuntu\x64
:check_centos_32
echo [build]: Checking if we're building for CentOS 32-bit
if %BUILD_CENTOS_32% == 1 goto build_centos_32
goto check_ubuntu_32
:build_centos_32
pushd centos\x86
SET COMPONENT=CentOS Build 32-bit
echo [build]: MapGuide CentOS build 32-bit
if exist build rd /S /Q build
call vagrant up
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if %TEARDOWN_CENTOS_32% == 1 (
    echo [build]: Tearing down CentOS 32 VM
    goto destroy_centos_32
) else (
    echo [build]: Not tearing down CentOS 32 VM
    goto centos_32_done
)
:destroy_centos_32
call vagrant destroy -f
:centos_32_done
popd
:check_ubuntu_32
echo [build]: Checking if we're building for Ubuntu 32-bit
if %BUILD_UBUNTU_32% == 1 goto build_ubuntu_32
goto check_centos_64
:build_ubuntu_32
pushd ubuntu\x86
SET COMPONENT=Ubuntu Build 32-bit
echo [build]: MapGuide Ubuntu build 32-bit
if exist build rd /S /Q build
call vagrant up
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if %TEARDOWN_UBUNTU_32% == 1 (
    echo [build]: Tearing down Ubuntu 32 VM
    goto destroy_ubuntu_32
) else (
    echo [build]: Not tearing down Ubuntu 32 VM
    goto ubuntu_32_done
)
:destroy_ubuntu_32
call vagrant destroy -f
:ubuntu_32_done
popd
:check_centos_64
echo [build]: Checking if we're building for CentOS 64-bit
if %BUILD_CENTOS_64% == 1 goto build_centos_64
goto check_ubuntu_64
:build_centos_64
pushd centos\x64
SET COMPONENT=CentOS Build 64-bit
echo [build]: MapGuide CentOS build 64-bit
if exist build rd /S /Q build
call vagrant up
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if %TEARDOWN_CENTOS_64% == 1 (
    echo [build]: Tearing down CentOS 64 VM
    goto destroy_centos_64
) else (
    echo [build]: Not tearing down CentOS 64 VM
    goto centos_64_done
)
:destroy_centos_64
call vagrant destroy -f
:centos_64_done
popd
:check_ubuntu_64
echo [build]: Checking if we're building for Ubuntu 64-bit
if %BUILD_UBUNTU_64% == 1 goto build_ubuntu_64
goto quit
:build_ubuntu_64
pushd ubuntu\x64
SET COMPONENT=Ubuntu Build 64-bit
echo [build]: MapGuide Ubuntu build 64-bit
if exist build rd /S /Q build
call vagrant up
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if %TEARDOWN_UBUNTU_64% == 1 (
    echo [build]: Tearing down Ubuntu 64 VM
    goto destroy_ubuntu_64
) else (
    echo [build]: Not tearing down Ubuntu 64 VM
    goto ubuntu_64_done
)
:destroy_ubuntu_64
call vagrant destroy -f
:ubuntu_64_done
popd
goto quit
:error
echo [ERROR]: There was an error building the component %COMPONENT%
exit /B 1
:quit