package org.osgeo.mapguide.test.common;

import java.io.File;
import java.util.HashMap;
import com.almworks.sqlite4java.*;

public abstract class PlatformApiTestExecutor extends TestExecutorBase
{
    private String _opName;
    private String _apiType;
    protected SQLiteConnection _conn;

    protected PlatformApiTestExecutor(String opName, String apiType, String dbPath)
    {
        _opName = opName;
        _apiType = apiType;
        try {
            _conn = new SQLiteConnection(new File(dbPath));
            _conn.open(false);
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    protected void ReadParameterValue(int paramSetId, String paramName, HashMap<String, String> paramValues) {
        CommonUtility.ReadParameterValue(_conn, paramSetId, paramName, paramValues);
    }

    protected void ReadParameterValue(int paramSetId, String paramName, HashMap<String, String> paramValues, boolean isPath) {
        CommonUtility.ReadParameterValue(_conn, paramSetId, paramName, paramValues, isPath);
    }

    public void close()
    {
        if (_conn != null) {
            _conn.dispose();
            _conn = null;
        }
    }

    public String getApi()
    {
        return _apiType;
    }

    public String getOperationName()
    {
        return _opName;
    }
}