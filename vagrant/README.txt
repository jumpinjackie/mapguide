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
 * Ubuntu 14.04 32-bit
 * Ubuntu 14.04 64-bit

These boxes will also be registered in vagrant as:

 * centos6-i386
 * centos6-amd64
 * ubuntu12-i386
 * ubuntu12-amd64
 * ubuntu14-i386
 * ubuntu14-amd64

Which you can confirm by running 'vagrant box list' from the command line

Building MapGuide and FDO
=========================

Building MapGuide and FDO requires you have source tarballs for MapGuide/FDO under the sources directory. See sources\README.txt for
more information about the expected layout and naming conventions of the tarballs.

Run build_target.bat with the following parameters:

 * -distro
   * Build MapGuide/FDO for the given distro
   * Valid values include:
     * centos6
     * ubuntu12
     * ubuntu14
 * -target
   * Build MapGuide/FDO for the given distro using the given CPU configuration
   * Valid values include:
     * x86
     * x64
 * -ext
   * The file extension of the build artifacts to collect from the vagrant VM's build output directory
   * Valid values include"
     * deb (for Ubuntu-based builds)
     * tar.xz (for non-Ubuntu builds)
 * -teardown
   * Indicates whether to automatically tear down the VM once the build has completed.
   * Set to 1 or 0

For the rest of this section:
 * $distro refers to the parameter you supplied for -distro
 * $target refers to the parameter you supplied for -target 

This will build MapGuide/FDO with the existing source tarballs at their last updated svn revision. It will spin up a build VM 
using the vagrantfile located under the $distro\$target directory.

If you have the computing grunt, you can run multiple build_target.bat instances for different distro/target configurations in parallel. 

When each build completes, the build artifacts and assorted log files and unit test result XML files will be copied to these directories:

 * builds/$distro_$target
 * builds/$distro_$target/logs

For Ubuntu builds, the build will produce debian package files (*.deb) of FDO and MapGuide along with the necessary install scripts
For non-Ubuntu builds, the build will produce xz-compressed tarballs (*.tar.xz) of FDO and MapGuide along with the necessary install scripts

Updating MapGuide/FDO sources
=============================

Run update_sources.bat to update the MapGuide/FDO tarballs under the sources directory to the HEAD revisions of their respective branches.

This will spin up a special centos6 VM to:

 1. Extract the MapGuide/FDO tarballs
 2. svn update the extracted working directories
 3. Re-package the working directories into new tarballs
 4. Overwrite the old tarballs under sources directories with the updated copies 

Troubleshooting
===============

If a given build does not produce the expected build artifacts, always check the build logs for any errors.

Release Versioning
==================

When doing branched releases, the following files have to be modified and their respective MapGuide and FDO versions and branch names updated to match:

 - $distro\$arch\Vagrantfile
 - scripts\mapguidecommon_excludes.txt
 - scripts\platformbase_excludes.txt
