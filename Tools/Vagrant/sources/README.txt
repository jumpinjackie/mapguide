Drop your source and thirdparty lib tarballs here. They must be named

fdo_rdbms_thirdparty.tar.gz
fdo-checkout.tar.gz
mapguide-checkout.tar.gz

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

mapguide-checkout.tar.gz is tarball containing the following directory structure:

    MgDev [svn checkout of http://svn.osgeo.org/mapguide/trunk/MgDev]