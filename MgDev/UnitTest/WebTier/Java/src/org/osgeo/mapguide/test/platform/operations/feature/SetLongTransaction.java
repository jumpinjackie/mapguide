package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.operations.*;
import org.osgeo.mapguide.test.platform.*;

public class SetLongTransaction extends FeatureServiceOperationExecutor
{
    private ISessionCreator _creator;
    private IApplySession _apply;

    public SetLongTransaction(MgFeatureService featSvc, String unitTestVm, ISessionCreator creator, IApplySession apply)
    {
        super("SetLongTransaction", featSvc, unitTestVm);
        _creator = creator;
        _apply = apply;
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "LONGTRANSACTIONNAME", param);
            ReadParameterValue(paramSetId, "CREATESESSION", param);

            MgResourceIdentifier resId = null;
            if (param.containsKey("RESOURCEID"))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            if (param.get("CREATESESSION").equals("TRUE"))
            {
                String sessionId = _creator.CreateSession();
                _apply.SetSessionId(sessionId);
            }

            boolean bRes = _featureService.setLongTransaction(resId, param.get("LONGTRANSACTIONNAME"));

            return TestResult.Boolean(bRes);
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