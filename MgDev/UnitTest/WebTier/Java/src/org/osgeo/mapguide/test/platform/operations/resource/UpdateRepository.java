package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class UpdateRepository extends ResourceServiceOperationExecutor
{
    public UpdateRepository(MgResourceService resSvc, String unitTestVm)
    {
        super("UpdateRepository", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "CONTENT", param, true);
            ReadParameterValue(paramSetId, "HEADER", param, true);

            MgResourceIdentifier resId = null;
            if (param.containsKey("RESOURCEID"))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgByteReader content = CommonUtility.GetByteReaderFromPath(param.get("CONTENT"));
            MgByteReader header = CommonUtility.GetByteReaderFromPath(param.get("HEADER"));

            _resourceService.updateRepository(resId, content, header);

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