@echo off

rem build.bat
rem
rem Performs basic smoke tests of CentOS and Ubuntu builds. 
rem
rem A basic web server is set up to serve files from the build artifact output directory
rem
rem With that set up, the following smoke tests are performed
rem
rem  1. CentOS/Ubuntu install scripts with sed'd URLs will properly download and install the required packages
rem  2. The httpd and MapGuide Servers are verified to be running and operational by the basic smoke test shell script
rem
rem Requires the sed utility, which is part of GNU on Windows (https://github.com/bmatzelle/gow)

IF "%DOWNLOAD_HOST%"=="" SET DOWNLOAD_HOST=192.168.0.6
IF "%MG_DIST_DIR%"=="" SET MG_DIST_DIR=builds30
IF "%VERIFY_UBUNTU_32%"=="" SET VERIFY_UBUNTU_32=1
IF "%VERIFY_UBUNTU_64%"=="" SET VERIFY_UBUNTU_64=1
IF "%VERIFY_CENTOS_32%"=="" SET VERIFY_CENTOS_32=1
IF "%VERIFY_CENTOS_64%"=="" SET VERIFY_CENTOS_64=1

echo **************** Verify Summary *********************
echo Download Host is: %DOWNLOAD_HOST%
echo Dist directory name: %MG_DIST_DIR%
echo Verifying Ubuntu (32-bit): %VERIFY_UBUNTU_32%
echo Verifying Ubuntu (64-bit): %VERIFY_UBUNTU_64%
echo Verifying CentOS (32-bit): %VERIFY_CENTOS_32%
echo Verifying CentOS (64-bit): %VERIFY_CENTOS_64%
echo ****************************************************
:check_centos_32
echo [build]: Checking if we're verifying CentOS 32-bit
if %VERIFY_CENTOS_32% == 1 goto test_centos_32
goto check_ubuntu_32
:test_centos_32
pushd centos\test_x86
SET COMPONENT=CentOS Build 32-bit
echo [build]: MapGuide CentOS build 32-bit
call vagrant up 2>&1 | tee smoke_test.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
:destroy_centos_32
call vagrant destroy -f
:centos_32_done
popd
:check_ubuntu_32
echo [build]: Checking if we're verifying Ubuntu 32-bit
if %VERIFY_UBUNTU_32% == 1 goto test_ubuntu_32
goto check_centos_64
:test_ubuntu_32
pushd ubuntu\test_x86
SET COMPONENT=Ubuntu Build 32-bit
echo [build]: MapGuide Ubuntu build 32-bit
call vagrant up 2>&1 | tee smoke_test.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
:destroy_ubuntu_32
call vagrant destroy -f
:ubuntu_32_done
popd
:check_centos_64
echo [build]: Checking if we're verifying CentOS 64-bit
if %VERIFY_CENTOS_64% == 1 goto test_centos_64
goto check_ubuntu_64
:test_centos_64
pushd centos\test_x64
SET COMPONENT=CentOS Build 64-bit
echo [build]: MapGuide CentOS build 64-bit
call vagrant up 2>&1 | tee smoke_test.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
:destroy_centos_64
call vagrant destroy -f
:centos_64_done
popd
:check_ubuntu_64
echo [build]: Checking if we're verifying Ubuntu 64-bit
if %VERIFY_UBUNTU_64% == 1 goto test_ubuntu_64
goto quit
:test_ubuntu_64
pushd ubuntu\test_x64
SET COMPONENT=Ubuntu Build 64-bit
echo [build]: MapGuide Ubuntu build 64-bit
call vagrant up 2>&1 | tee smoke_test.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
:destroy_ubuntu_64
call vagrant destroy -f
:ubuntu_64_done
popd
goto quit
:error
echo [ERROR]: There was an error building the component %COMPONENT%
exit /B 1
:quit