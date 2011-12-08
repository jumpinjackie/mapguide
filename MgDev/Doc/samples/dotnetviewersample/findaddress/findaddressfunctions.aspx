<%--
Copyright (C) 2004-2006  Autodesk, Inc.
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
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Net" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<script language="c#" runat="server">

/// <summary>
/// Summary description for FindAddressFunctions.
/// </summary>

void EmitAddressResults(MgFeatureService featureService, MgResourceIdentifier addressMarkerDataResId, String mgSessionId, HttpResponse response)
{
    response.Write("<table class=\"RegText\" border=\"0\" cellspacing=\"0\" width=\"100%\">");
    response.Write("<tr><td class=\"SubTitle\">Previous Results</td></tr>");

    MgAgfReaderWriter geometryReaderWriter = new MgAgfReaderWriter();
    MgFeatureReader featureReader = featureService.SelectFeatures(addressMarkerDataResId, "AddressMarker", null);

    while (featureReader.ReadNext())
    {
        String address = featureReader.GetString("Address");
        MgByteReader byteReader = featureReader.GetGeometry("Location");

        MgPoint point = geometryReaderWriter.Read(byteReader) as MgPoint;
        double x = point.GetCoordinate().GetX();
        double y = point.GetCoordinate().GetY();

        response.Write("<tr><td>");
        response.Write("<img src=\"../images/pushpinblue.jpg\">");
        response.Write("<a href=\"gotopoint.aspx?X="+x+"&Y="+y+"&Scale=2000\" target=\"scriptFrame\">" + address + "</a></td></tr>");
    }
    featureReader.Close();

    response.Write("</table>");
    response.Write("<form action=\"clearaddressresults.aspx\" method=\"get\" target=\"scriptFrame\">");
    response.Write("<input type=\"hidden\" name=\"SESSION\" value=\"" + mgSessionId + "\">");
    response.Write("<input type=\"button\" class=\"Ctrl\" value=\"Clear Results\" onClick=\"javascript:submit()\">");
    response.Write("</form>");
    response.Write("<hr>");
}

// Create a temporary Feature Source to store geocode results.

    void CreateAddressMarkerFeatureSource(MgFeatureService featureService, MgResourceIdentifier addressMarkerDataResId)
{
    String ll84Wkt = "GEOGCS[\"WGS84 Lat/Long's, Degrees, -180 ==> +180\",DATUM[\"D_WGS_1984]\",SPHEROID[\"World_Geodetic_System_of_1984\",6378137,298.257222932867]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";

    MgClassDefinition addressClass = new MgClassDefinition();
    addressClass.SetName("AddressMarker");
    MgPropertyDefinitionCollection properties = addressClass.GetProperties();

    MgDataPropertyDefinition idProperty = new MgDataPropertyDefinition("ID");
    idProperty.SetDataType(MgPropertyType.Int32);
    idProperty.SetReadOnly(true);
    idProperty.SetNullable(false);
    idProperty.SetAutoGeneration(true);
    properties.Add(idProperty);

    MgDataPropertyDefinition addressProperty = new MgDataPropertyDefinition("Address");
    addressProperty.SetDataType(MgPropertyType.String);
    addressProperty.SetLength(512);
    properties.Add(addressProperty);

    MgGeometricPropertyDefinition locationProperty = new MgGeometricPropertyDefinition("Location");
    locationProperty.SetGeometryTypes(MgGeometryType.Point);
    locationProperty.SetHasElevation(false);
    locationProperty.SetHasMeasure(false);
    locationProperty.SetReadOnly(false);
    locationProperty.SetSpatialContextAssociation("LL84");
    properties.Add(locationProperty);

    MgPropertyDefinitionCollection idProperties = addressClass.GetIdentityProperties();
    idProperties.Add(idProperty);

    addressClass.SetDefaultGeometryPropertyName("Location");

    MgFeatureSchema addressSchema = new MgFeatureSchema();
    addressSchema.SetName("AddressMarkerSchema");
    addressSchema.GetClasses().Add(addressClass);

    MgCreateSdfParams sdfParams = new MgCreateSdfParams("LL84", ll84Wkt, addressSchema);

    featureService.CreateFeatureSource(addressMarkerDataResId, sdfParams);
}

