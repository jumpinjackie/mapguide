package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class EnumerateResources extends ResourceServiceOperationExecutor
{
    public EnumerateResources(MgResourceService resSvc, String unitTestVm)
    {
        super("EnumerateResources", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "TYPE", param);
            ReadParameterValue(paramSetId, "DEPTH", param);

            MgResourceIdentifier resId = null;
            if (param.get("RESOURCEID") != null)
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }
            String type = "";
            if (param.get("TYPE") != null)
                type = param.get("TYPE");
            int depth = -1;
            if (param.get("DEPTH") != null)
                depth = Integer.parseInt(param.get("DEPTH"));

            MgByteReader byteReader = _resourceService.enumerateResources(resId, depth, type);

            return TestResult.FromByteReader(byteReader, "GETRESOURCEDATA");
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}