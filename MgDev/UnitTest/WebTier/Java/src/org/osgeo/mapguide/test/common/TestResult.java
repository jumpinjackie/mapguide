package org.osgeo.mapguide.test.common;

import java.util.UUID;
import java.text.DecimalFormat;

import org.osgeo.mapguide.*;
import org.osgeo.mapguide.test.*;

public class TestResult
{
    public Object ResultData;

    public String ContentType;

    public String HttpStatusCode;

    public boolean HasReturnValue;

    public boolean IsException;

    private TestResult()
    {
        this.ResultData = "";
        this.ContentType = "";
        this.HttpStatusCode = "";
        this.HasReturnValue = false;
        this.IsException = false;
    }

    private TestResult(String resultData)
    {
        this.ResultData = resultData;
        this.ContentType = "";
        this.HttpStatusCode = "";
        this.HasReturnValue = true;
        this.IsException = false;
    }

    private TestResult(String resultData, String contentType)
    {
        this.ResultData = resultData;
        this.ContentType = contentType;
        this.HttpStatusCode = "";
        this.HasReturnValue = true;
        this.IsException = false;
    }

    private TestResult(String resultData, String contentType, String statusCode)
    {
        this.ResultData = resultData;
        this.ContentType = contentType;
        this.HttpStatusCode = statusCode;
        this.HasReturnValue = true;
        this.IsException = false;
    }

    public static TestResult Integer(int value) {
        return new TestResult(value + "", "text/plain");
    }

    public static TestResult StringCollection(MgStringCollection value) {
        return new TestResult(CommonUtility.MgStringCollectionToString(value), "text/plain");
    }

    public static TestResult XmlString(String value) {
        return new TestResult(value, "text/xml");
    }

    public static TestResult Point(MgPoint value) {
        return new TestResult(CommonUtility.MgPointToString(value), "text/plain");
    }

    public static TestResult Envelope(MgEnvelope value) {
        return new TestResult(CommonUtility.MgEnvelopeToString(value), "text/plain");
    }

    public static TestResult Double(double value) {
        /*
        DecimalFormat format = new DecimalFormat("0.#");
        if (value < 0.0)
            return new TestResult("-" + format.format(Math.abs(value)), "text/plain");
        else
            return new TestResult(format.format(value), "text/plain");
        */
       return new TestResult(String.format("%g", value), "text/plain");
    }

    public static TestResult Boolean(boolean value) {
        return new TestResult(CommonUtility.BooleanToString(value), "text/plain");
    }

    public static TestResult String(String value) {
        return new TestResult(value, "text/plain");
    }

    public static TestResult Ok() {
        return new TestResult();
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
        TestResult result = new TestResult(CommonUtility.GetTypeName(ex).toLowerCase(), "text/plain");
        result.IsException = true;
        return result;
    }

    public static TestResult FromException(Exception ex)
    {
        TestResult result = new TestResult(ex.getMessage(), "text/plain");
        result.IsException = true;
        return result;
    }
}