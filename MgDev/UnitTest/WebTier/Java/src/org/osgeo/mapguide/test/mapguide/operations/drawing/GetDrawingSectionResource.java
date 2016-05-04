package org.osgeo.mapguide.test.mapguide.operations.drawing;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GetDrawingSectionResource extends DrawingServiceOperationExecutor
{
    public GetDrawingSectionResource(MgDrawingService drawSvc, String vm)
    {
        super("GetDrawingSectionResource", drawSvc, vm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "RESOURCENAME", param);

            MgResourceIdentifier resId = null;
            if (param.get("RESOURCEID") != null)
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgByteReader reader = _drawingService.getSectionResource(resId, param.get("RESOURCENAME"));
            return TestResult.FromByteReader(reader, "GETDRAWINGSECTIONRESOURCE");
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}