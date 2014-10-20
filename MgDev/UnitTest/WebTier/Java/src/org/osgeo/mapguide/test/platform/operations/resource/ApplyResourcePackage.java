package org.osgeo.mapguide.test.platform.operations.resource;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class ApplyResourcePackage extends ResourceServiceOperationExecutor
{
    public ApplyResourcePackage(MgResourceService resSvc, String unitTestVm)
    {
        super("ApplyResourcePackage", resSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "PACKAGE", param, true);

            String packagePath = "";
            if (param.get("PACKAGE") != null)
                packagePath = param.get("PACKAGE");

            if (CommonUtility.IsWindows()) {
                //Console.WriteLine("\t\tPackage Path: " + packagePath);
                MgByteReader reader = CommonUtility.GetByteReaderFromPath(packagePath, false);

                _resourceService.applyResourcePackage(reader);

                return TestResult.Ok();
            } else {
                throw new RuntimeException("FIXME: ApplyResourcePackage will kill the mgserver on invalid package files");
            }
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}
