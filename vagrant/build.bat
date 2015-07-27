@echo off

rem build.bat
rem
rem Builds MapGuide/FDO in CentOS and Ubuntu VMs all driven by vagrant
rem
rem NOTE: CentOS builds must *always* precede the Ubuntu builds as Ubuntu builds may depend
rem on the package FDO SDK tarball produced by the CentOS build
rem
rem Requires the tee utility, which is part of GNU on Windows (https://github.com/bmatzelle/gow)

IF "%MG_SVN_UPDATE_SOURCES%"=="" SET MG_SVN_UPDATE_SOURCES=1
IF "%BUILD_UBUNTU_32%"=="" SET BUILD_UBUNTU_32=1
IF "%BUILD_UBUNTU_64%"=="" SET BUILD_UBUNTU_64=1
IF "%BUILD_CENTOS_32%"=="" SET BUILD_CENTOS_32=1
IF "%BUILD_CENTOS_64%"=="" SET BUILD_CENTOS_64=1
IF "%TEARDOWN_UBUNTU_32%"=="" SET TEARDOWN_UBUNTU_32=1
IF "%TEARDOWN_UBUNTU_64%"=="" SET TEARDOWN_UBUNTU_64=1
IF "%TEARDOWN_CENTOS_32%"=="" SET TEARDOWN_CENTOS_32=1
IF "%TEARDOWN_CENTOS_64%"=="" SET TEARDOWN_CENTOS_64=1
SET ROOT=%CD%
SET COMPONENT=
echo **************** Build Summary *********************
echo Update Sources: %MG_SVN_UPDATE_SOURCES%
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
echo [build]: Prepare build artifact location
if not exist builds mkdir builds
if not exist builds\centos_x86 mkdir builds\centos_x86
if not exist builds\centos_x64 mkdir builds\centos_x64
if not exist builds\ubuntu_x86 mkdir builds\ubuntu_x86
if not exist builds\ubuntu_x64 mkdir builds\ubuntu_x64
if not exist builds\centos_x86\logs mkdir builds\centos_x86\logs
if not exist builds\centos_x64\logs mkdir builds\centos_x64\logs
if not exist builds\ubuntu_x86\logs mkdir builds\ubuntu_x86\logs
if not exist builds\ubuntu_x64\logs mkdir builds\ubuntu_x64\logs
echo [build]: Checking if we're updating sources
if %MG_SVN_UPDATE_SOURCES% == 1 goto update_sources
goto check_centos_32
:update_sources
SET COMPONENT=Update MapGuide and FDO tarballs
pushd sources
if exist updated rd /S /Q updated
popd
pushd centos\src_update
call vagrant up 2>&1 | tee up.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
call vagrant destroy -f
popd
pushd sources\updated
move /Y *.tar.gz ..
echo [build]: Sources updated
popd
:check_centos_32
echo [build]: Checking if we're building for CentOS 32-bit
if %BUILD_CENTOS_32% == 1 goto build_centos_32
goto check_ubuntu_32
:build_centos_32
pushd centos\x86
SET COMPONENT=CentOS Build 32-bit
echo [build]: MapGuide CentOS build 32-bit
if exist build rd /S /Q build
call vagrant up 2>&1 | tee up.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if not exist build\*.tar.xz goto error
echo [build]: Move artifacts to build output
move /Y build\*.sh %ROOT%\builds\centos_x86
move /Y build\*.tar.xz %ROOT%\builds\centos_x86
move /Y build\*.log %ROOT%\builds\centos_x86\logs
move /Y build\*.xml %ROOT%\builds\centos_x86\logs
echo [build]: Generate MD5 hashes
pushd %ROOT%\builds\centos_x86
if exist md5sums.txt del md5sums.txt
md5sum *.sh > md5sums.txt
md5sum *.tar.xz >> md5sums.txt
popd
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
call vagrant up 2>&1 | tee up.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if not exist build\*.deb goto error
echo [build]: Move artifacts to build output
move /Y build\*.sh %ROOT%\builds\ubuntu_x86
move /Y build\*.deb %ROOT%\builds\ubuntu_x86
move /Y build\*.log %ROOT%\builds\ubuntu_x86\logs
move /Y build\*.xml %ROOT%\builds\ubuntu_x86\logs
echo [build]: Generate MD5 hashes
pushd %ROOT%\builds\ubuntu_x86
if exist md5sums.txt del md5sums.txt
md5sum *.sh > md5sums.txt
md5sum *.deb >> md5sums.txt
popd
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
call vagrant up 2>&1 | tee up.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if not exist build\*.tar.xz goto error
echo [build]: Move artifacts to build output
move /Y build\*.sh %ROOT%\builds\centos_x64
move /Y build\*.tar.xz %ROOT%\builds\centos_x64
move /Y build\*.log %ROOT%\builds\centos_x64\logs
move /Y build\*.xml %ROOT%\builds\centos_x64\logs
echo [build]: Generate MD5 hashes
pushd %ROOT%\builds\centos_x64
if exist md5sums.txt del md5sums.txt
md5sum *.sh > md5sums.txt
md5sum *.tar.xz >> md5sums.txt
popd
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
call vagrant up 2>&1 | tee up.log
echo [build]: vagrant returned %errorlevel%
if "%errorlevel%"=="1" goto error
if not exist build\*.deb goto error
echo [build]: Move artifacts to build output
move /Y build\*.sh %ROOT%\builds\ubuntu_x64
move /Y build\*.deb %ROOT%\builds\ubuntu_x64
move /Y build\*.log %ROOT%\builds\ubuntu_x64\logs
move /Y build\*.xml %ROOT%\builds\ubuntu_x64\logs
echo [build]: Generate MD5 hashes
pushd %ROOT%\builds\ubuntu_x64
if exist md5sums.txt del md5sums.txt
md5sum *.sh > md5sums.txt
md5sum *.deb >> md5sums.txt
popd
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