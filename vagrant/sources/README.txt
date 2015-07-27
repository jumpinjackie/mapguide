Drop your source and thirdparty lib tarballs here. They must be named

fdo_rdbms_thirdparty.tar.gz
fdo-checkout.tar.gz
mapguide-checkout.tar.gz

For branch-based builds, append the respective MapGuide and FDO versions to the tarball name.

For example if building for MapGuide Open Source 2.6 (with FDO 3.9), your tarballs should be named:
 * fdo-checkout-3.9.tar.gz
 * mapguide-checkout-2.6.tar.gz

fdo_rdbms_thirdparty.tar.gz is a tarball containing the following directory structure:

    fdo_rdbms_thirdparty
        mysql
            x86 [Put 32-bit MySQL ConnectorC SDK here]
            x64 [Put 64-bit MySQL ConnectorC SDK here]
        oracle
            x86 [Put 32-bit Oracle Instant Client SDK here]
            x64 [Put 64-bit Oracle Instant Client SDK here]
        pgsql [Put libpq SDK here]

fdo-checkout.tar.gz is a tarball containing the following directory structure:

    trunk [svn checkout of http://svn.osgeo.org/fdo/trunk]

If this is a branch tarball, the folder should be the FDO version number, for example FDO 3.9 tarball should be:

    3.9 [svn checkout of http://svn.osgeo.org/fdo/branches/3.9]

mapguide-checkout.tar.gz is tarball containing the following directory structure:

    MgDev [svn checkout of http://svn.osgeo.org/mapguide/trunk/MgDev]

If this is a branch tarball, the folder should be the same, but it should be the svn checkout of the respective branch url, for example MGOS 2.6 tarball should be:

    MgDev [svn checkout of http://svn.osgeo.org/mapguide/branches/2.6/MgDev]