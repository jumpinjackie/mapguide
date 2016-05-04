package org.osgeo.mapguide.test.common;

public class XmlCompareResult
{
    public boolean Result;
    public String Expected;
    public String Actual;

    public boolean HasXml() { return this.Expected != null && this.Actual != null; }
}