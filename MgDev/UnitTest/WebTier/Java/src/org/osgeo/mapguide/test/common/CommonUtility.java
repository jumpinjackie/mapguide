package org.osgeo.mapguide.test.common;

import java.io.File;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.util.Collections;
import java.util.Comparator;
import java.util.Vector;
import java.util.HashMap;
import java.nio.file.*;
import java.nio.charset.*;
import com.almworks.sqlite4java.*;
import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Node;

public class CommonUtility
{
    public static boolean IsWindows()
    {
        return System.getProperty("os.name").toLowerCase().contains("win");
    }

    public static String DoubleToString(double d)
    {
        if(d == (long) d)
            return String.format("%d",(long)d);
        else
            return String.format("%s",d);
    }

    public static MgStringCollection StringToMgStringCollection(String str)
    {
        try
        {
            MgStringCollection coll = new MgStringCollection();
            String[] tokens = str.split(",");
            for (String token : tokens)
            {
                coll.add(token);
            }
            return coll;
        }
        catch (MgException ex)
        {
            throw new UnitTestException(String.format("Exception from MapGuide:\n%s", ex.getDetails()));
        }
    }

    public static String JoinStringArray(String glue, String[] s)
    {
        int k = s.length;
        if ( k == 0 )
        {
            return null;
        }
        StringBuilder out = new StringBuilder();
        out.append( s[0] );
        for ( int x=1; x < k; ++x )
        {
            out.append(glue).append(s[x]);
        }
        return out.toString();
    }

    public static String MgPointToString(MgPoint pt)
    {
        try
        {
            MgCoordinate coord = pt.getCoordinate();
            return String.format("(%d:%d)",
                (int)coord.getX(),
                (int)coord.getY());
        }
        catch (MgException ex)
        {
            throw new UnitTestException(String.format("Exception from MapGuide:\n%s", ex.getDetails()));
        }
    }

    public static String MgEnvelopeToString(MgEnvelope env)
    {
        try
        {
            MgCoordinate ll = env.getLowerLeftCoordinate();
            MgCoordinate ur = env.getUpperRightCoordinate();
            return String.format("(%d:%d)-(%d:%d)", 
                (int)ll.getX(), 
                (int)ll.getY(), 
                (int)ur.getX(), 
                (int)ur.getY());
        }
        catch (MgException ex)
        {
            throw new UnitTestException(String.format("Exception from MapGuide:\n%s", ex.getDetails()));
        }
    }

    public static String GetExtension(String name)
    {
        if (name.lastIndexOf(".") >= 0)
        {
            return name.substring(name.lastIndexOf(".") + 1);
        }
        else
        {
            if (name.equals("MG_USER_CREDENTIALS"))
                return "txt";
            else
                return "bin";
        }
    }

    public static String GetMimeType(String extension)
    {
        switch (extension)
        {
            case "agf":
                return "application/agf";
            case "bin":
                return "application/octet-stream";
            case "dwf":
                return "model/vnd.dwf";
            case "jpg":
            case "jpeg":
                return "image/jpeg";
            case "png":
                return "image/png";
            case "tif":
            case "tiff":
                return "image/tiff";
            case "html":
                return "text/html";
            case "txt":
                return "text/plain";
            case "xml":
                return "text/xml";
            default:
                return "application/octet-stream";
        }
    }

    public static MgByteReader GetByteReaderFromPath(String path)
    {
        return GetByteReaderFromPath(path, true);
    }

    public static MgByteReader GetByteReaderFromPath(String path, boolean bCheck)
    {
        if (path != null)
        {
            String normPath = GetPath(path);
            if (bCheck)
            {
                if (FileExists(normPath))
                {
                    MgByteSource source = new MgByteSource(normPath);
                    MgByteReader reader = source.getReader();
                    return reader;
                }
                return null;
            }
            else
            {
                MgByteSource source = new MgByteSource(normPath);
                MgByteReader reader = source.getReader();
                return reader;
            }
        }
        else
        {
            return null;
        }
    }

