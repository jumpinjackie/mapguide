@echo off

rem md5sums.bat
rem
rem (Re)generates MD5 hashes for the various MapGuide build artifacts

SET ROOT=%CD%

:check_ubuntu32
if exist %ROOT%\builds\ubuntu_x86 goto md5_ubuntu32
goto check_ubuntu64
:md5_ubuntu32
echo [md5]: Ubuntu 32-bit
pushd %ROOT%\builds\ubuntu_x86
if exist md5sums.txt del md5sums.txt
dir > filelist.txt
md5sum *.sh > md5sums.txt
md5sum *.deb >> md5sums.txt
popd

:check_ubuntu64
if exist %ROOT%\builds\ubuntu_x64 goto md5_ubuntu64
goto check_centos32
:md5_ubuntu64
echo [md5]: Ubuntu 64-bit
pushd %ROOT%\builds\ubuntu_x64
if exist md5sums.txt del md5sums.txt
dir > filelist.txt
md5sum *.sh > md5sums.txt
md5sum *.deb >> md5sums.txt
popd

:check_centos32
if exist %ROOT%\builds\centos_x86 goto md5_centos32
goto check_centos64
:md5_centos32
echo [md5]: CentOS 32-bit
pushd %ROOT%\builds\centos_x86
dir > filelist.txt
if exist md5sums.txt del md5sums.txt
md5sum *.sh > md5sums.txt
md5sum *.tar.xz >> md5sums.txt
popd

:check_centos64
if exist %ROOT%\builds\centos_x64 goto md5_centos64
goto check_windows
:md5_centos64
echo [md5]: CentOS 64-bit
pushd %ROOT%\builds\centos_x64
if exist md5sums.txt del md5sums.txt
dir > filelist.txt
md5sum *.sh > md5sums.txt
md5sum *.tar.xz >> md5sums.txt
popd

:check_windows
if exist %ROOT%\builds\windows goto md5_windows
goto done
:md5_windows
echo [md5]: Windows
pushd %ROOT%\builds\windows
if exist md5sums.txt del md5sums.txt
dir > filelist.txt
md5sum *.exe > md5sums.txt
popd

:done