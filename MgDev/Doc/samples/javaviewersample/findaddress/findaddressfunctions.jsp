<%--
  -Copyright (C) 2004-2006  Autodesk, Inc.
  -This library is free software; you can redistribute it and/or
  -modify it under the terms of version 2.1 of the GNU Lesser
  -General Public License as published by the Free Software Foundation.

  -This library is distributed in the hope that it will be useful,
  -but WITHOUT ANY WARRANTY; without even the implied warranty of
  -MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  -Lesser General Public License for more details.

  -You should have received a copy of the GNU Lesser General Public
  -License along with this library; if not, write to the Free Software
  -Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.net.*" %>
<%@ page import="javax.xml.parsers.*" %>
<%@ page import="javax.xml.transform.*" %>
<%@ page import="javax.xml.transform.dom.*" %>
<%@ page import="javax.xml.transform.stream.*" %>
<%@ page import="org.w3c.dom.*" %>
<%!

private class GeocodeAddress
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


void EmitAddressResults(JspWriter out, MgFeatureService featureService, MgResourceIdentifier addressMarkerDataResId, String mgSessionId) throws MgException, Exception
{
    out.write("<table class=\"RegText\" border=\"0\" cellspacing=\"0\" width=\"100%\">");
    out.write("<tr><td class=\"SubTitle\">Previous Results</td></tr>");

    MgAgfReaderWriter geometryReaderWriter = new MgAgfReaderWriter();
    MgFeatureReader featureReader = (MgFeatureReader)featureService.SelectFeatures(addressMarkerDataResId, "AddressMarker", null);

    while (featureReader.ReadNext())
    {
        String address = featureReader.GetString("Address");
        MgByteReader byteReader = featureReader.GetGeometry("Location");

        MgPoint point = (MgPoint)geometryReaderWriter.Read(byteReader);
        double x = point.GetCoordinate().GetX();
        double y = point.GetCoordinate().GetY();

        out.write("<tr><td>");
        out.write("<img src=\"../images/pushpinblue.jpg\">");
        out.write("<a href=\"gotopoint.jsp?X=" + x + "&Y=" + y + "&Scale=2000\" target=\"scriptFrame\">  " + address + "</a></td></tr>");
    }
    featureReader.Close();

    out.write("</table>");
    out.write("<form action=\"clearaddressresults.jsp\" method=\"get\" target=\"scriptFrame\">");
    out.write("<input type=\"hidden\" name=\"SESSION\" value=\"" + mgSessionId + "\">");
    out.write("<input type=\"button\" class=\"Ctrl\" value=\"Clear Results\" onClick=\"javascript:submit()\">");
    out.write("</form>");
    out.write("<hr>");
}

// Create a temporary Feature Source to store geocode results.

void CreateAddressMarkerFeatureSource(MgFeatureService featureService, MgResourceIdentifier addressMarkerDataResId) throws MgException
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

MgLayer CreateAddressMarkerLayer(MgResourceService resourceService, MgResourceIdentifier addressMarkerDataResId, String sessionId) throws MgException, Exception
{
    // Load the AddressMarker layer definition template into
    // a JSP DOM object, find the "ResourceId" element, and
    // modify it's content to reference the temporary
    // feature source.

    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    DocumentBuilder builder = factory.newDocumentBuilder();
    Document document = builder.parse(new File(getServletContext().getRealPath("/") + "javaviewersample//findaddress//templates" + File.separator + "addressmarker.xml"));
    NodeList nodes = document.getElementsByTagName("ResourceId");
    Node resNode = nodes.item(0);
    Node resContent = document.createTextNode(addressMarkerDataResId.ToString());
    resNode.appendChild(resContent);

    // write the modified layer XML definition into a byte stream
    //
    TransformerFactory tFactory = TransformerFactory.newInstance();
    Transformer transformer = tFactory.newTransformer();
    ByteArrayOutputStream bos = new ByteArrayOutputStream();
    DOMSource source = new DOMSource(document);
    StreamResult result = new StreamResult(bos);
    transformer.transform(source, result);

    // Save the updated layer definition to the session repository using the
    // ResourceService object.

    byte[] layerDefinition = bos.toByteArray();
    MgByteSource byteSource = new MgByteSource(layerDefinition, layerDefinition.length);
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
    String urlString;
    URL url = null;
    try
    {
        urlString = "http://geocoder.us/service/rest/geocode?address=" + URLEncoder.encode(address, "UTF-8");
        url = new URL(urlString);
    }
    catch (Exception e)
    {
        return null;
    }
    URLConnection conn = null;
    DataInputStream data = null;
    String line;
    StringBuffer buf = new StringBuffer();

    try
    {
        conn = url.openConnection();
        conn.connect();

        data = new DataInputStream(new BufferedInputStream(conn.getInputStream()));
        while ((line = data.readLine()) != null)
        {
            buf.append(line + "\n");
        }
        data.close();
    }
    catch (IOException e)
    {
        return null;
    }

    //Extract address elements from the XML response
    //
    //
    GeocodeAddress addr = new GeocodeAddress();
    try
    {
        ByteArrayInputStream bis = new ByteArrayInputStream(buf.toString().getBytes("UTF-8"));
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        Document document = builder.parse(bis);

        //get the longitude and latitude
        addr.lon = document.getElementsByTagName("geo:long").item(0).getFirstChild().getNodeValue();
        addr.lat = document.getElementsByTagName("geo:lat").item(0).getFirstChild().getNodeValue();

        //get and decompose address
        address = document.getElementsByTagName("dc:description").item(0).getFirstChild().getNodeValue();
        int sep = address.indexOf(',');
        if(sep != -1)
        {
            addr.address1 = address.substring(0, sep);
            addr.address2 = address.substring(sep + 1);
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

%>
