package org.osgeo.mapguide.test.mapguide.operations.drawing;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumerateDrawingLayers extends DrawingServiceOperationExecutor
{
    public EnumerateDrawingLayers(MgDrawingService drawSvc, String vm)
    {
        super("EnumerateDrawingLayers", drawSvc, vm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "SECTION", param);

            MgResourceIdentifier resId = null;
            if (param.get("RESOURCEID") != null)
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            String section = "";
            if (param.get("SECTION") != null)
                section = param.get("SECTION");
            MgStringCollection coll = _drawingService.enumerateLayers(resId, section);
            MgByteReader reader = coll.toXml();
            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}