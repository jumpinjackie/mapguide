package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class SelectFeatures extends FeatureServiceOperationExecutor
{
    public SelectFeatures(MgFeatureService featSvc, String unitTestVm)
    {
        super("SelectFeatures", featSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "CLASSNAME", param);
            ReadParameterValue(paramSetId, "PROPERTIES", param);
            ReadParameterValue(paramSetId, "FILTER", param);

            MgResourceIdentifier resId = null;
            if (param.containsKey("RESOURCEID"))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgFeatureQueryOptions query = new MgFeatureQueryOptions();
            //TODO: Set query options
            if (param.containsKey("FILTER"))
            {
                query.setFilter(param.get("FILTER"));
            }

            //PROPERTIES should be stored in the database as comma separated string without spaces
            if (param.containsKey("PROPERTIES"))
            {
                String [] props = param.get("PROPERTIES").split(",");
                for (String p : props)
                {
                    query.addFeatureProperty(p);
                }
            }

            MgFeatureReader reader = _featureService.selectFeatures(resId, param.get("CLASSNAME"), query);
            MgByteReader byteReader = reader.toXml();
            reader.close();
            return TestResult.FromByteReader(byteReader);
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