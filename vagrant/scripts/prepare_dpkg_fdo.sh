#!/bin/bash

# prepare_dpkg_fdo.sh
#
# Creates deb files from CentOS SDK files

save_current_file_list()
{
    echo "[info]: Saving current FDO dir file list"
    pushd $FDO_INST
    # For lazy folks who build from svn working copies instead of svn exports, we need to weed out any .svn dirs before compiling the file-list
    find . -name .svn -exec rm -rf {} \;
    find . -type f -print > ${FDO_FILELIST}/temp.lst
    find . -type l -print >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst > ${FDO_FILELIST}/orig.lst
    find . -type d -print | sort > ${FDO_FILELIST}/origdir.lst
    popd
}

update_fdocore_file_list()
{
    echo "[info]: Updating FDO core file list for deb packaging"
    pushd $FDO_INST
    # For lazy folks who build from svn working copies instead of svn exports, we need to weed out any .svn dirs before compiling the file-list
    find . -name .svn -exec rm -rf {} \;
    find . -type f -print > ${FDO_FILELIST}/temp.lst
    find . -type l -print >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst > ${FDO_FILELIST}/fdocore.lst
    find . -type d -print | sort > ${FDO_FILELIST}/fdocoredir.lst
    popd
}

update_provider_file_list()
{
    PROVIDER=$1
    echo "[info]: Updating $PROVIDER file list for deb packaging"
    pushd $FDO_INST
    # For lazy folks who build from svn working copies instead of svn exports, we need to weed out any .svn dirs before compiling the file-list
    find . -name .svn -exec rm -rf {} \;
    #mkdir -p $BUILDLIST
    find . -type f -print > ${FDO_FILELIST}/temp.lst
    find . -type l -print >> ${FDO_FILELIST}/temp.lst
    cat ${FDO_FILELIST}/orig.lst >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst | uniq -u > ${FDO_FILELIST}/${PROVIDER}.lst
    find . -type d -print | sort > ${FDO_FILELIST}/temp.lst
    cat ${FDO_FILELIST}/origdir.lst >> ${FDO_FILELIST}/temp.lst
    sort ${FDO_FILELIST}/temp.lst | uniq -u > ${FDO_FILELIST}/${PROVIDER}dir.lst
    popd
}

update_fdocore_file_list
for comp in shp sqlite gdal ogr wfs wms rdbms kingoracle sdf
do
    save_current_file_list
    update_provider_file_list $comp
done