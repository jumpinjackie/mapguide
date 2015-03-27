Vagrant Build Environment README
================================

This directory contains configurations and scripts to do the following:

 1. Create/register the necessary vagrant base boxes
 2. Build MapGuide and FDO through vagrant-provisioned VMs

Requirements
============

 * Vagrant (http://www.vagrantup.com/)
 * packer (http://www.packer.io/)
 * Build host assumed to be Windows
 * Read sources/README.txt for additional requirements

Building base boxes
===================

Run make_boxes.bat. This will run packer and create the following vagrant base boxes

 * CentOS 6.5 32-bit
 * CentOS 6.5 64-bit
 * Ubuntu 12.04 32-bit
 * Ubuntu 12.04 64-bit

These boxes will also be registered in vagrant as:

 * centos6-i386
 * centos6-amd64
 * ubuntu12-i386
 * ubuntu12-amd64

Building MapGuide and FDO
=========================

Run build.bat, this will build MapGuide and FDO for:

 * CentOS 6.5 32-bit
 * CentOS 6.5 64-bit
 * Ubuntu 12.04 32-bit
 * Ubuntu 12.04 64-bit

If MG_SVN_UPDATE_SOURCES is set to 1 before running build.bat, a special CentOS VM will be spun up to svn update the MapGuide and FDO source
tarballs to the latest revisions. Otherwise, the existing source tarballs will be built at their last updated svn revision.

When each build completes, the build artifacts and assorted log files and unit test result XML files will be copied to these directories:

 * builds/centos_x86
 * builds/centos_x86/logs
 * builds/centos_x64
 * builds/centos_x64/logs
 * builds/ubuntu_x86
 * builds/ubuntu_x86/logs
 * builds/ubuntu_x64
 * builds/ubuntu_x64/logs

For CentOS, the build will produce tarballs of FDO and MapGuide along with the necessary install scripts
For Ubuntu, the build will produce debian package files of FDO and MapGuide along with the necessary install scripts

At the end of the run, updated MapGuide and FDO source tarballs will be in sources/updated. You can move these tarballs up one level and
overwrite the existing copies to ensure you have the most updated checkouts for the next build run.

Troubleshooting
===============

If a given build does not produce the expected build artifacts, always check the build logs for any errors.

