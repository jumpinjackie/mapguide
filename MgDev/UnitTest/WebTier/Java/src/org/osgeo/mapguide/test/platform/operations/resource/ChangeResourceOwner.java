package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class ChangeResourceOwner extends ResourceServiceOperationExecutor
{
    public ChangeResourceOwner(MgResourceService resSvc, String unitTestVm)
    {
        super("ChangeResourceOwner", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "OWNER", param);
            ReadParameterValue(paramSetId, "INCLUDEDESCENDANTS", param);

            MgResourceIdentifier resId = null;
            if (param.get("RESOURCEID") != null)
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            boolean includeDescendants = false;
            if (param.get("INCLUDEDESCENDANTS") != null)
                includeDescendants = (param.get("INCLUDEDESCENDANTS").equals("1"));
            _resourceService.changeResourceOwner(resId, param.get("OWNER"), includeDescendants);

            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}