package org.osgeo.mapguide.test.common;

import org.osgeo.mapguide.*;

public interface IPlatformFactory
{
    MgService createService(int serviceType);
    MgMapBase createMap();
    MgLayerBase createLayer(MgResourceIdentifier resId);
}