// Create a temporary Layer to display geocode results.

MgLayer CreateAddressMarkerLayer(MgResourceService resourceService, MgResourceIdentifier addressMarkerDataResId, String sessionId)
{
    // Load the AddressMarker layer definition template into
    // a ASPX XML object, find the "ResourceId" element, and
    // modify it's content to reference the temporary
    // feature source.

    XmlDocument doc = new XmlDocument();
    doc.PreserveWhitespace = true;
    doc.Load(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "findaddress/templates/addressmarker.xml");
    XmlNode featureSourceNode = doc.GetElementsByTagName("ResourceId").Item(0);
    XmlNode resContent = doc.CreateTextNode(addressMarkerDataResId.ToString());
    featureSourceNode.AppendChild(resContent);

    // Get the updated layer definition from the DOM object
    // and save it to the session repository using the
    // ResourceService object.
    MemoryStream xmlStream = new MemoryStream();
    doc.Save(xmlStream);
    byte [] layerDefinition = xmlStream.ToArray();
    Encoding utf8 = Encoding.UTF8;
    String layerDefStr = new String(utf8.GetChars(layerDefinition));
    layerDefinition = new byte[layerDefStr.Length-1];
    int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length-1, layerDefinition, 0);
    MgByteSource byteSource = new MgByteSource(layerDefinition, layerDefinition.Length);
    byteSource.SetMimeType(MgMimeType.Xml);

    MgResourceIdentifier tempLayerResId = new MgResourceIdentifier("Session:" + sessionId + "//AddressMarker.LayerDefinition");

    resourceService.SetResource(tempLayerResId, byteSource.GetReader(), null);

    // Create an MgLayer object based on the new layer definition
    // and return it to the caller.

    MgLayer addressLayer = new MgLayer(tempLayerResId, resourceService);
    addressLayer.SetName("AddressMarker");
    addressLayer.SetLegendLabel("AddressMarker");
    addressLayer.SetDisplayInLegend(false);
    addressLayer.SetSelectable(false);

    return addressLayer;
}

GeocodeAddress requestGeocodeAddress(String address)
{
    //Request the specified address to the geocode service using REST, the
    // GET interface
    //
    String urlString = "http://geocoder.us/service/rest/geocode?address=" + Server.UrlEncode(address);
    HttpWebRequest getRequest = (HttpWebRequest)WebRequest.Create(urlString);
    getRequest.Method = "GET";
    WebResponse response = null;

    try
    {
        response = getRequest.GetResponse();
    }
    catch (Exception exc)
    {
        return null;
    }
    Stream responseStream = response.GetResponseStream();


    //Extract address elements from the XML response
    //
    //
    GeocodeAddress addr = new GeocodeAddress();
    try
    {
        XmlDocument doc = new XmlDocument();
        doc.Load(responseStream);

        //get the longitude and latitude
        addr.lon = doc.GetElementsByTagName("geo:long").Item(0).FirstChild.Value;
        addr.lat = doc.GetElementsByTagName("geo:lat").Item(0).FirstChild.Value;

        //get and decompose address
        address = doc.GetElementsByTagName("dc:description").Item(0).FirstChild.Value;
        int sep = address.IndexOf(',');
        if(sep != -1)
        {
            addr.address1 = address.Substring(0, sep);
            addr.address2 = address.Substring(sep + 1);
        }
        else
            addr.address1 = address;
    }
    catch (Exception e)
    {
        return null;
    }
    return addr;
}

class GeocodeAddress
{
    public GeocodeAddress()
    {
        address1 = "";
        address2 = "";
        lat = "";
        lon = "";
    }

    public String address1;
    public String address2;
    public String lat;
    public String lon;
}
</script>
