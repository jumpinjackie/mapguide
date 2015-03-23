package org.osgeo.mapguide.test.mapguide.operations.admin;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

import com.almworks.sqlite4java.*;

public abstract class ServerAdminOperationExecutor extends PlatformApiTestExecutor
{
    protected MgServerAdmin _serverAdmin;

    protected ServerAdminOperationExecutor(String opName, MgServerAdmin serverAdmin, String unitTestVm)
    {
        super(opName, ApiTypes.Platform, unitTestVm);
        _serverAdmin = serverAdmin;
    }
}