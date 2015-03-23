package org.osgeo.mapguide.test.mapguide.operations.admin;

import java.util.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import org.osgeo.mapguide.test.common.*;

public class GetLogByDate extends ServerAdminOperationExecutor
{
    public GetLogByDate(MgServerAdmin admin, String unitTestVm)
    {
        super("GetLogByDate", admin, unitTestVm);
    }

    public TestResult Execute(int paramSetId)
    {
        try
        {
            HashMap<String, String> param = new HashMap<String, String>();
            ReadParameterValue(paramSetId, "LOGTYPE", param);
            ReadParameterValue(paramSetId, "FROMDATE", param);
            ReadParameterValue(paramSetId, "TODATE", param);

            String[] fromDatePieces = (param.get("FROMDATE") == null ? "" : param.get("FROMDATE")).split(",");
            String[] toDatePieces = (param.get("TODATE") == null ? "" : param.get("TODATE")).split(",");

            MgDateTime fromDate = null;
            MgDateTime toDate = null;

            if (fromDatePieces.length == 3)
            {
                fromDate = new MgDateTime(Short.parseShort(fromDatePieces[0]), Short.parseShort(fromDatePieces[1]), Short.parseShort(fromDatePieces[2]));
            }
            else if (fromDatePieces.length == 7)
            {
                fromDate = new MgDateTime(Short.parseShort(fromDatePieces[0]), Short.parseShort(fromDatePieces[1]), Short.parseShort(fromDatePieces[2]), Short.parseShort(fromDatePieces[3]), Short.parseShort(fromDatePieces[4]), Short.parseShort(fromDatePieces[5]), Integer.parseInt(fromDatePieces[6]));
            }

            if (toDatePieces.length == 3)
            {
                toDate = new MgDateTime(Short.parseShort(toDatePieces[0]), Short.parseShort(toDatePieces[1]), Short.parseShort(toDatePieces[2]));
            }
            else if (toDatePieces.length == 7)
            {
                toDate = new MgDateTime(Short.parseShort(toDatePieces[0]), Short.parseShort(toDatePieces[1]), Short.parseShort(toDatePieces[2]), Short.parseShort(toDatePieces[3]), Short.parseShort(toDatePieces[4]), Short.parseShort(toDatePieces[5]), Integer.parseInt(toDatePieces[6]));
            }

            String logType = "";
            if (param.get("LOGTYPE") != null)
                logType = param.get("LOGTYPE");
            MgByteReader reader = _serverAdmin.getLog(logType, fromDate, toDate);
            return TestResult.FromByteReader(reader);
        }
        catch (MgException ex)
        {
            return TestResult.FromMgException(ex);
        }
    }
}