    public static String BooleanToString(boolean b)
    {
        return b ? "True" : "False";
    }

    public static String GetTypeName(Object o) {
        Class cls = o.getClass();
        String name = cls.getName();
        if (name.lastIndexOf('.') > 0) {
            name = name.substring(name.lastIndexOf('.') + 1); // Map$Entry
            name = name.replace('$', '.');      // Map.Entry
        }
        return name;
    }

    public static void DeleteFile(String path) {
        try {
            Files.deleteIfExists(Paths.get(path));
        } catch (IOException ex) {

        }
    }

    public static boolean FileExists(String path) {
        File f = new File(path);
        return (f.exists() && !f.isDirectory());
    }

    public static byte[] ReadAllBytes(String path) {
        try {
            return Files.readAllBytes(Paths.get(path));
        } catch (IOException ex) {
            return null;
        }
    }

    public static String ReadAllText(String path) throws IOException {
        return new String(Files.readAllBytes(Paths.get(path)), StandardCharsets.UTF_8);
    }

    public static String GetTempFileName() {
        try {
            Path p = Files.createTempFile(null, null);
            return p.toString();
        } catch (IOException ex) {
            return null;
        }
    }

    public static boolean IsNullOrEmpty(String str) {
        return str == null || str.length() == 0;
    }

    public static String ExceptionToString(Exception ex) {
        StringBuilder sb = new StringBuilder();
        sb.append(ex.toString() + "\n");
        sb.append(ex.getMessage() + "\n");
        StackTraceElement[] items = ex.getStackTrace();
        for (StackTraceElement item : items) {
            sb.append("\t" + item.toString() + "\n");
        }
        return sb.toString();
    }

    public static void GenerateDatabase(String dumpFileName, String dbName) throws SQLiteException, IOException {
        SQLiteConnection db = new SQLiteConnection(new File(dbName));
        try {
            db.open(true);

            String sql = new String(Files.readAllBytes(Paths.get(dumpFileName)), StandardCharsets.UTF_8);
            db.exec(sql);
        }
        finally
        {
            db.dispose();       
        }
    }

    public static String GetStringFromStatement(SQLiteStatement stmt, String colName) {
        try {
            for (int i = 0; i < stmt.columnCount(); i++) {
                String cn = stmt.getColumnName(i);
                if (cn.equals(colName)) {
                    if (stmt.columnNull(i)) {
                        return null;
                    } else {
                        return stmt.columnString(i);
                    }
                }
            }
            return null;
        } catch (Exception ex) {
            Console.WriteLine(ex.getMessage());
            return null;
        }
    }

    public static byte[] GetByteArrayFromStatement(SQLiteStatement stmt, String colName) {
        try {
            for (int i = 0; i < stmt.columnCount(); i++) {
                String cn = stmt.getColumnName(i);
                if (cn.equals(colName)) {
                    if (stmt.columnNull(i)) {
                        return null;
                    } else {
                        return stmt.columnBlob(i);
                    }
                }
            }
            return null;
        } catch (Exception ex) {
            Console.WriteLine(ex.getMessage());
            return null;
        }
    }

    public static String GetDbPath(String dumpFileName) throws SQLiteException, IOException
    {
        String dbPath = dumpFileName.replace(".dump", ".db");
        String dbName = CommonUtility.GetPath(dbPath);

        if (!FileExists(dumpFileName) && !FileExists(dbName))
        {
            throw new UnitTestException(String.format("Error: Dump file %s not found. Unable to create database file", dumpFileName));
        }
        else if (!FileExists(dbName))
        {
            GenerateDatabase(dumpFileName, dbName);
        }
        else
        {
            try
            {
                DeleteFile(dbName);
                GenerateDatabase(dumpFileName, dbName);
            }
            catch (Exception ex)
            {
                throw new UnitTestException(String.format("Unable to delete database file %s. The file is either in use or is read-only. The database has not been updated", dbName));
            }
        }

        return dbPath;
    }

