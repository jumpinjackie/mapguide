package org.osgeo.mapguide.test;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.common.*;

public class PlatformFactory implements IPlatformFactory
{
    private MgSiteConnection _siteConn;

    public PlatformFactory(MgSiteConnection siteConn)
    {
        _siteConn = siteConn;
    }

    public MgService createService(int serviceType)
    {
        return _siteConn.createService(serviceType);
    }

    public MgMapBase createMap()
    {
        return new MgMap(_siteConn);
    }

    public MgLayerBase createLayer(MgResourceIdentifier resId)
    {
        MgResourceService resSvc = (MgResourceService)_siteConn.createService(MgServiceType.ResourceService);
        return new MgLayer(resId, resSvc);
    }
}