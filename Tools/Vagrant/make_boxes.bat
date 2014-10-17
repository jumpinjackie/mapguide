@echo off
SET BUILD_UBUNTU_32=1
SET BUILD_UBUNTU_64=1
SET BUILD_CENTOS_32=1
SET BUILD_CENTOS_64=1
pushd packer
if "%BUILD_CENTOS_64%" == "1" (
echo [packer]: Build CentOS 64-bit base box
packer build -force centos6-amd64.json
echo [vagrant]: Export centos6-amd64 base box
rem call vagrant package --base "centos6-amd64" --output centos6-amd64.box
call vagrant box add "centos6-amd64" centos6-amd64.box --force
)
if "%BUILD_CENTOS_32%" == "1" (
echo [packer]: Build CentOS 32-bit base box
packer build -force centos6-i386.json
echo [vagrant]: Export centos6-i386 base box
rem call vagrant package --base "centos6-i386" --output centos6-i386.box
call vagrant box add "centos6-i386" centos6-i386.box --force
)
if "%BUILD_UBUNTU_64%" == "1" (
echo [packer]: Build Ubuntu 64-bit base box
packer build -force ubuntu12-amd64.json
echo [vagrant]: Export ubuntu12-amd64 base box
rem call vagrant package --base "ubuntu12-amd64" --output ubuntu12-amd64.box
call vagrant box add "ubuntu12-amd64" ubuntu12-amd64.box --force
)
if "%BUILD_UBUNTU_32%" == "1" (
echo [packer]: Build Ubuntu 32-bit base box
packer build -force ubuntu12-i386.json
echo [vagrant]: Export ubuntu12-i386 base box
rem call vagrant package --base "ubuntu12-i386" --output ubuntu12-i386.box
call vagrant box add "ubuntu12-i386" ubuntu12-i386.box --force
)
popd