    public static String GetPath(String dbPath)
    {
        try {
            if (IsNullOrEmpty(dbPath)) {
                return new File(".").getPath();
            } else {
                File f = new File(dbPath.replace("\\", "/"));
                if (f.isAbsolute())
                    return f.getPath();
                else
                    return new File(System.getProperty("user.dir"), dbPath.replace("\\","/")).getCanonicalPath();
            }
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    public static Object SpecialDataHandling(String operation, Object resultData, String mimeType)
    {
        Object res = resultData;
        switch (operation)
        {
            case "ENUMERATERESOURCES":
                res = RemoveTimeStamp(resultData.toString());
                break;
            case "GETDRAWINGLAYER":
                res = RemoveDwfSectionName(resultData);
                break;
            case "GETDRAWINGSECTION":
                res = RemoveDwfSectionName(resultData);
                break;
            case "GETLOG":
                res = RemoveLogEntryTimeStamp(resultData.toString());
                break;
            case "GETMAP":
                res = GetMapHeader(resultData.toString());
                break;
            case "GETLONGTRANSACTIONS":
                res = RemoveCreationDate(resultData.toString());
                break;
        }

        String strRes = null;
        if (res instanceof String)
            strRes = (String)res;
        if (strRes != null && mimeType.equals("text/xml"))
        {
            try {
                /*
                DocumentBuilderFactory documentBuildFactory = DocumentBuilderFactory.newInstance();
                DocumentBuilder documentBuilder = documentBuildFactory.newDocumentBuilder();
                Document doc = documentBuilder.parse(new ByteArrayInputStream(strRes.getBytes()));

                res = SortElement(doc, "");
                */
                res = strRes;
            }
            catch (Exception ex) {

            }
        }
        return res;
    }

    public static Object RemoveStackTraceFromResult(Object result)
    {
        String strResult = null;
        if (result instanceof String)
            strResult = (String)result;
        //TODO: Clean out stack trace
        return result;
    }

    private static String RemoveTimeStamp(String resultData)
    {
        String result = resultData;
        String newResult = result;
        while (result.indexOf("<CreatedDate>") >= 0)
        {
            newResult = result.substring(0, result.indexOf("<CreatedDate>"));
            newResult += result.substring(result.indexOf("</ModifiedDate>") + "</ModifiedDate>".length());
            result = newResult;
        }
        return newResult;
    }

    private static Object RemoveDwfSectionName(Object resultData)
    {
        boolean bFromByteArray = false;
        byte[] bResultData = null;
        if (resultData instanceof byte[])
            bResultData = (byte[])resultData;
        String strResultData = null;
        if (resultData instanceof String)
            strResultData = (String)resultData;
        /*
        if (strResultData == null)
        {
            if (bResultData != null)
            {
                strResultData = enc.GetString(bResultData);
                bFromByteArray = true;
            }
        }*/

        if (strResultData != null)
        {
            //Console.WriteLine("RemoveDwfSectionName: length = {0}", strResultData.Length);
            int idx = strResultData.indexOf(".w2d");
            //Console.WriteLine("RemoveDwfSectionName: widx = {0}", idx);
            if (idx >= 0)
            {
                String newResult = strResultData.substring(idx);
                int eidx = newResult.indexOf("EndOfDWF");
                //Console.WriteLine("RemoveDwfSectionName: eidx = {0}", eidx);
                if (0 != eidx)
                {
                    newResult = newResult.substring(0, eidx);
                    //Console.WriteLine("RemoveDwfSectionName: newlength = {0}", newResult.Length);
                }
                if (bFromByteArray)
                    return newResult.getBytes(StandardCharsets.UTF_8);
                else
                    return newResult;
            }
        }
        else if (bResultData != null)
        {
            byte[] bW2d = ".w2d".getBytes(StandardCharsets.UTF_8);
            byte[] bEOF = "EndOfDWF".getBytes(StandardCharsets.UTF_8);

            int widx = -1;
            int eidx = -1;

            int wMatches = 0;
            int eMatches = 0;

            int i = 0;
            while(i < bResultData.length)
            {
                //Haven't found .w2d sequence
                if (widx < 0 && wMatches == 0) 
                {
                    //We've found a "."
                    if (bResultData[i] == bW2d[0])
                    {
                        wMatches++;
                        i++;

                        //Now try to follow through this sequence to see if it is ".w2d"
                        while (wMatches < bW2d.length)
                        {
                            //End of array. Abort
                            if (i >= bResultData.length)
                                break;

                            //Next byte in sequence matches. Advance
                            if (bResultData[i] == bW2d[wMatches])
                            {
                                //Increment matches
                                wMatches++;
                                
                                //Check if full sequence matches
                                if (wMatches == bW2d.length)
                                {
                                    //Match. Record index which is current position minus the # of consecutive matches
                                    widx = i - wMatches;
                                    break;
                                }
                            }
                            else //Incomplete sequence. Break this loop
                            {
                                wMatches = 0; //Reset
                                break;
                            }
                        }
                    }
                }
                //Haven't found EndOfDWF sequence
                else if (eidx < 0 && eMatches == 0)
                {
                    //We've found a "E"
                    if (bResultData[i] == bEOF[0])
                    {
                        eMatches++;
                        i++;

                        //Now try to follow through this sequence to see if it is "EndOfDWF"
                        while (eMatches < bEOF.length)
                        {
                            //End of array. Abort
                            if (i >= bResultData.length)
                                break;

                            //Next byte in sequence matches. Advance
                            if (bResultData[i] == bEOF[eMatches])
                            {
                                //Increment matches
                                eMatches++;

                                //Check if full sequence matches
                                if (eMatches == bEOF.length)
                                {
                                    //Match. Record index which is current position minus the # of consecutive matches
                                    eidx = i - eMatches;
                                    break;
                                }
                            }
                            else //Incomplete sequence. Break this loop
                            {
                                eMatches = 0; //Reset
                                break;
                            }
                        }
                    }
                }

                //Found both offsets. We're done
                if (widx > 0 && eidx > widx)
                    break;

                i++;
            }

            if (widx > 0 && eidx > widx)
            {
                byte[] newResult = new byte[eidx - widx];
                int off = 0;
                for (int j = widx; j <= eidx; j++)
                {
                    newResult[off] = bResultData[j];
                }
            }
        }
        return resultData;
    }

    private static String RemoveLogEntryTimeStamp(String resultData)
    {
        String result = resultData;
        String newResult = result;
        while (result.indexOf("<") >= 0)
        {
            newResult = result.substring(0, result.indexOf("<"));
            newResult += result.substring(result.indexOf(">") + 1);
            result = newResult;
        }
        return newResult;
    }

    private static String GetMapHeader(String resultData)
    {
        if (resultData.indexOf("(DWF V06.01)") >= 0)
            resultData = "(DWF V06.01)";
        return resultData;
    }

    private static String RemoveCreationDate(String resultData)
    {
        String newResult = resultData;
        while (resultData.indexOf("<CreationDate>") >= 0)
        {
            newResult = resultData.substring(0, resultData.indexOf("<CreationDate>"));
            newResult += resultData.substring(resultData.indexOf("</CreationDate>") + "</CreationDate>".length());
            resultData = newResult;
        }
        return newResult;
    }

    public static Object ProcessExceptionMessage(Object resultData)
    {
        String strResultData = null;
        if (resultData instanceof String)
            strResultData = (String)resultData;
        if (strResultData != null)
        {
            String text = "exception occurred";
            if (strResultData.contains(text))
            {
                strResultData = strResultData.substring(0, strResultData.indexOf(text) + text.length());
            }
            return strResultData;
        }
        return resultData;
    }

    public static boolean SpecialValidation(String operation, Object resultData, Object expectedResult)
    {
        if (operation.equals("GETFEATUREPROVIDERS"))
        {
            //We expect both to be strings here
            return GetFeatureProvidersValidation(resultData.toString(), expectedResult.toString());
        }
        return false;
    }

    private static boolean GetFeatureProvidersValidation(String resultData, String expectedResult)
    {
        throw new RuntimeException("Not implemented");
    }

    public static boolean ByteArraysEqual(byte[] bExpected, byte[] bActual, String operation, String testName)
    {
        if (bExpected == null && bActual != null)
            return false;

        if (bExpected != null && bActual == null)
            return false;

        boolean bRet = true;
        for (int i = 0; i < bExpected.length; i++)
        {
            if (i >= bExpected.length ||
                i >= bActual.length)
            {
                break;
            }

            byte b1 = bExpected[i];
            byte b2 = bActual[i];

            if (b1 != b2)
            {
                return false;
            }
        }

        //System.Diagnostics.Debug.WriteLine(string.Format("[MgTestRunner]: {0} - {1} - COMPARE: {2} with {3} = {4}", testName, operation, bExpected.Length, bActual.Length, (bRet ? 0 : 1)));
        return bRet;
    }

    public static String GetExtensionFromMimeType(String mimeType)
    {
        String extension = "xml";
        if (mimeType.contains("ePlot"))
            return "dwf";
        if (mimeType.contains("text/plain"))
            return "txt";
        if (mimeType.contains("text/html"))
            return "html";

        switch (mimeType)
        { 
            case "application/agf":
                return "agf";
            case "application/octet-stream":
                return "bin";
            case "model/vnd.dwf":
                return "dwf";
            case "image/jpeg":
                return "jpg";
            case "image/png":
                return "png";
            case "image/tiff":
                return "tiff";
            case "application/x-w2d":
                return "dwf";
        }

        return extension;
    }

    public static boolean ReadParameterValue(SQLiteConnection conn, int paramSetId, String paramName, HashMap<String, String> result)
    {
        return ReadParameterValue(conn, paramSetId, paramName, result, false);
    }

    public static boolean ReadParameterValue(SQLiteConnection conn, int paramSetId, String paramName, HashMap<String, String> result, boolean bIsPath)
    {
        try {
            SQLiteStatement stmt = conn.prepare(String.format("Select ParamValue from Params WHERE ParamSet=%d AND ParamName=\"%s\"", paramSetId, paramName));
            if (stmt.step())
            {
                String str = GetStringFromStatement(stmt, "ParamValue");
                if (bIsPath)
                {
                    if (DebugSettings.Enabled)
                        Console.WriteLine("Read path from current directory");
                    str = CommonUtility.GetPath(str);
                }
                result.put(paramName, str);
                return true;
            }
            return false;
        } catch(Exception ex) {
            return false;
        }
    }

    public static boolean ReadParameterValue(SQLiteConnection conn, String paramName, HashMap<String, String> result)
    {
        try {
            SQLiteStatement stmt = conn.prepare(String.format("Select ParamValue from Params WHERE ParamName=\"%s\"", paramName));
            if (stmt.step())
            {
                result.put(paramName, GetStringFromStatement(stmt, "ParamValue"));
                return true;
            }
            return false;
        } catch (Exception ex) {
            return false;
        }
    }

    public static String MgStringCollectionToString(MgStringCollection coll)
    {
        try
        {
            //Sigh, we're too smart for our own good. Yes, the trailing comma
            //should be there!
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < coll.getCount(); i++)
            {
                sb.append(coll.getItem(i));
                sb.append(",");
            }
            return sb.toString();

            /*
            List<string> items = new List<string>();
            for (int i = 0; i < coll.GetCount(); i++)
            {
                items.Add(coll.GetItem(i));
            }
            return string.Join(",", items.ToArray());
             */
        }
        catch (MgException ex)
        {
            throw new UnitTestException(String.format("Exception from MapGuide:\n%s", ex.getDetails()));
        }
    }
}
