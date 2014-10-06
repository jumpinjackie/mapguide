package org.osgeo.mapguide.test.mapguide.operations.drawing;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

import com.almworks.sqlite4java.*;

public abstract class DrawingServiceOperationExecutor extends PlatformApiTestExecutor
{
    protected MgDrawingService _drawingService;

    protected DrawingServiceOperationExecutor(String opName, MgDrawingService drawSvc, String unitTestVm)
    {
        super(opName, ApiTypes.Platform, unitTestVm);
        _drawingService = drawSvc;
    }
}