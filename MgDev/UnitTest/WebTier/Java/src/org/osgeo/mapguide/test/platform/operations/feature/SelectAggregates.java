package org.osgeo.mapguide.test.platform.operations.feature;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;
import org.osgeo.mapguide.test.platform.*;

public class SelectAggregates extends FeatureServiceOperationExecutor
{
    public SelectAggregates(MgFeatureService featSvc, String unitTestVm)
    {
        super("SelectAggregates", featSvc, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "RESOURCEID", param);
            ReadParameterValue(paramSetId, "CLASSNAME", param);
            //ReadParameterValue(paramSetId, "PROPERTIES", param);
            //ReadParameterValue(paramSetId, "FILTER", param);

            MgResourceIdentifier resId = null;
            if (param.containsKey("RESOURCEID"))
            {
                resId = new MgResourceIdentifier(param.get("RESOURCEID"));
            }

            MgFeatureAggregateOptions query = new MgFeatureAggregateOptions();
            //TODO: Set query options
            /*
            if (param["FILTER"] != null)
            {
                query.SetFilter(param["FILTER"]);
            }
            */
            //PROPERTIES should be stored in the database as comma separated string without spaces
            /*
            if (param["PROPERTIES"] != null)
            {
                string[] props = param["PROPERTIES"].Split(',');
                foreach (var p in props)
                {
                    query.AddFeatureProperty(p);
                }
            }*/

            MgDataReader reader = _featureService.selectAggregate(resId, param.get("CLASSNAME"), query);
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