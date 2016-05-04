package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class SetResource extends ResourceServiceOperationExecutor
{
    public SetResource(MgResourceService resSvc, String unitTestVm)
    {
        super("SetResource", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            CommonUtility.ReadParameterValue(_conn, paramSetId, "RESOURCEID", param);
            CommonUtility.ReadParameterValue(_conn, paramSetId, "CONTENT", param, true);
            CommonUtility.ReadParameterValue(_conn, paramSetId, "HEADER", param, true);

            MgResourceIdentifier resId = null;
            if (param.get("RESOURCEID") != null)
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgByteReader content = CommonUtility.GetByteReaderFromPath(param.get("CONTENT"));
            MgByteReader header = CommonUtility.GetByteReaderFromPath(param.get("HEADER"));

            _resourceService.setResource(resId, content, header);

            return TestResult.Ok();
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}