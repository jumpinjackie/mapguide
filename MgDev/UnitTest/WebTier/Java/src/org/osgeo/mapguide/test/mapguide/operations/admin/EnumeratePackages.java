package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class EnumeratePackages extends ServerAdminOperationExecutor
{
    public EnumeratePackages(MgServerAdmin admin, String unitTestVm)
    {
        super("EnumeratePackages", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            MgStringCollection packages = _serverAdmin.enumeratePackages();
            return TestResult.StringCollection(packages);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}