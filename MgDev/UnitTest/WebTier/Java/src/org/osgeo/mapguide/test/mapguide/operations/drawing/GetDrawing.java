package org.osgeo.mapguide.test.mapguide.operations.drawing;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GetDrawing extends DrawingServiceOperationExecutor
{
    public GetDrawing(MgDrawingService drawSvc, String vm)
    {
        super("GetDrawing", drawSvc, vm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);

            MgResourceIdentifier resId = null;
            if (param.get("RESOURCEID") != null)
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgByteReader reader = _drawingService.getDrawing(resId);
            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}