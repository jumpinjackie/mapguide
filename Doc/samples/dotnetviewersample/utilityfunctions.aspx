<%--
Copyright (C) 2004-2007  Autodesk, Inc.
This library is free software; you can redistribute it and/or
modify it under the terms of version 2.1 of the GNU Lesser
General Public License as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<script runat="server">

static String s_basePath = "";
    
void InitializeWebTier()
{
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same directory as this script.
    s_basePath = Request.ServerVariables["APPL_PHYSICAL_PATH"];
    MapGuideApi.MgInitializeWebTier(s_basePath + "../webconfig.ini");
}

NameValueCollection GetRequestParameters()
{
    if ("POST"== Request.HttpMethod)
    {
        return Request.Form;
    }
    else
    {
        return Request.QueryString;
    }
}
    
static String GetBasePath()
{
    return s_basePath;
}

static String GetParameter(NameValueCollection parameters, String name)
{
    String strval = parameters[name];
    if (null == strval)
        return "";

    return strval.Trim();
}

static String GetFeaturePropertyValue(MgReader featureReader, String propName)
{
    String value = "";
    int propertyType = featureReader.GetPropertyType(propName);
    switch(propertyType)
    {
        case MgPropertyType.Boolean :
            value = featureReader.GetBoolean(propName).ToString();
            break;
        case MgPropertyType.Byte :
            value = featureReader.GetByte(propName).ToString();
            break;
        case MgPropertyType.Single :
            value = featureReader.GetSingle(propName).ToString();
            break;
        case MgPropertyType.Double :
            value = featureReader.GetDouble(propName).ToString();
            break;
        case MgPropertyType.Int16 :
            value = featureReader.GetInt16(propName).ToString();
            break;
        case MgPropertyType.Int32 :
            value = featureReader.GetInt32(propName).ToString();
            break;
        case MgPropertyType.Int64 :
            value = featureReader.GetInt64(propName).ToString();
            break;
        case MgPropertyType.String :
            value = featureReader.GetString(propName);
            break;
        case MgPropertyType.DateTime :
        case MgPropertyType.Null :
        case MgPropertyType.Blob :
        case MgPropertyType.Clob :
        case MgPropertyType.Feature :
        case MgPropertyType.Geometry :
        case MgPropertyType.Raster :
            value = "[unsupported data type]";
            break;
    }
    return value;
}
    
// Searches the map for a layer with the specified name.
MgLayer GetLayerByName(MgMap map, String layerName)
{
    MgLayer layer = null;
    for (int i = 0; i < map.GetLayers().GetCount(); i++)
    {
        MgLayer nextLayer = (MgLayer) map.GetLayers().GetItem(i);
        if (nextLayer.GetName().Equals(layerName))
        {
            layer = nextLayer;
            break;
        }
    }
    return layer;
}

void OutputReaderContent(HttpResponse response, MgByteReader byteReader)
{
    MemoryStream memBuf = new MemoryStream();

    byte[] byteBuffer = new byte[1024];
    int numBytes = byteReader.Read(byteBuffer, 1024);
    while(numBytes > 0)
    {
        memBuf.Write(byteBuffer, 0, numBytes);
        numBytes = byteReader.Read(byteBuffer, 1024);
    }

    response.ContentType = byteReader.GetMimeType();
    byte [] content = memBuf.ToArray();
    response.OutputStream.Write(content , 0, content.Length);
}

</script>

