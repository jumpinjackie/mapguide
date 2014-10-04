package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class SetResourceData extends ResourceServiceOperationExecutor
{
    public SetResourceData(MgResourceService resSvc, String unitTestVm)
    {
        super("SetResourceData", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "DATANAME", param);
            ReadParameterValue(paramSetId, "DATATYPE", param);
            ReadParameterValue(paramSetId, "DATA", param, true);

            MgResourceIdentifier resId = null;
            if (param.containsKey("RESOURCEID"))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            String extension = CommonUtility.GetExtension(param.get("DATANAME"));
            String mimeType = CommonUtility.GetMimeType(extension);
            MgByteSource dataSource = new MgByteSource(param.get("DATA"));
            dataSource.setMimeType(mimeType);
            MgByteReader dataReader = dataSource.getReader();

            _resourceService.setResourceData(resId, param.get("DATANAME"), param.get("DATATYPE"), dataReader);

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