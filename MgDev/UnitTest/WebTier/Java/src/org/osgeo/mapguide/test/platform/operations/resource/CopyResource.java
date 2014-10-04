package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class CopyResource extends ResourceServiceOperationExecutor
{
    public CopyResource(MgResourceService resSvc, String unitTestVm)
    {
        super("CopyResource", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "SOURCE", param);
            ReadParameterValue(paramSetId, "DESTINATION", param);

            MgResourceIdentifier srcId = null;
            if (param.containsKey("SOURCE"))
            {
                srcId = new MgResourceIdentifier(param.get("SOURCE"));
            }

            MgResourceIdentifier dstId = null;
            if (param.containsKey("DESTINATION"))
            {
                dstId = new MgResourceIdentifier(param.get("DESTINATION"));
            }

            _resourceService.copyResource(srcId, dstId, false);

            return TestResult.FromByteReader(null);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
        catch (Exception ex)
        {
            return TestResult.FromException(ex);
        }
    }
}