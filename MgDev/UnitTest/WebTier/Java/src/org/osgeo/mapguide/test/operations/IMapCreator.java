package org.osgeo.mapguide.test.operations;

import org.osgeo.mapguide.*;

public interface IMapCreator
{
    MgMapBase CreateMap(MgResourceIdentifier resId);
    MgLayerBase CreateLayer(MgResourceIdentifier resId);
}