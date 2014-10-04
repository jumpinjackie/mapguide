package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class EnumerateResourceReferences extends ResourceServiceOperationExecutor
{
    public EnumerateResourceReferences(MgResourceService resSvc, String unitTestVm)
    {
        super("EnumerateResourceReferences", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);

            MgResourceIdentifier resId = null;
            if (param.containsKey("RESOURCEID"))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgByteReader reader = _resourceService.enumerateReferences(resId);

            return TestResult.FromByteReader(reader);
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