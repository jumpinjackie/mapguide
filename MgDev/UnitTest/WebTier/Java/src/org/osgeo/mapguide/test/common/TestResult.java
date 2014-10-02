package org.osgeo.mapguide.test.common;

import java.util.UUID;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;

public class TestResult
{
    public Object ResultData;

    public String ContentType;

    public String HttpStatusCode;

    public TestResult()
    {
        this.ResultData = "";
        this.ContentType = "";
        this.HttpStatusCode = "";
    }

    public TestResult(String resultData)
    {
        this.ResultData = resultData;
        this.ContentType = "";
        this.HttpStatusCode = "";
    }

    public TestResult(String resultData, String contentType)
    {
        this.ResultData = resultData;
        this.ContentType = contentType;
        this.HttpStatusCode = "";
    }

    public TestResult(String resultData, String contentType, String statusCode)
    {
        this.ResultData = resultData;
        this.ContentType = contentType;
        this.HttpStatusCode = statusCode;
    }

    public static TestResult FromByteReader(MgByteReader byteReader)
    {
        return FromByteReader(byteReader, "");
    }

    public static TestResult FromByteReader(MgByteReader byteReader, String operation)
    {
        try
        {
            TestResult res = new TestResult();
            if (byteReader != null)
            {
                res.ContentType = byteReader.getMimeType();
                if (res.ContentType.equals(MgMimeType.Html) ||
                    res.ContentType.equals(MgMimeType.Json) ||
                    res.ContentType.equals(MgMimeType.Kml) ||
                    res.ContentType.equals(MgMimeType.Text) ||
                    res.ContentType.equals(MgMimeType.Xml))
                {
                    res.ResultData = byteReader.toString();
                }
                else
                {
                    MgByteSink sink = new MgByteSink(byteReader);
                    String path = operation + UUID.randomUUID().toString() + "Result.bin";
                    if (CommonUtility.IsNullOrEmpty(operation))
                        path = CommonUtility.GetTempFileName();
                    sink.toFile(path);
                    res.ResultData = CommonUtility.ReadAllBytes(path);
                    if (CommonUtility.IsNullOrEmpty(operation))
                        CommonUtility.DeleteFile(path);
                    else
                        System.out.println(String.format("[MgUnitTest]: Check out %s if binary comparison results are strange", path));
                    /*
                    byte[] bytes = new byte[byteReader.GetLength()];
                    byteReader.Read(bytes, bytes.Length);
                    res.ResultData = bytes;
                    */
                }
            }
            return res;
        }
        catch (MgException ex)
        {
            return FromMgException(ex);
        }
    }

    public static TestResult FromMgException(MgException ex)
    {
        //Need to be lowercase to satisfy a PHP-ism. Ugh!
        return new TestResult(CommonUtility.GetTypeName(ex).toLowerCase(), "text/plain");
    }

    public static TestResult FromException(Exception ex)
    {
        return new TestResult(ex.getMessage(), "text/plain");
    }
}